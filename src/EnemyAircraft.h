/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef EnemyAircraft_h
#define EnemyAircraft_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "ScreenItem.h"

class ActiveAmmo;
class Global;
class EnemyAircraft_Boss01;

enum EnemyType{	
	EnemyStraight, 
	EnemyOmni, 
	EnemyRayGun, 
	EnemyTank,
	EnemyGnat, 
	EnemyBoss00,
	EnemyBoss01,
	NumEnemyTypes
};


//====================================================================
class EnemyAircraft : public ScreenItem
{
public:
	EnemyAircraft(EnemyType et, float p[3], float randFact = 1.0);
	virtual ~EnemyAircraft();
	
	virtual void	update() = 0;
	virtual void	init();
	virtual void	init(float *p, float randFact = 1.0);
	
	bool	checkHit(ActiveAmmo *ammo);
	void	setTarget(ScreenItem *t) { target = t; }
	
	void	drawGL(GLuint tex, GLuint xtraTex);

	EnemyType	type;
	float		size[2];
	float		damage;
	float		baseDamage;
	float		collisionMove;
	float		secondaryMove[2];
	float		preFire;

	static EnemyAircraft *makeNewEnemy(EnemyType et, float p[3], float randFact = 1.0);

protected:
	virtual void	calcShootInterval();
	virtual void	move() = 0;
	
protected:
	EnemyAircraft	*next;
	EnemyAircraft	*back;
	EnemyAircraft	*over;
	
	float	shootVec[3];
	int		shootPause;
	int		shootInterval;
	int		shootSwap;

	float	randMoveX;
	float	lastMoveX;
	float	lastMoveY;
	
	float	speedAdj;
	
	ScreenItem	*target;

protected:
	Global	*game;

	inline void drawQuad(float szx, float szy)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1.0, 0.0); glVertex3f( szx,  szy, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-szx,  szy, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f( szx, -szy, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-szx, -szy, 0.0);
		glEnd();
	}

private:
	static int	allocated;
public:
	static void	printNumAllocated(void);

friend class EnemyFleet;
friend class ScreenItemAdd;
friend class EnemyAircraft_Boss01;
};

#endif //EnemyAircraft_h
