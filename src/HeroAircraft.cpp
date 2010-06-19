/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HeroAircraft.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Config.h"

#include "define.h"
#include "extern.h"
#include "Global.h"
#include "HeroAmmo.h"
#include "Explosions.h"
#include "EnemyFleet.h"
#include "EnemyAircraft.h"
#include "PowerUps.h"
#include "Audio.h"
#include "StatusDisplay.h"
#include "Image.h"

//====================================================================
HeroAircraft::HeroAircraft()
	: ScreenItem(ItemHero)
{
	game = Global::getInstance();
	
	size[0] = 0.7;
	size[1] = 0.85;
	
	superBomb	= 0;
	dontShow	= 0;
	lives		= 4;
	score		= 0.0;
	
	bound[0][0] = -10.0;
	bound[0][1] =  10.0;
	bound[1][0] = -7.5;
	bound[1][1] =  7.5;
	
	scoreStep = 50000.0;
	scoreTarget = scoreStep;
	
	currentItemIndex = 0;
	useItemArmed	= 0.0;
	
	loadTextures();
	
	reset();
}

HeroAircraft::~HeroAircraft()
{
	deleteTextures();
}

//----------------------------------------------------------
void HeroAircraft::loadTextures()
{
	heroTex = Image::load(dataLoc("png/hero.png"));
	bombTex = Image::load(dataLoc("png/superBomb.png"));
}

//----------------------------------------------------------
void HeroAircraft::deleteTextures()
{
	glDeleteTextures(1, &heroTex);	
	glDeleteTextures(1, &bombTex);	
	heroTex = 0;
	bombTex = 0;
}

//----------------------------------------------------------
float *HeroAircraft::getPos()
{
	epos[0] =  pos[0];
	epos[1] =  pos[1];
	epos[2] =  pos[2];
	return epos; 
}

//----------------------------------------------------------
void HeroAircraft::newGame()
{
	score = 0;
	superBomb = 0;
	reset();
}

//----------------------------------------------------------
void HeroAircraft::reset()
{
	pos[0] =  0.0;
	pos[1] = -3.0f;
	pos[2] = HERO_Z;
	
	dontShow	= 0;
	damage = HERO_DAMAGE;
	shields = HERO_SHIELDS;
	
	currentItemIndex = 0;
	
	secondaryMove[0] = secondaryMove[1] = 0.0;
	
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		gunPause[i]	= -1;
		ammoStock[i]	= 0;
//		ammoStock[i]	= AMMO_REFILL;
		gunActive[i]	= false;
		gunFlash0[i]		= 0.0;
		gunFlash1[i]		= 0.0;
	}
	gunTrigger = false;
	gunSwap = false;
}

//----------------------------------------------------------
void HeroAircraft::fullHealth()
{
	damage = HERO_DAMAGE;
	shields = HERO_SHIELDS;
	
	float p0[3] = {10.4,-8.30, 25.0 };
	float v0[3] = { 0.0, 0.08, 0.0 };
	float clr[4] = { 1.0, 1.0, 1.0, 1.0 };
	p0[0] = -10.4;
	game->explosions->addElectric(p0, v0, clr,  0);
	game->explosions->addElectric(p0, v0, clr,  0);
	game->explosions->addElectric(p0, v0, clr, -1);
	game->explosions->addElectric(p0, v0, clr, -3);
	game->explosions->addElectric(p0, v0, clr, -4);
	p0[0] = 10.4;
	game->explosions->addElectric(p0, v0, clr,  0);
	game->explosions->addElectric(p0, v0, clr,  0);
	game->explosions->addElectric(p0, v0, clr, -1);
	game->explosions->addElectric(p0, v0, clr, -3);
	game->explosions->addElectric(p0, v0, clr, -4);
	
	secondaryMove[0] = secondaryMove[1] = 0.0;
	
	gunTrigger = false;
	gunSwap = false;
}

//----------------------------------------------------------
void HeroAircraft::addScore(float in)
{
	if(game->gameMode != Global::HeroDead)
		score += in;
}

//----------------------------------------------------------
void HeroAircraft::addLife(bool score)
{
	float	p[3] = { 10.2, 0.0, 25.0 };
	p[1] = 7.4-lives*game->hero->size[1];
	game->audio->playSound(Audio::AddLife, p);
	game->explosions->addExplo(Explosions::AddLife, p);
	game->explosions->addExplo(Explosions::AddLife, p, -10);
	game->explosions->addExplo(Explosions::AddLife, p, -13);
	if(score)
	{
		p[0] = -7.9;
		p[1] = -8.0;
		game->audio->playSound(Audio::AddLife, p);
		game->explosions->addExplo(Explosions::PowerBurst, p);
		game->explosions->addExplo(Explosions::ScoreLife, p,  0);
		game->explosions->addExplo(Explosions::ScoreLife, p, -3);
		game->explosions->addExplo(Explosions::ScoreLife, p, -9);
		game->explosions->addExplo(Explosions::ScoreLife, p, -15);
	}
	if(lives < 9)
		lives++;
	else
		superBomb = 1;
}

//----------------------------------------------------------
void HeroAircraft::loseLife()
{
	float	p[3] = { 10.2, 0.0, 25.0 };
	//-- this has to be in sync w/ StatusDisplay positions...
	lives--;
	p[1] = 7.4-lives*game->hero->size[1];
	
	if(lives > -2)
	{
		game->audio->playSound(Audio::LoseLife, p);
		game->explosions->addExplo(Explosions::LoseLife, p,   0, 1.5);
		game->explosions->addExplo(Explosions::LoseLife, p, -10, 1.5);
		game->explosions->addExplo(Explosions::LoseLife, p, -13, 1.5);
		game->explosions->addExplo(Explosions::PowerBurst, p);
	}
	if(lives < 0 && game->gameMode != Global::HeroDead)
	{
		damage = 0;
		shields = 0;
		startDeath();
	}
}

//----------------------------------------------------------
void HeroAircraft::setLives(int in)
{
	if(in > 9)
		in = 9;
	lives = in;
}

//----------------------------------------------------------
void HeroAircraft::useItem(int index)
{
	if(currentItemIndex == index)
	{
		useItem();
	}
	else
	{
		if(index < NUM_HERO_ITEMS && index >= 0)
		{
			currentItemIndex = index;
			useItemArmed = 0.0;
			useItem();
		}
	}
}

//----------------------------------------------------------
void HeroAircraft::useItem()
{
	if(game->gameMode == Global::Game && !superBomb && !game->game_pause)
	{
		if(!useItemArmed)
		{
			useItemArmed = 1.0;
		}
		else
		{
			useItemArmed = 0.0;
			float v[3] = { 0.0, 0.7, 0.0 };
			PowerUp *pwrUp;
			int i;
			switch(currentItemIndex)
			{
				case 0: // self destruct
					for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
					{
						//eject all ammo - return remaining ammo to game->powerUps
						if(ammoStock[i] > 1.0)
						{
							v[0] = SRAND*0.15;
							v[1] = 0.1+FRAND*0.1;
							pwrUp = new PowerUp( (PowerUps::Type)(i+(int)PowerUps::HeroAmmo00), 
								pos, (ammoStock[i]/AMMO_REFILL), v);
							game->powerUps->addPowerUp(pwrUp);
						}
					}
					damage = 0.0;
					shields = 0.0;
					lives--;
					startDeath();
					break;
				case 1:
					break;
				default:
					break;
			}
		}
	}
}

//----------------------------------------------------------
void HeroAircraft::moveEvent(int x, int y)
{
	Config	*config = Config::instance();
	if(game->gameMode != Global::HeroDead && !game->game_pause)
	{
		pos[0] +=  x*config->mouseSpeed();
		pos[1] += -y*config->mouseSpeed();
		if		(pos[0] < bound[0][0])	pos[0] = bound[0][0];
		else if	(pos[0] > bound[0][1])	pos[0] = bound[0][1];
		if		(pos[1] < bound[1][0])	pos[1] = bound[1][0];
		else if	(pos[1] > bound[1][1])	pos[1] = bound[1][1];
	}
}

//----------------------------------------------------------
void HeroAircraft::ammoDamage(float d, float vec[3])
{
	if(superBomb)
		return;
	float f = d/50.0;
	secondaryMove[0] =  vec[0]*f;
	secondaryMove[1] =  vec[1]*f;
	float a = 2.0;
	pos[0] += vec[0]*f*a;
	pos[1] += vec[1]*f*a;
	pos[2] += vec[2]*f*a;
	
	doDamage(d);
}

//----------------------------------------------------------
void HeroAircraft::doDamage(float d)
{
	if(superBomb)
		return;
	
	if(shields > HERO_SHIELDS)
	{
		shields	-= d*0.25;
		game->explosions->addExplo(Explosions::HeroShields, pos);
		game->statusDisplay->setShieldAlpha(1.0);
	}
	else if(shields > 0.0)
	{
		shields	-= d*0.8;
		damage	+= d*0.2;
		if(shields < 0.0)
			shields = 0.0;
		game->explosions->addExplo(Explosions::HeroShields, pos);
		game->statusDisplay->setShieldAlpha(1.0);
		game->statusDisplay->setDamageAlpha(1.0);
	}
	else
	{
		damage += d;
		game->statusDisplay->setDamageAlpha(1.0);
	}

	if(damage > 0.0)
	{
		damage = 0.0;
		lives--;
		startDeath();
	}
}

//----------------------------------------------------------
void HeroAircraft::fireGun(bool status)
{
	if(dontShow)
		return;
	if(status && game->gameMode != Global::HeroDead)
	{
		gunTrigger = true;
		gunPause[0] = 0;
		if(gunActive[1])
			if(gunPause[1] < 0)
				gunPause[1] = 0;
		if(gunActive[2])
			if(gunPause[2] < 0)
				gunPause[2] = 0;
	}
	else
	{
		gunTrigger = false;
		gunPause[0] = -1;
	}
}

//----------------------------------------------------------
void HeroAircraft::shootGun()
{
	float p[3] = { 0.0, 0.0, pos[2] };
	
	if( gunPause[0] <= 0) 
	{
		gunPause[0] = 5;
		
		p[0] = pos[0]+0.3;
		p[1] = pos[1]+0.8;
		game->heroAmmo->addAmmo(0, p);
		p[0] = pos[0]-0.3;
		game->heroAmmo->addAmmo(0, p);
		if(gunActive[0])
		{
			p[0] = pos[0]+0.45;
			p[1] = pos[1]+0.2;
			game->heroAmmo->addAmmo(0, p);
			p[0] = pos[0]-0.45;
			game->heroAmmo->addAmmo(0, p);
			ammoStock[0] -= 0.5;
		}
		
		if(currentItemIndex == 1 && useItemArmed) // double fire
		{
			p[0] = pos[0]+0.37;
			p[1] = pos[1]+0.6;
			game->heroAmmo->addAmmo(0, p);
			p[0]  = pos[0]-0.37;
			game->heroAmmo->addAmmo(0, p);
			if(gunActive[0])
			{
				p[0]  = pos[0]+0.52;
				p[1]  = pos[1];
				game->heroAmmo->addAmmo(0, p);
				p[0]  = pos[0]-0.52; 
				game->heroAmmo->addAmmo(0, p);
				ammoStock[0] -= 0.75;
			}
		}
	}
	if(gunActive[1] && gunPause[1] <= 0.0)
	{
		gunPause[1] = 25;
		p[0] = pos[0];
		p[1] = pos[1] + 1.1;
		game->heroAmmo->addAmmo(1, p);
		ammoStock[1] -= 1.5;
		if(currentItemIndex == 1 && useItemArmed) // double fire
		{
			p[1] -= 0.2;
			p[0] = pos[0]+0.09;
			game->heroAmmo->addAmmo(1, p);
			p[0] = pos[0]-0.09;
			game->heroAmmo->addAmmo(1, p);
			ammoStock[1] -= 4.5;
		}
	}
	if(gunActive[2] && gunPause[2] <= 0.0)
	{
		gunSwap = !gunSwap;
		gunPause[2] = 5;
		p[1] = pos[1]+0.4;
		if(gunSwap)
		{
			p[0] = pos[0]+0.7;
			game->heroAmmo->addAmmo(2, p);
		}
		else
		{
			p[0] = pos[0]-0.7;
			game->heroAmmo->addAmmo(2, p);
		}
		ammoStock[2] -= 1.5;
		
		if(currentItemIndex == 1 && useItemArmed) // double fire
		{
			if(gunSwap)
			{
				p[0] = pos[0]+0.85;
				p[1] -= 0.75;
				game->heroAmmo->addAmmo(2, p);
			}
			else
			{
				p[0] = pos[0]-0.85;
				p[1] -= 0.75;
				game->heroAmmo->addAmmo(2, p);
			}
			ammoStock[2] -= 3.5;
		}
	}
	
	//-- clean up gun active
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
		if(ammoStock[i] < 0.0)
		{
			ammoStock[i] = 0.0;
			gunActive[i] = false;
		} 
}

//----------------------------------------------------------
void	HeroAircraft::setAmmoStock(int index, float value)
{
	if(index >= 0 && index < NUM_HERO_AMMO_TYPES)
	{
		game->statusDisplay->setAmmoAlpha(1.0);
		gunPause[index] = 1;
		gunActive[index] = true;
		ammoStock[index] = value;
	}
}

//----------------------------------------------------------
void HeroAircraft::dropBomb()
{
}

//----------------------------------------------------------
void HeroAircraft::checkForCollisions(EnemyFleet *fleet)
{
	float	p[3];
	float	r1,r2;
	float	diffX, diffY;
	float	dist;
	float	power;
	EnemyAircraft *enemy;
	
	fleet->toFirst();
	while( (enemy = fleet->getShip()) )
	{
		diffX = pos[0]-enemy->pos[0];
		diffY = pos[1]-enemy->pos[1];
		dist = fabs(diffX) + fabs(diffY);
		if(!dontShow && dist < enemy->size[0]+size[0])
		{
			//-- damage
			power = -enemy->damage*0.5;
			if(power > 35.0)
				power = 35.0;
			doDamage(power);
			if(shields > HERO_SHIELDS)
			{
				power*=0.5;	// reduce secondary movement when super shields are enabled
				enemy->damage += 70.0;
			}
			else
				enemy->damage += 40.0; // normal collision
				
			//-- explosions
			r1 = SRAND*0.3;
			r2 = SRAND*0.4;
			p[0] = enemy->pos[0]+r1;
			p[1] = enemy->pos[1]+r2;
			p[2] = enemy->pos[2];
			game->explosions->addExplo(Explosions::EnemyDamage, p);
			p[0] = pos[0]+r1;
			p[1] = pos[1]+0.2+r2;
			p[2] = pos[2];
			if(shields > 0.0)
				game->explosions->addExplo(Explosions::HeroShields, p);
			else
				game->explosions->addExplo(Explosions::HeroDamage, p);
			
			secondaryMove[0] =  diffX*power*0.03;
			secondaryMove[1] =  diffY*power*0.03;
			enemy->secondaryMove[0] -= diffX* enemy->collisionMove;
			enemy->secondaryMove[1] -= diffY*(enemy->collisionMove*0.5);
			
		}
		if(superBomb)
		{
			diffX = -enemy->pos[0];
			diffY = -15.0-enemy->pos[1];
			float dist = sqrt(diffX*diffX + diffY*diffY);
			if( (dist < superBomb*0.1 && enemy->type < EnemyBoss00) || 
				(enemy->pos[1] < -11.0) )
			{
				enemy->damage += 5000.0;
			}
			if(superBomb > 300)
				superBomb = 0;
		}
	}
	if(superBomb)
		superBomb += 2;
}

//----------------------------------------------------------
void HeroAircraft::checkForPowerUps(PowerUps *powerUps)
{
	if(dontShow)
		return;
	float	dist;
	float	stock;
	PowerUp *pwrUp;
	PowerUp *delUp;
	
	if(score > scoreTarget)
	{
		scoreTarget += scoreStep;
		addLife(true);
	}
	
	float p0[3] = {10.4,-8.30, 25.0 };
	float v0[3] = { 0.0, 0.08, 0.0 };
	float clr[4] = { 1.0, 1.0, 1.0, 1.0 };
	if(game->gameMode == Global::Game)
		pwrUp = powerUps->getFirst();
	else
		pwrUp = 0;
	while( pwrUp )
	{
		dist = fabs(pos[0]-pwrUp->pos[0]) + fabs(pos[1]-pwrUp->pos[1]);
		if(dist < size[1])
		{
			game->audio->playSound(Audio::PowerUp, pos);
			switch(pwrUp->type)
			{
				case PowerUps::Shields:
					if(shields < HERO_SHIELDS)
						shields = HERO_SHIELDS;
					game->statusDisplay->setShieldAlpha(5.0);
					p0[0] = -10.4;
					game->explosions->addElectric(p0, v0, clr,  0);
					game->explosions->addElectric(p0, v0, clr, -1);
					game->explosions->addElectric(p0, v0, clr, -3);
					game->explosions->addElectric(p0, v0, clr, -4);
					break;
				case PowerUps::SuperShields:
					game->tipSuperShield++;
					damage -= shields;
					if(damage < HERO_DAMAGE)
						damage = HERO_DAMAGE;
					shields = HERO_SHIELDS*2;
					game->statusDisplay->setDamageAlpha(5.0);
					game->statusDisplay->setShieldAlpha(5.0);
					p0[0] = -10.4;
					game->explosions->addElectric(p0, v0, clr,  0);
					game->explosions->addElectric(p0, v0, clr, -1);
					game->explosions->addElectric(p0, v0, clr, -3);
					game->explosions->addElectric(p0, v0, clr, -4);
					game->explosions->addElectric(p0, v0, clr, -10);
					game->explosions->addElectric(p0, v0, clr, -11);
					game->explosions->addElectric(p0, v0, clr, -13);
					game->explosions->addElectric(p0, v0, clr, -14);
					p0[0] = 10.4;
					game->explosions->addElectric(p0, v0, clr,  0);
					game->explosions->addElectric(p0, v0, clr, -1);
					game->explosions->addElectric(p0, v0, clr, -3);
					game->explosions->addElectric(p0, v0, clr, -4);
					game->explosions->addElectric(p0, v0, clr, -10);
					game->explosions->addElectric(p0, v0, clr, -11);
					game->explosions->addElectric(p0, v0, clr, -13);
					game->explosions->addElectric(p0, v0, clr, -14);
					break;
				case PowerUps::Repair:
					game->statusDisplay->setDamageAlpha(5.0);
					damage = HERO_DAMAGE;
					p0[0] = 10.4;
					game->explosions->addElectric(p0, v0, clr,  0);
					game->explosions->addElectric(p0, v0, clr, -1);
					game->explosions->addElectric(p0, v0, clr, -3);
					game->explosions->addElectric(p0, v0, clr, -4);
					break;
				case PowerUps::HeroAmmo00:
					addScore(100.0);
					stock = ammoStock[0] + pwrUp->power*AMMO_REFILL;
					if(stock > AMMO_REFILL)
						stock = AMMO_REFILL;
					setAmmoStock(0, stock);
					break;
				case PowerUps::HeroAmmo01:
					addScore(100.0);
					stock = ammoStock[1] + pwrUp->power*AMMO_REFILL;
					if(stock > AMMO_REFILL)
						stock = AMMO_REFILL;
					setAmmoStock(1, stock);
					break;
				case PowerUps::HeroAmmo02:
					addScore(100.0);
					stock = ammoStock[2] + pwrUp->power*AMMO_REFILL;
					if(stock > AMMO_REFILL)
						stock = AMMO_REFILL;
					setAmmoStock(2, stock);
					break;
				default:
					break;
			}
			game->explosions->addExplo(Explosions::PowerBurst, pwrUp->pos);
			delUp = pwrUp;
			pwrUp = pwrUp->next;
			powerUps->remove(delUp);
		}
		else
		{
			pwrUp = pwrUp->next;
		}
	}
}

//----------------------------------------------------------
void HeroAircraft::update()
{
	if(dontShow > 1)
	{
		pos[0] =  		cos(game->frame*0.02) * 9.0;
		pos[1] =  4.0 + sin(game->frame*0.07) * 2.0;
	}
	else if(dontShow == 1)
	{
		pos[0] =  0.0f;
		pos[1] = -3.0f;
	}
	
	//-- Gun flashes are drawn in StatusDisplay
	if(gunTrigger)
		shootGun();
	for(int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		if(gunPause[i] >= 0)
			gunPause[i] -= game->speedAdj;
		if(gunTrigger)
		{
			float flash;
			float pause;
			switch(i)
			{
				case 0:
					flash = 5.0/game->speedAdj;
					pause = gunPause[i]/game->speedAdj;
					gunFlash0[i] = (flash-pause)/flash;
					if(gunActive[i])
						gunFlash1[i] = (flash-pause)/flash;
					else
						gunFlash1[i] = 0.0;
					break;
				case 1:
					flash = 10.0/game->speedAdj;
					pause = gunPause[i]/game->speedAdj;
					if(gunActive[i] && gunPause[i] < flash)
						gunFlash0[i] = (flash-pause)/flash;
					else
						gunFlash0[i] = 0.0;
					break;
				case 2:
					flash = 5.0/game->speedAdj;
					pause = gunPause[i]/game->speedAdj;
					if(gunActive[i])
					{
						if(gunPause[i] < flash)
						{
							if(gunSwap)
							{
								gunFlash0[i] = (flash-pause)/flash;
								gunFlash1[i] = 0.0;
							}
							else
							{
								gunFlash0[i] = 0.0;
								gunFlash1[i] = (flash-pause)/flash;
							}
						}
					}
					else
					{
						gunFlash0[i] = 0.0;
						gunFlash1[i] = 0.0;
					}
					break;
			}
		}
		else
		{
			if(gunFlash0[i] > 0.0)	gunFlash0[i] -= 0.075*game->speedAdj;
			else	gunFlash0[i] = 0.0;
			
			if(gunFlash1[i] > 0.0)	gunFlash1[i] -= 0.075*game->speedAdj;
			else	gunFlash1[i] = 0.0;
		}
	}
	
	//-- decrement item activation
	switch(currentItemIndex)
	{
		case 0: // self destruct
			useItemArmed -= 0.02;
			break;
		case 1:
			if(useItemArmed)
				doDamage(1);
			break;
	}
	if(useItemArmed < 0.0)
		useItemArmed = 0.0;
	
	//-- decrement supershields
	if(shields >= HERO_SHIELDS)
	{
		shields -= 0.15*game->speedAdj;
		
	}
	
	float s = (1.0-game->speedAdj)+(game->speedAdj*0.8);
	secondaryMove[0] *= s;
	secondaryMove[1] *= s;
	pos[0] += secondaryMove[0]*game->speedAdj;
	pos[1] += secondaryMove[1]*game->speedAdj;
	moveEvent(0,0);
	
}

//----------------------------------------------------------
void HeroAircraft::drawGL()
{
	//-- draw hero
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	if(!dontShow)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, heroTex);
		drawQuad(size[0], size[1]);
	}
	else
	{
		dontShow--;
	}
	//-- draw super shields in StatusDisplay to get better blend mode...	
	glPopMatrix();

	if(superBomb)
	{
		float s = superBomb*0.1;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, bombTex);
		glPushMatrix();
		glTranslatef(0.0, -15.0, HERO_Z);
		glRotatef(IRAND, 0.0, 0.0, 1.0);
		drawQuad(s,s);
		glPopMatrix();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

//----------------------------------------------------------
void HeroAircraft::startDeath()
{
	deathExplosions();
	if(lives < 0)
	{
		fireGun(false);
		game->heroDeath = DEATH_TIME;
		game->gameMode = Global::HeroDead;
		dontShow = -1;
	}
	else
	{
		superBomb = 1;
		reset();
		dontShow = 130;
	}
}

//----------------------------------------------------------
void HeroAircraft::deathExplosions()
{
	int		i;
	float	r;
	float	p[3] = { 0.0, -0.5, pos[2] };
	int		w, skip;
	
	deathStereo = 5.0;
	p[0] = -deathStereo;
	game->audio->playSound(Audio::Explosion, p);
	game->audio->playSound(Audio::Explosion, p, -45);
	p[0] =  deathStereo;
	game->audio->playSound(Audio::Explosion, p);
	game->audio->playSound(Audio::Explosion, p, -20);
	p[0] =  0.0;
	game->audio->playSound(Audio::ExploBig, p);
	game->audio->playSound(Audio::ExploPop, p);
	
	//-- Caclulate radius
	for(i = 0; i < DEATH_SPIKES; i++)
	{
		double rnd = FRAND;
		double a = rnd+2.0*M_PI*((double)i/(double)DEATH_SPIKES);
		double s = 0.5+0.5*FRAND;
		deathCircle[i][0] = sin(a)*s;
		deathCircle[i][1] = cos(a)*s;
		p[0] = pos[0]+deathCircle[i][0];
		p[1] = pos[1]+deathCircle[i][1];
		p[2] = pos[2];
		game->explosions->addExplo(Explosions::EnemyDestroyed, p);
	}
	
	//-- Set up explosions
	for(i = 0; i < DEATH_TIME; i++)
	{
		w = i%DEATH_SPIKES; 
		skip = 1+(int)(3.0f*FRAND);
		
		p[2] = pos[2];
		if(i < DEATH_TIME-DEATH_SPIKES)
		{
			if( !(i%skip) )
			{
				r = 1.25+4.0*((float)i/(float)DEATH_TIME);
				p[0] = pos[0]+deathCircle[w][0]*r;
				p[1] = pos[1]+deathCircle[w][1]*r;
				game->explosions->addExplo(Explosions::EnemyDestroyed, p, -i);
			}
		}
		else
		{
			p[0] = pos[0]+deathCircle[w][0];
			p[1] = pos[1]+deathCircle[w][1];
			game->explosions->addExplo(Explosions::EnemyDestroyed, p, -i);
		}
		if(!(i%21))
		{
			deathStereo = -deathStereo*1.5;
			p[0] = deathStereo;
			p[1] = -5.0;
			game->audio->playSound(Audio::Explosion, p, -i);
		}
	}
}







