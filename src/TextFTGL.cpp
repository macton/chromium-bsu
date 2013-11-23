/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#ifdef TEXT_FTGL

#include "gettext.h"

#include "TextFTGL.h"

#include <sys/stat.h>
#include <FTGL/ftgl.h>

#ifdef HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif

using namespace std;

//====================================================================
TextFTGL::TextFTGL()
{
	fontFile = findFont();
	ftFont = new FTBufferFont(fontFile);
	if(ftFont->Error())
	{
		fprintf(stderr, _("FTGL: error loading font: %s\n"), fontFile);
		delete ftFont; ftFont = NULL;
		free((void*)fontFile); fontFile = NULL;
		throw _("FTGL: error loading font");
	}
	free((void*)fontFile); fontFile = NULL;
	ftFont->FaceSize(24);
}

TextFTGL::~TextFTGL()
{
	delete ftFont; ftFont = NULL;
	free((void*)fontFile); fontFile = NULL;
}

void TextFTGL::Render(const char* str, const int len)
{
	/*
	  FTGL renders the whole string when len == 0
	  but we don't want any text rendered then.
	*/
	if(len != 0)
		ftFont->Render(str, len);
}

float TextFTGL::Advance(const char* str, const int len)
{
	return ftFont->Advance(str, len);
}

float TextFTGL::LineHeight(const char* str, const int len)
{
	return ftFont->LineHeight();
}

const char* TextFTGL::findFont()
{
  return strdup("c:\\windows\\fonts\\arial.ttf");
}

#endif // TEXT_FTGL
