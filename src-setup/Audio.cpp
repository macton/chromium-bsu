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

#include "Audio.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef _WIN32
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

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

//====================================================================
Audio::Audio()
{
	initialized = false;
	
	alAttenuationScale	= 0;
	alutLoadMP3			= 0;
	alutLoadVorbis		= 0;
	
	strcpy(formatString[WAV], "*.wav;*.WAV");
	strcpy(formatString[MP3], "*.mp3;*.MP3");
	strcpy(formatString[OGG], "*.ogg;*.OGG");
	strcpy(formatString[NumAudioFormats], "");
	
	for(int i = 0; i < NumSoundTypes; i++)
	{
		buffer[i]	= 0;
		source[i]	= 0;
	}

	initAudio();	
}

Audio::~Audio()
{
	killAudio();
}

//----------------------------------------------------------
void Audio::killAudio()
{
	fprintf(stderr, "stopping OpenAL...");
	if(initialized)
		alcDestroyContext(context_id);
	initialized = false;
	fprintf(stderr, "done.\n");	
}

//----------------------------------------------------------
bool Audio::initAudio()
{
//	fprintf(stderr, "Audio::initSound() begin...\n");
	bool retVal = true;
	ALfloat pos[] = { 0.0, -1.0, 0.0 };
	
	if(!initialized)
	{
		fprintf(stderr, "starting OpenAL...");
		context_id = alcCreateContext(NULL);
		initialized = (bool)context_id;
		fprintf(stderr, "done.\n");

		if(initialized)
		{
			fillInfo();

#ifndef _WIN32
			alAttenuationScale = (void (*)(ALfloat param))
				alGetProcAddress((const ALubyte *)"alAttenuationScale_LOKI");
			if(alAttenuationScale == NULL) 
				fprintf(stderr, "alAttenuationScale NOT present\n");
			else
				alAttenuationScale(1.0);

			alutLoadMP3 = (ALboolean (*)(ALuint, ALvoid *, ALint))
				alGetProcAddress((const ALubyte *)"alutLoadMP3_LOKI");
			if(alutLoadMP3)
				fprintf(stderr, "alutLoadMP3_LOKI present\n");
			else
				fprintf(stderr, "alutLoadMP3_LOKI NOT present\n");

			alutLoadVorbis = (ALboolean (*)(ALuint, ALvoid *, ALint))
				alGetProcAddress((const ALubyte *)"alutLoadVorbis_LOKI");
			if(alutLoadVorbis)
				fprintf(stderr, "alutLoadVorbis_LOKI present\n");
			else
				fprintf(stderr, "alutLoadVorbis_LOKI NOT present\n");
#endif

			alListenerfv(AL_POSITION, pos);

			buffer[MusicGame] = 0;
			alGenSources(1, &source[MusicGame]);
			alSourcei ( source[MusicGame], AL_LOOPING, AL_TRUE);
			alSourcef ( source[MusicGame], AL_GAIN, 1.0);
			retVal = true;
		}
		else
		{
			fprintf(stderr, "ERROR: failed to initialize OpenAL context!\n");
			checkError("Audio::initAudio()");
			retVal = false;
		}
	}
	else
	{
		fprintf(stderr, "WARNING: trying to re-initialize audio\n");
		retVal = true;
	}
	return retVal;
}
	
//----------------------------------------------------------
bool Audio::hasFormat(AudioFormat af)
{
	if(initialized)
	{
		switch(af)
		{
			case WAV:
				return true;
			case MP3:
				return (bool)alutLoadMP3;
			case OGG:
				return (bool)alutLoadVorbis;
			default:
				return false;
		}
	}
	else 
		return false;
}

	
//----------------------------------------------------------
const char* Audio::formatExtension(AudioFormat af)
{
	if( ((int)af < (int)NumAudioFormats) && ((int)af >= 0) )
	{
		return formatString[af];
	}
	else
		return "";
}

//----------------------------------------------------------
bool Audio::checkError(const char* tag)
{
	bool retVal = false;
	if(initialized)
	{
		ALenum	error = alGetError();
		if(error != AL_NO_ERROR)
		{
			sprintf(errorBuffer, "ERROR!! <%s> alGetError() = %s", tag, alGetString(error) );
			fprintf(stderr, "%s\n", errorBuffer);
			retVal = true;
		}
	}
	return retVal;
}

//----------------------------------------------------------
void Audio::fillInfo()
{
	//-- Yes, I know sprintf isn't 'safe'
	char *walker;
	char buffer[4096];
	if(initialized)
	{
		strcpy(infoBuffer, "");
		//sprintf(buffer, "-OpenAL---------------------------------------\n");	strcpy(infoBuffer, buffer);
		sprintf(buffer, "Vendor     : %s  \n", alGetString( AL_VENDOR ) );		strcat(infoBuffer, buffer);
		sprintf(buffer, "Renderer   : %s  \n", alGetString( AL_RENDERER ) );	strcat(infoBuffer, buffer);
		sprintf(buffer, "Version    : %s  \n", alGetString( AL_VERSION ) );		strcat(infoBuffer, buffer);
		sprintf(buffer, "Extensions : \n");										strcat(infoBuffer, buffer);
		sprintf(buffer, "%s ", alGetString(AL_EXTENSIONS));
		walker = buffer;
		while(*walker)
		{
			if(*walker == ' ')
				*walker = '\n';
			walker++;
		}
		strcat(infoBuffer, buffer);
		//sprintf(buffer, "----------------------------------------------\n");	strcat(infoBuffer, buffer);
	}
	else
	{
		sprintf(infoBuffer, "ERROR!! OpenAL is not available!\n");		
	}
}

//----------------------------------------------------------
bool Audio::playSound(const char *filename)
{
	bool loadSuccess = true;
	
	if(initialized)
	{
		ALfloat pos[] = { 0.0, 0.0, 0.0 };
		stopSound(); //sets buffer to 0

		alGenBuffers(1, &buffer[MusicGame]);

		switch(extensionFormat(filename))
		{
			case WAV:
				loadSuccess = loadWAV(filename);
				break;
			case MP3:
				loadSuccess = loadMP3(filename);
				break;
			case OGG:
				loadSuccess = loadVorbis(filename);
				break;
			default:
				sprintf(errorBuffer, "ERROR: Unknown file type\n(Click on OpenAL button to see supported filetypes)");
				loadSuccess = false;
				break;
		}
		if(!loadSuccess)
		{
			alDeleteBuffers(1 , &buffer[MusicGame]);
			buffer[MusicGame] = 0;
		}

		if(buffer[MusicGame])
		{
			alSourcei ( source[MusicGame], AL_BUFFER, buffer[MusicGame]);
			alSourcefv( source[MusicGame], AL_POSITION, pos );
			alSourcef ( source[MusicGame], AL_GAIN, 1.0);
			alSourcei ( source[MusicGame], AL_LOOPING, AL_TRUE);
			alSourcePlay(source[MusicGame]);
		}
	}
	else
		sprintf(errorBuffer, "ERROR: OpenAL not initialized");
	
	return loadSuccess;
}


//----------------------------------------------------------
void Audio::stopSound()
{
	if(initialized && buffer)
	{
		alSourceStop(source[MusicGame]);
		alSourcei(source[MusicGame], AL_BUFFER, 0);
		alDeleteBuffers(1 , &buffer[MusicGame]);
		buffer[MusicGame] = 0;
	}
}

//----------------------------------------------------------
bool Audio::loadWAV(const char *filename)
{
	bool retVal = false;
	if(initialized)
	{
		ALsizei size, freq, bits;
		ALenum format;
		ALvoid *data;
		struct	stat sbuf;
		
		if(stat(filename, &sbuf) == -1) 
		{
			sprintf(errorBuffer, "ERROR: \"%s\"\n", filename);
			strcat(errorBuffer, strerror(errno));
			return false;
		}
		
		retVal = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);
		if(retVal)
		{
			alBufferData (buffer[MusicGame], format, data, size, freq);
			free(data);	
		}
		else
		{
			sprintf(errorBuffer, "ERROR: alutLoadWAV failed for\n\"%s\"\nin Audio::loadWAV", filename);
			fprintf(stderr, "%s\n", errorBuffer);
		}
	}
	return retVal;
}

//----------------------------------------------------------
bool Audio::loadMP3(const char *filename)
{
	if(initialized)
	{
		if(alutLoadMP3)
		{
			FILE	*file;
			struct	stat sbuf;
			int		size;
			void	*data;

			if(stat(filename, &sbuf) == -1) 
			{
				sprintf(errorBuffer, "ERROR: \"%s\"\n", filename);
				strcat(errorBuffer, strerror(errno));
				return false;
			}
			size = sbuf.st_size;
			data = malloc(size);
			if(!data)
			{
				sprintf(errorBuffer, "ERROR: Could not allocate memory in Audio::loadMP3");
				fprintf(stderr, "%s\n", errorBuffer);
				return false;
			}	
			file = fopen(filename, "rb");
			if(!file)
			{
				sprintf(errorBuffer, "ERROR: Could not open \"%s\" in Audio::loadMP3", filename);
				fprintf(stderr, "%s\n", errorBuffer);
				free(data);
				return false;
			}	
			fread(data, 1, size, file);
			fclose(file);
			if( !(alutLoadMP3(buffer[MusicGame], data, size)) ) 
			{
				sprintf(errorBuffer, "ERROR: alutLoadMP3() failed in Audio::loadMP3");
				fprintf(stderr, "%s\n", errorBuffer);
				free(data);
				return false;
			}
			free(data);
			return true;
		}
		else
		{
			sprintf(errorBuffer, "ERROR: MP3 playback not supported.\n(Click on OpenAL button to see supported filetypes)");
			fprintf(stderr, "%s\n", errorBuffer);
			return false;
		}
	}
	else
	{
		sprintf(errorBuffer, "ERROR: OpenAL not initialized!");
		fprintf(stderr, "%s\n", errorBuffer);
		return false;
	}
}

//----------------------------------------------------------
bool Audio::loadVorbis(const char *filename)
{
	if(initialized)
	{
		if(alutLoadVorbis)
		{
			FILE	*file;
			struct	stat sbuf;
			int		size;
			void	*data;

			if(stat(filename, &sbuf) == -1) 
			{
				sprintf(errorBuffer, "ERROR: \"%s\"\n", filename);
				strcat(errorBuffer, strerror(errno));
				return false;
			}
			size = sbuf.st_size;
			data = malloc(size);
			if(!data)
			{
				sprintf(errorBuffer, "ERROR: Could not allocate memory in Audio::loadVorbis\n");
				fprintf(stderr, "%s\n", errorBuffer);
				return false;
			}	
			file = fopen(filename, "rb");
			if(!file)
			{
				sprintf(errorBuffer, "ERROR: Could not open \"%s\" in Audio::loadVorbis\n", filename);
				fprintf(stderr, "%s\n", errorBuffer);
				free(data);
				return false;
			}	
			fread(data, 1, size, file);
			fclose(file);
			if( !(alutLoadVorbis(buffer[MusicGame], data, size)) ) 
			{
				sprintf(errorBuffer, "ERROR: alutLoadVorbis() failed in Audio::loadVorbis\n");
				fprintf(stderr, "%s\n", errorBuffer);
				free(data);
				return false;
			}
			free(data);
			return true;
		}
		else
		{
			sprintf(errorBuffer, "ERROR: Ogg/Vorbis playback not supported.\n(Click on OpenAL button to see supported filetypes)");
			fprintf(stderr, "%s\n", errorBuffer);
			return false;
		}
	}
	else
	{
		sprintf(errorBuffer, "ERROR: OpenAL not initialized!");
		fprintf(stderr, "%s\n", errorBuffer);
		return false;
	}
}

//----------------------------------------------------------
Audio::AudioFormat Audio::extensionFormat(const char* filename)
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
