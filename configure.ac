#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.58])

AC_INIT([Chromium B.S.U.], [0.9.12+], [http://sf.net/projects/chromium-bsu], [chromium])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_SRCDIR([src/main.cpp])
AM_CONFIG_HEADER([config.h])

AC_SUBST([bindir], ['${exec_prefix}/games'])
AC_SUBST([datadir], ['${datarootdir}/games'])

# Programs
AC_PROG_CXX

# Headers
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Types and compiler bits
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM

# Functions
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit floor setenv sqrt strcasecmp strchr strerror strrchr])

# Libraries

# Need OpenGL and GLU for drawing stuff
AX_CHECK_GL
AX_CHECK_GLU

# Need a recent version of FTGL for font rendering
PKG_CHECK_MODULES([FTGL], [ftgl >= 2.1.3], [HAVE_FTGL=yes], [HAVE_FTGL=no])

# Allow specifiying a hard-coded font file at build time
AC_ARG_WITH(font-path,
	AS_HELP_STRING([--with-font-path=file.ttf],
	[Path to the TTF font chromium should use at runtime (bold Gothic Uralic recommended]),
	[FONT_PATH="$withval"],
	[FONT_PATH=""])

# Need fontconfig for finding font files
PKG_CHECK_MODULES([FONTCONFIG], [fontconfig], [HAVE_FONTCONFIG=yes], [HAVE_FONTCONFIG=no])

# Need SDL or GLUT for windowing
PKG_CHECK_MODULES([SDL], [sdl >= 1.1.6], [HAVE_SDL=yes], [HAVE_SDL=no])
AX_CHECK_GLUT

# Need OpenAL or SDL Mixer for sound
PKG_CHECK_MODULES([OPENAL], [openal], [HAVE_OPENAL=yes], [HAVE_OPENAL=no])
PKG_CHECK_MODULES([FREEALUT], [freealut], [HAVE_FREEALUT=yes], [HAVE_FREEALUT=no])
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio], [HAVE_SDLMIXER=yes], [HAVE_SDLMIXER=no])

# Need glpng so we can display PNG images using OpenGL
AC_CHECK_LIB([glpng], [pngBind],[HAVE_GLPNG=yes],[HAVE_GLPNG=no])


if test "x$no_gl" = xyes ; then
	AC_MSG_ERROR([cannot find OpenGL (drawing system)])
fi

if test "x$no_glu" = xyes ; then
	AC_MSG_ERROR([cannot find GLU (drawing system)])
fi

if test "x$HAVE_FTGL" != xyes ; then
	AC_MSG_ERROR([cannot find FTGL >= 2.1.3 (text renderer)])
fi

if test "x$FONT_PATH" != x ; then
	FONT_CFLAGS="-DFONT_PATH=\\\"$FONT_PATH\\\""
	AC_SUBST(FONT_CFLAGS)
fi

if test "x$HAVE_FONTCONFIG" = xyes ; then
	AC_DEFINE(HAVE_FONTCONFIG, 1, [define to use fontconfig])
	FONT_CFLAGS+=" $FONTCONFIG_CFLAGS"
	AC_SUBST(FONT_CFLAGS)
	FONT_LIBS="$FONTCONFIG_LIBS"
	AC_SUBST(FONT_LIBS)
	FONTCONFIG=fontconfig
fi

if  test "x$HAVE_FONTCONFIG" != xyes -a "x$FONT_PATH" = x ; then
	AC_MSG_WARN([no font path supplied and cannot find fontconfig, font searching will probably fail])
fi

if test "x$HAVE_SDL" = xyes ; then
	AC_DEFINE(USE_SDL, 1, [define to use SDL])
	WINDOW_CFLAGS="$SDL_CFLAGS"
	AC_SUBST(WINDOW_CFLAGS)
	WINDOW_LIBS="$SDL_LIBS"
	AC_SUBST(WINDOW_LIBS)
	WINDOW_TYPE=SDL
elif test "x$no_glut" != xyes ; then
	AC_DEFINE(USE_GLUT, 1, [define to use GLUT])
	WINDOW_CFLAGS="$GLUT_CFLAGS"
	AC_SUBST(WINDOW_CFLAGS)
	WINDOW_LIBS="$GLUT_LIBS"
	AC_SUBST(WINDOW_LIBS)
	WINDOW_TYPE=GLUT
else
	AC_MSG_ERROR([cannot find SDL >= 1.1.6 or GLUT (window system)])
fi

if test "x$HAVE_OPENAL" = xyes -a "x$HAVE_FREEALUT" = xyes ; then
	AC_DEFINE(AUDIO_OPENAL, 1, [define to use OpenAL])
	AUDIO_CFLAGS="$OPENAL_CFLAGS $FREEALUT_CFLAGS"
	AC_SUBST(AUDIO_CFLAGS)
	AUDIO_LIBS="$OPENAL_LIBS $FREEALUT_LIBS"
	AC_SUBST(AUDIO_LIBS)
	AUDIO_TYPE=OpenAL
elif test "x$HAVE_SDLMIXER" = xyes ; then
	AC_DEFINE(AUDIO_SDLMIXER, 1, [define to use SDL Mixer])
	AUDIO_CFLAGS=""
	AC_SUBST(AUDIO_CFLAGS)
	AUDIO_LIBS=-lSDL_mixer
	AC_SUBST(AUDIO_LIBS)
	AUDIO_TYPE="SDL mixer"
else
	AC_MSG_ERROR([cannot find OpenAL & ALUT or SDL Mixer (sound)])
fi

if test "x$HAVE_GLPNG" = xyes ; then
	GLPNG_CFLAGS=""
	AC_SUBST(GLPNG_CFLAGS)
	GLPNG_LIBS=-lglpng
	AC_SUBST(GLPNG_LIBS)
else
	AC_MSG_ERROR([cannot find glpng (image loader)])
fi

AC_CONFIG_FILES([
	Makefile
	data/doc/Makefile
	data/doc/images/Makefile
	data/png/Makefile
	data/wav/Makefile
	data/Makefile
	misc/Makefile
	src/Makefile
	m4/Makefile
])

AC_OUTPUT

echo "

Chromium B.S.U. ready for building!

  Window type: $WINDOW_TYPE
  Audio type:  $AUDIO_TYPE
  Font path:   $FONT_PATH $FONTCONFIG

Type 'make' to build.

"
