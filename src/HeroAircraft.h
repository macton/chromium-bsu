/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef HeroAircraft_h
#define HeroAircraft_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "ScreenItem.h"

#include "define.h"

class Global;
class EnemyFleet;
class PowerUps;

#define DEATH_SPIKES 7
#define DEATH_TIME 50

//====================================================================
class HeroAircraft : public ScreenItem
{
public:
	HeroAircraft();
	~HeroAircraft();
	
	void	moveEvent(int x, int y);
	void	fireGun(bool);
	void	shootGun();
	void	useItem(int index); 
	void	useItem(); 
	void	nextItem() { useItemArmed = 0.0; currentItemIndex = ++currentItemIndex%NUM_HERO_ITEMS; }
	void	dropBomb();	

	void	doDamage(float d);
	void	ammoDamage(float d, float vec[3]);
	void	checkForCollisions(EnemyFleet *fleet);
	void	checkForPowerUps(PowerUps *powerUps);
	
	int		currentItem()		{ return currentItemIndex; }
	float	itemArmed()	{ return useItemArmed; }
	
	int		getLives()			{	return lives;	}
	void	setLives(int in);
	float	getScore()	{ return score; }
	void	addScore(float in);
	void	addLife(bool score = false);
	void	loseLife();

	void	startDeath();

	void	update();
	void	drawGL();
	void	reset();
	void	newGame();
	void	fullHealth();
	
	inline float	getAmmoStock(int index) { return ammoStock[index]; }
	inline float	getDamage()		{	return damage; }
	inline float	getShields()	{	return shields; }
	inline bool 	isVisible()		{	return !dontShow; }
	
	float	*getPos();	
	inline float getSize(int i)	{ return size[i]; }
	
	GLuint	heroTex;
	GLuint	bombTex;
	
	void	loadTextures();
	void	deleteTextures();
	
	friend class StatusDisplay;
protected:
	void	setAmmoStock(int, float);
	void	deathExplosions();
	
	float	gunFlash0[NUM_HERO_AMMO_TYPES];
	float	gunFlash1[NUM_HERO_AMMO_TYPES];
	bool	gunActive[NUM_HERO_AMMO_TYPES];
	
private:
	float	epos[3];
	float	size[2];
	float	bound[2][2];
	
	float	deathCircle[DEATH_SPIKES][2];
	float	deathStereo;
	
	int		lives;
	int		dontShow;
	int		superBomb;
	float	scoreStep;
	float	scoreTarget;
	
	float	ammoStock[NUM_HERO_AMMO_TYPES];
	float	gunPause[NUM_HERO_AMMO_TYPES];
	bool	gunSwap;
	bool	gunTrigger;
	
	float	damage;
	float	shields;
	float	score;
	
	int		currentItemIndex;
	float	useItemArmed;
	
	//-- secondary movement
	float	move[2];
	float	secondaryMove[2];
	
	inline void drawQuad(float szx, float szy)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1.0, 0.0); glVertex3f( szx,  szy, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-szx,  szy, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f( szx, -szy, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-szx, -szy, 0.0);
		glEnd();
	}
	
	//-- tip variables
	int tipShipPast;
	int tipSuperShield;

private:
	Global *game;
};

#endif // HeroAircraft_h

