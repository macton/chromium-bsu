##################################
## Chromium B.S.U. Project File ##
##################################

#   Definitions 
#--------------------------------------------------------------------
#	CHECK_ERRORS   = check for GL, AL and SDL errors every frame
#	AUDIO_OPENAL   = use OpenAL for audio (preferred)
#	AUDIO_SDLMIXER = use SDL_mixer for audio
#       if AUDIO_OPENAL and AUDIO_SDLMIXER are defined, both must be linked, and 
#       the config determines which API to use.
#	WITH_JOYSTICK  = enable joystick (joystick control sucks - don't bother)
#	USE_SDL        = use SDL as window toolkit
#	NO_PARACHUTE   = disable SDL's parachute
#	USE_GLUT       = use GLUT as window toolkit
#	_REENTRANT     = required for OpenAL
#--------------------------------------------------------------------

TARGET		= chromium
TEMPLATE	= app
CLEAN_FILES += *.bck

##############################
## default (Linux)
##############################
#
# NOTE:
# The Makefile NEEDS to 'include ../config.mak'. I added this line:
# "#$ Expand("TMAKE_TEXT");"
# at line 202 of tmake's unix/generic.t to support this. 
TMAKE_TEXT	= include ../config.mak

TMAKE_CFLAGS	+= $(PKG_CFLAGS) $(AL_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)
TMAKE_CXXFLAGS	+= $(PKG_CFLAGS) $(AL_CFLAGS) $(SDL_CFLAGS) $(SMPEG_CFLAGS)

INCLUDEPATH = ../support/include ../support/glpng/include /usr/X11R6/include
LIBDIRS		= -L../support/glpng/lib -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib
LIBS		= $$LIBDIRS $(GL_LIBS) $(AL_LIBS) $(SDL_LIBS) $(SMPEG_LIBS) $(VORBIS_LIBS)
#LIBS		= $$LIBDIRS $(GL_LIBS) $(AL_LIBS) $(SDL_LIBS) $(SMPEG_LIBS) $(VORBIS_LIBS) -lSDL_mixer

##-- Debug --
#CONFIG		= warn_on debug
#DEFINES 	= _REENTRANT AUDIO_OPENAL OLD_OPENAL CHECK_ERRORS NO_PARACHUTE
##DEFINES 	= _REENTRANT AUDIO_SDLMIXER CHECK_ERRORS NO_PARACHUTE
##DEFINES 	= _REENTRANT AUDIO_SDLMIXER AUDIO_OPENAL CHECK_ERRORS NO_PARACHUTE
#-- Release --
CONFIG		= warn_off release
DEFINES 	= OLD_OPENAL AUDIO_OPENAL _REENTRANT 

##############################
## IRIX
##############################
irix-n32:DEFINES		= _REENTRANT WITHOUT_AUDIO USE_GLUT
irix-n32:INCLUDEPATH	= /u/people/mallan/include ../support/include ../support/glpng/include 
irix-n32:LIBDIRS		= -L/u/people/mallan/lib -L/usr/X11R6/lib -L../support/glpng/lib
irix-n32:GLUT_LIBS		= -lglpng -lglut -lGL -lGLU -lX11 -lXi -lXmu
#irix-n32:AL_LIBS		= -lopenal -lm -ldl -lpthread
irix-n32:AL_LIBS		= -lm -ldl -lpthread
irix-n32:LIBS			= $$LIBDIRS $$GLUT_LIBS $$AL_LIBS 

##############################
## WIN32
##############################
win32-msvc:TEMPLATE		= vcapp

#win32-msvc:DEVEL		= F:\devel
win32-msvc:DEVEL		= G:\devel

win32-msvc:INCLUDEPATH	= ..\support\include $$DEVEL\SDL\include $$DEVEL\openal\win $$DEVEL\glpng\include

win32-msvc:GL_LIBS		= $$DEVEL\glpng\lib\glpng.lib opengl32.lib glu32.lib
win32-msvc:AL_LIBS		= $$DEVEL\openal\win\lib\OpenAL32.lib $$DEVEL\openal\win\lib\ALut.lib 
win32-msvc:SDL_LIBS		= $$DEVEL\SDL\lib\SDLmain.lib $$DEVEL\SDL\lib\SDL.lib 

win32-msvc:LIBS			= $$SDL_LIBS $$AL_LIBS $$GL_LIBS
win32-msvc:CONFIG		= windows thread dll warn_on release
win32-msvc:DEFINES		= USE_SDL

##############################
## Source files
##############################
HEADERS		= \
			textGeometry.h \
			Audio.h \
			AudioOpenAL.h \
			AudioSDLMixer.h \
			Ammo.h \
			Config.h \
			EnemyAircraft.h \
			EnemyAircraft_Straight.h \
			EnemyAircraft_Omni.h \
			EnemyAircraft_RayGun.h \
			EnemyAircraft_Tank.h \
			EnemyAircraft_Gnat.h \
			EnemyAircraft_Boss00.h \
			EnemyAircraft_Boss01.h \
			EnemyAmmo.h \
			EnemyFleet.h \
			Explosions.h \
			HeroAircraft.h \
			HeroAmmo.h \
			Global.h \
			Ground.h \
			GroundMetal.h \
			GroundMetalSegment.h \
			GroundSea.h \
			GroundSeaSegment.h \
			GroundSegment.h \
			HiScore.h \
			MainToolkit.h \
			MainGLUT.h \
			MainSDL.h \
			MainGL.h \
			MenuGL.h \
			NCString.h \
			PowerUps.h \
			ScreenItem.h \
			ScreenItemAdd.h \
			StatusDisplay.h \
			TexFont.h \
			compatibility.h \
			define.h \
			extern.h \
			main.h
			
SOURCES		= \
			textGeometryChromium.cpp \
			textGeometryBSU.cpp \
			Audio.cpp \
			AudioOpenAL.cpp \
			AudioSDLMixer.cpp \
			Ammo.cpp \
			Config.cpp \
			EnemyAircraft.cpp \
			EnemyAircraft_Straight.cpp \
			EnemyAircraft_Omni.cpp \
			EnemyAircraft_RayGun.cpp \
			EnemyAircraft_Tank.cpp \
			EnemyAircraft_Gnat.cpp \
			EnemyAircraft_Boss00.cpp \
			EnemyAircraft_Boss01.cpp \
			EnemyAmmo.cpp \
			EnemyFleet.cpp \
			Explosions.cpp \
			HeroAircraft.cpp \
			HeroAmmo.cpp \
			Global.cpp \
			Ground.cpp \
			GroundMetal.cpp \
			GroundMetalSegment.cpp \
			GroundSea.cpp \
			GroundSeaSegment.cpp \
			GroundSegment.cpp \
			HiScore.cpp \
			NCString.cpp \
			MainToolkit.cpp \
			MainGLUT.cpp \
			MainSDL.cpp \
			MainSDL_Event.cpp \
			MainGL.cpp \
			MenuGL.cpp \
			PowerUps.cpp \
			ScreenItem.cpp \
			ScreenItemAdd.cpp \
			StatusDisplay.cpp \
			TexFont.cpp \
			main.cpp
