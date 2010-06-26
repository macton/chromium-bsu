/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef EnemyFleet_h
#define EnemyFleet_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "EnemyAircraft.h"

class Global;

//====================================================================
class EnemyFleet
{
public:
	EnemyFleet();
	~EnemyFleet();

	void	drawGL();
	void	clear();
	
	void	addEnemy(EnemyAircraft *);
	void	killEnemy(EnemyAircraft *);
	
	void	destroyAll();
	void	retarget(EnemyType et, ScreenItem *newTarget);
	
	void			toFirst();
	EnemyAircraft	*getShip();
	
	void	update();
	
	void	loadTextures();
	void	deleteTextures();
	
private:
	inline void drawQuad(float szx, float szy)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1.0, 0.0); glVertex3f( szx,  szy, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-szx,  szy, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f( szx, -szy, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-szx, -szy, 0.0);
		glEnd();
	}
	
	void	bossExplosion(EnemyAircraft *);

private:
	GLuint			shipTex[NumEnemyTypes];
	GLuint			extraTex[NumEnemyTypes];
	
	EnemyAircraft	*squadRoot;
	EnemyAircraft	*currentShip;

private:
	Global	*game;
};

#endif // EnemyFleet_h
