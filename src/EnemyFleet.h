/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef EnemyFleet_h
#define EnemyFleet_h

#include "compatibility.h"
#include <GL/gl.h>

#include "EnemyAircraft.h"

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
	
};

#endif // EnemyFleet_h
