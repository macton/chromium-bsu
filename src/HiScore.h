/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef HiScore_h
#define HiScore_h

#include <stdio.h>
#include <time.h>

#include "define.h"

//====================================================================
class HiScore
{
public:
	~HiScore();
	
	static HiScore	*init();
	static HiScore	*getInstance();
	static void		destroy();
	
	bool		readFile();
	bool		saveFile();
	int			set();
	int			check();
	void		print();

	
private:
	void	insertScore(int level, int rank, float score);

private:
	static HiScore	*instance;
	
	double	hiScore[10][HI_SCORE_HIST];
	char	hiScoreName[10][HI_SCORE_HIST][64];
	time_t	hiScoreDate[10][HI_SCORE_HIST];

	HiScore();
};

#endif // HiScore_h
