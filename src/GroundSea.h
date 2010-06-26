/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef GroundSea_h
#define GroundSea_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

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
