/*
 * Copyright 2008 Tristan Heaven
 * Copyright 2008 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */

#ifndef Image_h
#define Image_h

//====================================================================
enum ImageMipMap { IMG_NOMIPMAPS, IMG_SIMPLEMIPMAPS, IMG_BUILDMIPMAPS };
enum ImageBlend { IMG_SOLID, IMG_ALPHA, IMG_BLEND1, IMG_BLEND2, IMG_BLEND3 };

class Image
{
public:

	static GLuint load(const char *filename, ImageMipMap mipmap = IMG_NOMIPMAPS, ImageBlend trans = IMG_ALPHA, GLint wrapst = GL_CLAMP, GLint minfilter = GL_LINEAR, GLint magfilter = GL_LINEAR);

private:
	Image();
};

#endif // Image_h
