/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifndef extern_h
#define extern_h

#include <cstdio>

const char *dataLoc(const char *filename, bool doCheck = true);
const char *alterPathForPlatform(char *filename);
void printExtensions(FILE *fstream, const char* extstr_in);

#endif // extern_h
