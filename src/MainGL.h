/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef MainGL_h
#define MainGL_h

class Global;

//====================================================================
class MainGL
{
public:
	MainGL();
	~MainGL();
	
	int		initGL();
	void	drawGL();
	void	drawGameGL();
	void	drawDeadGL();
	void	drawSuccessGL();
	void	drawTextGL(char *string, float pulse, float scale);
	void	reshapeGL( int w, int h );
	
	void	loadTextures();
	void	deleteTextures();

private:
	Global	*game;
};

#endif // mainGL_h
