/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Config_h
#define Config_h

#include "compatibility.h"

/**
 * contains data obtained from config file. Singleton.
 */
//====================================================================
class Config
{
public:
	~Config();
	
	static Config	*init();
	static Config	*getInstance();
	static void		destroy();
	
	bool	readFile();
	bool	saveFile();
	
	
	void	setScreenSize(int m);

	int		getScreenSize()		{	return screenSize;		}
	int		getScreenW()		{	return screenW;			}
	int		getScreenH()		{	return screenH;			}
	float	getScreenA()		{	return screenA;			}
	float	getScreenFOV()		{	return screenFOV;		}
	float	getScreenNear()		{	return screenNear;		}
	float	getScreenFar()		{	return screenFar;		}
	float	getScreenBoundX()	{	return screenBound[0];	}
	float	getScreenBoundY()	{	return screenBound[1];	}
	float	getZTrans()			{	return zTrans;			}
	
	bool	getFullScreen()		{	return	full_screen;	}
	bool	getBlendEnabled()	{	return	blend_enabled;	}
	bool	getAudioEnabled()	{	return	audio_enabled;	}
	bool	getSwapStereo()		{	return  swap_stereo;	}
	bool	getAutoSpeed()		{	return  auto_speed;		}
	bool	getShowFPS()		{	return  show_fps;		}
	bool	getTrueColor()		{	return  true_color;		}
	bool	getUsePlayList()	{	return  use_playList;	}
	bool	getUseCDROM()		{	return  use_cdrom;		}
	void	setFullScreen(bool s)	{	full_screen = s;	}
	void	setBlendEnabled(bool s)	{	blend_enabled = s;	}
	void	setAudioEnabled(bool s)	{	audio_enabled = s;	}
	void	setSwapStereo(bool s)	{	swap_stereo = s;	}
	void	setAutoSpeed(bool s)	{	auto_speed = s;		}
	void	setShowFPS(bool s)		{	show_fps = s;		}
	void	setTrueColor(bool s)	{	true_color = s;		}
	void	setUsePlayList(bool s)	{	use_playList = s;	}
	void	setUseCDROM(bool s)		{	use_cdrom = s;		}
	
	int 	getGfxLevel()		{	return gfxLevel;	}
	int 	getMaxLevel()		{	return maxLevel;	}
	void	setGfxLevel(int a)	{	gfxLevel = a; if(gfxLevel < 0) gfxLevel = 0; if(gfxLevel > 2) gfxLevel = 2; }
	void	setMaxLevel(int a)	{	maxLevel = a; if(maxLevel > 10) maxLevel = 10;	}
	
	float	 getMouseSpeed()		{	return mouseSpeed;		}
	float	 getGameSkillBase()		{	return gameSkillBase;	}
	float	 getViewGamma()			{	return viewGamma;		}
	float	 getVolSound()			{	return volSound;		}
	float	 getVolMusic()			{	return volMusic;		}
	void	 setMouseSpeed(float f)		{	mouseSpeed = f;	if(mouseSpeed < 0.01) mouseSpeed = 0.01; if(mouseSpeed > 0.1) mouseSpeed = 0.1;	}
	void	 setGameSkillBase(float f)	{	gameSkillBase = f;	if(gameSkillBase > 0.9) gameSkillBase = 0.9; if(gameSkillBase < 0.2) gameSkillBase = 0.2;}
	void	 setViewGamma(float f)		{	viewGamma = f;		}
	void	 setVolSound(float f)		{	volSound = f;	if(volSound < 0.0) volSound = 0.0; if(volSound > 1.0) volSound = 1.0;	}
	void	 setVolMusic(float f)		{	volMusic = f;	if(volMusic < 0.0) volMusic = 0.0; if(volMusic > 1.0) volMusic = 1.0;	}
	
	int		getIntSkill()	{ return (int)((gameSkillBase+0.05)*10.0); }

	enum AudioType { AudioOpenAL, AudioSDL_Mixer, NumAudioTypes };
	AudioType	getAudioType()	{ return audioType; };
	
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
	
	int 	 gfxLevel;
	int 	 maxLevel;

	float	 mouseSpeed;
	float	 gameSkillBase;
	float	 viewGamma;
	float	 volSound;
	float	 volMusic;
	
	AudioType	audioType;

private:
	static Config	*instance;
	
	Config();
};

#endif // Config_h
