/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

//----------------------------------------------------------
bool MainSDL::process(SDL_Event *event)
{
	Global	*game = Global::getInstance();
#if 0
	static int cnt = 0;
	cnt++;
	switch( event->type ) 
	{
	    case SDL_ACTIVEEVENT:		fprintf(stderr, "%-5d SDL_ACTIVEEVENT  \n"		, cnt); break;
	    case SDL_KEYDOWN:			fprintf(stderr, "%-5d SDL_KEYDOWN  \n"			, cnt); break;
	    case SDL_KEYUP: 			fprintf(stderr, "%-5d SDL_KEYUP  \n"			, cnt); break;
	    case SDL_MOUSEMOTION:		fprintf(stderr, "%-5d SDL_MOUSEMOTION  \n"		, cnt); break;
	    case SDL_MOUSEBUTTONDOWN:	fprintf(stderr, "%-5d SDL_MOUSEBUTTONDOWN  \n"	, cnt); break;
	    case SDL_MOUSEBUTTONUP: 	fprintf(stderr, "%-5d SDL_MOUSEBUTTONUP  \n"	, cnt); break;
		case SDL_JOYBUTTONDOWN: 	fprintf(stderr, "%-5d SDL_JOYBUTTONDOWN  \n"	, cnt); break;
		case SDL_JOYBUTTONUP:		fprintf(stderr, "%-5d SDL_JOYBUTTONUP  \n"		, cnt); break;
		default:
			break;
	}
#endif
	switch(event->type) 
	{
	    case SDL_ACTIVEEVENT:
			activation(event);
			break;
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
		SDL_ActiveEvent 		*evA = 0;
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
		SDL_ActiveEvent 		*evA = (SDL_ActiveEvent*)&ev;
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
				case 'A':
					evA->type	= (Uint8)a;
					evA->gain	= (Uint8)b;
					evA->state	= (Uint8)c;
					retVal = &ev;
					break;
				case 'K':
					evK->type	= (Uint8)a;
					evK->state	= (Uint8)b;
					evK->keysym.scancode = (Uint8)c;
					evK->keysym.sym		 = (SDLKey)d;
					evK->keysym.mod		 = (SDLMod)e;
					evK->keysym.unicode	 = (Uint16)f;
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
					fprintf(stderr, "MainSDL::getEvent HUH?\n");
					break;
			}
			count++;
		}
		bool ok = true;
		if(frame <= game->gameFrame)
			ok = fgets(buffer, 256, infile);
		if(!ok)
		{
			fprintf(stderr, "buffer = \"%s\n\"", buffer);
			exit(1);
		}
	}
	return retVal;
}

//----------------------------------------------------------
void MainSDL::activation(SDL_Event *event)
{
	Global *game = Global::getInstance();
//	fprintf(stderr, "app %s ", event->active.gain ? "gained" : "lost" );
	bool grab_mouse = game->gameMode == Global::Game && event->active.gain ? true : false;
	if ( event->active.state & SDL_APPACTIVE ) 
	{
		grabMouse( grab_mouse, grab_mouse );
//		fprintf(stderr, "app active " );
	} 
	else if ( event->active.state & SDL_APPMOUSEFOCUS ) 
	{
//		fprintf(stderr, "mouse active" );
		SDL_GetMouseState(&xLast, &yLast);
	} 
	else if ( event->active.state & SDL_APPINPUTFOCUS ) 
	{
		grabMouse( grab_mouse, grab_mouse );
//		fprintf(stderr, "input active" );
	}
//	fprintf(stderr, "focus\n" );
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
				grabMouse(true);
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
			else
			{
				MainToolkit::Key key;
				switch(event->key.keysym.sym)
				{
					case SDLK_UP:		key = MainToolkit::KeyUp;		break;
					case SDLK_DOWN:		key = MainToolkit::KeyDown;		break;
					case SDLK_LEFT:		key = MainToolkit::KeyLeft;		break;
					case SDLK_RIGHT:	key = MainToolkit::KeyRight;	break;
					case SDLK_RETURN:	key = MainToolkit::KeyEnter;	break;
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
	switch(event->key.keysym.sym)
	{
//	    case SDLK_RETURN:
//			resetMouseMotion();
//			break;
	    case SDLK_PAUSE:
	    case SDLK_p:
			grabMouse(game->game_pause);
			game->game_pause = !game->game_pause;
			game->audio->pauseGameMusic(game->game_pause);
			break;
	    case SDLK_n:
			game->audio->nextMusicIndex();
			break;
	    case SDLK_LEFT:
			game->hero->moveEvent(-10, 0);
			break;
	    case SDLK_RIGHT:
			game->hero->moveEvent( 10, 0);
			break;
	    case SDLK_UP:
			game->hero->moveEvent(0, -10);
			break;
	    case SDLK_DOWN:
			game->hero->moveEvent(0,  10);
			break;
	    case SDLK_SPACE:
			game->hero->fireGun(true);
			break;
		default:
			fprintf(stderr, "key '%s' pressed\n", SDL_GetKeyName(event->key.keysym.sym));
			fprintf(stderr, "game->gameFrame = %d\n", game->gameFrame);
			break;
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
				SDL_WarpMouse(xMid, yMid);
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
//	fprintf(stderr, "MainSDL::grabMouse(%d)\n", status);
	Config *config = Config::instance();
	
	mouseToggle = status;
	if(status)
		SDL_ShowCursor(0);
	else
		SDL_ShowCursor(1);
	if(!warpmouse)
		return;
	xMid = config->screenW()/2;
	yMid = config->screenH()/2;
	SDL_WarpMouse(xMid, yMid);
	xLast = xMid;
	yLast = yMid;
}

//----------------------------------------------------------
void MainSDL::joystickMotion(SDL_Event *)
{
//	static int c = 0;
//	fprintf(stderr, "joy %05d : axis(%d), value(%d)\n", c++, event->jaxis.axis, event->jaxis.value);
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
