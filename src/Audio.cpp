/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"

#include "Audio.h"

#include "Config.h"
#include "Global.h"

#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>
using namespace std;

#if !defined(USE_SDL) && defined(SDL_CD_STUBS)
void	SDL_CDPlayTracks(void*, int, int, int, int) {}
void	SDL_CDPause(void*)	{}
void	SDL_CDResume(void*)	{}
void	SDL_CDStop(void*)	{}
int		SDL_CDStatus(void*)	{ return 0; }
#endif //USE_SDL

//====================================================================
Audio::Audio()
{
	Config	*config = Config::instance();
	
    fileNames[HeroAmmo00]	= "wav/boom.wav";
	fileNames[PowerUp]		= "wav/power.wav";
	fileNames[Explosion]	= "wav/exploStd.wav";
	fileNames[ExploPop]		= "wav/exploPop.wav";
	fileNames[ExploBig]		= "wav/exploBig.wav";
	fileNames[LoseLife]		= "wav/life_lose.wav";
	fileNames[AddLife]		= "wav/life_add.wav";
	fileNames[MusicGame]	= "wav/music_game.wav";
	fileNames[MusicMenu]	= "wav/music_menu.wav";
	
	cdrom = NULL;
	musicMax = 1;
	musicIndex = 0;
	
	if(config->audioEnabled() == true)
	{
		initCDROM();
	}
}

Audio::~Audio()
{
	if(cdrom)
	{
		SDL_CDStop(cdrom);
	}
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::update()
{
}


/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::playSound(SoundType, float *, int )
{
}


/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::initSound()
{
}

/**
 * If SDL is being used as the windowing toolkit, check
 * availability of CDROM for music tracks. Does nothing
 * if GLUT is being used.
 */
//----------------------------------------------------------
#ifndef USE_SDL
void	Audio::initCDROM()
{
	cdrom = 0;
}
#else // USE_SDL
static const char *trackType(int t)
{
	static char buf[64];
	switch(t)
	{
		case SDL_AUDIO_TRACK:	return _("audio");
		case SDL_DATA_TRACK:	return _("data");
		default:
			sprintf(buf, _("UNKNOWN(type=%d)"), t);
			return buf;
	}
}
void Audio::initCDROM()
{
	Config	*config = Config::instance();
	int tmp;
	if(config->useCDROM())
	{
		tmp = SDL_CDNumDrives();
		if( config->debug() ) fprintf(stderr, _("%d CDROM drive(s).\n"), tmp);
		config->setCDROMCount(tmp);
		if(tmp > 0)
		{
			cdrom = SDL_CDOpen(config->CDROMDevice());
			if(cdrom)
			{
				tmp = SDL_CDStatus(cdrom);
				if(tmp)
				{
					musicMax = cdrom->numtracks;
					if( config->debug() ) fprintf(stderr, _("Tracks: %d\n"), cdrom->numtracks);
					int music = 0;
					for(int i = 0; i < cdrom->numtracks; i++)
					{
						if( config->debug() ) fprintf(stderr, _("track %2d: %s\n"), i, trackType(cdrom->track[i].type));
						// SDL BUG? 'type' should only be SDL_DATA_TRACK(0x04)or SDL_AUDIO_TRACK(0x00), but 
						// this is returning SDL_AUDIO_TRACK=0x02 and SDL_DATA_TRACK=0x06 on some copied CDRs!
						// Orignal CDs return correct values, but CDR duplications are off by 2!
						if(cdrom->track[i].type < SDL_DATA_TRACK)
							music++;
					}
					if(!music)
					{		
						if( config->debug() ) fprintf(stderr, _("\n Data track(s) only. CD audio not available.\n\n"));
						SDL_CDStop(cdrom);
						cdrom = 0;
					}
					else
					{
						fprintf(stderr, _("\n use_cdrom enabled. Set to '0' in the configuration file to disable.\n"));
						fprintf(stderr, _(" Press the \'N\' key to skip to next CD track during a game.\n\n"));
					}	
				}
				else
				{
					SDL_CDClose(cdrom);
					cdrom = 0;
				}
			}
			else
			{
				if( config->debug() ) fprintf(stderr, _("ERROR! Could not access CDROM device %d : %s\n"), config->CDROMDevice(), SDL_GetError());
				SDL_ClearError();
				config->setCDROMDevice(0); 
			}
		}
	}
	else
		cdrom = 0;
}
#endif // USE_SDL

/**
 * stop CD track
 */
//----------------------------------------------------------
void	Audio::stopMusic()
{
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		if(cdrom)
			SDL_CDStop(cdrom);
	}
}

/**
 * pause/resume CDROM playback
 */
//----------------------------------------------------------
void	Audio::pauseGameMusic(bool status)
{
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		if(cdrom)
		{
			int cdromStatus = SDL_CDStatus(cdrom);
			if(status && cdromStatus == CD_PLAYING)
				SDL_CDPause(cdrom);
			else if( !status && cdromStatus == CD_PAUSED )
				SDL_CDResume(cdrom);
			else
			{
				fprintf(stderr, _("CDROM error in AudioOpenAL::pauseGameMusic(%d)\n"), (int)status);
				fprintf(stderr, _("CDROM status = %d\n"), cdromStatus);
			}
		}
	}
}

/** 
 * switch between menu and game music modes
 */
//----------------------------------------------------------
void	Audio::setMusicMode(SoundType mode)
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		int cdromStatus = 0;
		if(cdrom)
		{
			cdromStatus = SDL_CDStatus(cdrom);
			switch(mode)
			{
				default:
				case MusicGame:
					if(cdromStatus == CD_PAUSED && !game->game_pause)
						SDL_CDResume(cdrom);
					if(cdromStatus == CD_STOPPED)
					{
						SDL_CDPlayTracks(cdrom, musicIndex, 0, 1, 0);
						if(game->game_pause)
							SDL_CDPause(cdrom);
					}
					break;
				case MusicMenu:
					if(cdromStatus == CD_PLAYING)
						SDL_CDPause(cdrom);
					break;
			}
		}
	}
}

/**
 * set volume for music channel.
 */
//----------------------------------------------------------
void	Audio::setMusicVolume(float value)
{
#ifdef __linux__
	Config	*config = Config::instance();
	if (config->audioEnabled() && config->useCDROM())
	{
		// determine desired volume
		const int MAX_CD_VOLUME = 100; // 100 percent
		int desiredVolume = floor(MAX_CD_VOLUME * value);
		stringstream volume;
		volume << desiredVolume;

		// set the desired volume
		string command = "amixer set CD ";
		command += volume.str();
		command += "% unmute";
		system(command.c_str());
	}
#endif // __linux__
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::setSoundVolume(float)
{
}

/**
 * play CD track at index
 */
//----------------------------------------------------------
void	Audio::setMusicIndex(int index)
{
#ifdef USE_SDL
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		bool	wasPlaying = false;
		if(musicMax)
			musicIndex = index%musicMax;
		if( config->debug() ) fprintf(stderr, _("Audio::setMusicIndex(%d)\n"), musicIndex);
		
		if(cdrom)
		{
			int cdromStatus = SDL_CDStatus(cdrom);
			if(cdromStatus == CD_PLAYING)
				wasPlaying = true;
			//if it's a data track, increment the index and try again.
			if(cdrom->track[musicIndex].type < SDL_DATA_TRACK)
			{
				SDL_CDStop(cdrom);
				SDL_CDPlayTracks(cdrom, musicIndex, 0, 1, 0);
				if(!wasPlaying)
					SDL_CDPause(cdrom);
			}
			else
			{
				if( config->debug() ) fprintf(stderr, _("track %d is data - trying next track...\n"), musicIndex);
				Audio::setMusicIndex(musicIndex+1);
			}
		}
	}
#endif
}

/**
 * skip to next CD track
 */
//----------------------------------------------------------
void	Audio::nextMusicIndex()
{
	int i = musicIndex + 1;
	if(i >= musicMax )
		i = 0;
	setMusicIndex(i);
}

//====================================================================
SoundInfo::SoundInfo()
{
	type = Audio::Explosion;
	pos[0] = 0.0;
	pos[1] = 0.0;
	pos[2] = 0.0;
	age	= 1;
	back = next = 0;
}
SoundInfo::SoundInfo(Audio::SoundType t, float p[3], int a)
{
	type = t;
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	age	= a;
	back = next = 0;
}



