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
	game = Global::getInstance();
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
	reshapeGL(game->screenW, game->screenH);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
		
	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_BLEND);
//	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if(game->blend_enable)
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
	
	pngSetViewingGamma(game->viewGamma);
	
	return 0;
}

//----------------------------------------------------------
void MainGL::loadTextures()
{
	GLuint	texobj;
	game->texFont = txfLoadFont( dataLoc("fonts/space.txf") );
	if(!game->texFont)
	{
		fprintf(stderr, "\nERROR loading texture font. Check data path and try again.\n\n");
		exit(1);
	}
	glGenTextures(1, &texobj);
	txfEstablishTexture(game->texFont, texobj, GL_FALSE);
}

//----------------------------------------------------------
void MainGL::deleteTextures()
{
	glDeleteTextures(1, &game->texFont->texobj);
	txfUnloadFont(game->texFont);
	game->texFont = 0;
}

//----------------------------------------------------------
void MainGL::drawGL()
{
	switch(game->gameMode)
	{
		case game->Game:
			drawGameGL();
			break;
		case game->HeroDead:
			drawDeadGL();
			break;
		case game->LevelOver:
			drawSuccessGL();
			break;
		case game->Menu:
			game->menu->drawGL();
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
	glTranslatef(0.0, 0.0, game->zTrans);
//	glTranslatef(0.0, 5.0, -12.0);
	
	if(!game->game_pause)
	{
		//-- Add items to scene
		game->itemAdd->putScreenItems();
		//addItems();
	
		//-- Update scene
		game->enemyFleet->update();
		game->powerUps->update();
		game->heroAmmo->updateAmmo();
		game->enemyAmmo->updateAmmo();
		game->heroAmmo->checkForHits(game->enemyFleet);
		if(game->gameMode == game->Game)
		{
			game->enemyAmmo->checkForHits(game->hero);
			game->hero->checkForCollisions(game->enemyFleet);
			game->hero->checkForPowerUps(game->powerUps);
		}
		game->explosions->update();
		game->audio->update();
		
		game->hero->update();
		game->gameFrame++;
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//-- Draw background
	game->ground->drawGL();
	
	//-- Draw actors
	game->enemyFleet->drawGL();
	game->hero->drawGL();
	
	if(game->gfxLevel > 0)
		game->statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	game->powerUps->drawGL();
	
	//-- Draw ammo
	game->heroAmmo->drawGL();
	game->enemyAmmo->drawGL();
	
	//-- Draw explosions
	game->explosions->drawGL();
	
	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);
	
}

//----------------------------------------------------------
void MainGL::drawDeadGL()
{
	game->heroDeath--;
	
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	if(game->heroDeath > 0)
	{
		float z = 1.0*game->heroDeath/DEATH_TIME;
		glTranslatef(0.0, 0.0, game->zTrans-z*z);
	}
	else
		glTranslatef(0.0, 0.0, game->zTrans);
	
	//-- Add items to scene
	game->itemAdd->putScreenItems();
	//-- Update scene
	game->explosions->update();
	game->powerUps->update();
	game->enemyFleet->update();
	game->heroAmmo->updateAmmo();
	game->enemyAmmo->updateAmmo();
	game->heroAmmo->checkForHits(game->enemyFleet);
	game->audio->update();
	game->hero->update();
	game->gameFrame++;
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	game->ground->drawGL();
	//-- Draw actors
	game->enemyFleet->drawGL();
	
	if(game->gfxLevel > 0)
		game->statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	game->powerUps->drawGL();
	//-- Draw ammo
	game->heroAmmo->drawGL();
	game->enemyAmmo->drawGL();
	//-- Draw explosions
	game->explosions->drawGL();
	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);
	
	char buffer[128];
	if(game->checkHiScore() == 1)
	{
		sprintf(buffer, "new high score!\n\n%d", (int)game->hero->getScore());
		drawTextGL(buffer, game->heroDeath, 0.15);
	}
	else if(game->checkHiScore() > 1)
	{
		sprintf(buffer, "n o t   b a d !\nrank : %d\n\n%d", game->checkHiScore(), (int)game->hero->getScore());
		drawTextGL(buffer, game->heroDeath, 0.15);	
	}
	else
	{
		drawTextGL("l o s e r", game->heroDeath, 0.25);
	}
}

//----------------------------------------------------------
void MainGL::drawSuccessGL()
{
	game->heroSuccess--;
	
	if(game->heroSuccess < -500)
	{
		game->gotoNextLevel();
		game->gameMode = game->Game;
		game->audio->setMusicMode(Audio::MusicGame);
		game->audio->setMusicVolume(game->volMusic);
		return;
	}
	
	float f	= -game->heroSuccess/450.0;
	if(game->heroSuccess < 0)
	{
		float vol = game->volMusic - (game->volMusic*f);
		game->audio->setMusicVolume(vol);
	}
	
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, game->zTrans);
	
	//-- Update scene
	game->enemyFleet->update();
	game->explosions->update();
	game->heroAmmo->updateAmmo();
	game->hero->update();
	game->audio->update();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//-- Draw background
	game->ground->drawGL();
	//-- Draw actors
	game->hero->drawGL();
	
	if(game->gfxLevel > 0)
		game->statusDisplay->darkenGL();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//-- Draw ammo
	game->heroAmmo->drawGL();
	//-- Draw explosions
	game->explosions->drawGL();
	//-- Draw stats
	game->statusDisplay->drawGL(game->hero);
		
	char	buffer[256];
	sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n", game->gameLevel);
//	if(game->hero->getScore() > game->hiScore[INT_GAME_SKILL_BASE][0])
//	{
//		sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n", game->gameLevel);
//	}
//	else
//	{
//		sprintf(buffer, "congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \nn e w   h i g h   s c o r e : \n %g \n", game->gameLevel, game->hero->getScore());
//	}
	
	drawTextGL(buffer, game->heroSuccess, 0.15);
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

	height = 1.5*txfStringHeight(game->texFont);
	
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
				x_sin = 1.75*sin(i+game->frame*0.06);
				y_sin = 0.75*sin(i+game->frame*0.09);

				glPushMatrix();
				glScalef(scale, scale*0.75, 1.0);
				width = txfStringLength(game->texFont, index[l], strlen(index[l]));
				glTranslatef(-(width/2.0)-x_sin, y+y_sin, 0.0);
				txfBindFontTexture(game->texFont);
				txfRenderString(game->texFont, index[l], strlen(index[l]));
				glPopMatrix();

			}
		}
	}	
}

//----------------------------------------------------------
void MainGL::reshapeGL(int w, int h)
{
	game->screenW = w;
	game->screenH = h;
	game->screenA = (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( game->screenFOV, 
					game->screenA, 
					game->screenNear, 
					game->screenFar);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, game->screenW, game->screenH);
}
