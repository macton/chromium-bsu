/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */
#ifndef Text_h
#define Text_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * Base class upon which all text rendering calls are made.
 */
//====================================================================
class Text
{
public:

	Text();
	virtual ~Text();

	virtual void Render(const char*, const int = -1);
	virtual float Advance(const char*, const int = -1);

	virtual float LineHeight(const char* = " ", const int = -1);

};

#endif // Text_h
