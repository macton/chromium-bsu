/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef EnemyAircraft_Omni_h
#define EnemyAircraft_Omni_h

#include "EnemyAircraft.h"

class ActiveAmmo;
class Global;

//====================================================================
class EnemyAircraft_Omni : public EnemyAircraft
{
public:
	EnemyAircraft_Omni(EnemyType et, float p[3], float randFact = 1.0);
	virtual ~EnemyAircraft_Omni();
	
	void	update();
	void	init();
	void	init(float *p, float randFact = 1.0);
	
friend class EnemyFleet;
friend class ScreenItemAdd;

protected:
	void	move();
};

#endif //EnemyAircraft_Omni_h
