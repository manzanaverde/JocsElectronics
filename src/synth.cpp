#include "synth.h"
#include "framework.h"
#include <math.h>

Synth::Synth()
{
	volume = 0.2;
	noise_volume = 0;
}

Synth::~Synth()
{
}


void Synth::generateAudio(Uint8* buffer, int len, SDL_AudioSpec& spec)
{
	generateOscillator(osc1, spec);
	generateOscillator(osc2, spec);
	generateOscillator(osc3, spec);

	applyFilter(osc1, spec);
	applyFilter(osc2, spec);
	applyFilter(osc3, spec);

	//mix
	for (int i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
	{
		float s = osc1.buffer[i];
		s += osc2.buffer[i];
		s += osc3.buffer[i];
		s += noise_volume * ((rand() % 255) / 255.0);
		buffer[i] = clamp( volume * s * 128 + 128, 0, 256);
	}
}

void Synth::generateOscillator(Oscillator& osc, SDL_AudioSpec& spec)
{
	double wave_length = osc.freq / (spec.freq);
	double pos = osc._phase;
	int tmp;
	float amplitude = osc.amplitude;

	switch (osc.wave)
	{
		case SIN:
			for (int i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
			{
				osc.buffer[i] = sin(pos * (2.0 * PI)) * amplitude;
				pos += wave_length;
			}
			break;
		case SAW:
			for (int i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
			{
				osc.buffer[i] = (pos - (int)pos) * amplitude;
				pos += wave_length;
			}
			break;
		case SQR:
			for (int i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
			{
				osc.buffer[i] = (pos - (int)pos) > osc.pw ? 0.0 : amplitude;
				pos += wave_length;
			}
			break;
		default:
			break;
	}
	osc._phase = pos;
}

void Synth::applyFilter( Oscillator& osc, SDL_AudioSpec& spec )
{
	if (osc.LPF >= 1)
		return;

	float filter = clamp(osc.LPF,0.0,1.0);

	float current = 0;
	float last = osc._last;

	for (int i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
	{
		current = osc.buffer[i];
		last = osc.buffer[i] = last - filter * (last - current);
	}

	osc._last = last;
}
