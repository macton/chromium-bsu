/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef StatusDisplay_h
#define StatusDisplay_h

#include "compatibility.h"
#include <GL/gl.h>
#include "HeroAircraft.h"

//====================================================================
class StatusDisplay
{
public:
	StatusDisplay();
	~StatusDisplay();
	
	void	darkenGL();
	void	drawGL(HeroAircraft	*hero);
	
	void	setAmmoAlpha(float in)	{ ammoAlpha = in; };
	void	setDamageAlpha(float in) { damageAlpha = in; };
	void    setShieldAlpha(float in) { shieldAlpha = in; };
	
	void	enemyWarning(float v) { if(v > enemyWarn) enemyWarn = v; }
	
	void	loadTextures();
	void	deleteTextures();
	
private:
	GLuint		statTex;
	GLuint		shldTex;
	GLuint		topTex;
	GLuint		heroSuperTex;
	GLuint		heroShieldTex;
	GLuint		heroAmmoFlash[NUM_HERO_AMMO_TYPES];
	GLuint		useFocus;
	GLuint		useItem[NUM_HERO_ITEMS];
	
	float		ammoAlpha;
	float		damageAlpha;
        float           shieldAlpha;
	bool		blink;
	
	inline void drawQuad(float szx, float szy)
	{
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1.0, 0.0); glVertex3f( szx,  szy, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-szx,  szy, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f( szx, -szy, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-szx, -szy, 0.0);
		glEnd();
	}
	
	int tipShipShow;
	int tipSuperShow;
	
	float enemyWarn;
};

#endif // StatusDisplay_h

