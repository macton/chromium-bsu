/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MenuGL.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glpng.h>

#include "Config.h"

#include "extern.h"
#include "define.h"
#include "Global.h"
#include "HiScore.h"
#include "TexFont.h"
#include "Ground.h"
#include "Audio.h"

#include "textGeometry.h"

static const int NumMssg = 6;
static char mssgHelpText[NumMssg][128] = {
"  d o   n o t   a l l o w  -a n y-   e n e m i e s   g e t   p a s t   y o u !",
"  e v e r y   e n e m y   t h a t   g e t s   b y   c o s t s   y o u   a   l i f e !",
"  a l l o w   p o w e r - u p s   t o   p a s s   b y   f o r   b i g   p o i n t s !",
"  c r a s h   i n t o   e n e m i e s   t o   d e s t r o y   t h e m !",
"  r i g h t   c l i c k   t w i c e   t o   s e l f - d e s t r u c t !",
"  s e l f - d e s t r u c t   t o   p r e s e r v e   y o u r   a m m u n i t i o n !"
};


//====================================================================
MenuGL::MenuGL()
{
	game = Global::getInstance();
	
	curSel = NewGame;

	elecStretch = 10.0;
	elecOffX = 0.0;
	elecOffY = 0.0;
	textAngle = 0.0;
	txtHeight = 0.5;
	titleTilt = -10.0;

	menuText[NewGame]	= "n e w    g a m e";
	menuText[GameLevel]	= "l e v e l";
	menuText[SkillLevel]= "s k i l l";
	menuText[Graphics]	= "g f x    d e t a i l";
	menuText[FullScreen]= "f u l l s c r e e n";
	menuText[ScreenSize]= "s c r e e n    s i z e";
	menuText[Sound]		= "s o u n d    f x    v o l u m e";
	menuText[Music]		= "m u s i c    v o l u m e";
	menuText[MouseSpeed]= "m o u s e   s p e e d";
	menuText[Quit]		= "q u i t";
	
	loadTextures();
	
	thickText = true;

	mssgAlpha = 0.0;
	sprintf(mssgText, " ");
	mssgIndex = 0;
	mssgCount = 0;
	mssgHelpOverride = false;
	
	startMenu();
}

//----------------------------------------------------------
MenuGL::~MenuGL()
{
	deleteTextures();
}

//----------------------------------------------------------
void MenuGL::loadTextures()
{
	pngInfo tmpInfo;
	elecTex   = pngBind(dataLoc("png/electric.png"),  PNG_NOMIPMAPS, PNG_BLEND3, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	backTex   = pngBind(dataLoc("png/menu_back.png"), PNG_NOMIPMAPS, PNG_SOLID, &tmpInfo, GL_REPEAT, GL_LINEAR, GL_LINEAR);
//	csrTex    = pngBind(dataLoc("png/cursor.png"),    PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	csrTex    = pngBind(dataLoc("png/heroAmmoFlash00.png"),PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	updwnTex  = pngBind(dataLoc("png/menu_updown.png"),    PNG_NOMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	//-- Environment map
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	envTex = pngBind(dataLoc("png/reflect.png"), PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);

	listChrom = glGenLists(1);
	listBSU = glGenLists(1);
	createLists( (thickText = true) );
}

//----------------------------------------------------------
void MenuGL::deleteTextures()
{
	glDeleteTextures(1, &elecTex);
	glDeleteTextures(1, &backTex);
	glDeleteTextures(1, &envTex);
	glDeleteTextures(1, &csrTex);
	glDeleteTextures(1, &updwnTex);
	elecTex	= 0;
	backTex	= 0;
	envTex	= 0;
	csrTex	= 0;
	updwnTex = 0;
	
	glDeleteLists(listChrom, 1);
	glDeleteLists(listBSU, 1);
}

//----------------------------------------------------------
void MenuGL::createLists(bool thick)
{
	if(!thick)
	{
//		fprintf(stderr, "ATTENTION: Using 'thin' text to improve framerate...\n");
	}
	titleTilt	= -10.0;
	
//	fprintf(stderr, "MenuGL::createLists\n");

	glNewList(listChrom, GL_COMPILE);
	textGeometryChromium(thick);
	glEndList();
	
	glNewList(listBSU, GL_COMPILE);
	textGeometryBSU(thick);
	glEndList();
}

static const char *skillString(int i)
{
	switch(i)
	{
		case 2: return "fish in a barrel";
		case 3: return "whimp"; 
		case 4: return "easy";
		case 5: return "normal"; 
		case 6: return "experienced"; 
		case 7: return "fun"; 
		case 8: return "insane"; 
		case 9: return "impossible";
		default:return "-";
	}
}

//----------------------------------------------------------
void MenuGL::startMenu()
{
//	elecOffX	= elecStretch*0.4;
	elecOffX	= 0.0;
	textAngle	= 90.0;
	textCount	= 500;
	createLists( (thickText = true) );
	Global::cursorPos[0] = 0.0;
	Global::cursorPos[1] = 0.0;
	
}

//----------------------------------------------------------
void MenuGL::drawGL()
{
	Config	*config = Config::getInstance();
	Global	*game = Global::getInstance();
	HiScore	*hiScore = HiScore::getInstance();
	int		i;
		
	if(--textCount < 0)
	{
		textCount = 500;
//		textAngle = 360.0;
	}
	if(textAngle > 0.0) 
		textAngle -=  5.0;
	else		
		textAngle  =  0.0;
	
//	glClearColor(0.27451, 0.235294, 0.392157, 1.0);
	//-- Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Place camera
	glLoadIdentity();
	glTranslatef(0.0, 0.0, config->getZTrans());
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	//-- Draw background
	game->ground->drawGL();
	
	//-- Update audio
	game->audio->update();
	
	float	szx		=   9.0;
	float	szy		=   4.5;
	float	top		=   1.0;
	float	left	=  -8.0;
	float	inc		=  -txtHeight*2.5;
	
	//----- Draw credits texture --------------------------------
	glPushMatrix();
		// NOTE: corners of back tex is white, alpha 1 and
		// we are in modulate blend...
		glBindTexture(GL_TEXTURE_2D, backTex);
		glTexCoord2f(1.0, 0.0); 
		
		//-- darken
		glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 0.8);
			glVertex3f( szx,  szy+0.25-3.0, 10.0);
			glVertex3f(-szx,  szy+0.25-3.0, 10.0);
		glColor4f(0.0, 0.0, 0.0, 0.4);
			glVertex3f(-szx, -13.0, 10.0);
			glVertex3f( szx, -13.0, 10.0);
		glEnd();

		glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, mssgAlpha);
			glVertex3f( 16.0,  -10.7, 10.0);
			glVertex3f(-16.0,  -10.7, 10.0);
		glColor4f(0.0, 0.0, 0.0, mssgAlpha);
			glVertex3f(-16.0, -11.9, 10.0);
			glVertex3f( 16.0, -11.9, 10.0);
		glEnd();

		szx = 12.0;
		szy =  txtHeight*0.5;
		glPushMatrix();
		glTranslatef(left, top+(inc*curSel), 10.0);
		glBegin(GL_QUADS);
			glColor4f(0.5, 0.5, 1.0, 1.0);
			glVertex3f(-szx,  szy*3.0, 0.0);
			glVertex3f(-szx, -szy, 0.0);
			glColor4f(0.2, 0.2, 1.0, 0.0);
			glVertex3f( szx, -szy, 0.0);
			glVertex3f( szx,  szy*3.0, 0.0);
		glEnd();
		drawIndicator();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		drawElectric();
		glPopMatrix();
		txfBindFontTexture(game->texFont);
		glColor4f(1.0, 1.0, 1.0, 0.9);
		float sc = 0.035;
		for(i = 0; i < NumSelections; i++)
		{
			glPushMatrix();
			glTranslatef(left, top+(inc*i), 10.0);
			glRotatef(textAngle, 1.0, 0.0, 0.0);
			glScalef(sc, sc*0.75, 1.0);
			txfRenderString(game->texFont, menuText[i], strlen(menuText[i]));
			glPopMatrix();
		}
		
		{
			float f = (float)-game->frame;
			float r = cos(f*0.02);
			glPushMatrix();
//			glColor4f(1.0+r, r, r, 0.6+0.2*r);
			glColor4f(1.0, 1.0, 1.0, 0.6+0.2*r);
			glTranslatef(-18.75, -8.5, 0.0);
			glScalef(sc, sc*0.75, 1.0);
			txfRenderString(game->texFont, "high scores", 11);
			glTranslatef(-100.0, -30.0, 0.0);
			char buf[16];
			int i,len;
			float trans;
			time_t nowTime = time(NULL);
			int l = config->getIntSkill();
			int recentHiScore = -1;
			time_t mostRecent = 0;
			for(i = 0; i < HI_SCORE_HIST; i++)
			{
				if(	hiScore->getDate(l, i) > nowTime-300 && // highlight score for 5 minutes (300)
					hiScore->getDate(l, i) > mostRecent )
				{
					recentHiScore = i;
					mostRecent = hiScore->getDate(l, i);
				}
			}
			for(i = 0; i < HI_SCORE_HIST; i++)
			{
				f += 30.0;
				r = cos(f*0.02);
				if(i == recentHiScore)
					glColor4f(1.5, 0.5, 0.5, 0.6+0.1*r);
				else
					glColor4f(1.0, 1.0, 1.0, 0.2+0.2*r);
//				glColor4f(0.5+r*0.5, 0.5, 0.25-r*0.25, 0.2+0.2*r);
				sprintf(buf, "%d", (int)hiScore->getScore(config->getIntSkill(), i) );
				len = strlen(buf);
				trans = txfStringLength(game->texFont, buf, len);
				glTranslatef( 80-trans, 0.0, 0.0);
				txfRenderString(game->texFont, buf, len);
				glTranslatef(-80, -30.0, 0.0);
			}
			glPopMatrix();
		}
		
		//---- credits
		if(true)
		{
			int		 n = 0;
			float	alpha;
			static double c = 0.0;
			if(c > 75.0)
			{
				c = 0.0;
			}
			c += 0.2;
			glPushMatrix();
			if(c > 25)	alpha = 0.4*(75.0-c)/50.0;
			else		alpha = 0.4;
			glColor4f(1.0, 1.0, 1.0, alpha);
			sc = 0.03;
			glTranslatef(14.0, -11.5, 0.0);
			glScalef(sc, sc, 1.0);
			glTranslatef(-c*1.5, c, 0.0);
			if(c < 3)	n = (int)c;
			else		n = 3;
			txfRenderString(game->texFont, "the", n);
			glTranslatef(c-txfStringLength(game->texFont, "the", n), -38+c, 0.0);
			if(c < 10)	n = (int)(c-3);
			else		n = 7;
			txfRenderString(game->texFont, "reptile", n);
			glTranslatef(c-txfStringLength(game->texFont, "reptile", n), -38+c, 0.0);
			if(c < 16)	n = (int)c-10;
			else		n = 6;
			txfRenderString(game->texFont, "labour", n);
			glTranslatef(c-txfStringLength(game->texFont, "labour", n), -38+c, 0.0);
			if(c < 23)	n = (int)(c-16);
			else		n = 7;
			txfRenderString(game->texFont, "project", n);
			// font height is 23
			glPopMatrix();
		}
		
		//-------- draw help message
		if(mssgAlpha > 0.0)
		{
			if(mssgHelpOverride)
				glColor4f(1.3, mssgAlpha, mssgAlpha, mssgAlpha);
			else
				glColor4f(0.5, 0.5, 0.9, (0.2+mssgAlpha));		
			sc = 0.042;
			glTranslatef(-19.5, -14.0, 0.0);
			glScalef(sc, sc*0.75, 1.0);
			unsigned int	ti = (unsigned int)(112.0*mssgAlpha);
			if(ti > strlen(mssgText))
				ti = strlen(mssgText);
			txfRenderString(game->texFont, mssgText, ti);
			mssgAlpha -= 0.004;
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
		
		
		
	glPopMatrix();
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glPushMatrix();
	glTranslatef(0.0, 4.75, 25.0);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDepthMask(GL_FALSE);	//XXX Hack to make Voodoo3 XF4 work
	drawTitleBack();
	glDepthMask(GL_TRUE);	//XXX Hack to make Voodoo3 XF4 work
	drawTitle();
	glPopMatrix();
	
//	//-- draw cursor...
//	{
//		float x = Global::cursorPos[0]*16.60;
//		float y = Global::cursorPos[1]*12.45;
//		float z = 10.0;
//		float sz;
//		glBindTexture(GL_TEXTURE_2D, csrTex);
//		sz = 0.2;
//		glBegin(GL_QUADS);
//		glTexCoord2f(1.0, 1.0); glVertex3f( x+sz, y+sz, z);
//		glTexCoord2f(0.0, 1.0); glVertex3f( x-sz, y+sz, z);
//		glTexCoord2f(0.0, 0.0); glVertex3f( x-sz, y-sz, z);
//		glTexCoord2f(1.0, 0.0); glVertex3f( x+sz, y-sz, z);
//		glEnd();
//	}
	
	if(thickText && game->fps < 30)
	{
		//fprintf(stderr, "ATTENTION: Using 'thin' text to improve framerate...\n");
		createLists( (thickText = false) );	
	}
	if(!thickText && game->fps > 40)
	{
		//fprintf(stderr, "ATTENTION: Reverting to 'thick' text...\n");
		createLists( (thickText = true) );	
	}
	
	//---------- Help messages
	if(mssgHelpOverride && mssgAlpha < 0.05)
	{
		mssgHelpOverride = false;
		mssgCount = 0;
	}
	if(!mssgHelpOverride)
	{
		int interval = (mssgCount++)%500;
		if(!interval)
		{
			strcpy(mssgText, mssgHelpText[mssgIndex%NumMssg]);
			mssgIndex++;
		}
		if(interval < 150)
		{
			mssgAlpha = interval/150.0;
		}
	}
}

//----------------------------------------------------------
void MenuGL::drawIndicator()
{
	Config	*config = Config::getInstance();
	Global	*game = Global::getInstance();
	char	buf[32];
	float	szx = 10.0;
	float	szy = txtHeight;
	float	sud = szy*2.0;
	float	level = 0.0;
	float	sc = 0.025;
	int		tmp;
	switch(curSel)
	{
		case GameLevel: 
			level = game->gameLevel/9.0;
			sprintf(buf, "%d", game->gameLevel);	 
			break;
		case SkillLevel: 
			level = config->getGameSkillBase();
			tmp = (int)((level+0.05)*10.0);
			sprintf(buf, skillString(tmp));
			break;
		case Graphics: 
			level = config->getGfxLevel()/2.0;
			switch(config->getGfxLevel())
			{
				case 0: sprintf(buf, "low"); break;
				case 1: sprintf(buf, "med"); break;
				case 2: sprintf(buf, "high"); break;
			}
			break;
		case ScreenSize: 
			level = (float)config->getScreenSize()/(float)MAX_SCREEN_SIZE; 
			switch(config->getScreenSize())
			{
				case 0: sprintf(buf, "512x384"); break;
				case 1: sprintf(buf, "640x480"); break;
				case 2: sprintf(buf, "800x600"); break;
				case 3: sprintf(buf, "1024x768"); break;
				case 4: sprintf(buf, "1280x960"); break;
			}
			break;
		case FullScreen: 
			level = (float)config->getFullScreen(); 
			if(config->getFullScreen()) sprintf(buf, "true");
			else sprintf(buf, "false");
			break;
		case Sound: 
			level = config->getVolSound(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case Music: 
			level = config->getVolMusic(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case MouseSpeed: 
			level = config->getMouseSpeed()*10.0; 
			sprintf(buf, "%d", (int)((level+0.005)*100.0));	 
			break;
		default: 
			level = -5.0;
			break;
	}
	glPushMatrix();
		glTranslatef(0.0, -txtHeight, 0.0);
		glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 0.3);
			glVertex3f(szx+szy, szy, 0.0);
			glVertex3f(   -szx, szy, 0.0);
			glColor4f(1.0, 1.0, 1.0, 0.15);
			glVertex3f(   -szx, 0.0, 0.0);
			glVertex3f(    szx, 0.0, 0.0);
		glEnd();
		
		//-- draw level indicator and/or text
		glPushMatrix();
		if(level > -1.0)
		{
			glBegin(GL_QUADS);
				glColor4f(1.0, 0.0, 0.0, 1.0);
				glVertex3f(szy+szx*level, szy, 0.0);
				glVertex3f(          0.0, szy, 0.0);
				glColor4f(0.0, 0.0, 0.0, 0.0);
				glVertex3f(          0.0, 0.0, 0.0);
				glVertex3f(    szx*level, 0.0, 0.0);
			glEnd();
			
			float udo = 2.5;
			glBindTexture(GL_TEXTURE_2D, updwnTex);
			glBegin(GL_QUADS);
				glColor4f(1.0, 1.0, 1.0, 0.7);
				glTexCoord2f(1.0, 0.0);	glVertex3f(	sud-udo, szy, 0.0);
				glTexCoord2f(0.0, 0.0);	glVertex3f(	0.0-udo, szy, 0.0);
				glTexCoord2f(0.0, 1.0);	glVertex3f(	0.0-udo, 0.0, 0.0);
				glTexCoord2f(1.0, 1.0);	glVertex3f( sud-udo, 0.0, 0.0);
			glEnd();
			
			glColor4f(1.0, 1.0, 1.0, 0.5);
			glTranslatef(11.0, 0.0, 0.0);
			glScalef(sc, sc, 1.0);
			txfBindFontTexture(game->texFont);
			txfRenderString(game->texFont, buf, strlen(buf));
		}
		glPopMatrix();
		
		//-- draw updown
		sc *= 1.5;
		glTranslatef(-2.5, 0.0, 0.0);
		glScalef(sc, sc, 1.0);
		//txfRenderString(game->texFont, "-|+", 3);
		
	glPopMatrix();
	
}
	
//----------------------------------------------------------
void MenuGL::drawElectric()
{
	float es = elecStretch;
	elecOffY = FRAND*0.7;
	if( (elecOffX+=0.175) > es)
		elecOffX = -5.0;
	float szx = 30.0;
	float szy = txtHeight;
	glBindTexture(GL_TEXTURE_2D, elecTex);
	glPushMatrix();
	glTranslatef(0.0, txtHeight*0.5, 0.0);
	glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glTexCoord2f(   elecOffX, elecOffY+0.3); glVertex3f( -8.0,  szy, 0.0);
		glTexCoord2f(   elecOffX,     elecOffY); glVertex3f( -8.0, -szy, 0.0);
		glColor4f(0.0, 0.0, 0.1, 1.0);
		glTexCoord2f(elecOffX-es,     elecOffY); glVertex3f(  szx, -szy, 0.0);
		glTexCoord2f(elecOffX-es, elecOffY+0.3); glVertex3f(  szx,  szy, 0.0);
	glEnd();
	glPopMatrix();
}
	
//----------------------------------------------------------
void MenuGL::drawTitleBack()
{
	float	clr_c[4] = { 1.0, 1.0, 1.0, 0.0 };
	float	clr_w[4] = { 1.0, 1.0, 1.0, 1.0 };
	float	w = 9.0;
	float	a = 2.0;
	float	h = 1.4;
	float	z = 0.5;
		
	float	as = a/(w+a);
	float	at = a/(h+a);
	
	glBindTexture(GL_TEXTURE_2D, backTex);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	static float amt = 0.0;
	glTranslatef(amt*0.1, amt*0.5, 0.0);
	glRotatef(-amt*100.0, 0.0, 1.0, 1.0);
	amt -= 0.01;
	//-- Top right
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,  h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  w+a,  h+a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,  h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f(  w  ,  h+a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0,  h+a, 0.0);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,	h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f(  w  , 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,	h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,	h, 0.0);
	glEnd();
	//-- Top left
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0,  h+a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,  h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f( -w  ,  h+a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,  h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f( -w-a,  h+a, 0.0);
	glEnd();		
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,	h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f( -w  , 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,	h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,	h, 0.0);
	glEnd();
	//-- bottom right
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,  -h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,  -h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f(  w  , 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  -h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, -h-a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a, -h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f(  w  , -h-a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  , -h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0, -h-a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0, -h  , 0.0);
	glEnd();
	//bottom left
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  -h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,  -h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f( -w  , 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,  -h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, 0.0, z);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0, -h-a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0, -h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f( -w  , -h-a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  , -h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, -h-a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a, -h  , 0.0);
	glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
}

//----------------------------------------------------------
void MenuGL::drawTitle()
{
	static int tiltCount = 600;
	static float ta0  = -60.0;
	static float ta1  = -90.0;
	float &tilt = titleTilt;
	if(ta0 < 90.0)	ta0 += 0.7;
	else if(!thickText) ta0 += 180.0;
	else ta0 += 5.0;
	if(ta0 > 270.0)	ta0 = ta0-360.0;
		
	if(ta1 < 90.0)	ta1 += 0.55;
	else if(!thickText) ta1 += 180.0;
	else ta1 += 8.0;
	if(ta1 > 270.0)	ta1 = ta1-360.0;
	
	if(thickText)
	{
		tiltCount--;
		if(tiltCount == 0)
			tilt = 360.0+tilt;
		else if(tiltCount < 0)
		{
			tilt -= 1.0;
			if(tilt < -10.0)
			{
				tiltCount = 1500;
				tilt = -10.0;
			}
		}
		else
			tilt -= 0.01;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glBindTexture(GL_TEXTURE_2D, envTex);
		glPushMatrix();
			glTranslatef(0.0,  1.0, 0.0);
			glRotatef( tilt, 1.0, 0.0, 0.0);
			glRotatef(   ta0, 0.0, 1.0, 0.0);
			glCallList(listChrom);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0, -1.0, 0.0);
			glRotatef( tilt, 1.0, 0.0, 0.0);
			glRotatef(   ta1, 0.0, 1.0, 0.0);
			glCallList(listBSU);
		glPopMatrix();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------
void MenuGL::keyHit(MainToolkit::Key key)
{
	switch(key)
	{
		case MainToolkit::KeyUp:
			if(curSel == (MenuSelection)0)
				curSel = (MenuSelection)(NumSelections-1);
			else
				curSel = (MenuSelection)(curSel - 1);
//			curSel = (MenuSelection)(curSel - 1);
//			if(curSel < (MenuSelection)0)
//				curSel = (MenuSelection)(NumSelections-1);
			elecOffX = 0.0;
			break;
		case MainToolkit::KeyDown:
			curSel = (MenuSelection)((curSel+1)%NumSelections);
			elecOffX = 0.0;
			break;
		case MainToolkit::KeyLeft:
			decItem();
			break;
		case MainToolkit::KeyRight:
			incItem();
			break;
		case MainToolkit::KeyEnter:
			activateItem();
			break;
		default:
			break;
	}
}

//----------------------------------------------------------
void MenuGL::activateItem()
{
	Config	*config = Config::getInstance();
	Global *game = Global::getInstance();
	switch(curSel)
	{
		case NewGame:
			game->gameMode = Global::Game;
			game->newGame();
			game->toolkit->grabMouse(true);
			game->audio->setMusicMode(Audio::MusicGame);
			break;
		case SkillLevel:
			break;
		case GameLevel:
			break;
		case Graphics:
			break;
		case ScreenSize:
			break;
		case FullScreen:
			config->setFullScreen(!config->getFullScreen());
			game->deleteTextures();
			game->toolkit->setVideoMode();
			game->loadTextures();
			break;
		case Sound:
			break;
		case Music:
			break;
		case MouseSpeed:
			game->gameMode = Global::Game;
			game->toolkit->grabMouse(true);
			game->audio->setMusicMode(Audio::MusicGame);
			break;
		case Quit:
			game->game_quit = true;
			break;
		case NumSelections:
			break;
	}
}

//----------------------------------------------------------
void MenuGL::incItem()
{
	Config	*config = Config::getInstance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			activateItem();
			break;
		case SkillLevel:
			config->setGameSkillBase(config->getGameSkillBase()+0.1);
			hiScore->print(config->getIntSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel++;
			if(game->gameLevel > config->getMaxLevel()) 
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, "---- you must complete level %d before you can select level %d ----", config->getMaxLevel(), game->gameLevel);
				game->gameLevel = config->getMaxLevel();
			}
			else
				game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->getGfxLevel()+1);
			break;
		case ScreenSize:
			config->setScreenSize(config->getScreenSize()+1);
			game->deleteTextures();
			game->toolkit->setVideoMode();
			game->loadTextures();
			break;
		case FullScreen:
			if(!config->getFullScreen())
			{
				config->setFullScreen(true);
				game->deleteTextures();
				game->toolkit->setVideoMode();
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->getVolSound()+0.05);
			game->audio->setSoundVolume(config->getVolSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->getVolMusic()+0.05);
			game->audio->setMusicVolume(config->getVolMusic());
			break;
		case MouseSpeed:
			config->setMouseSpeed(config->getMouseSpeed()+0.005);
			break;
		case Quit:
			activateItem();
			break;
		case NumSelections:
			break;
	}
}

//----------------------------------------------------------
void MenuGL::decItem()
{
	Config	*config = Config::getInstance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			break;
		case SkillLevel:
			config->setGameSkillBase(config->getGameSkillBase()-0.1);
			hiScore->print(config->getIntSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel -= 1;
			if(game->gameLevel < 1) 
				game->gameLevel = 1;
			game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->getGfxLevel()-1);
			break;
		case ScreenSize:
			config->setScreenSize(config->getScreenSize()-1);
			game->deleteTextures();
			game->toolkit->setVideoMode();
			game->loadTextures();
			break;
		case FullScreen:
			if(config->getFullScreen())
			{
				config->setFullScreen(false);
				game->deleteTextures();
				game->toolkit->setVideoMode();
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->getVolSound()-0.05);
			game->audio->setSoundVolume(config->getVolSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->getVolMusic()-0.05);
			game->audio->setMusicVolume(config->getVolMusic());
			break;
		case MouseSpeed:
			config->setMouseSpeed(config->getMouseSpeed()-0.005);
			break;
		case Quit:
			break;
		case NumSelections:
			break;
	}
}

/**
 * horrible, hacky, but quick to implement....
 */
//----------------------------------------------------------
void MenuGL::mousePress(MainToolkit::Button but, int xi, int yi)
{
	if(but == MainToolkit::Left)
	{
		float x,y;
		Config *config = Config::getInstance();
		x = -2.0*(0.5-(((float)xi)/config->getScreenW()))* 16.60;
		y =  2.0*(0.5-(((float)yi)/config->getScreenH()))* 12.45;

		float	p = -y+(1.0+txtHeight*1.5);
		float	s = txtHeight*2.5;
		int		mSel = -1;
		if(p > 0.0)
		{
			// reset electric 
			elecOffX = 0.0;

			p = p/s;
			mSel = (int)floor(p);
			if( mSel >= 0 && mSel < (int)NumSelections)
			{
				if(mSel != (int)curSel)
				{
					curSel = (MenuSelection)mSel;
					elecOffX = 0.0;
					mSel = -1;
				}
			}
		}
		float l = -8.0 + -2.5;
		if(mSel >= 0)
		{
			if(x > l && x < l+txtHeight )
				decItem();
			else if (x > l+txtHeight && x < l+txtHeight*2.0)
				incItem();
			else
				activateItem();
		}
	}
}

