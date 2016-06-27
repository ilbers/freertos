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
#include <FreeRTOS.h>
#include <mango.h>
#include <sys_print.h>
#include <task.h>
#include <PTK.h>

#include "fb_text.h"
#include "fb_pixmap.h"

/* Pre-calculated Sin(x) table */
static float sine_table[63];

extern float sineX(float x);

static void demo_colors()
{
	struct picoGC gc;
	int i, j;

	picoCreateGC(&gc);
	picoSetForeground(&gc, 15);
	picoSetFont(&gc, &font_helv_m20);
	picoDrawTextCentered(&gc, 480, 312, "Demo: Available colors");
	picoSetFont(&gc, &font_8x13);

	for (j = 0; j < 16; j++)
	{
		for (i = 0; i < 16; i++)
		{
			int xo, yo, col;
			char buf[10];

			xo = 345 + i * 17;
			yo = 10 + j * 17;
			col = i + j * 16;

			picoSetForeground(&gc, col);
			picoFillRect(&gc, xo, yo, 16, 16);
			picoSetForeground(&gc, col == 0 ? 15 : 0);
			picoSetBackground(&gc, col);
			sys_print_to_str(buf, 10, "%2x", col);
			picoDrawText(&gc, xo, yo + 1, buf);
		}
	}

	vTaskDelay(configTICK_RATE_HZ * 10);
	picoSetForeground(&gc, 0);
	picoFillRect(&gc, 321, 0, 319, 334);
	picoSetForeground(&gc, 7);
}

static void demo_time(void)
{
	struct picoGC gc;
	int i;

	picoCreateGC(&gc);
	picoSetForeground(&gc, 15);
	picoSetFont(&gc, &font_helv_m20);
	picoDrawTextCentered(&gc, 480, 312, "Demo: Running time");
	picoSetFont(&gc, &font_8x13);

	picoSetFont(&gc,&font_helv_b34);

	for (i = 0; i < 10; i++)
	{
		volatile int t = mango_partition_run_time() / 1000;
		char buf[10];
		int s, m, h;
		int xs, ys;

		/* Get hours */
		h = t / (60 * 60);
		t -= h * 60 * 60;

		/* Get minutes */
		m = t / 60;
		t -= m * 60;

		/* Get seconds */
		s = t;

		sys_print_to_str(buf, 10, "%2d:%2d:%2d", h, m, s);

		xs = picoTextWidth(gc.font, buf);
		ys = picoTextHeight(gc.font, buf);

		picoDrawRect(&gc, 410 - 2, 170 - 2, xs + 4, ys + 4);
		picoDrawText(&gc, 410, 170, buf);

		vTaskDelay(configTICK_RATE_HZ);
	}

	picoSetForeground(&gc, 0);
	picoFillRect(&gc, 350, 165, 250, 200);
	picoSetForeground(&gc, 7);
}

static void demo_sine(void)
{
	struct picoGC gc;
	int i;

	picoSetForeground(&gc, 15);
	picoSetFont(&gc, &font_helv_m20);
	picoDrawTextCentered(&gc, 480, 312, "Demo: sin(x)");
	picoSetFont(&gc, &font_8x13);

	picoDrawRect(&gc, 330, 100, 300, 100);

	for (i = 290; i < 2000; i++)
	{
		int x;
		float y1, y2;

		x = 625;
		y1 = 130 + sine_table[i % 63] * 10;
		y2 = 170 + sine_table[(i * 2) % 63] * 4;

		picoScrollX(&gc, 335 + 1, 120, 291, 20, 1);
		picoScrollX(&gc, 335 + 1, 165, 291, 10, 1);

		picoSetForeground(&gc, 2);
		picoDrawPoint(&gc, x, y1);
		picoSetForeground(&gc, 4);
		picoDrawPoint(&gc, x, y2);

		vTaskDelay(configTICK_RATE_HZ / 200);
	}

	picoSetForeground(&gc, 0);
	picoFillRect(&gc, 330, 100, 300, 100);
	picoSetForeground(&gc, 7);
}

static void fbTask2(void *unused)
{
	int i;

	for (i = 0; i < 63; i++)
	{
		sine_table[i] = sineX((float)i / 10);
	}

	while (1)
	{
		demo_colors();
		demo_time();
		demo_sine();
	}
}

static void demo_term(void)
{
	struct picoGC gc;
	struct picoTerminal term1;

	picoCreateGC(&gc);

	picoSetForeground(&gc, 0);
	picoSetBackground(&gc, 9);
	picoDrawPixmap(&gc, 10, 10, (struct picoPixmap *)pixmap_lotos);

	picoSetForeground(&gc, 7);
	picoSetBackground(&gc, 0);
	picoSetFont(&gc, &font_8x13);
	picoTerminalCreate(&term1, &gc, 40, 100, 24, 16);
	picoDrawRect(&gc, term1.x - 1, term1.y - 1, term1.w + 4, term1.h + 2);

	while(1)
	{
		char *p;

		p = termtxt;
		while(*p)
		{
			picoTerminalPrintf(&term1, "%c", *(p++));
			vTaskDelay(configTICK_RATE_HZ / 20);
		}     
	}
}

void fbTask(void *unused)
{
	struct picoGC gc;
	portBASE_TYPE ret;

	picoInit();

	picoCreateGC(&gc);
	picoSetForeground(&gc,7);
	picoSetFont(&gc,&font_helv_m25);

	picoDrawText(&gc,
		     10,
                     400,
		     "Actually this is running under\n"
		     "FreeRTOS in another partition.\n"
		     );

	ret = xTaskCreate(fbTask2,
			  (portCHAR *)"fbTask2",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating second FB task, code (%d)\r\n", ret);
		return;
	}

	demo_term();

	for (;;);
}
