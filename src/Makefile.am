MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = chromium
bindir = $(exec_prefix)/games

chromium_CPPFLAGS = $(GL_CFLAGS) $(GLU_CFLAGS) $(FTGL_CFLAGS) $(GLPNG_CFLAGS) \
	$(WINDOW_CFLAGS) $(AUDIO_CFLAGS) $(FONT_CFLAGS)

chromium_LDADD = $(GL_LIBS) $(GLU_LIBS) $(FTGL_LIBS) $(GLPNG_LIBS) \
	$(WINDOW_LIBS) $(AUDIO_LIBS) $(FONT_LIBS)

chromium_SOURCES = \
	Ammo.cpp \
	Ammo.h \
	Audio.cpp \
	Audio.h \
	AudioOpenAL.cpp \
	AudioOpenAL.h \
	AudioSDLMixer.cpp \
	AudioSDLMixer.h \
	compatibility.h \
	Config.cpp \
	Config.h \
	define.h \
	EnemyAircraft_Boss00.cpp \
	EnemyAircraft_Boss00.h \
	EnemyAircraft_Boss01.cpp \
	EnemyAircraft_Boss01.h \
	EnemyAircraft.cpp \
	EnemyAircraft_Gnat.cpp \
	EnemyAircraft_Gnat.h \
	EnemyAircraft.h \
	EnemyAircraft_Omni.cpp \
	EnemyAircraft_Omni.h \
	EnemyAircraft_RayGun.cpp \
	EnemyAircraft_RayGun.h \
	EnemyAircraft_Straight.cpp \
	EnemyAircraft_Straight.h \
	EnemyAircraft_Tank.cpp \
	EnemyAircraft_Tank.h \
	EnemyAmmo.cpp \
	EnemyAmmo.h \
	EnemyFleet.cpp \
	EnemyFleet.h \
	Explosions.cpp \
	Explosions.h \
	extern.h \
	Global.cpp \
	Global.h \
	Ground.cpp \
	Ground.h \
	GroundMetal.cpp \
	GroundMetal.h \
	GroundMetalSegment.cpp \
	GroundMetalSegment.h \
	GroundSea.cpp \
	GroundSea.h \
	GroundSeaSegment.cpp \
	GroundSeaSegment.h \
	GroundSegment.cpp \
	GroundSegment.h \
	HeroAircraft.cpp \
	HeroAircraft.h \
	HeroAmmo.cpp \
	HeroAmmo.h \
	HiScore.cpp \
	HiScore.h \
	MainGL.cpp \
	MainGL.h \
	MainGLUT.cpp \
	MainGLUT.h \
	MainSDL.cpp \
	MainSDL_Event.cpp \
	MainSDL.h \
	MainToolkit.cpp \
	MainToolkit.h \
	MenuGL.cpp \
	MenuGL.h \
	PowerUps.cpp \
	PowerUps.h \
	ScreenItemAdd.cpp \
	ScreenItemAdd.h \
	ScreenItem.cpp \
	ScreenItem.h \
	StatusDisplay.cpp \
	StatusDisplay.h \
	textGeometryBSU.cpp \
	textGeometryChromium.cpp \
	textGeometry.h \
	main.cpp \
	main.h
