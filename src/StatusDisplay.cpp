/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "StatusDisplay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glpng.h>

#include "define.h"
#include "extern.h"
#include "Global.h"
#include "Explosions.h"
#include "TexFont.h"


static float statPosAmmo[3] =	{-10.5,  8.00, 25.0 };
static float statPosShld[3] =	{-10.4, -7.80, 25.0 };
static float statClrWarn[4] = 	{ 1.1, 0.6, 0.1, 1.1 }; 
static float statClrZero[4] = 	{ 0.0, 0.0, 0.0, 0.0 }; 
static float statClrAmmo[NUM_HERO_AMMO_TYPES][4] = {	 
										{ 1.0, 0.7, 0.5, 0.6 },
										{ 0.0, 1.0, 0.5, 0.7 },
										{ 0.3, 0.0, 1.0, 0.7 } };

//====================================================================
StatusDisplay::StatusDisplay()
{	
	ammoAlpha = 0.0;
	damageAlpha = 0.0;
    shieldAlpha = 0.0;
		
	tipShipShow  = 0;
	tipSuperShow = 0;
	
	loadTextures();
	
	blink	= true;
}

StatusDisplay::~StatusDisplay()
{
	deleteTextures();
}

//----------------------------------------------------------
void StatusDisplay::loadTextures()
{
	int i;
	char	filename[128];
	pngInfo tmpInfo;
	GLenum clamp = GL_CLAMP;
#ifdef GL_VERSION_1_2
	clamp = GL_CLAMP_TO_EDGE;
#endif

	statTex      = pngBind(dataLoc("png/statBar.png"),      PNG_NOMIPMAPS,     PNG_BLEND1, &tmpInfo, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	shldTex      = pngBind(dataLoc("png/shields.png"),      PNG_SIMPLEMIPMAPS, PNG_BLEND1, &tmpInfo, clamp, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	topTex       = pngBind(dataLoc("png/stat-top.png"),     PNG_NOMIPMAPS,     PNG_BLEND1, &tmpInfo, clamp, GL_LINEAR, GL_NEAREST);
	heroSuperTex = pngBind(dataLoc("png/heroSuper.png"),    PNG_NOMIPMAPS,     PNG_ALPHA,  &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
	heroShieldTex = pngBind(dataLoc("png/heroShields.png"), PNG_NOMIPMAPS,     PNG_BLEND2, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		sprintf(filename, "png/heroAmmoFlash%02d.png", i);
		heroAmmoFlash[i] = pngBind(dataLoc(filename), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
	}
	useFocus = pngBind(dataLoc("png/useFocus.png"), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
	for(i = 0; i < NUM_HERO_ITEMS; i++)
	{
		sprintf(filename, "png/useItem%02d.png", i);
		useItem[i] = pngBind(dataLoc(filename), PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, clamp, GL_LINEAR, GL_LINEAR);
	}
}

//----------------------------------------------------------
void StatusDisplay::deleteTextures()
{
	int i;
	glDeleteTextures(1, &statTex);	
	glDeleteTextures(1, &shldTex);	
	glDeleteTextures(1, &heroSuperTex);	
	glDeleteTextures(1, &heroShieldTex);	
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
		glDeleteTextures(1, &heroAmmoFlash[i]);
	}
	for(i = 0; i < NUM_HERO_ITEMS; i++)
	{
		glDeleteTextures(1, &useItem[i]);
	}
}

//----------------------------------------------------------
void StatusDisplay::darkenGL()
{
	//-- sidebars
	glBindTexture(GL_TEXTURE_2D, shldTex);
	glBegin(GL_QUADS);
	glColor4f(0.25, 0.2, 0.2, 0.6);
		glTexCoord2f(0.0,  0.0); glVertex3f( -9.2,  8.5, 25.0);
		glTexCoord2f(1.0,  0.0); glVertex3f(-11.5,  8.5, 25.0);
	glColor4f(0.25, 0.25, 0.35, 0.6);
		glTexCoord2f(1.0,  1.7); glVertex3f(-11.5, -8.5, 25.0);
		glTexCoord2f(0.0,  1.7); glVertex3f( -9.2, -8.5, 25.0);

	glColor4f(0.25, 0.2, 0.2, 0.6);
		glTexCoord2f(1.0, 0.0); glVertex3f( 11.5,  8.5, 25.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(  9.2,  8.5, 25.0);
	glColor4f(0.25, 0.25, 0.35, 0.6);
		glTexCoord2f(0.0, 1.7); glVertex3f(  9.2, -8.5, 25.0);
		glTexCoord2f(1.0, 1.7); glVertex3f( 11.5, -8.5, 25.0);
		
	glEnd();
}

//----------------------------------------------------------
void StatusDisplay::drawGL(HeroAircraft	*hero)
{
	static	char scoreBuf[32];
	int 	i;
	bool 	statClrWarnAmmo = false;
	float	w = 0.1;
	float	x = 0.0,y,y3;
	float	size[2];
	float	ammoStock;
	
	if(!hero)
		return;
	if(!(Global::frame%15) )
		blink = !blink;
		
	ammoAlpha *= 0.96;
		
	float	shields = hero->getShields();
	float	superShields = 0.0;
	float	damage	= hero->getDamage();
	if(shields > HERO_SHIELDS)
	{
		superShields = HERO_SHIELDS-(shields-HERO_SHIELDS);
//		superShields = (shields-HERO_SHIELDS);
		shields = HERO_SHIELDS;
	}
	
	//-- draw score
	glColor4f(1.0, 1.0, 1.0, 0.4);
	glPushMatrix();
		sprintf(scoreBuf, "%07d", (int)hero->getScore());
		txfBindFontTexture(Global::texFont);
		glTranslatef(-9.0, -8.2, 25.0);
		glScalef(0.025, 0.02, 1.0);
		txfRenderString(Global::texFont, scoreBuf, strlen(scoreBuf));
	glPopMatrix();
	//-- draw fps
	if(Global::show_fps)
	{
		glPushMatrix();
			sprintf(scoreBuf, "%3.1f", Global::fps);
			txfBindFontTexture(Global::texFont);
			glTranslatef(7.75, 8.0, 25.0);
			glScalef(0.018, 0.015, 1.0);
			txfRenderString(Global::texFont, scoreBuf, strlen(scoreBuf));
		glPopMatrix();
	}
	
	//-- draw ship lives
	glPushMatrix();
	glColor4f(0.6, 0.6, 0.7, 1.0);
	glBindTexture(GL_TEXTURE_2D, Global::hero->heroTex);
	glTranslatef(10.2, 7.4, 25.0);
	size[0] = Global::hero->getSize(0)*0.5;
	size[1] = Global::hero->getSize(1)*0.5;
	for(i = 0; i < Global::hero->getLives(); i++)
	{
		drawQuad(size[0], size[1]);
		glTranslatef(0.0, -size[1]*2.0, 0.0);
	}
	glPopMatrix();	
		
	//-- draw usable items
	if(Global::gfxLevel > 1)
	{
		glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glTranslatef(8.5, -7.7, 25.0);
		size[0] = 0.4;
		size[1] = 0.5;
		for(i = 0; i < NUM_HERO_ITEMS; i++)
		{
			if(i == Global::hero->currentItem())
			{
				float a = Global::hero->itemArmed()*0.8;
				glColor4f(0.4+a, 0.4, 0.4, 0.4+a);
				glBindTexture(GL_TEXTURE_2D, useFocus);
				drawQuad(size[1], size[1]);
				glColor4f(1.0, 1.0, 1.0, 1.0);
			}
			glBindTexture(GL_TEXTURE_2D, useItem[i]);
			drawQuad(size[0], size[0]);
			glTranslatef(-size[1]*2.0, 0.0, 0.0);
		}
		glPopMatrix();	
	}
	
	//-- draw 'enemy-got-past' Warning
	if(enemyWarn && Global::hero->getLives() >= 0)
	{
		glPushMatrix();
		glColor4f(1.0, 0.0, 0.0, enemyWarn+0.15*sin(Global::gameFrame*0.7));
		glTranslatef(0.0, -8.75, 25.0);
		glBindTexture(GL_TEXTURE_2D, heroAmmoFlash[0]);
		drawQuad(12.0, 3.0);
		glPopMatrix();	
		enemyWarn = 0.0;
	}
	
	//-- draw AMMO
	glPushMatrix();
	glTranslatef(statPosAmmo[0], statPosAmmo[1], statPosAmmo[2]);
	
	
	//--draw ammo reserves
	glBindTexture(GL_TEXTURE_2D, statTex);
	glBegin(GL_QUADS);
	for(i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	{
 		glColor4fv(statClrAmmo[i]);
		ammoStock = hero->getAmmoStock(i);
		if(ammoStock > 0.0)
		{
			x = i*0.3;
			y = ammoStock*0.02;
			y3= y*2.65;
			if( blink || ammoStock > 50.0 )
				glColor4fv(statClrAmmo[i]);
			else
			{
				statClrWarnAmmo = true;
				glColor4fv(statClrWarn);
			}
			
			glTexCoord2f(1.0, 0.00); glVertex3f( x+w, -y3, 0.0 );
			glTexCoord2f(1.0,    y); glVertex3f( x+w, 0.0, 0.0 );
			glTexCoord2f(0.0,    y); glVertex3f( x-w, 0.0, 0.0 );			
			glTexCoord2f(0.0, 0.00); glVertex3f( x-w, -y3, 0.0 );
		}
	}
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, topTex);
	if(statClrWarnAmmo)
		glColor4f(statClrWarn[0], statClrWarn[1], statClrWarn[2], 0.5+ammoAlpha);
	else
		glColor4f(0.5, 0.5, 0.5+ammoAlpha, 0.2+ammoAlpha);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0,  1.0); glVertex3f(  1.25, -1.85, 0.0 );
		glTexCoord2f(1.0,  0.0); glVertex3f(  1.25,  0.47, 0.0 );
		glTexCoord2f(0.0,  0.0); glVertex3f( -0.75,  0.47, 0.0 );
		glTexCoord2f(0.0,  1.0); glVertex3f( -0.75, -2.85, 0.0 );
	glEnd();
	x += w*1.5;
	
	glPopMatrix();

	//--draw Shields
	damageAlpha *= 0.94;
	shieldAlpha *= 0.94;
	float	dc = damageAlpha*0.5;
	float   sc = shieldAlpha * 0.5;
	float	sl, sls, dl, dls;
	float	szx = 0.5;
	float	szy = 6.0;
	static	float rot = 0;
	rot+=2.0*Global::speedAdj;
	float	rot2;
	rot2 = 2*((int)rot%180);
	
	sl  = sls = (shields/HERO_SHIELDS)-1.0;
	dl  = dls = ( damage/HERO_DAMAGE)-1.0;
	if(superShields)
		sls = dls = ((shields+superShields)/HERO_SHIELDS)-1.0;
	
	//------ draw Engine
	if(hero->isVisible() && Global::gfxLevel >= 1)
	{
		float c1f = 1.0+dl;
		float c2f = -dl;
		float c1[4] = { 0.85, 0.65, 1.00, 0.7 };
		float c2[4] = { 1.00, 0.20, 0.25, 0.7 };
//		glColor4f(0.9, 0.7, 1.0, 0.7);
		glColor4f(	c1[0]*c1f+c2[0]*c2f,
					c1[1]*c1f+c2[1]*c2f,
					c1[2]*c1f+c2[2]*c2f,
					c1[3]*c1f+c2[3]*c2f);
		glBindTexture(GL_TEXTURE_2D, heroAmmoFlash[0]);
		glPushMatrix();
		glTranslatef(hero->pos[0], hero->pos[1]-0.625, hero->pos[2]);
		float esz = 1.0+c2f;
		drawQuad(1.3, 0.5*esz);
		glTranslatef(0.0, -0.18, 0.0);
		glRotatef(IRAND, 0.0, 0.0, 1.0);
		drawQuad(0.85*esz, 0.6*esz);
		glPopMatrix();
	}
	
//	if(shields > 0)
//	{
//		glPushMatrix();
//		float sz = hero->getSize(1)*1.5;
//		glColor4f(0.5, 0.5, 1.0, 0.2);
//		glBindTexture(GL_TEXTURE_2D, heroShieldTex);
//		glTranslatef(hero->pos[0], hero->pos[1]-0.05, hero->pos[2]);
//		glRotatef(IRAND, 0.0, 0.0, 1.0);
//		drawQuad(sz, sz);
//		glPopMatrix();
//	}
	
	//------ draw Super Shields
	if(superShields)
	{
		glPushMatrix();
		float sz = hero->getSize(1)*1.3;
		glColor4f(1.0, 1.0, 1.0, 1.0-sls*sls);
		glBindTexture(GL_TEXTURE_2D, heroSuperTex);
		glTranslatef(hero->pos[0], hero->pos[1], hero->pos[2]);
		glRotatef(IRAND, 0.0, 0.0, 1.0);
		drawQuad(sz, sz);
		glPopMatrix();
		
		//------ add a bit of Glitter...
		if(Global::gfxLevel > 1 && (!Global::game_pause) )
		{
			float p[3] = { 0.0, 0.0, hero->pos[2] };
			float v[3] = { 0.01*SRAND, 0.0, 0.0 };
			float c[4] = { 1.0, 1.0, 0.7, 1.0-sls*sls };
			switch(Global::gameFrame%2)
			{
				case 0:
					v[1] = -0.3+FRAND*0.05;
					p[0] = hero->pos[0];
					p[1] = hero->pos[1]-0.8;
					Global::explosions->addGlitter(p, v, c, 0, 0.4+0.4*FRAND);
					v[1] = -0.25+FRAND*0.05;
					p[0] = hero->pos[0]+0.95;
					p[1] = hero->pos[1]+0.1;
					Global::explosions->addGlitter(p, v, c, 0, 0.4+0.4*FRAND);
					p[0] = hero->pos[0]-0.95;
					p[1] = hero->pos[1]+0.1;
					Global::explosions->addGlitter(p, v, c, 0, 0.4+0.4*FRAND);
					break;
				case 1:
					v[1] = -0.25+FRAND*0.05;
					p[0] = hero->pos[0]+0.95;
					p[1] = hero->pos[1]+0.1;
					Global::explosions->addGlitter(p, v, c, 0, 0.4+0.4*FRAND);
					p[0] = hero->pos[0]-0.95;
					p[1] = hero->pos[1]+0.1;
					Global::explosions->addGlitter(p, v, c, 0, 0.4+0.4*FRAND);
					break;
			}
		}
	}
	
	//---------- Draw ammo flash
	if(Global::gfxLevel > 1)
	{
		glPushMatrix();
		glTranslatef(hero->pos[0], hero->pos[1], hero->pos[2]);
		if(hero->gunFlash0[0])
		{
			glBindTexture(GL_TEXTURE_2D, heroAmmoFlash[0]);
			szx = hero->gunFlash0[0];
			szy = 0.46f*szx;
			glColor4f(0.75f, 0.75f, 0.75f, szx);
			glTranslatef( 0.275,  0.25, 0.0);
			drawQuad(szy, szy);
			glTranslatef(-0.550,  0.00, 0.0);
			drawQuad(szy, szy);
			glTranslatef( 0.275, -0.25, 0.0);

			if(hero->gunFlash1[0])
			{
				glTranslatef( 0.45, -0.10, 0.0);
				drawQuad(szy, szy);
				glTranslatef(-0.90,  0.00, 0.0);
				drawQuad(szy, szy);
				glTranslatef( 0.45,  0.10, 0.0);
			}
		}
		if(hero->gunFlash0[1])
		{
			glBindTexture(GL_TEXTURE_2D, heroAmmoFlash[1]);
			szx = hero->gunFlash0[1];
			szy = 0.8f*szx;
			glColor4f(1.0f, 1.0f, 1.0f, szx);
			glTranslatef(0.0,  0.7, 0.0);
			drawQuad(szy, szy);
			glTranslatef(0.0, -0.7, 0.0);
		}
		glBindTexture(GL_TEXTURE_2D, heroAmmoFlash[2]);
		if(hero->gunFlash0[2])
		{
			szx = hero->gunFlash0[2];
			szy = 0.65*szx;
			glColor4f(1.0f, 1.0f, 1.0f, szx);
			glTranslatef(-0.65, -0.375, 0.0);
			drawQuad(szy, szy);
			glTranslatef( 0.65,  0.375, 0.0);
		}
		if(hero->gunFlash1[2])
		{
			szx = hero->gunFlash1[2];
			szy = 0.65f*szx;
			glColor4f(1.0f, 1.0f, 1.0f, szx);
			glTranslatef( 0.65, -0.375, 0.0);
			drawQuad(szy, szy);
			glTranslatef(-0.65,  0.375, 0.0);
		}
		glPopMatrix();
	}

//	//-- shield indicator
	glBindTexture(GL_TEXTURE_2D, shldTex);
	glColor4f(0.2, 0.2, 0.2, 0.5);
	glBegin(GL_QUADS);
	szx = 0.6;
	szy = 6.0;
		glTexCoord2f( 1.0, 1.0); glVertex3f(  statPosShld[0]+szx,  statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f(-2.5, 1.0); glVertex3f(  statPosShld[0]-2.0,  statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f(-2.5, 0.0); glVertex3f(  statPosShld[0]-2.0,  statPosShld[1]+0.0, statPosShld[2] );
		glTexCoord2f( 1.0, 0.0); glVertex3f(  statPosShld[0]+szx,  statPosShld[1]+0.0, statPosShld[2] );
	
		glTexCoord2f( 3.5, 1.0); glVertex3f( -statPosShld[0]+2.0,  statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f( 0.0, 1.0); glVertex3f( -statPosShld[0]-szx,  statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f( 0.0, 0.0); glVertex3f( -statPosShld[0]-szx,  statPosShld[1]+0.0, statPosShld[2] );
		glTexCoord2f( 3.5, 0.0); glVertex3f( -statPosShld[0]+2.0,  statPosShld[1]+0.0, statPosShld[2] );
	glEnd();
		
	if(Global::gfxLevel > 0)
	{
		//-- Shields 
		if( (sl < -0.7 && blink && shields > 0.0) || superShields )
			glColor4fv(statClrWarn);
		else
			glColor4f(0.7+dc, 0.6+dc, 0.8+dc, 0.5+damageAlpha);
//			glColor4f(0.0+sc, 0.35+sc, 1.0+sc, 0.5+shieldAlpha);
		glPushMatrix();
		glTranslatef(statPosShld[0], statPosShld[1], statPosShld[2]);
//		glScalef(1.0, 1.0, 1.5);
		glRotatef(-rot, 0.0, 1.0, 0.0);
		glBegin(GL_QUADS);
		szx = 0.5;
		glTexCoord2f( 1.0,     sls); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 0.0,     sls); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 0.0, 1.0+sls); glVertex3f( -szx,  0.0,  szx );
		glTexCoord2f( 1.0, 1.0+sls); glVertex3f(  szx,  0.0,  szx );
		
		glTexCoord2f( 0.0,     sls); glVertex3f(  szx,  szy, -szx );
		glTexCoord2f( 0.0, 1.0+sls); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 1.0, 1.0+sls); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 1.0,     sls); glVertex3f( -szx,  szy, -szx );
		
		glTexCoord2f( 1.0,     sls); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 1.0, 1.0+sls); glVertex3f(  szx,  0.0,  szx );
		glTexCoord2f( 0.0, 1.0+sls); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 0.0,     sls); glVertex3f(  szx,  szy, -szx );
		
		glTexCoord2f( 1.0,     sls); glVertex3f( -szx,  szy, -szx );
		glTexCoord2f( 1.0, 1.0+sls); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 0.0, 1.0+sls); glVertex3f( -szx,  0.0,  szx );
		glTexCoord2f( 0.0,     sls); glVertex3f( -szx,  szy,  szx );
		
		if(shields)
		{
			glTexCoord2f( 1.0, 1.0); 
			glColor4f(0.3+sc, 0.4+sc, 1.0+sc, 0.5);
			glVertex3f(  szx,  0.0,  szx );
			glVertex3f(  szx,  0.0, -szx );
			glVertex3f( -szx,  0.0, -szx );
			glVertex3f( -szx,  0.0,  szx );
		}	
		glEnd();
		
		glRotatef( rot2, 0.0, 1.0, 0.0);
//		glColor4f(0.4+sc, 0.5+sc, 1.0+sc, 0.6+shieldAlpha);
		glColor4f(0.1+sc, 0.15+sc, 0.9+sc, 0.6+shieldAlpha);
		glBegin(GL_QUADS);
		szx = 0.4;
		glTexCoord2f( 1.0,     sl); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 0.0,     sl); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 0.0, 1.0+sl); glVertex3f( -szx,  0.0,  szx );
		glTexCoord2f( 1.0, 1.0+sl); glVertex3f(  szx,  0.0,  szx );
		
		glTexCoord2f( 0.0,     sl); glVertex3f(  szx,  szy, -szx );
		glTexCoord2f( 0.0, 1.0+sl); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 1.0, 1.0+sl); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 1.0,     sl); glVertex3f( -szx,  szy, -szx );
		
		glTexCoord2f( 1.0,     sl); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 1.0, 1.0+sl); glVertex3f(  szx,  0.0,  szx );
		glTexCoord2f( 0.0, 1.0+sl); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 0.0,     sl); glVertex3f(  szx,  szy, -szx );
		
		glTexCoord2f( 0.0,     sl); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 1.0,     sl); glVertex3f( -szx,  szy, -szx );
		glTexCoord2f( 1.0, 1.0+sl); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 0.0, 1.0+sl); glVertex3f( -szx,  0.0,  szx );
		glEnd();
		glPopMatrix();



		if( (dl < -0.7 && blink) || superShields )
		{
			glColor4fv(statClrWarn);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, statClrWarn );
		}
		else
		{
			glColor4f(0.9+dc, 0.6+dc, 0.7+dc, 0.5+damageAlpha);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, statClrZero );
		}
		//-- Life
		glPushMatrix();
		glTranslatef(-statPosShld[0], statPosShld[1], statPosShld[2]);
		glRotatef( rot, 0.0, 1.0, 0.0);
		
		glBegin(GL_QUADS);
		szx = 0.5;
		glTexCoord2f( 1.0,     dls); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 0.0,     dls); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 0.0, 1.0+dls); glVertex3f( -szx,  0.0,  szx );
		glTexCoord2f( 1.0, 1.0+dls); glVertex3f(  szx,  0.0,  szx );
		
		glTexCoord2f( 0.0,     dls); glVertex3f(  szx,  szy, -szx );
		glTexCoord2f( 0.0, 1.0+dls); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 1.0, 1.0+dls); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 1.0,     dls); glVertex3f( -szx,  szy, -szx );
		
		glTexCoord2f( 1.0,     dls); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 1.0, 1.0+dls); glVertex3f(  szx,  0.0,  szx );
		glTexCoord2f( 0.0, 1.0+dls); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 0.0,     dls); glVertex3f(  szx,  szy, -szx );
		
		glTexCoord2f( 0.0,     dls); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 1.0,     dls); glVertex3f( -szx,  szy, -szx );
		glTexCoord2f( 1.0, 1.0+dls); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 0.0, 1.0+dls); glVertex3f( -szx,  0.0,  szx );
		
		if(damage)
		{
			glTexCoord2f( 1.0, 1.0); 
			glColor4f(1.0+dc, 0.0+dc, 0.0+dc, 0.5);
			glVertex3f(  szx,  0.0,  szx );
			glVertex3f(  szx,  0.0, -szx );
			glVertex3f( -szx,  0.0, -szx );
			glVertex3f( -szx,  0.0,  szx );
		}
		glEnd();
		
		glRotatef(-rot2, 0.0, 1.0, 0.0);
		glColor4f(1.0+dc, 0.0+dc, 0.0+dc, 0.6+damageAlpha);
		glBegin(GL_QUADS);
		szx = 0.4;
		glTexCoord2f( 1.0,     dl); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 0.0,     dl); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 0.0, 1.0+dl); glVertex3f( -szx,  0.0,  szx );
		glTexCoord2f( 1.0, 1.0+dl); glVertex3f(  szx,  0.0,  szx );
		
		glTexCoord2f( 0.0,     dl); glVertex3f(  szx,  szy, -szx );
		glTexCoord2f( 0.0, 1.0+dl); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 1.0, 1.0+dl); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 1.0,     dl); glVertex3f( -szx,  szy, -szx );
		
		glTexCoord2f( 1.0,     dl); glVertex3f(  szx,  szy,  szx );
		glTexCoord2f( 1.0, 1.0+dl); glVertex3f(  szx,  0.0,  szx );
		glTexCoord2f( 0.0, 1.0+dl); glVertex3f(  szx,  0.0, -szx );
		glTexCoord2f( 0.0,     dl); glVertex3f(  szx,  szy, -szx );
		
		glTexCoord2f( 0.0,     dl); glVertex3f( -szx,  szy,  szx );
		glTexCoord2f( 1.0,     dl); glVertex3f( -szx,  szy, -szx );
		glTexCoord2f( 1.0, 1.0+dl); glVertex3f( -szx,  0.0, -szx );
		glTexCoord2f( 0.0, 1.0+dl); glVertex3f( -szx,  0.0,  szx );

		glEnd();
		glPopMatrix();
	}
	else
	{
		szx = 0.8;
		if( (sl < -0.7 && blink && shields > 0.0) || superShields )
			glColor4fv(statClrWarn);
		else
			glColor4f(0.0+sc, 0.35+sc, 1.0+sc, 0.7+shieldAlpha);
		//-- Shields
		glBegin(GL_QUADS);
		glTexCoord2f( 1.0,     sl); glVertex3f( statPosShld[0]    , statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f( 0.0,     sl); glVertex3f( statPosShld[0]-szx, statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f( 0.0, 1.0+sl); glVertex3f( statPosShld[0]-szx, statPosShld[1]    , statPosShld[2] );
		glTexCoord2f( 1.0, 1.0+sl); glVertex3f( statPosShld[0]    , statPosShld[1]    , statPosShld[2] );
		//-- Life

		if( (dl < -0.7 && blink) )
			glColor4fv(statClrWarn);
		else
			glColor4f(1.0+dc, 0.0+dc, 0.0+dc, 0.7+damageAlpha);
		glTexCoord2f( 1.0,     dl); glVertex3f( -statPosShld[0]    , statPosShld[1]+szy, statPosShld[2] );
		glTexCoord2f( 1.0, 1.0+dl); glVertex3f( -statPosShld[0]    , statPosShld[1]    , statPosShld[2] );
		glTexCoord2f( 0.0, 1.0+dl); glVertex3f( -statPosShld[0]+szx, statPosShld[1]    , statPosShld[2] );
		glTexCoord2f( 0.0,     dl); glVertex3f( -statPosShld[0]+szx, statPosShld[1]+szy, statPosShld[2] );
		glEnd();
	}
	
	//-- print message if we're paused...
	if(Global::game_pause)
	{
		float off[2];
		off[0] = 2.0 * sin(Global::frame*0.01);
		off[1] = 1.0 * cos(Global::frame*0.011);
		glPushMatrix();
		txfBindFontTexture(Global::texFont);
		glTranslatef(-14.5, -3.0, 0.0);
		glScalef(0.21, 0.21, 1.0);
		glPushMatrix();
		glColor4f(1.0, 1.0, 1.0, 0.10*fabs(sin(Global::frame*0.05)) );
		txfRenderString(Global::texFont, "p a u s e d", 11);
		glPopMatrix();
		glColor4f(1.0, 1.0, 1.0, 0.10*fabs(sin(Global::frame*0.03)) );
		glTranslatef(off[0], off[1], 0.0);
		txfRenderString(Global::texFont, "p a u s e d", 11);
		glPopMatrix();
	}
	if( Global::tipShipPast == 1 && Global::gameLevel == 1)
	{
		Global::tipShipPast++;
		tipShipShow = 200;
	}
	if( Global::tipSuperShield == 1 && Global::gameLevel == 1)
	{
		Global::tipSuperShield++;
		tipSuperShow = 200;
	}
	if(	tipShipShow > 0 )
	{
		tipShipShow--;
		glPushMatrix();
		txfBindFontTexture(Global::texFont);
		glTranslatef(-16, 13.0, 0.0);
		glScalef(0.035, 0.035, 1.0);
		glColor4f(1.0, 1.0, 1.0, tipShipShow/300.0 );
		char *str = "do not let -any- ships past you! each one costs you a life!";
		txfRenderString(Global::texFont, str, strlen(str));
		glPopMatrix();
	}
	if(	tipSuperShow > 0 )
	{
		tipSuperShow--;
		glPushMatrix();
		txfBindFontTexture(Global::texFont);
		glTranslatef(-16, 13.0, 0.0);
		glScalef(0.035, 0.035, 1.0);
		glColor4f(1.0, 1.0, 1.0, tipSuperShow/300.0 );
		char *str = "let super shields pass by for an extra life!";
		txfRenderString(Global::texFont, str, strlen(str));
		glPopMatrix();
	}
}
