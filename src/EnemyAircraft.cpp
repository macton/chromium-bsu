/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "EnemyAircraft.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "define.h"
#include "Ammo.h"
#include "Global.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"

//=================================================================
EnemyAircraft::EnemyAircraft(EnemyType et, float p[3], float randFact)
	: ScreenItem(ScreenItem::ItemEnemy)
{
	type = et;
	
	game = game->getInstance();
	
	float xBound = game->screenBound[0]-2.0;
	if(pos[0] < -xBound)
		pos[0] = -xBound;
	if(pos[0] > xBound)
		pos[0] = xBound;
		
	init(p, randFact);
}

//----------------------------------------------------------
EnemyAircraft::~EnemyAircraft()
{
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft::init() 
{
	ScreenItem::init();
}

//----------------------------------------------------------
void EnemyAircraft::init(float *p, float randFact)
{
	ScreenItem::init();
	
	over = 0;	//-- if this points to another aircraft, this aircraft will be inserted after that
				//   aircraft in the fleet list
	
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	
	shootInterval = 1;
	shootSwap	= 0;
	randMoveX	= randFact*FRAND;
	lastMoveX	= 0.0;
	lastMoveY	= 0.0;
	preFire		= 0.0;
	target = game->hero;
	
	next = 0;
	back = 0;
	
	secondaryMove[0] = secondaryMove[1] = 0.0;
	
	shootVec[0] =  0.0;
	shootVec[1] = -0.2;
	shootVec[2] =  0.0;
	
	vel[0] = 0.0;
	vel[1] = 0.0;
	vel[2] = 0.0;
	
	float frand = FRAND*randFact;
	switch(type)
	{
		case EnemyStraight:
			damage = baseDamage = -110.0*game->gameSkill;
			size[0] = 0.75;
			size[1] = 1.02;
			collisionMove = 0.5;
			vel[1] = -0.046-frand*0.04;
			break;
		case EnemyOmni:
			damage = baseDamage = -45.0;
			size[0] = 0.7;
			size[1] = 0.7;
			collisionMove = 0.7;
			vel[1] = -0.071-frand*0.04;
//			vel[1] = -0.021-frand*0.04;
			break;
		case EnemyRayGun:
			damage = baseDamage = -1000.0*game->gameSkill;
			size[0] = 1.2;
			size[1] = 1.2;
			collisionMove = 0.2;
			vel[1] =  0.03;
			randMoveX = 0.5+0.5*randMoveX;
			break;
		case EnemyTank:
			damage = baseDamage = -2000.0*game->gameSkill;
			size[0] = 1.9;
			size[1] = 2.1;
			collisionMove = 0.1;
			vel[1] =  0.03;
			break;
		case EnemyGnat:
			damage = baseDamage = -10.0;
			size[0] = 0.45;
			size[1] = 0.45;
			collisionMove = 0.0;
			randMoveX = 0.5+0.5*randMoveX;
			vel[0] = 0.2;
			vel[1] = 0.1;
			break;
		case EnemyBoss00:
			damage = baseDamage = -10000.0*game->gameSkill;
//			damage = baseDamage = -8.0*game->gameSkill;
			size[0] = 3.5;
			size[1] = 2.275;
			collisionMove = 0.05;
			vel[1] = 0.02;
			break;
		case EnemyBoss01:
			damage = baseDamage = -10000.0*game->gameSkill;
//			damage = baseDamage = -8.0*game->gameSkill;
			size[0] = 2.6;
			size[1] = 2.3;
			collisionMove = 0.1;
			vel[1] = 0.02;
			age = 600;
			break;
		case NumEnemyTypes:
			break;
	}
}

//-- NOTE: Many of the firing rates are not adjusted by game->speedAdj
//-- so they will not be totally correct. Should be close enough for jazz, though.
//----------------------------------------------------------
void EnemyAircraft::update()
{
	EnemyAircraft *tmpAircraft = 0;
	float	v[3] = { 0.0, -0.2, 0.0 };
//	float	*hpos = target->getPos();
	float	*hpos = game->hero->getPos();
	float	a = hpos[0]-pos[0];
	float	b = hpos[1]-pos[1];
	float	dist;
	float	ammoSpeed = 0.35*game->speedAdj;
	
	int 	omniSwap = 108;
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
	switch(type)
	{
		//------------------------------- Straight
		case EnemyStraight:	
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
			break;
		//------------------------------- Omni
		case EnemyOmni:
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
			if(pos[1] < game->screenBound[1])
				shootSwap++;
			break;
		//------------------------------- Ray Gun
		case EnemyRayGun:	
			if(fabs(a) < 1.5) //-- big center gun
			{
				v[1] = -0.6;
				p[1] = pos[1]-0.5;
				game->enemyAmmo->addAmmo(3, p, v);
			}
			break;
		//------------------------------- Tank
		case EnemyTank:		
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
//				else
//					shootSwap = 0;

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
			break;
		//------------------------------- Gnat
		case EnemyGnat:	
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
			break;
		//------------------------------- Boss 0
		case EnemyBoss00:	
			if(fabs(a) < 1.6) //-- big center gun
			{
				v[1] = -0.6;
				p[1] = pos[1]-1.7;
				game->enemyAmmo->addAmmo(3, p, v);
			}
			if(!(age%5)) //-- side cannons
			{
				shootSwap = ++shootSwap%15;
				if(shootSwap < 6)
				{
					v[1] = -0.2;
					p[1] = pos[1]-1.9;
					p[0] = pos[0]+2.0+((shootSwap%3)*0.4);
					game->enemyAmmo->addAmmo(0, p, v);
					p[0] = pos[0]-2.0-((shootSwap%3)*0.4);
					game->enemyAmmo->addAmmo(0, p, v);
				}
			}
			if(!((age-1)%7))
			{
				dist = fabs(a) + fabs(b);
				shootVec[0] = ammoSpeed*a/dist;
				shootVec[1] = ammoSpeed*b/dist;
			}
			if(!((age/200)%2)) //-- omni shooters
			{
				if(!((age/100)%2))
				{
					if(!((age/50)%2))
					{
						p[1] = pos[1]-0.45;
						p[0] = pos[0]-1.1;
						game->enemyAmmo->addAmmo(1, p, shootVec);
						p[0] = pos[0]+1.1;
						game->enemyAmmo->addAmmo(1, p, shootVec);
					}
					preFire = (age%100)/100.0f;
				}
				else if(!(age%10))
				{
					p[1] = pos[1]-0.45;
					b = hpos[1]-p[1];

					p[0] = pos[0]-1.1;
					a = hpos[0]-p[0];
					dist = fabs(a) + fabs(b);
					shootVec[0] = 2.0*ammoSpeed*a/dist;
					shootVec[1] = 2.0*ammoSpeed*b/dist;
					game->enemyAmmo->addAmmo(2, p, shootVec);
					p[0] = pos[0]+1.1;
					a = hpos[0]-p[0];
					dist = fabs(a) + fabs(b);
					shootVec[0] = 2.0*ammoSpeed*a/dist;
					shootVec[1] = 2.0*ammoSpeed*b/dist;
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
			break;
		//------------------------------- Boss 1
		case EnemyBoss01:
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
			break;
		case NumEnemyTypes:
			break;
	}
}

//----------------------------------------------------------
bool EnemyAircraft::checkHit(ActiveAmmo *ammo)
{
	bool retVal = false;
	if( ammo->pos[1] > pos[1]-size[1] ) 
	{
		if( (ammo->pos[1] < pos[1]+size[1]) &&
			(ammo->pos[0] > pos[0]-size[0]) &&
			(ammo->pos[0] < pos[0]+size[0]) )
		{
			retVal = true;
		}
	}
	return retVal;
}

//----------------------------------------------------------
void EnemyAircraft::calcShootInterval()
{
	switch(type)
	{
		case EnemyStraight:
			shootInterval = (int)((30.0 + FRAND*90.0)/game->speedAdj);
			break;
		case EnemyOmni:
			shootInterval = 1;
			break;
		case EnemyRayGun:
			shootInterval = 1;
			break;
		case EnemyTank:
			shootInterval = 1;
			break;
		case EnemyGnat:
			shootInterval = (int)((1.0 + FRAND*5.0)/game->speedAdj);
			break;
		case EnemyBoss00:
			shootInterval = 1;
			break;
		case EnemyBoss01:
			shootInterval = 1;
			break;
		case NumEnemyTypes:
			break;
	}
}	
	
//----------------------------------------------------------
void EnemyAircraft::move()
{
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
	float	approachDist;
	switch(type)
	{
		case EnemyStraight:
			pos[1] += (game->speedAdj*(vel[1] * game->gameSkill));
			break;
		case EnemyOmni:
			lastMoveX = (0.9*lastMoveX)+(0.1*(0.01*diff[0]));
			pos[0] += game->speedAdj*(randMoveX*lastMoveX);
			pos[1] += (game->speedAdj*(vel[1] * game->gameSkill));
			break;
		case EnemyRayGun:
			if( (tmpd = fabs(diff[0])) < 3.0)
				tmps = ((3.0-tmpd)/3.0)*(0.1*sin(age*0.25));
			if(fabs(diff[1]) < 7.0)
				diff[1] *= 0.1;
			lastMoveX = (0.975*lastMoveX)+(0.0020*diff[0]);
			lastMoveY = (0.90*lastMoveY)+(0.001*diff[1]);
			pos[0] += game->speedAdj*(randMoveX*lastMoveX * (game->gameSkill+0.1) + tmps);
			pos[1] += game->speedAdj*(lastMoveY+vel[1] * (game->gameSkill+0.1));
			break;
		case EnemyTank:
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
				pos[0] = game->speedAdj*(0.998*pos[0] + 0.002*(-game->screenBound[0]+2.85));
			else
				pos[0] = game->speedAdj*(0.998*pos[0] + 0.002*( game->screenBound[0]-2.85));
			switch((age/50)%8)
			{
				case 2:
					pos[1] += game->speedAdj*(0.05);
					break;
				default:
					pos[1] -= game->speedAdj*(vel[1]);
					break;
			}
			break;
		case EnemyGnat:
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
			
			if(pos[1] < -10.0)
				pos[1] = -10.0;
			break;
		case EnemyBoss00:
			approachDist = 7.0*(2.0-game->gameSkill);
			if(fabs(diff[1]) < (approachDist+0.0*sin(game->frame*0.05)) )
			{
				diff[1] = diff[1] * diff[1]/approachDist;
			}
			diff[0] += 5.0*sin(age*0.1);
			lastMoveX = (0.98*lastMoveX)+(0.0005*game->gameSkill*diff[0]);
			lastMoveY = (0.90*lastMoveY)+(0.001*game->gameSkill*diff[1]);
			pos[0] += game->speedAdj*(lastMoveX);
			pos[1] += game->speedAdj*(lastMoveY+vel[1]);
			break;
		case EnemyBoss01:
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
			break;
		default:
			pos[1] -= game->speedAdj*0.02;
			break;
	}
	if(pos[0] < -game->screenBound[0])
		pos[0] = -game->screenBound[0];
	if(pos[0] >  game->screenBound[0])
		pos[0] =  game->screenBound[0];
}
