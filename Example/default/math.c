/*
 * Copyright (C) 2015-2016 ilbers GmbH
 *
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

#define DEFAULT_PRECISION	8	/* Maclaurin series length */

/* Maclaurin series for sin(x) */
float sineX(float x)
{
	float val = 1;
	int k;

	for (k = DEFAULT_PRECISION - 1; k >= 0; --k)
	{
		val = 1 - (x * x) / (2 * k + 2) / (2 * k + 3) * val;
	}

	return x * val;
}
