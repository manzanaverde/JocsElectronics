#include "application.h"
#include "utils.h"
#include "image.h"
#include <string> 

Image font;
Image sprite;
int xPosition;
int yPosition;

// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
// things create here cannot access opengl
Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);
	Application::instance = this;

	xPosition = 0;
	yPosition = 0;

	// initialize attributes
	int w,h;
	SDL_GetWindowSize(window,&w,&h);
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
	must_exit = false;

	//open gamepads
	_joy1 = _joy2 = NULL;
	_joy1 = openGamepad(0);
	_joy2 = openGamepad(1);
	updateGamepadState(_joy1, gamepad1);
	updateGamepadState(_joy2, gamepad2);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//enableAudio(); //enable this line if you plan to add audio to your application
	font.loadTGA("data/bitmap-font-white.tga"); //load bitmap-font image
	sprite.loadTGA("data/spritesheet.tga"); //example to load an sprite

	//here add your loads and initializations
	//...








}

// Render one frame *************************************
void Application::render(void)
{
	//Create a new Image (or we could create a global one if we want to keep the previous frame)
	Image framebuffer( 128, 128 );
	
	//add your code here to fill the framebuffer
	//...

	//some new useful functions
		//framebuffer.fill(Color(40, 40, 40));						//fills the image with one color
		//framebuffer.drawLine( 0, 0, 100,100, Color::RED );		//draws a line
		//framebuffer.drawImage(sprite, 0, 0 );						//draws a full image
		framebuffer.drawImage( sprite, xPosition, yPosition, Rect(0,0,7,9) );	//draws only a part of an image
		//framebuffer.drawText( "HELLO", 0, 0, font );				//draws some text using a bitmap font in an image (assuming every char is 7x9)


		



	//send image to screen
	showFramebuffer( &framebuffer );
}

//called after render, update your variables here *******
void Application::update(double seconds_elapsed)
{
	//Add here your update method




	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (isKeyPressed(SDL_SCANCODE_UP)) //if key up
	{
		xPosition++;
	}
	if (isKeyPressed(SDL_SCANCODE_DOWN)) //if key down
	{
	}
	if (isKeyPressed(SDL_SCANCODE_A)) //if key A is pressed
	{
	}
	if (isKeyPressed(SDL_SCANCODE_Z)) //if key Z is pressed
	{
	}

	//to read the gamepad state
	if (gamepad1.isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (gamepad1.direction & PAD_UP) //left stick pointing up
	{
	}
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			must_exit = true; //this exists the mainloop and ends the app
			break; //ESC key, closes the app
	}
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		//if you read mouse position from the event, careful, Y is reversed, use mouse_position instead
	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

void Application::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	if (event.button == A_BUTTON)
	{
	}
}

void Application::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
}

void Application::onGamepadDirectionChange( int pad_index, char direction )
{
	if( direction & PAD_UP )
	{
	}
	if (direction & PAD_DOWN)
	{
	}
}

//when the app starts, launches the main loop
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	//infinite loop till the user press ESC
	launchLoop(this);
	//exit here
}


//sends the image to the framebuffer of the GPU
void Application::showFramebuffer(Image* img )
{
	Image resized = *img;
	//resized.quantize(1); //change this line to have a more retro look
	resized.scale(window_width, window_height);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (1) //flip
	{
		glRasterPos2f(-1, 1);
		glPixelZoom(1, -1);
	}

	glDrawPixels( resized.width, resized.height, GL_RGBA, GL_UNSIGNED_BYTE, resized.pixels );
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
	Uint8* stream,
	int    len)
{
	static double audio_time = 0;

	memset(stream, 0, len);//clear
	if (!Application::instance)
		return;

	Application::instance->onAudio(stream, len, audio_time, audio_spec);
	audio_time += len / (double)audio_spec.freq;
}

void Application::enableAudio()
{
	SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
	audio_spec.freq = 48000;
	audio_spec.format = AUDIO_U8;
	audio_spec.channels = 1;
	audio_spec.samples = 1024;
	audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */
	if (SDL_OpenAudio(&audio_spec, &audio_spec) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudio(0);
}

void Application::onAudio(Uint8* buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
	//fill the audio buffer using our custom retro synth
	synth.generateAudio(buffer, len, audio_spec);
}


Application* Application::instance = NULL;