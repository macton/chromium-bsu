/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MainWidget.h"

#include <stdio.h>
#include <stdlib.h>

#include <qapp.h>
#include <qfile.h>
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>

#include "../src/define.h"
#include "../src/Config.h"

extern const char* argv_0;

//====================================================================
MainWidget::MainWidget()
	: QMainWindow(0, 0)
{
	Config	*config = Config::instance();
	tabWidget = new QTabWidget(this);
	setCentralWidget(tabWidget);
	
	playList	= new PlayList(this);
	configFile	= new ConfigFile(this);
	hiScore		= new HiScoreWidget(this);
	
	tabWidget->addTab(configFile, "Config");
	tabWidget->addTab(playList,   "Music PlayList");
	tabWidget->addTab(hiScore,    "High Scores");
	tabWidget->setBackgroundMode(PaletteMid);
//	tabWidget->setBackgroundMode(PaletteButton);
	
	if(!config->usePlayList())
		tabWidget->setTabEnabled(playList, false);	
		
	helpPage = new HelpPage(this, 0, true);
	
	fileDialog = new QFileDialog(this);
	fileDialog->hide();
	fileDialog->setShowHiddenFiles(true);

	fileMenu = new QPopupMenu;
	fileMenu->insertItem("Save Config", configFile, SLOT( saveCurrentConfig()) );
	fileMenu->insertItem("Save PlayList", playList, SLOT( saveCurrentList()) );
	fileMenu->insertSeparator();
	fileMenu->insertItem("Load PlayList...", this, SLOT( loadPlayList()) );
	fileMenu->insertItem("Save PlayList As...", this, SLOT( savePlayListAs()) );
	fileMenu->insertSeparator();
	fileMenu->insertItem("E&xit", this, SLOT( exit()) );
	
	helpMenu = new QPopupMenu;
	helpMenu->insertItem("Game Information", this, SLOT( showHelpInfo()) );
	helpMenu->insertItem("F.A.Q. / Troubleshooting", this, SLOT( showHelpFAQ()) );
	
	menuBar()->insertItem ("&File", fileMenu);
	menuBar()->insertSeparator();
	menuBar()->insertItem ("&Help", helpMenu);
	
	//--Connect
	QObject::connect(configFile, SIGNAL(playListEnabled(bool)), this, SLOT(enablePlayList(bool)) );
	QObject::connect(configFile, SIGNAL(launchGame()), this, SLOT(launchGame()) );
	QObject::connect(configFile, SIGNAL(newSkillSelected(const QString &)), hiScore, SLOT(setLevelText(const QString &)) );

	configFile->setSkill(0);
}

MainWidget::~MainWidget()
{
}

//----------------------------------------------------------
void MainWidget::exit()
{
	configFile->exit();
	playList->exit();
	qApp->quit();
}

//----------------------------------------------------------
void MainWidget::savePlayListAs()
{
	const char *homeDir = getenv("HOME");
	QString fileName = fileDialog->getSaveFileName(homeDir, "*", this);
	if(!fileName.isEmpty())
		playList->saveCurrentListAs((const char*)fileName);
}

//----------------------------------------------------------
void MainWidget::loadPlayList()
{
	const char *homeDir = getenv("HOME");
	QString fileName = fileDialog->getOpenFileName(homeDir, "*", this);
	if(!fileName.isEmpty())
		playList->loadCurrentListAs((const char*)fileName);
}

//----------------------------------------------------------
void MainWidget::enablePlayList(bool status)
{
	tabWidget->setTabEnabled(playList, status);
}

//----------------------------------------------------------
void MainWidget::showHelpInfo()
{
	QFileInfo fileInfo(CHROMIUM_DATA_DIR"/doc/info.htm");
	
	if(fileInfo.isReadable())
	{
		//NOTE: Qt doesn't recognize html files if the path is relative!
		helpPage->setPageSource(fileInfo.absFilePath());
		helpPage->exec();
	}
	else
	{
		char buffer[128];
		sprintf(buffer, "Could not open help file\n\"%s\"", (const char*)fileInfo.absFilePath());
		QMessageBox::warning(this, "WARNING", buffer);
	}
}

//----------------------------------------------------------
void MainWidget::showHelpFAQ()
{
	QFileInfo fileInfo(CHROMIUM_DATA_DIR"/doc/faq.htm");
	
	if(fileInfo.isReadable())
	{	
		//NOTE: Qt doesn't recognize html files if the path is relative!
		helpPage->setPageSource(fileInfo.absFilePath());
		helpPage->exec();
	}
	else
	{
		char buffer[128];
		sprintf(buffer, "Could not open help file\n\"%s\"", (const char*)fileInfo.absFilePath());
		QMessageBox::warning(this, "WARNING", buffer);
	}
}

//----------------------------------------------------------
void MainWidget::launchGame()
{
	char	buffer[256];
	FILE	*inpipe;
	QFileInfo	fileInfo(CHROMIUM_EXEC);
	
	if(!fileInfo.isExecutable()) // if file isn't there, let's look in the same directory as this one...
	{
		int i;
		QString execName = argv_0;
		i = execName.findRev('/');
		if(i > 0)
			execName.truncate(i+1);
		execName += "chromium";
		fileInfo.setFile(execName);
		fprintf(stderr, "\nWARNING: could not find default exec (%s), trying \"%s\"\n\n", 
			CHROMIUM_EXEC, (const char*)fileInfo.filePath());
	}
	
	if(fileInfo.isExecutable())
	{
		playList->stop();
		playList->stopAudio();
		playList->saveCurrentList();
		configFile->saveCurrentConfig();

		inpipe = popen((const char*)fileInfo.filePath(), "r");
		if(inpipe)
		{
			while( fgets(buffer, 256, inpipe) )
			{
				fprintf(stderr, "%s", buffer);
			}
			pclose(inpipe);
			configFile->loadCurrentConfig();
			hiScore->updateScores();
		}
		playList->startAudio();
	}
	else
	{
		char buffer[128];
		sprintf(buffer, "Error launching Chromium B.S.U.\nExecutable does not exist!\n(Tried \"%s\" and \"%s\")", 
			CHROMIUM_EXEC, (const char*)fileInfo.filePath());
		QMessageBox::critical(this, "ERROR", buffer);	
	}
}

