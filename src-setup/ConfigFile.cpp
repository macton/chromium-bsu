/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "ConfigFile.h"

#include <stdio.h>
#include <stdlib.h>

#include <qapp.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtooltip.h>

#include "Global.h"

static bool altered = false;

//====================================================================
ConfigFile::ConfigFile(QWidget *parent, const char* name)
	: QWidget(parent, name)
{
	int i;
	
	chromLabel = new QLabel(this);
	chromLabel->setFrameStyle( QFrame::Sunken | QFrame::Panel );
	chromLabel->setText("c h r o m i u m   b . s . u .   c o n f i g u r a t i o n");
	chromLabel->setAlignment(AlignCenter);
	chromLabel->setBackgroundMode(PaletteMidlight);
	
	for(i = 0; i < (int)NumSelectOptions; i++)
	{
		configSelect[i] = new ConfigSelect(this);
	}
	for(i = 0; i < (int)NumFloatOptions; i++)
	{
		configFloat[i] = new ConfigFloat(this);
	}
	for(i = 0; i < (int)NumBoolOptions; i++)
	{
		configBool[i] = new ConfigBool(this);
	}
	
	//----
	configSelect[screenSize]->setText("Screen Size");
	configSelect[screenSize]->insertItem(" 512x384");
	configSelect[screenSize]->insertItem(" 640x480");
	configSelect[screenSize]->insertItem(" 800x600");
	configSelect[screenSize]->insertItem("1024x768");
	configSelect[screenSize]->insertItem("1280x960");
	
	configSelect[gfxLevel]->setText("Graphics Level");
	configSelect[gfxLevel]->insertItem("low");
	configSelect[gfxLevel]->insertItem("medium");
	configSelect[gfxLevel]->insertItem("high");
	
	configSelect[Skill]->setText("Skill Level");
	configSelect[Skill]->insertItem("fish in a barrel");
	configSelect[Skill]->insertItem("whimp");
	configSelect[Skill]->insertItem("easy");
	configSelect[Skill]->insertItem("normal");
	configSelect[Skill]->insertItem("experienced");
	configSelect[Skill]->insertItem("fun");
	configSelect[Skill]->insertItem("insane");
	configSelect[Skill]->insertItem("impossible");
	
	//----
	configFloat[mouseSpeed]->setText("Mouse Speed");
	configFloat[mouseSpeed]->setRange(0.0, 0.1, 0.05);
	configFloat[volSound]->setText("Sound Volume");
	configFloat[volSound]->setRange(0.0, 1.0);
	configFloat[volMusic]->setText("Music Volume");
	configFloat[volMusic]->setRange(0.0, 1.0);
	configFloat[viewGamma]->setText("Viewing Gamma");
	configFloat[viewGamma]->setRange(0.5, 2.0);
	
	//----
	configBool[full_screen]->setText("Full Screen");
	configBool[true_color]->setText("True Color");
	configBool[swap_stereo]->setText("Swap Stereo");
	configBool[auto_speed]->setText("Auto Speed");
	configBool[show_fps]->setText("Show FPS");
	configBool[use_playList]->setText("Enable PlayList");
	configBool[use_cdrom]->setText("Enable CDROM");
	
	launchBut = new QPushButton(this);
	launchBut->setText("\nlaunch\nc h r o m i u m   b . s . u\n");
	
	//-- Connext
	QObject::connect(configSelect[Skill], SIGNAL(activated(int)), this, SLOT(setSkill(int)) );
	QObject::connect(launchBut, SIGNAL(clicked()), this, SIGNAL(launchGame()) );
	QObject::connect(configBool[use_playList], SIGNAL(toggled(bool)), this, SIGNAL(playListEnabled(bool)) );
	
	//-- Layout
	topVLayout	= new QVBoxLayout(this, 5, 5);
	hLayout		= new QHBoxLayout(50);
	v1Layout	= new QVBoxLayout(5);
	v2Layout	= new QVBoxLayout(5);
	
	topVLayout->addWidget(chromLabel, 0);
	topVLayout->addLayout(hLayout, 1);
		hLayout->addLayout(v1Layout, 3);
			for(i = 0; i < (int)NumSelectOptions; i++)
				v1Layout->addWidget(configSelect[i], 0);
			v1Layout->addSpacing(20);	
			v1Layout->addStretch(1);
			for(i = 0; i < (int)NumFloatOptions; i++)
				v1Layout->addWidget(configFloat[i], 0);
		hLayout->addLayout(v2Layout, 2);
			for(i = 0; i < (int)NumBoolOptions; i++)
				v2Layout->addWidget(configBool[i], 0);
			v2Layout->addStretch(1);
			v2Layout->addWidget(launchBut, 0);
	topVLayout->addSpacing(5);
	
	
	
	loadCurrentConfig();
	setupToolTips();
}

ConfigFile::~ConfigFile()
{
}

//----------------------------------------------------------
void ConfigFile::exit()
{
	if(altered)
		if(QMessageBox::warning(this, "", "Save current <B>Config</B> \nbefore exiting?",
			"&Yes", "&No") == 0)
			saveCurrentConfig();
}
	
//----------------------------------------------------------
void ConfigFile::saveCurrentConfig()
{
	char	configFilename[256];
	FILE	*file;
	const char *homeDir = getenv("HOME");

	if(!homeDir)
		homeDir = "./";
		
	sprintf(configFilename, "%s/.chromium", homeDir);
	file = fopen(configFilename, "w");
	if(file)
	{
		updateVars();
		fprintf(file, "screenSize %d\n",	Global::screenSize);
		fprintf(file, "gfxLevel %d\n",		Global::gfxLevel);
		fprintf(file, "gameSkillBase %g\n",	Global::gameSkillBase);
		fprintf(file, "mouseSpeed %g\n",	Global::mouseSpeed);
		fprintf(file, "maxLevel %d\n",		Global::maxLevel);
		fprintf(file, "volSound %g\n",		Global::volSound);
		fprintf(file, "volMusic %g\n",		Global::volMusic);
		fprintf(file, "full_screen %d\n", 	(int)Global::full_screen);
		fprintf(file, "true_color %d\n", 	(int)Global::true_color);
		fprintf(file, "swap_stereo %d\n",	(int)Global::swap_stereo);
		fprintf(file, "auto_speed %d\n",	(int)Global::auto_speed);
		fprintf(file, "show_fps %d\n",		(int)Global::show_fps);
		fprintf(file, "use_playList %d\n",	(int)Global::use_playList);
		fprintf(file, "use_cdrom %d\n",		(int)Global::use_cdrom);
		fprintf(file, "viewGamma %g\n",		Global::viewGamma);

		fclose(file);
		fprintf(stderr, "wrote config file (%s)\n", configFilename);
		altered = false;
	}
	else
		fprintf(stderr, "WARNING: could not write config file (%s)\n", configFilename);
}
	
//----------------------------------------------------------
void ConfigFile::loadCurrentConfig()
{
	int		i;
	int		numLines;
	int		tmp;
	FILE	*file;
	char	configStrings[32][64];
	char	configFilename[256];
	const char *homeDir = getenv("HOME");
	if(!homeDir)
		homeDir = "./";
	sprintf(configFilename, "%s/.chromium", homeDir);
	file = fopen(configFilename, "r");
	if(file)
	{
		fprintf(stderr, "loading \"%s\"...", configFilename);
		i = numLines = 0;
		while( fgets(configStrings[i], 64, file) )
			i++;
		fprintf(stderr, "done.\n");
		fclose(file);
		
		numLines = i;
		for(i = 0; i < numLines; i++)
		{
			if(strncmp(configStrings[i], "screenSi", 8) == 0) { sscanf(configStrings[i], "screenSize %d\n", &Global::screenSize); }
			if(strncmp(configStrings[i], "mouseSpe", 8) == 0) { sscanf(configStrings[i], "mouseSpeed %f\n", &Global::mouseSpeed); }
			if(strncmp(configStrings[i], "gameSkil", 8) == 0) { sscanf(configStrings[i], "gameSkillBase %f\n", &Global::gameSkillBase); }
			if(strncmp(configStrings[i], "gfxLevel", 8) == 0) { sscanf(configStrings[i], "gfxLevel %d\n", &Global::gfxLevel);   }
			if(strncmp(configStrings[i], "volSound", 8) == 0) { sscanf(configStrings[i], "volSound %f\n", &Global::volSound);   }
			if(strncmp(configStrings[i], "volMusic", 8) == 0) { sscanf(configStrings[i], "volMusic %f\n", &Global::volMusic);   }
			if(strncmp(configStrings[i], "full_scr", 8) == 0) { sscanf(configStrings[i], "full_screen %d\n", &tmp);	Global::full_screen	= (bool)tmp; }
			if(strncmp(configStrings[i], "true_col", 8) == 0) { sscanf(configStrings[i], "true_color %d\n", &tmp);	Global::true_color	= (bool)tmp; }
			if(strncmp(configStrings[i], "swap_ste", 8) == 0) { sscanf(configStrings[i], "swap_stereo %d\n", &tmp);	Global::swap_stereo	= (bool)tmp;  }
			if(strncmp(configStrings[i], "auto_spe", 8) == 0) { sscanf(configStrings[i], "auto_speed %d\n", &tmp);	Global::auto_speed	= (bool)tmp;  }
			if(strncmp(configStrings[i], "show_fps", 8) == 0) { sscanf(configStrings[i], "show_fps %d\n", &tmp);	Global::show_fps	= (bool)tmp;  }
			if(strncmp(configStrings[i], "use_play", 8) == 0) { sscanf(configStrings[i], "use_playList %d\n", &tmp);Global::use_playList= (bool)tmp;  }
			if(strncmp(configStrings[i], "use_cdro", 8) == 0) { sscanf(configStrings[i], "use_cdrom %d\n", &tmp);   Global::use_cdrom   = (bool)tmp;  }
			if(strncmp(configStrings[i], "maxLevel", 8) == 0) { sscanf(configStrings[i], "maxLevel %d\n", &Global::maxLevel);  }
			if(strncmp(configStrings[i], "viewGamm", 8) == 0) { sscanf(configStrings[i], "viewGamma %f\n", &Global::viewGamma); }
		}
		altered = false;
	}
	else
		altered = true;
		
	initWidgets();
}
	
//----------------------------------------------------------
void ConfigFile::initWidgets()
{
	bool tmp = altered;
	configSelect[Skill]->setValue(skillToOption(Global::gameSkillBase));
	configSelect[screenSize]->setValue(Global::screenSize);
	configSelect[gfxLevel]->setValue(Global::gfxLevel);
	
	configFloat[volSound]->setValue(Global::volSound);
	configFloat[volMusic]->setValue(Global::volMusic);
	configFloat[mouseSpeed]->setValue(Global::mouseSpeed);
	configFloat[viewGamma]->setValue(Global::viewGamma);
	
	configBool[full_screen]->setValue(Global::full_screen);
	configBool[true_color]->setValue(Global::true_color);
	configBool[swap_stereo]->setValue(Global::swap_stereo);
	configBool[auto_speed]->setValue(Global::auto_speed);
	configBool[show_fps]->setValue(Global::show_fps);
	configBool[use_playList]->setValue(Global::use_playList);
	configBool[use_cdrom]->setValue(Global::use_cdrom);
	// setting these triggers altered to true...
	altered = tmp;
}
		
//----------------------------------------------------------
void ConfigFile::updateVars()
{
	Global::gameSkillBase = optionToSkill(configSelect[Skill]->getValue());
	Global::screenSize	= configSelect[screenSize]->getValue();
	Global::gfxLevel	= configSelect[gfxLevel]->getValue();
	
	Global::volSound	= configFloat[volSound]->getValue();
	Global::volMusic	= configFloat[volMusic]->getValue();
	Global::mouseSpeed	= configFloat[mouseSpeed]->getValue();
	Global::viewGamma	= configFloat[viewGamma]->getValue();
	
	Global::full_screen	= configBool[full_screen]->getValue();
	Global::true_color	= configBool[true_color]->getValue();
	Global::swap_stereo	= configBool[swap_stereo]->getValue();
	Global::auto_speed	= configBool[auto_speed]->getValue();
	Global::show_fps	= configBool[show_fps]->getValue();
	Global::use_playList= configBool[use_playList]->getValue();
	Global::use_cdrom   = configBool[use_cdrom]->getValue();
}

//----------------------------------------------------------
int ConfigFile::skillToOption(float s)
{
	int tmp = (int)((s+0.05)*10.0);
	return tmp-2;
}

float ConfigFile::optionToSkill(int o)
{
	int tmp = o+2;
	return tmp/10.0;
}
//----------------------------------------------------------
void ConfigFile::setSkill(int)
{
	static QString tmp;
	Global::gameSkillBase = optionToSkill(configSelect[Skill]->getValue());
	tmp = configSelect[Skill]->currentText();
	emit newSkillSelected(tmp);
}

//====================================================================
ConfigBool::ConfigBool(QWidget *parent, const char* name)
	: QCheckBox(parent, name)
{
	setText("bool option");
	QObject::connect(this, SIGNAL(clicked()), this, SLOT(setAltered()) );
}
ConfigBool::~ConfigBool()
{
}

void ConfigBool::setAltered()
{
	altered = true;
}

//====================================================================
ConfigFloat::ConfigFloat(QWidget *parent, const char* name)
	: QWidget(parent, name)
{
	textLabel	= new QLabel("text", this);
	textLabel->setMinimumWidth(100);
	textLabel->setAlignment( AlignRight | AlignVCenter);
	valueLabel	= new QLabel("0", this);
	valueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	valueLabel->setFixedWidth(40);
	slider	= new QSlider(QSlider::Horizontal, this);
	
	QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)) );
	
	hLayout = new QHBoxLayout(this, 0, 5);
	hLayout->addWidget(textLabel, 0);
	hLayout->addWidget(slider, 1);
	hLayout->addWidget(valueLabel, 0);
	
}

ConfigFloat::~ConfigFloat()
{
}

void ConfigFloat::setText(const char* txt)
{
	textLabel->setText(txt);
}
void ConfigFloat::setValue(float val)
{
	val = val+(0.05*scale);
	slider->setValue((int)((val/scale)*10.0));
}
void ConfigFloat::setRange(float min, float max, float s)
{
	scale = s;
	slider->setMinValue((int)((min/scale)*10.0));
	slider->setMaxValue((int)((max/scale)*10.0));
}
void ConfigFloat::valueChanged(int val)
{
	altered = true;
	QString tmp;
	tmp.setNum(val*scale*0.1);
	valueLabel->setText(tmp);
}
float ConfigFloat::getValue()
{
	float val = (float)slider->value();
	return (val*scale*0.1);
}


//====================================================================
ConfigSelect::ConfigSelect(QWidget *parent, const char* name)
	: QWidget(parent, name)
{
	textLabel = new QLabel("text", this);
	textLabel->setMinimumWidth(100);
	textLabel->setAlignment( AlignRight | AlignVCenter);
	comboBox = new QComboBox(false, this);
//	comboBox = new QComboBox(this);
	
	QObject::connect(comboBox, SIGNAL(activated(int)), this, SLOT(setAltered(int)) );
	QObject::connect(comboBox, SIGNAL(activated(int)), this, SIGNAL(activated(int)) );
	
	hLayout = new QHBoxLayout(this, 0, 5);
	hLayout->addWidget(textLabel, 0);
	hLayout->addWidget(comboBox, 1);
}

ConfigSelect::~ConfigSelect()
{
}

void ConfigSelect::setText(const char* txt)
{
	textLabel->setText(txt);
}
void ConfigSelect::insertItem(const char* txt)
{
	comboBox->insertItem(txt);
}
void ConfigSelect::setValue(int index)
{
	comboBox->setCurrentItem(index);
}
void ConfigSelect::setAltered(int)
{
	altered = true;
}



//----------------------------------------------------------
void ConfigFile::setupToolTips()
{
//	QToolTip::add(configSelect[Skill], 		" self-explainatory ");
//	QToolTip::add(configSelect[screenSize],	" duh. ");
	QToolTip::add(configSelect[gfxLevel],	" \'high\' should work for most decent gfx cards, \n"
											" \'med\' lowers fillrate requirements a bit, and \n"
											" \'low\' looks like crap.\n"
											" \n"
											" If framerate is erratic, lower the gfx level");

	QToolTip::add(configFloat[volSound], 	" sound volume ");
	QToolTip::add(configFloat[volMusic], 	" music volume ");
	QToolTip::add(configFloat[mouseSpeed], 	" that mouse is wired! ");
	QToolTip::add(configFloat[viewGamma], 	" adjust viewing gamma. Values lower than 1 increase \n"
											" brightness, values greater than 1 will make the game \n"
											" darker");
	QToolTip::add(configBool[full_screen],  " run the game full-screen ");
	QToolTip::add(configBool[true_color],   " request a 32 bit visual. Default is 16 bit color ");
	QToolTip::add(configBool[swap_stereo],  " reverse speaker orientation. ");
	QToolTip::add(configBool[auto_speed],   " attempt to maintain a consistent game speed if\n"
											" framerate varies. Useful if your card is fillrate limited. \n"
											" Can make things worse if framerate is choppy, though.");
	QToolTip::add(configBool[show_fps], 	" show Frames Per Second in the upper right hand corner of \n"
											" the screen. A steady 50fps is optimal.");
	QToolTip::add(configBool[use_playList], " the playList allows you to select your own background music \n"
											" for Chromium. Depending on what options are compiled into \n"
											" OpenAL, you can use WAV, MP3 and/or Ogg/Vorbis files. ");
	QToolTip::add(configBool[use_cdrom],    " If a music CD is in your CDROM player, the CD tracks will be \n"
											" played as background music (overriding the playList). Level 1 \n"
											" plays track 1, etc. \n"
											" Hit \"N\" to skip to the next track during a game.");
	
	QToolTip::add(launchBut, " save current options and launch the game! ");

}

