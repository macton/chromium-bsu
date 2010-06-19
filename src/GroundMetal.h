/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef GroundMetal_h
#define GroundMetal_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Ground.h"

class GroundSegment;

//====================================================================
class GroundMetal : public Ground
{
public:
	GroundMetal();
	~GroundMetal();
	
	void	drawGL();
	void	setVariation(int index);
	
	void	loadTextures();
	void	deleteTextures();

	float	size;

friend class GroundSegment;
friend class GroundMetalSegment;
};
#endif // GroundMetal_h
