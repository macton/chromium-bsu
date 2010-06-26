/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef HeroAmmo_h
#define HeroAmmo_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "define.h"
#include "Ammo.h"

class Global;
class EnemyFleet;

//====================================================================
class HeroAmmo
{
public:
	HeroAmmo();
	~HeroAmmo();
	
	void	addAmmo(int type, float pos[3]);
	void	updateAmmo();
	
	void	checkForHits(EnemyFleet *fleet);
	
	void	printList(int type);
	
	void	drawGL();
	void	clear();
	
	void	loadTextures();
	void	deleteTextures();
	
private:
	ActiveAmmo	*getNewAmmo();
	void 		killAmmo(ActiveAmmo *dead);	

private:
	ActiveAmmo	*ammoRoot[NUM_HERO_AMMO_TYPES];
	ActiveAmmo	*ammoPool;
	float		ammoSize[NUM_HERO_AMMO_TYPES][2];
	float		ammoDamage[NUM_HERO_AMMO_TYPES];
		
	GLuint	ammoTex[NUM_HERO_AMMO_TYPES];

private:
	Global	*game;
};


#endif // HeroAmmo_h
