/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef MenuGL_h
#define MenuGL_h

#include "compatibility.h"
#include "GL/gl.h"
#include "TexFont.h"
#include "MainToolkit.h"

class Global;

//====================================================================
class MenuGL
{
public:
	enum MenuSelection {	NewGame, GameLevel, SkillLevel, Graphics, 
							FullScreen, ScreenSize, Sound, 
							Music, MouseSpeed, Quit, 
							NumSelections };
	
	MenuGL();
	~MenuGL();
	
	void	startMenu();
	void	keyHit(MainToolkit::Key key);
	void	mousePress(MainToolkit::Button but, int x, int y);

	void	drawGL();
	
	void	loadTextures();
	void	deleteTextures();
	
protected:
	void	activateItem();
	void	incItem();
	void	decItem();
	
	void	drawIndicator();
	void	drawElectric();
	
	void	drawTitle();
	void	drawTitleBack();
	
	void	createLists(bool thick);
private:
	MenuSelection curSel;
	
	char *menuText[NumSelections];
	
	GLuint	listChrom;
	GLuint	listBSU;
	
	GLuint	envTex;
	GLuint	csrTex;
	GLuint	backTex;
	GLuint	elecTex;
	GLuint	updwnTex;
	
	float	elecOffX;
	float	elecOffY;
	float	elecStretch;
	float	textAngle;
	int		textCount;
	
	float	txtHeight;

	bool	thickText;
	
	float 	titleTilt;
	
	float	mssgAlpha;
	char	mssgText[256];
	int		mssgIndex;
	int		mssgCount;
	bool	mssgHelpOverride;

private:
	Global	*game;
};

#endif //MenuGL_h
