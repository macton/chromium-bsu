/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "GroundSea.h"

#include <stdlib.h>
#include <GL/glpng.h>

#include "extern.h"
#include "Global.h"
#include "GroundSeaSegment.h"


float GroundSea::vert[4][3] = { {  20.5,  15.5,  0.0 },
								{ -20.5,  15.5,  0.0 },
								{  20.5, -15.5,  0.0 },
								{ -20.5, -15.5,  0.0 } };

//==============================================================================
GroundSea::GroundSea()
	: Ground()
{
	pos[0] =  0.0;
	pos[1] =  0.0;
	pos[2] =  0.0;
	
	pngInfo tmpInfo;
	tex[Base] = pngBind(dataLoc("png/gndBaseSea.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);

	size = 21.0;
	float s[2] = { size, size };
	rootSeg = new GroundSeaSegment(pos, s, this);
	
	GroundSegment	*seg;
	GroundSegment	*tmp;
	pos[1] = size*2.0;
	seg = new GroundSeaSegment(pos, s, this);
	rootSeg->next = seg;
	
	pos[1] = 0.0;
	tmp = new GroundSeaSegment(pos, s, this);
	seg->next = tmp;
	tmp->back = seg;
}

GroundSea::~GroundSea()
{
	delete rootSeg;
}

//----------------------------------------------------------
void GroundSea::drawGL()
{
	glClearColor( 0.15, 0.12, 0.10, 1.0 );
//	glClearColor(0.27451, 0.235294, 0.392157, 1.0); // web page background color
	
	GroundSegment	*seg;
	GroundSegment	*tmp;
	float	s2 = size * 2.0;
	
	//-- draw ground segments
	seg = rootSeg->next;
	while(seg)
	{
		seg->pos[1] += game->scrollSpeed;
		seg = seg->next;
	}

	glColor4f(0.9, 0.9, 0.9, 0.7);
	seg = rootSeg->next;
	while(seg)
	{
		seg->drawGL();
		
		seg->pos[1] += game->scrollSpeed;
		if(seg->pos[1] < -s2)
		{
			seg->back->next = 0;
			delete seg;
			float p[3] = {0.0, size*2.0, 0.0};
			float s[2] = { size, size };
			tmp = new GroundSeaSegment(p, s, this);
			rootSeg->next->back = tmp;
			tmp->next = rootSeg->next;
			tmp->back = rootSeg;
			rootSeg->next = tmp;
			break;
		}
		seg = seg->next;
	}
	
	return;
}

//----------------------------------------------------------
void GroundSea::setVariation(int)
{
	variation = 0;
}
