/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef EnemyAircraft_RayGun_h
#define EnemyAircraft_RayGun_h

#include "EnemyAircraft.h"

class ActiveAmmo;
class Global;

//====================================================================
class EnemyAircraft_RayGun : public EnemyAircraft
{
public:
	EnemyAircraft_RayGun(EnemyType et, float p[3], float randFact = 1.0);
	virtual ~EnemyAircraft_RayGun();
	
	void	update();
	void	init();
	void	init(float *p, float randFact = 1.0);
	
friend class EnemyFleet;
friend class ScreenItemAdd;

protected:
	void	move();
};

#endif //EnemyAircraft_RayGun_h
