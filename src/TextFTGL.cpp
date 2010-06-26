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
	struct stat statbuf;
	const char* font = NULL;
	const char* path = NULL;

	#define CHECK_FONT_PATH(filename) \
	{ \
		path = filename; \
		if( !font && path && 0 == stat(path, &statbuf) ) \
			font = strdup(path); \
	}

	// Get user-specified font path
	CHECK_FONT_PATH(getenv("CHROMIUM_BSU_FONT"))

#ifdef FONT_PATH
	// Get distro-specified font path
	CHECK_FONT_PATH(FONT_PATH)
#endif

#ifdef HAVE_FONTCONFIG
	// Get default font via fontconfig
	if( !font && FcInit() )
	{
		FcResult result;
		FcFontSet *fs;
		FcPattern* pat;
		FcPattern *match;

		/*
		TRANSLATORS: If using the FTGL backend, this should be the font
		name of a font that contains all the Unicode characters in use in
		your translation.
		*/
		pat = FcNameParse((FcChar8 *)_("Gothic Uralic"));
		FcConfigSubstitute(0, pat, FcMatchPattern);

		FcPatternDel(pat, FC_WEIGHT);
		FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD);

		FcDefaultSubstitute(pat);
		fs = FcFontSetCreate();
		match = FcFontMatch(0, pat, &result);

		if (match) FcFontSetAdd(fs, match);
		if (pat) FcPatternDestroy(pat);
		if(fs){
			FcChar8* file;
			if( FcPatternGetString (fs->fonts[0], FC_FILE, 0, &file) == FcResultMatch ){
				CHECK_FONT_PATH((const char*)file)
			}
			FcFontSetDestroy(fs);
		}
		FcFini();
	}
#endif

	// Check a couple of common paths for Gothic Uralic/bold as a last resort
	// Debian
	/*
	TRANSLATORS: If using the FTGL backend, this should be the path of a bold
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Debian it should be the Debian path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/truetype/uralic/gothub__.ttf"))
	/*
	TRANSLATORS: If using the FTGL backend, this should be the path of a
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Debian it should be the Debian path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/truetype/uralic/gothu___.ttf"))
	// Mandrake
	/*
	TRANSLATORS: If using the FTGL backend, this should be the path of a bold
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Mandrake it should be the Mandrake path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF"))
	/*
	TRANSLATORS: If using the FTGL backend, this should be the path of a
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Mandrake it should be the Mandrake path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/TTF/uralic/GOTHU___.TTF"))

	// Check the non-translated versions of the above
	CHECK_FONT_PATH("/usr/share/fonts/truetype/uralic/gothub__.ttf")
	CHECK_FONT_PATH("/usr/share/fonts/truetype/uralic/gothu___.ttf")
	CHECK_FONT_PATH("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF")
	CHECK_FONT_PATH("/usr/share/fonts/TTF/uralic/GOTHU___.TTF")

	return font;
}

#endif // TEXT_FTGL
