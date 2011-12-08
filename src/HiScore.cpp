/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "HiScore.h"
#include "extern.h"

#include "Config.h"

#include "HeroAircraft.h"

//====================================================================
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

HiScore	*HiScore::instance = 0;

static const char* header = "# Chromium B.S.U. high scores file: skill rank score date time(UTC) name (do not remove this comment line)\n";

/**
 * initialize variables, then read high score file
 */
//----------------------------------------------------------
HiScore::HiScore()
{
	int i,j;
	for(i = 0; i < 10; i++)
	{
		for(j = 0; j < HI_SCORE_HIST; j++)
		{
			//-- default high scores
			switch(j)
			{
				case 0: hiScore[i][j] = 250000.0; break;
				case 1: hiScore[i][j] = 200000.0; break;
				case 2: hiScore[i][j] = 150000.0; break;
				case 3: hiScore[i][j] = 100000.0; break;
				case 4: hiScore[i][j] =  50000.0; break;
				default: hiScore[i][j] = 99.0; break;
			}
			//-- default player
			sprintf(hiScoreName[i][j], _("nobody"));
			//-- default date (01/01/2000);
			hiScoreDate[i][j] = 946713600;
		}
	}
	readFile();
}

HiScore::~HiScore()
{
}

/**
 * create single HiScore object
 * @returns HiScore::instance
 */
//----------------------------------------------------------
HiScore *HiScore::init()
{
	if(!HiScore::instance)
	{
		HiScore::instance = new HiScore;
	}
	else
	{
		fprintf(stderr, _("WARNING: HiScore::init() has already been called.\n"));
	}
	return HiScore::instance;
}

/**
 * @returns HiScore::instance
 */
//----------------------------------------------------------
HiScore *HiScore::getInstance()
{
	if(!instance)
	{
		return HiScore::init();
	}
	else
		return HiScore::instance;
}

/**
 * deletes singleton instance and sets HiScore::instance to 0.
 */
//----------------------------------------------------------
void HiScore::destroy()
{
	delete HiScore::instance;
	HiScore::instance = 0;
}

/**
 * return score for given level and index.
 * @returns score, or -1 if skill,index is out of range
 */
//----------------------------------------------------------
double HiScore::getScore(int skill, int index)
{
	double retVal = -1.0;
	if(skill > 0 && skill < 10)
		if(index >= 0 && index < HI_SCORE_HIST)
			retVal = hiScore[skill][index];
	return retVal;
}

/**
 * return high scorer's name for given level and index.
 * @returns name, or "OUT_OF_RANGE" if skill,index is out of range
 */
//----------------------------------------------------------
const char *HiScore::getName(int skill, int index)
{
	const char *retVal = "OUT_OF_RANGE";
	if(skill > 0 && skill < 10)
		if(index >= 0 && index < HI_SCORE_HIST)
			retVal = hiScoreName[skill][index];
	return retVal;
}

/**
 * return date of high score for given level and index.
 * @returns date (int time_t format), or 0 if skill,index is out of range
 */
//----------------------------------------------------------
time_t HiScore::getDate(int skill, int index)
{
	int retVal = 0;
	if(skill > 0 && skill < 10)
		if(index >= 0 && index < HI_SCORE_HIST)
			retVal = hiScoreDate[skill][index];
	return retVal;
}

/**
 * If CHROMIUM_BSU_SCORE environment variable is set, that
 * filename will be used. Otherwise, the default score file.
 * @returns name of score file
 */
//----------------------------------------------------------
const char *HiScore::getFileName()
{
	static char	configFilename[256];
	const char *envFile = getenv("CHROMIUM_BSU_SCORE");
	if(envFile && strlen(envFile) < 256)
	{
		strcpy(configFilename, envFile);
	}
	else
	{
		const char *homeDir = getenv("HOME");
		if(!homeDir)
			homeDir = "./";
		sprintf(configFilename, "%s/%s", homeDir, CONFIG_SCORE_FILE);
		alterPathForPlatform(configFilename);
	}
	return configFilename;
}

/**
 * Prints the name of the old score file
 * @returns name of oldscore file
 */
//----------------------------------------------------------
const char *HiScore::getOldFileName()
{
	static char	configFilename[256];
	const char *homeDir = getenv("HOME");
	if(!homeDir)
		homeDir = "./";
	sprintf(configFilename, "%s/.chromium-score"CONFIG_EXT, homeDir);
	alterPathForPlatform(configFilename);
	return configFilename;
}

/**
 * Save high score file. 
 * @returns success
 */
//----------------------------------------------------------
bool HiScore::saveFile()
{
	bool retVal = true;
	FILE	*file;

	file = fopen(getFileName(), "w");
	if(file)
	{
#ifdef HAVE_LOCALE_H
		char* locale = setlocale(LC_NUMERIC,"C");
#endif
		struct tm* time;
		int i,j;
		fprintf(file, "%s", header);
		for(i = 0; i < 10; i++)
		{
			for(j = 0; j < HI_SCORE_HIST; j++)
			{
				time = gmtime(&hiScoreDate[i][j]);
				if( time != NULL ){
					fprintf(file, 
					        "%d %d %f %04d-%02d-%02d %02d:%02d:%02d %s\n",
					        i, j, 
					        hiScore[i][j],
					        1900+time->tm_year,
					        1+time->tm_mon,
					        time->tm_mday,
					        time->tm_hour,
					        time->tm_min,
					        time->tm_sec,
					        hiScoreName[i][j]);
				}
			}
		}
		fclose(file);
#ifdef HAVE_LOCALE_H
		setlocale(LC_NUMERIC,locale);
#endif
	}
	else
	{
		fprintf(stderr, _("WARNING: could not write score file (%s)\n"), getFileName());
		retVal = false;
	}
	return retVal;
}


/**
 * Read high score file.
 * @returns success
 */
//----------------------------------------------------------
bool HiScore::readFile()
{
	bool retVal = true;
	FILE	*file;

	const char* fileName = getFileName();
	file = fopen(fileName, "r");
	if(file)
	{
		int chr = fgetc(file);
		if( EOF != chr ){
			fseek(file, 0L, SEEK_SET);
			if( '#' == chr ){
				// Save and reset locale/timezone info
#ifdef HAVE_LOCALE_H
				char* locale = setlocale(LC_NUMERIC,"C");
#endif
				char *tz = getenv("TZ");
				setenv("TZ", "", 1);
				tzset();

				// Discard the comment line
				fscanf(file,"%*[^\n]");

				char name[100];
				struct tm time;
				int i, j;
				double score;
				int fields;
				do{
					i = j = -1;
					memset(&time,0,sizeof(time));
					fields = fscanf(file,
					                "%d %d %lf %d-%d-%d %d:%d:%d %99s%*[^\n]",
					                &i, &j,
					                &score,
					                &time.tm_year,
					                &time.tm_mon,
					                &time.tm_mday,
					                &time.tm_hour,
					                &time.tm_min,
					                &time.tm_sec,
					                name);
					if( fields == 10 && i >=0 && i < 10 && j >= 0 && j < HI_SCORE_HIST ){
						hiScore[i][j] = score;
						time.tm_year -= 1900;
						time.tm_mon--;
						hiScoreDate[i][j] = mktime(&time);
						strncpy(hiScoreName[i][j], name, 99);
						hiScoreName[i][j][99] = '\0';
					}
				} while( fields != EOF );
				fclose(file);

				// Reset locale/timezone info
#ifdef HAVE_LOCALE_H
				setlocale(LC_NUMERIC,locale);
#endif
				if (tz) setenv("TZ", tz, 1);
				else unsetenv("TZ");
				tzset();

			} else {
				// Nasty old memory dump format
				fread(hiScore,        sizeof(double), 10*HI_SCORE_HIST, file);
				fread(hiScoreName, 64*sizeof(char),   10*HI_SCORE_HIST, file);
				fread(hiScoreDate,    sizeof(time_t), 10*HI_SCORE_HIST, file);
				fclose(file);
				// Resave the file in plain text format
				saveFile();
			}
		}
		else 
		{
			Config* config = Config::instance();
			if( config->debug() ) fprintf(stderr, _("WARNING: empty score file (%s)\n"), getFileName());
			retVal = false;
		}
	}
	else
	{
		fileName = getOldFileName();
		file = fopen(fileName, "r");
		if(file)
		{
			fread(hiScore,        sizeof(double), 10*HI_SCORE_HIST, file);
			fread(hiScoreName, 64*sizeof(char),   10*HI_SCORE_HIST, file);
			fread(hiScoreDate,    sizeof(time_t), 10*HI_SCORE_HIST, file);
			fclose(file);

			// Try to save the new file and delete the old one if successful
			if( saveFile() )
				remove(fileName);
		}
		else 
		{
			Config* config = Config::instance();
			if( config->debug() ) fprintf(stderr, _("WARNING: could not read score file (%s)\n"), getFileName());
			retVal = false;
		}
	}
		
	return retVal;
}


//----------------------------------------------------------
void HiScore::insertScore(int skill, int rank, float score)
{
	int i;
	i = HI_SCORE_HIST-2;
	while(i >= rank)
	{
		hiScore[skill][i+1] = hiScore[skill][i];
		strcpy(hiScoreName[skill][i+1], hiScoreName[skill][i]);
		memcpy(&(hiScoreDate[skill][i+1]), &(hiScoreDate[skill][i]), sizeof(time_t));
		i--;
	}
	hiScore[skill][rank] = score;
	char *name = getenv("USER");
	if(name)
		strcpy(hiScoreName[skill][rank], name);
	else
		strcpy(hiScoreName[skill][rank], "player");
	time(&hiScoreDate[skill][rank]);
}

/**
 * reads high score file, inserts current score (if appropriate), then saves 
 * high score file. If multiple users are sharing a common high score file, 
 * we want to keep it as current as possible.
 */
//----------------------------------------------------------
int HiScore::set(int skill, float score)
{
	int retVal = 0;
	if(skill > 0 && skill < 10)
	{
		readFile();
		int i;
		int rank = -1;
		for(i = HI_SCORE_HIST-1; i >= 0; i--)
		{
			if(score > hiScore[skill][i])
				rank = i;
		}
		if(rank > -1)
		{
			insertScore(skill, rank, score);
			saveFile();
			retVal = rank+1;
		}
	}
			
	return retVal;
}

/**
 * check whether score qualifies as high score
 * returns rank of player {1..HI_SCORE_HIST}, or 0
 */
//----------------------------------------------------------
int HiScore::check(int skill, float score)
{
	int retVal = 0;
	if(skill > 0 && skill < 10)
	{
		int i;
		int rank = -1;
		for(i = HI_SCORE_HIST-1; i >= 0; i--)
		{
			if(score > hiScore[skill][i])
				rank = i;
		}
		if(rank > -1)
		{
			retVal = rank+1;
		}
	}
	return retVal;
}

/**
 * print high scores to stderr
 */
//----------------------------------------------------------
void HiScore::print(int skill)
{
	struct tm *tmptr; 
	fprintf(stderr, _("high scores:\n"));
	for(int j = 0; j < HI_SCORE_HIST; j++)
	{
		tmptr = localtime(&hiScoreDate[skill][j]);
		if (!tmptr)
			break;
		fprintf(stderr, _("%02d/%02d/%04d %16s %d\n"), 1+tmptr->tm_mon, tmptr->tm_mday, 1900+tmptr->tm_year,
				hiScoreName[skill][j], (int)(hiScore[skill][j]));
	}
}
