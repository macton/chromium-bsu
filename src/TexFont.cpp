
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */
#include "compatibility.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glu.h>
#include "TexFont.h"

#if 0
/* Uncomment to debug various scenarios. */
#undef GL_VERSION_1_1
#undef GL_EXT_texture_object
#undef GL_EXT_texture
#endif

#ifndef GL_VERSION_1_1
#if defined(GL_EXT_texture_object) && defined(GL_EXT_texture)
#define glGenTextures glGenTexturesEXT
#define glBindTexture glBindTextureEXT
#ifndef GL_INTENSITY4
#define GL_INTENSITY4 GL_INTENSITY4_EXT
#endif
int useLuminanceAlpha = 0;
#else
/* Intensity texture format not in OpenGL 1.0; added by the EXT_texture
extension and now part of OpenGL 1.1. */
int useLuminanceAlpha = 1;
#endif
#else
int useLuminanceAlpha = 0;
#endif

/* byte swap a 32-bit value */
#define SWAPL(x, n) { \
n = ((char *) (x))[0];\
((char *) (x))[0] = ((char *) (x))[3];\
((char *) (x))[3] = n;\
n = ((char *) (x))[1];\
((char *) (x))[1] = ((char *) (x))[2];\
((char *) (x))[2] = n; }

/* byte swap a short */
#define SWAPS(x, n) { \
n = ((char *) (x))[0];\
    ((char *) (x))[0] = ((char *) (x))[1];\
                        ((char *) (x))[1] = n; }

int	testWidth = 0;
int testHeight = 0;

/*------------------------------------------------------------------*/
static TexGlyphVertexInfo *getTCVI(TexFont * txf, int c)
{
	TexGlyphVertexInfo *tgvi;
	
	/* Automatically substitute uppercase letters with lowercase if not
	   uppercase available (and vice versa). */
	if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range))
	{
		tgvi = txf->lut[c - txf->min_glyph];
		if (tgvi)
		{
			return tgvi;
		}
		if (islower(c))
		{
			c = toupper(c);
			if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range))
			{
				return txf->lut[c - txf->min_glyph];
			}
		}
		else if (isupper(c))
		{
			c = tolower(c);
			if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range))
			{
				return txf->lut[c - txf->min_glyph];
			}
		}
	}
	return getTCVI(txf, '?');
	fprintf(stderr, "texfont: tried to access unavailable font character \"%c\" (%d)\n",
	        isprint(c) ? c : ' ', c);
	abort();
	/* NOTREACHED */
}

static const char *lastError;

/*------------------------------------------------------------------*/
const char *txfErrorString(void)
{
	return lastError;
}

/*------------------------------------------------------------------*/
TexFont *txfLoadFont(const char *filename)
{
	TexFont *txf;
	FILE *file;
	GLfloat w, h, xstep, ystep;
	char fileid[4], tmp;
	unsigned char *texbitmap;
	int min_glyph, max_glyph;
	int endianness, swap, format, stride, width, height;
	int i, j, got;
	GLfloat	s_adj, t_adj;

	short tgi_x	= 0;
	short tgi_y	= 0;
	unsigned char	tgi_width	= 0;
	unsigned char	tgi_height	= 0;
	signed char		tgi_advance	= 0;
	
	txf = NULL;
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		lastError = "file open failed.";
		goto error;
	}
	txf = (TexFont *) malloc(sizeof(TexFont));
	if (txf == NULL)
	{
		lastError = "out of memory.";
		goto error;
	}
	/* For easy cleanup in error case. */
	txf->texobj		= 0;
	txf->tgi		= NULL;
	txf->tgvi		= NULL;
	txf->lut		= NULL;
	txf->teximage	= NULL;

	got = fread(fileid, 1, 4, file);
	if (got != 4 || strncmp(fileid, "\377txf", 4))
	{
		lastError = "not a texture font file.";
		goto error;
	}
	assert(sizeof(int) == 4);  /* Ensure external file format size. */
	got = fread(&endianness, sizeof(int), 1, file);
	if (got == 1 && endianness == 0x12345678)
	{
		swap = 0;
	}
	else if (got == 1 && endianness == 0x78563412)
	{
		swap = 1;
	}
	else
	{
		lastError = "not a texture font file.";
		goto error;
	}
#define EXPECT(n) if (got != n) { lastError = "premature end of file."; goto error; }
	got = fread(&format, sizeof(int), 1, file);
	EXPECT(1);
	got = fread(&txf->tex_width, sizeof(int), 1, file);
	EXPECT(1);
	got = fread(&txf->tex_height, sizeof(int), 1, file);
	EXPECT(1);
	got = fread(&txf->max_ascent, sizeof(int), 1, file);
	EXPECT(1);
	got = fread(&txf->max_descent, sizeof(int), 1, file);
	EXPECT(1);
	got = fread(&txf->num_glyphs, sizeof(int), 1, file);
	EXPECT(1);

	if (swap)
	{
		SWAPL(&format, tmp);
		SWAPL(&txf->tex_width, tmp);
		SWAPL(&txf->tex_height, tmp);
		SWAPL(&txf->max_ascent, tmp);
		SWAPL(&txf->max_descent, tmp);
		SWAPL(&txf->num_glyphs, tmp);
	}
	txf->tgi = (TexGlyphInfo *) malloc(txf->num_glyphs * sizeof(TexGlyphInfo));
	if (txf->tgi == NULL)
	{
		lastError = "out of memory.";
		goto error;
	}
	assert(sizeof(TexGlyphInfo) == 12);  /* Ensure external file format size. */
	got = fread(txf->tgi, sizeof(TexGlyphInfo), txf->num_glyphs, file);
	EXPECT(txf->num_glyphs);

	if (swap)
	{
		for (i = 0; i < txf->num_glyphs; i++)
		{
			SWAPS(&txf->tgi[i].c, tmp);
			SWAPS(&txf->tgi[i].x, tmp);
			SWAPS(&txf->tgi[i].y, tmp);
		}
	}
	txf->tgvi = (TexGlyphVertexInfo *)
	            malloc(txf->num_glyphs * sizeof(TexGlyphVertexInfo));
	if (txf->tgvi == NULL)
	{
		lastError = "out of memory.";
		goto error;
	}
	w = txf->tex_width;
	h = txf->tex_height;
	xstep = 0.5 / w;
	ystep = 0.5 / h;
	
	txf->max_width	= 0;
	txf->max_height	= 0;
	
	//-- move s and t --
	s_adj = -0.001;
	t_adj = -0.0;
	
	for (i = 0; i < txf->num_glyphs; i++)
	{
		TexGlyphInfo *tgi;
		tgi = &txf->tgi[i];
		if(tgi->width > txf->max_width)
			txf->max_width = tgi_width;
		if(tgi->height > txf->max_height)
			txf->max_height = tgi->height;

		//Give a little extra padding...
		tgi_x		= tgi->x-1;
		tgi_y		= tgi->y-2;
		tgi_width	= tgi->width+2;
		tgi_height	= tgi->height+2;
		tgi_advance	= tgi->advance+1;

		txf->tgvi[i].t0[0] = tgi_x / w + xstep 					+ s_adj;
		txf->tgvi[i].t0[1] = tgi_y / h + ystep 					+ t_adj;
		txf->tgvi[i].v0[0] = tgi->xoffset;
		txf->tgvi[i].v0[1] = tgi->yoffset;
		txf->tgvi[i].t1[0] = (tgi_x + tgi_width) / w + xstep	+ s_adj;
		txf->tgvi[i].t1[1] = tgi_y / h + ystep 					+ t_adj;
		txf->tgvi[i].v1[0] = tgi->xoffset + tgi_width;
		txf->tgvi[i].v1[1] = tgi->yoffset;
		txf->tgvi[i].t2[0] = (tgi_x + tgi_width) / w + xstep	+ s_adj;
		txf->tgvi[i].t2[1] = (tgi_y + tgi_height) / h + ystep	+ t_adj;
		txf->tgvi[i].v2[0] = tgi->xoffset + tgi_width;
		txf->tgvi[i].v2[1] = tgi->yoffset + tgi_height;
		txf->tgvi[i].t3[0] = tgi_x / w + xstep					+ s_adj;
		txf->tgvi[i].t3[1] = (tgi_y + tgi_height) / h + ystep	+ t_adj;
		txf->tgvi[i].v3[0] = tgi->xoffset;
		txf->tgvi[i].v3[1] = tgi->yoffset + tgi_height;
		txf->tgvi[i].advance = tgi_advance;
	}

	min_glyph = txf->tgi[0].c;
	max_glyph = txf->tgi[0].c;
	for (i = 1; i < txf->num_glyphs; i++)
	{
		if (txf->tgi[i].c < min_glyph)
		{
			min_glyph = txf->tgi[i].c;
		}
		if (txf->tgi[i].c > max_glyph)
		{
			max_glyph = txf->tgi[i].c;
		} 
	}
	txf->min_glyph = min_glyph;
	txf->range = max_glyph - min_glyph + 1;

	txf->lut = (TexGlyphVertexInfo **)
	           calloc(txf->range, sizeof(TexGlyphVertexInfo *));
	if (txf->lut == NULL)
	{
		lastError = "out of memory.";
		goto error;
	}
	for (i = 0; i < txf->num_glyphs; i++)
	{
		txf->lut[txf->tgi[i].c - txf->min_glyph] = &txf->tgvi[i];
	}

/*#ifdef RIVA_GLX*/
/*	fprintf(stderr, "useLuminanceAlpha = %d", useLuminanceAlpha);	*/
	useLuminanceAlpha = 1;
/*#endif */

	switch (format)
	{
	case TXF_FORMAT_BYTE:
		fprintf(stderr, "TXF_FORMAT_BYTE was removed because\n");
		fprintf(stderr, "it didn't do padding properly.\n");
	case TXF_FORMAT_BITMAP:
		width = txf->tex_width;
		height = txf->tex_height;
		stride = (width + 7) >> 3;
		texbitmap = (unsigned char *) malloc(stride * height);
		if (texbitmap == NULL)
		{
			lastError = "out of memory.";
			goto error;
		}
		got = fread(texbitmap, 1, stride * height, file);
		EXPECT(stride * height);
		if (useLuminanceAlpha)
		{
			txf->teximage = (unsigned char *) calloc(width * height * 2, 1);
			if (txf->teximage == NULL)
			{
				lastError = "out of memory.";
				goto error;
			}
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					if (texbitmap[i * stride + (j >> 3)] & (1 << (j & 7)))
					{
						txf->teximage[(i * width + j) * 2] = 255;
						txf->teximage[(i * width + j) * 2 + 1] = 255;
					}
				}
			}
		}
		else
		{
			testWidth	= width;
			testHeight	= height;
			txf->teximage = (unsigned char *) calloc(width * height, 1);
			if (txf->teximage == NULL)
			{
				lastError = "out of memory.";
				goto error;
			}
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					if (texbitmap[i * stride + (j >> 3)] & (1 << (j & 7)))
					{
						txf->teximage[i * width + j] = 255;
					}
				}
			}
		}
		free(texbitmap);
		break;
	}

	fclose(file);
	return txf;

error:

	if (txf)
	{
		if (txf->tgi)
			free(txf->tgi);
		if (txf->tgvi)
			free(txf->tgvi);
		if (txf->lut)
			free(txf->lut);
		if (txf->teximage)
			free(txf->teximage);
		free(txf);
	}
	if (file)
		fclose(file);
	return NULL;
}

/*------------------------------------------------------------------*/
GLuint txfEstablishTexture( TexFont * txf, GLuint texobj, GLboolean setupMipmaps)
{
	if (txf->texobj == 0)
	{
		if (texobj == 0)
		{
			glGenTextures(1, &txf->texobj);
		}
		else
		{
			txf->texobj = texobj;
		}
	}
	glBindTexture(GL_TEXTURE_2D, txf->texobj);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (useLuminanceAlpha)
	{
		if (setupMipmaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE_ALPHA,
			                  txf->tex_width, txf->tex_height,
			                  GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, txf->teximage);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA,
			             txf->tex_width, txf->tex_height, 0,
			             GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, txf->teximage);
		}
	}
	else
	{
		/* Use GL_INTENSITY4 as internal texture format since we want to use as
		   little texture memory as possible. */
		if (setupMipmaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY4,
			                  txf->tex_width, txf->tex_height,
			                  GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->teximage);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY4,
			             txf->tex_width, txf->tex_height, 0,
			             GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->teximage);
		}
	}

	return txf->texobj;
}

/*------------------------------------------------------------------*/
void txfBindFontTexture( TexFont * txf)
{
//fprintf(stderr, "txfBindFontTexture(%p) txf->texobj = %d || glIsTexture = %d\n", txf, txf->texobj, glIsTexture(txf->texobj));
	glBindTexture(GL_TEXTURE_2D, txf->texobj);
}

/*------------------------------------------------------------------*/
void txfUnloadFont( TexFont * txf)
{
	if (txf->teximage)
	{
		free(txf->teximage);
	}
	free(txf->tgi);
	free(txf->tgvi);
	free(txf->lut);
	free(txf);
}

/*------------------------------------------------------------------*/
void txfGetStringMetrics( TexFont * txf, char *string, int len, int *width, int *max_ascent, int *max_descent)
{
	TexGlyphVertexInfo *tgvi;
	int w, i;

	w = 0;
	for (i = 0; i < len; i++)
	{
		if (string[i] == 27)
		{
			switch (string[i + 1])
			{
			case 'M':
				i += 4;
				break;
			case 'T':
				i += 7;
				break;
			case 'L':
				i += 7;
				break;
			case 'F':
				i += 13;
				break;
			}
		}
		else
		{
			tgvi = getTCVI(txf, string[i]);
			w += (int)tgvi->advance;
		}
	}
	*width = w;
	*max_ascent = txf->max_ascent;
	*max_descent = txf->max_descent;
}

/*------------------------------------------------------------------*/
void txfRenderFixedGlyph(TexFont * txf, int c)
{
	float	adv;
	TexGlyphVertexInfo *tgvi;

	if(c < 0)
		return;
	tgvi = getTCVI(txf, c);
	adv = ((txf->max_width*TXF_FIXED_SPACING)-tgvi->advance)/2.0;
	glTranslatef(adv, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2fv(tgvi->t0);
	glVertex2sv(tgvi->v0);
	glTexCoord2fv(tgvi->t1);
	glVertex2sv(tgvi->v1);
	glTexCoord2fv(tgvi->t2);
	glVertex2sv(tgvi->v2);
	glTexCoord2fv(tgvi->t3);
	glVertex2sv(tgvi->v3);
	glEnd();
	glTranslatef(adv+tgvi->advance, 0.0, 0.0);
}

/*------------------------------------------------------------------*/
float txfRenderGlyph(TexFont * txf, int c)
{
	TexGlyphVertexInfo *tgvi;

	tgvi = getTCVI(txf, c);
	glBegin(GL_QUADS);
	glTexCoord2fv(tgvi->t0);
	glVertex2sv(tgvi->v0);
	glTexCoord2fv(tgvi->t1);
	glVertex2sv(tgvi->v1);
	glTexCoord2fv(tgvi->t2);
	glVertex2sv(tgvi->v2);
	glTexCoord2fv(tgvi->t3);
	glVertex2sv(tgvi->v3);
	glEnd();
	glTranslatef(tgvi->advance, 0.0f, 0.0f);
	return tgvi->advance;
}

/*------------------------------------------------------------------*/
GLfloat txfStringLength(TexFont * txf, const char *string, int len)
{
	int 	i;
	float	length = 0.0;
	TexGlyphVertexInfo *tgvi;
	for (i = 0; i < len; i++)
	{
		tgvi = getTCVI(txf, string[i]);
		length += tgvi->advance;
	}
	return length;
}

/*------------------------------------------------------------------*/
GLfloat txfStringHeight(TexFont * txf)
{
	return txf->max_ascent - txf->max_descent;
}

/*------------------------------------------------------------------*/
GLfloat txfFontMaxHeight(TexFont * txf)
{
	return txf->max_height;
}

/*------------------------------------------------------------------*/
GLfloat txfFontMaxWidth(TexFont * txf)
{
	return txf->max_width;
}

/*------------------------------------------------------------------*/
float txfRenderString( TexFont * txf, const char *string, int len)
{
	int i;
	float adv = 0.0f;
	for (i = 0; i < len; i++)
	{
		adv += txfRenderGlyph(txf, string[i]);
	}
	return adv;
}

/*------------------------------------------------------------------*/
void txfRenderFixedString( TexFont * txf, char *string, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		txfRenderFixedGlyph(txf, string[i]);
	}
}

enum {
    MONO, TOP_BOTTOM, LEFT_RIGHT, FOUR
};

/*------------------------------------------------------------------*/
void txfRenderFancyString( TexFont * txf, char *string, int len)
{
	TexGlyphVertexInfo *tgvi;
	GLubyte c[4][3];
	int mode = MONO;
	int i;

	for (i = 0; i < len; i++)
	{
		if (string[i] == 27)
		{
			switch (string[i + 1])
			{
			case 'M':
				mode = MONO;
				glColor3ubv((GLubyte *) & string[i + 2]);
				i += 4;
				break;
			case 'T':
				mode = TOP_BOTTOM;
				memcpy(c, &string[i + 2], 6);
				i += 7;
				break;
			case 'L':
				mode = LEFT_RIGHT;
				memcpy(c, &string[i + 2], 6);
				i += 7;
				break;
			case 'F':
				mode = FOUR;
				memcpy(c, &string[i + 2], 12);
				i += 13;
				break;
			}
		}
		else
		{
			switch (mode)
			{
			case MONO:
				txfRenderGlyph(txf, string[i]);
				break;
			case TOP_BOTTOM:
				tgvi = getTCVI(txf, string[i]);
				glBegin(GL_QUADS);
				glColor3ubv(c[0]);
				glTexCoord2fv(tgvi->t0);
				glVertex2sv(tgvi->v0);
				glTexCoord2fv(tgvi->t1);
				glVertex2sv(tgvi->v1);
				glColor3ubv(c[1]);
				glTexCoord2fv(tgvi->t2);
				glVertex2sv(tgvi->v2);
				glTexCoord2fv(tgvi->t3);
				glVertex2sv(tgvi->v3);
				glEnd();
				glTranslatef(tgvi->advance, 0.0, 0.0);
				break;
			case LEFT_RIGHT:
				tgvi = getTCVI(txf, string[i]);
				glBegin(GL_QUADS);
				glColor3ubv(c[0]);
				glTexCoord2fv(tgvi->t0);
				glVertex2sv(tgvi->v0);
				glColor3ubv(c[1]);
				glTexCoord2fv(tgvi->t1);
				glVertex2sv(tgvi->v1);
				glColor3ubv(c[1]);
				glTexCoord2fv(tgvi->t2);
				glVertex2sv(tgvi->v2);
				glColor3ubv(c[0]);
				glTexCoord2fv(tgvi->t3);
				glVertex2sv(tgvi->v3);
				glEnd();
				glTranslatef(tgvi->advance, 0.0, 0.0);
				break;
			case FOUR:
				tgvi = getTCVI(txf, string[i]);
				glBegin(GL_QUADS);
				glColor3ubv(c[0]);
				glTexCoord2fv(tgvi->t0);
				glVertex2sv(tgvi->v0);
				glColor3ubv(c[1]);
				glTexCoord2fv(tgvi->t1);
				glVertex2sv(tgvi->v1);
				glColor3ubv(c[2]);
				glTexCoord2fv(tgvi->t2);
				glVertex2sv(tgvi->v2);
				glColor3ubv(c[3]);
				glTexCoord2fv(tgvi->t3);
				glVertex2sv(tgvi->v3);
				glEnd();
				glTranslatef(tgvi->advance, 0.0, 0.0);
				break;
			}
		}
	}
}

/*------------------------------------------------------------------*/
int txfInFont(TexFont * txf, int c)
{
/*	TexGlyphVertexInfo *tgvi; */

	/* NOTE: No uppercase/lowercase substituion. */
	if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range))
	{
		if (txf->lut[c - txf->min_glyph])
		{
			return 1;
		}
	}
	return 0;
}
