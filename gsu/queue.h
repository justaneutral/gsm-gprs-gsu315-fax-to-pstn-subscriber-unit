#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct Queue_tag
{
	QUEUEVALUE *queue;
	int queue_length;
	int nodes_taken;
	QUEUEVALUE *queue_head;
	QUEUEVALUE *queue_tail;
} Queue, *PQueue;

#define MAXQUEUES 3
extern Queue queue[MAXQUEUES];

void init_all_queues(void);
void queue_init(PQueue, QUEUEVALUE*,int);
void queue_flush(PQueue);
void queue_put(PQueue,QUEUEVALUE *,int);
int queue_empty(PQueue);
QUEUEVALUE queue_get(PQueue);

// unprotected queue put, no check for overrun and interrupts are enabled
void queue_put_fast(PQueue pq, QUEUEVALUE *data);

#endif // __QUEUE_H__
