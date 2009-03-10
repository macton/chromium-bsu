/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "ScreenItem.h"

#include <cstdio>


int ScreenItem::itemCount = 0;

//====================================================================
ScreenItem::ScreenItem(ItemType t)
{
	screenItemType = t;
	ScreenItem::itemCount++;
	ScreenItem::init();
}

ScreenItem::~ScreenItem()
{
	itemCount--;
//	fprintf(stderr, _("ScreenItem::itemCount = %d\n"), ScreenItem::itemCount);
}

//----------------------------------------------------------
void ScreenItem::init()
{
	pos[0] = 0.0;
	pos[1] = 0.0;
	pos[2] = 0.0;
	age = 0;
	back = next = 0;
}

//----------------------------------------------------------
float	*ScreenItem::getPos()
{
	return pos;
}
