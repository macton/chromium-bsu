/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Ammo.h"

#include <stdio.h>


int ActiveAmmo::ammoCount = 0;

//====================================================================
ActiveAmmo::ActiveAmmo()
{
	back = 0;
	next = 0;
	ammoCount++;
}

ActiveAmmo::~ActiveAmmo()
{
	ammoCount--;
}

//----------------------------------------------------------
void ActiveAmmo::init(float p[3], float v[3], float d)
{
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	vel[0] = v[0];
	vel[1] = v[1];
	vel[2] = v[2];
	damage = d;
	back = 0;
	next = 0;
}


