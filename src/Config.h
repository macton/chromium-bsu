/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Config_h
#define Config_h

//====================================================================
class Config
{
public:
	~Config();
	
	static Config	*init();
	static Config	*getInstance();
	static void		destroy();
	
	bool		readFile();
	bool		saveFile();
	
	void		setScreenSize(int m);

	static char	configFileName[128];
	
	int		getScreenSize()	{	return screenSize;	}
	int		getScreenW()	{	return screenW;		}
	int		getScreenH()	{	return screenH;		}
	float	getScreenA()	{	return screenA;		}
	float	getScreenFOV()	{	return screenFOV;	}
	float	getScreenNear()	{	return screenNear;	}
	float	getScreenFar()	{	return screenFar;	}
	float	getScreenBoundX()	{	return screenBound[0];	}
	float	getScreenBoundY()	{	return screenBound[1];	}
	float	getZTrans()		{	return zTrans;	}
	
	bool	isFullScreen()	{	return full_screen;		}
	bool	isBlendEnable()	{	return blend_enable;	}
	
	static float 	mouseSpeed;
	static bool 	mouseActive;
	
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
	
private:
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
	
private:
	static Config	*instance;
	
	Config();
};

#endif // Config_h
