/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef AUDIO_OPENAL

#include "gettext.h"

#include "AudioOpenAL.h"

#ifdef macintosh
	#include <MacHeaders.h>
	void	*alcCreateContext(int) { alInit(NULL); return (void*)1; }
	void	alcDestroyContext(void*) { alExit(); }
#endif //macintosh

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

#ifndef _WIN32
	#include <unistd.h>
	#include <cerrno>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif //_WIN32

#ifdef HAVE_OPENAL_AL_H
#include <OpenAL/al.h>
#elif defined(HAVE_AL_AL_H)
#include <AL/al.h>
#endif

#ifdef HAVE_OPENAL_ALC_H
#include <OpenAL/alc.h>
#elif defined(HAVE_AL_ALC_H)
#include <AL/alc.h>
#endif

#ifdef HAVE_ALUT_ALUT_H
#include <ALUT/alut.h>
#elif defined(HAVE_AL_ALUT_H)
#include <AL/alut.h>
#endif

#include "Config.h"

#include "extern.h"
#include "define.h"
#include "Global.h"

#ifdef OLD_OPENAL
#ifdef USE_SDL 
	//try to use OpenAL alc[GS]etAudioChannel extensions in linux...
	#ifdef __linux__ 
		#define CD_VOLUME 1
//		#include <AL/alext.h>
		#include <AL/alkludgetypes.h>
	#endif //__linux__
#endif //USE_SDL
#else
#ifdef USE_SDL 
	//try to use OpenAL alc[GS]etAudioChannel extensions in linux...
	#ifdef __linux__ 
		#define CD_VOLUME 1
		#include <AL/alext.h>
//		#include <AL/alexttypes.h>
	#endif //__linux__
#endif //USE_SDL
#endif

// Don't try to build the CD volume stuff if it isn't available
#ifndef ALC_CHAN_CD_LOKI
#undef CD_VOLUME
#endif

#ifdef __linux__
#define USE_PLAYLIST 1
#endif

//====================================================================
AudioOpenAL::AudioOpenAL()
	: Audio()
{
	Config	*config = Config::instance();
	int i;
	context_id	= 0;
	initialized = false;
	explosionIndex = 0;
	exploPopIndex = 0;
	origCDvolume = 0.7;

	//-- OpenAL extensions
	alAttenuationScale	= 0;
	alcSetAudioChannel	= 0;
	alcGetAudioChannel	= 0;
	alutLoadMP3			= 0;
	alutLoadVorbis		= 0;
	
	soundQueue = new SoundInfo;
	
	for(i = 0; i < NumSoundTypes; i++)
	{
		buffer[i]	= 0;
		source[i]	= 0;
		gain[i]		= 0;
		sourcePos[i][0] = 0.0;
		sourcePos[i][1] = 0.0;
		sourcePos[i][2] = 0.0;
		numReqThisFrame[i] = 0;
	}
	
	for(i = 0; i < NUM_EXPLO; i++)
	{
		sourceExplosion[i]	= 0;
	}
	for(i = 0; i < NUM_EXPLO_POP; i++)
	{
		sourceExploPop[i]	= 0;
	}
	
	// NOTE: If we can't create a valid context, config->audioEnabled() will
	//       be set to false.
	if(config->audioEnabled() == true) 
		config->setAudio(createContext());
	
	if(config->audioEnabled() == true)
	{
		initSound();

		if(alAttenuationScale)
		{
			audioScale[0] = 3.0;
			audioScale[1] = 1.0;
			audioScale[2] = 1.0;
		}
		else
		{
			if( config->debug() ) fprintf(stderr, _("alAttenuationScale == 0. Kludge it.\n"));
			audioScale[0] = 0.5;
			audioScale[1] = 0.3;
			audioScale[2] = 0.3;
			alDistanceModel(AL_NONE);
		}

		if(config->swapStereo())
			audioScale[0] = -audioScale[0];
		
		if(config->usePlayList() && !cdrom)
			loadMusicList();
	}
	if( config->debug() ) fprintf(stderr, _("AudioOpenAL::Audio done\n"));
}

AudioOpenAL::~AudioOpenAL()
{
	if(initialized)
	{
		Config	*config = Config::instance();
		if( config->debug() ) fprintf(stderr, _("stopping OpenAL..."));
	
		#ifdef CD_VOLUME
		if(cdrom && alcSetAudioChannel)
		{ 
			alcSetAudioChannel(ALC_CHAN_CD_LOKI, origCDvolume);
		}
		#endif //CD_VOLUME

		if(cdrom)
			SDL_CDStop(cdrom);

		checkError(_("AudioOpenAL::~Audio()"));

		alDeleteSources(NUM_EXPLO_POP-1, sourceExploPop);
		alDeleteSources(NUM_EXPLO-1, sourceExplosion);
		alDeleteSources(NumSoundTypes, source);
		alDeleteBuffers(NumSoundTypes, buffer);

		#ifdef _WIN32
		if(context_id)
			alcDestroyContext((ALCcontext*)context_id);
		#else
		if(context_id)
			alcDestroyContext(context_id);
		#endif

		alcCloseDevice(dev);
		
		alutExit();

		if( config->debug() ) fprintf(stderr, _("done.\n"));
	}

	delete soundQueue;
}

//----------------------------------------------------------
#ifdef _WIN32
static void warning(const char *msg, ...)
{
	va_list ap;
	va_start( ap, msg );
	vfprintf( stderr, msg, ap );
	va_end( ap );
	fprintf( stderr, "\n");
}
#else
static void warning(const char *, ...)
{
}
#endif

/**
 * create OpenAL context. 
 * @returns success
 */
//----------------------------------------------------------
bool AudioOpenAL::createContext()
{
#ifdef _WIN32
 	dev=alcOpenDevice((unsigned char*)"DirectSound",NULL);
	context_id=(void*)alcCreateContext(dev,NULL);
	alcMakeContextCurrent((ALCcontext*)context_id);
#else
#ifdef OLD_OPENAL
	context_id = alcCreateContext(NULL);
#else
	dev = alcOpenDevice( NULL );
	context_id = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(context_id);
#endif
#endif

	if(!context_id)
	{
		fprintf(stderr, _(
			"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			"!! ATTENTION !! - one or more errors were encountered during audio check.\n"
			"!!                Audio will be disabled.\n"
			"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
		));
	}
	return (bool)context_id;
}

/**
 * Initialize OpenAL. The context should already have been created
 * in createContext()
 */
//----------------------------------------------------------
void AudioOpenAL::initSound()
{
	Config	*config = Config::instance();
	int i;
	ALfloat pos[] = { 0.0, -5.0,  25.0 };
	
	if( config->debug() ) fprintf(stderr, _("AudioOpenAL::initSound() begin...\n"));
			
	if( config->debug() ) {
		fprintf(stderr, _(
			"-OpenAL-----------------------------------------------------\n"
			"Vendor     : %s\n"
			"Renderer   : %s\n"
			"Version    : %s\n"),
			alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_VERSION) );
		printExtensions(stderr,  (const char*)alGetString( AL_EXTENSIONS ));
		fprintf(stderr, _("------------------------------------------------------------\n"));
	}

	alutInitWithoutContext(0, NULL);

	checkForExtensions();
	
	alListenerfv(AL_POSITION, pos);
	
#if !defined(ALUT_API_MAJOR_VERSION) || ALUT_API_MAJOR_VERSION < 1
	alGenBuffers(NumSoundTypes, buffer);
#endif
	alGenSources(NumSoundTypes, source);
	
	loadSounds();
	
	for(i = 0; i < NumSoundTypes; i++)
	{
		switch(i)
		{
			case PowerUp:	gain[PowerUp]	= 0.7;	break;
			case ExploPop:	gain[ExploPop]	= 0.8;	break;
			case MusicGame:	gain[MusicGame]	= 0.7;	break;
			case MusicMenu:	gain[MusicMenu]	= 0.7;	break;
			default:
				gain[i] = 1.0;
				break;
		}
	}
	
	for(i = 0; i < NumSoundTypes; i++)
	{
		warning(_("Audio.cpp - init %s"), fileNames[i]);
		alSourcefv( source[i], AL_POSITION, pos );
		alSourcei ( source[i], AL_LOOPING, AL_FALSE);
		alSourcei ( source[i], AL_BUFFER, buffer[i] );
		alSourcef ( source[i], AL_GAIN, gain[i]);
		checkError();
	}
	
	//-- We want to have overlapped Explosion
	alGenSources(NUM_EXPLO-1, sourceExplosion);
	sourceExplosion[NUM_EXPLO-1] = source[Explosion];
	for(i = 0; i < NUM_EXPLO-1; i++)
	{
		warning(_("Audio.cpp - init explosion %d"), i);
		alSourcefv( sourceExplosion[i], AL_POSITION, pos );
		alSourcei ( sourceExplosion[i], AL_LOOPING, AL_FALSE);
		alSourcei ( sourceExplosion[i], AL_BUFFER, buffer[Explosion] );
		alSourcef ( sourceExplosion[i], AL_GAIN, gain[Explosion]*(0.7+0.3*FRAND) );
		checkError();
	}
	
	//-- We want to have overlapped ExploPop
	alGenSources(NUM_EXPLO_POP-1, sourceExploPop);
	sourceExploPop[NUM_EXPLO_POP-1] = source[ExploPop];
	for(i = 0; i < NUM_EXPLO_POP-1; i++)
	{
		warning(_("Audio.cpp - init exploPop %d"), i);
		alSourcefv( sourceExploPop[i], AL_POSITION, pos );
		alSourcei ( sourceExploPop[i], AL_LOOPING, AL_FALSE);
		alSourcei ( sourceExploPop[i], AL_BUFFER, buffer[ExploPop] );
		alSourcef ( sourceExploPop[i], AL_GAIN, 0.0*gain[ExploPop]*(0.1+0.9*FRAND) );
		checkError();
	}
	
	
	setSoundVolume(config->volSound());
	setMusicVolume(config->volMusic());
		
	initialized = true;
	warning(_("Audio.cpp - initSound complete."), i);
}
	
//----------------------------------------------------------
void AudioOpenAL::checkForExtensions()
{
#ifndef _WIN32	// Win32 doesn't have any useful extensions yet, and 
				// the alGetProcAddress prototype is different (sigh).
				
	//-- check AttenuationScale extension
	alAttenuationScale = (void (*)(ALfloat param))
						alGetProcAddress("alAttenuationScale_LOKI");

	alGetError(); // Don't care what the problem is
	if(alAttenuationScale != NULL)
		alAttenuationScale(6.0);
	
	//-- check Audio Channel extension
	alcGetAudioChannel = (float (*)(ALuint channel))
						alGetProcAddress("alcGetAudioChannel_LOKI");
	alcSetAudioChannel = (void (*)(ALuint channel, ALfloat volume))
						alGetProcAddress("alcSetAudioChannel_LOKI");
#ifdef CD_VOLUME
	if(alcGetAudioChannel)
		origCDvolume = alcGetAudioChannel(ALC_CHAN_CD_LOKI);
#endif //CD_VOLUME
	
	//-- check MP3 extension
	alutLoadMP3 = (ALboolean (*)(ALuint, ALvoid *, ALint))
		alGetProcAddress("alutLoadMP3_LOKI");
	//-- check Ogg/Vorbis extension
	alutLoadVorbis = (ALboolean (*)(ALuint, ALvoid *, ALint))
		alGetProcAddress("alutLoadVorbis_LOKI");

#endif //_WIN32
}

/**
 * Stop OpenAL music. Also calls Audio::stopMusic()
 */
//----------------------------------------------------------
void AudioOpenAL::stopMusic()
{
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		Audio::stopMusic();
		alSourceStop(source[MusicMenu]);
		alSourceStop(source[MusicGame]);
	}
}

/**
 * Pause OpenAL music. Also calls Audio::pauseMusic()
 */
//----------------------------------------------------------
void AudioOpenAL::pauseGameMusic(bool status)
{
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		if(cdrom)
		{
			Audio::pauseGameMusic(status);
		}
		else
		{
			if(status)
				alSourcePause(source[MusicGame]);
			else
				alSourcePlay(source[MusicGame]);
		}
	}
}

//----------------------------------------------------------
void AudioOpenAL::setMusicMode(SoundType mode)
{
	Global	*game = Global::getInstance();
	Config	*config = Config::instance();
	if( config->debug() ) fprintf(stderr, _("AudioOpenAL::setMusicMode(SoundType mode)\n"));
	if(config->audioEnabled() == true)
	{
		Audio::setMusicMode(mode);
		switch(mode)
		{
			default:
			case MusicGame:
				alSourceStop (source[MusicMenu]);
				if(!cdrom)
				{
					alSourcei    (source[MusicGame], AL_LOOPING, AL_TRUE);
					if(!game->game_pause)
						alSourcePlay (source[MusicGame]);
				}
				break;
			case MusicMenu:
				if(!cdrom)
				{
					alSourcePause(source[MusicGame]);
				}
				alSourcei    (source[MusicMenu], AL_LOOPING, AL_TRUE);
				alSourcePlay (source[MusicMenu]);
				break;
		}
	}
}

/**
 * adjust gain of OpenAL sounds
 */
//----------------------------------------------------------
void AudioOpenAL::setSoundVolume(float vol)
{
	Config	*config = Config::instance();
	if( config->debug() ) fprintf(stderr, _("AudioOpenAL::setSoundVolume(%f)\n"), vol);
	if(config->audioEnabled() == true)
	{
		int i;
		if(vol > 1.0) vol = 1.0;
		if(vol < 0.0) vol = 0.0;
		soundVolume = vol;
		for(i = 0; i < MusicGame; i++)
		{
			alSourcef ( source[i], AL_GAIN, gain[i]*vol);
		}
		for(i = 0; i < NUM_EXPLO; i++)
		{
			alSourcef ( sourceExplosion[i], AL_GAIN, (0.5+(FRAND*0.5))*gain[Explosion]*soundVolume);
		}
		for(i = 0; i < NUM_EXPLO_POP; i++)
		{
			alSourcef (  sourceExploPop[i], AL_GAIN, (0.2+(FRAND*0.8))*gain[Explosion]*soundVolume);
		}
	}	
}

/**
 * adjust gain of OpenAL music. Also adjusts CD channel of hardware
 * mixer if appropriate OpenAL extensions are available.
 */
//----------------------------------------------------------
void AudioOpenAL::setMusicVolume(float vol)
{
	Config	*config = Config::instance();
	if(config->audioEnabled() == true)
	{
		if(vol > 1.0) vol = 1.0;
		if(vol < 0.0) vol = 0.0;
		musicVolume = vol;
		alSourcef ( source[MusicGame], AL_GAIN, gain[MusicGame]*musicVolume);
		alSourcef ( source[MusicMenu], AL_GAIN, gain[MusicGame]*musicVolume);
#ifdef CD_VOLUME
		if(cdrom && alcSetAudioChannel)
		{
			alcSetAudioChannel(ALC_CHAN_CD_LOKI, musicVolume);
		}
#endif
		if( config->debug() ) fprintf(stderr, _("Music volume = %f\n"), vol);
	}
}

/**
 * load WAV sound effects
 */
//----------------------------------------------------------
void AudioOpenAL::loadSounds()
{
	int i;

	for(i = 0; i < NumSoundTypes; i++)
	{
		if(!fileNames[i])
		{
			fprintf(stderr, _("\nERROR! - sound enumerant index %d does not have valid filename!\n"), i);
		}
		else
		{
#if defined(ALUT_API_MAJOR_VERSION) && ALUT_API_MAJOR_VERSION >= 1
			buffer[i] = alutCreateBufferFromFile(dataLoc(fileNames[i]));
			if( buffer[i] == AL_NONE ) checkError();
#else
			ALsizei size, freq;
			ALenum format;
			ALvoid *data;
			ALboolean loop;
#ifndef _WIN32
#ifdef __APPLE__
			alutLoadWAVFile(const_cast<ALbyte*>(dataLoc(fileNames[i])), &format, &data, &size, &freq);
#else
			alutLoadWAVFile(const_cast<ALbyte*>(dataLoc(fileNames[i])), &format, &data, &size, &freq, &loop);
#endif
			alBufferData (buffer[i], format, data, size, freq);
			alutUnloadWAV(format,data,size,freq);
#else //_WIN32
			char nameBuffer[256];
			sprintf(nameBuffer, "%s", dataLoc(fileNames[i]));
			if( config->debug() ) fprintf(stderr, _("alutLoadWAVFile(\"%s\",...);\n"), nameBuffer);
			alutLoadWAVFile(nameBuffer,&format,&data,&size,&freq);
			alBufferData(buffer[i],format,data,size,freq);
			alutUnloadWAV(format,data,size,freq);
#endif//_WIN32
#endif
		}
	}
}

//----------------------------------------------------------
void AudioOpenAL::checkError(const char* tag)
{
	ALenum	error = alGetError();
	if(error != AL_NO_ERROR)
	{
		fprintf(stderr, _("ERROR!! <%s> alGetError() = %s\n"), tag, alGetString(error) );
	}
	ALCenum error2 = alcGetError(dev);
	if(error2 != ALC_NO_ERROR)
	{
		fprintf(stderr, _("ERROR!! <%s> alcGetError() = %s\n"), tag, alcGetString(dev, error2) );
	}
	error = alutGetError();
	if(error != ALUT_ERROR_NO_ERROR)
	{
		fprintf(stderr, _("ERROR!! <%s> alutGetError() = %s\n"), tag, alutGetErrorString(error) );
	}
}

/**
 * This should be called every frame to ensure that delayed sounds are played
 * and that the numReqThisFrame counter is reset to 0
 */
//----------------------------------------------------------
void AudioOpenAL::update()
{
	SoundInfo *sound;
	SoundInfo *nextSound;
	SoundInfo *backSound;
	
	// play any delayed sounds
	sound = soundQueue->next;
	while(sound)
	{
		sound->age++;
		if(sound->age >= 0)
		{
			backSound = sound->back;
			nextSound = sound->next;
			backSound->next = nextSound;
			if(nextSound)
				nextSound->back = backSound;
			playSound(sound->type, sound->pos);
			delete sound;
			sound = backSound;
		}
		sound = sound->next;
	}
	
	// reset the number of requests to 0
	for(int i = 0; i < NumSoundTypes; i++)
	{
		numReqThisFrame[i] = 0;
	}
}

/**
 * play sound of a given type. 
 * @param type type of sound to play
 * @param pos position of the sound in world coords
 * @param age If age is negative, playback of the sound will
 * be delayed |age| frames.
 */
//----------------------------------------------------------
void AudioOpenAL::playSound(SoundType type, float *pos, int age)
{
	Config	*config = Config::instance();
	if(config->audioEnabled())
	{
		float p[3];
		p[0] =  pos[0]*audioScale[0];
		p[1] =  pos[1]*audioScale[1];
		p[2] =  pos[2]*audioScale[2];
		
		if(age >= 0)
		{
			numReqThisFrame[type]++;
			// only play one sound of any given type per frame...
			if(numReqThisFrame[type] < 2) 
			{
				switch(type)
				{
					case Explosion:
						playSoundExplosion(p);
						break;
					case ExploPop:
						playSoundExploPop(p);
						break;
					default:
						alSourceStop(source[type]);
						alSourcefv( source[type], AL_POSITION, p );
						alSourcePlay(source[type]);
//						fprintf(stderr, _("play %d\n"), type);
						break;
				}
			}
		}
		else
		{
			SoundInfo *first = soundQueue->next;
			SoundInfo *newSound;
			newSound = new SoundInfo(type, pos, age);
			newSound->back = soundQueue;
			newSound->next = soundQueue->next;
			soundQueue->next = newSound;
			if(first)
				first->back = newSound;
		}
	}
}

//----------------------------------------------------------
void AudioOpenAL::playSoundExplosion(float p[3])
{
	explosionIndex++;
	explosionIndex = explosionIndex%NUM_EXPLO;
	
	alSourceStop(sourceExplosion[explosionIndex]);
	alSourcefv  (sourceExplosion[explosionIndex], AL_POSITION, p );
	alSourcePlay(sourceExplosion[explosionIndex]);
}

//----------------------------------------------------------
void AudioOpenAL::playSoundExploPop(float p[3])
{
	p[0] *= 1.5;
	exploPopIndex++;
	exploPopIndex = exploPopIndex%NUM_EXPLO_POP;
	
	alSourceStop(sourceExploPop[exploPopIndex]);
	alSourcefv  (sourceExploPop[exploPopIndex], AL_POSITION, p );
	alSourcePlay(sourceExploPop[exploPopIndex]);
}

//----------------------------------------------------------
void AudioOpenAL::loadMusicList()
{
	Config	*config = Config::instance();
	int		i;
	bool	ok = false;
	int		lineCount = 0;
	char	buffer[1024];
	char	configFilename[256];
	FILE	*file;
	
	const char *homeDir = getenv("HOME");
	if(!homeDir)
		homeDir = "./";
	
	musicMax = 0;
	musicIndex = 0;
	sprintf(configFilename, "%s/%s", homeDir, CONFIG_MUSIC_FILE);
	alterPathForPlatform(configFilename);
	file = fopen(configFilename, "r");
	if(!file)
	{
		char	configFilename[256];
		sprintf(configOldFilename, "%s/.chromium-music"CONFIG_EXT, homeDir);
		alterPathForPlatform(configOldFilename);
		rename(configOldFilename,configFilename)
		file = fopen(configFilename, "r");
	}

	if(file)
	{	
		while( fgets(buffer, 1024, file) )
		{
			int tmp = strlen(buffer);
			buffer[tmp-1] = '\0'; //-- get rid of new line
			
			ok = false;
			lineCount++;
			if(strlen(buffer) > 255)
			{
				fprintf(stderr, _("ERROR: filename too long: \"%s\", line %d\n"), configFilename, lineCount);
			}
			else if(strlen(buffer) > 4)
			{
				switch(extensionFormat(buffer))
				{	
					case WAV:
						strcpy(musicFile[musicMax], buffer);
						musicMax++;
						break;
					case MP3:
						if(alutLoadMP3)
						{
							strcpy(musicFile[musicMax], buffer);
							musicMax++;							
						}
						else
						{
							fprintf(stderr, _("WARNING: MP3 support not available: \"%s\", line %d\n"), configFilename, lineCount);
						}
						break;
					case OGG:
						if(alutLoadVorbis)
						{
							strcpy(musicFile[musicMax], buffer);
							musicMax++;							
						}
						else
						{
							fprintf(stderr, _("WARNING: Ogg/Vorbis support not available: \"%s\", line %d\n"), configFilename, lineCount);
						}
						break;
					default:
						fprintf(stderr, _("WARNING: file type not recognized: \"%s\": \"%s\", line %d\n"), buffer, configFilename, lineCount);
						break;
				}
			}
			if(musicMax+1 >= MAX_MUSIC) // limited music array size.
				break;
		}
	}
	else
		config->setUsePlayList(false);
	if(musicMax < 1)
		config->setUsePlayList(false);
	
	fprintf(stderr, _("music playlist:\n"));
	for(i = 0; i < musicMax; i++)
	{
		fprintf(stderr, _(" %2d : %s\n"), i+1, musicFile[i]);
	}
	fprintf(stderr, _("loaded %s\n"), configFilename);
}

/**
 * Set music track to index. If CDROM is enabled, calls Audio::setMusicIndex().
 * If playList is enabled, set to Nth track. 
 */
//----------------------------------------------------------
void AudioOpenAL::setMusicIndex(int index)
{
	Config	*config = Config::instance();
	bool	wasPlaying = false;
	
	if(musicMax)
		musicIndex = index%musicMax;
		
	if(initialized && cdrom)
	{
		Audio::	setMusicIndex(index);		
	}
	else if(initialized && config->usePlayList())
	{
		bool loadSuccess = true;
		if(config->audioEnabled())
		{
			checkError(_("AudioOpenAL::setMusicIndex -- begin"));
			//-- if music is currently playing, we want to 
			//   re-start playing after loading new song
			ALint state = AL_INITIAL;
#ifdef OLD_OPENAL
			alGetSourcei(source[MusicGame], AL_SOURCE_STATE, &state);
#else
			alGetSourceiv(source[MusicGame], AL_SOURCE_STATE, &state);
#endif
			if(state == AL_PLAYING) 
				wasPlaying = true;

			alSourceStop(source[MusicGame]);
			checkError(_("AudioOpenAL::setMusicIndex -- before setting source buffer to 0"));
			alSourcei(source[MusicGame], AL_BUFFER, 0);	
//			alSourcei(source[MusicGame], AL_BUFFER, AL_NONE);	
			checkError(_("AudioOpenAL::setMusicIndex -- after setting source buffer to 0"));
			alDeleteBuffers(1 , &buffer[MusicGame]);
			alGenBuffers(1, &buffer[MusicGame]);
			switch(extensionFormat(musicFile[musicIndex]))
			{
				case WAV:
					loadSuccess = loadWAV(musicFile[musicIndex]);
					break;
				case MP3:
					loadSuccess = loadMP3(musicFile[musicIndex]);
					break;
				case OGG:
					loadSuccess = loadVorbis(musicFile[musicIndex]);
					break;
				default:
					loadSuccess = false;
					break;
			}
			if(loadSuccess)
			{
				ALfloat pos[] = { 0.0, -5.0,  25.0 };
				alSourcei ( source[MusicGame], AL_BUFFER, buffer[MusicGame]);
				alSourcefv( source[MusicGame], AL_POSITION, pos );
				alSourcef ( source[MusicGame], AL_GAIN, gain[MusicGame]*musicVolume);
				alSourcei ( source[MusicGame], AL_LOOPING, AL_TRUE);
				if(wasPlaying)
					alSourcePlay(source[MusicGame]);

			}
			else
			{
				//-- fall back to default loop
				loadSuccess = loadWAV(dataLoc(fileNames[MusicGame]));
			}
		}
		checkError(_("AudioOpenAL::setMusicIndex -- end"));
	}
	else if(initialized)
	{
		ALint state = AL_INITIAL;
#ifdef OLD_OPENAL
		alGetSourcei(source[MusicGame], AL_SOURCE_STATE, &state);
#else
		alGetSourceiv(source[MusicGame], AL_SOURCE_STATE, &state);
#endif
		if(state == AL_PLAYING) 
			wasPlaying = true;
		alSourceStop(source[MusicGame]);
		if(wasPlaying)
			alSourcePlay(source[MusicGame]);
	}
}

#ifndef __linux__
// kludge so things compile on Win32 and Mac
int strcasecmp(const char *a, const char *b)
{
	return strcmp(a, b);
}
#endif

//----------------------------------------------------------
AudioOpenAL::AudioFormat AudioOpenAL::extensionFormat(char* filename)
{
	AudioFormat retVal = Unknown;
	char *walker;
	walker = strrchr(filename, '.');
	if(walker)
	{
		if     (strcasecmp(walker, ".wav") == 0)
			retVal = WAV;
		else if(strcasecmp(walker, ".mp3") == 0)
			retVal = MP3;
		else if(strcasecmp(walker, ".ogg") == 0)
			retVal = OGG;
	}
	else 
		retVal = Unknown;

	return retVal;
}
//----------------------------------------------------------
bool AudioOpenAL::loadWAV(const char *filename)
{
#ifndef USE_PLAYLIST
	fprintf(stderr, _("WARNING: PlayList support not compiled in (AudioOpenAL.cpp): %s\n"), filename);
	return false;
#else //USE_PLAYLIST
#if defined(ALUT_API_MAJOR_VERSION) && ALUT_API_MAJOR_VERSION >= 1
	buffer[MusicGame] = alutCreateBufferFromFile(const_cast<ALbyte*>(filename));
	if( buffer[MusicGame] == AL_NONE ) checkError();
	return buffer[MusicGame] != AL_NONE;
#else
	bool retVal;
	ALsizei size, freq;
	ALenum format;
	ALvoid *data;
	ALboolean loop;
#ifdef __APPLE__
	alutLoadWAVFile(const_cast<ALbyte*>(filename), &format, &data, &size, &freq);
#else
	alutLoadWAVFile(const_cast<ALbyte*>(filename), &format, &data, &size, &freq, &loop);
#endif
	retVal = (alGetError() == AL_NO_ERROR);
	if(retVal)
	{
		alBufferData (buffer[MusicGame], format, data, size, freq);
		alutUnloadWAV(format,data,size,freq);
	}
	return retVal;
#endif
#endif//USE_PLAYLIST
}

//----------------------------------------------------------
bool AudioOpenAL::loadMP3(const char *filename)
{
#ifndef USE_PLAYLIST
	fprintf(stderr, _("WARNING: PlayList support not compiled in (AudioOpenAL.cpp): %s\n"), filename);
	return false;
#else//USE_PLAYLIST
	FILE	*file;
	struct	stat sbuf;
	int		size;
	void	*data;
	
	if(stat(filename, &sbuf) == -1) 
	{
		perror(filename);
		return false;
	}
	size = sbuf.st_size;
	data = malloc(size);
	if(!data)
	{
		fprintf(stderr, _("ERROR: Could not allocate memory in AudioOpenAL::loadMP3\n"));
		return false;
	}	
	file = fopen(filename, "rb");
	if(!file)
	{
		fprintf(stderr, _("ERROR: Could not open \"%s\" in AudioOpenAL::loadMP3\n"), filename);
		free(data);
		return false;
	}	
	fread(data, 1, size, file);
	fclose(file);
	if( !(alutLoadMP3(buffer[MusicGame], data, size)) ) 
	{
		fprintf(stderr, _("ERROR: alutLoadMP3() failed in AudioOpenAL::loadMP3\n"));
		free(data);
		return false;
	}
	free(data);
	return true;
#endif//USE_PLAYLIST
}

//----------------------------------------------------------
bool AudioOpenAL::loadVorbis(const char *filename)
{
#ifndef USE_PLAYLIST
	fprintf(stderr, _("WARNING: PlayList support not compiled in (AudioOpenAL.cpp): %s\n"), filename);
	return false;
#else//USE_PLAYLIST
	if(initialized)
	{
		FILE	*file;
		struct	stat sbuf;
		int		size;
		void	*data;
		
		if(stat(filename, &sbuf) == -1) 
		{
			perror(filename);
			return false;
		}
		size = sbuf.st_size;
		data = malloc(size);
		if(!data)
		{
			fprintf(stderr, _("ERROR: Could not allocate memory in AudioOpenAL::loadVorbis\n"));
			return false;
		}	
		file = fopen(filename, "rb");
		if(!file)
		{
			fprintf(stderr, _("ERROR: Could not open \"%s\" in AudioOpenAL::loadVorbis\n"), filename);
			free(data);
			return false;
		}	
		fread(data, 1, size, file);
		fclose(file);
		if( !(alutLoadVorbis(buffer[MusicGame], data, size)) ) 
		{
			fprintf(stderr, _("ERROR: alutLoadVorbis() failed in AudioOpenAL::loadVorbis\n"));
			free(data);
			return false;
		}
		free(data);
		return true;
	}
	else
		return false;
#endif//USE_PLAYLIST
}

#endif // AUDIO_OPENAL


