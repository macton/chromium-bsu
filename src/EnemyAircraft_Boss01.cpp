/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft_Boss01.h"

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
EnemyAircraft_Boss01::EnemyAircraft_Boss01(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_Boss01::~EnemyAircraft_Boss01()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_Boss01::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_Boss01::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);

	damage = baseDamage = -10000.0*game->gameSkill;
	size[0] = 2.6;
	size[1] = 2.3;
	collisionMove = 0.1;
	vel[1] = 0.02;
	age = 600;
}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_Boss01::update()
{
	EnemyAircraft *tmpAircraft = 0;
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
	if(fabs(a) < 5.0)
	{	
		shootVec[1] = -0.65;
		preFire = (age%6)/6.0;
		if( !(age%6) )
		{
			shootSwap = !shootSwap;
			if(shootSwap)
			{
				p[0] = pos[0]+0.55;
				p[1] = pos[1]-1.7;
				game->enemyAmmo->addAmmo(0, p, shootVec);
				p[1] += 0.5;
				game->enemyAmmo->addAmmo(0, p, shootVec);
			}
			else
			{
				p[0] = pos[0]-1.22;
				p[1] = pos[1]-1.22;
				game->enemyAmmo->addAmmo(0, p, shootVec);
				p[1] += 0.5;
				game->enemyAmmo->addAmmo(0, p, shootVec);
			}
		}
	}
	else
	{
		if(preFire > 0.0)	preFire -= 0.05;
		else 				preFire = 0.0;
	}
	//-- add Gnats
	if( !((age/512)%2) )
	{
		if( !((age/64)%2) && !(age%5) )
		{
			p[0] += 1.7;
			p[1] += 1.2;
			tmpAircraft = game->itemAdd->dynamicEnemyAdd(EnemyGnat, p, game->gameFrame+2);
			tmpAircraft->over = this;
			tmpAircraft->setTarget(this);
		}
	}
}
	
//----------------------------------------------------------
void EnemyAircraft_Boss01::move()
{
	Config *config = Config::getInstance();
	float	*hpos;
	if(target)
		hpos = target->getPos();
	else
		hpos = pos;
	float	diff[2] = { hpos[0]-pos[0], hpos[1]-pos[1] };
	float	approachDist;

	if( (((age+25)/512)%2) )
		approachDist = 9.0*(2.0-game->gameSkill);
	else
		approachDist = 12.0*(2.0-game->gameSkill);

	if(fabs(diff[1]) < (approachDist+2.0*sin(game->frame*0.05)) )
		diff[1] = diff[1] * diff[1]/approachDist;
	diff[0] += 5.0*sin(age*0.1);

	if( ((age/512)%2) )
	{
		lastMoveX = (0.98*lastMoveX)+(0.0010*game->gameSkill*diff[0]);
		lastMoveY = (0.90*lastMoveY)+(0.0020*game->gameSkill*diff[1]);
	}
	else //-- release gnats
	{
		lastMoveX = (0.90*lastMoveX)+(0.0003*game->gameSkill*diff[0]);
		lastMoveY = (0.90*lastMoveY)+(0.0010*game->gameSkill*diff[1]);
	}
	pos[0] += game->speedAdj*(lastMoveX);
	pos[1] += game->speedAdj*(lastMoveY+vel[1]);

	if(pos[0] < -config->getScreenBoundX())
		pos[0] = -config->getScreenBoundX();
	if(pos[0] >  config->getScreenBoundX())
		pos[0] =  config->getScreenBoundX();
}
