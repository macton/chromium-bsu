/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef HeroAmmo_h
#define HeroAmmo_h

#include "compatibility.h"
#include <GL/gl.h>

#include "define.h"
#include "Ammo.h"

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
};


#endif // HeroAmmo_h
