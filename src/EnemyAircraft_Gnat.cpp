/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft_Gnat.h"

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
EnemyAircraft_Gnat::EnemyAircraft_Gnat(EnemyType et, float p[3], float randFact)
	: EnemyAircraft(et, p, randFact)
{
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft_Gnat::~EnemyAircraft_Gnat()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft_Gnat::init() 
{
	EnemyAircraft::init();
}

//----------------------------------------------------------
void EnemyAircraft_Gnat::init(float *p, float randFact)
{
	EnemyAircraft::init(p, randFact);

	damage = baseDamage = -10.0;
	size[0] = 0.45;
	size[1] = 0.45;
	collisionMove = 0.0;
	randMoveX = 0.5+0.5*randMoveX;
	vel[0] = 0.2;
	vel[1] = 0.1;

}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft_Gnat::update()
{
	float	v[3] = { 0.0, -0.2, 0.0 };
//	float	*hpos = target->getPos();
	float	*hpos = game->hero->getPos();
	float	a = hpos[0]-pos[0];
	float	b = hpos[1]-pos[1];
	
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

	if(!shootInterval)
	{
		calcShootInterval();
		if(fabs(a) < 2.0 && b < 0.0) //-- 
		{
			v[1] = -0.39;
			p[1] = pos[1]-0.5;
			game->enemyAmmo->addAmmo(4, p, v);
		}
	}
}

//----------------------------------------------------------
void EnemyAircraft_Gnat::calcShootInterval()
{
	shootInterval = (int)((1.0 + FRAND*5.0)/game->speedAdj);
}	
	
//----------------------------------------------------------
void EnemyAircraft_Gnat::move()
{
	Config *config = Config::getInstance();
	float	*hpos;
	if(target)
		hpos = target->getPos();
	else
		hpos = pos;
	float	diff[2] = { hpos[0]-pos[0], hpos[1]-pos[1] };
	float	tmp  = 0.0;
	float	tmpd = 0.0;
	float	tmps = 0.0;
	float	randX;
	float	v0,v1;
	float	x,y,tmpX,tmpY,speed;
	float	dist;

	if(target == game->hero)
		randX = randMoveX;
	else
		randX = 0.75+FRAND*0.15;
	tmps = 3.8;
	dist = sqrt(diff[0]*diff[0]+diff[1]*diff[1])*randX; 
//			dist = fabs(diff[0])+fabs(diff[1])*randX;
	tmpd = 0.4+0.6*((dist+0.2*sin(age*0.001))/tmps);
	speed = tmpd*0.25*randX;
	tmp = (diff[0]/dist);
	x = speed*tmp;//*(1.0+tmp);
	tmp = (diff[1]/dist);
	y = speed*tmp;
	if(dist < tmps)
	{
		tmpX = x;
		tmpY = y;
		x = tmpd*tmpX + -(1.0-tmpd)*diff[1]/tmpd;
		y = tmpd*tmpY +  (1.0-tmpd)*diff[0]/tmpd;
		y += 0.01*sin(game->gameFrame*0.001);
	}
	else
	{
		tmpd = .97;
		tmpX = x;
		tmpY = y;
		if(randX < 0.65)
		{
			x = tmpd*tmpX +  (1.0-tmpd)*diff[1]/tmpd;
			y = tmpd*tmpY + -(1.0-tmpd)*diff[0]/tmpd;
		}
		else
		{
			x = tmpd*tmpX + -(1.0-tmpd)*diff[1]/tmpd;
			y = tmpd*tmpY +  (1.0-tmpd)*diff[0]/tmpd;
		}
	}

	tmp = randX*0.2;
	if( (age/8)%2 )
		v0 = vel[0]*(0.85-tmp) + (0.2+tmp)*(randX-0.2)*x;
	else
		v0 = vel[0];
	v1 = vel[1]*(0.85-tmp) + (0.2+tmp)*(randX-0.2)*y;

	if(age < 50)
	{
		float amt;
		if(age > 20)	amt = (age-20)/30.0;
		else			amt = 0.0;
		vel[0] = (1.0-amt)*vel[0] + amt*v0;
		vel[1] = (1.0-amt)*vel[1] + amt*v1;
	}
	else
	{
		vel[0] = v0;
		vel[1] = v1;
	}

	pos[0] += game->speedAdj*vel[0];
	pos[1] += game->speedAdj*vel[1];

	//-- don't allow to kill hero life...
	if(pos[1] < -10.0)
		pos[1] = -10.0;

	if(pos[0] < -config->getScreenBoundX())
		pos[0] = -config->getScreenBoundX();
	if(pos[0] >  config->getScreenBoundX())
		pos[0] =  config->getScreenBoundX();
}
