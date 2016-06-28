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
#include "rb.h"

/* Pre-calculated Sin(x) table */
static float sine_table[63];

static rb_t sine1, sine2, sine3, sine4, sine5, sine6;

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

static void draw_sine(rb_t *rb, int x, int y, int color)
{
	struct picoGC gc;
	int size = sizeof(rb->data) / sizeof(int);
	int i;

	picoSetForeground(&gc, 0);
	picoDrawPoint(&gc, x, y + rb->data[rb->head]);

	for (i = 1; i < rb->count; i++)
	{
		int p = (rb->head + i) % size;

		picoSetForeground(&gc, 0);
		picoDrawPoint(&gc, x + i, y + rb->data[p]);
		picoSetForeground(&gc, color);
		picoDrawPoint(&gc, x + i - 1, y + rb->data[p]);
	}
}

static void demo_sine(void)
{
	struct picoGC gc;
	int i;

	picoSetForeground(&gc, 15);
	picoSetFont(&gc, &font_helv_m20);
	picoDrawTextCentered(&gc, 480, 312, "Demo: sin(x)");
	picoSetFont(&gc, &font_8x13);

	picoDrawRect(&gc, 330, 100, 300, 130);

	/* Flush sine buffers */
	rb_init(&sine1);
	rb_init(&sine2);
	rb_init(&sine3);
	rb_init(&sine4);
	rb_init(&sine5);
	rb_init(&sine6);

	for (i = 0; i < 1500; i++)
	{
		/* Y = 110 +/- 2 */
		rb_push(&sine1, sine_table[(i * 5) % 63] * 2);
		draw_sine(&sine1, 335, 110, 1);

		/* Y = 124 +/- 4 */
		rb_push(&sine2, sine_table[(i * 5) % 63] * 4);
		draw_sine(&sine2, 335, 124, 2);

		/* Y = 140 +/- 5 */
		rb_push(&sine3, sine_table[(i * 2) % 63] * 5);
		draw_sine(&sine3, 335, 140, 3);

		/* Y = 160 +/- 7 */
		rb_push(&sine4, sine_table[(i * 3) % 63] * 7);
		draw_sine(&sine4, 335, 160, 4);

		/* Y = 185 +/- 9 */
		rb_push(&sine5, sine_table[i % 63] * 9);
		draw_sine(&sine5, 335, 185, 5);

		/* Y = 210 +/- 10 */
		rb_push(&sine6, sine_table[(i * 2) % 63] * 10);
		draw_sine(&sine6, 335, 210, 6);

		vTaskDelay(configTICK_RATE_HZ / 100);
	}

	picoSetForeground(&gc, 0);
	picoFillRect(&gc, 330, 100, 300, 150);
	picoSetForeground(&gc, 7);
}

static void fbTask2(void *unused)
{
	int i;

	/* Build sine table */
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
