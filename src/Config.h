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
	
	bool	getFullScreen()		{	return	full_screen;	}
	bool	getBlendEnabled()	{	return	blend_enabled;	}
	bool	getAudioEnabled()	{	return	audio_enabled;	}
	bool	getSwapStereo()		{	return  swap_stereo;	}
	bool	getAutoSpeed()		{	return  auto_speed;	}
	bool	getShowFPS()		{	return  show_fps;	}
	bool	getTrueColor()		{	return  true_color;	}
	bool	getUsePlayList()	{	return  use_playList;	}
	bool	getUseCDROM()		{	return  use_cdrom;	}
	
	void	setFullScreen(bool s)	{	full_screen = s;	}
	void	setBlendEnabled(bool s)	{	blend_enabled = s;	}
	void	setAudioEnabled(bool s)	{	audio_enabled = s;	}
	void	setSwapStereo(bool s)	{	swap_stereo = s;	}
	void	setAutoSpeed(bool s)	{	auto_speed = s;		}
	void	setShowFPS(bool s)		{	show_fps = s;		}
	void	setTrueColor(bool s)	{	true_color = s;		}
	void	setUsePlayList(bool s)	{	use_playList = s;	}
	void	setUseCDROM(bool s)		{	use_cdrom = s;		}
	
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
	
	bool	 blend_enabled;
	bool	 audio_enabled;
	bool	 swap_stereo;
	bool	 auto_speed;
	bool	 show_fps;
	bool	 true_color;
	bool	 use_playList;
	bool	 use_cdrom;
	
private:
	static Config	*instance;
	
	Config();
};

#endif // Config_h
