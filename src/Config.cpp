/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Config.h"
#include "extern.h"
#include "HiScore.h"

//====================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Config	*Config::instance = 0;

/**
 * initialize variables, then read config file
 */
//----------------------------------------------------------
Config::Config()
{
	//-- setup defaults
	screenW		= 800;
	screenH		= 600;
	screenA		=   1.3333333f;
	screenFOV	=  30.0f;
	screenNear	=  10.0;
	screenFar	=  70.0;
	full_screen	=  false;
	screenSize	=  2;
	screenBound[0] = 11.0;
	screenBound[1] =  9.0;
	zTrans		= -56.5;
	blend_enabled = true;
	
	audio_enabled	= true;
	swap_stereo		= false;
	show_fps		= true;
	auto_speed		= false;
	true_color		= false;
	use_playList	= false;
	use_cdrom		= true;
	
	cdromDevice		= 0;
	cdromCount		= 1;

	maxLevel		= 1;
	gfxLevel		= 2;

	mouseSpeed		=  0.03;
	gameSkillBase	=  0.5;
	viewGamma		=  1.1;
	volSound		=  0.9;
	volMusic		=  0.5;

	audioType	= AudioOpenAL;
	
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
 * @returns Config::instance
 */
//----------------------------------------------------------
Config *Config::init()
{
	if(!Config::instance)
	{
		Config::instance = new Config;
	}
	else
	{
		fprintf(stderr, "WARNING: Config::init() has already been called.\n");
	}
	return Config::instance;
}

/**
 * @returns Config::instance
 */
//----------------------------------------------------------
Config *Config::getInstance()
{
	if(!instance)
	{
		return Config::init();
	}
	else
		return Config::instance;
}

/**
 * deletes singleton instance and sets Config::instance to 0.
 */
//----------------------------------------------------------
void Config::destroy()
{
	delete Config::instance;
	Config::instance = 0;
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
		i = numLines = 0;
		while( fgets(configStrings[i], 64, file) )
			i++;
		numLines = i;
		for(i = 0; i < numLines; i++)
		{
			if(strncmp(configStrings[i], "screenSi", 8) == 0) { sscanf(configStrings[i], "screenSize %d\n", &screenSize); }
			if(strncmp(configStrings[i], "mouseSpe", 8) == 0) { sscanf(configStrings[i], "mouseSpeed %f\n", &mouseSpeed); }
			if(strncmp(configStrings[i], "gameSkil", 8) == 0) { sscanf(configStrings[i], "gameSkillBase %f\n", &gameSkillBase); }
			if(strncmp(configStrings[i], "gfxLevel", 8) == 0) { sscanf(configStrings[i], "gfxLevel %d\n", &gfxLevel);   }
			if(strncmp(configStrings[i], "volSound", 8) == 0) { sscanf(configStrings[i], "volSound %f\n", &volSound);   }
			if(strncmp(configStrings[i], "volMusic", 8) == 0) { sscanf(configStrings[i], "volMusic %f\n", &volMusic);   }
			if(strncmp(configStrings[i], "full_scr", 8) == 0) { sscanf(configStrings[i], "full_screen %d\n", &tmp);	full_screen	= (bool)tmp; }
			if(strncmp(configStrings[i], "true_col", 8) == 0) { sscanf(configStrings[i], "true_color %d\n", &tmp);	true_color	= (bool)tmp; }
			if(strncmp(configStrings[i], "swap_ste", 8) == 0) { sscanf(configStrings[i], "swap_stereo %d\n", &tmp);	swap_stereo	= (bool)tmp;  }
			if(strncmp(configStrings[i], "auto_spe", 8) == 0) { sscanf(configStrings[i], "auto_speed %d\n", &tmp);	auto_speed	= (bool)tmp;  }
			if(strncmp(configStrings[i], "show_fps", 8) == 0) { sscanf(configStrings[i], "show_fps %d\n", &tmp);	show_fps	= (bool)tmp;  }
			if(strncmp(configStrings[i], "use_play", 8) == 0) { sscanf(configStrings[i], "use_playList %d\n", &tmp);use_playList= (bool)tmp;  }
			if(strncmp(configStrings[i], "use_cdro", 8) == 0) { sscanf(configStrings[i], "use_cdrom %d\n", &tmp);   use_cdrom   = (bool)tmp;  }
			if(strncmp(configStrings[i], "maxLevel", 8) == 0) { sscanf(configStrings[i], "maxLevel %d\n", &maxLevel);  }
			if(strncmp(configStrings[i], "viewGamm", 8) == 0) { sscanf(configStrings[i], "viewGamma %f\n", &viewGamma); }
			if(strncmp(configStrings[i], "audioTyp", 8) == 0) { sscanf(configStrings[i], "audioType %d\n", &tmp); audioType = (AudioType)tmp; }
			if(strncmp(configStrings[i], "cdromCou", 8) == 0) { sscanf(configStrings[i], "cdromCount %d\n", &cdromCount); }
			if(strncmp(configStrings[i], "cdromDev", 8) == 0) { sscanf(configStrings[i], "cdromDevice %d\n", &cdromDevice); }
		}
	}
	else
	{
		fprintf(stderr, "WARNING: could not read config file (%s)\n", configFilename);
		retVal = false;
	}
	
	//-- update all screen size variables
	setScreenSize(screenSize);
	
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
		fprintf(file, "use_playList %d\n",	(int)use_playList);
		fprintf(file, "use_cdrom %d\n",		(int)use_cdrom);
		fprintf(file, "full_screen %d\n", 	(int)full_screen);
		fprintf(file, "true_color %d\n", 	(int)true_color);
		fprintf(file, "swap_stereo %d\n",	(int)swap_stereo);
		fprintf(file, "auto_speed %d\n",	(int)auto_speed);
		fprintf(file, "show_fps %d\n",		(int)show_fps);
		fprintf(file, "screenSize %d\n",	screenSize);
		fprintf(file, "gfxLevel %d\n",		gfxLevel);
		fprintf(file, "gameSkillBase %g\n",	gameSkillBase);
		fprintf(file, "mouseSpeed %g\n",	mouseSpeed);
		fprintf(file, "maxLevel %d\n",		maxLevel);
		fprintf(file, "volSound %g\n",		volSound);
		fprintf(file, "volMusic %g\n",		volMusic);
		fprintf(file, "viewGamma %g\n",		viewGamma);
		fprintf(file, "audioType %d\n",		(int)audioType);
		fprintf(file, "cdromCount %d\n",	cdromCount);
		fprintf(file, "cdromDevice %d\n",	cdromDevice);

		fclose(file);
		fprintf(stderr, "wrote config file (%s)\n", configFilename);
		retVal = true;
	}
	else
	{
		fprintf(stderr, "WARNING: could not write config file (%s)\n", configFilename);
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
	screenSize = m;
	if(screenSize > MAX_SCREEN_SIZE)
		screenSize = MAX_SCREEN_SIZE;
	if(screenSize < MIN_SCREEN_SIZE)
		screenSize = MIN_SCREEN_SIZE;
	switch(screenSize)
	{
		case 0:
			screenW = 512;
			screenH = 384;
			break;
		case 1:
			screenW = 640;
			screenH = 480;
			break;
		case 2:
			screenW = 800;
			screenH = 600;
			break;
		case 3:
			screenW = 1024;
			screenH = 768;
			break;
		case 4:
			screenW = 1280;
			screenH = 960;
			break;
		default:
			screenSize = 1;
			screenW = 640;
			screenH = 480;
			break;
	}
	screenA = (float)screenW/(float)screenH;
}

