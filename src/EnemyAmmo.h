/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef EnemyAmmo_h
#define EnemyAmmo_h

#include "compatibility.h"
#include <GL/gl.h>

#include "define.h"
#include "Ammo.h"

class Global;
class HeroAircraft;

//====================================================================
class EnemyAmmo
{
public:
	EnemyAmmo();
	~EnemyAmmo();
	
	void	addAmmo(int type, float pos[3], float vel[3]);
	void	updateAmmo();
	
	void	checkForHits(HeroAircraft *hero);
	
	void	drawGL();
	void	clear();
	
	void	loadTextures();
	void	deleteTextures();
	
private:
	ActiveAmmo	*getNewAmmo();
	void 		killAmmo(ActiveAmmo *dead);	

private:
	ActiveAmmo	*ammoRoot[NUM_ENEMY_AMMO_TYPES];
	ActiveAmmo	*ammoPool;
	float		ammoSize[NUM_ENEMY_AMMO_TYPES][2];
	float		ammoDamage[NUM_ENEMY_AMMO_TYPES];
		
	GLuint	ammoTex[NUM_ENEMY_AMMO_TYPES];

private:
	Global *game;
};


#endif // EnemyAmmo_h
