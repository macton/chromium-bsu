/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "Ground.h"

#include "Global.h"

//==============================================================================
Ground::Ground()
{
	game = Global::getInstance();
}

Ground::~Ground()
{
}

void Ground::nextVariation()
{
	setVariation(variation+1);
}
