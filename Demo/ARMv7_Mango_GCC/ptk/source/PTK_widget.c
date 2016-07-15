/*
 * Copyright (C) 2015-2016 ilbers GmbH
 * Alexander Smirnov <asmirnov@ilbers.de>
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

#include "PTK.h"
#include "PTK_config.h"

int picoCreateWidget(int x, int y, int w, int h)
{
	int n;
	struct picoWidget *wg;

	if (cb->nr_widgets == MAX_NR_WIDGETS)
	{
		return -1;
	}

	for (n = 0; n < MAX_NR_WIDGETS; n++)
	{
		wg = &cb->widgets[n];

		if (wg->flags == 0)
		{
			wg->flags |= PICO_WG_ALLOCATED;
			wg->x      = x;
			wg->y      = y;
			wg->width  = w;
			wg->height = h;
			wg->rcount = 0;

			cb->nr_widgets++;

			break;
		}
	}

	return n;
}

void picoRemoveWidget(int nr)
{
	struct picoWidget *wg;

	if (nr >= MAX_NR_WIDGETS)
	{
		return;
	}

	wg = &cb->widgets[nr];

	if (wg->flags & PICO_WG_ALLOCATED)
	{
		wg->flags  = 0;

		if (cb->nr_widgets > 0)
		{
			cb->nr_widgets--;
		}
	}
}

void picoUpdateWidget(int nr)
{
	struct picoWidget *wg;

	wg = &cb->widgets[nr];

	wg->rcount++;
}
