/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "GroundMetalSegment.h"

#include <math.h>

#include "compatibility.h"
#include <GL/gl.h>
//#define GL_EXT_
//#include <GL/glext.h>

#include "Global.h"
#include "Ground.h"

//====================================================================
GroundMetalSegment::GroundMetalSegment(float p[3], float s[2], Ground *prnt)
	: GroundSegment(p, s, prnt)
{
}

GroundMetalSegment::~GroundMetalSegment()
{
}

//----------------------------------------------------------
void GroundMetalSegment::drawGL()
{
	static float c0_clr[4] = { 0.65, 0.62, 0.53, 1.0 };
	static float c1_clr[4] = { 0.79, 0.82, 0.69, 1.0 };
	static float r0_clr[4]  = { 0.07, 0.07, 0.13, 1.0 };
	static float r1_clr[4]  = { 0.31, 0.30, 0.30, 1.0 }; 
	static float r2_clr[4]  = { 0.31, 0.30, 0.30, 1.0 }; 
	float S;
	float tmp;
	float rep;
	float tilt;
	float clr_sin;
	bool blipMirrorT = false;
	
	age += 1.0;
	
	clr_sin = 0.5*sin(Global::gameFrame*0.001);
	r1_clr[0] = 0.15+clr_sin;
	r2_clr[0] = 0.15+clr_sin;
	
	clr_sin = 0.2*sin(Global::gameFrame*0.0005);
	c0_clr[0] = 0.28+clr_sin;
	c0_clr[1] = 0.25+clr_sin;
	c0_clr[2] = 0.16+clr_sin;
	c1_clr[0] = 0.42+clr_sin;
	c1_clr[1] = 0.45+clr_sin;
	c1_clr[2] = 0.34+clr_sin;
	
//	glPushMatrix();
//	glTranslatef(pos[0], pos[1], pos[2]);
	
	switch(parent->variation)
	{
		default:
		case 0:
			rep = 0.26;
			tilt = 0.1;
			S = -Global::frame*0.001;
			blipMirrorT = false;
			break;
		case 1:
			rep = 0.4;
			tilt = 0.2;
			S = -Global::frame*0.001;
			blipMirrorT = true;
			break;
		case 2:
			S = -Global::frame*0.005;
			tmp = sin(S);
			rep = 0.7+tmp;
			tilt = 0.5+tmp;
			blipMirrorT = true;
			break;
	}
	
	if(Global::gfxLevel > 1)
	{
		float repA = 0.0;
		float repB = rep;
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Blip]);
		glBegin(GL_QUADS);
			glTexCoord2f( rep,  repA+S+tilt); glVertex3f(         pos[0],	       pos[1], pos[2]);
			glTexCoord2f( 0.0,  repA+S);      glVertex3f(-size[0]+pos[0],	       pos[1], pos[2]);
			glTexCoord2f( 0.0,  repB+S);      glVertex3f(-size[0]+pos[0], -size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  repB+S+tilt); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);

			glTexCoord2f( 0.0,  repA+S);      glVertex3f( size[0]+pos[0],	       pos[1], pos[2]);
			glTexCoord2f( rep,  repA+S+tilt); glVertex3f(         pos[0],	       pos[1], pos[2]);
			glTexCoord2f( rep,  repB+S+tilt); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);		
			glTexCoord2f( 0.0,  repB+S);      glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);

			if(blipMirrorT) { repA = rep; repB = 0.0; }
			
			glTexCoord2f( 0.0,  repA+S);      glVertex3f( size[0]+pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  repA+S+tilt); glVertex3f(	      pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  repB+S+tilt); glVertex3f(	      pos[0],	       pos[1], pos[2]);
			glTexCoord2f( 0.0,  repB+S);      glVertex3f( size[0]+pos[0],	       pos[1], pos[2]);

			glTexCoord2f( rep,  repA+S+tilt); glVertex3f(	      pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  repA+S);      glVertex3f(-size[0]+pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  repB+S);      glVertex3f(-size[0]+pos[0],	       pos[1], pos[2]);
			glTexCoord2f( rep,  repB+S+tilt); glVertex3f(         pos[0],	       pos[1], pos[2]);
		glEnd();
	}
	if(true)
	{
		rep = 1.0;
		glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Base]);
		glBegin(GL_TRIANGLES); //-- use triangles to prevent color popping on Utah
		if(Global::gfxLevel > 0)
		{
			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);
			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);
			glColor4fv(r2_clr); glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],          pos[1], pos[2]);

			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);
			glColor4fv(r2_clr); glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],          pos[1], pos[2]);
			glColor4fv(r0_clr); glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0], -size[1]+pos[1], pos[2]);

			glColor4fv(r1_clr); glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);
			glColor4fv(r0_clr); glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],          pos[1], pos[2]);
			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);

			glColor4fv(r1_clr); glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);
			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);
			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);

			glColor4fv(r0_clr); glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],          pos[1], pos[2]);
			glColor4fv(r1_clr); glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0],  size[1]+pos[1]+0.1, pos[2]);
			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1]+0.1, pos[2]);

			glColor4fv(r0_clr); glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],          pos[1], pos[2]);
			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1]+0.1, pos[2]);
			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);

			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);
			glColor4fv(c0_clr); glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1]+0.1, pos[2]);
			glColor4fv(r0_clr); glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0],  size[1]+pos[1]+0.1, pos[2]);

			glColor4fv(c1_clr); glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],          pos[1], pos[2]);
			glColor4fv(r0_clr); glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0],  size[1]+pos[1]+0.1, pos[2]);
			glColor4fv(r2_clr); glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],          pos[1], pos[2]);
		}
		else
		{
			float b = -0.1;
			glColor4f(c0_clr[0]-b, c0_clr[1]-b, c0_clr[2]-b, c0_clr[3] );			
			glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0], 		   pos[1], pos[2]);
			glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],  	       pos[1], pos[2]);

			glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],          pos[1], pos[2]);
			glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0], -size[1]+pos[1], pos[2]);

			glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],  	       pos[1], pos[2]);
			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],  	       pos[1], pos[2]);

			glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],  	       pos[1], pos[2]);
			glTexCoord2f( rep,  rep); glVertex3f(         pos[0], -size[1]+pos[1], pos[2]);

			glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],          pos[1], pos[2]);
			glTexCoord2f( 0.0,  rep); glVertex3f( size[0]+pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1], pos[2]);

			glTexCoord2f( 0.0,  0.0); glVertex3f( size[0]+pos[0],  	       pos[1], pos[2]);
			glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],  	       pos[1], pos[2]);

			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],  	       pos[1], pos[2]);
			glTexCoord2f( rep,  rep); glVertex3f(         pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0],  size[1]+pos[1], pos[2]);

			glTexCoord2f( rep,  0.0); glVertex3f(         pos[0],  	       pos[1], pos[2]);
			glTexCoord2f( 0.0,  rep); glVertex3f(-size[0]+pos[0],  size[1]+pos[1], pos[2]);
			glTexCoord2f( 0.0,  0.0); glVertex3f(-size[0]+pos[0],  	       pos[1], pos[2]);
		}
		glEnd();
	}
	
//	glPopMatrix();	
}
