/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef ScreenItemAdd_h
#define ScreenItemAdd_h

#include "ScreenItem.h"
#include "EnemyAircraft.h"

class Global;
class ItemThing;
class EnemyWave;

//====================================================================
class ScreenItemAdd
{
public:
	ScreenItemAdd();
	~ScreenItemAdd();
	
	void putScreenItems();
	void killScreenItem(ScreenItem *);
	
	bool loadScreenItems(const char*);
	void clear();
	
	EnemyAircraft *dynamicEnemyAdd(EnemyType et, float *pos, int relTime);

private:
	void clearDeadPool();
	void addItem(int relTime, ScreenItem *newItem);
	void addWave(EnemyWave &ew );

	void loadLevelXXX();
	void loadLevel1();
	void loadLevel2();
	void loadLevel3();

	void addStraightWave(int o, int duration, float density = 1.0);
	void addOmniWave(int o, int duration, float density = 1.0);
	void addStraightArrowWave(int o, int duration, float density = 1.0);
	void addOmniArrowWave(int o, int duration, float density = 1.0);
	void addGnatWave(int o, int duration, float density = 1.0, bool mixed = true);
	
	void addAmmunition(int o, int duration, int a =   0, int b =  100, int c = 1000);
	void addPowerUps  (int o, int duration, int a = 300, int b = 1200, int c = 1000);
	
	ItemThing *root;
	
	EnemyAircraft	*deadPool[NumEnemyTypes];

private:
	Global *game;
};

//------------------
class ItemThing
{
public:
	ItemThing();
	ItemThing(int relTime, ScreenItem *newScreenItem);
	
	int releaseTime;
	ScreenItem *item;
	
	ItemThing *next;
};


//------------------
class EnemyWave
{
public:
	EnemyWave(EnemyType t);
	
	enum Formation { None, Arrow, NumFormations };
	
	void setInOut(int b, int e)					{ begin = b; end = e; if(e <= b) end = b+1; }
	void setPos(float x, float y)				{ pos[0] = x; pos[1] = y; 	}
	void setXRand(float j)						{ xJitter = j;	}
	void setFrequency(int p, int j = 0)			{ period = p; jitter = j;	}
	void setFormation(Formation f)				{ formation = f; }

	EnemyType type;
	int begin;
	int end;
	int period;
	int jitter;
	
	Formation formation;
	
	float	pos[3];
	float	xJitter;
};

#endif // ScreenItemAdd_h
