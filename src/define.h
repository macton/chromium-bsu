/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef define_h
#define define_h

//#define SRAND (2.0*(0.5-(rand()/(double)RAND_MAX)))
//#define FRAND (rand()/(double)RAND_MAX)
//#define IRAND (rand())
#define SRAND (Global::randS[(++Global::randIndex)%256])
#define FRAND (Global::randF[(++Global::randIndex)%256])
#define IRAND (Global::randI[(++Global::randIndex)%256])

#define INT_GAME_SKILL_BASE ((int)((Global::gameSkillBase+0.05)*10.0))

#define AMMO_REFILL 150
#define HERO_Z	25.0f

#define MAX_SCREEN_SIZE 4 // 4 == 1280x960

#define NUM_ENEMY_AMMO_TYPES 5
#define NUM_HERO_AMMO_TYPES 3
#define NUM_HERO_ITEMS 1

#define HERO_DAMAGE -500.0
#define HERO_SHIELDS 500.0 

#ifdef macintosh
#define CONFIG_FILE			"chromium.cfg"
#define CONFIG_SCORE_FILE	"chromium-score.cfg"
#define CONFIG_MUSIC_FILE	"chromium-music.cfg"
#else
#define CONFIG_FILE			".chromium"
#define CONFIG_SCORE_FILE	".chromium-score"
#define CONFIG_MUSIC_FILE	".chromium-music"
#endif //macintosh

#define HI_SCORE_HIST 5

#define MAX_BUFF 256

#ifdef PKGBINDIR
#define CHROMIUM_EXEC PKGBINDIR"/chromium"
#else 
#define CHROMIUM_EXEC "./chromium"
#endif //PKGBINDIR

#ifdef PKGDATADIR
#define CHROMIUM_DATA_DIR PKGDATADIR"/data"
#else
#define CHROMIUM_DATA_DIR "../data"
#endif //PKGDATADIR

#endif // define_h
