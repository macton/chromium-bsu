/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "HeroAmmo.h"

#include <cstdio>
#include <cstdlib>

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Config.h"

#include "extern.h"
#include "Global.h"
#include "Explosions.h"
#include "EnemyFleet.h"
#include "EnemyAircraft.h"
#include "Image.h"

//====================================================================
HeroAmmo::HeroAmmo()
{
	game = Global::getInstance();
	int i;
	
	//-- initialize everything to sane values...
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		ammoRoot[i]  = new ActiveAmmo();
		ammoSize[i][0]	= 0.1;
		ammoSize[i][0]	= 0.5;
		ammoDamage[i]	= 2.5;
	}
	ammoPool  = new ActiveAmmo();

	loadTextures();
	
	ammoSize[0][0] = 0.05;	ammoSize[0][1] = 0.65;
	ammoSize[1][0] = 0.11;	ammoSize[1][1] = 1.5;
	ammoSize[2][0] = 0.3;	ammoSize[2][1] = 1.5;
	
	ammoDamage[0] =  3.5;
	ammoDamage[1] =  6.0;
	ammoDamage[2] = 40.0;
}	

HeroAmmo::~HeroAmmo()
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
	
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
		delete 	ammoRoot[i];
	delete ammoPool;
	
	deleteTextures();
}

//----------------------------------------------------------
void HeroAmmo::loadTextures()
{
	char filename[256];
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		sprintf(filename, "png/heroAmmo%02d.png", i);
		ammoTex[i] = Image::load(dataLoc(filename));
	}
}

//----------------------------------------------------------
void HeroAmmo::deleteTextures()
{
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		glDeleteTextures(1, &ammoTex[i]);
		ammoTex[i] = 0;
	}
}

//----------------------------------------------------------
ActiveAmmo *HeroAmmo::getNewAmmo()
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
void HeroAmmo::killAmmo(ActiveAmmo *dead)
{
	dead->back = 0;
	dead->next = ammoPool->next;
	ammoPool->next = dead;
}

//----------------------------------------------------------
void	HeroAmmo::clear()
{
	int i;
	ActiveAmmo *cur;
	ActiveAmmo *del;
	
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
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
void HeroAmmo::addAmmo(int type, float pos[3])
{
	float	vel[3] = { 0.0, 0.0, 0.0 };
	ActiveAmmo *newAmmo = 0;
	ActiveAmmo *first = 0;
	
	switch(type)
	{
		case 0:	vel[1] = 0.5*game->speedAdj;	break;
		case 1:	vel[1] = 0.2*game->speedAdj;	break;
		case 2:	vel[1] = 0.3*game->speedAdj;	break;
		default:	break;
	}
	if(type >= 0 && type < NUM_HERO_AMMO_TYPES)
	{
		Config* config = Config::instance();
		if( config->debug() ) fprintf(stderr, _("HeroAmmo::addAmmo(%d, <%f %f %f> )\n"), type, pos[0], pos[1], pos[2]);
		newAmmo = getNewAmmo();
		newAmmo->init(pos, vel, ammoDamage[type]);
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
void HeroAmmo::printList(int type)
{
	ActiveAmmo *thisAmmo;
	if(type >= 0 && type < NUM_HERO_AMMO_TYPES)
	{
		thisAmmo = ammoRoot[type];
		while(thisAmmo)
		{
			fprintf(stderr, _("%p <--%p--> %p\n"), (void*)thisAmmo->back, (void*)thisAmmo, (void*)thisAmmo->next);
			thisAmmo = thisAmmo->next;
		}
	}
	fprintf(stderr, " \n");
}
//----------------------------------------------------------
void HeroAmmo::updateAmmo()
{
	Config *config = Config::instance();
	int i;
	ActiveAmmo *thisAmmo;
	
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		thisAmmo = ammoRoot[i]->next;
		while(thisAmmo)
		{
			//-- clean up ammo
			if(thisAmmo->pos[1] > config->screenBoundY()) // remove ammo
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
void HeroAmmo::checkForHits(EnemyFleet *fleet)
{
	int		i;
	float	minShipY = 100.0;
	ActiveAmmo		*thisAmmo;
	ActiveAmmo		*backAmmo;
	ActiveAmmo		*nextAmmo;
	EnemyAircraft	*enemy;
	
	//-- Get minimum ship Y location so we can ignore some of the ammunition
	fleet->toFirst();
	enemy = fleet->getShip();
	if(!enemy) //-- no ships - return immediately
		return;
	while(enemy)
	{
		if(enemy->pos[1]-3.0 < minShipY)
			minShipY = enemy->pos[1]-3.0;
		enemy = fleet->getShip();
	}
	
	//-- Go through all the ammunition and check for hits
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		thisAmmo = ammoRoot[i]->next;
		while(thisAmmo)
		{
			if(thisAmmo->pos[1] < minShipY)
			{
				thisAmmo = thisAmmo->next;
				continue;
			}
			fleet->toFirst();
			enemy = fleet->getShip();
			while(enemy)
			{
				if(enemy->checkHit(thisAmmo) == true)
				{
					//do damage
					if(i == 1)
						enemy->damage += ammoDamage[i]*game->speedAdj;
					else
						enemy->damage += ammoDamage[i];
					
					//add explosion
					game->explosions->addExplo((Explosions::ExploType)(Explosions::HeroAmmo00+i), thisAmmo->pos);					

					if(i != 1) // ammo type 1 doesn't get killed
					{
						backAmmo = thisAmmo->back;
						nextAmmo = thisAmmo->next;
						backAmmo->next = nextAmmo;
						if(nextAmmo)
							 nextAmmo->back = backAmmo;
						killAmmo(thisAmmo);
						thisAmmo = backAmmo;
						enemy = 0; //-- break out of enemy loop
					}
					else
						enemy = fleet->getShip();
				}
				else
					enemy = fleet->getShip();
			}
			thisAmmo = thisAmmo->next;
		}
	}
}

//----------------------------------------------------------
void HeroAmmo::drawGL()
{
	int i;
	float	*pos;
	ActiveAmmo 	*thisAmmo;
	
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, ammoTex[i]);
		thisAmmo = ammoRoot[i]->next;
		glBegin(GL_QUADS);
		while(thisAmmo)
		{
			pos = thisAmmo->pos;
			glTexCoord2f(0.0, 0.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1],     pos[2]);
			glTexCoord2f(0.0, 1.0); glVertex3f(pos[0]-ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
			glTexCoord2f(1.0, 1.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1]-ammoSize[i][1], pos[2]);
			glTexCoord2f(1.0, 0.0); glVertex3f(pos[0]+ammoSize[i][0], pos[1],     pos[2]);
			thisAmmo = thisAmmo->next; //ADVANCE
		}
		glEnd();
	}
}


	
