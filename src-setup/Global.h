/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Global_h
#define Global_h

#include <time.h>
#include "../src/define.h"

//====================================================================
class Global
{
public:
	static int		screenSize;
	static int		gfxLevel;
	
	static float	volSound;
	static float	volMusic;
	static float 	mouseSpeed;
	static float	viewGamma;
	static float	gameSkillBase;
	
	static bool		full_screen;
	static bool		true_color;
	static bool		swap_stereo;
	static bool		auto_speed;
	static bool		show_fps;
	static bool		use_playList;
	static bool		use_cdrom;
	
	static int		maxLevel; //-- not a user-selectable option
	
	static double	hiScore[10][HI_SCORE_HIST];
	static char		hiScoreName[10][HI_SCORE_HIST][64];
	static time_t	hiScoreDate[10][HI_SCORE_HIST];
	
};

#endif // Global_h
