/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MainGLUT.h"

#ifdef USE_GLUT

#include <stdio.h>
#include <unistd.h>

#include <GL/glut.h>

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
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(Global::screenW, Global::screenH);
	glutCreateWindow("Chromium B.S.U.");
	
	//-- Initialize OpenGL
	Global::createGame();
	
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
	fprintf(stderr, "MainGLUT::run()\n");
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
		fprintf(stderr, "ERROR!!! OpenGL error: %s\n", gluErrorString(gl_error) );
		retVal = true;
	}

	return retVal;
}

//----------------------------------------------------------
void MainGLUT::grabMouse(bool status)
{
	mouseToggle = status;
	if(status)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		glutMotionFunc(MainGLUT::mouseMotion);
		glutPassiveMotionFunc(MainGLUT::mouseMotion);
		xMid = Global::screenW/2;
		yMid = Global::screenH/2;
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
	Global::setScreenSize(Global::screenSize); //  set screenW & screenH for new screenSize
	glutReshapeWindow(Global::screenW, Global::screenH);
	Global::full_screen = false;
}

//----------------------------------------------------------
void MainGLUT::reshape(int w, int h)
{
	Global::mainGL->reshapeGL(w, h);
}

//----------------------------------------------------------
void MainGLUT::nullFunc()
{
	Global::mainGL->drawGL();
	glutSwapBuffers();
	
	Global::frame++;
	if( !(Global::gameFrame%10) )
	{
		now_time = glutGet(GLUT_ELAPSED_TIME);
		if(last_time)
		{
			Global::fps = (10.0/(now_time-last_time))*1000.0;
		}
		last_time = now_time;
	}
	
}

//----------------------------------------------------------
void MainGLUT::drawGame(int)
{
	glutTimerFunc(33, MainGLUT::drawGame, 0);

	Global::mainGL->drawGL();
	#ifdef CHECK_ERRORS
	Global::toolkit->checkErrors();
	#endif// CHECK_ERRORS
	
	glutPostRedisplay();
}

//----------------------------------------------------------
void MainGLUT::keyboardASCII(unsigned char key, int, int)
{
	switch(key)
	{
		case 'q':
		case 'Q':
			Global::saveConfigFile();
			Global::deleteGame();
			exit(0);
			break;
		case 'G':
		case 'g':
			Global::toolkit->grabMouse(!mouseToggle);
			break;
		case 27: // <esc>
			if(Global::gameMode == Global::Menu)
			{
				Global::gameMode = Global::Game;
				Global::audio->setMusicMode(Audio::MusicGame);
				Global::toolkit->grabMouse(true);
			}
			else
			{
				if(Global::gameMode != Global::Game)
				{
					Global::newGame();
				}
				Global::gameMode = Global::Menu;
				Global::menu->startMenu();
				Global::audio->setMusicMode(Audio::MusicMenu);
				Global::toolkit->grabMouse(false);
			}
			break;
		default:
			if(Global::gameMode == Global::Menu)
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
				Global::menu->keyHit(tkkey);
			}
			break;
		
	}
//	fprintf(stderr, "key = %d\n", (int)key);
}

//----------------------------------------------------------
void MainGLUT::keyboardSpecial(int special, int, int)
{
	if(Global::gameMode == Global::Menu)
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
		Global::menu->keyHit(tkkey);		
	}
}

//----------------------------------------------------------
void MainGLUT::mouseEvent(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				Global::hero->fireGun(++fire);
				break;
			case GLUT_MIDDLE_BUTTON:
				Global::hero->nextItem();
				break;
			case GLUT_RIGHT_BUTTON:
				Global::hero->useItem();
				break;
		}
	}
	else if(state == GLUT_UP)
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				Global::hero->fireGun(--fire);
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
				Global::hero->moveEvent(xDiff, yDiff);
				glutWarpPointer(xMid, yMid);
			}
		}
		xLast = xNow;
		yLast = yNow;
	}
}


#endif // USE_GLUT
