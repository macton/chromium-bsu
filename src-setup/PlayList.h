/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef PlayList_h
#define PlayList_h

#include <qlabel.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qtextview.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>

#include "Audio.h"

//====================================================================
class PlayList : public QWidget
{
	Q_OBJECT
public:
	PlayList(QWidget *parent = 0, const char *name = 0);
	~PlayList();

	void startAudio();
	void stopAudio();
	
public slots:
	void play();
	void stop();
	void exit();
	
	void saveCurrentList();
	void loadCurrentList();
	void saveCurrentListAs(const char *filename);
	void loadCurrentListAs(const char *filename);
	
protected slots:
	void addSongToList();
	void removeSongFromList();
	
	void moveCurrentUp();
	void moveCurrentDown();

	void showOpenALInfo();
	
private:
	QVBoxLayout	*topVLayout;
	QHBoxLayout	*hLayout;
	QVBoxLayout	*listLayout;
	QVBoxLayout	*butVLayout;
	QHBoxLayout	*butHLayout;
	
	QLabel		*chromLabel;
	
	QListBox	*listBox;
	
	QFileDialog	*fileDialog;
	
	enum  ButtonEnum { Add, Remove, Up, Down, Play, Stop, OpenAL, NumButtons };
	QPushButton	*but[NumButtons];
	
	Audio	*audio;
	
	bool altered;
};
	
//====================================================================
class OpenALInfo : public QDialog
{
	Q_OBJECT
public:
	OpenALInfo(QWidget *parent, Audio *audio);
	~OpenALInfo();

private:
	QVBoxLayout	*topVLayout;
	QHBoxLayout	*butLayout;
	
	QLabel		*suppLabel;
	
//	QTextView	*textView;
	QMultiLineEdit	*textView;
	QPushButton	*closeBut;
};


#endif // PlayList_h
