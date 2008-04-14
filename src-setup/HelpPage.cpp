/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "HelpPage.h"

#include <cstdio>

#include <qwidget.h>
#include <qapplication.h>

//====================================================================
HelpPage::HelpPage(QWidget *parent, const char* name, bool modal)
	: QDialog(parent, name, modal)
{
	page = new QTextBrowser(this);
	
#if QT_VERSION >= 300
	page->setColor(QColor(192, 192, 255));
	page->setPaper(QBrush(QColor(70, 60, 100)));
#else
	QColorGroup	cgroup;
	cgroup.setColor(QColorGroup::Base, QColor(70, 60, 100));
	cgroup.setColor(QColorGroup::Text, QColor(220, 220, 220));
	page->setPaperColorGroup(cgroup);
	page->setLinkColor(QColor(192, 192, 255));
#endif

	backBut = new QPushButton(this);
	backBut->setText("&Back");
	backBut->hide();
	
	closeBut = new QPushButton(this);
	closeBut->setText("&Close");
	
	linkEdit = new QLineEdit(this);
	linkEdit->setReadOnly(true);
	
	//-- Connect
	QObject::connect( closeBut, SIGNAL(clicked()), this, SLOT(accept()) );
	QObject::connect( backBut, SIGNAL(clicked()), page, SLOT(backward()) );
	QObject::connect( page, SIGNAL(highlighted(const QString&)), this, SLOT(linkText(const QString&)) );
	
	//-- Layout
	topVLayout	= new QVBoxLayout(this, 5, 5);
	butLayout	= new QHBoxLayout(5);
	
	topVLayout->addLayout(butLayout, 0);
		butLayout->addWidget(backBut, 0);
		butLayout->addStretch(1);
		butLayout->addWidget(closeBut, 0);
	topVLayout->addWidget(page, 1);
	topVLayout->addWidget(linkEdit, 0);
		
	int w = QApplication::desktop()->width();
	int h = QApplication::desktop()->height();
	resize( (int)(w*0.7), (int)(h*0.9) );
}

HelpPage::~HelpPage()
{
}

//--------------------------------------------------------------------
bool HelpPage::setPageSource(const char* filename)
{
	QFont	font("Helvetica", 12, QFont::Normal);
	page->setFont(font);
	linkEdit->clear();
	page->setSource(filename);
	page->setFont(font);
	return true;
}

//----------------------------------------------------------
void HelpPage::linkText(const QString &str)
{
	if(!str.isEmpty())
	{
		linkEdit->setText(str);
	}
}
