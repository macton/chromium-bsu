/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef Ground_h
#define Ground_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

class Global;
class GroundSegment;

//====================================================================
class Ground
{
public:
	Ground();
	virtual ~Ground();

	enum TexNames { Base, Blip, NumGndTex };
	
	virtual void drawGL() = 0;
	virtual void setVariation(int index) = 0;
	void nextVariation();

	virtual void loadTextures() = 0;
	virtual void deleteTextures() = 0;
	
friend class GroundMetalSegment;
friend class GroundSeaSegment;

protected:
	int		variation;
	float	pos[3];
	GLuint	tex[NumGndTex];

	GroundSegment *rootSeg;

protected:
	Global	*game;
};
#endif // Ground_h
