/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This contains several functions that can be useful when programming your game.
*/
#ifndef UTILS_H
#define UTILS_H

#include "includes.h"
#include "framework.h"

//General functions **************
class Application;
class Image;

//check opengl errors
bool checkGLErrors();

SDL_Window* createWindow(const char* caption, int width, int height);
void launchLoop(Application* app);

void showImage(Image* img);

//fast random generator
inline unsigned long frand(void) {          //period 2^96-1
	unsigned long t;
	static unsigned long x = 123456789, y = 362436069, z = 521288629;
	x ^= x << 16; x ^= x >> 5; x ^= x << 1;
	t = x; x = y;y = z;
	z = t ^ x ^ y;
	return z;
}

//random between 0 and 1
inline float random() { return (frand() % 10000) / 10000.0; }


//mapped as in SDL
enum Gamepad
{
	//axis
	LEFT_ANALOG_X = 0,
	LEFT_ANALOG_Y = 1,
	RIGHT_ANALOG_X = 4,
	RIGHT_ANALOG_Y = 3,
	TRIGGERS = 2, //both triggers share an axis (positive is right, negative is left trigger)

	//buttons
	A_BUTTON = 0,
	B_BUTTON = 1,
	X_BUTTON = 2,
	Y_BUTTON = 3,
	LB_BUTTON = 4,
	RB_BUTTON = 5,
	BACK_BUTTON = 6,
	START_BUTTON = 7,
	LEFT_ANALOG_BUTTON = 8,
	RIGHT_ANALOG_BUTTON = 9
};

enum HATState
{
	PAD_CENTERED = 0x00,
	PAD_UP = 0x01,
	PAD_RIGHT = 0x02,
	PAD_DOWN = 0x04,
	PAD_LEFT = 0x08,
	PAD_RIGHTUP = (PAD_RIGHT | PAD_UP),
	PAD_RIGHTDOWN = (PAD_RIGHT | PAD_DOWN),
	PAD_LEFTUP = (PAD_LEFT | PAD_UP),
	PAD_LEFTDOWN = (PAD_LEFT | PAD_DOWN)
};

struct GamepadState
{
	int num_axis;	//num analog sticks
	int num_buttons; //num buttons
	float axis[8]; //analog stick
	char button[16]; //buttons
	char prev_button[16]; //buttons in the previous frame
	char direction; //which direction is the left stick pointing at
	char prev_direction; //which direction was the left stick before
	HATState hat; //digital pad

	bool isButtonPressed(int num) { return button[num]; }
	bool wasButtonPressed(int num) { return button[num] & !prev_button[num]; }
	bool didDirectionChanged(int num) { return direction != prev_direction; }
};

SDL_Joystick* openGamepad( int index );
void updateGamepadState(SDL_Joystick* joystick, GamepadState& state);


#endif
