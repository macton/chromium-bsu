/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "HiScore.h"
#include "extern.h"

#include "HeroAircraft.h"

//====================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HiScore	*HiScore::instance = 0;

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
			sprintf(hiScoreName[i][j], "nobody");
			//-- default date (01/01/2000);
			hiScoreDate[i][j] = 946713600;
		}
	}
	readFile();
}

/**
 * save file before we go away...
 */
HiScore::~HiScore()
{
	saveFile();
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
		fprintf(stderr, "WARNING: HiScore::init() has already been called.\n");
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
	char *retVal = "OUT_OF_RANGE";
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
 * Save high score file. If CHROMIUM_SCORE environment variable is set, that
 * filename will be used. Otherwise, ~/.chromium-score.
 * @returns success
 */
//----------------------------------------------------------
bool HiScore::saveFile()
{
	bool retVal = true;
	char	configFilename[256];
	FILE	*file;

	const char *envFile = getenv("CHROMIUM_SCORE");
	if(envFile && strlen(envFile) < 256)
	{
		fprintf(stderr, "CHROMIUM_SCORE=%s\n", envFile);
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
	
	file = fopen(configFilename, "w");
	if(file)
	{
		fwrite(hiScore,        sizeof(double), 10*HI_SCORE_HIST, file);
		fwrite(hiScoreName, 64*sizeof(char),   10*HI_SCORE_HIST, file);
		fwrite(hiScoreDate,    sizeof(time_t), 10*HI_SCORE_HIST, file);
		fclose(file);
	}
	else
	{
		fprintf(stderr, "WARNING: could not write score file (%s)\n", configFilename);
		retVal = false;
	}
	return retVal;
}


/**
 * Read high score file. If CHROMIUM_SCORE environment variable is set, that
 * filename will be used. Otherwise, ~/.chromium-score.
 * @returns success
 */
//----------------------------------------------------------
bool HiScore::readFile()
{
	bool retVal = true;
	char	configFilename[256];
	FILE	*file;

	const char *envFile = getenv("CHROMIUM_SCORE");
	if(envFile && strlen(envFile) < 256)
	{
		fprintf(stderr, "CHROMIUM_SCORE=%s\n", envFile);
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
		
	file = fopen(configFilename, "r");
	if(file)
	{
		fread(hiScore,        sizeof(double), 10*HI_SCORE_HIST, file);
		fread(hiScoreName, 64*sizeof(char),   10*HI_SCORE_HIST, file);
		fread(hiScoreDate,    sizeof(time_t), 10*HI_SCORE_HIST, file);
		fclose(file);
	}
	else 
	{
		fprintf(stderr, "WARNING: could not read score file (%s)\n", configFilename);
		retVal = false;
	}
		
	return retVal;
}


//----------------------------------------------------------
void HiScore::insertScore(int level, int rank, float score)
{
	int i;
	i = HI_SCORE_HIST-2;
	while(i >= rank)
	{
		HiScore::hiScore[level][i+1] = HiScore::hiScore[level][i];
		strcpy(HiScore::hiScoreName[level][i+1], HiScore::hiScoreName[level][i]);
		memcpy(&(HiScore::hiScoreDate[level][i+1]), &(HiScore::hiScoreDate[level][i]), sizeof(time_t));
		i--;
	}
	HiScore::hiScore[level][rank] = score;
	char *name = getenv("USER");
	if(name)
		strcpy(HiScore::hiScoreName[level][rank], name);
	else
		strcpy(HiScore::hiScoreName[level][rank], "player");
	time(&HiScore::hiScoreDate[level][rank]);
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
			if(score > HiScore::hiScore[skill][i])
				rank = i;
		}
		if(rank > -1)
		{
			insertScore(skill, rank, score);
			retVal = rank+1;
		}
		saveFile();
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
	fprintf(stderr, "high scores:\n");
	for(int j = 0; j < HI_SCORE_HIST; j++)
	{
		tmptr = localtime(&HiScore::hiScoreDate[skill][j]);
		fprintf(stderr, "%02d/%02d/%04d %16s %d\n", 1+tmptr->tm_mon, tmptr->tm_mday, 1900+tmptr->tm_year,
				HiScore::hiScoreName[skill][j], (int)(HiScore::hiScore[skill][j]));
	}
}
