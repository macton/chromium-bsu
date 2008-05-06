/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifdef AUDIO_SDLMIXER
 
#include "AudioSDLMixer.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

#ifndef _WIN32
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _WIN32

#include "extern.h"
#include "define.h"
#include "Config.h"
#include "Global.h"

//====================================================================
AudioSDLMixer::AudioSDLMixer()
	: Audio()
{    
	Config	*config = Config::instance();
	//UNCLEAN - if initSound fails, config->audioEnabled() will be set to false
	if(config->audioEnabled() == true)
		initSound();
}

AudioSDLMixer::~AudioSDLMixer()
{
	Config	*config = Config::instance();
	if(config->audioEnabled()) 
	{
    	for (int i = 0; i < NumSoundTypes; i++)
        	Mix_FreeChunk (sounds[i]);

    	Mix_CloseAudio ();
	}
}

/** 
 * open audio device and load sounds
 */
//----------------------------------------------------------
void	AudioSDLMixer::initSound()
{
	Config	*config = Config::instance();
    if ( Mix_OpenAudio (22050, AUDIO_S16, 2, 512) < 0 )
	{
		fprintf(stderr, "ERROR initializing audio - AudioSDLMixer::initSound()\n");
        config->setAudio(false);
	}
	else
	{
		for (int i = 0; i < NumSoundTypes; i++)
    		sounds[i] = Mix_LoadWAV ( dataLoc ( fileNames[i] ) );

		Mix_ReserveChannels (1); // channel 0 is for music

		atexit (Mix_CloseAudio);
	}
}

/** 
 * play sound
 */
//----------------------------------------------------------
void	AudioSDLMixer::playSound(SoundType type, float[3], int)
{
	Config	*config = Config::instance();
	if (config->audioEnabled()) 
	{
		Mix_PlayChannel (-1, sounds[type], 0);
    }
}

/**
 * pause music channel. If CDROM enabled, call Audio::pauseGameMusic();
 */
//----------------------------------------------------------
void AudioSDLMixer::pauseGameMusic(bool status)
{
	Config	*config = Config::instance();
	if (config->audioEnabled()) 
	{
		if(cdrom)
		{
			Audio::pauseGameMusic(status);
		}
		else
		{
	    	if (status)
				Mix_PauseMusic();
			else
				Mix_ResumeMusic();
		}
	}
}

/**
 * stop music channel. If CDROM enabled, call Audio::stopMusic();
 */
//----------------------------------------------------------
void	AudioSDLMixer::stopMusic()
{
	Config	*config = Config::instance();
    if (config->audioEnabled()) 
	{
		Audio::stopMusic();
		Mix_HaltChannel (0);
	}
}

//----------------------------------------------------------
void	AudioSDLMixer::setMusicMode(SoundType mode)
{
	Config	*config = Config::instance();
    if (config->audioEnabled())
	{
		Audio::setMusicMode(mode);
		switch(mode)
		{
			default:
			case MusicGame:
				if(cdrom)
					Mix_HaltChannel (0);
				else
					Mix_PlayChannel (0, sounds[mode], -1);
				break;
			case MusicMenu:
				Mix_PlayChannel (0, sounds[mode], -1);
				break;
		}
	}
}

/**
 * set volume for music channel 
 */
//----------------------------------------------------------
void	AudioSDLMixer::setMusicVolume(float value)
{
	Audio::setMusicVolume(value);
	Config	*config = Config::instance();
    if (config->audioEnabled())
	{
		Mix_Volume (0, (int)(MIX_MAX_VOLUME*value) );
	}
}

/**
 * set volume for sound effects channels
 */
//----------------------------------------------------------
void	AudioSDLMixer::setSoundVolume(float value)
{
	Config	*config = Config::instance();
    if (config->audioEnabled())
	{
		for (int i = 1; i < MIX_CHANNELS; i++)
			Mix_Volume (i, (int)(MIX_MAX_VOLUME*value) );
	}
}

#endif // AUDIO_SDLMIXER

