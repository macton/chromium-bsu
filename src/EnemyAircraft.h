/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef EnemyAircraft_h
#define EnemyAircraft_h

#include "ScreenItem.h"

class ActiveAmmo;
class Global;

enum EnemyType{	EnemyStraight, EnemyOmni, 
				EnemyRayGun, EnemyTank,
				EnemyGnat, 
				EnemyBoss00,
				EnemyBoss01,
				NumEnemyTypes};

//====================================================================
class EnemyAircraft : public ScreenItem
{
public:
	EnemyAircraft(EnemyType et, float p[3], float randFact = 1.0);
	~EnemyAircraft();
	
	bool	checkHit(ActiveAmmo *ammo);
	void	update();
	void	init();
	void	init(float *p, float randFact = 1.0);
	
	void	setTarget(ScreenItem *t) { target = t; }

	EnemyType	type;
	float		size[2];
	float		damage;
	float		baseDamage;
	float		collisionMove;
	float		secondaryMove[2];
	float		preFire;

friend class EnemyFleet;
friend class ScreenItemAdd;

protected:
	EnemyAircraft	*next;
	EnemyAircraft	*back;
	
	EnemyAircraft	*over;
	
	float	shootVec[3];
	int		shootPause;
	int		shootInterval;
	int		shootSwap;

private:
	void	calcShootInterval();
	void	move();
	
	float	randMoveX;
	float	lastMoveX;
	float	lastMoveY;
	
	float	speedAdj;
	
	ScreenItem	*target;

private:
	Global	*game;
};

#endif //EnemyAircraft_h
