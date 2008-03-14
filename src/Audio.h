/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Audio_h
#define Audio_h

#define NUM_EXPLO		2
#define NUM_EXPLO_POP	6

#define MAX_MUSIC 32

class SoundInfo;
class Global;

#ifdef USE_SDL
#include <SDL/SDL.h> // for CDROM
#else
#define SDL_CD void
#endif

#if !defined(USE_SDL) && !defined(SDL_CD_STUBS)
	#define SDL_CD_STUBS
	enum { CD_ERROR = -1, CD_TRAYEMPTY, CD_STOPPED, CD_PLAYING, CD_PAUSED };
	void	SDL_CDPlayTracks(void*, int, int, int, int);
	void	SDL_CDPause(void*);
	void	SDL_CDResume(void*);
	void	SDL_CDStop(void*);
	int		SDL_CDStatus(void*);
#endif //USE_SDL


/**
 * Base class upon which all audio calls are made. If USE_SDL is defined, 
 * this class will do CD music playback, but does not do any sound effects.
 */
//====================================================================
class Audio
{
public:
	/** available sound effects types */
	enum SoundType	{	
						HeroAmmo00, 	/**< unused */			
						PowerUp,		/**< power up sound */ 	
						Explosion,		/**< standard explosion */ 
						ExploPop,		/**< 'light' explosion */ 
						ExploBig,		/**< deep, long explosion */
						AddLife,		/**< new ship earned */  
						LoseLife,		/**< ship lost */		
						MusicGame,		/**< gameplay music */ 	
						MusicMenu, 		/**< menu music */ 		
						NumSoundTypes 	/**< total number of sounds available */
					};
	
	Audio();
	virtual ~Audio();
	
	virtual void	update();
	virtual void	playSound(SoundType type, float *pos, int age = 0);
	virtual void	stopMusic();
	virtual void	pauseGameMusic(bool);
	virtual void	setMusicMode(SoundType);
	virtual void	setMusicVolume(float);
	virtual void	setSoundVolume(float);
	virtual void	setMusicIndex(int);
	virtual void	nextMusicIndex();
	
protected:
	virtual void	initSound();
	virtual void	initCDROM();

	const char	*fileNames[NumSoundTypes];	/**< base filenames for sound effects */
	SDL_CD	*cdrom; 					/**< pointer to CDROM struct. Is void* if not using SDL */
	char	musicFile[MAX_MUSIC][256];	/**< array of filenames for playlist */
	int		musicMax;					/**< max number of user-defined songs (CD or playlist) */
	int		musicIndex; 				/**< current track ( 0 < musicIndex < musicMax ) */

};

/**
 * node class used for 'audio queue'
 */ 
//======================================
class SoundInfo
{
public:
	SoundInfo();
	SoundInfo(Audio::SoundType t, float p[3], int a);
	
	Audio::SoundType	type;
	float		pos[3];
	int			age;
	
	SoundInfo	*next;
	SoundInfo	*back;
};

#endif // Audio_h
