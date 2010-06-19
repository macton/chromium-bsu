/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef MainWidget_h
#define MainWidget_h

#include <qmainwindow.h>
#include <qtabwidget.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>

#include "HelpPage.h"
#include "PlayList.h"
#include "ConfigFile.h"
#include "HiScoreWidget.h"

//====================================================================
class MainWidget : public QMainWindow
{
	Q_OBJECT
public:
	MainWidget();
	~MainWidget();
		
protected slots:
	void	exit();
	void	loadPlayList();
	void	savePlayListAs();
	void	enablePlayList(bool);
	
	void	showHelpInfo();
	void	showHelpFAQ();
	
	void	launchGame();

private:
	QPopupMenu	*fileMenu;
	QPopupMenu	*helpMenu;
	
	QTabWidget	*tabWidget;
	
	PlayList	*playList;
	ConfigFile	*configFile;
	HiScoreWidget		*hiScore;
	
	HelpPage	*helpPage;

	QFileDialog	*fileDialog;
};

#endif // MainWidget_h
