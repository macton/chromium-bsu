/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"

#include "ScreenItemAdd.h"

#include <cstdio>
#include <cstdlib>

#include "define.h"
#include "Global.h"
#include "EnemyFleet.h"
#include "PowerUps.h"

//==============================================================================
ScreenItemAdd::ScreenItemAdd()
{
	game = Global::getInstance();
	
	root = new ItemThing();
	
	float p[3] = { 0.0, 0.0, 0.0 };
	for(int i = 0; i < NumEnemyTypes; i++)
	{
//		deadPool[i] = new EnemyAircraft((EnemyType)i, p);
		deadPool[i] = EnemyAircraft::makeNewEnemy((EnemyType)i, p);
		deadPool[i]->next = 0;
	}
}

ScreenItemAdd::~ScreenItemAdd()
{
	clear();
	delete root;
}

//----------------------------------------------------------	
void ScreenItemAdd::clear()
{
	ItemThing	*cur;
	ItemThing *del;
	
	cur = root->next;
	while(cur)
	{
		delete cur->item;
		del = cur;
		cur = cur->next;
		delete del;
	}
	root->next = 0;
	clearDeadPool();
}
	
//----------------------------------------------------------	
void ScreenItemAdd::clearDeadPool()
{
	EnemyAircraft *cur;
	EnemyAircraft *del;
	for(int i = 0; i < NumEnemyTypes; i++)
	{
		cur = deadPool[i]->next;
		while(cur)
		{
			del = cur;
			cur = cur->next;
			delete del;
		}
		deadPool[i]->next = 0;
	}
}
	
//----------------------------------------------------------	
void ScreenItemAdd::putScreenItems()
{
//	ItemThing *delItem = 0;
	ItemThing *curItem = root->next;
	
	while(curItem)
	{
		if(curItem->releaseTime <= game->gameFrame)
		{
			switch(curItem->item->itemType())
			{
				case ScreenItem::ItemEnemy:
					game->enemyFleet->addEnemy( (EnemyAircraft*)(curItem->item) );
					break;
				case ScreenItem::ItemPowerUp:
					game->powerUps->addPowerUp( (PowerUp*)(curItem->item) );
					break;
				case ScreenItem::ItemHero:
					fprintf(stderr, _("ScreenItemAdd::putScreenItems() Hero??? HUH???\n"));
					break;
			}
			//delItem = curItem;
			curItem = curItem->next;
			root->next = curItem;
		}
		else
			curItem = 0;
	}
}

//----------------------------------------------------------	
void ScreenItemAdd::killScreenItem(ScreenItem *del)
{
	EnemyAircraft *enemy;
	switch(del->itemType())
	{
		case ScreenItem::ItemEnemy: // add enemy to dead pool
			enemy = (EnemyAircraft*)del;
			enemy->back = 0;
			enemy->next = deadPool[enemy->type]->next;
			deadPool[enemy->type]->next = enemy;
			break;
		default:
			delete del;
			break;
	}
}

//----------------------------------------------------------	
bool ScreenItemAdd::loadScreenItems(const char*)
{
	switch((game->gameLevel-1)%3)
	{
		case 0:
			loadLevel1();
			break;
		case 1:
			loadLevel2();
			break;
		case 2:
			loadLevel3();
			break;
		default:
			loadLevel1();
			break;
	}
	return true;
}

//----------------------------------------------------------	
void ScreenItemAdd::addItem(int relTime, ScreenItem *newScreenItem)
{
	ItemThing *curItem = root;
	ItemThing *newItem = new ItemThing(relTime, newScreenItem);
	
	while(curItem)
	{
		if(curItem->next == 0)
		{
			curItem->next = newItem;
			break;
		}
		if(relTime < curItem->next->releaseTime)
		{
			newItem->next = curItem->next;
			curItem->next = newItem;
			break;
		}
		curItem = curItem->next;
	}
}


//----------------------------------------------------------	
void ScreenItemAdd::addWave(EnemyWave &ew)
{
	int		i;
	int		interval = 1;
	int		iteration = 0;
	float	p[3] = { ew.pos[0], ew.pos[1], ew.pos[2] };
	int	jitter = (int)(ew.jitter * (2.0-game->gameSkill));
	int	period = (int)(ew.period * (2.0-game->gameSkill));
	
	if(jitter >= period)
		jitter = period - 1;
	for(i = ew.begin; i < ew.end; i++)
	{
		interval--;
		if(interval < 1)
		{
			switch(ew.formation)
			{
				case EnemyWave::None:
					p[0] = ew.pos[0] + ew.xJitter*SRAND;
//					addItem(i, new EnemyAircraft(ew.type, p) );	
					addItem(i, EnemyAircraft::makeNewEnemy(ew.type, p) );	
					interval = period + (int)(SRAND*jitter);
					break;
				case EnemyWave::Arrow:
					p[0] = ew.pos[0] + ew.xJitter*iteration;
//					addItem(i, new EnemyAircraft(ew.type, p, 0.0) );
					addItem(i, EnemyAircraft::makeNewEnemy(ew.type, p, 0.0) );
					if(iteration > 0)
					{
						p[0] = ew.pos[0] - ew.xJitter*iteration;
//						addItem(i, new EnemyAircraft(ew.type, p, 0.0) );
						addItem(i, EnemyAircraft::makeNewEnemy(ew.type, p, 0.0) );
					}
					interval = (int)period + (int)(SRAND*jitter);
					iteration++;
					break;
				default:
					p[0] = 0.0;
//					addItem(i, new EnemyAircraft(ew.type, p, 0.0) );
					addItem(i, EnemyAircraft::makeNewEnemy(ew.type, p, 0.0) );
					interval = 25;
					break;
			}
		}
	}
}

//----------------------------------------------------------	
EnemyAircraft *ScreenItemAdd::dynamicEnemyAdd(EnemyType et, float *pos, int relTime)
{
	EnemyAircraft *addEnemy;
	
	if(deadPool[et]->next)
	{
		addEnemy = deadPool[et]->next;
		deadPool[et]->next = addEnemy->next;
		addEnemy->init(pos);
	}
	else
	{
//		addEnemy = new EnemyAircraft(et, pos);
		addEnemy = EnemyAircraft::makeNewEnemy(et, pos);
	}
	
	addItem(relTime, addEnemy);
	
	return 	addEnemy;
}

//----------------------------------------------------------	
void ScreenItemAdd::loadLevelXXX()
{
	int i;
	int	numIterations = 12000;
//	int	numIterations = (int)(11100 * game->gameSkill);
//	int	numIterations = (int)(1100 * game->gameSkill);
	
	clearDeadPool();
	//-- Enemies
	float	r;
	float	d;
	int		waveDuration = 500;	
	i = 600;
	addStraightWave(1, i, 0.4);
	while(i < numIterations-1000)
	{
		if(i < 1500)
			d = (i+250.0)/2000.0;
		else
			d = 1.0;
		r = FRAND;
		if      (r < 0.15)	addStraightArrowWave(i, waveDuration, d);
		else if (r < 0.25)	addOmniArrowWave(i, waveDuration, d);
		else if (r > 0.60)	addStraightWave(i, waveDuration, d);
		else				addOmniWave(i, waveDuration, d);

		i += waveDuration;
		waveDuration = (int)(600.0*game->gameSkill) + (int)(100*SRAND);
		i += 50 + (int)(50*FRAND);
	}
	
	//-- ray gun enemy starts halfway through...
	EnemyWave	rayWave(EnemyRayGun);
	rayWave.setXRand(8.0);
	rayWave.setFrequency(60, 5);	
	rayWave.setFrequency(2000, 1000);
	rayWave.setInOut(numIterations/2, i-1000);
//	rayWave.setInOut(100, i-1000);
	addWave(rayWave);
	
	//-- Boss
	EnemyWave	bossWave(EnemyBoss00);
	bossWave.setInOut(i+75, i+1000);	
	bossWave.setPos(0.0, 15.0);
	bossWave.setXRand(4.0);
	bossWave.setFrequency(5000, 0);
	addWave(bossWave);
		
	//-- Ammunition and PowerUps
	addAmmunition(0, numIterations+9000);
	addPowerUps(0, numIterations+9000);
}

//----------------------------------------------------------	
void ScreenItemAdd::loadLevel1()
{
	loadLevelXXX();
	
//	addStraightArrowWave(051, 800);
//	addOmniArrowWave(800, 1400);
//	addStraightArrowWave(1400, 2000);
//	addOmniArrowWave(2000, 2600);
//	
//
//	addAmmunition(0, 10000);
//	addAmmunition(0, 10000);
//	addPowerUps(0, 10000);
//
//	//-- Boss
//	EnemyWave	bossWave(EnemyBoss00);
//	bossWave.setInOut(100, 1000);	
//	bossWave.setPos(0.0, 15.0);
//	bossWave.setXRand(4.0);
//	bossWave.setFrequency(5000, 0);
//	addWave(bossWave);
//	
//	EnemyWave	tankWave(EnemyTank);
//	tankWave.setPos(9.0, 11.0);
//	tankWave.setXRand(0.0);
//	tankWave.setFrequency(4000, 50);
//	tankWave.setInOut(1, 10000);
//	addWave(tankWave);
//	
//	tankWave.setPos(-9.0, 11.0);
//	tankWave.setXRand(0.0);
//	tankWave.setFrequency(4000, 50);
//	tankWave.setInOut(2001, 10000);
//	addWave(tankWave);
//	
//	
//	EnemyWave	gnatWave(EnemyGnat);
//	gnatWave.setInOut(1, 500);
//	gnatWave.setPos(FRAND*4.0, 10.0);
//	gnatWave.setFrequency(50, 40);
//	gnatWave.setXRand(5.0);
//	addWave(gnatWave);
//	
//	addAmmunition(5000);
//	addPowerUps(5000);
}

//----------------------------------------------------------	
void ScreenItemAdd::loadLevel2()
{
	int i;
	int	numIterations = 14000;
//	int	numIterations = 1400;
	
	clearDeadPool();
	//-- Enemies
	int		waves = 0;
	float	r;
	int		waveDuration = 500;	
	i = 500;
	addStraightWave(100, i, 0.4);
	while(i < numIterations)
	{
		r = FRAND;
		waves++;
		
		if( waves == 5  || waves == 6  || 
			waves == 11 || waves == 12 ||
			waves == 15 || waves == 16 )
		{
			addGnatWave(i, waveDuration, 1.0, false);
		}
		else if (waves < 5)
		{
			if     (r < 0.20)	addStraightArrowWave(i, waveDuration);
			else if(r < 0.30)	addOmniArrowWave	(i, waveDuration);
			else if(r < 0.60)	addOmniWave			(i, waveDuration);
			else				addStraightWave		(i, waveDuration);
		}
		else
		{
			if     (r < 0.25) 	addGnatWave			(i, waveDuration);
			else if(r < 0.35)	addStraightArrowWave(i, waveDuration);
			else if(r < 0.50)	addOmniArrowWave	(i, waveDuration);
			else if(r < 0.80)	addOmniWave			(i, waveDuration);
			else				addStraightWave		(i, waveDuration);
		}
		i += waveDuration;
		waveDuration = (int)(700.0 + 100.0*SRAND);
		i += 50 + (int)(50*FRAND);
	}
	EnemyWave	gnatWave(EnemyGnat);
	gnatWave.setInOut(2200, 5000);
	gnatWave.setPos(FRAND*4.0, 10.0);
	gnatWave.setFrequency(150, 140);
	gnatWave.setXRand(5.0);
	addWave(gnatWave);
	
//	gnatWave.setInOut(8000, 11000);
		
	//-- Boss
	EnemyWave	bossWave(EnemyBoss01);
	bossWave.setPos(0.0, 15.0);
	bossWave.setXRand(4.0);
	bossWave.setFrequency(5000, 0);
	bossWave.setInOut(numIterations+700, numIterations+710);	
	addWave(bossWave);
	
	//-- Ammunition and PowerUps
	addAmmunition(0, numIterations+9000);
	addPowerUps(0, numIterations+9000);
	
}

//----------------------------------------------------------	
void ScreenItemAdd::loadLevel3()
{
	int i;
	int	numIterations = 14000;
//	int	numIterations = 1400;
	
	clearDeadPool();
	//-- Enemies
	int		waves = 0;
	float	r;
	int		waveDuration = 500;	
	i = 500;
	addStraightWave(100, i, 0.5);
	while(i < numIterations)
	{
		r = FRAND;
		waves++;
		
		if( waves ==  5 || 
			waves == 12 )
		{
			addGnatWave(i, waveDuration, 0.9, false);
		}
		else if (	waves ==  6 ||
					waves == 11 ||
					waves == 15 || waves == 16 )
		{
			EnemyWave	tankWave(EnemyTank);
			tankWave.setPos(0.0, 11.0);
			tankWave.setXRand(10.0);
			tankWave.setFrequency(700, 100);
			tankWave.setInOut(i+50, waveDuration-50);
			addWave(tankWave);
			
			addStraightWave		(i, 300);
		}
		else if (waves < 5)
		{
			if     (r < 0.20)	addStraightArrowWave(i, waveDuration);
			else if(r < 0.30)	addOmniArrowWave	(i, waveDuration);
			else if(r < 0.60)	addOmniWave			(i, waveDuration);
			else				addStraightWave		(i, waveDuration);
		}
		else
		{
			if     (r < 0.25) 	addGnatWave			(i, waveDuration);
			else if(r < 0.35)	addStraightArrowWave(i, waveDuration);
			else if(r < 0.50)	addOmniArrowWave	(i, waveDuration);
			else if(r < 0.80)	addOmniWave			(i, waveDuration);
			else				addStraightWave		(i, waveDuration);
		}
		i += waveDuration;
		waveDuration = (int)(700.0 + 100.0*SRAND);
		i += 50 + (int)(50*FRAND);
	}
	EnemyWave	gnatWave(EnemyGnat);
	gnatWave.setInOut(3000, 5000);
	gnatWave.setPos(FRAND*4.0, 10.0);
	gnatWave.setFrequency(150, 140);
	gnatWave.setXRand(5.0);
	addWave(gnatWave);
	
	gnatWave.setInOut(8000, 11000);
	addWave(gnatWave);
	
	//-- give some extra power ups...
	addAmmunition(0, 2000, 500, 700, 0);
	addAmmunition(10000, 2000, 500, 700, 0);
	addPowerUps(3000, 2000, 2500, 2500, 0);
	addPowerUps(9500, 2000, 2500, 2500, 0);
	addPowerUps(numIterations, 2000, 2500, 2500, 1);
	
	//-- Boss
	EnemyWave	bossWave(EnemyBoss01);
	bossWave.setPos(0.0, 15.0);
	bossWave.setXRand(4.0);
	bossWave.setFrequency(5000, 0);
	bossWave.setInOut(numIterations+700, numIterations+710);	
	addWave(bossWave);
	
	EnemyWave	tankWave(EnemyTank);
	tankWave.setPos(0.0, 11.0);
	tankWave.setXRand(10.0);
	tankWave.setFrequency(2000, 200);
	tankWave.setInOut(numIterations+400, numIterations+3000);
	addWave(tankWave);
			
	//-- Ammunition and PowerUps
	addAmmunition(0, numIterations+9000);
	addPowerUps(0, numIterations+9000);
	
}

//----------------------------------------------------------	
void ScreenItemAdd::addStraightWave(int o, int duration, float density)
{
	float freq = 1.0/density;
	EnemyWave	straightWave(EnemyStraight);
	straightWave.setXRand(8.0);
	straightWave.setFrequency((int)(60*freq), 5);
	straightWave.setInOut(o, o+duration);
	addWave(straightWave);
}

//----------------------------------------------------------	
void ScreenItemAdd::addOmniWave(int o, int duration, float density)
{
	float freq = 1.0/density;
	EnemyWave	omniWave(EnemyOmni);
	omniWave.setXRand(1.0);
	omniWave.setFrequency((int)(39*freq), 7);
	omniWave.setPos((SRAND*8.0), 9.0);
	omniWave.setInOut(o, o+(duration/2)+50);
	addWave(omniWave);
	omniWave.setPos((SRAND*8.0), 9.0);
	omniWave.setInOut(o+(duration/2)-50, o+duration);
	addWave(omniWave);

	EnemyWave	straightWave(EnemyStraight);
	straightWave.setXRand(8.0);
	straightWave.setFrequency(200, 50);
	straightWave.setInOut(o+100, o+duration);
	addWave(straightWave);
}

//----------------------------------------------------------	
void ScreenItemAdd::addStraightArrowWave(int o, int /*duration*/, float density)
{
	float freq = 1.0/density;
	float c = SRAND;
	EnemyWave	straightArrow(EnemyStraight);
	straightArrow.setInOut(o, o+130);
	straightArrow.setXRand(0.0);
	straightArrow.setPos(c, 10.0);
	straightArrow.setFrequency((int)(50*freq), 0);
	straightArrow.setXRand(1.6);
	straightArrow.setFormation(EnemyWave::Arrow);
	addWave(straightArrow);

	EnemyWave	omniWave(EnemyOmni);
	omniWave.setFrequency((int)(15*freq), 5);
	omniWave.setPos(c, 9.0);
	omniWave.setXRand(2.0);
	omniWave.setInOut(o+220, o+260);
	addWave(omniWave);

	omniWave.setXRand(3.0);
	omniWave.setFrequency((int)(22*freq), 5);
	omniWave.setInOut(o+440, o+600);
	addWave(omniWave);
}

//----------------------------------------------------------	
void ScreenItemAdd::addOmniArrowWave(int o, int /*duration*/, float density)
{
	float freq = 1.0/density;
	float c = SRAND*2.0;

	EnemyWave	omniArrow(EnemyOmni);
	omniArrow.setFormation(EnemyWave::Arrow);
	omniArrow.setPos(c, 10.0);
	omniArrow.setFrequency((int)(25*freq), 0);
	omniArrow.setXRand(1.0);
	omniArrow.setInOut(o+50, o+150);
	addWave(omniArrow);

	omniArrow.setInOut(o+250, o+320);
	addWave(omniArrow);
	omniArrow.setInOut(o+300, o+330);
	addWave(omniArrow);
	omniArrow.setInOut(o+350, o+470);
	addWave(omniArrow);
	
	omniArrow.setFrequency(5, 0);
	omniArrow.setXRand(1.8);
	omniArrow.setInOut(o+550, o+555);
	addWave(omniArrow);
	
}

//----------------------------------------------------------	
void ScreenItemAdd::addGnatWave(int o, int duration, float density, bool mixed)
{
	float freq = 1.0/density;
	float c = -FRAND*3.0;

	if(mixed)
	{
		EnemyWave	straightArrow(EnemyStraight);
		straightArrow.setInOut(o+50, o+duration);
		straightArrow.setXRand(0.0);
		straightArrow.setPos(c, 10.0);
		straightArrow.setFrequency((int)(90*freq), 20);
		straightArrow.setXRand(8.0);
		if(c > 0.0)
		{
			addWave(straightArrow);
		}
		else
		{
			EnemyWave	omniArrow(EnemyOmni);
			omniArrow.setInOut(o+50, o+130);
			omniArrow.setPos(c, 10.0);
			omniArrow.setFrequency((int)(20*freq), 0);
			omniArrow.setXRand(1.1);
			addWave(omniArrow);
			omniArrow.setInOut(o+320, o+400);
			addWave(omniArrow);

			straightArrow.setInOut(o+200, o+250);
			addWave(straightArrow);
		}
		
		EnemyWave	gnatWave(EnemyGnat);
	//	gnatWave.setInOut(o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
		gnatWave.setInOut(o, o+(int)((25*game->gameSkill)*(1.0+FRAND*0.2)));
		gnatWave.setPos(SRAND*5.0, 10.0);
		gnatWave.setFrequency((int)(3*freq), 0);
		gnatWave.setXRand(3.0);
		addWave(gnatWave);
	}
	else
	{
		EnemyWave	gnatWave(EnemyGnat);
	//	gnatWave.setInOut(o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
		gnatWave.setPos(-3.0+SRAND*5.0, 10.0);
		gnatWave.setFrequency((int)(1*freq), 0);
		gnatWave.setXRand(3.0);
		gnatWave.setInOut(o, o+35);
		addWave(gnatWave);
		gnatWave.setPos(SRAND*5.0, 10.0);
		gnatWave.setInOut(o+300, o+310);
		addWave(gnatWave);
		gnatWave.setFrequency((int)(30*freq), 0);
		gnatWave.setInOut(o+300, o+400);
		addWave(gnatWave);
	}	
}

//----------------------------------------------------------	
void ScreenItemAdd::addAmmunition(int o, int duration, int a, int b, int c)
{
	//-- Ammunition
	int		i;
#if 0
	int		ammoPause00 = 1;
	int		ammoPause01 = 1;
	int		ammoPause02 = 1;
#else
	int		ammoPause00 = a + (int)(FRAND*200);
	int		ammoPause01 = b +(int)(FRAND*200);
	int		ammoPause02 = c +(int)(FRAND*500);
#endif
	float	skill = 2.0-game->gameSkill;
	float	p[3] = {0.0, 9.0, 25.0};

	for(i = o; i < o+duration; i++)
	{
		ammoPause00--;
		ammoPause01--;
		ammoPause02--;
		p[0] = 8.0*SRAND;
		if(ammoPause00 < 1)
		{
			ammoPause00 = (int)(skill * 2000+(int)(SRAND*500));
			addItem(i, new PowerUp(PowerUps::HeroAmmo00, p) );	
		}
		else if(ammoPause01 < 1)
		{
			ammoPause01 = (int)(skill * 2500+(int)(SRAND*1000));
			addItem(i, new PowerUp(PowerUps::HeroAmmo01, p) );	
		}
		else if(ammoPause02 < 1)
		{
			ammoPause02 = (int)(skill*skill * 4000+(int)(SRAND*1000));
			addItem(i, new PowerUp(PowerUps::HeroAmmo02, p) );	
		}
	}
}


//----------------------------------------------------------	
void ScreenItemAdd::addPowerUps(int o, int duration, int a, int b, int c)
{
	//-- PowerUps
	int		i;
#if 0
	int		pwrPause00 = 1;
	int		pwrPause01 = 1;
	int		pwrPause02 = 1;
#else
	int		pwrPause00 = a+(int)(FRAND*500);
	int		pwrPause01 = b+(int)(FRAND*500);
	int		pwrPause02 = c+(int)(FRAND*1500);
#endif
	float	p[3] = {0.0, 9.0, 25.0};

	for(i = o; i < o+duration; i++)
	{
		pwrPause00--;
		pwrPause01--;
		pwrPause02--;
		p[0] = 8.0*SRAND;
		if(pwrPause00 < 1)
		{
			pwrPause00 = 2500+(int)(SRAND*700);
			addItem(i, new PowerUp(PowerUps::Shields, p) );	
		}
		else if(pwrPause01 < 1)
		{
			pwrPause01 = 4000+(int)(SRAND*900);
			addItem(i, new PowerUp(PowerUps::Repair, p) );	
		}
		else if(pwrPause02 < 1)
		{
			pwrPause02 = 5000+(int)(SRAND*3000);
//			pwrPause02 = 500;
			addItem(i, new PowerUp(PowerUps::SuperShields, p) );	
		}
	}
}

//==============================================================================
ItemThing::ItemThing()
{
	releaseTime = -1;
	item = 0;
	next = 0;
}
ItemThing::ItemThing(int relTime, ScreenItem *newScreenItem)
{
	releaseTime = relTime;
	item = newScreenItem;
	next = 0;
}


//==============================================================================
EnemyWave::EnemyWave(EnemyType t)
{
	type	= t;
	begin	= 1;
	end		= 500;
	period	= 60;
	jitter	= 10;
	formation = None;
	
	pos[0]	= 0.0;
	pos[1]	= 10.0;
	pos[2]	= 25.0;
	xJitter	= 8.0;
}



