/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef TextFTGL_h
#define TextFTGL_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef TEXT_FTGL

#include <FTGL/ftgl.h>

#include "Text.h"

/**
 * Use FTGL for rendering text in OpenGL
 */
//====================================================================
class TextFTGL : public Text
{
public:

	TextFTGL();
	virtual ~TextFTGL();

	virtual void Render(const char*, const int = -1);
	virtual float Advance(const char*, const int = -1);

	virtual float LineHeight(const char*, const int = -1);

private:
	FTFont *ftFont;
	const char* fontFile;

	const char* findFont();

};

#endif // TEXT_FTGL

#endif // TextFTGL_h
