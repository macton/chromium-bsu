/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef GroundSea_h
#define GroundSea_h

#include "compatibility.h"
#include <GL/gl.h>

#include "Ground.h"

class GroundSegment;

//====================================================================
class GroundSea  : public Ground
{
public:
	GroundSea();
	~GroundSea();

	void	drawGL();
	void	setVariation(int index);

protected:
	float size;
	
	static float	vert[4][3];
};
#endif // GroundSea_h
