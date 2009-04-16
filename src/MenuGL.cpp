/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"

#include "MenuGL.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

#include "compatibility.h"

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#include <glpng/glpng.h>
#else
#include <GL/gl.h>
#include <GL/glpng.h>
#endif

#include "Config.h"

#include "extern.h"
#include "define.h"
#include "Global.h"
#include "HiScore.h"
#include "Ground.h"
#include "Audio.h"

#include "textGeometry.h"

static const int NumMssg = 7;
static char mssgHelpText[NumMssg][128] = {
N_("  d o   n o t   a l l o w  -a n y-   e n e m i e s   g e t   p a s t   y o u !"),
N_("  e v e r y   e n e m y   t h a t   g e t s   b y   c o s t s   y o u   a   l i f e !"),
N_("  a l l o w   p o w e r - u p s   t o   p a s s   b y   f o r   b i g   p o i n t s !"),
N_("  c r a s h   i n t o   e n e m i e s   t o   d e s t r o y   t h e m !"),
N_("  r i g h t   c l i c k   t w i c e   t o   s e l f - d e s t r u c t !"),
N_("  s e l f - d e s t r u c t   t o   p r e s e r v e   y o u r   a m m u n i t i o n !"),
N_("  d o w n l o a d   C h r o m i u m   B. S. U.   a t   http://chromium-bsu.sf.net/"),
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
	
	butHeight	= 0.5;
	butWidth	= butHeight*4.0;
	butOffset	= 3.05;

	menuText[NewGame]	= _("n e w    g a m e");
	menuText[GameLevel]	= _("l e v e l");
	menuText[SkillLevel]= _("s k i l l");
	menuText[Graphics]	= _("g f x    d e t a i l");
	menuText[FullScreen]= _("f u l l s c r e e n");
	menuText[ScreenSize]= _("s c r e e n    s i z e");
	menuText[Sound]		= _("s o u n d    f x    v o l u m e");
	menuText[Music]		= _("m u s i c    v o l u m e");
	menuText[MouseSpeed]= _("m o u s e   s p e e d");
	menuText[Quit]		= _("q u i t");
	
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
	Config* config = Config::instance();

	if(!thick)
	{
		if( config->debug() ) fprintf(stderr, _("ATTENTION: Using 'thin' text to improve framerate...\n"));
	}
	titleTilt	= -10.0;
	
	if( config->debug() ) fprintf(stderr, _("MenuGL::createLists\n"));

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
		case 2: return _("fish in a barrel");
		case 3: return _("whimp");
		case 4: return _("easy");
		case 5: return _("normal");
		case 6: return _("experienced");
		case 7: return _("fun");
		case 8: return _("insane");
		case 9: return _("impossible");
		default:return _("-");
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
	Config	*config = Config::instance();
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
	glTranslatef(0.0, 0.0, config->zTrans());
	
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
		glColor4f(1.0, 1.0, 1.0, 0.9);
		float sc = 0.035;
		for(i = 0; i < NumSelections; i++)
		{
			glPushMatrix();
			glTranslatef(left, top+(inc*i), 10.0);
			glRotatef(textAngle, 1.0, 0.0, 0.0);
			glScalef(sc, sc*0.75, 1.0);
			game->text->Render(menuText[i]);
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
			game->text->Render(_("high scores"));
			glTranslatef(-100.0, -30.0, 0.0);
			char buf[16];
			int i,len;
			float trans;
			time_t nowTime = time(NULL);
			int l = config->intSkill();
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
				sprintf(buf, "%d", (int)hiScore->getScore(config->intSkill(), i) );
				len = strlen(buf);
				trans = 80.0 + game->text->Advance(_("high scores")) - game->text->Advance(buf);
				glTranslatef( trans, 0.0, 0.0 );
				game->text->Render(buf);
				glTranslatef( -trans, -30.0, 0.0) ;
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
			if(n>0) game->text->Render(_("the"), n);
			glTranslatef(c, -38+c, 0.0);
			if(c < 10)	n = (int)(c-3);
			else		n = 7;
			if(n>0) game->text->Render(_("reptile"), n);
			glTranslatef(c, -38+c, 0.0);
			if(c < 16)	n = (int)c-10;
			else		n = 6;
			if(n>0) game->text->Render(_("labour"), n);
			glTranslatef(c, -38+c, 0.0);
			if(c < 23)	n = (int)(c-16);
			else		n = 7;
			if(n>0) game->text->Render(_("project"), n);
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
			size_t len = mbstowcs(NULL,mssgText,0);
			unsigned int	ti = (unsigned int)(112.0*mssgAlpha);
			if(ti > len) ti = len;
			if(ti) game->text->Render(mssgText, ti);
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
		if( config->debug() ) fprintf(stderr, _("ATTENTION: Using 'thin' text to improve framerate...\n"));
		createLists( (thickText = false) );	
	}
	if(!thickText && game->fps > 40)
	{
		if( config->debug() ) fprintf(stderr, _("ATTENTION: Reverting to 'thick' text...\n"));
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
			strcpy(mssgText, gettext(mssgHelpText[mssgIndex%NumMssg]));
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
	Config	*config = Config::instance();
	Global	*game = Global::getInstance();
	char	buf[64];
	float	szx = 10.0;
	float	szy = txtHeight;
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
			level = config->gameSkillBase();
			tmp = (int)((level+0.05)*10.0);
			sprintf(buf, skillString(tmp));
			break;
		case Graphics: 
			level = config->gfxLevel()/2.0;
			switch(config->gfxLevel())
			{
				case 0: sprintf(buf, _("low")); break;
				case 1: sprintf(buf, _("med")); break;
				case 2: sprintf(buf, _("high")); break;
			}
			break;
		case ScreenSize: 
			level = (float)config->screenSize()/(float)MAX_SCREEN_SIZE; 
			switch(config->screenSize())
			{
				case 0: sprintf(buf, _("512x384")); break;
				case 1: sprintf(buf, _("640x480")); break;
				case 2: sprintf(buf, _("800x600")); break;
				case 3: sprintf(buf, _("1024x768")); break;
				case 4: sprintf(buf, _("1280x960")); break;
			}
			break;
		case FullScreen: 
			level = (float)config->fullScreen(); 
			if(config->fullScreen()) sprintf(buf, _("true"));
			else sprintf(buf, _("false"));
			break;
		case Sound: 
			level = config->volSound(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case Music: 
			level = config->volMusic(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case MouseSpeed: 
			level = config->mouseSpeed()*10.0; 
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
			
			//-- draw +/- buttons ---
			float	bx = butWidth;
			float	by = butHeight;
			float	bo = butOffset;
			glBindTexture(GL_TEXTURE_2D, updwnTex);
			glBegin(GL_QUADS);
				glColor4f(1.0, 1.0, 1.0, 0.6);
				glTexCoord2f(1.0, 0.0);	glVertex3f(	 bx-bo,  by, 0.0);
				glTexCoord2f(0.0, 0.0);	glVertex3f(	0.0-bo,  by, 0.0);
				glTexCoord2f(0.0, 1.0);	glVertex3f(	0.0-bo, 0.0, 0.0);
				glTexCoord2f(1.0, 1.0);	glVertex3f(  bx-bo, 0.0, 0.0);
			glEnd();
			
			glColor4f(1.0, 1.0, 1.0, 0.5);
			glTranslatef(11.0, 0.0, 0.0);
			glScalef(sc, sc, 1.0);
			game->text->Render(buf);
		}
		glPopMatrix();
		
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
	Config	*config = Config::instance();
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
			config->setFullScreen(!config->fullScreen());
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				if( config->fullScreen() )
					sprintf(mssgText, _("---- error setting full screen mode ----"));
				else
					sprintf(mssgText, _("---- error setting window mode ----"));
				config->setFullScreen(!config->fullScreen());
			}
			game->loadTextures();
			break;
		case Sound:
			break;
		case Music:
			break;
		case MouseSpeed:
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
	Config	*config = Config::instance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			activateItem();
			break;
		case SkillLevel:
			config->setGameSkillBase(config->gameSkillBase()+0.1);
			if( config->debug() ) hiScore->print(config->intSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel++;
			if(game->gameLevel > config->maxLevel()) 
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- you must complete level %d before you can select level %d ----"), config->maxLevel(), game->gameLevel);
				game->gameLevel = config->maxLevel();
			}
			else
				game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->gfxLevel()+1);
			break;
		case ScreenSize:
			config->setScreenSize(config->screenSize()+1);
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- error setting screen size ----"));
				config->setScreenSize(config->screenSize()-1);
			}
			game->loadTextures();
			break;
		case FullScreen:
			if(!config->fullScreen())
			{
				config->setFullScreen(true);
				game->deleteTextures();
				if( !game->toolkit->setVideoMode() )
				{
					mssgHelpOverride = true;
					mssgAlpha = 1.1;
					sprintf(mssgText, _("---- error setting full screen mode ----"));
					config->setFullScreen(false);
				}
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->volSound()+0.05);
			game->audio->setSoundVolume(config->volSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->volMusic()+0.05);
			game->audio->setMusicVolume(config->volMusic());
			break;
		case MouseSpeed:
			config->setMouseSpeed(config->mouseSpeed()+0.005);
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
	Config	*config = Config::instance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			break;
		case SkillLevel:
			config->setGameSkillBase(config->gameSkillBase()-0.1);
			if( config->debug() ) hiScore->print(config->intSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel -= 1;
			if(game->gameLevel < 1) 
				game->gameLevel = 1;
			game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->gfxLevel()-1);
			break;
		case ScreenSize:
			config->setScreenSize(config->screenSize()-1);
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- error setting screen size ----"));
				config->setScreenSize(config->screenSize()+1);
			}
			game->loadTextures();
			break;
		case FullScreen:
			if(config->fullScreen())
			{
				config->setFullScreen(false);
				game->deleteTextures();
				if( !game->toolkit->setVideoMode() )
				{
					mssgHelpOverride = true;
					mssgAlpha = 1.1;
					sprintf(mssgText, _("---- error setting full screen mode ----"));
					config->setFullScreen(true);
				}
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->volSound()-0.05);
			game->audio->setSoundVolume(config->volSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->volMusic()-0.05);
			game->audio->setMusicVolume(config->volMusic());
			break;
		case MouseSpeed:
			config->setMouseSpeed(config->mouseSpeed()-0.005);
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
		Config *config = Config::instance();
		x = -2.0*(0.5-(((float)xi)/config->screenW()))* 16.60;
		y =  2.0*(0.5-(((float)yi)/config->screenH()))* 12.45;

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
		float l  = -8.0 - butOffset;
		float hw = butWidth*0.5;
		if(mSel >= 0)
		{
			if(x > l && x < l+hw )
				decItem();
			else if (x > l+hw && x < l+butWidth)
				incItem();
			else
				activateItem();
		}
	}
}

