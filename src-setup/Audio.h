/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef Audio_h
#define Audio_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_OPENAL_AL_H
#include <OpenAL/al.h>
#elif defined(HAVE_AL_AL_H)
#include <AL/al.h>
#endif

//====================================================================
class Audio
{
public:
	Audio();
	~Audio();
	
	enum SoundType { MusicGame, NumSoundTypes };
	bool	playSound(const char *filename);
	void	stopSound();
	const char *openALInfo() { return infoBuffer; }
	const char *openALError() { return errorBuffer; }
	
	enum AudioFormat { Unknown = -1, WAV, MP3, OGG, NumAudioFormats };
	bool		hasFormat(AudioFormat af);
	const char	*formatExtension(AudioFormat af);
	Audio::AudioFormat extensionFormat(const char* filename);

	bool	initAudio();
	void	killAudio();
	
private:
	bool	checkError(const char* tag);
	void	fillInfo();
	
	void		(*alAttenuationScale)(ALfloat param);
	ALboolean	(*alutLoadMP3)(ALuint, ALvoid *, ALint);
	ALboolean	(*alutLoadVorbis)(ALuint, ALvoid *, ALint);
	
	bool	loadWAV(const char* filename);
	bool	loadMP3(const char* filename);
	bool	loadVorbis(const char* filename);
	
private:
	ALuint	buffer[NumSoundTypes];
	ALuint	source[NumSoundTypes];
	
	void	*context_id;
	bool	initialized;
	
	char errorBuffer[512];
	char infoBuffer[4096];
	char formatString[NumAudioFormats][16];
};

#endif // Audio_h
