/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MainGL.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glpng.h>

#include "extern.h"
#include "TexFont.h"
#include "Global.h"
#include "EnemyFleet.h"
#include "HeroAmmo.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "Explosions.h"
#include "PowerUps.h"
#include "Audio.h"
#include "MenuGL.h"
#include "StatusDisplay.h"
#include "ScreenItemAdd.h"

#include "GroundMetal.h"
#include "GroundSea.h"

//====================================================================
MainGL::MainGL()
{
	initGL();
	loadTextures();
}


MainGL::~MainGL()
{
}

//----------------------------------------------------------
int MainGL::initGL()
{
//	fprintf(stderr, "initGL()\n");
	reshapeGL(Global::screenW, Global::screenH);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
		
	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
//	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if(Global::blend_enable)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
	else
	{
		glAlphaFunc(GL_GREATER, 0.2);

		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
	}
	
//	glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
	
	glPointSize(1.0);
	glLineWidth(1.0);
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	
	pngSetViewingGamma(Global::viewGamma);
	
	return 0;
}

//----------------------------------------------------------
void MainGL::loadTextures()
{
	GLuint	texobj;
	Global::texFont = txfLoadFont( dataLoc("fonts/space.txf") );
	if(!Global::texFont)
	{
		fprintf(stderr, "\nERROR loading texture font. Check data path and try again.\n\n");
		exit(1);
	}
	glGenTextures(1, &texobj);
	txfEstablishTexture(Global::texFont, texobj, GL_FALSE);
}

//----------------------------------------------------------
void MainGL::deleteTextures()
{
	glDeleteTextures(1, &Global::texFont->texobj);
	txfUnloadFont(Global::texFont);
	Global::texFont = 0;
}

//----------------------------------------------------------
void MainGL::drawGL()
{
	switch(Global::gameMode)
	{
		case Global::Game:
			drawGameGL();
			break;
		case Global::HeroDead:
			drawDeadGL();
			break;
		case Global::LevelOver:
			drawSuccessGL();
			break;
		case Global::Menu:
			Global::menu->drawGL();
			break;
		default:
			fprintf(stderr, "!!MainGL::drawGL() HUH?\n");
			break;
	}
}

//----------------------------------------------------------
void MainGL::drawGameGL()
{
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear( GL_COLOR_BUFFER_BIT );

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, Global::zTrans);
//	glTranslatef(0.0, 5.0, -12.0);
	
	if(!Global::game_pause)
	{
		//-- Add items to scene
		Global::itemAdd->putScreenItems();
		//addItems();
	
		//-- Update scene
		Global::enemyFleet->update();
		Global::powerUps->update();
		Global::heroAmmo->updateAmmo();
		Global::enemyAmmo->updateAmmo();
		Global::heroAmmo->checkForHits(Global::enemyFleet);
		if(Global::gameMode == Global::Game)
		{
			Global::enemyAmmo->checkForHits(Global::hero);
			Global::hero->checkForCollisions(Global::enemyFleet);
			Global::hero->checkForPowerUps(Global::powerUps);
		}
		Global::explosions->update();
		Global::audio->update();
		
		Global::hero->update();
		Global::gameFrame++;
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//-- Draw background
	Global::ground->drawGL();
	
	//-- Draw actors
	Global::enemyFleet->drawGL();
	Global::hero->drawGL();
	
	if(Global::gfxLevel > 0)
		Global::statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	Global::powerUps->drawGL();
	
	//-- Draw ammo
	Global::heroAmmo->drawGL();
	Global::enemyAmmo->drawGL();
	
	//-- Draw explosions
	Global::explosions->drawGL();
	
	//-- Draw stats
	Global::statusDisplay->drawGL(Global::hero);
	
}

//----------------------------------------------------------
void MainGL::drawDeadGL()
{
	Global::heroDeath--;
	
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	if(Global::heroDeath > 0)
	{
		float z = 1.0*Global::heroDeath/DEATH_TIME;
		glTranslatef(0.0, 0.0, Global::zTrans-z*z);
	}
	else
		glTranslatef(0.0, 0.0, Global::zTrans);
	
	//-- Add items to scene
	Global::itemAdd->putScreenItems();
	//-- Update scene
	Global::explosions->update();
	Global::powerUps->update();
	Global::enemyFleet->update();
	Global::heroAmmo->updateAmmo();
	Global::enemyAmmo->updateAmmo();
	Global::heroAmmo->checkForHits(Global::enemyFleet);
	Global::audio->update();
	Global::hero->update();
	Global::gameFrame++;
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	Global::ground->drawGL();
	//-- Draw actors
	Global::enemyFleet->drawGL();
	
	if(Global::gfxLevel > 0)
		Global::statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	Global::powerUps->drawGL();
	//-- Draw ammo
	Global::heroAmmo->drawGL();
	Global::enemyAmmo->drawGL();
	//-- Draw explosions
	Global::explosions->drawGL();
	//-- Draw stats
	Global::statusDisplay->drawGL(Global::hero);
	
	char buffer[128];
	if(Global::checkHiScore() == 1)
	{
		sprintf(buffer, "new high score!\n\n%d", (int)Global::hero->getScore());
		drawTextGL(buffer, Global::heroDeath, 0.15);
	}
	else if(Global::checkHiScore() > 1)
	{
		sprintf(buffer, "n o t   b a d !\nrank : %d\n\n%d", Global::checkHiScore(), (int)Global::hero->getScore());
		drawTextGL(buffer, Global::heroDeath, 0.15);	
	}
	else
	{
		drawTextGL("l o s e r", Global::heroDeath, 0.25);
	}
}

//----------------------------------------------------------
void MainGL::drawSuccessGL()
{
	Global::heroSuccess--;
	
	if(Global::heroSuccess < -500)
	{
		Global::gotoNextLevel();
		Global::gameMode = Global::Game;
		Global::audio->setMusicMode(Audio::MusicGame);
		Global::audio->setMusicVolume(Global::volMusic);
		return;
	}
	
	float f	= -Global::heroSuccess/450.0;
	if(Global::heroSuccess < 0)
	{
		float vol = Global::volMusic - (Global::volMusic*f);
		Global::audio->setMusicVolume(vol);
	}
	
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, Global::zTrans);
	
	//-- Update scene
	Global::enemyFleet->update();
	Global::explosions->update();
	Global::heroAmmo->updateAmmo();
	Global::hero->update();
	Global::audio->update();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	Global::ground->drawGL();
	//-- Draw actors
	Global::hero->drawGL();
	
	if(Global::gfxLevel > 0)
		Global::statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//-- Draw ammo
	Global::heroAmmo->drawGL();
	//-- Draw explosions
	Global::explosions->drawGL();
	//-- Draw stats
	Global::statusDisplay->drawGL(Global::hero);
		
	char	buffer[256];
	sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n", Global::gameLevel);
//	if(Global::hero->getScore() > Global::hiScore[INT_GAME_SKILL_BASE][0])
//	{
//		sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n", Global::gameLevel);
//	}
//	else
//	{
//		sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \nn e w   h i g h   s c o r e : \n %g \n", Global::gameLevel, Global::hero->getScore());
//	}
	
	drawTextGL(buffer, Global::heroSuccess, 0.15);
}

//----------------------------------------------------------
void MainGL::drawTextGL(char *string, float pulse, float scale)
{
	int i, l, lines = 1;
	float	aa, ca;
	float	clr;
	float	x_sin, y_sin, y, min_y;
	float	width, height;
	char	*walker,*newline;
	char	*index[10] = { 0,0,0,0,0,0,0,0,0,0 };
	char	buffer[128];

	//-- alpha
	float tmp = 0.5+0.5*(sin(pulse*0.02));
	aa = 0.7-0.5*tmp;
	if(pulse > -50.0)
		aa *= (-pulse/50.0);
	ca = 1.0-tmp;

	height = 1.5*txfStringHeight(Global::texFont);
	
	strncpy(buffer, string, 128);
	index[0] = buffer;
	walker   = buffer;
	while( (newline = strchr(walker, '\n')) != NULL )
	{
		walker = newline+1;
		index[lines] = newline+1;
		*newline = '\0';
		lines++;
	}
	
	min_y = 0.5*height*lines;
	for(l = 0; l< lines; l++)
	{
		y = min_y-height*(l+1);
		if(index[l] && strlen(index[l]) > 0)
		{
			for(i = 0; i < 6; i++)
			{
				clr = ((float)i)/5.0;
				clr = clr;
				glColor4f(1.0, ca*ca*0.3, ca*0.3, aa*aa);
				x_sin = 1.75*sin(i+Global::frame*0.06);
				y_sin = 0.75*sin(i+Global::frame*0.09);

				glPushMatrix();
				glScalef(scale, scale*0.75, 1.0);
				width = txfStringLength(Global::texFont, index[l], strlen(index[l]));
				glTranslatef(-(width/2.0)-x_sin, y+y_sin, 0.0);
				txfBindFontTexture(Global::texFont);
				txfRenderString(Global::texFont, index[l], strlen(index[l]));
				glPopMatrix();

			}
		}
	}	
}

//----------------------------------------------------------
void MainGL::reshapeGL(int w, int h)
{
//	fprintf(stderr, "reshape %d x %d\n", w, h);
	Global::screenW = w;
	Global::screenH = h;
	Global::screenA = (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( Global::screenFOV, Global::screenA, Global::screenNear, Global::screenFar);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, Global::screenW, Global::screenH);
}
