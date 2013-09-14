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
#include <cstdio>
#include <cstring>

#include "Config.h"

#include "Global.h"
#include "HeroAircraft.h"
#include "MenuGL.h"
#include "Audio.h"
#include "Ground.h"

#if SDL_VERSION_ATLEAST(2,0,0)
		#define SDLK_KP1 SDLK_KP_1
		#define SDLK_KP2 SDLK_KP_2
		#define SDLK_KP3 SDLK_KP_3
		#define SDLK_KP4 SDLK_KP_4
		#define SDLK_KP5 SDLK_KP_5
		#define SDLK_KP6 SDLK_KP_6
		#define SDLK_KP7 SDLK_KP_7
		#define SDLK_KP8 SDLK_KP_8
		#define SDLK_KP9 SDLK_KP_9
#endif

//----------------------------------------------------------
bool MainSDL::process(SDL_Event *event)
{
	Global	*game = Global::getInstance();
#if 0
	static int cnt = 0;
	cnt++;
	switch( event->type ) 
	{
	    case SDL_ACTIVEEVENT:		fprintf(stderr, _("%-5d SDL_ACTIVEEVENT  \n")		, cnt); break;
	    case SDL_KEYDOWN:			fprintf(stderr, _("%-5d SDL_KEYDOWN  \n")			, cnt); break;
	    case SDL_KEYUP: 			fprintf(stderr, _("%-5d SDL_KEYUP  \n")			, cnt); break;
	    case SDL_MOUSEMOTION:		fprintf(stderr, _("%-5d SDL_MOUSEMOTION  \n")		, cnt); break;
	    case SDL_MOUSEBUTTONDOWN:	fprintf(stderr, _("%-5d SDL_MOUSEBUTTONDOWN  \n")	, cnt); break;
	    case SDL_MOUSEBUTTONUP: 	fprintf(stderr, _("%-5d SDL_MOUSEBUTTONUP  \n")	, cnt); break;
		case SDL_JOYBUTTONDOWN: 	fprintf(stderr, _("%-5d SDL_JOYBUTTONDOWN  \n")	, cnt); break;
		case SDL_JOYBUTTONUP:		fprintf(stderr, _("%-5d SDL_JOYBUTTONUP  \n")		, cnt); break;
		default:
			break;
	}
#endif
	bool shown, mouse, input, gain = false;
	switch(event->type) 
	{
#if SDL_VERSION_ATLEAST(2,0,0)
	    case SDL_WINDOWEVENT:
		switch (event->window.event) {
			case SDL_WINDOWEVENT_RESTORED: shown = true; gain = true; break;
			case SDL_WINDOWEVENT_MINIMIZED: shown = true; gain = false; break;
			case SDL_WINDOWEVENT_ENTER: mouse = true; gain = true; break;
			case SDL_WINDOWEVENT_LEAVE: mouse = true; gain = false; break;
			case SDL_WINDOWEVENT_FOCUS_GAINED: input = true; gain = true; break;
			case SDL_WINDOWEVENT_FOCUS_LOST: input = true; gain = false; break;
		}
		activation(shown, mouse, input, gain);
		break;
#else
	    case SDL_ACTIVEEVENT:
			shown = event->active.state & SDL_APPACTIVE ? true : false;
			mouse = event->active.state & SDL_APPMOUSEFOCUS ? true : false;
			input = event->active.state & SDL_APPINPUTFOCUS ? true : false;
			gain = event->active.gain ? true : false;
			activation(shown, mouse, input, gain);
			break;
#endif
	    case SDL_KEYDOWN:
			keyDown(event);
			break;
	    case SDL_KEYUP:
			keyUp(event);
			break;
	    case SDL_MOUSEMOTION:
			mouseMotion(event);
			break;
	    case SDL_MOUSEBUTTONDOWN:
			mouseButtonDown(event);
			break;
	    case SDL_MOUSEBUTTONUP:
			mouseButtonUp(event);
			break;
//		case SDL_JOYAXISMOTION:
//			joystickMotion(event);
//			break;
		case SDL_JOYBUTTONDOWN:
			joystickButtonDown(event);
			break;
		case SDL_JOYBUTTONUP:
			joystickButtonUp(event);
			break;
		case SDL_QUIT:
			return true;
		default:
			break;
	}
	return game->game_quit;
}

//----------------------------------------------------------
void MainSDL::saveEvent(SDL_Event *event)
{
	Global	*game = Global::getInstance();
	if(game->eventFile && game->gameMode == Global::Game)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_WindowEvent 		*evW = 0;
#else
		SDL_ActiveEvent 		*evA = 0;
#endif
		SDL_KeyboardEvent 		*evK = 0;
		SDL_MouseMotionEvent 	*evM = 0;
		SDL_MouseButtonEvent 	*evB = 0;
		switch(event->type) 
		{
//	    	case SDL_ACTIVEEVENT:
//				evA = (SDL_ActiveEvent*)event;
//				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "A",
//					(int)evA->type, (int)evA->gain, (int)evA->state, 0, 0, 0);
//				break;
//	    	case SDL_KEYDOWN:
//	    	case SDL_KEYUP:
//				evK = (SDL_KeyboardEvent*)event;
//				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
//					(int)evK->type, (int)evK->state, 
//					(int)evK->keysym.scancode, (int)evK->keysym.sym, (int)evK->keysym.mod, (int)evK->keysym.unicode);
//				break;
//	    	case SDL_MOUSEMOTION:
//				evM = (SDL_MouseMotionEvent*)event;
//				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "M",
//					(int)evM->type, (int)evM->state, (int)evM->x, (int)evM->y, (int)evM->xrel, (int)evM->yrel);
//				break;
//	    	case SDL_MOUSEBUTTONDOWN:
//	    	case SDL_MOUSEBUTTONUP:
//				evB = (SDL_MouseButtonEvent*)event;
//				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "B",
//					(int)evB->type, (int)evB->button, (int)evB->state, (int)evB->x, (int)evB->y, 0);
//				break;
//			default:
//				break;
#if SDL_VERSION_ATLEAST(2,0,0)
		case SDL_WINDOWEVENT:
				evW = (SDL_WindowEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "W",
					evW->type, evW->windowID, evW->event, evW->data1, evW->data2, 0);
				break;
	    	case SDL_KEYDOWN:
	    	case SDL_KEYUP:
				evK = (SDL_KeyboardEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
					evK->type, evK->state, 
					evK->keysym.scancode, evK->keysym.sym, evK->keysym.mod);
				break;
#else
	    	case SDL_ACTIVEEVENT:
				evA = (SDL_ActiveEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "A",
					evA->type, evA->gain, evA->state, 0, 0, 0);
				break;
	    	case SDL_KEYDOWN:
	    	case SDL_KEYUP:
				evK = (SDL_KeyboardEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
					evK->type, evK->state, 
					evK->keysym.scancode, evK->keysym.sym, evK->keysym.mod, evK->keysym.unicode);
				break;
#endif
	    	case SDL_MOUSEMOTION:
				evM = (SDL_MouseMotionEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "M",
					evM->type, evM->state, evM->x, evM->y, evM->xrel, evM->yrel);
				break;
	    	case SDL_MOUSEBUTTONDOWN:
	    	case SDL_MOUSEBUTTONUP:
				evB = (SDL_MouseButtonEvent*)event;
				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "B",
					evB->type, evB->button, evB->state, evB->x, evB->y, 0);
				break;
			default:
				break;
		}
	}
}

//----------------------------------------------------------
SDL_Event *MainSDL::getEvent(FILE *infile)
{
	Global	*game = Global::getInstance();
	static int count = 0;
	static	SDL_Event ev;
	static	char buffer[256] = { "-1 :K: 2 0 0 0 0 0"};
	SDL_Event	*retVal = 0;
	
	if(infile)
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_WindowEvent 		*evW = (SDL_WindowEvent*)&ev;
#else
		SDL_ActiveEvent 		*evA = (SDL_ActiveEvent*)&ev;
#endif
		SDL_KeyboardEvent 		*evK = (SDL_KeyboardEvent*)&ev;
		SDL_MouseMotionEvent 	*evM = (SDL_MouseMotionEvent*)&ev;
		SDL_MouseButtonEvent 	*evB = (SDL_MouseButtonEvent*)&ev;
		int		frame;
		int		a,b,c,d,e,f;
		char	t;
		
		sscanf(buffer, "%d :%c: %d %d %d %d %d %d", &frame, &t, &a, &b, &c, &d, &e, &f);
		if(frame == game->gameFrame)
		{
			switch(t)
			{
#if SDL_VERSION_ATLEAST(2,0,0)
				case 'W':
					evW->type	= (Uint32)a;
					evW->windowID	= (Uint32)b;
					evW->event	= (Uint8)c;
					evW->data1	= d;
					evW->data2	= e;
					retVal = &ev;
					break;
#else
				case 'A':
					evA->type	= (Uint8)a;
					evA->gain	= (Uint8)b;
					evA->state	= (Uint8)c;
					retVal = &ev;
					break;
#endif
				case 'K':
					evK->type	= (Uint8)a;
					evK->state	= (Uint8)b;
#if SDL_VERSION_ATLEAST(2,0,0)
					evK->keysym.scancode = (SDL_Scancode)c;
					evK->keysym.sym		 = (SDL_Keycode)d;
					evK->keysym.mod		 = (SDL_Keymod)e;
#else
					evK->keysym.scancode = (Uint8)c;
					evK->keysym.sym		 = (SDLKey)d;
					evK->keysym.mod		 = (SDLMod)e;
					evK->keysym.unicode	 = (Uint16)f;
#endif
					retVal = &ev;	
					break;
				case 'M':
					evM->type	= (Uint8)a;
					evM->state	= (Uint8)b;
					evM->x		= (Uint16)c;
					evM->y		= (Uint16)d;
					evM->xrel	= (Sint16)e;
					evM->yrel	= (Sint16)f;
					retVal = &ev;
					break;
				case 'B':
					evB->type	= (Uint8)a;
					evB->button	= (Uint8)b;
					evB->state	= (Uint8)c;
					evB->x		= (Uint16)d;
					evB->y		= (Uint16)e;
					retVal = &ev;
					break;
				default:
					fprintf(stderr, _("MainSDL::getEvent HUH?\n"));
					break;
			}
			count++;
		}
		bool ok = true;
		if(frame <= game->gameFrame)
			ok = fgets(buffer, 256, infile);
		if(!ok)
		{
			fprintf(stderr, _("buffer = \"%s\n\""), buffer);
			exit(1);
		}
	}
	return retVal;
}

//----------------------------------------------------------
void MainSDL::activation(bool shown, bool mouse, bool input, bool gain)
{
	Global *game = Global::getInstance();
	Config* config = Config::instance();
	if( config->debug() ) fprintf(stderr, "app %s ", gain ? "gained" : "lost" );
	bool grab_mouse = game->gameMode == Global::Game && !game->game_pause && gain ? true : false;
	if ( shown )
	{
		grabMouse( grab_mouse, grab_mouse );
		if( config->debug() ) fprintf(stderr, "app active " );
	} 
	else if ( mouse )
	{
		if( config->debug() ) fprintf(stderr, _("mouse active") );
		SDL_GetMouseState(&xLast, &yLast);
	} 
	else if ( input )
	{
		grabMouse( grab_mouse, grab_mouse );
		if( config->debug() ) fprintf(stderr, "input active" );
	}
	if( config->debug() ) fprintf(stderr, _("focus\n") );
}

//----------------------------------------------------------
void MainSDL::keyUp(SDL_Event *event)
{
	Global *game = Global::getInstance();
	switch(event->key.keysym.sym)
	{
		case SDLK_SPACE:
			game->hero->fireGun(false);
			break;
		default:
			break;
	}
}

//----------------------------------------------------------
void MainSDL::keyDown(SDL_Event *event)
{
	Global	*game = Global::getInstance();
	switch(event->key.keysym.sym)
	{
	    case SDL_QUIT:
		case SDLK_q:
			game->game_quit = true;
			break;
		case SDLK_g:
			grabMouse(!mouseToggle);
			break;
		case SDLK_0:
			game->hero->useItem(0);
			break;
		case SDLK_9:
			game->hero->useItem(1);
			break;
		case SDLK_ESCAPE:
			if(game->gameMode == Global::Menu)
			{
				game->gameMode = Global::Game;
				game->audio->setMusicMode(Audio::MusicGame);
				grabMouse(!game->game_pause);
			}
			else
			{
				if(game->gameMode != Global::Game)
				{
					game->newGame();
				}
				game->gameMode = Global::Menu;
				game->menu->startMenu();
				game->audio->setMusicMode(Audio::MusicMenu);
				grabMouse(false);
			}
			break;
		default:
			if(game->gameMode == Global::Game)
				keyDownGame(event);
			else if(game->gameMode == Global::LevelOver)
			{
				game->gotoNextLevel();
				game->gameMode = Global::Game;
				game->audio->setMusicMode(Audio::MusicGame);
			}
			else if(game->gameMode == Global::HeroDead)
			{
				game->gameMode = Global::Game;
				game->newGame();
				game->toolkit->grabMouse(true);
				game->audio->setMusicMode(Audio::MusicGame);
			}
			else
			{
				MainToolkit::Key key;
				switch(event->key.keysym.sym)
				{
					case SDLK_KP9: case SDLK_KP7: case SDLK_KP8: case SDLK_UP:		key = MainToolkit::KeyUp;		break;
					case SDLK_KP1: case SDLK_KP3: case SDLK_KP2: case SDLK_DOWN:		key = MainToolkit::KeyDown;		break;
					case SDLK_KP4: case SDLK_LEFT:		key = MainToolkit::KeyLeft;		break;
					case SDLK_KP6: case SDLK_RIGHT:	key = MainToolkit::KeyRight;	break;
					case SDLK_KP_ENTER: case SDLK_RETURN:	key = MainToolkit::KeyEnter;	break;
					case SDLK_KP5: return;
					default: key = MainToolkit::KeyEnter;	break;
				}
				game->menu->keyHit(key);
			}
			break;
	}
}

//----------------------------------------------------------
void MainSDL::keyDownGame(SDL_Event *event)
{
	Global	*game = Global::getInstance();
	Config *config = Config::instance();
	switch(event->key.keysym.sym)
	{
		case SDLK_KP_PLUS:
		case SDLK_QUOTE:
				game->hero->nextItem();
			break;
		case SDLK_KP_ENTER:
		case SDLK_RETURN:
				game->hero->useItem();
			break;
	    case SDLK_PAUSE:
	    case SDLK_p:
			grabMouse(game->game_pause);
			game->game_pause = !game->game_pause;
			game->audio->pauseGameMusic(game->game_pause);
			break;
	    case SDLK_n:
			game->audio->nextMusicIndex();
			break;
	    case SDLK_KP7:
			key_speed_x -= 5.0;
			key_speed_y -= 5.0;
			break;
	    case SDLK_KP9:
			key_speed_x += 5.0;
			key_speed_y -= 5.0;
			break;
	    case SDLK_KP3:
			key_speed_x += 5.0;
			key_speed_y += 5.0;
			break;
	    case SDLK_KP1:
			key_speed_x -= 5.0;
			key_speed_y += 5.0;
			break;
	    case SDLK_KP4:
	    case SDLK_LEFT:
			key_speed_x -= 5.0;
			break;
	    case SDLK_KP6:
	    case SDLK_RIGHT:
			key_speed_x += 5.0;
			break;
	    case SDLK_KP8:
	    case SDLK_UP:
			key_speed_y -= 5.0;
			break;
	    case SDLK_KP2:
	    case SDLK_DOWN:
			key_speed_y += 5.0;
			break;
	    case SDLK_SPACE:
			game->hero->fireGun(true);
			break;
		default:
			if( config->debug() )
			{
				fprintf(stderr, _("key '%s' pressed\n"), SDL_GetKeyName(event->key.keysym.sym));
				fprintf(stderr, _("game->gameFrame = %d\n"), game->gameFrame);
			}
			break;
	}
}

//----------------------------------------------------------
void MainSDL::keyMove()
{
	Global	*game = Global::getInstance();
	if(game->gameMode == Global::Game){
#if SDL_VERSION_ATLEAST(2,0,0)
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);
		#define LEFT SDL_SCANCODE_LEFT
		#define RIGHT SDL_SCANCODE_RIGHT
		#define UP SDL_SCANCODE_UP
		#define DOWN SDL_SCANCODE_DOWN
		#define KP1 SDL_SCANCODE_KP_1
		#define KP2 SDL_SCANCODE_KP_2
		#define KP3 SDL_SCANCODE_KP_3
		#define KP4 SDL_SCANCODE_KP_4
		#define KP5 SDL_SCANCODE_KP_5
		#define KP6 SDL_SCANCODE_KP_6
		#define KP7 SDL_SCANCODE_KP_7
		#define KP8 SDL_SCANCODE_KP_8
		#define KP9 SDL_SCANCODE_KP_9
#else
		Uint8 *keystate = SDL_GetKeyState(NULL);
		#define LEFT SDLK_LEFT
		#define RIGHT SDLK_RIGHT
		#define UP SDLK_UP
		#define DOWN SDLK_DOWN
		#define KP1 SDLK_KP1
		#define KP2 SDLK_KP2
		#define KP3 SDLK_KP3
		#define KP4 SDLK_KP4
		#define KP5 SDLK_KP5
		#define KP6 SDLK_KP6
		#define KP7 SDLK_KP7
		#define KP8 SDLK_KP8
		#define KP9 SDLK_KP9
#endif
		if( keystate[LEFT]  || keystate[KP4] ) key_speed_x -= 2.0 + abs(key_speed_x)*0.4;
		if( keystate[RIGHT] || keystate[KP6] ) key_speed_x += 2.0 + abs(key_speed_x)*0.4;
		if( keystate[UP]    || keystate[KP8] ) key_speed_y -= 2.0 + abs(key_speed_y)*0.4;
		if( keystate[DOWN]  || keystate[KP2] ) key_speed_y += 2.0 + abs(key_speed_y)*0.4;
		if( keystate[KP7] ){ key_speed_x -= 2.0 + abs(key_speed_x)*0.4; key_speed_y -= 2.0 + abs(key_speed_y)*0.4; }
		if( keystate[KP9] ){ key_speed_x += 2.0 + abs(key_speed_x)*0.4; key_speed_y -= 2.0 + abs(key_speed_y)*0.4; }
		if( keystate[KP3] ){ key_speed_x += 2.0 + abs(key_speed_x)*0.4; key_speed_y += 2.0 + abs(key_speed_y)*0.4; }
		if( keystate[KP1] ){ key_speed_x -= 2.0 + abs(key_speed_x)*0.4; key_speed_y += 2.0 + abs(key_speed_y)*0.4; }
		//float s = (1.0-game->speedAdj)+(game->speedAdj*0.7);
		float s = 0.7;
		key_speed_x *= s;
		key_speed_y *= s;
		game->hero->moveEvent(key_speed_x,key_speed_y);
	}
}

//----------------------------------------------------------
void MainSDL::mouseMotion(SDL_Event *event)
{
	Global	*game = Global::getInstance();
	int xNow;
	int yNow;
	int xDiff;
	int yDiff;
	if(mouseToggle)
	{
		xNow = event->motion.x;
		yNow = event->motion.y;
		if(xNow == xMid && yNow == yMid)
		{
			// ignore
		}
		else
		{
			xDiff =  xNow - xLast;
			yDiff =  yNow - yLast;
			if(xDiff || yDiff)
			{
				game->hero->moveEvent(xDiff, yDiff);
#if SDL_VERSION_ATLEAST(2,0,0)
				SDL_WarpMouseInWindow(window, xMid, yMid);
#else
				SDL_WarpMouse(xMid, yMid);
#endif
			}
		}
		xLast = xNow;
		yLast = yNow;
	}
}

//----------------------------------------------------------
void MainSDL::mouseButtonDown(SDL_Event *ev)
{
	Global	*game = Global::getInstance();
	SDL_MouseButtonEvent *mEv = (SDL_MouseButtonEvent*)ev;
	if(game->gameMode == Global::Game)
	{
		switch(mEv->button)
		{
			case SDL_BUTTON_LEFT:
//				game->hero->fireGun(++fire);
				game->hero->fireGun(true);
				break;
			case SDL_BUTTON_MIDDLE:
				game->hero->nextItem();
				break;
			case SDL_BUTTON_RIGHT:
				game->hero->useItem();
				break;
			default:
				break;
		}
	}
	else if(game->gameMode == Global::HeroDead)
	{
		game->menu->keyHit(MainToolkit::KeyEnter);
	}
	
}

//----------------------------------------------------------
void MainSDL::mouseButtonUp(SDL_Event *ev)
{
	Global	*game = Global::getInstance();
	SDL_MouseButtonEvent *mEv = (SDL_MouseButtonEvent*)ev;
	if(game->gameMode == Global::Menu)
	{
		switch(mEv->button)
		{
			case SDL_BUTTON_LEFT:
				game->menu->mousePress(MainToolkit::Left, mEv->x, mEv->y);
				break;
			case SDL_BUTTON_MIDDLE:
				game->menu->mousePress(MainToolkit::Middle, mEv->x, mEv->y);
				break;
			case SDL_BUTTON_RIGHT:
				game->menu->mousePress(MainToolkit::Right, mEv->x, mEv->y);
				break;
			default:
				break;
		}
	}
	else
	{
		switch(mEv->button)
		{
			case  SDL_BUTTON_LEFT:
//				game->hero->fireGun(--fire);
//				game->hero->fireGun(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1));
				game->hero->fireGun(false);
				break;
			default:
				break;
		}
	}
}

//----------------------------------------------------------
void MainSDL::grabMouse(bool status, bool warpmouse)
{
	Config *config = Config::instance();
	if( config->debug() ) fprintf(stderr, _("MainSDL::grabMouse(%d)\n"), status);
	
	mouseToggle = status;
	if(status)
		SDL_ShowCursor(0);
	else
		SDL_ShowCursor(1);
	if(!warpmouse)
		return;
	xMid = config->screenW()/2;
	yMid = config->screenH()/2;
#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_WarpMouseInWindow(window, xMid, yMid);
#else
	SDL_WarpMouse(xMid, yMid);
#endif
	xLast = xMid;
	yLast = yMid;
}

//----------------------------------------------------------
void MainSDL::joystickMotion(SDL_Event *)
{
//	static int c = 0;
//	if( config->debug() ) fprintf(stderr, _("joy %05d : axis(%d), value(%d)\n"), c++, event->jaxis.axis, event->jaxis.value);
//	int xNow;
//	int yNow;
//	int xDiff;
//	int yDiff;
//	if(joystickToggle)
//	{
//		xNow = event->motion.x;
//		yNow = event->motion.y;
//		if(xNow == xMid && yNow == yMid)
//		{
//			// ignore
//		}
//		else
//		{
//			xDiff =  xNow - xLast;
//			yDiff =  yNow - yLast;
//			if(xDiff || yDiff)
//			{
//				game->hero->moveEvent(xDiff, yDiff);
//				SDL_WarpMouse(xMid, yMid);
//			}
//		}
//			xLast = xNow;
//			yLast = yNow;
//	}
}

//----------------------------------------------------------
void MainSDL::joystickButtonDown(SDL_Event *)
{
	Global	*game = Global::getInstance();
	game->hero->fireGun(++fire);
}

//----------------------------------------------------------
void MainSDL::joystickButtonUp(SDL_Event *)
{
	Global	*game = Global::getInstance();
	game->hero->fireGun(--fire);
}

//----------------------------------------------------------
void MainSDL::joystickMove()
{
#ifdef WITH_JOYSTICK
	Global	*game = Global::getInstance();
	static int div = 32768/16;
	if(joystick)
	{
		xjoy = SDL_JoystickGetAxis(joystick, 0)/div;
		yjoy = SDL_JoystickGetAxis(joystick, 1)/div;
		xjNow = 0.8*xjNow + 0.2*xjoy;
		yjNow = 0.8*yjNow + 0.2*yjoy;
		game->hero->moveEvent((int)xjNow, (int)yjNow);
	}
#endif
}

#endif // USE_SDL
