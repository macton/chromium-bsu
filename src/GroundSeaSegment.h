/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef GroundSeaSegment_h
#define GroundSeaSegment_h

#include "GroundSegment.h"

class Ground;
class GroundMetal;

//====================================================================
class GroundSeaSegment : public GroundSegment
{
public:
	GroundSeaSegment(float p[3], float s[2], Ground *prnt);
	~GroundSeaSegment();

	void	drawGL();

friend class Ground;
friend class GroundMetal;

private:
	float vert[4][3];
};

#endif //GroundSeaSegment_h
