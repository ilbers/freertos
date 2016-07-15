/*
 * Copyright (C) 2015-2016 ilbers GmbH
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * Based on picoTK
 * Copyright (C) Thomas Gallenkamp <tgkamp@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include "PTK.h"
#include "PTK_config.h"

#define PICO_X		(cb->screen_x)
#define PICO_Y		(cb->screen_y)

inline int picoInfoSizeX(void)
{
	return PICO_X;
}

inline int picoInfoSizeY(void)
{
	return PICO_Y;
}

inline int picoInfoColorDepth(void)
{
	return 8;
}

inline void picoDrawPointRaw(int x, int y, int color)
{
	fbuf[x + y * PICO_X] = color;
}

int picoDrawPixmapRaw(int x, int y,
		      struct picoPixmap *pixmap,
		      unsigned char fgc,
		      unsigned char bgc)
{
	unsigned char *dst;
	unsigned char *src;
	int height, width;
	int width_bytes;
	int xi, yi;
	int xm8,xm8m7;

	/* We can only do depth 1 pixmaps */
	if (pixmap->type != PICO_PIXMAP_1BPP)
	{
		return 0;
	}

	dst = fbuf + (y * PICO_X) + x;
	height = (pixmap->height_msb * 256) + pixmap->height_lsb;
	width = (pixmap->width_msb * 256) + pixmap->width_lsb;
	width_bytes = (width + 7) / 8;
	src = pixmap->data;

	for (yi = 0; yi < height; yi++)
	{
		unsigned char *p;
		p = dst;

		for (xi=0; xi < width; )
		{
			unsigned char data;
			int i;

			data =* (src++);
			for (i = 0; (i < 8) && (xi < width); xi++, i++)
			{
				*(p++) = ((data << i) & 0x80) ? fgc : bgc;
			} /* i */
		} /* xi */

		dst += PICO_X;
	}
}

void picoFillRectRaw(int x, int y, int w, int h, int color)
{
	unsigned char *dst;
	int xt;
	int xi;

	xt = x + w - 1;
	for (h += y; y < h; y++)
	{
		unsigned char mask;

		dst = fbuf + (y * PICO_X) + x;
		for (xi = x; xi <= xt; xi++)
		{
			*(dst++) = color;
		}
	}
}

void picoReverseRectRaw(int x, int y, int w, int h)
{
	unsigned char *dst;
	int xt;
	int xi;

	xt = x + w - 1;
	for (h += y; y < h; y++)
	{
		unsigned char mask;

		dst = fbuf + (y * PICO_X) + x;
		for (xi = x; xi <= xt; xi++)
		{
			*(dst++) ^= 255;
		}
	}
}

int picoDrawTextRaw(int x, int y,
		    char *txt,
		    struct picoFont *font,
		    int fgc, int bgc)
{
	int ch;
	int cont = 1;
	int x_orig = x;

	if (font == NULL)
	{
		return 0;
	}

	while (ch = (unsigned char)*txt)
	{
		int height, width;
		int width_in_bytes;
		int yi, xi, xp;
		unsigned char *src;
		unsigned char *dst;
		unsigned char lrmask;
		unsigned char xm2;
		unsigned char src_data;

		height = font->height;
		width = font->per_char[ch].width;

		if ((x + width) > PICO_X)
		{
			cont = 0;
			break;
		}

		width_in_bytes = (width + 7) / 8;
		src = font->data + (font->per_char[ch].offset_msb * 256) + font->per_char[ch].offset_lsb;
		dst = fbuf + (y * PICO_X) + x;

		/* generic code */
		xm2 = x & 1;
		for (yi = 0; yi < height; yi++)
		{
			unsigned char *p;
			p = dst;

			for (xi = 0; xi < width; )
			{
				unsigned char data;
				int i;

				data = *(src++);
				for (i = 0; (i < 8) && (xi < width); xi++, i++)
				{
					*(p++) = ((data << i) & 0x80) ? fgc : bgc;
				} /* rof i */
			} /* rof xi */

			dst += PICO_X;
		} /* rof yi */

		x += width;
		txt++;
	} /* elihw ch */

	return x - x_orig;
}

void picoScrollXRaw(int x, int y, int w, int h, int delta, int bgc)
{
	volatile uint8_t *src;
	volatile uint8_t *dst;
	int xi, yi;

	if (delta > 0)
	{

		for (xi = 0; xi < w; xi++)
		{
			for (yi = 0; yi < h; yi++)
			{
				src = fbuf + (y + yi) * PICO_X + x + xi;
				dst = src - delta;
				*dst = *src;
			}
		}

		if (bgc != PTK_NO_COLOR)
		{
			/* TODO */
		}
	}
	else
	{
		/* TODO */
	}
}

void picoScrollYRaw(int x, int y, int w, int h, int delta, int bgc)
{
	unsigned char *src;
	unsigned char *dst;
	int xi, yi, xt;

	xt = x + w - 1;

	if (delta > 0)
	{
		dst = fbuf + (y * PICO_X);
		src = dst + (delta * PICO_X);

		for (yi = 0; yi < (h - delta); yi++)
		{
			for (xi = x; xi <= xt; xi++)
			{
				dst[xi] = src[xi];
			}

			dst += PICO_X;
			src += PICO_X;
		}

		if (bgc != PTK_NO_COLOR)
		{
			for (; yi < h; yi++)
			{
				for (xi = x; xi <= xt; xi++)
				{
					dst[xi] = bgc;
				}

				dst += PICO_X;
			}
		}
	}
	else
	{
		/* not yet tested ... */
		dst = fbuf + ((y + h - 1) * PICO_X);
		src = dst - (delta * PICO_X);

		for (yi = h - 1; yi >= (h - delta); yi--)
		{
			for (xi = x; xi <= xt; xi++)
			{
				dst[xi] = src[xi];
			}

			dst += PICO_X / 2;
			src += PICO_X / 2;
		}

		if (bgc != PTK_NO_COLOR)
		{
			for (; yi <= 0; yi--)
			{
				for (xi = x; xi <= xt; xi++)
				{
					dst[xi] = bgc;
				}

				dst += PICO_X / 2;
			}
		}
	}
}

void picoDriverInit(void)
{
	/* Clear Screen */
	picoFillRectRaw(0, 0, PICO_X, PICO_Y, 0);
}
