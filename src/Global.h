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

#include "GlobalStuff.h"

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
	
	~Global();
	
	static Global	*init();
	static Global	*getInstance();
	static void		destroy();
	
	bool		readConfigFile();
	bool		saveConfigFile();
	bool		readHiScoreFile();
	bool		saveHiScoreFile();
	
	void		newGame();
	void		gotoNextLevel();
	
	void		setScreenSize(int m);

	static void	generateRandom(bool r = true);
	
	void		createGame();
	void		deleteGame();
	
	void		deleteTextures();
	void		loadTextures();
	
	int			setHiScore();
	int			checkHiScore();
	void		printHiScore();

	static char	configFileName[128];
	
	static MainToolkit *toolkit;
	
	int 	 screenW;
	int 	 screenH;
	float	 screenA;
	float	 screenFOV;
	float	 screenNear;
	float	 screenFar;
	bool	 full_screen;
	int 	 screenSize;
	float	 screenBound[2];
	float	 zTrans;
	bool	 blend_enable;
	
	static float 	mouseSpeed;
	static bool 	mouseActive;
	
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
	
	bool	 audio_enabled;
	bool	 swap_stereo;
	bool	 auto_speed;
	bool	 show_fps;
	bool	 true_color;
	bool	 use_playList;
	bool	 use_cdrom;
	
	bool	 game_pause;
	bool	 game_quit;
	
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

private:
	static Global	*instance;
	
	Global();
};

#endif // Global_h
