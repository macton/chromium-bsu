/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef define_h
#define define_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

//#define SRAND (2.0*(0.5-(rand()/(double)RAND_MAX)))
//#define FRAND (rand()/(double)RAND_MAX)
//#define IRAND (rand())
#define SRAND (Global::randS[(++Global::randIndex)%256])
#define FRAND (Global::randF[(++Global::randIndex)%256])
#define IRAND (Global::randI[(++Global::randIndex)%256])

#define AMMO_REFILL 150
#define HERO_Z	25.0f

#define MIN_SCREEN_SIZE 0 // 512x384
#define MAX_SCREEN_SIZE 4 // 1280x960

#define NUM_ENEMY_AMMO_TYPES 5
#define NUM_HERO_AMMO_TYPES 3
#define NUM_HERO_ITEMS 1

#define HERO_DAMAGE -500.0
#define HERO_SHIELDS 500.0 

#ifdef macintosh
#define CONFIG_EXT			".cfg"
#define CONFIG_FILE			PACKAGE".cfg"
#define CONFIG_SCORE_FILE	PACKAGE"-score.cfg"
#define CONFIG_MUSIC_FILE	PACKAGE"-music.cfg"
#else
#define CONFIG_EXT
#define CONFIG_FILE			"."PACKAGE
#define CONFIG_SCORE_FILE	"."PACKAGE"-score"
#define CONFIG_MUSIC_FILE	"."PACKAGE"-music"
#endif //macintosh

#define HI_SCORE_HIST 5

#endif // define_h
