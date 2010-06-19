/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#include "PlayList.h"

#include <cstdio>
#include <cstdlib>

#include <qapp.h>
#include <qtooltip.h>
#include <qmessagebox.h>

//====================================================================
PlayList::PlayList(QWidget *parent, const char* name)
	: QWidget(parent, name)
{
	int i;
	altered = false;
	
	chromLabel = new QLabel(this);
	chromLabel->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	chromLabel->setText("c h r o m i u m   b . s . u .   p l a y   l i s t  ");
	chromLabel->setAlignment(AlignCenter);
	chromLabel->setBackgroundMode(PaletteMidlight);
	
	audio = new Audio();
	
	but[Add] = new QPushButton(this);
	but[Add]->setText("&Add");
	
	but[Remove] = new QPushButton(this);
	but[Remove]->setText("&Remove");
	
	but[Up] = new QPushButton(this);
	but[Up]->setText("&Up");
	
	but[Down] = new QPushButton(this);
	but[Down]->setText("&Down");
	
	but[Play] = new QPushButton(this);
	but[Play]->setText("&Play");
	
	but[Stop] = new QPushButton(this);
	but[Stop]->setText("S&top");

	but[OpenAL] = new QPushButton(this);
	but[OpenAL]->setText("OpenAL");

	listBox = new QListBox(this);

	fileDialog = new QFileDialog(this, 0, true);
	fileDialog->hide();
	fileDialog->setShowHiddenFiles(false); 
	QString     filterString;
	QStringList filterList;
	filterString += "OpenAL supported files (";
	filterString += audio->formatExtension(Audio::WAV); // WAV is always supported
	for(i = 1; i < Audio::NumAudioFormats; i++)
	{
		if(audio->hasFormat((Audio::AudioFormat)i))
		{
			filterString += ";";
			filterString += audio->formatExtension((Audio::AudioFormat)i);
		}
	}
	filterString += ")";
	filterList += filterString;
	for(i = 0; i < Audio::NumAudioFormats; i++)
	{
		if(audio->hasFormat((Audio::AudioFormat)i))
			filterList += audio->formatExtension((Audio::AudioFormat)i);
	}
	filterList += "all files (*)";
	fileDialog->setFilters(filterList);
		
	{
		QToolTip::add(but[Add],     "Add song to Play List");
		QToolTip::add(but[Remove],  "Remove selected song from Play List");
		QToolTip::add(but[Up],      "Move selected song UP in Play List");
		QToolTip::add(but[Down],    "Move selected song DOWN in Play List");
		QToolTip::add(but[Play],    "Play selected song");
		QToolTip::add(but[Stop],    "Stop audio playback");
		QToolTip::add(but[OpenAL],  "Check OpenAL information");
	}

	//-- Connect
	QObject::connect(but[Add],    SIGNAL(clicked()),	this, SLOT(addSongToList()) );
	QObject::connect(but[Remove], SIGNAL(clicked()),	this, SLOT(removeSongFromList()) );
	QObject::connect(but[Up],     SIGNAL(clicked()),	this, SLOT(moveCurrentUp()) );
	QObject::connect(but[Down],   SIGNAL(clicked()),	this, SLOT(moveCurrentDown()) );
	QObject::connect(but[OpenAL], SIGNAL(clicked()),	this, SLOT(showOpenALInfo()) );
	QObject::connect(but[Play],   SIGNAL(clicked()),	this, SLOT(play()) );
	QObject::connect(but[Stop],   SIGNAL(clicked()),	this, SLOT(stop()) );
	
	//-- Layout
	topVLayout	= new QVBoxLayout(this, 5, 5);
	hLayout		= new QHBoxLayout(5);
	butVLayout	= new QVBoxLayout(5);
	butHLayout	= new QHBoxLayout(5);
	listLayout	= new QVBoxLayout(5);
	
	topVLayout->addWidget(chromLabel, 0);
	topVLayout->addLayout(hLayout, 1);
		hLayout->addLayout(listLayout, 1);
			listLayout->addWidget(listBox);
			listLayout->addLayout(butHLayout, 0);
				butHLayout->addStretch(1);
				butHLayout->addWidget(but[Play]);
				butHLayout->addWidget(but[Stop]);
				butHLayout->addStretch(1);
		hLayout->addLayout(butVLayout, 0);
			butVLayout->addWidget(but[Add]);
			butVLayout->addWidget(but[Remove]);
			butVLayout->addStretch(1);		
			butVLayout->addWidget(but[Up]);
			butVLayout->addWidget(but[Down]);
			butVLayout->addStretch(2);		
			butVLayout->addWidget(but[OpenAL]);
	
	loadCurrentList();	
}

PlayList::~PlayList()
{
	stop(); // stop any audio
	delete audio;
}

//----------------------------------------------------------
void PlayList::startAudio()
{
	audio->initAudio();
}
	

//----------------------------------------------------------
void PlayList::stopAudio()
{
	audio->killAudio();
}

//----------------------------------------------------------
void PlayList::addSongToList()
{
	altered = true;
	QString currentFile = listBox->currentText();
	if(currentFile.isEmpty())
		currentFile = ".";
	fileDialog->setSelection(currentFile);
	fileDialog->exec();
	if(fileDialog->result() == QDialog::Accepted)
	{
		QString	fileName = fileDialog->selectedFile();
		if(!fileName.isEmpty())
		{
			listBox->insertItem(fileName);
			listBox->setCurrentItem(listBox->count()-1);
		}
	}
}

//----------------------------------------------------------
void PlayList::removeSongFromList()
{
	altered = true;
	if(listBox->currentItem() >= 0)
		listBox->removeItem(listBox->currentItem());
}

//----------------------------------------------------------
void PlayList::exit()
{
	if(altered)
		if(QMessageBox::warning(this, "", "Save current Play List \nbefore exiting?",
			"&Yes", "&No") == 0)
			saveCurrentList();
}
	
//----------------------------------------------------------
void PlayList::saveCurrentList()
{
	char	configFilename[256];
	const char *homeDir = getenv("HOME");
	if(!homeDir)
		homeDir = "./";
	sprintf(configFilename, "%s/.chromium-music", homeDir);
	saveCurrentListAs(configFilename);
}

//----------------------------------------------------------
void PlayList::saveCurrentListAs(const char *configFilename)
{
	int		i;
	FILE	*file;
	
	file = fopen(configFilename, "w");
	if(file)
	{
		for(i = 0; i < (int)listBox->count(); i++)
		{
			fprintf(file, "%s\n", (const char*)listBox->text(i));
		}
		fprintf(stderr, "wrote playList file (%s)\n", configFilename);
		fclose(file);
	}
	altered = false;
}
	
//----------------------------------------------------------
void PlayList::loadCurrentList()
{
	char	configFilename[256];
	const char *homeDir = getenv("HOME");
	if(!homeDir)
		homeDir = "./";
	sprintf(configFilename, "%s/.chromium-music", homeDir);
	loadCurrentListAs(configFilename);
	altered = false;
}

//----------------------------------------------------------
void PlayList::loadCurrentListAs(const char *configFilename)
{
	FILE	*file;
	char	buffer[1024];
	int		len;
	
	file = fopen(configFilename, "r");
	if(file)
	{
		listBox->clear();
		fprintf(stderr, "loading \"%s\"...", configFilename);
		while( fgets(buffer, 1024, file) )
		{
			len=strlen(buffer);
			buffer[len-1] = '\0'; // kill newline
			if( strlen(buffer) > 4) // we need at least 4 characters for name + . + extension
			{
				listBox->insertItem(buffer);
			}
		}
		fprintf(stderr, "done.\n");
		fclose(file);
	}
	listBox->setCurrentItem(0);
	altered = true;
}

//----------------------------------------------------------
void PlayList::moveCurrentUp()
{
	altered = true;
	int cur;
	QString	text;
	
	cur = listBox->currentItem();	
	if(cur > 0)
	{
		text = listBox->currentText();
		listBox->removeItem(cur);
		listBox->insertItem(text, cur-1);
		listBox->setCurrentItem(cur-1);
	}
}

//----------------------------------------------------------
void PlayList::moveCurrentDown()
{
	altered = true;
	int cur;
	QString	text;
	
	cur = listBox->currentItem();
	if( cur >= 0 && cur < (int)listBox->count()-1 )
	{
		text = listBox->currentText();
		listBox->removeItem(cur);
		listBox->insertItem(text, cur+1);
		listBox->setCurrentItem(cur+1);
	}
}

//----------------------------------------------------------
void PlayList::play()
{
	bool ok;
	QString filename = listBox->currentText();
	if(!filename.isEmpty())
	{
		fprintf(stderr, "play \"%s\"\n", (const char*)filename);
		ok = audio->playSound((const char*)filename);
		if(!ok)
		{
			QMessageBox::warning(this, "ERROR", audio->openALError());
		}
	}
}

//----------------------------------------------------------
void PlayList::stop()
{
	audio->stopSound();
}

//----------------------------------------------------------
void PlayList::showOpenALInfo()
{
	OpenALInfo openALInfo(this, audio);
	openALInfo.resize(400, 400);
	openALInfo.exec();
}

//====================================================================
OpenALInfo::OpenALInfo(QWidget *parent, Audio *audio)
	: QDialog(parent, 0, true)
{
	setCaption("OpenAL Information");
	
//	textView = new QTextView(this);
	textView = new QMultiLineEdit(this);
	textView->setFont(QFont("fixed"));
	if(audio)
		textView->setText(audio->openALInfo());
	
	suppLabel = new QLabel(this);
	suppLabel->setFrameStyle(QFrame::Box | QFrame::Sunken);
	QString suppText = "<b>Supported file types:</b><br>\n";
	if(audio)
	{
		suppLabel->setTextFormat(RichText);
		suppText += "<font face=fixed>";
		for(int i = 0; i < Audio::NumAudioFormats; i++)
		{
			if(audio->hasFormat((Audio::AudioFormat)i))
			{
				suppText += audio->formatExtension((Audio::AudioFormat)i);
				suppText += "<br>\n";
			}
		}
		suppText += "</font>";
	}
	suppLabel->setText(suppText);
	
	closeBut = new QPushButton(this);
	closeBut->setText("&Close");
	
	//-- Connect
	QObject::connect( closeBut, SIGNAL(clicked()), this, SLOT(accept()) );
	
	//-- Layout
	topVLayout	= new QVBoxLayout(this, 5, 5);
	butLayout	= new QHBoxLayout(5);
	
	topVLayout->addWidget(textView, 1);
	topVLayout->addWidget(suppLabel, 0);
	topVLayout->addLayout(butLayout, 0);
		butLayout->addStretch(1);
		butLayout->addWidget(closeBut, 0);
}

OpenALInfo::~OpenALInfo()
{
}


	
