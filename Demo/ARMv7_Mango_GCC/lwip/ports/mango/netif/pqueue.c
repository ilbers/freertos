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
