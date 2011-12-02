/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#include "EnemyAircraft_Straight.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Config.h"

#include "gettext.h"
#include "define.h"
#include "Ammo.h"
#include "Global.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"

//=================================================================
EnemyAircraft_Straight::EnemyAircraft_Straight(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_Straight::~EnemyAircraft_Straight()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_Straight::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_Straight::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);
	float frand = FRAND*randFact;

	damage = baseDamage = -110.0*game->gameSkill;
	size[0] = 0.75;
	size[1] = 1.02;
	collisionMove = 0.5;
	vel[1] = -0.046-frand*0.04;
}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_Straight::update()
{
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

	if(shootInterval < 10)
		preFire = (10-shootInterval)/10.0;
	else
		preFire = 0.0;
	if(!shootInterval)
	{
		calcShootInterval();
		p[1] -= 0.9;
		game->enemyAmmo->addAmmo(0, p, shootVec);
	}
}

//----------------------------------------------------------
void EnemyAircraft_Straight::calcShootInterval()
{
	shootInterval = (int)((30.0 + FRAND*90.0)/game->speedAdj);
}	
	
//----------------------------------------------------------
void EnemyAircraft_Straight::move()
{
	Config *config = Config::instance();

	pos[1] += (game->speedAdj*(vel[1] * game->gameSkill));
	if( config->debug() ) fprintf(stderr, _("EnemyAircraft_Straight %p ... pos[1] = %g, vel[1] = %g\n"), this, pos[1], vel[1]);

	if(pos[0] < -config->screenBoundX())
		pos[0] = -config->screenBoundX();
	if(pos[0] >  config->screenBoundX())
		pos[0] =  config->screenBoundX();
}
