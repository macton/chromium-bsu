TARGET		= chromium-setup
TEMPLATE	= app
CLEAN_FILES += *.bck
MOC_DIR		=	moc

CONFIG		= qt warn_on release

## NOTE:
## The Makefile NEEDS to 'include ../config.mak'. I added this line:
## "#$ Expand("TMAKE_TEXT");"
## at line 202 of tmake's unix/generic.t to support this. 
TMAKE_TEXT	= include ../config.mak

TMAKE_CFLAGS	+= $(PKG_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)
TMAKE_CXXFLAGS	+= $(PKG_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)

DEFINES		= _REENTRANT
INCLUDEPATH = ../support/openal/include
LIBDIRS		= -L../support/openal/lib

LIBS		= $$LIBDIRS $(SDL_LIBS) $(AL_LIBS) $(SMPEG_LIBS) $(VORBIS_LIBS) -lm -pthread


##############################
## Sources
##############################
HEADERS		= \
			Audio.h \
			ConfigFile.h \
			Global.h \
			HelpPage.h \
			HiScore.h \
			MainWidget.h \
			PlayList.h \
			../src/define.h \
			main.h
			
SOURCES		= \
			Audio.cpp \
			ConfigFile.cpp \
			Global.cpp \
			HelpPage.cpp \
			HiScore.cpp \
			MainWidget.cpp \
			PlayList.cpp \
			main.cpp

INTERFACES	= 
