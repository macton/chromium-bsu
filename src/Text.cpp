/*
 * Copyright 2009 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Text.h"

using namespace std;

//====================================================================
Text::Text()
{
}

Text::~Text()
{
}

void Text::Render(const char*, const int)
{
}

float Text::Advance(const char*, const int)
{
	return 0;
}

float Text::LineHeight(const char*, const int)
{
	return 0;
}
