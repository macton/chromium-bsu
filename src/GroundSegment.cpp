/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#include "GroundSegment.h"

#include "Global.h"
#include "Ground.h"

//====================================================================
GroundSegment::GroundSegment(float p[3], float s[2], Ground *prnt)
{
	parent = prnt;

	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	
	size[0]	= s[0];
	size[1]	= s[1];
	
	age = 0.0;
	
	back = 0; 
	next = 0;
}

GroundSegment::~GroundSegment()
{
}

