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

#ifndef __RB_H__
#define __RB_H__

/* Trivial ring-buffer implementation */

#define RB_SIZE		290

typedef struct {
	int head;
	int count;
	int data[RB_SIZE];
} rb_t;

static inline void rb_init(rb_t *rb)
{
	rb->head = 0;
	rb->count = 0;
}

static inline void rb_push(rb_t *rb, int val)
{
	int size = sizeof(rb->data) / sizeof(int);
	int tail = (rb->head + rb->count) % size;

	rb->data[tail] = val;

	if (rb->count == size)
	{
		rb->head = (rb->head + 1) % size;
	}
	else
	{
		rb->count++;
	}
}

#endif /* __RB_H__ */
