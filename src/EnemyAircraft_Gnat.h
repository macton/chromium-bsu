/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef EnemyAircraft_Gnat_h
#define EnemyAircraft_Gnat_h

#include "EnemyAircraft.h"

class ActiveAmmo;
class Global;
class EnemyAircraft_Boss01;

//====================================================================
class EnemyAircraft_Gnat : public EnemyAircraft
{
public:
	EnemyAircraft_Gnat(EnemyType et, float p[3], float randFact = 1.0);
	virtual ~EnemyAircraft_Gnat();
	
	void	update();
	void	init();
	void	init(float *p, float randFact = 1.0);
	
friend class EnemyFleet;
friend class ScreenItemAdd;

protected:
	void	calcShootInterval();
	void	move();
};

#endif //EnemyAircraft_Gnat_h
