/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft_Tank.h"

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
EnemyAircraft_Tank::EnemyAircraft_Tank(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_Tank::~EnemyAircraft_Tank()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_Tank::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_Tank::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);

	damage = baseDamage = -2000.0*game->gameSkill;
	size[0] = 1.9;
	size[1] = 2.1;
	collisionMove = 0.1;
	vel[1] =  0.03;

}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_Tank::update()
{
	float	v[3] = { 0.0, -0.2, 0.0 };
//	float	*hpos = target->getPos();
	float	*hpos = game->hero->getPos();
	float	a = hpos[0]-pos[0];
	float	b = hpos[1]-pos[1];
	float	dist;
	float	ammoSpeed = 0.35*game->speedAdj;
	
	int		tmpInt;
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

	p[1] = pos[1] - 1.7;
	if(fabs(a) < 4.0)
	{
		if(shootSwap == 0 || shootSwap == 8 || shootSwap == 16 )
		{
			v[1] = -0.2;
			p[0] = pos[0] + 1.5;
			game->enemyAmmo->addAmmo(0, p, v);
			p[0] = pos[0] - 1.5;
			game->enemyAmmo->addAmmo(0, p, v);
		}
		shootSwap = ++shootSwap%100;
	}

	if(!((tmpInt = age/200)%2)) //-- omni shooters
	{
		tmpInt = age%200;
		if(tmpInt < 100)
		{
			preFire = (float)tmpInt/100.0f;
		}
		else if(tmpInt < 170)
		{
			if(!(age%10))
			{
				p[1] = pos[1]-0.45;
				b = hpos[1]-p[1];

				p[0] = pos[0];
				a = hpos[0]-p[0];
				dist = fabs(a) + fabs(b);
				shootVec[0] = 2.0*ammoSpeed*a/dist;
				shootVec[1] = 2.0*ammoSpeed*b/dist;
				secondaryMove[0] -= shootVec[0]*0.1;
				secondaryMove[1] -= shootVec[1]*0.1;
				game->enemyAmmo->addAmmo(2, p, shootVec);
				preFire -= 0.4;
				if(preFire < 0.0)
					preFire = 0.0;	
			}
			else
				preFire += 0.035;
		}
		else
			preFire = 0.0;
	}

}

//----------------------------------------------------------
void EnemyAircraft_Tank::move()
{
	Config *config = Config::instance();
	float	*hpos;
	if(target)
		hpos = target->getPos();
	else
		hpos = pos;
	float	diff[2] = { hpos[0]-pos[0], hpos[1]-pos[1] };
	float	v1;
	

	if(fabs(diff[0]) > 8.0)
		v1 = 0.04;
	else
	{
		v1 = 0.04*(fabs(diff[0])/8.0); 
	}
	vel[1] = 0.99*vel[1] + 0.01*v1;

	if(pos[1] < -3.0)
		vel[1] = -0.1;
	else if(pos[1] < 0.0)
		vel[1] *= 0.99;

	if(pos[0] < 0.0)
		pos[0] = game->speedAdj*(0.998*pos[0] + 0.002*(-config->screenBoundX()+2.85));
	else
		pos[0] = game->speedAdj*(0.998*pos[0] + 0.002*( config->screenBoundX()-2.85));
	switch((age/50)%8)
	{
		case 2:
			pos[1] += game->speedAdj*(0.05);
			break;
		default:
			pos[1] -= game->speedAdj*(vel[1]);
			break;
	}


	if(pos[0] < -config->screenBoundX())
		pos[0] = -config->screenBoundX();
	if(pos[0] >  config->screenBoundX())
		pos[0] =  config->screenBoundX();
}
