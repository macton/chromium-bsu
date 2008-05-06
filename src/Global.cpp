/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Global.h"
#include "extern.h"
#include "HiScore.h"
#include "Config.h"

TexFont	*Global::texFont	= 0;

MainToolkit *Global::toolkit = 0;

bool	Global::mouseActive		= false;

float	Global::fps			= 50.0;
int		Global::frame		= 0;
int		Global::gameFrame	= 0;
float	Global::gameSpeed	= 0.5;
float	Global::gameSkill	= 1.0;
int		Global::gameLevel	= 1;
float	Global::speedAdj	= 1.0;
Global::GameMode Global::gameMode = Menu;

int		Global::heroDeath	= 0;
int		Global::heroSuccess	= 0;

float	Global::scrollSpeed		= -0.045;

HeroAircraft	*Global::hero		= 0;
EnemyFleet		*Global::enemyFleet	= 0;
HeroAmmo		*Global::heroAmmo	= 0;
EnemyAmmo		*Global::enemyAmmo	= 0;
ScreenItemAdd	*Global::itemAdd	= 0;
Explosions		*Global::explosions	= 0;
PowerUps		*Global::powerUps	= 0;
Audio			*Global::audio		= 0;
Ground			*Global::ground		= 0;
Ground			*Global::groundGame	= 0;
Ground			*Global::groundMenu	= 0;
MenuGL			*Global::menu		= 0;
MainGL			*Global::mainGL		= 0;
StatusDisplay	*Global::statusDisplay = 0;

float	Global::cursorPos[3] = { 0.0, 0.0, HERO_Z };

FILE	*Global::eventFile = 0;

int Global::tipShipPast		= 0;
int Global::tipSuperShield	= 0;

//====================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Audio.h"
#include "Ground.h"
#include "MenuGL.h"
#include "MainGL.h"
#include "PowerUps.h"
#include "HeroAmmo.h"
#include "EnemyAmmo.h"
#include "EnemyFleet.h"
#include "Explosions.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"
#include "StatusDisplay.h"

#include "GroundMetal.h"

#if defined(AUDIO_OPENAL)
#include "AudioOpenAL.h"
#endif
#if defined(AUDIO_SDLMIXER)
#include "AudioSDLMixer.h"
#endif

Global	*Global::instance = 0;

//----------------------------------------------------------
Global::Global()
{
	game_pause		= false;
	game_quit		= false;

//	readConfigFile();
}

Global::~Global()
{
//	saveConfigFile();
}

//----------------------------------------------------------
Global *Global::init()
{
	if(!Global::instance)
	{
		Global::instance = new Global;
	}
	else
	{
		fprintf(stderr, "WARNING: Global::init() has already been called.\n");
	}
	return Global::instance;
}

//----------------------------------------------------------
Global *Global::getInstance()
{
	if(!instance)
	{
		return Global::init();
	}
	else
		return Global::instance;
}

//----------------------------------------------------------
void Global::destroy()
{
	delete Global::instance;
	Global::instance = 0;
}

//----------------------------------------------------------
void Global::newGame()
{
	Config *config = Config::instance();
	HiScore::getInstance()->set(config->intSkill(), hero->getScore());
	gameSkill = config->gameSkillBase() + 0.5;
	gameSkill += (gameLevel-1)*0.05;
	gameFrame = 0;
	enemyFleet->clear();
	powerUps->clear();
	enemyAmmo->clear();
	heroAmmo->clear();
	itemAdd->clear();
	explosions->clear();
	hero->newGame();
	hero->setLives(4);
	game_pause = false;
	
//	Global::generateRandom();
	itemAdd->loadScreenItems("");
	if(groundGame != groundMenu) 
	{
		//delete game ground and set to menu/game common ground
		delete groundGame;
		groundGame = groundMenu;
	}
	ground = groundGame;
	ground->setVariation(gameLevel-1);
	if(eventFile)
		fclose(eventFile);
	
	audio->setMusicIndex(gameLevel-1);
//	//-- cheezy, partially functional record mechanism
//	Global::eventFile = fopen("EVENT_FILE.txt", "w");
//	Global::eventFile = fopen("EVENT_FILE.txt", "r");

//	Global::scrollSpeed = -Global::gameSkill*0.05;
//	fprintf(stderr, "%f\n", Global::scrollSpeed);
}

//----------------------------------------------------------
void Global::gotoNextLevel()
{
	Config *config = Config::instance();
	gameLevel++;
	if(config->maxLevel() < gameLevel)
		config->setMaxLevel(gameLevel);
	gameSkill += 0.05;
	if(gameSkill > 1.9)
		gameSkill = 1.9;
	gameFrame = 0;
	enemyFleet->clear();
	powerUps->clear();
	enemyAmmo->clear();
	heroAmmo->clear();
	itemAdd->clear();
	hero->fullHealth();
	
//	generateRandom();
	itemAdd->loadScreenItems("");
	
	// when more than one ground is used, check here if it 
	// need to be created.
	ground->nextVariation();
	
	audio->nextMusicIndex();
}

//----------------------------------------------------------
void Global::createGame()
{
//	fprintf(stderr, "begin startup...");
	mainGL		= new MainGL();
	explosions	= new Explosions();
	enemyFleet	= new EnemyFleet();
	hero		= new HeroAircraft();
	heroAmmo	= new HeroAmmo();
	enemyAmmo	= new EnemyAmmo();
	statusDisplay = new StatusDisplay();
	powerUps	= new PowerUps();
	ground		= new GroundMetal();
	menu		= new MenuGL();
	itemAdd		= new ScreenItemAdd();

#if defined(AUDIO_OPENAL) && defined(AUDIO_SDLMIXER) 
	Config *config = Config::instance();
	if(config->audioType() == Config::AudioOpenAL)
		audio = new AudioOpenAL();
	else
		audio = new AudioSDLMixer();
#elif defined(AUDIO_OPENAL)
	audio		= new AudioOpenAL();
#elif defined(AUDIO_SDLMIXER)
	audio		= new AudioSDLMixer();
#else
	audio		= new Audio();
#endif

	groundGame	= groundMenu = ground;

	newGame();
	
	audio->setMusicMode(Audio::MusicMenu);
	fprintf(stderr, "...startup complete.\n");
}

//----------------------------------------------------------
void Global::deleteGame()
{
//	fprintf(stderr, "begin shutdown...\n");
	delete mainGL;
	delete enemyFleet;
	delete hero;
	delete heroAmmo;
	delete enemyAmmo;
	delete statusDisplay;
	delete explosions;
	delete powerUps;
	delete ground;
	delete menu;
	delete itemAdd;
	delete audio;
//	fprintf(stderr, "...shutdown complete.\n");
}

//----------------------------------------------------------
void Global::deleteTextures()
{
	fprintf(stderr, "deleteTextures()\n");
//	return;
	glFinish();
	mainGL->deleteTextures();
	enemyAmmo->deleteTextures();
	enemyFleet->deleteTextures();
	explosions->deleteTextures();
	hero->deleteTextures();
	heroAmmo->deleteTextures();
	ground->deleteTextures();
	menu->deleteTextures();
	powerUps->deleteTextures();
	statusDisplay->deleteTextures();
	glFinish();
}

//----------------------------------------------------------
void Global::loadTextures()
{
	fprintf(stderr, "Global::loadTextures()\n");
//	return;
	glFinish();
	mainGL->loadTextures();
	enemyAmmo->loadTextures();
	enemyFleet->loadTextures();
	explosions->loadTextures();
	hero->loadTextures();
	heroAmmo->loadTextures();
	ground->loadTextures();
	menu->loadTextures();
	powerUps->loadTextures();
	statusDisplay->loadTextures();
	glFinish();
}

//====================================================================
static int	randData[256] = {
	 248102004,  328117286,  993377775,  854015073, 1439374375, 1767159396,   70069201, 1907231987, 
	 211119157,  479401751,  622212282,    9231838,  734458600, 1262181023, 1588506079, 1045482140, 
	1339687125, 1560344577, 1213849032,  633406194,  614125827, 1554327504, 1806655959,    9023993, 
	1078727242, 1176063139,  976187378,  810931292,  746270205, 1562136994, 2087884076,  424129806, 
	1193958768, 1095781440,  983612896,  149343863,  360133739, 1686051724, 1739362510,  731185254, 
	1288405356, 1885751450,  384971725, 1815492077,  997042586, 1665612979,  571718101, 1202528724, 
	 823422130,  648247351, 1310410582, 1276554002, 2101005116, 1295452586, 1016404054,  195103114, 
	   7612905, 1430120805,  978590810, 1546776775,  401848397, 1779173400, 1807914296, 1682029851, 
	 349817859,  942374862, 1461561077,  120026595,  751306787,  799796867,  996682091,  482884166, 
	1144154542, 1217981297,  372372860, 1751855466,  579591149, 1722265308, 2087439369,  351051816, 
	1456317295,  271195445,  219723916, 1255424177,  378781518,  753683640,  427750009, 1556077179, 
	1273818659,  559565258,  960093753,  729768891, 1647764074, 1227026302, 1149585425, 1765259304, 
	1868939152,  774702399,  750059288,  398832958,  520820276, 2112703334, 1362399002, 1561545469, 
	1780640386,  244572965,  852682320, 2037075953, 1225051875,  504049456,  225120512, 1844582338, 
	1338663083, 1074570311,  160244553, 1121950973,  418405917,  624843041, 1149855877, 1119779436, 
	1094178976, 2033148251,  371488856, 2051932162, 1859053526,  942438374, 1067346010, 1955231938, 
	  85831261,  504170563, 1461599154, 1331453662,   78953775, 1213982141, 2003116935, 2079514895, 
	1687748987, 1680501530, 1947364831, 1565907115, 1040177840,  595297516, 2075848483,  378042878, 
	 871418310, 1568735684, 1444462324,  917004149,  611204198, 1676918350,  938958743, 1638988003, 
	1515187085,  939044846, 1962493447,    2135470,  295376586, 1762079157, 2062507824, 1282493848, 
	2073691441, 1602732461, 1201070569,  947659542, 2056481520,   88202495, 1417304658, 1289086053, 
	1384048210, 1502072312, 1061581827, 1102754594,   73404482,   49379974, 1408723948,  727887420, 
	1723089720, 1543825878, 1107100043, 1024116412,  824309689, 1446638616,  235123901,  852436277, 
	2090136943, 1615752050, 1578900715,  127475878, 1171040344,  329650992, 1033285319, 1060667470, 
	2014529907,  800975903,  515148648, 1066046014,  394386848, 2064959457,  924398949,  504237184, 
	 159062826,  950457702, 1833692758,  734688828, 1997793182,  589278825, 1532000472,  106596900, 
	   4377635,  184959073, 1672457626,  996215687, 1750355520, 2022517378,  517653891, 1473385649, 
	  63013760,   12922586, 1450588006, 1252083068, 1894542412,  116284061, 1272942781,  936597537, 
	2055707879,  850455160,  534369042, 1501836375, 1522849318, 1521806822, 2002946071,  977846244, 
	1544448690, 1241478873,  803625621,   81198469, 1810959889, 1082459409,  149777578, 1164775679, 
	  17445490,  527728284, 1544356102,  522717168, 1632812509, 1097328719,  389651448, 1720278104, 
	 807163977, 1248380721,    1044108, 1107180198,  388105756, 1264247211,  377234805, 1930260065
};

int		Global::randI[256];
float	Global::randF[256];
float	Global::randS[256];
int		Global::randIndex = 0;
//----------------------------------------------------------
void Global::generateRandom(bool r)
{
	Global::randIndex = 0;
	if(r)
	{
		fprintf(stderr, "randomizing.\n");
		for(int i = 0; i < 256; i++)
		{
			randI[i] = rand();
			randS[i] = (2.0*(0.5-(rand()/(double)RAND_MAX)));
			randF[i] = (rand()/(double)RAND_MAX);
		}
	}
	else
	{
		for(int i = 0; i < 256; i++)
		{
			randI[i] = randData[i];
			randS[i] = (2.0*(0.5-(randData[i]/(double)RAND_MAX)));
			randF[i] = (randData[i]/(double)RAND_MAX);
		}
	}
//	fprintf(stdout, "int	Global::irand[256] = {");
//	for(i = 0; i < 256; i++)
//	{
//		if( !(i%8) )
//			fprintf(stdout, "\n\t");
//		fprintf(stdout, "%10d, ", irand[i]);
//	}
//	fprintf(stdout, "\n};\n\n");
}

