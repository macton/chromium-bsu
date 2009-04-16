/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef MainGLUT_h
#define MainGLUT_h

#ifdef USE_GLUT

#include "MainToolkit.h"
#include "MainGL.h"

//====================================================================
class MainGLUT : public MainToolkit
{
public:
	MainGLUT(int argc = 0, char **argv = 0);
	~MainGLUT();

	bool run();
	bool checkErrors();
	
	bool setVideoMode();
	
	void grabMouse(bool,bool=true);

private:
	static void nullFunc();
	static void drawGame(int);
	static void updateGame();
	static void reshape(int w, int h);
	static void keyboardASCII(unsigned char key, int x, int y);
	static void keyboardSpecial(int key, int x, int y);
	static void mouseEvent(int button, int state, int x, int y);	
	static void mouseMotion(int x, int y);	

private:
	static int		fire;
	static int		xLast, yLast;
	static int		xMid, yMid;
	static bool		mouseToggle;
	
	static int last_time;
	static int now_time;
	
};


#endif //USE_GLUT

#endif // MainGLUT_h
