#ifndef SYNTH_H
#define SYNTH_H

#include "includes.h"

#define AUDIO_BUFFER_LENGTH 1024

class Synth {

	public:

		enum {
			SIN = 1, //sinousidal
			SAW, //saw wave
			SQR //square
		};

		//this class encapsulates an oscillator
		struct Oscillator
		{
			char wave; //wave shape: SIN,SAW,SQR
			float freq; //frequency in Hz
			float amplitude; //0 to 1
			float pw;  //pulse width (only in SQR),
			float LPF; //low-pass filter: 1 no filter 0 all filtered

			float buffer[AUDIO_BUFFER_LENGTH];
			double _phase;
			float _last;

			Oscillator() {
				freq = 440; 
				amplitude = 0; 
				wave = SIN; 
				pw = 0.5;
				_phase = 0;
				_last = 0;
				LPF = 1;
			}
		};

		//master volume
		float volume;

		//oscilators
		Oscillator osc1;
		Oscillator osc2;
		Oscillator osc3;
		float noise_volume;

		float buffer[AUDIO_BUFFER_LENGTH];
		float time;

		Synth();
		~Synth();

		void generateAudio( Uint8* buffer, int len, SDL_AudioSpec& spec );
		void generateOscillator(Oscillator& osc, SDL_AudioSpec& spec);
		void applyFilter(Oscillator& osc, SDL_AudioSpec& spec);
};

#endif