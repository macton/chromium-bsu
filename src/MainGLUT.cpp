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

#include "gettext.h"

#include "MainGLUT.h"

#ifdef USE_GLUT

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <GL/glut.h>

#include "Config.h"
#include "HiScore.h"
#include "Global.h"
#include "Audio.h"
#include "MenuGL.h"
#include "HeroAircraft.h"


int		MainGLUT::fire	= 0;
int		MainGLUT::xLast	= 0;
int		MainGLUT::yLast	= 0;
int		MainGLUT::xMid	= 0;
int		MainGLUT::yMid	= 0;
bool	MainGLUT::mouseToggle = false;
int		MainGLUT::last_time = 0;
int		MainGLUT::now_time = 0;

//====================================================================
MainGLUT::MainGLUT(int argc, char **argv)
	: MainToolkit(argc, argv)
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(config->screenW(), config->screenH());
	glutCreateWindow("Chromium B.S.U.");
	
	//-- Initialize OpenGL
	game->createGame();
	
	glutDisplayFunc(MainGLUT::nullFunc);
	glutReshapeFunc(MainGLUT::reshape);	
	glutKeyboardFunc(MainGLUT::keyboardASCII);	
	glutSpecialFunc(MainGLUT::keyboardSpecial);	
	glutMouseFunc(MainGLUT::mouseEvent);
//	glutMotionFunc(MainGLUT::mouseMotion);

}

MainGLUT::~MainGLUT()
{
}

//----------------------------------------------------------
bool MainGLUT::run()
{
	fprintf(stderr, _("MainGLUT::run()\n"));
	glutTimerFunc(100, MainGLUT::drawGame, 0);
	glutMainLoop();
	return true;
}

//----------------------------------------------------------
bool MainGLUT::checkErrors()
{
	bool retVal = false;
	GLenum	gl_error;
	
	//-- Check for GL errors
	gl_error = glGetError( );
	if( gl_error != GL_NO_ERROR ) 
	{
		fprintf(stderr, _("ERROR!!! OpenGL error: %s\n"), gluErrorString(gl_error) );
		retVal = true;
	}

	return retVal;
}

//----------------------------------------------------------
void MainGLUT::grabMouse(bool status, bool warpmouse)
{
	Config	*config = Config::instance();
	mouseToggle = status;
	if(status)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		glutMotionFunc(MainGLUT::mouseMotion);
		glutPassiveMotionFunc(MainGLUT::mouseMotion);
		if(!warpmouse)
			return;
		xMid = config->screenW()/2;
		yMid = config->screenH()/2;
		glutWarpPointer(xMid, yMid);
		xLast = xMid;
		yLast = yMid;
	}
	else
	{
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		glutMotionFunc(0);
		glutPassiveMotionFunc(0);
	}
}

//----------------------------------------------------------
void MainGLUT::setVideoMode()
{
	Config	*config = Config::instance();
	config->setScreenSize(config->screenSize()); //  set screenW & screenH for new screenSize
	glutReshapeWindow(config->screenW(), config->screenH());
	config->setFullScreen(false);
}

//----------------------------------------------------------
void MainGLUT::reshape(int w, int h)
{
	Global	*game = Global::getInstance();
	game->mainGL->reshapeGL(w, h);
}

//----------------------------------------------------------
void MainGLUT::nullFunc()
{
	Global	*game = Global::getInstance();
	game->mainGL->drawGL();
	glutSwapBuffers();
	
	game->frame++;
	if( !(game->gameFrame%10) )
	{
		now_time = glutGet(GLUT_ELAPSED_TIME);
		if(last_time)
		{
			game->fps = (10.0/(now_time-last_time))*1000.0;
		}
		last_time = now_time;
	}
	
}

//----------------------------------------------------------
void MainGLUT::drawGame(int)
{
	Global	*game = Global::getInstance();
	glutTimerFunc(33, MainGLUT::drawGame, 0);

	game->mainGL->drawGL();
	#ifdef CHECK_ERRORS
	game->toolkit->checkErrors();
	#endif// CHECK_ERRORS
	
	glutPostRedisplay();
}

//----------------------------------------------------------
void MainGLUT::keyboardASCII(unsigned char key, int, int)
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	HiScore	*hiScore = HiScore::getInstance();
	switch(key)
	{
		case 'q':
		case 'Q':
			config->saveFile();
			hiScore->saveFile();
			game->deleteGame();
			exit(0);
			break;
		case 'G':
		case 'g':
			game->toolkit->grabMouse(!mouseToggle);
			break;
		case 27: // <esc>
			if(game->gameMode == Global::Menu)
			{
				game->gameMode = Global::Game;
				game->audio->setMusicMode(Audio::MusicGame);
				game->toolkit->grabMouse(true);
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
				game->toolkit->grabMouse(false);
			}
			break;
		default:
			if(game->gameMode == Global::Menu)
			{
				MainToolkit::Key tkkey = MainToolkit::KeySpace;
				switch(key)
				{
					// up, down, etc. have to be handled with the 'special' func. Annoying.
					case 13: // <enter>
						tkkey = MainToolkit::KeyEnter;	
						break;
					default:	break;
				}
				game->menu->keyHit(tkkey);
			}
			break;
		
	}
	// Emulate a quit key
	if( game->game_quit )
		keyboardASCII('q',0,0);
//	fprintf(stderr, _("key = %d\n"), (int)key);
}

//----------------------------------------------------------
void MainGLUT::keyboardSpecial(int special, int, int)
{
	Global	*game = Global::getInstance();
	if(game->gameMode == Global::Menu)
	{
		MainToolkit::Key tkkey;
		switch(special)
		{
			case GLUT_KEY_UP:		tkkey = MainToolkit::KeyUp;		break;
			case GLUT_KEY_DOWN:		tkkey = MainToolkit::KeyDown;	break;
			case GLUT_KEY_LEFT:		tkkey = MainToolkit::KeyLeft;	break;
			case GLUT_KEY_RIGHT:	tkkey = MainToolkit::KeyRight;	break;
			default: break;
		}
		game->menu->keyHit(tkkey);		
	}
}

//----------------------------------------------------------
void MainGLUT::mouseEvent(int button, int state, int x, int y)
{
	Global	*game = Global::getInstance();
	if(state == GLUT_DOWN)
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				game->hero->fireGun(++fire);
				break;
			case GLUT_MIDDLE_BUTTON:
				game->hero->nextItem();
				break;
			case GLUT_RIGHT_BUTTON:
				game->hero->useItem();
				break;
		}
	}
	else if(state == GLUT_UP)
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				game->hero->fireGun(--fire);
				break;
			case GLUT_MIDDLE_BUTTON:
				break;
			case GLUT_RIGHT_BUTTON:
				break;
		}
	}
		
	xLast = x;
	yLast = y;
}

//----------------------------------------------------------
void MainGLUT::mouseMotion(int x, int y)
{
	Global	*game = Global::getInstance();
	int xNow;
	int yNow;
	int xDiff;
	int yDiff;
	
	if(mouseToggle)
	{
		xNow = x;
		yNow = y;
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
				glutWarpPointer(xMid, yMid);
			}
		}
		xLast = xNow;
		yLast = yNow;
	}
}


#endif // USE_GLUT
