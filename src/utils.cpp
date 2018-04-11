#include "utils.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#include "includes.h"
#include "application.h"
#include "image.h"


//this function is used to access OpenGL Extensions (special features not supported by all cards)
void* getGLProcAddress(const char* name)
{
	return SDL_GL_GetProcAddress(name);
}

bool checkGLErrors()
{
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL Error: " << errString << std::endl;
		return false;
	}

	return true;
}

SDL_Joystick* openGamepad(int index )
{
	// Check for number of joysticks available
	if (SDL_NumJoysticks() <= index)
		return NULL;

	// Open joystick and return it
	return SDL_JoystickOpen(index);
}

void updateGamepadState( SDL_Joystick* joystick, GamepadState& state )
{
	//save old state
	int prev_direction = state.direction;
	char prev_button[16];
	memcpy(prev_button, state.button, 16 );

	//set all to 0
	memset(&state, 0, sizeof(GamepadState)); 

	if (joystick == NULL)
		return;

	state.num_axis = SDL_JoystickNumAxes((::SDL_Joystick*) joystick);
	state.num_buttons = SDL_JoystickNumButtons((::SDL_Joystick*)joystick);

	if (state.num_axis > 8) state.num_axis = 8;
	if (state.num_buttons > 16) state.num_buttons = 16;

	for (int i = 0; i < state.num_axis; ++i) //axis
		state.axis[i] = SDL_JoystickGetAxis((::SDL_Joystick*) joystick, i) / 32768.0f; //range -32768 to 32768
	for (int i = 0; i < state.num_buttons; ++i) //buttons
		state.button[i] = SDL_JoystickGetButton((::SDL_Joystick*) joystick, i);
	state.hat = (HATState)(SDL_JoystickGetHat((::SDL_Joystick*) joystick, 0) - SDL_HAT_CENTERED); //one hat is enough
	memcpy(state.prev_button, prev_button, 16); //copy prev buttons state

	Vector2 axis_direction( state.axis[LEFT_ANALOG_X], state.axis[LEFT_ANALOG_Y] );
	state.prev_direction = prev_direction;
	state.direction = 0;
	float limit = 0.6;
	if (axis_direction.x < -limit)
		state.direction |= PAD_LEFT;
	else if (axis_direction.x > limit)
		state.direction |= PAD_RIGHT;
	if (axis_direction.y < -limit)
		state.direction |= PAD_UP;
	else if (axis_direction.y > limit)
		state.direction |= PAD_DOWN;
}


//create a window using SDL
SDL_Window* createWindow(const char* caption, int width, int height)
{
	int bpp = 0;

	SDL_Init(SDL_INIT_EVERYTHING);

	//GPU
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); //or 24
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	//antialiasing (disable this lines if it goes too slow)
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8 ); //increase to have smoother polygons

	// Initialize the joystick subsystem
	SDL_InitSubSystem( SDL_INIT_JOYSTICK );

	//create the window
	SDL_Window *window = SDL_CreateWindow(
		caption, 100, 100, width, height, 
		SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

	if(!window)
	{
		fprintf(stderr, "Window creation error: %s\n", SDL_GetError());
		exit(-1);
	}
  
	// Create an OpenGL context associated with the window.
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//in case of exit...
	atexit(SDL_Quit);

	//get events from the queue of unprocessed events
	SDL_PumpEvents(); //without this line asserts could fail on windows

	return window;
}

//The application main loop
void launchLoop(Application* app)
{
	SDL_Event sdlEvent;
	double last_time = SDL_GetTicks();
	int x,y;

	SDL_GetMouseState(&x,&y);
	y = app->window_height - y;
	app->mouse_position.set(x,y);
	app->keystate = SDL_GetKeyboardState(NULL);

	double start_time = SDL_GetTicks();

	//infinite loop
	while ( !app->must_exit )
	{
		//read keyboard state and stored in keystate
		memcpy( (void*)&app->prev_keystate, app->keystate, SDL_NUM_SCANCODES );
		app->keystate = SDL_GetKeyboardState(NULL);


		//Render frame and send it to screen

			// Clear the window and the depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//call render function
			app->render();
			//swap between front buffer and back buffer to show it 
			SDL_GL_SwapWindow(app->window);

		//read events from the system
		while(SDL_PollEvent(&sdlEvent))
		{
			switch(sdlEvent.type)
				{
					case SDL_QUIT: return; break; //EVENT for when the user clicks the [x] in the corner
					case SDL_MOUSEBUTTONDOWN: //EXAMPLE OF sync mouse input
						app->mouse_state |= SDL_BUTTON(sdlEvent.button.button);
						app->onMouseButtonDown(sdlEvent.button);
						break;
					case SDL_MOUSEBUTTONUP:
						app->mouse_state &= ~SDL_BUTTON(sdlEvent.button.button);
						app->onMouseButtonUp(sdlEvent.button);
						break;
					case SDL_KEYDOWN:
						app->onKeyDown(sdlEvent.key);
						break;
					case SDL_KEYUP: 
						app->onKeyUp(sdlEvent.key);
						break;
					case SDL_JOYBUTTONDOWN: 
						app->onGamepadButtonDown(sdlEvent.jbutton);
						break;
					case SDL_JOYBUTTONUP:
						app->onGamepadButtonUp(sdlEvent.jbutton);
						break;
					case SDL_TEXTINPUT:
						// you can read the ASCII character from sdlEvent.text.text 
						break;
					case SDL_WINDOWEVENT:
						switch (sdlEvent.window.event) {
							case SDL_WINDOWEVENT_RESIZED: //resize opengl context
								std::cout << "window resize" << std::endl;
								app->setWindowSize( sdlEvent.window.data1, sdlEvent.window.data2 );
								break;
						}
				}
		}

		//get mouse position and delta
		app->mouse_state = SDL_GetMouseState(&x,&y);
		y = app->window_height - y; //reverse
		app->mouse_delta.set( app->mouse_position.x - x, app->mouse_position.y - y );
		app->mouse_position.set(x,y);

		//get gamepad state
		if (app->_joy1)
		{
			updateGamepadState(app->_joy1, app->gamepad1);
			if (app->gamepad1.prev_direction != app->gamepad1.direction)
				app->onGamepadDirectionChange(1, app->gamepad1.direction);
		}
		if (app->_joy2)
		{
			updateGamepadState(app->_joy2, app->gamepad2);
			if (app->gamepad2.prev_direction != app->gamepad2.direction)
				app->onGamepadDirectionChange(2, app->gamepad2.direction);
		}

		//update logic
		double now = SDL_GetTicks();
		double elapsed_time = (now - last_time) * 0.001; //0.001 converts from milliseconds to seconds
		app->time = (now - start_time) * 0.001;
		app->update(elapsed_time);
		last_time = now;

		//check errors in opengl only when working in debug
		#ifdef _DEBUG
			checkGLErrors();
		#endif
	}

	//destroy all
	SDL_CloseAudio();

	return;
}






