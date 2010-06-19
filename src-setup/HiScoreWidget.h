/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef HiScoreWidget_h
#define HiScoreWidget_h

#include <ctime>

#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlistview.h>

//====================================================================
class HiScoreWidget : public QWidget
{
	Q_OBJECT
public:
	HiScoreWidget(QWidget *parent = 0, const char *name = 0);
	~HiScoreWidget();

	void	updateScores();
	void	show();

public slots:
	void	setLevelText(const QString &s);
	
protected:
	bool	readScoreFile();
	void	checkFilePermissions();

private:
	QVBoxLayout	*topVLayout;
	
	QLabel		*chromLabel;
	QLabel		*levelLabel;
	QLabel		*envLabel;
	
	QListView	*listView;
};
	

#endif // HiScoreWidget_h
