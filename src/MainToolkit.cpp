/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MainToolkit.h"

#include "Global.h"

MainToolkit::MainToolkit(int, char **)
{
	game = Global::getInstance();
}

MainToolkit::~MainToolkit()
{
}


