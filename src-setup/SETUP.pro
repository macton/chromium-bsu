TARGET		= chromium-setup
TEMPLATE	= app
CLEAN_FILES += *.bck
MOC_DIR		=	moc

#CONFIG		= qt warn_on release
CONFIG		= qt warn_on debug

## NOTE:
## The Makefile NEEDS to 'include ../config.mak'. I added this line:
## "#$ Expand("TMAKE_TEXT");"
## at line 202 of tmake's unix/generic.t to support this. 
TMAKE_TEXT	= include ../config.mak

TMAKE_CFLAGS	+= $(PKG_CFLAGS) $(AL_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)
TMAKE_CXXFLAGS	+= $(PKG_CFLAGS) $(AL_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)

DEFINES		= _REENTRANT
INCLUDEPATH = 
LIBDIRS		= 

LIBS		= $$LIBDIRS $(AL_LIBS) $(SDL_LIBS) $(SMPEG_LIBS) $(VORBIS_LIBS) -lm -pthread


##############################
## Sources
##############################
HEADERS		= \
			Audio.h \
			ConfigFile.h \
			HelpPage.h \
			HiScoreWidget.h \
			MainWidget.h \
			PlayList.h \
			../src/define.h \
			../src/Config.h \
			../src/HiScore.h \
			main.h
			
SOURCES		= \
			Audio.cpp \
			ConfigFile.cpp \
			HelpPage.cpp \
			HiScoreWidget.cpp \
			MainWidget.cpp \
			PlayList.cpp \
			../src/Config.cpp \
			../src/HiScore.cpp \
			main.cpp

INTERFACES	= 
