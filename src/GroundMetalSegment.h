/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef GroundMetalSegment_h
#define GroundMetalSegment_h

#include "GroundSegment.h"

class Ground;
class GroundMetal;

//====================================================================
class GroundMetalSegment : public GroundSegment
{
public:
	GroundMetalSegment(float p[3], float s[2], Ground *prnt);
	~GroundMetalSegment();

	void	drawGL();

	void	drawBlip(float rep, float S, float tilt, bool blipMirrorT);
	void	drawSurface(float *c0_clr,
						float *c1_clr,
						float *r0_clr,
						float *r1_clr,
						float *r2_clr);

friend class Ground;
friend class GroundMetal;
};

#endif //GroundMetalSegment_h
