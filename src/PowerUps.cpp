/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "PowerUps.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glpng.h>

#include "Config.h"

#include "extern.h"
#include "Global.h"
#include "Explosions.h"
#include "EnemyFleet.h"
#include "EnemyAircraft.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"


//====================================================================
PowerUps::PowerUps()
{
	game = Global::getInstance();
	int i;
	
	activeCount = 0;
	currentPwrUp = 0;
	
	for(i = 0; i < NumPowerUps; i++)
	{		
		pwrUpSize[i][0] = 0.6;
		pwrUpSize[i][1] = 0.6;
		tex[i] = 0;
	}
	
	speed = game->scrollSpeed * 0.8;
	
	loadTextures();
	
	pwrUpColor[Shields][0] = 0.5;
	pwrUpColor[Shields][1] = 0.7;
	pwrUpColor[Shields][2] = 1.0;
	pwrUpColor[Shields][3] = 1.0;
	
	pwrUpColor[SuperShields][0] = 1.0;
	pwrUpColor[SuperShields][1] = 0.6;
	pwrUpColor[SuperShields][2] = 0.0;
	pwrUpColor[SuperShields][3] = 1.0;
	
	pwrUpColor[Repair][0] = 1.0;
	pwrUpColor[Repair][1] = 0.1;
	pwrUpColor[Repair][2] = 0.0;
	pwrUpColor[Repair][3] = 1.0;
	
	pwrUpColor[HeroAmmo00][0] = 1.0;
	pwrUpColor[HeroAmmo00][1] = 0.8;
	pwrUpColor[HeroAmmo00][2] = 0.5;
	pwrUpColor[HeroAmmo00][3] = 0.8;
	
	pwrUpColor[HeroAmmo01][0] = 0.0;
	pwrUpColor[HeroAmmo01][1] = 1.0;
	pwrUpColor[HeroAmmo01][2] = 0.5;
	pwrUpColor[HeroAmmo01][3] = 0.8;
	
	pwrUpColor[HeroAmmo02][0] = 0.4;
	pwrUpColor[HeroAmmo02][1] = 0.2;
	pwrUpColor[HeroAmmo02][2] = 1.0;
	pwrUpColor[HeroAmmo02][3] = 1.0;
	
	float p[3] = { 0.0, 0.0, 0.0 };
	pwrUpRoot = new PowerUp((PowerUps::Type)-1, p);
	
	double twoPi = 2.0*M_PI;
	for(i = 0; i < WOBBLE_0; i++)
		wobble_0[i] = 0.1*sin(twoPi * ((double)i/(double)WOBBLE_0) );
	for(i = 0; i < WOBBLE_1; i++)
		wobble_1[i] = 0.3*sin(twoPi * ((double)i/(double)WOBBLE_1) );
	
}	

PowerUps::~PowerUps()
{
	deleteTextures();
	clear();
	delete pwrUpRoot;
}

//----------------------------------------------------------
void	PowerUps::loadTextures()
{
	pngInfo tmpInfo;
	pwrTex			= pngBind(dataLoc("png/powerUpTex.png"),	PNG_NOMIPMAPS, PNG_BLEND3, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	tex[Shields]	= pngBind(dataLoc("png/powerUpShield.png"),	PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	tex[SuperShields] = tex[Shields];
	tex[Repair]		= tex[Shields];
	tex[HeroAmmo00]	= pngBind(dataLoc("png/powerUpAmmo.png"),	PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	tex[HeroAmmo01]	= tex[HeroAmmo00];
	tex[HeroAmmo02]	= tex[HeroAmmo00];
}

//----------------------------------------------------------
void	PowerUps::deleteTextures()
{
	for(int i = 0; i < NumPowerUps; i++)
	{
		glDeleteTextures(1, &tex[i]);	
	}
	glDeleteTextures(1, &pwrTex);	
}

//----------------------------------------------------------
void	PowerUps::clear()
{
	PowerUp	*cur;
	PowerUp *del;
	
	currentPwrUp = 0;	
	cur = pwrUpRoot->next;
	while(cur)
	{
//		fprintf(stderr, "cur = %p\n", cur);
		del = cur;
		cur = cur->next;
		game->itemAdd->killScreenItem(del);
//		delete del;
	}
	pwrUpRoot->next = 0;
}

//----------------------------------------------------------
PowerUp	*PowerUps::getFirst()
{	
	currentPwrUp = pwrUpRoot->next;
	return 	currentPwrUp;
}

//----------------------------------------------------------
PowerUp	*PowerUps::getNext()	
{	
	PowerUp *retVal = 0;
	if(currentPwrUp)
	{
		currentPwrUp = currentPwrUp->next;
		retVal = currentPwrUp;
	}
	return retVal;
}

//----------------------------------------------------------
void	PowerUps::remove(PowerUp *pwr)
{
	if(pwr)
	{
		currentPwrUp = pwr->back;
		pwr->seal();
	}
	game->itemAdd->killScreenItem(pwr);
	activeCount--;
	fprintf(stderr, "active power ups = %d\n", activeCount);
//	delete pwr;
}

//----------------------------------------------------------
void PowerUps::addPowerUp(PowerUp *pwrUp)
{
	PowerUp *first = 0;
	
	activeCount++;
	pwrUp->back = pwrUpRoot;
	pwrUp->next = pwrUpRoot->next;
	if(pwrUpRoot->next)
	{
		first = pwrUpRoot->next;
		first->back = pwrUp;
	}
	pwrUpRoot->next = pwrUp;
}

//----------------------------------------------------------
void PowerUps::update()
{
	Config	*config = Config::instance();

	PowerUp	*pwrUp;
	PowerUp *delUp;
	pwrUp = pwrUpRoot->next;
	while( pwrUp  )
	{
		pwrUp->age++;
		pwrUp->pos[1] += (speed*game->speedAdj);
		if(pwrUp->vel[0] || pwrUp->vel[1])
		{
			float s = (1.0-game->speedAdj)+(game->speedAdj*0.982);
			pwrUp->vel[0] *= s;
			pwrUp->vel[1] *= s;
			pwrUp->pos[0] += pwrUp->vel[0];
			pwrUp->pos[1] += pwrUp->vel[1];
			if(pwrUp->vel[0] < 0.01) pwrUp->vel[0] = 0.0;
			if(pwrUp->vel[1] < 0.01) pwrUp->vel[1] = 0.0;
		}
		float b = config->screenBoundX()-1.0;
		if(pwrUp->pos[0] < -b)
			pwrUp->pos[0] = -b;
		if(pwrUp->pos[0] >  b)
			pwrUp->pos[0] =  b;
			
		if(pwrUp->pos[1] < -12)
		{
			if(game->gameMode == Global::Game)
				switch(pwrUp->type)
				{
					case PowerUps::SuperShields:
						game->hero->addLife();
						game->hero->addScore(2500.0);
						break;
					case PowerUps::Shields:
					case PowerUps::Repair:
						game->hero->addScore(10000.0);
						break;
					default:
						game->hero->addScore(2500.0);
						break;
				}
			delUp = pwrUp;
			pwrUp = pwrUp->next;
			remove(delUp);
		}
		else
		{
			pwrUp = pwrUp->next;
		}
	}
}

//----------------------------------------------------------
void PowerUps::drawGL()
{
	float	*pos, *sz, szp;
	PowerUp	*pwrUp;
	
	pwrUp = pwrUpRoot->next;
	while(pwrUp)
	{
		pos	= pwrUp->pos;
		sz	= pwrUpSize[pwrUp->type];
		szp = sz[0]*2.5;
		
		glColor4fv(pwrUpColor[pwrUp->type]);
		glBindTexture(GL_TEXTURE_2D, pwrTex);
		glPushMatrix();
		glTranslatef(	pos[0]+wobble_0[pwrUp->age%WOBBLE_0], 
						pos[1]+wobble_1[pwrUp->age%WOBBLE_1], 
						pos[2]);
		glRotatef(IRAND, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-szp,  szp, 0.0 );
		glTexCoord2f(0.0, 1.0); glVertex3f(-szp, -szp, 0.0 );
		glTexCoord2f(1.0, 1.0); glVertex3f( szp, -szp, 0.0 );
		glTexCoord2f(1.0, 0.0); glVertex3f( szp,  szp, 0.0 );
		glEnd();
		glPopMatrix();

		pwrUp = pwrUp->next; //ADVANCE
	}
		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	pwrUp = pwrUpRoot->next;
	while(pwrUp)
	{
		pos	= pwrUp->pos;
		sz	= pwrUpSize[pwrUp->type];
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, tex[pwrUp->type]);
		glPushMatrix();
		glTranslatef(	pos[0]+wobble_0[pwrUp->age%WOBBLE_0], 
						pos[1]+wobble_1[pwrUp->age%WOBBLE_1], 
						pos[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-sz[0],  sz[1], 0.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-sz[0], -sz[1], 0.0);
		glTexCoord2f(1.0, 1.0); glVertex3f( sz[0], -sz[1], 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f( sz[0],  sz[1], 0.0);
		glEnd();
		glPopMatrix();
		
		pwrUp = pwrUp->next; //ADVANCE
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}


int PowerUp::count_this = 0;
//====================================================================
PowerUp::PowerUp(PowerUps::Type t, float p[3], float pwr, float *v)
	: ScreenItem(ScreenItem::ItemPowerUp)
{
	count_this++;
	type = t;
	power	= pwr;
	age		= 0;	

	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	if(v)
	{
		vel[0] = v[0];
		vel[1] = v[1];
		vel[2] = v[2];
	}
	else
	{
		vel[0] = 0.0;
		vel[1] = 0.0;
		vel[2] = 0.0;
	}
	
	back = 0;
	next = 0;
}

PowerUp::~PowerUp()
{
	count_this--;
}

//----------------------------------------------------------
void PowerUp::seal()
{
	if(back)
	{
		if(next)
		{
			back->next = next;
			next->back = back;
		}
		else
		{
			back->next = 0;
		}
	}
	else
	{
		fprintf(stderr, "?? PowerUp::seal()");
	}
}
	
	
