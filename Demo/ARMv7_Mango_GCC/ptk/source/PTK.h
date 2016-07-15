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

#ifndef PTK_H
#define PTK_H

#include <stdint.h>

#ifndef NULL
  #define NULL ((void *)0)
#endif /* NULL */

#define PTK_NO_COLOR	(-1)	/* Do no fill background */

/* picoTK uses graphic contexts (GC) to allow multiple tasks to have
 * their own environment. Most of the picoTK functions required to
 * be called with a gc parameter. If a NULL pointer is used instead
 * of a pointer to a GC the default GC is used.
 */
struct picoGC {
	int             foreground;
	int             background;
	int             lastx;
	int             lasty;
	struct picoFont *font;
};

struct picoFont {
	unsigned char phony_name[16];
	unsigned char height;
	unsigned char base_line;
	struct {
		unsigned char width;
		unsigned char offset_msb;
		unsigned char offset_lsb;
	} per_char[256];
	unsigned char data[1];
};

#define PICO_PIXMAP_1BPP	1

struct picoPixmap {
	unsigned char width_msb;
	unsigned char width_lsb;
	unsigned char height_msb;
	unsigned char height_lsb;
	unsigned char type;
	unsigned char data[1];
};

struct picoTerminal {
	int           x;		/* x offset window / pixels (e.g. 0) */
	int           y;		/* y offset window / pixels (e.g. 0) */
	int           cursor_x;		/* cursor pos x, 0..wc-1 */
	int           cursor_y;		/* cursor pos y, 0..hc-1 */
	int           w;		/* window width in pixels (e.g. 640) */
	int           h;		/* window height in pixels (e.g. 480) */
	int           wc;		/* window width in chars (e.g. 80)  */
	int           hc;		/* window height in chars (e.g. 24) */
	int           cw;		/* single char width (e.g. 8) */
	int           ch;		/* single char height (e.g. 13) */
	int           fgc;		/* default foreground color */
	struct picoGC gc;		/* local copy of gc */
	int           esc_state;	/* used for escape seq handling */
	char          esc_buf[5];
	int           esc_len;
	int           term_widget;
	int           cmdline_widget;
};

#define PICO_WG_ALLOCATED	(1 << 0)	/* Widget is allocated */

struct picoWidget {
	int flags;				/* Flags to indicate widget state */
	int x;					/* Top left corner X pos */
	int y;					/* Top left corner Y pos */
	int width;				/* Width of widget in pixels */
	int height;				/* Height of widget in pixels */
	int rcount;				/* Widget refresh counter */
};

struct picoControlBlock {
	int               magic;		/* MangoFB magic signature */
	int               screen_x;		/* Screen X resolution */
	int               screen_y;		/* Screen Y resolution */
	int               colors;		/* Bits per pixel */
	int               data_offset;		/* Raw FB data offset */
	int               max_nr_widgets;	/* Max nr widgets supported */
	int               nr_widgets;		/* Nr widgets allocated */
	struct picoWidget widgets[1];		/* Widgets data */
};

extern unsigned char *fbuf;
extern struct picoControlBlock *cb;

extern struct picoFont font_10x20;
extern struct picoFont font_9x15;
extern struct picoFont font_8x13;
extern struct picoFont font_8x16;
extern struct picoFont font_7x14;
extern struct picoFont font_6x12;
extern struct picoFont font_5x7;

extern struct picoFont font_helv_b34;
extern struct picoFont font_helv_m20;
extern struct picoFont font_helv_m25;
extern struct picoFont font_helv_m34;
extern struct picoFont font_school_m34;


/* Driver API:
 * Those functions are not to be called by the user program. As the interface
 *  might change.
 */

void picoDriverInit(void);
void picoDrawPointRaw(int x, int y, int color);
void picoFillRectRaw(int x, int y, int w, int h, int color);
void picoReverseRectRaw(int x, int y, int w, int h);
int  picoDrawTextRaw(int x, int y, char *txt,struct picoFont *font,
                     int fgcolor, int bgcolor);
int  picoDrawPixmapRaw (int x, int y, struct picoPixmap *pixmap,
                        unsigned char fgc, unsigned char bgc);
void  picoScrollXRaw(int x, int y, int w, int h, int delta,int bgc);
void  picoScrollYRaw(int x, int y, int w, int h, int delta,int bgc);

/* Generic API: */


/* picoInit() intializes picoTK and clears (color=0) the display
 * canvas. picoTK uses a canvas with an origin in the upper left
 * corner. One LSB in the coordinate system equals a single pixel.
 */
void picoInit(void);

/* picoCreateGC() fills an empty pico 'graphical context' with 
 * meaningful defaults. Graphical contexts are used in multitasking/
 * multithreading environments to keep individual states (active colors, 
 * active font, last recent line point, etc.) for each task.
 */  
void picoCreateGC(struct picoGC *gc);

/* picoDrawPoint() plots single pixel at position x, y using the
 * foreground color. (default 1 for 1bpp, 15 elsewise)  
 */
void picoDrawPoint(struct picoGC *gc, int x, int y);

/* picoDrawLine() draws one pixel wide line between (x1,y1) and
 * (x2,y2) using the foreground color
 */
void picoDrawLine(struct picoGC *gc,int x1, int y1, int x2, int y2);

/* Internally used. Do not use this function. */
void picoDrawLineRaw(int x1, int y1, int x2, int y2, int color);

/* picoDrawText() draws Text starting at x, y (upper left corner of 
 * rectangle around text). Use GC-specified foreground/background color 
 * for text display. Use the GC-specified font.
 */
void picoDrawText(struct picoGC *gc,int x, int y, char *txt);

/* picoDrawTextCentered() is similar to picoDrawText() but centered 
 * around x posistion, i.e. text extends to left and right of x to 
 * the same amount 
 */
void picoDrawTextCentered(struct picoGC *gc, int x, int y, char *txt);

/* picoTextWidth() calculates the textwidth in pixels giving a font 
 * and the text 
 */
int  picoTextWidth(struct picoFont *font, char *txt);

/* picoTextHeight() calculates textheight in pixels giving a font and 
 * the text 
 */
int  picoTextHeight(struct picoFont *font, char *txt);

/* picoDrawPixmap() draws pixmap at x,y (upper left corner of rectangle
 * around pixmap). Use foreground/background color for pixmap (monochrome
 * pixmaps only).
 */
void picoDrawPixmap (struct picoGC *gc, int x, int y, 
                     struct picoPixmap *pixmap);

/* picoDrawRect() draws Rectangle with an upper left corner at (x,y)
 * and lower right corner at (x+w-1,y+h-1)
 */
void picoDrawRect(struct picoGC *gc,int x, int y, int w, int h);

/* picoDrawRect() draws a filled Rectangle with an upper left corner at 
 * (x,y) and lower right corner at (x+w-1,y+h-1)
 */
void picoFillRect(struct picoGC *gc,int x, int y, int w, int h);


/* picoDrawRect() reverse  rectangular area with an upper left corner at 
 * (x,y) and lower right corner at (x+w-1,y+h-1). Reversing means inverting
 * every bit of the current color value.
 */
void picoReverseRect(struct picoGC *gc,int x, int y, int w, int h);

/* picoSroll() vertically scrolls area with an upper left corner at (x,y)
 * and a lower right corner at (x+w-1,y+h-1). The area is scrolled by delta
 * pixels. If delta is positive the area is scrolled up, elsewise it is
 * scrolled down 
 */
void picoScrollX(struct picoGC *gc, int x, int y, int w, int h, int delta);
void picoScrollY(struct picoGC *gc, int x, int y, int w, int h, int delta);

/* picoSetFont() sets font for given graphical context gc */
void picoSetFont(struct picoGC *gc, struct picoFont *font);

/* picoSetForeground() sets the foreground colot for the given graphical 
 * context gc
 */
void picoSetForeground(struct picoGC *gc, int color);

/* picoSetBackground() sets the background color for the given graphical 
 * context gc
 */
void picoSetBackground(struct picoGC *gc, int color);

/* picoTerminalCreate() creates a scrollable Terminal Window using 
 * the settings (font, foreground color, backgroundcolor) from the
 * gc. The font is required to have fixed spacing in order for the 
 * terminal to work correctly. (x/y) is the upper left corner of the
 * terminal window. wc (width in characters) is the number of columns.
 * hc (height in character) is the number of line. The actual size in
 * pixels is depends on the font size and is internally calculated. It
 * can be obtained after the call to picoTerminalCreate() by looking into
 * the term struct() - namely term->w, term->h.
 */

void picoTerminalCreate(struct picoTerminal *term,
                        struct picoGC *gc,
                        int x, int y, int wc, int hc);
/* picoTerminalPutc() types single character into given terminal. 
 * The terminal handles some special ASCII characters as such, including
 * \r (ADE 13) \n (ADE 10). 
 */
void picoTerminalPutc(struct picoTerminal *term, int ch);

/* picoTerminalPrintf() types characters to the terminal according to
 * the format string and the following parameters. It is compatible to
 * how POSIX printf() handles its format string. Special character are
 * interpreted by the terminal just as with picoTerminalPutc() 
 */
void picoTerminalPrintf(struct picoTerminal *term, const char *fmt, ...);

/* returns X display size */
int picoInfoSizeX(void); 

/* returns Y display size */
int picoInfoSizeY(void);

/* returns display color depth */
int picoInfoColorDepth(void);

/* Widgets API is used to optimize virtual framebuffer displaying.
 * The only specified parts of the screen will be refreshed, so
 * the overall CPU loading is reduced
 */

/* picoCreateWidget() allocates new widget */
int picoCreateWidget(int x, int y, int h, int w);
/* picoUpdateWidget() updates widget's refresh counter */
void picoUpdateWidget(int nr);
/* picoRemoveWidget() removes widget */
void picoRemoveWidget(int nr);

#endif

