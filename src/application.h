/*  by Javi Agenjo 2018 UPF  javi.agenjo@gmail.com
	This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"
#include "framework.h"
#include "utils.h"
#include "synth.h"

class Image;

class Application
{
public:
	//window
	SDL_Window* window;
	float window_width;
	float window_height;

	float time;
	bool must_exit;

	//keyboard state
	const Uint8* keystate; //now
	Uint8 prev_keystate[ SDL_NUM_SCANCODES ]; //previous before

	//audio
	Synth synth;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame

	//gamepad state
	GamepadState gamepad1;
	GamepadState gamepad2;

	//constructor
	Application(const char* caption, int width, int height);

	//main methods
	void init( void );
	void render( void );
	void update( double dt );

	//methods for events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp( SDL_MouseButtonEvent event );
	void onGamepadButtonDown(SDL_JoyButtonEvent event );
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onGamepadDirectionChange( int pad_index, char direction );

	//keyboard
	bool isKeyPressed(int key_code) { return keystate[key_code]; }
	bool wasKeyPressed(int key_code) { return keystate[key_code] && !prev_keystate[key_code]; }

	//audio stuff
	void enableAudio(); //opens audio channel to play sound
	void onAudio( Uint8* buffer, unsigned int len, double time, SDL_AudioSpec &audio_spec ); //called constantly to fill the audio buffer

	void showFramebuffer(Image* img);
	void start();

	//other methods to control the app
	void setWindowSize(int width, int height) {	glViewport( 0,0, width, height ); this->window_width = width; this->window_height = height;	}
	Vector2 getWindowSize()	{ int w,h; SDL_GetWindowSize(window,&w,&h); return Vector2(w,h); }

	static Application* instance;

	//internal
	SDL_Joystick* _joy1;
	SDL_Joystick* _joy2;
};


#endif 