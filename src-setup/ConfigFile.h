/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef ConfigFile_h
#define ConfigFile_h

#include <qframe.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qslider.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

class ConfigBool;
class ConfigFloat;
class ConfigSelect;

//====================================================================
class ConfigFile : public QWidget
{
	Q_OBJECT
public:
	ConfigFile(QWidget *parent = 0, const char *name = 0);
	~ConfigFile();

signals:
	void playListEnabled(bool);
	void launchGame();
	void newSkillSelected(const QString &);

public slots:
	void setSkill(int);
	void cdromEnabled(bool);
	void saveCurrentConfig();
	void loadCurrentConfig();
	void exit();
	
protected:
	void initWidgets();
	void updateVars();
	
	void setupToolTips();

private:
	QVBoxLayout	*topVLayout;
	QHBoxLayout	*hLayout;
	QVBoxLayout	*v1Layout;
	QVBoxLayout	*v2Layout;
	
	QLabel	*chromLabel;
	
	int		skillToOption(float s);
	float	optionToSkill(int o);
	
	QPushButton	*launchBut;
	
	enum			OptionSelect	{ Skill, screenSize, gfxLevel, cdromDevice, NumSelectOptions };
	int				valueSelect[NumSelectOptions];
	ConfigSelect	*configSelect[NumSelectOptions];
	
	enum			OptionFloat		{ volSound, volMusic, mouseSpeed, viewGamma, NumFloatOptions };
	float			valueFloat[NumFloatOptions];
	ConfigFloat		*configFloat[NumFloatOptions];
	
	enum			OptionBool		{ full_screen, true_color, swap_stereo, auto_speed, show_fps, use_playList, use_cdrom, NumBoolOptions };
	bool			valueBool[NumBoolOptions];
	ConfigBool		*configBool[NumBoolOptions];
	
};
	
//====================================================================
class ConfigBool : public QCheckBox
{
	Q_OBJECT
public:
	ConfigBool(QWidget *parent = 0, const char* name = 0);
	~ConfigBool();
	
	void setValue(bool status) { setChecked(status); }
	bool getValue() { return isChecked(); }

protected slots:	
	void setAltered();
};

//====================================================================
class ConfigFloat : public QFrame
{
	Q_OBJECT
public:
	ConfigFloat(QWidget *parent = 0, const char* name = 0);
	~ConfigFloat();
	
	void setText(const char* t);
	void setValue(float);
	void setRange(float min, float max, float scale = 1.0);
	float getValue();
	
protected slots:
	void valueChanged(int);
	
private:
	QHBoxLayout	*hLayout;
	QLabel		*textLabel;
	QSlider		*slider;
	QLabel		*valueLabel;
	float scale;
};

//====================================================================
class ConfigSelect : public QFrame
{
	Q_OBJECT
public:
	ConfigSelect(QWidget *parent = 0, const char* name = 0);
	~ConfigSelect();

	void	setText(const char* t);
	void	insertItem(const char* t);
	void	setValue(int);
	int		getValue() { return comboBox->currentItem(); }
	QString currentText() { return comboBox->currentText(); }
	void	clear();

signals:	
	void activated(int);
	
protected slots:	
	void setAltered(int);

private:
	QHBoxLayout	*hLayout;
	QLabel		*textLabel;
	QComboBox	*comboBox;
};


#endif // ConfigFile_h
