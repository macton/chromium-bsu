/*
 * Copyright 2008 Tristan Heaven
 * Copyright 2008 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if IMAGE_SDL

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#elif IMAGE_GLPNG

#ifdef HAVE_APPLE_OPENGL_FRAMEWORK
#include <OpenGL/gl.h>
#include <glpng/glpng.h>
#else
#include <GL/gl.h>
#include <GL/glpng.h>
#endif

#endif

#include "Image.h"

// Load image from file and return the texture id
//====================================================================
GLuint Image::load(const char *filename, ImageMipMap mipmap, ImageBlend trans, GLint wrapst, GLint minfilter, GLint magfilter)
{
	GLuint texture = 0;

#if IMAGE_SDL

	SDL_Surface *image = IMG_Load(filename);
	if (!image)
	{
		fprintf(stderr, "Failed to load %s: %s\n", filename, IMG_GetError());
		return texture;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapst);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapst);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);

	// For the blend types, we need to convert to RGBA
	if( trans != IMG_SOLID && trans != IMG_ALPHA )
	{
		SDL_Surface* tmp;
		SDL_PixelFormat rgba;
		rgba.palette = NULL; rgba.colorkey = 0; rgba.alpha = 0;
		rgba.BitsPerPixel = 32; rgba.BytesPerPixel = 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rgba.Rmask = 0xFF000000; rgba.Rshift = 0; rgba.Rloss = 0;
		rgba.Gmask = 0x00FF0000; rgba.Gshift = 8; rgba.Gloss = 0;
		rgba.Bmask = 0x0000FF00; rgba.Bshift = 16; rgba.Bloss = 0;
		rgba.Amask = 0x000000FF; rgba.Ashift = 24; rgba.Aloss = 0;
#else
		rgba.Rmask = 0x000000FF; rgba.Rshift = 24; rgba.Rloss = 0;
		rgba.Gmask = 0x0000FF00; rgba.Gshift = 16; rgba.Gloss = 0;
		rgba.Bmask = 0x00FF0000; rgba.Bshift = 8; rgba.Bloss = 0;
		rgba.Amask = 0xFF000000; rgba.Ashift = 0; rgba.Aloss = 0;
#endif

		tmp = SDL_ConvertSurface(image, &rgba, SDL_SWSURFACE|SDL_SRCALPHA);

		if( tmp )
		{
			Uint16 pitch = tmp->pitch;
			Uint8 bpp = tmp->format->BytesPerPixel;
			int w = tmp->w, h = tmp->w;
			Uint32 a;
			Uint8* p;

			SDL_LockSurface( tmp );

			// This stuff probably faster than using SDL_GetRGBA and SDL_MapRGBA
			// Since we are using RGBA here, it works fine
			switch ( trans )
			{
				case IMG_BLEND1:
					for(int y = 0; y < h; y++){
						for( int x = 0; x < w; x++ ){
							p = (Uint8 *)tmp->pixels + y*tmp->pitch + x*bpp;
							// a = r+g+b (clipped to 0xFF)
							a = p[0]+p[1]+p[2];
							if (a > 0xFF) p[3] = 0xFF; else p[3] = a;
						}
					}
					break;
				case IMG_BLEND2:
					for(int y = 0; y < h; y++){
						for( int x = 0; x < w; x++ ){
							p = (Uint8 *)tmp->pixels + y*tmp->pitch + x*bpp;
							// a = (r+g+b)/2 (clipped to 0xFF)
							a = p[0]+p[1]+p[2];
							if (a > 0x1FE) p[3] = 0xFF; else p[3] = a/2;
						}
					}
					break;
				case IMG_BLEND3:
					for(int y = 0; y < h; y++){
						for( int x = 0; x < w; x++ ){
							p = (Uint8 *)tmp->pixels + y*tmp->pitch + x*bpp;
							// a = (r+g+b)/3
							a = p[0]+p[1]+p[2];
							p[3] = a/3;
						}
					}
					break;
				default:
					fprintf(stderr, "Bad trans type loading %s\n", filename);
			}

			SDL_UnlockSurface( tmp );

			SDL_FreeSurface( image );

			image = tmp;
		}
	}

	GLenum format;
	switch ( image->format->BytesPerPixel )
	{
		case 4: format = GL_RGBA; break;
		case 3: format = GL_RGB; break;
		case 2: format = GL_LUMINANCE_ALPHA; break;
		default: format = GL_LUMINANCE; break;
	}

	SDL_LockSurface( image );

	GLint pack, unpack;
	glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	switch ( mipmap )
	{
		case IMG_NOMIPMAPS:
			glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
			break;
		case IMG_SIMPLEMIPMAPS:
			gluBuild2DMipmaps(GL_TEXTURE_2D, image->format->BytesPerPixel, image->w, image->h, format, GL_UNSIGNED_BYTE, image->pixels);
			break;
		case IMG_BUILDMIPMAPS:
			gluBuild2DMipmaps(GL_TEXTURE_2D, image->format->BytesPerPixel, image->w, image->h, format, GL_UNSIGNED_BYTE, image->pixels);
			break;
		default:
			fprintf(stderr, "Bad mipmap type loading %s\n", filename);
	}

	glPixelStorei(GL_PACK_ALIGNMENT, pack);
	glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

	SDL_UnlockSurface( image );

	SDL_FreeSurface( image );

#elif IMAGE_GLPNG

	int glpng_mipmap = PNG_NOMIPMAPS;
	switch ( mipmap )
	{
		case IMG_NOMIPMAPS: glpng_mipmap = PNG_NOMIPMAPS; break;
		case IMG_SIMPLEMIPMAPS: glpng_mipmap = PNG_SIMPLEMIPMAPS; break;
		case IMG_BUILDMIPMAPS: glpng_mipmap = PNG_BUILDMIPMAPS; break;
		default:
			fprintf(stderr, "Bad mipmap type loading %s\n", filename);
	}

	int glpng_trans = PNG_ALPHA;
	switch ( trans )
	{
		case IMG_SOLID: glpng_trans = PNG_SOLID; break;
		case IMG_ALPHA: glpng_trans = PNG_ALPHA; break;
		case IMG_BLEND1: glpng_trans = PNG_BLEND1; break;
		case IMG_BLEND2: glpng_trans = PNG_BLEND2; break;
		case IMG_BLEND3: glpng_trans = PNG_BLEND3; break;
		default:
			fprintf(stderr, "Bad trans type loading %s\n", filename);
	}

	texture = pngBind(filename, glpng_mipmap, glpng_trans, NULL, wrapst, minfilter, magfilter);
	if (!texture)
		fprintf(stderr, "Failed to load %s\n", filename);

#endif

	return texture;
}
