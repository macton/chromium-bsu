/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Clarified Artistic License"
 */
#ifndef HelpPage_h
#define HelpPage_h

#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>

//====================================================================
class HelpPage : public QDialog
{
	Q_OBJECT
public:
	HelpPage(QWidget *parent = 0, const char* name = 0, bool modal = false);
	~HelpPage();
	
	bool setPageSource(const char*);

protected slots:
	void	linkText(const QString &);

protected:
	QVBoxLayout		*topVLayout;
	QHBoxLayout		*butLayout;
	
	QPushButton		*closeBut;
	QPushButton		*backBut;
	
	QLineEdit		*linkEdit;
	
	QTextBrowser	*page;
};

#endif // HelpPage_h
