/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Global.h"

int		Global::screenSize	= 2;
int		Global::gfxLevel	= 2;

float	Global::volSound	= 0.9;
float	Global::volMusic	= 0.5;
float	Global::mouseSpeed	= 0.03;
float	Global::viewGamma	= 1.1;
float	Global::gameSkillBase	= 0.5;

bool	Global::full_screen		=  false;
bool	Global::true_color		= false;
bool	Global::swap_stereo		= false;
bool	Global::auto_speed		= false;
bool	Global::show_fps		= true;
bool	Global::use_playList	= false;
bool	Global::use_cdrom		= false;

int		Global::maxLevel	= 1;

double	Global::hiScore[10][HI_SCORE_HIST] = {	{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 }, 
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 },
												{ 30000.0, 25000.0, 20000.0, 15000.0, 10000.0 } };	  

char	Global::hiScoreName[10][HI_SCORE_HIST][64] = {	
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" },
													{ "nobody",  "nobody",  "nobody",  "nobody",  "nobody" }
												};	  

time_t	Global::hiScoreDate[10][HI_SCORE_HIST];
	
