/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef HiScore_h
#define HiScore_h

#include <time.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlistview.h>

//====================================================================
class HiScore : public QWidget
{
	Q_OBJECT
public:
	HiScore(QWidget *parent = 0, const char *name = 0);
	~HiScore();

	void	updateScores();
	void	show();

public slots:
	void	setLevelText(const QString &s);
	
protected:
	bool	readScoreFile();
	void	checkFilePermissions();
	const char *getFileName();

private:
	QVBoxLayout	*topVLayout;
	
	QLabel		*chromLabel;
	QLabel		*levelLabel;
	QLabel		*envLabel;
	
	QListView	*listView;
};
	

#endif // HiScore_h
