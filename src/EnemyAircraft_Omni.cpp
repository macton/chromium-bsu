/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft_Omni.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Config.h"

#include "define.h"
#include "Ammo.h"
#include "Global.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"

//=================================================================
EnemyAircraft_Omni::EnemyAircraft_Omni(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_Omni::~EnemyAircraft_Omni()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_Omni::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_Omni::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);
	float frand = FRAND*randFact;
	
	damage = baseDamage = -45.0;
	size[0] = 0.7;
	size[1] = 0.7;
	collisionMove = 0.7;
	vel[1] = -0.071-frand*0.04;
	
}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_Omni::update()
{
	Config	*config = Config::instance();
	float	v[3] = { 0.0, -0.2, 0.0 };
//	float	*hpos = target->getPos();
	float	*hpos = game->hero->getPos();
	float	a = hpos[0]-pos[0];
	float	b = hpos[1]-pos[1];
	float	dist;
	float	ammoSpeed = 0.35*game->speedAdj;
	
	int 	omniSwap = 108;
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

		shootSwap = shootSwap%omniSwap;
		if(shootSwap < 18)
		{
			if(!(shootSwap%6))
			{
				//ammoSpeed = 0.22;
				ammoSpeed = 0.3*game->gameSkill*game->speedAdj;
				dist = fabs(a) + fabs(b);
				//dist = sqrt(a*a+b*b);
				v[0] = a/dist;
				v[1] = b/dist;

				shootVec[0] = ammoSpeed*v[0];
				shootVec[1] = ammoSpeed*v[1]/*+vel[1]*/;
			}
			game->enemyAmmo->addAmmo(1, p, shootVec);
		}
		if(pos[1] < config->screenBoundY())
			shootSwap++;

}

//----------------------------------------------------------
void EnemyAircraft_Omni::move()
{
	Config *config = Config::instance();
	float	*hpos;
	if(target)
		hpos = target->getPos();
	else
		hpos = pos;
	float	diff[2] = { hpos[0]-pos[0], hpos[1]-pos[1] };

	lastMoveX = (0.9*lastMoveX)+(0.1*(0.01*diff[0]));
	pos[0] += game->speedAdj*(randMoveX*lastMoveX);
	pos[1] += (game->speedAdj*(vel[1] * game->gameSkill));


	if(pos[0] < -config->screenBoundX())
		pos[0] = -config->screenBoundX();
	if(pos[0] >  config->screenBoundX())
		pos[0] =  config->screenBoundX();
}
