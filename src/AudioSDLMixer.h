/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef AudioSDLMixer_h
#define AudioSDLMixer_h

#ifdef AUDIO_SDLMIXER

#include <SDL/SDL_mixer.h>

#include "Audio.h"

/**
 * Use SDL_mixer for simple audio effects (no position or panning)
 */
//====================================================================
class AudioSDLMixer	: public Audio
{
public:
	AudioSDLMixer();
	~AudioSDLMixer();

	virtual void	playSound(SoundType type, float *pos, int age = 0);
	virtual void	stopMusic();
	virtual void	pauseGameMusic(bool);
	virtual void	setMusicMode(SoundType);
	virtual void	setMusicVolume(float);
	virtual void	setSoundVolume(float);
	
protected:
	virtual void	initSound();

private:
	Mix_Chunk *sounds[NumSoundTypes];
};

#endif // AUDIO_SDLMIXER

#endif // AudioSDLMixer_h
