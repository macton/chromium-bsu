/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "main.h"

#include <qapp.h> 
#include <qfont.h> 
#include <qtooltip.h> 

#if QT_VERSION >= 220
#include <qsgistyle.h> 
#endif

#if        QT_VERSION < 200
#error "------------------------------------"
#error "The Chromium setup app requires Qt2."
#error "You are trying to build it with Qt1."
#error "   Please read the INSTALL file.    "
#error "------------------------------------"
#else   // QT_VERSION < 200

#include "MainWidget.h"
#include "PlayList.h"

#endif  // QT_VERSION < 200

#include "../src/Config.h"
#include "../src/HiScore.h"

#include "icon.xpm"

const char *argv_0 = 0; // we'll use this in MainWidget::launchGame

//====================================================================
int main( int argc, char **argv )
{
	argv_0 = argv[0];
	// This is a hack to set the color. It gives better results with
	// QSGIStyle than setting QPalette directly
	if(argc > 12)
		argc = 12;
	int	my_argc = argc+4;
	char *my_argv[16];
	int i;
	
	switch(0)
	{
		case 0:
			my_argv[0] = argv[0];
			my_argv[1] = "-bg";
			my_argv[2] = "#9494B0";
			my_argv[3] = "-fg";
			my_argv[4] = "black";
			break;
		case 1:
			my_argv[0] = argv[0];
			my_argv[1] = "-bg";
			my_argv[2] = "grey55";
			my_argv[3] = "-fg";
			my_argv[4] = "grey10";
			break;
	}
	for(i = 5; i < argc+4; i++)
		my_argv[i] = argv[i-4];

	argc = my_argc;
	argv = my_argv;
	
	Config::init();
	HiScore::init();
	
	QApplication::setColorSpec( QApplication::ManyColor );
    QApplication a(argc,argv);

	QFont font("Helvetica", 12, QFont::Bold);
	a.setFont(font);

#if QT_VERSION >= 220
	a.setStyle(new QSGIStyle());
#endif

	MainWidget mainWidget;
	
    a.setMainWidget( &mainWidget );
	mainWidget.setIcon(QPixmap((const char**)icon_xpm));
	
	QFont ttfont("Helvetica", 10);
	QToolTip::setFont(ttfont);
	
	mainWidget.resize(550, 300);
	mainWidget.show();

	return a.exec();
}
