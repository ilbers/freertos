/*
 * Copyright (C) 2015 ilbers GmbH
 *
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License** as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
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

#include <stdlib.h>
#include "netif/pqueue.h"

#define NUM_QUEUES	2

pq_queue_t pq_queue[NUM_QUEUES];

pq_queue_t *pq_create_queue()
{
	static int i;
	pq_queue_t *q = NULL;

	if (i >= NUM_QUEUES)
	{
		return q;
	}

	q = &pq_queue[i++];

	if (!q)
	{
		return q;
	}

	q->head = q->tail = q->len = 0;

	return q;
}

int pq_enqueue(pq_queue_t *q, void *p)
{
	if (q->len == PQ_QUEUE_SIZE)
	{
		return -1;
	}

	q->data[q->head] = p;
	q->head = (q->head + 1) % PQ_QUEUE_SIZE;
	q->len++;

	return 0;
}

void* pq_dequeue(pq_queue_t *q)
{
	int ptail;

	if (q->len == 0)
	{
		return NULL;
	}

	ptail = q->tail;
	q->tail = (q->tail + 1) % PQ_QUEUE_SIZE;
	q->len--;

	return q->data[ptail];
}

int 
pq_qlength(pq_queue_t *q)
{
	return q->len;
}
