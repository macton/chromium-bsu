/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "MainSDL.h"

#ifdef USE_SDL

#include <cstdlib>
#include <cmath>
#include <SDL.h>

#ifdef ENABLE_WINDOW_ICON
#include <SDL/SDL_image.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || (defined(HAVE_OPENGL_GL_H) && defined(HAVE_OPENGL_GLU_H))
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Config.h"
#include "Global.h"

#include "extern.h"
#include "Global.h"
#include "HeroAircraft.h"
#include "Audio.h"
#include "MainGL.h"


//====================================================================
MainSDL::MainSDL(int argc, char **argv)
	: MainToolkit(argc, argv)
{	
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	mouseToggle = game->mouseActive;
	fire = 0;
	xjoy = yjoy = xjNow = yjNow = 0;
	adjCount = 0;
	key_speed_x = key_speed_y = 0;
	
	Uint32 initOpts;
	
	initOpts = SDL_INIT_VIDEO;
	
#ifdef WITH_JOYSTICK
	initOpts = initOpts|SDL_INIT_JOYSTICK;
#endif
#ifdef NO_PARACHUTE
	initOpts = initOpts|SDL_INIT_NOPARACHUTE;
#endif
	if(config->useCDROM())
		initOpts = initOpts|SDL_INIT_CDROM;
		
	if( SDL_Init( initOpts ) < 0 ) 
	{
		fprintf(stderr,_("Couldn't initialize SDL: %s\n"), SDL_GetError());
		exit( 1 );
	}
	if( config->debug() ) fprintf(stderr, _("SDL initialized.\n"));

#ifdef WITH_JOYSTICK
	int nj = SDL_NumJoysticks();
	if(nj > 0)
	{
		if( config->debug() ) fprintf(stderr, _("num joysticks = %d\n"), nj);
		joystick = SDL_JoystickOpen(0);
		if( config->debug() ) fprintf(stderr, _("   joystick 0 = %p\n"), joystick);
		if(joystick)
			SDL_JoystickEventState(SDL_ENABLE);
	}
	else
	{
		if( config->debug() ) fprintf(stderr, _("no joysticks found\n"));
		joystick = 0;
	}
#else
	joystick = 0;
#endif

#ifdef ENABLE_WINDOW_ICON
	//-- Set the window manager icon
#ifdef _WIN32
	// On Windows SDL can only do 32x32 icons
	#define ICON "png/icon32.png"
#else
	// Elsewhere SDL can do any size icons
	#define ICON "png/icon64.png"
#endif
	SDL_Surface *icon = IMG_Load(dataLoc(ICON));
	if (icon) {
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}
#endif

	if( !setVideoMode() )
	{
		fprintf(stderr, _("Couldn't set video mode: %s\n"), SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	if( config->debug() )
	{
		fprintf(stderr, _(
			"-OpenGL-----------------------------------------------------\n"
			"Vendor     : %s\n"
			"Renderer   : %s\n"
			"Version    : %s\n"),
			glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION) );
		printExtensions(stderr,  (const char*)glGetString( GL_EXTENSIONS ));
		fprintf(stderr, _("------------------------------------------------------------\n"));
	}

	//-- Set the window manager title bar
	SDL_WM_SetCaption( "Chromium B.S.U.", "Chromium B.S.U." );
	
	//-- Create game
	game->createGame();
}

MainSDL::~MainSDL()
{
}

//----------------------------------------------------------
bool MainSDL::run()
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	float targetAdj		= 1.0;
	Uint32 now_time		= 0; 
	Uint32 last_time	= 0;
	key_speed_x  = key_speed_y = 0.0;
	int done = 0;
	int frames;

	//-- enter main loop...
	frames = 0;
	while( !done ) 
	{
		SDL_Event event;
		
		//-- Draw our scene...
		game->mainGL->drawGL();
		
		SDL_GL_SwapBuffers( );
		
		#ifdef CHECK_ERRORS
		checkErrors();
		#endif// CHECK_ERRORS

		//-- Delay
		SDL_Delay( 32-(int)(24.0*game->gameSpeed) );
//		SDL_Delay( 8 );
//		SDL_Delay( 16 );
//		SDL_Delay( 32 );
//		SDL_Delay( 66 );

//		//-- cheezy, partially functional record mechanism
//		bool write = false;
//		SDL_Event *fileEvent;
//		if( !write && game->gameMode == Global::Game)
//		{
//			while( (fileEvent = getEvent(game->eventFile)) ) 
//				done = this->process(fileEvent);
//		}
		
		/* Check if there's a pending event. */
		while( SDL_PollEvent( &event ) ) 
		{
//			if(write)
//				saveEvent(&event);
			done = this->process(&event);
		}
		this->joystickMove();
		this->keyMove();
		++frames;
		
		game->frame++;
		if( !(game->gameFrame%10) )
		{
			now_time = SDL_GetTicks();
			if(last_time)
			{
				game->fps = (10.0/(now_time-last_time))*1000.0;
			}
			last_time = now_time;
			
			if(game->gameMode != Global::Menu)
			{
				if(game->gameFrame < 400)
				{
					if(game->fps < 48.0 && game->gameSpeed < 1.0)
					{
						game->gameSpeed += 0.02;
						if( config->debug() ) fprintf(stdout, _("init----> %3.2ffps gameSpeed = %g\n"), game->fps, game->gameSpeed);
					}
					else if(game->gameFrame > 20)
					{
						float tmp = 50.0/game->fps;
						tmp = 0.8*targetAdj + 0.2*tmp;
						targetAdj = floor(100.0*(tmp+0.005))/100.0;
						if( config->debug() ) fprintf(stdout, _("init----> %3.2ffps targetAdj = %g, tmp = %g\n"), game->fps, targetAdj, tmp);
					}
				}
				else if( config->autoSpeed() && (game->fps > 30.0 && game->fps < 100.0))  // discount any wacky fps from pausing
				{
					//game->speedAdj = targetAdj;
					// Everything was originally based on 50fps - attempt to adjust
					// if we're outside a reasonable range
					float tmp = 50.0/game->fps;
					if( fabs(targetAdj-tmp) > 0.1)
					{
						adjCount++;
						game->speedAdj = tmp;
						if( config->debug() ) fprintf(stdout, _("adjust--> %3.2f targetAdj = %g -- game->speedAdj = %g\n"), game->fps, targetAdj, game->speedAdj);
					}
					else
						game->speedAdj = targetAdj;
				}
				else
					game->speedAdj = targetAdj;
					
//				if( !(frames%500) )
//					if( config->debug() ) fprintf(stdout, _("fps = %g speedAdj = %g\n"), game->fps, game->speedAdj);
			}
			
		}
	}
	fflush(stdout);
	
	//-- Delete game objects
	game->deleteGame();
	
	if(adjCount > 20)
	{
		fprintf(stderr, _(
			"%d speed adjustments required.\n"
			"NOTE: The game was not able to maintain a steady 50 frames per\n"
			"      second. You should consider reducing your screen resolution\n"
			"      or lowering the graphics detail setting.\n"
			"      -OR-\n"
			"      make sure that you aren't running any system monitoring\n"
			"      tools (like \'top\', \'xosview\', etc.) These kinds of tools\n"
			"      can make it difficult to maintain a steady frame rate.\n"),
			adjCount);
	}
	
	//-- Destroy our GL context, etc.
	if( config->debug() ) fprintf(stderr, _("exit.\n"));
	SDL_Quit();
	
	return false;
}

//----------------------------------------------------------
bool MainSDL::checkErrors()
{
	bool retVal = false;
	GLenum	gl_error;
	char*	sdl_error;
	
	//-- Check for GL errors
	gl_error = glGetError( );
	if( gl_error != GL_NO_ERROR ) 
	{
		fprintf(stderr, _("ERROR!!! OpenGL error: %s\n"), gluErrorString(gl_error) );
		retVal = true;
	}

	//-- Check for SDL errors
	sdl_error = SDL_GetError( );
	if( sdl_error[0] != '\0' ) 
	{
		fprintf(stderr, _("ERROR!!! SDL error '%s'\n"), sdl_error);
		SDL_ClearError();
		retVal = true;
	}
	
	return retVal;
}

//----------------------------------------------------------
bool MainSDL::setVideoMode()
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	int w;
	int h;
	Uint32 video_flags;
	SDL_Surface *glSurface = 0;
	
	//-- Set the flags we want to use for setting the video mode
	video_flags = SDL_OPENGL;
	if(config->fullScreen())
		video_flags |= SDL_FULLSCREEN;
	
	w = config->screenW();
	h = config->screenH();
	
	int rs, gs, bs, ds;
	int bpp;
	if(config->trueColor())
	{
		//-- 24 bit color
		bpp = 24;
		rs = gs = bs = 8;
		ds = 16;
	}
	else
	{
		//-- 16 bit color
		bpp = 16;
		rs = bs = 5;
		gs = 6;
		ds = 16;
	}
	
	//-- Initialize display
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,	rs );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,	gs );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,	bs );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,	ds );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	if ( (glSurface = SDL_SetVideoMode( w, h, bpp, video_flags )) == NULL ) 
	{
		fprintf(stderr, _("Couldn't set video mode: %s\n"), SDL_GetError());
		return false;
	}
	else
	{
		if( config->debug() ) fprintf(stderr, _("video mode set "));
	}
	
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, 	&rs);
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE,	&gs);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE,	&bs);
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE,	&ds);
	if( config->debug() ) fprintf(stderr, _("(bpp=%d RGB=%d%d%d depth=%d)\n"), glSurface->format->BitsPerPixel, rs, gs, bs, ds);

	if(game->mainGL)
		game->mainGL->initGL();

	return true;
}

#endif // USE_SDL
