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

#include <FreeRTOSConfig.h>
#include <stdarg.h>
#include <sys_print.h>
#include <mm.h>

#include "PTK.h"

#define ABS(x)	((x < 0) ? (-x) : (x))

struct picoGC picoDefaultGC;

unsigned char *fbuf;

void picoCreateGC(struct picoGC *gc)
{
	gc->foreground = 15;
	gc->background = 0;
	gc->font = &font_8x13;
	gc->lastx = -1;
	gc->lasty = -1;
}

void picoDrawPoint(struct picoGC *gc, int x, int y)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoDrawPointRaw(x, y, gc->foreground);
}

void picoDrawLine(struct picoGC *gc,int x1, int y1, int x2, int y2)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoDrawLineRaw(x1, y1, x2, y2, gc->foreground);
}

void picoDrawLineRaw(int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, i, e, x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if (ABS(dx) > ABS(dy))
	{
		if (dx < 0)
		{
			/* eventually exchange point 1 and 2*/
			int tmpx, tmpy;

			tmpx = x2;
			tmpy = y2;
			x2   = x1;
			y2   = y1;
			x1   = tmpx;
			y1   = tmpy;
			dx   = -dx;
			dy   =-dy;
		}

		if (dy < 0)
		{
			i = -1;
			dy = -dy;
		}
		else
		{
			i = 1;
		}

		e = dx / 2;
		y = y1;

		for (x = x1; x <= x2; x++)
		{
			if (e >= dx)
			{
				e -= dx;
				y += i;
			}

			e += dy;
			picoDrawPointRaw(x, y, color);
		}
	}
	else
	{
		if (dy < 0)
		{
			/* eventually exchange point 1 and 2*/
			int tmpx, tmpy;

			tmpx = x2;
			tmpy = y2;
			x2   = x1;
			y2   = y1;
			x1   = tmpx;
			y1   = tmpy;
			dx   = -dx;
			dy   = -dy;
		}

		if (dx < 0)
		{
			i = -1;
			dx = -dx;
		}
		else
		{
			i = 1;
		}

		e = dy / 2;
		x = x1;

		for (y = y1; y <= y2; y++)
		{
			if (e >= dy)
			{
				e -= dy;
				x += i;
			}

			e += dx;
			picoDrawPointRaw(x, y, color);
		}
	}
}

void picoDrawPixmap(struct picoGC *gc,
		   int x, int y, 
		   struct picoPixmap *pixmap)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoDrawPixmapRaw(x, y, pixmap, gc->foreground, gc->background);
}

void picoDrawText(struct picoGC *gc, int x, int y, char *txt)
{
	char *p, *q;
	int ch;
	char buf[80];

	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	while(*txt)
	{
		for(p = txt, q = buf; *p && *p != '\n'; p++, q++)
		{
			*q = *p;
		}

		*q = 0;
		picoDrawTextRaw(x, y, buf, gc->font, gc->foreground, gc->background);

		if (*p)
		{
			txt = p + 1;
		}
		else
		{
			txt = p;
		}

		y += (gc->font->height * 11) / 10;
	}
}

int picoTextWidth(struct picoFont *font, char *txt)
{
	int ch;
	int width;

	if (font == NULL)
	{
		return 0;
	}

	width = 0;
	while (*txt)
	{
		int w = 0;

		while ((ch = (unsigned char)*txt) && (ch != '\n'))
		{
			w += font->per_char[ch].width;
			txt++;
		} /* elihw ch */

		if (w > width)
		{
			width = w;
		}

		if (*txt == '\n')
		{
			txt++;
		}
	}

	return width;
}

int picoTextHeight(struct picoFont *font, char *txt)
{
	int h; 

	if (font == NULL)
	{
		return 0;
	}

	h = font->height;
	while (*txt)
	{
		if (*txt == '\n')
		{
			h += (font->height * 11) / 10;
		}

		txt++;
	}

	return h;
}

void picoDrawTextCentered(struct picoGC *gc, int x, int y, char *txt)
{
	char *p, *q;
	int ch;
	char buf[80];

	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	while (*txt)
	{
		for (p = txt, q = buf; *p && *p != '\n'; p++, q++)
		{
			*q =*p;
		}

		*q = 0;

		picoDrawTextRaw(x - picoTextWidth(gc->font, txt) / 2,
				y,
				buf,
				gc->font,
				gc->foreground,
				gc->background);

		if (*p)
		{
			txt = p + 1;
		}
		else
		{
			txt = p;
		}

		y += (gc->font->height * 11) / 10;
	}
}

void picoSetFont(struct picoGC *gc, struct picoFont *font)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	gc->font = font;
}

void picoSetForeground(struct picoGC *gc, int color)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	gc->foreground = color;
}

void picoSetBackground(struct picoGC *gc, int color)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	gc->background = color;
}

void picoDrawRect(struct picoGC *gc, int x, int y, int w, int h)
{
	int color;

	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	color = gc->foreground;
	w--;
	h--;

	picoDrawLineRaw(x, y, x + w, y, color);
	picoDrawLineRaw(x + w, y, x + w, y + h, color);
	picoDrawLineRaw(x + w, y + h, x, y + h, color);
	picoDrawLineRaw(x, y + h, x, y, color);
}

void picoFillRect(struct picoGC *gc, int x, int y, int w, int h)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoFillRectRaw(x, y, w, h, gc->foreground);
}

void picoReverseRect(struct picoGC *gc, int x, int y, int w, int h)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoReverseRectRaw(x, y, w, h);
}

void picoScroll(struct picoGC *gc, int x, int y, int w, int h, int delta)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	picoScrollRaw(x, y, w, h, delta, gc->background);
}


void picoTerminalCreate(struct picoTerminal *term,
			struct picoGC *gc,
			int x, int y, int wc, int hc)
{
	if (gc == NULL)
	{
		gc = &picoDefaultGC;
	}

	term->cursor_x = 0;
	term->cursor_y = 0;
	term->x = x;
	term->y = y;

	memcpy(&(term->gc), gc, sizeof(struct picoGC));

	term->fgc=gc->foreground;
	term->hc=hc;
	term->wc=wc;
	term->ch=gc->font->height;
	term->cw=gc->font->per_char['A'].width;
	term->h=hc*gc->font->height;
	term->w=wc*term->cw;
	term->esc_state=0;
	term->esc_len=0;

	picoFillRectRaw(x,y,term->w,term->h,gc->background);
}

void picoTerminalPutc(struct picoTerminal *term, int ch)
{
	/*
	 * Handling of escape sequence.
	 * esc_state==1:  Already rcvd escape character (ADE 27)
	 * esc_state==2:  Already rcvd <ESC>[
	 */  
	if (term->esc_state == 1)
	{
		switch(ch)
		{
		case '[':
			term->esc_state = 2;
			term->esc_len=0;
			break;
		default:
			term->esc_state = 0;
			break;
		}
	}
	else
	if (term->esc_state == 2)
	{
		switch(ch)
		{
		case 'A':
			/* Cursor up */
			if (term->cursor_y > 0)
			{
				term->cursor_y--;
			}
			term->esc_state = 0;
			break;
		case 'B':
			/* Cursor down */
			if (term->cursor_y < (term->hc - 1))
			{
				term->cursor_y++;
			}
			term->esc_state = 0;
			break;
		case 'C':
			/* Cursor right */
			if (term->cursor_x < (term->wc - 1))
			{
				term->cursor_x++;
			}
			term->esc_state = 0;
			break;
		case 'D':
			/* Cursor left */
			if (term->cursor_x > 0)
			{
				term->cursor_x--;
			}
			term->esc_state = 0;
			break;
		case 'm':
			if (term->esc_len == 2)
			{
				if (term->esc_buf[0] == '3')
				{
					/* Set foreground */
					if (term->esc_buf[1] == '7')
					{
						term->gc.foreground = term->fgc;
					}
					else
					{
						term->gc.foreground="\x00\x09\x0a\x0b\x0c\x0d\x0e\x08\x0f"[term->esc_buf[1] - '0'];
					}
					term->esc_state = 0;
				}
				else
				if (term->esc_buf[0] == '4')
				{
					/* Set foreground */
					term->gc.background="\x00\x09\x0a\x0b\x0c\x0d\x0e\x08\x0f"[term->esc_buf[1] - '0'];
					term->esc_state = 0;
				}
			}
		default:
			if (term->esc_len < 2)
			{
				term->esc_buf[term->esc_len++] = ch;
			}
			else
			{
				term->esc_state = 0;
			}
			break;
		}
	}
	else
	{
		switch(ch)
		{
		case 27: 
			/* <ESC> */
			term->esc_state = 1;
			break;
		case 13:
			/* <CR> */
			term->cursor_x = 0;
			break;
		case 10:
			/* <NL> */
			term->cursor_y++;
			break;
		case 8:
			/* <BS> */
			if (term->cursor_x > 0)
			{
				term->cursor_x--;
			}
			picoDrawTextRaw(term->x + term->cursor_x * term->cw,
					term->y + term->cursor_y * term->ch,
					" ",
					term->gc.font,
					term->gc.foreground,
					term->gc.background);
			break;
		default:
			{
				char chr[2];
				chr[0] = ch;
				chr[1] = 0;

				picoDrawTextRaw(term->x + term->cursor_x * term->cw,
						term->y + term->cursor_y * term->ch,
						chr,
						term->gc.font,
						term->gc.foreground,
						term->gc.background);

				term->cursor_x++;
				if (term->cursor_x == term->wc)
				{
					term->cursor_y++;
					term->cursor_x = 0;
				}
			}
		}
	}

	if (term->cursor_y >= term->hc)
	{
		picoScroll(&term->gc, term->x, term->y, term->w, term->h, term->ch);
		term->cursor_y--;
	}
}

void picoTerminalPrintf(struct picoTerminal *term, const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	char *p;

	va_start(ap, fmt);
	format_print_message(buf, 128, fmt, ap);
	va_end(ap);

	for (p=buf; *p; p++)
	{
		picoTerminalPutc(term,*p);
	}
}

void picoInit(void)
{
	int fd;
	int i;

	fbuf = (unsigned char *)MANGOFB_BASE_ADDR;

	picoCreateGC(&picoDefaultGC);
	picoDriverInit();
}
