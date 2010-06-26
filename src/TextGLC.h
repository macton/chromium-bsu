/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef TextGLC_h
#define TextGLC_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#ifdef TEXT_GLC

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef HAVE_GL_GLC_H
#include <GL/glc.h>
#endif

#ifdef HAVE_QUESOGLC_GLC_H
#include <QuesoGLC/glc.h>
#endif

#include "Text.h"

/**
 * Use GLC for rendering text in OpenGL
 */
//====================================================================
class TextGLC : public Text
{
public:

	TextGLC();
	virtual ~TextGLC();

	virtual void Render(const char*, const int = -1);
	virtual float Advance(const char*, const int = -1);

	virtual float LineHeight(const char*, const int = -1);

private:
	GLint context;
	GLint defaultFont;
	GLint distroFont;
	GLint userFont;

};

#endif // TEXT_GLC

#endif // TextGLC_h
