/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Global_h
#define Global_h

#include <stdio.h>
#include <time.h>

#include "define.h"
#include "TexFont.h"
#include "MainToolkit.h"

class HeroAircraft;
class HeroAmmo;
class EnemyFleet;
class EnemyAmmo;
class Explosions;
class PowerUps;
class Audio;
class Ground;
class MenuGL;
class MainGL;
class StatusDisplay;
class ScreenItemAdd;

//====================================================================
class Global
{
public:
	enum GameMode { Game, Menu, LevelOver, HeroDead };
	
	static bool		readConfigFile();
	static bool		saveConfigFile();
	static bool		readHiScoreFile();
	static bool		saveHiScoreFile();
	
	static void		newGame();
	static void		gotoNextLevel();
	
	static void		setScreenSize(int m);

	static void		generateRandom(bool r = true);
	
	static void		createGame();
	static void		deleteGame();
	
	static void		deleteTextures();
	static void		loadTextures();
	
	static int		setHiScore();
	static int		checkHiScore();
	static void		printHiScore();

	static char		configFileName[128];
	
	static MainToolkit *toolkit;
	
	static int		screenW;
	static int		screenH;
	static float	screenA;
	static float	screenFOV;
	static float	screenNear;
	static float	screenFar;
	static bool		full_screen;
	static int		screenSize;
	static float	screenBound[2];
	static float	zTrans;
	static bool		blend_enable;
	
	static TexFont	*texFont;

	static float	fps;
	static int		frame;
	static int		gameFrame;
	static float	gameSpeed;
	static float	gameSkillBase;
	static float	gameSkill;
	static int		gameLevel;
	static int		maxLevel;
	static int		gfxLevel;
	static float	viewGamma;
	static float	speedAdj;
	
	static float	scrollSpeed;
	
	static float 	mouseSpeed;
	static bool 	mouseActive;
	
	static bool		has_multitex;
	static bool		audio_enabled;
	static bool		swap_stereo;
	static bool		auto_speed;
	static bool		show_fps;
	static bool		true_color;
	static bool		use_playList;
	static bool		use_cdrom;
	
	static bool		game_pause;
	static bool		game_quit;
	
	static float	volSound;
	static float	volMusic;
	
	static int		heroDeath;
	static int		heroSuccess;
	
	static GameMode	gameMode;
	
	static HeroAircraft	*hero;
	static EnemyFleet	*enemyFleet;
	static HeroAmmo		*heroAmmo;
	static EnemyAmmo	*enemyAmmo;
	static ScreenItemAdd *itemAdd;
	
	static Explosions	*explosions;
	static PowerUps		*powerUps;
	static Audio		*audio;
	static Ground		*ground;
	static Ground		*groundGame;
	static Ground		*groundMenu;
	static MenuGL		*menu;
	static MainGL		*mainGL;
	static StatusDisplay *statusDisplay;
	
	static int		randI[256];
	static float	randF[256];
	static float	randS[256];
	static int		randIndex;
	
	static double	hiScore[10][HI_SCORE_HIST];
	static char		hiScoreName[10][HI_SCORE_HIST][64];
	static time_t	hiScoreDate[10][HI_SCORE_HIST];
	
	static FILE	*eventFile;
	
	static int tipShipPast;
	static int tipSuperShield;
};

#endif // Global_h
