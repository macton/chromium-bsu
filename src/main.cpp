/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "compatibility.h"
#include <GL/gl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#endif // _WIN32

#include "main.h"
#include "define.h"
#include "MainSDL.h"
#include "MainGLUT.h"

#include "Global.h"

static char dataDir[256] = "../data";

//----------------------------------------------------------
int main(int argc, char **argv)
{
	int i;
	int	tmp;
	int	vm = 2;

#ifdef __linux__
	// by default, disable SDL's use of DGA mouse. If SDL_VIDEO_X11_DGAMOUSE is
	// set however, use default value.
	setenv("SDL_VIDEO_X11_DGAMOUSE", "0", false);
#endif
	
	Global *game = Global::init();

	for ( i=1; i < argc; i++ )
	{
		if ( strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0 )
		{
			game->full_screen = true;
		}
		else if ( strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--window") == 0 )
		{
			game->full_screen = false;
		}
		else if ( strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vidmode") == 0 )
		{
			if((i+1) < argc)
			{
				sscanf(argv[i+1], "%d", &tmp);
				if(tmp <= MAX_SCREEN_SIZE && tmp >= 0)
				{
					vm = tmp;
					i++;
				}
			}
			game->screenSize = vm;
		}
		else if( strcmp(argv[i], "-na") == 0 || strcmp(argv[i], "--noaudio") == 0)
		{
			game->audio_enabled = false;
		}
		else if( strcmp(argv[i], "-nb") == 0 || strcmp(argv[i], "--noblend") == 0)
		{
			game->blend_enable = false;
		}
		else
		{
			fprintf(stderr, "\n");
			fprintf(stderr, "--------------------------------------------------\n");
			fprintf(stderr, "Chromium B.S.U. options\n");
			fprintf(stderr, "--------------------------------------------------\n");
			fprintf(stderr, "   -f/--fullscreen     : run in fullscreen mode\n");
			fprintf(stderr, "   -w/--window         : run in windowed mode\n");
			fprintf(stderr, "   -v/--vidmode <mode> : mode 0 =  512 x  384\n");
			fprintf(stderr, "                       :      1 =  640 x  480\n");
			fprintf(stderr, "                       :      2 =  800 x  600\n");
			fprintf(stderr, "                       :      3 = 1024 x  768\n");
			fprintf(stderr, "                       :      4 = 1280 x  960\n");
			fprintf(stderr, "  -na/--noaudio        : do not initialize audio\n");
			fprintf(stderr, "--------------------------------------------------\n");
			fprintf(stderr, "\n");
			exit(0);
		}
	}
	
#ifdef PKGDATADIR
	strcpy(dataDir, PKGDATADIR);
	strcat(dataDir, "/data");
#else	
	char *chromData = getenv("CHROMIUM_DATA");
	if(chromData)
		strcpy(dataDir, chromData);
	else
	{
		#ifdef macintosh
		strcpy(dataDir, "::data");	
		#else
		strcpy(dataDir, "../data");	
		#endif
		fprintf(stderr, "!!\n");
		fprintf(stderr, "!! WARNING - CHROMIUM_DATA environment variable is not set!\n");
		fprintf(stderr, "!!           Please read the INSTALL file and set the \n");
		fprintf(stderr, "!!           CHROMIUM_DATA variable to the data directory.\n");
		fprintf(stderr, "!!\n");
		fprintf(stderr, "!! 		  (using %s)\n", dataDir);
		fprintf(stderr, "!!\n");
	}
#endif

	srand(time(NULL));
	
	game->generateRandom();
	
#ifdef USE_SDL
	game->toolkit = new MainSDL(argc, argv);
#elif USE_GLUT
	game->toolkit = new MainGLUT(argc, argv);
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif
		
	game->printHiScore();
	game->toolkit->run();

	game->saveConfigFile();
	game->saveHiScoreFile();

	fprintf(stderr, "done.\n");
	fprintf(stderr, " \n");
	fprintf(stderr, "Download the latest version of Chromium B.S.U. at\n");
	fprintf(stderr, "http://www.reptilelabour.com/software/chromium\n");
	fprintf(stderr, " \n");
	
	return 0;
}

// Changes '/' to ':' for Mac paths, is a no-op for other platforms.
//----------------------------------------------------------
const char* alterPathForPlatform(char* filename)
{
#ifdef macintosh
	char    *walker = filename;
	while( *walker )
	{
		if(*walker == '/' )
			*walker =  ':';
		walker++;
	}
#endif
	return filename;
}

//----------------------------------------------------------
const char* dataLoc(const char* filename, bool doCheck)
{
	static char buffer[256];
	if( (strlen(dataDir)+strlen(filename)) < 254)
	{
        sprintf(buffer, "%s/%s", dataDir, filename);	
	}
	else
	{
		fprintf(stderr, "!! ERROR !! dataLoc -- filename too long!\n");
		fprintf(stderr, "!! dataDir  = \"%s\"\n", dataDir);
		fprintf(stderr, "!! filename = \"%s\"\n", filename);
		return "badFilename";
	}

	alterPathForPlatform(buffer);

#ifndef _WIN32 // WIN32 users don't get error checks...
	if(doCheck)
	{
		struct	stat sbuf;
		if(stat(buffer, &sbuf) == -1) 
		{
			fprintf(stderr, "!! ERROR !! - ");
			perror(buffer);
		}
	}
#endif

#ifdef CHECK_ERRORS
//	fprintf(stderr, "dataLoc(\"%s\")\n", buffer);
#endif
	return buffer;
}

