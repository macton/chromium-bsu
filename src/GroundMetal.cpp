/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "GroundMetal.h"

#include <GL/glpng.h>
#include <math.h>

#include "extern.h"
#include "Global.h"
#include "GroundMetalSegment.h"

//==============================================================================
GroundMetal::GroundMetal()
{
	pos[0] =  0.0;
	pos[1] =  0.0;
	pos[2] =  0.0;
	
	variation = 0;
	loadTextures();
	
	size = 21.0;
	float s[2] = { size, size };
	rootSeg = new GroundMetalSegment(pos, s, this);
	
	GroundSegment	*seg;
	GroundSegment	*tmp;
	pos[1] = size*2.0;
	seg = new GroundMetalSegment(pos, s, this);
	rootSeg->next = seg;
	
	pos[1] = 0.0;
	tmp = new GroundMetalSegment(pos, s, this);
	seg->next = tmp;
	tmp->back = seg;
}

GroundMetal::~GroundMetal()
{
	deleteTextures();
	//XXX ADD clear() method
	delete rootSeg;
}

//----------------------------------------------------------
void GroundMetal::loadTextures()
{
	pngInfo tmpInfo;
	float tbc[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLenum clamp = GL_REPEAT;
#ifdef GL_VERSION_1_2
	clamp = GL_CLAMP_TO_EDGE;
#endif
	switch(variation)
	{
		case 0:
			tex[Base] = pngBind(dataLoc("png/gndMetalBase00.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
			break;
		case 1:
			tex[Base] = pngBind(dataLoc("png/gndMetalBase01.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
			break;
		case 2:
			tex[Base] = pngBind(dataLoc("png/gndMetalBase02.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
			break;
		default:
			tex[Base] = pngBind(dataLoc("png/gndMetalBase00.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
			break;
	}
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tbc);
	tex[Blip] = pngBind(dataLoc("png/gndMetalBlip.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_REPEAT, GL_LINEAR, GL_LINEAR);
}

//----------------------------------------------------------
void GroundMetal::deleteTextures()
{
	glDeleteTextures(1, &tex[Base]);	
	glDeleteTextures(1, &tex[Blip]);
	tex[Base] = 0;
	tex[Blip] = 0;
}

//----------------------------------------------------------
void GroundMetal::setVariation(int index)
{
	variation = index%3;
	deleteTextures();
	loadTextures();
}

//----------------------------------------------------------
void GroundMetal::drawGL()
{
	GroundSegment	*seg;
	GroundSegment	*tmp;
	float	s2 = size * 2.0;
	
	//-- Set background color for low and med gfx
	float	pulse = sin(Global::gameFrame*0.03);
	if(pulse < 0.0) pulse = 0.0;
	glClearColor( 0.2+pulse, 0.2, 0.25, 1.0 );
	
	//-- draw ground segments
	if( !Global::game_pause || Global::gameMode == Global::Menu)
	{
		seg = rootSeg->next;
		while(seg)
		{
			seg->pos[1] += Global::scrollSpeed*Global::speedAdj;
			seg = seg->next;
		}
	}

	seg = rootSeg->next;
	while(seg)
	{
		seg->drawGL();
		
		if(seg->pos[1] < -s2)
		{
			float p[3] = { 0.0, seg->pos[1]+s2+s2, 0.0};
			float s[2] = { size, size };
			seg->back->next = 0;
			delete seg;
			tmp = new GroundMetalSegment(p, s, this);
			rootSeg->next->back = tmp;
			tmp->next = rootSeg->next;
			tmp->back = rootSeg;
			rootSeg->next = tmp;
			break;
		}
		seg = seg->next;
	}

}


