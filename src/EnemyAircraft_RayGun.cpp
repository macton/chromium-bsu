/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft_RayGun.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Config.h"

#include "define.h"
#include "Ammo.h"
#include "Global.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"

//=================================================================
EnemyAircraft_RayGun::EnemyAircraft_RayGun(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_RayGun::~EnemyAircraft_RayGun()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_RayGun::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_RayGun::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);

	damage = baseDamage = -1000.0*game->gameSkill;
	size[0] = 1.2;
	size[1] = 1.2;
	collisionMove = 0.2;
	vel[1] =  0.03;
	randMoveX = 0.5+0.5*randMoveX;

}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_RayGun::update()
{
	float	v[3] = { 0.0, -0.2, 0.0 };
//	float	*hpos = target->getPos();
	float	*hpos = game->hero->getPos();
	float	a = hpos[0]-pos[0];
	
	//-- update age
	age++;
	shootInterval--;
	
	pos[0] += secondaryMove[0]*game->speedAdj;
	pos[1] += secondaryMove[1]*game->speedAdj;
	float s = (1.0-game->speedAdj)+(game->speedAdj*0.7);
	secondaryMove[0] *= s;
	secondaryMove[1] *= s;
	move();
	
	float	p[3] = { pos[0], pos[1], pos[2] };
	
	if(fabs(a) < 1.5) //-- big center gun
	{
		v[1] = -0.6;
		p[1] = pos[1]-0.5;
		game->enemyAmmo->addAmmo(3, p, v);
	}
}

//----------------------------------------------------------
void EnemyAircraft_RayGun::move()
{
	Config *config = Config::instance();
	float	*hpos;
	if(target)
		hpos = target->getPos();
	else
		hpos = pos;
	float	diff[2] = { hpos[0]-pos[0], hpos[1]-pos[1] };
	float	tmpd = 0.0;
	float	tmps = 0.0;

	if( (tmpd = fabs(diff[0])) < 3.0)
		tmps = ((3.0-tmpd)/3.0)*(0.1*sin(age*0.25));
	if(fabs(diff[1]) < 7.0)
		diff[1] *= 0.1;
	lastMoveX = (0.975*lastMoveX)+(0.0020*diff[0]);
	lastMoveY = (0.90*lastMoveY)+(0.001*diff[1]);
	pos[0] += game->speedAdj*(randMoveX*lastMoveX * (game->gameSkill+0.1) + tmps);
	pos[1] += game->speedAdj*(lastMoveY+vel[1] * (game->gameSkill+0.1));

	if(pos[0] < -config->screenBoundX())
		pos[0] = -config->screenBoundX();
	if(pos[0] >  config->screenBoundX())
		pos[0] =  config->screenBoundX();
}
