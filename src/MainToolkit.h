/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef MainToolkit_h
#define MainToolkit_h

class Global;

//====================================================================
class MainToolkit
{
public:
	MainToolkit(int argc = 0, char **argv = 0);
	virtual ~MainToolkit();
	
	enum Key { KeyEnter, KeySpace, KeyTab, KeyEscape, 
				KeyUp, KeyDown, KeyLeft, KeyRight };

	virtual bool run() = 0;
	virtual bool checkErrors() = 0;
	
	virtual void setVideoMode() = 0;
	
	virtual void grabMouse(bool) = 0;

protected:
	Global *game;
};




#endif // MainToolkit_h



