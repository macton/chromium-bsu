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
	static Config	*instance();
	static void		destroy();
	
	bool	readFile();
	bool	saveFile();
	
	
	void	setScreenSize(int m);

	int		screenSize()		{	return m_screenSize;	}
	int		screenW()			{	return m_screenW;		}
	int		screenH()			{	return m_screenH;		}
	float	screenA()			{	return m_screenA;		}
	float	screenFOV()			{	return m_screenFOV;		}
	float	screenNear()		{	return m_screenNear;	}
	float	screenFar()			{	return m_screenFar;		}
	float	screenBoundX()		{	return m_screenBound[0];}
	float	screenBoundY()		{	return m_screenBound[1];}
	float	zTrans()			{	return m_zTrans;		}
	
	bool	fullScreen()		{	return	m_full_screen;	}
	bool	blend()				{	return	m_blend_enabled;}
	bool	texBorder()			{	return	m_tex_border;	}
	bool	audioEnabled()		{	return	m_audio_enabled;}
	bool	swapStereo()		{	return  m_swap_stereo;	}
	bool	autoSpeed()			{	return  m_auto_speed;	}
	bool	showFPS()			{	return  m_show_fps;		}
	bool	trueColor()			{	return  m_true_color;	}
	bool	usePlayList()		{	return  m_use_playList;	}
	bool	useCDROM()			{	return  m_use_cdrom;		}
	void	setFullScreen(bool s)		{	m_full_screen = s;	}
	void	setBlend(bool s)			{	m_blend_enabled = s;}
	void	setTexBorder(bool s)		{	m_tex_border = s;	}
	void	setAudio(bool s)			{	m_audio_enabled = s;}
	void	setSwapStereo(bool s)		{	m_swap_stereo = s;	}
	void	setAutoSpeed(bool s)		{	m_auto_speed = s;	}
	void	setShowFPS(bool s)			{	m_show_fps = s;		}
	void	setTrueColor(bool s)		{	m_true_color = s;	}
	void	setUsePlayList(bool s)		{	m_use_playList = s;	}
	void	setUseCDROM(bool s)			{	m_use_cdrom = s;	}
	
	int		CDROMDevice()			{   return m_cdromDevice;	}
	int		CDROMCount()			{   return m_cdromCount;	}
	void	setCDROMDevice(int i)	{	m_cdromDevice = i;		}
	void	setCDROMCount(int c)	{	m_cdromCount  = c;		}
	
	int 	gfxLevel()		{	return m_gfxLevel;	}
	int 	maxLevel()		{	return m_maxLevel;	}
	void	setGfxLevel(int a)	{	m_gfxLevel = a; if(m_gfxLevel < 0) m_gfxLevel = 0; if(m_gfxLevel > 2) m_gfxLevel = 2; }
	void	setMaxLevel(int a)	{	m_maxLevel = a; if(m_maxLevel > 10) m_maxLevel = 10;	}
	
	float	mouseSpeed()		{	return m_mouseSpeed;		}
	float	gameSkillBase()		{	return m_gameSkillBase;	}
	float	viewGamma()			{	return m_viewGamma;		}
	float	volSound()			{	return m_volSound;		}
	float	volMusic()			{	return m_volMusic;		}
	void	setMouseSpeed(float f)		{	m_mouseSpeed = f;	if(m_mouseSpeed < 0.01) m_mouseSpeed = 0.01; if(m_mouseSpeed > 0.1) m_mouseSpeed = 0.1;	}
	void	setGameSkillBase(float f)	{	m_gameSkillBase = f;	if(m_gameSkillBase > 0.9) m_gameSkillBase = 0.9; if(m_gameSkillBase < 0.2) m_gameSkillBase = 0.2;}
	void	setViewGamma(float f)		{	m_viewGamma = f;		}
	void	setVolSound(float f)		{	m_volSound = f;	if(m_volSound < 0.0) m_volSound = 0.0; if(m_volSound > 1.0) m_volSound = 1.0;	}
	void	setVolMusic(float f)		{	m_volMusic = f;	if(m_volMusic < 0.0) m_volMusic = 0.0; if(m_volMusic > 1.0) m_volMusic = 1.0;	}
	
	int		intSkill()	{ return (int)((m_gameSkillBase+0.05)*10.0); }

	enum AudioType { AudioOpenAL, AudioSDL_Mixer, NumAudioTypes };
	AudioType	audioType()	{ return m_audioType; };
	
private:
	int 	 m_screenW;
	int 	 m_screenH;
	float	 m_screenA;
	float	 m_screenFOV;
	float	 m_screenNear;
	float	 m_screenFar;
	bool	 m_full_screen;
	int 	 m_screenSize;
	float	 m_screenBound[2];
	float	 m_zTrans;
	
	bool	 m_blend_enabled;
	bool	 m_tex_border;
	bool	 m_audio_enabled;
	bool	 m_swap_stereo;
	bool	 m_auto_speed;
	bool	 m_show_fps;
	bool	 m_true_color;
	bool	 m_use_playList;
	bool	 m_use_cdrom;
	
	int 	 m_gfxLevel;
	int 	 m_maxLevel;
	int		 m_cdromDevice;
	int		 m_cdromCount;

	float	 m_mouseSpeed;
	float	 m_gameSkillBase;
	float	 m_viewGamma;
	float	 m_volSound;
	float	 m_volMusic;
	
	AudioType	m_audioType;

private:
	static Config	*m_instance;
	
	Config();
};

#endif // Config_h
