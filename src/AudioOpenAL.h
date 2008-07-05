/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef AudioOpenAL_h 
#define AudioOpenAL_h

#ifdef AUDIO_OPENAL

#include "Audio.h"

#ifdef macintosh
#include <openal.h>
#else //macintosh
#include <AL/al.h>
#include <AL/alut.h>
#endif //macintosh

/**
 * Use OpenAL for sound effects (positional audio) as well as music
 * playback if playlist support is selected.
 */
//====================================================================
class AudioOpenAL	: public Audio
{
public:
	AudioOpenAL();
	~AudioOpenAL();
	
	virtual void	update();
	virtual void	playSound(SoundType type, float *pos, int age = 0);
	virtual void	stopMusic();
	virtual void	pauseGameMusic(bool);
	virtual void	setMusicMode(SoundType);
	virtual void	setMusicVolume(float);
	virtual void	setSoundVolume(float);
	virtual void	setMusicIndex(int);
	
protected:
	virtual void	initSound();

	bool	createContext();
	void	loadSounds();
	
private:
	void	checkError(const char* tag = "");
	
	void	playSoundExplosion(float pos[3]);
	void	playSoundExploPop(float pos[3]);
	
	//-- OpenAL extensions
	void		checkForExtensions();
	void		(*alAttenuationScale)(ALfloat param);
	void		(*alcSetAudioChannel)(ALuint channel, ALfloat volume);
	float		(*alcGetAudioChannel)(ALuint channel);
	ALboolean	(*alutLoadMP3)(ALuint, ALvoid *, ALint);
	ALboolean	(*alutLoadVorbis)(ALuint, ALvoid *, ALint);
	
	//-- Format loaders
	bool	loadWAV(const char* filename);
	bool	loadMP3(const char* filename);
	bool	loadVorbis(const char* filename);
	
private:
	ALuint	buffer[NumSoundTypes];
	ALuint	source[NumSoundTypes];
	ALfloat	sourcePos[NumSoundTypes][3];
	
	ALuint	sourceExplosion[NUM_EXPLO];
	ALuint	sourceExploPop[NUM_EXPLO_POP];
	int		explosionIndex;
	int		exploPopIndex;
	int		numReqThisFrame[NumSoundTypes];
	
	ALfloat	gain[NumSoundTypes];
	
	/** audio formats for playList. Determined my file extension */
	enum AudioFormat	{ 
							Unknown = -1,	/**< error condition */
							WAV,			/**< .wav extension */
							MP3,			/**< .mp3 extension */
							OGG,			/**< .ogg extension */
							NumAudioFormats	/**< number of available extensions */
						};
	AudioFormat	extensionFormat(char*);
	void		loadMusicList();
	
	float	origCDvolume;
	
	ALCdevice	*dev;
	ALCcontext	*context_id;
	bool	initialized;
	float	audioScale[3];
	
	float	soundVolume;
	float	musicVolume;
	
	SoundInfo	*soundQueue;
};

#endif // AUDIO_OPENAL

#endif // AudioOpenAL_h
