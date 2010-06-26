/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "GroundMetal.h"

#include <cmath>

#include "extern.h"
#include "Global.h"
#include "Config.h"
#include "GroundMetalSegment.h"
#include "Image.h"

//==============================================================================
GroundMetal::GroundMetal()
	: Ground()
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
	delete rootSeg->next->next;
	delete rootSeg->next;
	delete rootSeg;
}

//----------------------------------------------------------
void GroundMetal::loadTextures()
{
	Config	*config = Config::instance();
	float tbc[4] = { 0.2, 0.2, 0.2, 1.0 };
#ifdef GL_CLAMP_TO_EDGE
	GLenum clamp = GL_CLAMP_TO_EDGE;
#else
	GLenum clamp = GL_REPEAT;
#endif

	switch(variation)
	{
		case 0:
			tex[Base] = Image::load(dataLoc("png/gndMetalBase00.png"), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
			break;
		case 1:
			tex[Base] = Image::load(dataLoc("png/gndMetalBase01.png"), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
			break;
		case 2:
			tex[Base] = Image::load(dataLoc("png/gndMetalBase02.png"), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
			break;
		default:
			tex[Base] = Image::load(dataLoc("png/gndMetalBase00.png"), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
			break;
	}
	if(config->texBorder())
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tbc);
	tex[Blip] = Image::load(dataLoc("png/gndMetalBlip.png"), IMG_NOMIPMAPS, IMG_ALPHA, GL_REPEAT, GL_LINEAR, GL_LINEAR);
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
	float	pulse = sin(game->gameFrame*0.03);
	if(pulse < 0.0) 
		pulse = 0.0;
	glClearColor( 0.2+pulse, 0.2, 0.25, 1.0 );
	
	//-- draw ground segments
	if( !game->game_pause || game->gameMode == Global::Menu)
	{
		seg = rootSeg->next;
		while(seg)
		{
			seg->pos[1] += game->scrollSpeed*game->speedAdj;
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


