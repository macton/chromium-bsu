/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAmmo.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glpng.h>

#include "extern.h"
#include "Global.h"
#include "Explosions.h"
#include "EnemyFleet.h"
#include "EnemyAircraft.h"

#include "HeroAircraft.h"

//====================================================================
EnemyAmmo::EnemyAmmo()
{
	int i;
	
	//-- initialize everything to sane values...
	for(i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		ammoRoot[i]  = new ActiveAmmo();
		ammoSize[i][0]	= 0.13;
		ammoSize[i][1]	= 0.13;
		ammoDamage[i]	= 510.0;
	}
	ammoPool = new ActiveAmmo();
	
	loadTextures();

	ammoDamage[0]	= 75.0;
	ammoSize[0][0]	= 0.25;
	ammoSize[0][1]	= 0.55;
		
	ammoDamage[1]	= 6.0;
	ammoSize[1][0]	= 0.225;
	ammoSize[1][1]	= 0.225;
		
	ammoDamage[2]	= 100.0;
	ammoSize[2][0]	= 0.45;
	ammoSize[2][1]	= 0.45;
		
	ammoDamage[3]	= 20.0;
	ammoSize[3][0]	= 0.3;
	ammoSize[3][1]	= 0.5;
		
	ammoDamage[4]	= 8.5;
	ammoSize[4][0]	= 0.12;
	ammoSize[4][1]	= 0.5;
		
}	

EnemyAmmo::~EnemyAmmo()
{
	ActiveAmmo *cur;
	ActiveAmmo *del;
	
	clear();
	cur = ammoPool->next;
	while(cur)
	{
		del = cur;
		cur = cur->next;
		delete del;
	}
	
	deleteTextures();
	
	for(int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		delete ammoRoot[i];
	}
	delete ammoPool;
}

//----------------------------------------------------------
void EnemyAmmo::loadTextures()
{
	char filename[256];
	pngInfo tmpInfo;
	for(int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		sprintf(filename, "png/enemyAmmo%02d.png", i);
		ammoTex[i] = pngBind(dataLoc(filename), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	}
	
}

//----------------------------------------------------------
void EnemyAmmo::deleteTextures()
{
	for(int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		glDeleteTextures(1, &ammoTex[i]);
		ammoTex[i] = 0;
	}
}

//----------------------------------------------------------
ActiveAmmo *EnemyAmmo::getNewAmmo()
{
	ActiveAmmo *retAmmo;
	if(ammoPool->next)
	{
		retAmmo = ammoPool->next;
		ammoPool->next = retAmmo->next;
	}
	else
	{
		retAmmo = new ActiveAmmo();
	}
	return retAmmo;
}

//----------------------------------------------------------
void EnemyAmmo::killAmmo(ActiveAmmo *dead)
{
	dead->back = 0;
	dead->next = ammoPool->next;
	ammoPool->next = dead;
}


//----------------------------------------------------------
void	EnemyAmmo::clear()
{
	int i;
	ActiveAmmo *cur;
	ActiveAmmo *del;
	
	for(i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		cur = ammoRoot[i]->next;
		while(cur)
		{
			del = cur;
			cur = cur->next;
			killAmmo(del);
		}
		ammoRoot[i]->next = 0;
	}
}

//----------------------------------------------------------
void EnemyAmmo::addAmmo(int type, float pos[3], float vel[3])
{
	ActiveAmmo *newAmmo = 0;
	ActiveAmmo *first = 0;
	
	if(type >= 0 && type < NUM_ENEMY_AMMO_TYPES)
	{
		float v[3] = { 
			vel[0]*Global::speedAdj,
			vel[1]*Global::speedAdj,
			vel[2]*Global::speedAdj, };
		newAmmo = getNewAmmo();
		newAmmo->init(pos, v, ammoDamage[type]);
		newAmmo->back = ammoRoot[type];
		newAmmo->next = ammoRoot[type]->next;
		if(ammoRoot[type]->next)
		{
			first = ammoRoot[type]->next;
			first->back = newAmmo;
		}
		ammoRoot[type]->next = newAmmo;
	}
}

//----------------------------------------------------------
void EnemyAmmo::updateAmmo()
{
	int i;
	ActiveAmmo *thisAmmo;
	
	for(i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		thisAmmo = ammoRoot[i]->next;
		while(thisAmmo)
		{
			//-- clean up ammo
			if(	thisAmmo->pos[0] >  Global::screenBound[0] ||
				thisAmmo->pos[0] < -Global::screenBound[0] ||
				thisAmmo->pos[1] >  Global::screenBound[1] ||
				thisAmmo->pos[1] < -Global::screenBound[1] ) // remove ammo
			{
				ActiveAmmo *backAmmo = thisAmmo->back;
				ActiveAmmo *nextAmmo = thisAmmo->next;
				backAmmo->next = nextAmmo;
				if(nextAmmo)
					 nextAmmo->back = backAmmo;
				killAmmo(thisAmmo);
				thisAmmo = nextAmmo; //ADVANCE
			}
			else
			{
				thisAmmo->updatePos();
				thisAmmo = thisAmmo->next; //ADVANCE
			}
		}
	}
}

//----------------------------------------------------------
void EnemyAmmo::checkForHits(HeroAircraft *hero)
{
	int		i;
	ActiveAmmo		*thisAmmo;
	ActiveAmmo		*backAmmo;
	ActiveAmmo		*nextAmmo;
	float	minDist;
	float	dist;
	float	*p;
	
	minDist = (hero->getSize(0)+hero->getSize(1))*0.5;
	
	if(!hero->isVisible())
		return;
	//-- Go through all the ammunition and check for hits
	for(i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		thisAmmo = ammoRoot[i]->next;
		while(thisAmmo)
		{
			p = thisAmmo->pos;
			dist = fabs(p[0]-hero->pos[0]) + fabs(p[1]-hero->pos[1]);	
			if(dist < minDist)
			{
				Explo *explo;
				//do damage
//				hero->doDamage(ammoDamage[i]);
				hero->ammoDamage(ammoDamage[i], thisAmmo->vel);
				//add explosion
				explo = Global::explosions->addExplo((Explosions::ExploType)(Explosions::EnemyAmmo00+i), thisAmmo->pos);
				if(i > 1)	// add second explosion for the bug guns...		
					explo = Global::explosions->addExplo((Explosions::ExploType)(Explosions::EnemyAmmo00+i), thisAmmo->pos, -5);
				else
					if(explo) 
						explo->vel[1] = -0.1;
				

				backAmmo = thisAmmo->back;
				nextAmmo = thisAmmo->next;
				backAmmo->next = nextAmmo;
				if(nextAmmo)
					 nextAmmo->back = backAmmo;
				killAmmo(thisAmmo);
				thisAmmo = backAmmo;
			}
			thisAmmo = thisAmmo->next;
		}
	}
}

//----------------------------------------------------------
void EnemyAmmo::drawGL()
{
	int i;
	float	*pos;
	ActiveAmmo 	*thisAmmo;
	
	for(i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, ammoTex[i]);
		thisAmmo = ammoRoot[i]->next;
		glBegin(GL_QUADS);
		while(thisAmmo)
		{
			pos = thisAmmo->pos;
			switch(IRAND%4)
			{
				case 0:
					glTexCoord2f(0.0, 0.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 1.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 1.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 0.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					break;
				case 1:
					glTexCoord2f(1.0, 0.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 1.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 1.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 0.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					break;
				case 2:
					glTexCoord2f(0.0, 1.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 0.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 0.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 1.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					break;
				case 3:
					glTexCoord2f(1.0, 1.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					glTexCoord2f(1.0, 0.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 0.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
					glTexCoord2f(0.0, 1.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]+ammoSize[i][1], pos[2]);
					break;
			}
			thisAmmo = thisAmmo->next; //ADVANCE
		}
		glEnd();
	}
}


	
