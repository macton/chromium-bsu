#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.58])

AC_INIT([Chromium B.S.U.], [0.9.15.1], [http://sf.net/projects/chromium-bsu/bugs], [chromium-bsu])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([chromium-bsu-config.h])

# Programs
AC_PROG_CXX
PKG_PROG_PKG_CONFIG

# Headers
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h locale.h])

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

# Enable some special macintosh path handling
AC_ARG_WITH(macintosh,
	AS_HELP_STRING([--with-macintosh],[Enable some special macintosh path handling]),
	AC_DEFINE(macintosh, 1, [define to enable some special macintosh path handling]),
	[])

# Libraries

# gettext for internationalisation
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])

# Need OpenGL and GLU for drawing stuff
AX_CHECK_GL
AX_CHECK_GLU

# Need a recent version of FTGL or GLC for text rendering
AC_ARG_ENABLE(glc, AS_HELP_STRING([--disable-glc], [do not use GLC]))
AC_ARG_ENABLE(ftgl, AS_HELP_STRING([--disable-ftgl], [do not use FTGL]))

if test "x$enable_glc" != xno ; then
	AC_CHECK_HEADERS([GL/glc.h], [HAVE_GL_GLC_H=yes], [HAVE_GL_GLC_H=no])
	AC_CHECK_HEADERS([QuesoGLC/glc.h], [HAVE_QUESOGL_GLC_H=yes], [HAVE_QUESOGL_GLC_H=no])
	AC_CHECK_LIB([GLC], [glcGenContext], [HAVE_GLC_LIB=yes], [HAVE_GLC_LIB=no])
	AC_CHECK_LIB([glc32], [glcGenContext], [HAVE_GLC32_LIB=yes], [HAVE_GLC32_LIB=no])
fi

if test "x$enable_ftgl" != xno ; then
	PKG_CHECK_MODULES([FTGL], [ftgl >= 2.1.3], [HAVE_FTGL=yes], [HAVE_FTGL=no])
fi

# Allow specifiying a hard-coded font file at build time
AC_ARG_WITH(font-path,
	AS_HELP_STRING([--with-font-path=file.ttf],
		[Path to the TTF font to use at runtime with FTGL (bold Gothic Uralic recommended)]),
	[FONT_PATH="$withval"],
	[FONT_PATH=""])

if test "x$HAVE_FTGL" = xyes -a "x$enable_ftgl" != xno ; then
	# Need fontconfig for finding font files when using FTGL
	PKG_CHECK_MODULES([FONTCONFIG], [fontconfig], [HAVE_FONTCONFIG=yes], [HAVE_FONTCONFIG=no])
fi

# Allow specifiying a hard-coded font name at build time
AC_ARG_WITH(font-name,
	AS_HELP_STRING([--with-font-name="Gothic Uralic"],
		[Name of the font to default to at runtime with GLC (bold Gothic Uralic is default)]),
	[FONT_NAME="$withval"],
	[FONT_NAME=""])

# Need SDL or GLUT for windowing
AC_ARG_ENABLE(sdl, AS_HELP_STRING([--disable-sdl], [do not use SDL]))
AC_ARG_ENABLE(glut, AS_HELP_STRING([--disable-glut], [do not use GLUT]))

if test "x$enable_sdl" != xno ; then
	PKG_CHECK_MODULES([SDL], [sdl >= 1.1.6], [HAVE_SDL=yes], [HAVE_SDL=no])
fi

if test "x$enable_glut" != xno ; then
	AX_CHECK_GLUT
fi

# Need OpenAL or SDL Mixer for sound
AC_ARG_ENABLE(openal, AS_HELP_STRING([--disable-openal], [do not use OpenAL]))
AC_ARG_ENABLE(sdlmixer, AS_HELP_STRING([--disable-sdlmixer], [do not use SDL_Mixer]))

if test "x$enable_openal" != xno ; then
	PKG_CHECK_MODULES([OPENAL], [openal], [HAVE_OPENAL=yes], [HAVE_OPENAL=no])
	PKG_CHECK_MODULES([FREEALUT], [freealut], [HAVE_FREEALUT=yes], [HAVE_FREEALUT=no])
fi

if test "x$enable_sdlmixer" != xno ; then
	PKG_CHECK_MODULES([SDLMIXER], [SDL_mixer], [HAVE_SDLMIXER=yes], [HAVE_SDLMIXER=no])
fi

# Need glpng or SDL_image so we can display PNG images using OpenGL
AC_ARG_ENABLE(glpng, AS_HELP_STRING([--disable-glpng], [do not use glpng]))
AC_ARG_ENABLE(sdlimage, AS_HELP_STRING([--disable-sdlimage], [do not use SDL_image]))

if test "x$enable_sdlimage" != xno ; then
	PKG_CHECK_MODULES([SDLIMAGE], [SDL_image], [HAVE_SDLIMAGE=yes], [HAVE_SDLIMAGE=no])
fi

if test "x$enable_glpng" != xno ; then
	AC_CHECK_HEADERS([GL/glpng.h], [HAVE_GL_GLPNG_H=yes], [HAVE_GL_GLPNG_H=no])
	AC_CHECK_HEADERS([glpng/glpng.h], [HAVE_GLPNG_GLPNG_H=yes], [HAVE_GLPNG_GLPNG_H=no])
	AC_CHECK_LIB([glpng], [pngBind], [HAVE_GLPNG_LIB=yes], [HAVE_GLPNG_LIB=no], [$GL_LIBS])
fi


if test "x$no_gl" = xyes ; then
	AC_MSG_ERROR([cannot find OpenGL (drawing system)])
fi

if test "x$no_glu" = xyes ; then
	AC_MSG_ERROR([cannot find GLU (drawing system)])
fi


if test \( "x$HAVE_GL_GLC_H" = xyes -o "x$HAVE_QUESOGLC_GLC_H" = xyes \) -a "x$HAVE_GLC_LIB" = xyes -a "x$enable_glc" != xno ; then
	AC_DEFINE(TEXT_GLC, 1, [define to use GLC])
	TEXT_LIBS="-lGLC"
	AC_SUBST(TEXT_LIBS)
	TEXT_TYPE=GLC
elif test \( "x$HAVE_GL_GLC_H" = xyes -o "x$HAVE_QUESOGLC_GLC_H" = xyes \) -a "x$HAVE_GLC32_LIB" = xyes -a "x$enable_glc" != xno ; then
	AC_DEFINE(TEXT_GLC, 1, [define to use GLC])
	TEXT_LIBS="-lglc32"
	AC_SUBST(TEXT_LIBS)
	TEXT_TYPE=GLC
elif test "x$HAVE_FTGL" = xyes -a "x$enable_ftgl" != xno ; then
	AC_DEFINE(TEXT_FTGL, 1, [define to use FTGL])
	TEXT_CFLAGS="$FTGL_CFLAGS"
	AC_SUBST(TEXT_CFLAGS)
	TEXT_LIBS="$FTGL_LIBS"
	AC_SUBST(TEXT_LIBS)
	TEXT_TYPE=FTGL
else
	AC_MSG_ERROR([cannot find GLC or FTGL >= 2.1.3 (text renderer)])
fi

if test "x$TEXT_TYPE" = xGLC -a "x$FONT_NAME" != x ; then
	TEXT_CFLAGS="$TEXT_CFLAGS -DFONT_NAME=\"\\\"$FONT_NAME\\\"\""
	AC_SUBST(TEXT_CFLAGS)
	FONT="$FONT_NAME"
elif test "x$TEXT_TYPE" = xGLC ; then
	FONT="default (bold Gothic Uralic)"
fi

if test "x$TEXT_TYPE" = xFTGL -a "x$FONT_PATH" != x ; then
	TEXT_CFLAGS="$TEXT_CFLAGS -DFONT_PATH=\"\\\"$FONT_PATH\\\"\""
	AC_SUBST(TEXT_CFLAGS)
	FONT="$FONT_PATH "
fi

if test "x$TEXT_TYPE" = xFTGL -a "x$HAVE_FONTCONFIG" = xyes ; then
	AC_DEFINE(HAVE_FONTCONFIG, 1, [define to use fontconfig])
	TEXT_CFLAGS="$TEXT_CFLAGS $FONTCONFIG_CFLAGS"
	AC_SUBST(TEXT_CFLAGS)
	TEXT_LIBS="$TEXT_LIBS $FONTCONFIG_LIBS"
	AC_SUBST(TEXT_LIBS)
	FONT="$FONT fontconfig (bold Gothic Uralic) "
fi

if  test "x$TEXT_TYPE" = xFTGL -a "x$HAVE_FONTCONFIG" != xyes -a "x$FONT_PATH" = x ; then
	AC_MSG_WARN([FTGL: no font path supplied and cannot find fontconfig, font searching will probably fail])
fi

if test "x$HAVE_SDL" = xyes -a "x$enable_sdl" != xno ; then
	AC_DEFINE(USE_SDL, 1, [define to use SDL])
	AC_DEFINE(USE_SDL_CDROM, 1, [define to use SDL CDROM support])
	WINDOW_CFLAGS="$SDL_CFLAGS"
	AC_SUBST(WINDOW_CFLAGS)
	WINDOW_LIBS="$SDL_LIBS"
	AC_SUBST(WINDOW_LIBS)
	WINDOW_TYPE=SDL
elif test "x$no_glut" != xyes -a "x$enable_glut" != xno ; then
	AC_DEFINE(USE_GLUT, 1, [define to use GLUT])
	WINDOW_CFLAGS="$GLUT_CFLAGS"
	AC_SUBST(WINDOW_CFLAGS)
	WINDOW_LIBS="$GLUT_LIBS"
	AC_SUBST(WINDOW_LIBS)
	WINDOW_TYPE=GLUT
else
	AC_MSG_ERROR([cannot find SDL >= 1.1.6 or GLUT (window system)])
fi

if test "x$HAVE_OPENAL" = xyes -a "x$HAVE_FREEALUT" = xyes -a "x$enable_openal" != xno ; then
	AC_DEFINE(AUDIO_OPENAL, 1, [define to use OpenAL])
	AUDIO_CFLAGS="$OPENAL_CFLAGS $FREEALUT_CFLAGS"
	AC_SUBST(AUDIO_CFLAGS)
	AUDIO_LIBS="$OPENAL_LIBS $FREEALUT_LIBS"
	AC_SUBST(AUDIO_LIBS)
	AUDIO_TYPE=OpenAL

	# Need to check for MacOS X OpenAL differences
	ac_save_CPPFLAGS="$CPPFLAGS"
	ac_save_CFLAGS="$CFLAGS"
	CPPFLAGS="${CPPFLAGS} ${OPENAL_CFLAGS} ${FREEALUT_CFLAGS}"
	CFLAGS="${CFLAGS} ${OPENAL_CFLAGS} ${FREEALUT_CFLAGS}"
	AC_CHECK_HEADERS(OpenAL/al.h OpenAL/alc.h ALUT/alut.h)
	AC_CHECK_HEADERS(AL/al.h AL/alc.h AL/alut.h)
	CPPFLAGS="${ac_save_CPPFLAGS}"
	CFLAGS="${ac_save_CFLAGS}"
elif test "x$HAVE_SDLMIXER" = xyes -a "x$enable_sdlmixer" != xno ; then
	AC_DEFINE(AUDIO_SDLMIXER, 1, [define to use SDL Mixer])
	AUDIO_CFLAGS="$SDLMIXER_CFLAGS"
	AC_SUBST(AUDIO_CFLAGS)
	AUDIO_LIBS="$SDLMIXER_LIBS"
	AC_SUBST(AUDIO_LIBS)
	AUDIO_TYPE="SDL mixer"
else
	AC_MSG_ERROR([cannot find OpenAL & ALUT or SDL Mixer (sound)])
fi

if test  \( "x$HAVE_GL_GLPNG_H" = xyes -o "x$HAVE_GLPNG_GLPNG_H" = xyes \) -a "x$HAVE_GLPNG_LIB" = xyes -a "x$enable_glpng" != no ; then
	AC_DEFINE(IMAGE_GLPNG, 1, [define to use glpng])
	IMAGE_TYPE=glpng
	IMAGE_GLPNG=yes
elif test "x$WINDOW_TYPE" = xSDL -a "x$HAVE_SDLIMAGE" = xyes -a "x$enable_sdlimage" != no ; then
	AC_MSG_WARN([Using SDL_image to load images into textures, some rendering artifacts may be present])
	AC_DEFINE(IMAGE_SDL, 1, [define to use SDL_image])
	IMAGE_TYPE=SDL_image
	IMAGE_SDLIMAGE=yes
else
	AC_MSG_ERROR([cannot find glpng or SDL_image (image loader)])
fi

if test "x$WINDOW_TYPE" = xSDL -a "x$HAVE_SDLIMAGE" = xyes -a "x$enable_sdlimage" != no ; then
	AC_DEFINE(ENABLE_WINDOW_ICON, 1, [define to enable the window icon (needs SDL_image)])
	WINDOW_ICON=yes
	IMAGE_SDLIMAGE=yes
else
	AC_MSG_WARN([SDL_image is not usable, window icon will not be set])
	WINDOW_ICON=no
fi

if test "x$IMAGE_GLPNG" = xyes ; then
	IMAGE_CFLAGS="$IMAGE_CFLAGS"
	AC_SUBST(IMAGE_CFLAGS)
	IMAGE_LIBS="$IMAGE_LIBS -lglpng"
	AC_SUBST(IMAGE_LIBS)
fi

if test "x$IMAGE_SDLIMAGE" = xyes ; then
	IMAGE_CFLAGS="$IMAGE_CFLAGS $SDLIMAGE_CFLAGS"
	AC_SUBST(IMAGE_CFLAGS)
	IMAGE_LIBS="$IMAGE_LIBS $SDLIMAGE_LIBS"
	AC_SUBST(IMAGE_LIBS)
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
	po/Makefile.in
])

AC_OUTPUT

echo "

Chromium B.S.U. ready for building!

  Window type: $WINDOW_TYPE
  Window icon: $WINDOW_ICON
  Image type:  $IMAGE_TYPE
  Audio type:  $AUDIO_TYPE
  Text type:   $TEXT_TYPE
  Font search: $FONT

Type 'make' to build.

"
