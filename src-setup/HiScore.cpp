#include "HiScore.h"

#include <stdio.h>
#include <stdlib.h>

#include <qfileinfo.h>

#include "../src/define.h"
#include "Global.h"

//====================================================================
HiScore::HiScore(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	chromLabel = new QLabel(this);
	chromLabel->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	chromLabel->setText("c h r o m i u m   b . s . u .   h i g h   s c o r e s");
	chromLabel->setAlignment(AlignCenter);
	chromLabel->setBackgroundMode(PaletteMidlight);
	
	levelLabel = new QLabel(this);
//	levelLabel->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	levelLabel->setText("high scores for -- skill level : ");
	levelLabel->setAlignment(AlignLeft | AlignBottom);
//	levelLabel->setBackgroundMode(PaletteMidlight);
	
	envLabel = new QLabel(this);
	envLabel->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	envLabel->setText("");
	envLabel->setAlignment(AlignLeft | AlignVCenter);
	envLabel->setBackgroundMode(PaletteMidlight);
	
	listView = new QListView(this);
	listView->addColumn("Date");
	listView->addColumn("Player");
	listView->addColumn("Score");
	listView->setColumnAlignment(0, AlignCenter);
	listView->setColumnAlignment(1, AlignCenter);
	listView->setColumnAlignment(2, AlignRight | AlignVCenter);
	listView->setColumnWidthMode(0, QListView::Manual);
	listView->setColumnWidthMode(1, QListView::Manual);
	listView->setColumnWidthMode(2, QListView::Manual);
	listView->setColumnWidth(0, 150);
	listView->setColumnWidth(1, 150);
	listView->setColumnWidth(2, 100);
	listView->setSorting(-1);
	
	
	//-- Layout --------------
	topVLayout	= new QVBoxLayout(this, 5, 5);
	
	topVLayout->addWidget(chromLabel, 0);
	topVLayout->addSpacing(5);
	topVLayout->addWidget(levelLabel, 0);
	topVLayout->addWidget(listView, 1);
	topVLayout->addWidget(envLabel, 0);
	
	updateScores();
}

HiScore::~HiScore()
{
}

//----------------------------------------------------------
void HiScore::setLevelText(const QString &s)
{
	char buffer[128];
	sprintf(buffer, "high scores for \"%s\" skill level : ", (const char*)s);
	levelLabel->setText(buffer);
	updateScores();
}

//----------------------------------------------------------
void HiScore::show()
{
//	updateScores();
	QWidget::show();
}

//----------------------------------------------------------
void HiScore::updateScores()
{
	QListViewItem *item;
	
	listView->clear();
	if(readScoreFile())
	{
		char sDate[32];
		char sName[64];
		char sScore[32];
		struct tm *tmptr; 
		int l = INT_GAME_SKILL_BASE;
		for(int j = HI_SCORE_HIST-1; j >= 0; j--)
		{
			tmptr = localtime(&Global::hiScoreDate[l][j]);
			sprintf(sDate,  "%02d/%02d/%04d", 1+tmptr->tm_mon, tmptr->tm_mday, 1900+tmptr->tm_year);
			sprintf(sName,  "%s", Global::hiScoreName[l][j]);
			sprintf(sScore, "%d", (int)(Global::hiScore[l][j]));
			item = new QListViewItem(listView, sDate, sName, sScore);
		}
	}
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
const char *HiScore::getFileName()
{
	static char	configFilename[256];
	const char *envFile = getenv("CHROMIUM_SCORE");
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

//----------------------------------------------------------
bool HiScore::readScoreFile()
{
	bool retVal = true;
	FILE	*file;
		
	file = fopen(getFileName(), "r");
	if(file)
	{
		fread(Global::hiScore,        sizeof(double), 10*HI_SCORE_HIST, file);
		fread(Global::hiScoreName, 64*sizeof(char),   10*HI_SCORE_HIST, file);
		fread(Global::hiScoreDate,    sizeof(time_t), 10*HI_SCORE_HIST, file);
		fclose(file);
	}
	else 
		retVal = false;
	
	checkFilePermissions();
	
	return retVal;
}

//----------------------------------------------------------
void HiScore::checkFilePermissions()
{
	char		filename[256];
	QFileInfo	fileInfo;
	QString		tmpString;
	
	sprintf(filename, "%s", getFileName());
	const char *envFile = getenv("CHROMIUM_SCORE");
	if(envFile)
	{
		tmpString.sprintf("CHROMIUM_SCORE environment variable is set to:\n\n\"%s\"\n", filename);
	}
	else
	{
		tmpString.sprintf("CHROMIUM_SCORE environment variable is not set. Using: \n\n\"%s\"\n", filename);
	}
	
	fileInfo.setFile(filename);
	if(fileInfo.exists())
	{
		if(fileInfo.isReadable())
			tmpString += "\nfile is readable.";
		else
			tmpString += "\nWARNING: file is NOT readable!";
			
		if(fileInfo.isWritable())
			tmpString += "\nfile is writable.";
		else
			tmpString += "\nWARNING: you do not have permission to write to this file!";
	}
	else
	{
		tmpString += "\nfile does not exist.";
	}
	
	envLabel->setText(tmpString);
}


