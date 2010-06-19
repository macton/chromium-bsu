/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef MainSDL_h
#define MainSDL_h

#ifdef USE_SDL
#include <SDL/SDL.h>

#include "MainToolkit.h"

class Global;

//====================================================================
class MainSDL : public MainToolkit
{
public:
	MainSDL(int argc = 0, char **argv = 0);
	~MainSDL();

	bool run();
	bool checkErrors();
	
	bool setVideoMode();
	
	void grabMouse(bool,bool=true);

	bool process(SDL_Event *event);
	void joystickMove();
	
private:
	void activation(SDL_Event *event);
	void keyDown(SDL_Event *event);
	void keyDownGame(SDL_Event *event);
	void keyUp(SDL_Event *event);
	void mouseMotion(SDL_Event *event);
	void mouseButtonDown(SDL_Event *event);
	void mouseButtonUp(SDL_Event *event);
	void joystickMotion(SDL_Event *event);
	void joystickButtonDown(SDL_Event *event);
	void joystickButtonUp(SDL_Event *event);

	void		saveEvent(SDL_Event *event);
	SDL_Event	*getEvent(FILE *infile);
	
private:
	SDL_Joystick	*joystick;
	int		fire;
	bool	mouseToggle;
	int		xLast, yLast;
	int		xMid, yMid;
	float	xjoy,yjoy;
	float	xjNow,yjNow;
	
	int	adjCount;
};

#endif // USE_SDL

#endif // MainSDL_h
