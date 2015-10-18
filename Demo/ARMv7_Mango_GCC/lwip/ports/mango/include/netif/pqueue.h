#ifndef __LWIP_PBUF_QUEUE_H__
#define __LWIP_PBUF_QUEUE_H__

#define PQ_QUEUE_SIZE 4096

typedef struct {
	void *data[PQ_QUEUE_SIZE];
	int head, tail, len;
} pq_queue_t;

pq_queue_t* pq_create_queue();
int         pq_enqueue(pq_queue_t *q, void *p);
void*       pq_dequeue(pq_queue_t *q);
int         pq_qlength(pq_queue_t *q);

#endif /* __LWIP_PBUF_QUEUE_H__ */
