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

#include "Config.h"
#include "extern.h"
#include "HiScore.h"

//====================================================================
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include "Audio.h"
#include "Ground.h"
#include "MenuGL.h"
#include "MainGL.h"
#include "PowerUps.h"
#include "HeroAmmo.h"
#include "EnemyAmmo.h"
#include "EnemyFleet.h"
#include "Explosions.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"
#include "StatusDisplay.h"

#include "GroundMetal.h"

Config	*Config::m_instance = 0;

/**
 * initialize variables, then read config file
 */
//----------------------------------------------------------
Config::Config()
{
	//-- setup defaults
	m_screenW		= 800;
	m_screenH		= 600;
	m_screenA		=   1.3333333f;
	m_screenFOV		=  30.0f;
	m_screenNear	=  10.0;
	m_screenFar		=  70.0;
	m_full_screen	=  false;
	m_screenSize	=  2;
	m_screenBound[0] = 11.0;
	m_screenBound[1] =  9.0;
	m_zTrans		= -56.5;

	m_blend_enabled	= true;
	m_tex_border	= true;
	
	m_audio_enabled	= true;
	m_swap_stereo	= false;
	m_show_fps		= true;
	m_auto_speed	= false;
	m_true_color	= false;
	m_use_playList	= false;
	m_use_cdrom		= false;
	
	m_cdromDevice	= 0;
	m_cdromCount	= 1;

	m_maxLevel		= 1;
	m_gfxLevel		= 2;

	m_mouseSpeed	=  0.03;
	m_gameSkillBase	=  0.5;
	m_viewGamma		=  1.1;
	m_volSound		=  0.9;
	m_volMusic		=  0.5;

	m_audioType		= AudioOpenAL;
	m_textType		= TextGLC;
	
	readFile();
}

/**
 * save file before we go away...
 */
Config::~Config()
{
	saveFile();
}

/**
 * create single Config object
 * @returns Config::m_instance
 */
//----------------------------------------------------------
Config *Config::init()
{
	if(!Config::m_instance)
	{
		Config::m_instance = new Config;
	}
	else
	{
		fprintf(stderr, _("WARNING: Config::init() has already been called.\n"));
	}
	return Config::m_instance;
}

/**
 * @returns Config::m_instance
 */
//----------------------------------------------------------
Config *Config::instance()
{
	if(!Config::m_instance)
	{
		return Config::init();
	}
	else
		return Config::m_instance;
}

/**
 * deletes singleton instance and sets Config::m_instance to 0.
 */
//----------------------------------------------------------
void Config::destroy()
{
	delete Config::m_instance;
	Config::m_instance = 0;
}

/**
 * just a quickie
 */ 
//----------------------------------------------------------
bool Config::readFile()
{
	int		i;
	int		numLines;
	bool	retVal = true;
	char	configFilename[256];
	FILE	*file;
	int		tmp;
	const char *homeDir = getenv("HOME");
	char	configStrings[32][64];

	if(!homeDir)
		homeDir = "./";

	sprintf(configFilename, "%s/%s", homeDir, CONFIG_FILE);
	alterPathForPlatform(configFilename);
	file = fopen(configFilename, "r");
	if(file)
	{
#ifdef HAVE_LOCALE_H
		char* locale = setlocale(LC_NUMERIC,"C");
#endif
		i = numLines = 0;
		while( fgets(configStrings[i], 64, file) )
			i++;
		numLines = i;
		for(i = 0; i < numLines; i++)
		{
			if(strncmp(configStrings[i], "screenSi", 8) == 0) { sscanf(configStrings[i], "screenSize %d\n",    &m_screenSize); }
			if(strncmp(configStrings[i], "mouseSpe", 8) == 0) { sscanf(configStrings[i], "mouseSpeed %f\n",    &m_mouseSpeed); }
			if(strncmp(configStrings[i], "gameSkil", 8) == 0) { sscanf(configStrings[i], "gameSkillBase %f\n", &m_gameSkillBase); }
			if(strncmp(configStrings[i], "gfxLevel", 8) == 0) { sscanf(configStrings[i], "gfxLevel %d\n",      &m_gfxLevel);   }
			if(strncmp(configStrings[i], "volSound", 8) == 0) { sscanf(configStrings[i], "volSound %f\n",      &m_volSound);   }
			if(strncmp(configStrings[i], "volMusic", 8) == 0) { sscanf(configStrings[i], "volMusic %f\n",      &m_volMusic);   }
			if(strncmp(configStrings[i], "full_scr", 8) == 0) { sscanf(configStrings[i], "full_screen %d\n",  &tmp);	m_full_screen = (bool)tmp; }
			if(strncmp(configStrings[i], "true_col", 8) == 0) { sscanf(configStrings[i], "true_color %d\n",   &tmp);	m_true_color  = (bool)tmp; }
			if(strncmp(configStrings[i], "swap_ste", 8) == 0) { sscanf(configStrings[i], "swap_stereo %d\n",  &tmp);	m_swap_stereo = (bool)tmp;  }
			if(strncmp(configStrings[i], "auto_spe", 8) == 0) { sscanf(configStrings[i], "auto_speed %d\n",   &tmp);	m_auto_speed  = (bool)tmp;  }
			if(strncmp(configStrings[i], "show_fps", 8) == 0) { sscanf(configStrings[i], "show_fps %d\n",     &tmp);	m_show_fps    = (bool)tmp;  }
			if(strncmp(configStrings[i], "use_play", 8) == 0) { sscanf(configStrings[i], "use_playList %d\n", &tmp);	m_use_playList= (bool)tmp;  }
			if(strncmp(configStrings[i], "use_cdro", 8) == 0) { sscanf(configStrings[i], "use_cdrom %d\n",    &tmp);	m_use_cdrom   = (bool)tmp;  }
			if(strncmp(configStrings[i], "audioTyp", 8) == 0) { sscanf(configStrings[i], "audioType %d\n",    &tmp);	m_audioType = (AudioType)tmp; }
			if(strncmp(configStrings[i], "textType", 8) == 0) { sscanf(configStrings[i], "textType %d\n",    &tmp);	m_textType = (TextType)tmp; }
			if(strncmp(configStrings[i], "maxLevel", 8) == 0) { sscanf(configStrings[i], "maxLevel %d\n",      &m_maxLevel);  }
			if(strncmp(configStrings[i], "viewGamm", 8) == 0) { sscanf(configStrings[i], "viewGamma %f\n",     &m_viewGamma); }
			if(strncmp(configStrings[i], "cdromCou", 8) == 0) { sscanf(configStrings[i], "cdromCount %d\n",    &m_cdromCount); }
			if(strncmp(configStrings[i], "cdromDev", 8) == 0) { sscanf(configStrings[i], "cdromDevice %d\n",   &m_cdromDevice); }
		}
#ifdef HAVE_LOCALE_H
		setlocale(LC_NUMERIC,locale);
#endif
	}
	else
	{
		fprintf(stderr, _("WARNING: could not read config file (%s)\n"), configFilename);
		retVal = false;
	}
	
	//-- update all screen size variables
	setScreenSize(m_screenSize);
	
	return retVal;
}

/**
 * save config opts
 */
//----------------------------------------------------------
bool Config::saveFile()
{
	bool retVal;
	char	configFilename[256];
	FILE	*file;
	const char *homeDir = getenv("HOME");

	if(!homeDir)
		homeDir = "./";
		
	sprintf(configFilename, "%s/%s", homeDir, CONFIG_FILE);
	alterPathForPlatform(configFilename);
	file = fopen(configFilename, "w");
	if(file)
	{
#ifdef HAVE_LOCALE_H
		char* locale = setlocale(LC_NUMERIC,"C");
#endif
		fprintf(file, "use_playList %d\n",	(int)m_use_playList);
		fprintf(file, "use_cdrom %d\n",		(int)m_use_cdrom);
		fprintf(file, "full_screen %d\n", 	(int)m_full_screen);
		fprintf(file, "true_color %d\n", 	(int)m_true_color);
		fprintf(file, "swap_stereo %d\n",	(int)m_swap_stereo);
		fprintf(file, "auto_speed %d\n",	(int)m_auto_speed);
		fprintf(file, "show_fps %d\n",		(int)m_show_fps);
		fprintf(file, "screenSize %d\n",	m_screenSize);
		fprintf(file, "gfxLevel %d\n",		m_gfxLevel);
		fprintf(file, "gameSkillBase %g\n",	m_gameSkillBase);
		fprintf(file, "mouseSpeed %g\n",	m_mouseSpeed);
		fprintf(file, "maxLevel %d\n",		m_maxLevel);
		fprintf(file, "volSound %g\n",		m_volSound);
		fprintf(file, "volMusic %g\n",		m_volMusic);
		fprintf(file, "viewGamma %g\n",		m_viewGamma);
		fprintf(file, "audioType %d\n",		(int)m_audioType);
		fprintf(file, "textType %d\n",		(int)m_textType);
		fprintf(file, "cdromCount %d\n",	m_cdromCount);
		fprintf(file, "cdromDevice %d\n",	m_cdromDevice);
#ifdef HAVE_LOCALE_H
		setlocale(LC_NUMERIC,locale);
#endif

		fclose(file);
		fprintf(stderr, _("wrote config file (%s)\n"), configFilename);
		retVal = true;
	}
	else
	{
		fprintf(stderr, _("WARNING: could not write config file (%s)\n"), configFilename);
		retVal = false;
	}
	
	
	return retVal;
}

/**
 * given screen size, set all other variables.
 */
//----------------------------------------------------------
void Config::setScreenSize(int m)
{
	m_screenSize = m;
	if(m_screenSize > MAX_SCREEN_SIZE)
		m_screenSize = MAX_SCREEN_SIZE;
	if(m_screenSize < MIN_SCREEN_SIZE)
		m_screenSize = MIN_SCREEN_SIZE;
	switch(m_screenSize)
	{
		case 0:
			m_screenW = 512;
			m_screenH = 384;
			break;
		case 1:
			m_screenW = 640;
			m_screenH = 480;
			break;
		case 2:
			m_screenW = 800;
			m_screenH = 600;
			break;
		case 3:
			m_screenW = 1024;
			m_screenH = 768;
			break;
		case 4:
			m_screenW = 1280;
			m_screenH = 960;
			break;
		default:
			m_screenSize = 1;
			m_screenW = 640;
			m_screenH = 480;
			break;
	}
	m_screenA = (float)m_screenW/(float)m_screenH;
}

