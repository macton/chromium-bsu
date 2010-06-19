/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GroundSeaSegment.h"

#include <cmath>

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
//#define GL_EXT_
//#include <GL/glext.h>

#include "define.h"
#include "Global.h"
#include "Ground.h"

//#undef FRAND
//#define FRAND 1.0
//====================================================================
GroundSeaSegment::GroundSeaSegment(float p[3], float s[2], Ground *prnt)
	: GroundSegment(p, s, prnt)
{
	float a = 1.0+(FRAND*0.5);
	vert[0][0] =   a * s[0];	a = 1.0+(FRAND*0.5);
	vert[1][0] = - a * s[0];	a = 1.0+(FRAND*0.5);
	vert[2][0] =   a * s[0];	a = 1.0+(FRAND*0.5);
	vert[3][0] = - a * s[0];	a = 1.0+(FRAND*0.5);
	vert[0][1] =   a * s[1];	a = 1.0+(FRAND*0.5);
	vert[1][1] =   a * s[1];	a = 1.0+(FRAND*0.5);
	vert[2][1] = - a * s[1];	a = 1.0+(FRAND*0.5);
	vert[3][1] = - a * s[1];	a = 1.0+(FRAND*0.5);
	vert[0][2] = 0.0;
	vert[1][2] = 0.0;
	vert[2][2] = 0.0;
	vert[3][2] = 0.0;
}

GroundSeaSegment::~GroundSeaSegment()
{
}

//----------------------------------------------------------
void GroundSeaSegment::drawGL()
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
		glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Base]);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f( 1.0, 0.0); glVertex3fv(vert[0]);
			glTexCoord2f( 0.0, 0.0); glVertex3fv(vert[1]);
			glTexCoord2f( 1.0, 1.0); glVertex3fv(vert[2]);
			glTexCoord2f( 0.0, 1.0); glVertex3fv(vert[3]);
		glEnd();
	glPopMatrix();	
}
