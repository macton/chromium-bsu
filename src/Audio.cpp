/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Audio.h"

#include "Global.h"

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
	
	if(Global::audio_enabled == true)
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
	static char buf[32];
	switch(t)
	{
		case SDL_AUDIO_TRACK:	return "audio";
		case SDL_DATA_TRACK:	return "data";
		default:
			sprintf(buf, "UNKNOWN(type=%d)", t);
			return buf;
	}
}
void Audio::initCDROM()
{
	int tmp;
	if(Global::use_cdrom)
	{
		tmp = SDL_CDNumDrives();
		fprintf(stderr, "%d CDROM drive(s).\n", tmp);
		if(tmp > 0)
		{
			cdrom = SDL_CDOpen(0);
			if(cdrom)
			{
				tmp = SDL_CDStatus(cdrom);
				if(tmp)
				{
					musicMax = cdrom->numtracks;
					fprintf(stderr, "Tracks: %d\n", cdrom->numtracks);
					int music = 0;
					for(int i = 0; i < cdrom->numtracks; i++)
					{
						fprintf(stderr, "track %2d: %s\n", i, trackType(cdrom->track[i].type));
						// SDL BUG? 'type' should only be SDL_DATA_TRACK(0x04)or SDL_AUDIO_TRACK(0x00), but 
						// this is returning SDL_AUDIO_TRACK=0x02 and SDL_DATA_TRACK=0x06 on some copied CDRs!
						// Orignal CDs return correct values, but CDR duplications are off by 2!
						if(cdrom->track[i].type < SDL_DATA_TRACK)
							music++;
					}
					if(!music)
					{		
						fprintf(stderr, "\n Data track(s) only. CD audio not available.\n\n");
						SDL_CDStop(cdrom);
						cdrom = 0;
					}
					else
					{
						fprintf(stderr, "\n use_cdrom enabled. Set to '0' in ~/.chromium to disable.\n");
						fprintf(stderr, " Press the \'N\' key to skip to next CD track during a game.\n\n");	
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
				fprintf(stderr, "Could not access CDROM device : %s\n", SDL_GetError());
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
	if(Global::audio_enabled == true)
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
	if(Global::audio_enabled == true)
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
				fprintf(stderr, "CDROM error in AudioOpenAL::pauseGameMusic(%d)\n", (int)status);
				fprintf(stderr, "CDROM status = %d\n", cdromStatus);
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
	if(Global::audio_enabled == true)
	{
		int cdromStatus = 0;
		if(cdrom)
		{
			cdromStatus = SDL_CDStatus(cdrom);
			switch(mode)
			{
				default:
				case MusicGame:
					if(cdromStatus == CD_PAUSED && !Global::game_pause)
						SDL_CDResume(cdrom);
					if(cdromStatus == CD_STOPPED)
					{
						SDL_CDPlayTracks(cdrom, musicIndex, 0, 1, 0);
						if(Global::game_pause)
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
 * NOOP
 */
//----------------------------------------------------------
void	Audio::setMusicVolume(float)
{
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
	if(Global::audio_enabled == true)
	{
		bool	wasPlaying = false;
		if(musicMax)
			musicIndex = index%musicMax;
		fprintf(stderr, "Audio::setMusicIndex(%d)\n", musicIndex);
		
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
				fprintf(stderr, "track %d is data - trying next track...\n", musicIndex);
				Audio::setMusicIndex(musicIndex+1);
			}
		}
	}
}

/**
 * skip to next CD track
 */
//----------------------------------------------------------
void	Audio::nextMusicIndex()
{
	setMusicIndex(musicIndex+1);
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



