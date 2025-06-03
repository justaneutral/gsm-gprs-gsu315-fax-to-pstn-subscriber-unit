#include "su.h"

#define QLUA 256
#define QLUB 256
#define QLBSP0RX	400
#define QLBSP0TX	400

#ifdef _FAR_MODE_
#include "watchdog.h"
volatile ioport unsigned WATCH_DOG;
#endif

int queue0_fed_up = 0;

//#pragma DATA_SECTION(queue,".scratch")
//#pragma DATA_SECTION(ar_main,".scratch")
Queue queue[MAXQUEUES];
QUEUEVALUE ar_main[QLMAIN];
QUEUEVALUE ar_ua[QLUA];
QUEUEVALUE ar_ub[QLUB];
//QUEUEVALUE ar_bsp0rx[QLBSP0RX];
//QUEUEVALUE ar_bsp0tx[QLBSP0TX];

void init_all_queues(void)
{
	queue_init(&(queue[0]),ar_main,QLMAIN);
	queue_init(&(queue[1]),ar_ua,QLUA);
	queue_init(&(queue[2]),ar_ub,QLUB);
//	queue_init(&(queue[2]),ar_bsp0rx,QLBSP0RX);
//	queue_init(&(queue[3]),ar_bsp0tx,QLBSP0TX);
}


void queue_flush(PQueue pq)
{
	///
	pq->queue_tail = pq->queue_head = pq->queue;
	pq->nodes_taken = 0;
	///
}

void queue_ptr_advance(PQueue pq,QUEUEVALUE **queue_ptr)
{
	QUEUEVALUE *t;
	
	if((*queue_ptr) >= (&(pq->queue[(pq->queue_length)-1])))
	{
		(*queue_ptr) = pq->queue;
	}
	else
	{
		//such bad construction to avoid problems wit addr++
		//above 0x8000 called from uart a isr.
		t = *queue_ptr + 1;
		*queue_ptr = t;
	}	
}

int queue_ready_amaunt(PQueue pq, int length)
{
	if((pq->queue_length) - (pq->nodes_taken) >= length)
		return 1; //yes, the place is available.
	return 0; // no room.
}


void check_queue_overrun(PQueue pq)
{
	int i;
	
	if(pq->queue_tail == pq->queue_head) //overrun.
	{
		for(i=1;i<=3;i++)
			leds_update(i,orange,0);
#ifdef __DEBUG__
		DebugPrintf("Queue overrun\r\n");
#endif
#ifndef _FAR_MODE_
		c_int00();
#endif
		for(;;); //wait for watchdog.
	}
}

void queue_put(PQueue pq, QUEUEVALUE *data, int length)
{
	int i;
	int amt;
	
	amt = length? length:1;
	
	INTR_GLOBAL_DISABLE;
	
	if(!queue_ready_amaunt(pq, amt))
		return;
	
	if(!length)
	{
		queue_ptr_advance(pq,&(pq->queue_tail));
		pq->nodes_taken++;
		check_queue_overrun(pq);
		*(pq->queue_tail) = (QUEUEVALUE)data;
	}
	else
	{
		pq->nodes_taken += length;
		for(i=0;i<length;i++)
		{
			queue_ptr_advance(pq,&(pq->queue_tail));
			check_queue_overrun(pq);
			*(pq->queue_tail) = ((QUEUEVALUE*)data)[i];
		}
	}
	INTR_GLOBAL_ENABLE;
}

void queue_put_fast(PQueue pq, QUEUEVALUE *data)
{
	queue_ptr_advance(pq,&(pq->queue_tail));
	pq->nodes_taken++;
	*(pq->queue_tail) = (QUEUEVALUE)data;
}

int queue_empty(PQueue pq)
{
#ifdef _FAR_MODE_
	watchdog_feed
#endif
	if(pq->queue_head == pq->queue_tail || pq->nodes_taken <= 0)
		return 1;
	return 0;
}


QUEUEVALUE queue_get(PQueue pq)
{
	QUEUEVALUE ret_val;
	INTR_GLOBAL_DISABLE;
	if(pq->queue_head != pq->queue_tail)
	{
		queue_ptr_advance(pq,&(pq->queue_head));
		if(pq->nodes_taken > 0)
		{
			pq->nodes_taken--;
		}
		else
		{
			pq->nodes_taken = 0;
		}
    }
    ret_val = (*(pq->queue_head));
    INTR_GLOBAL_ENABLE;
    return ret_val;
}

QUEUEVALUE queue_peek(PQueue pq)
{
	QUEUEVALUE ret_val;
	QUEUEVALUE *t;
	
	INTR_GLOBAL_DISABLE;
	t = pq->queue_head;
	queue_ptr_advance(pq,&t);
    ret_val = (*(t));
    INTR_GLOBAL_ENABLE;
    return ret_val;
}

void queue_init(PQueue pq, QUEUEVALUE *array, int length)
{
	pq->queue_length = length;
	pq->nodes_taken = 0;
	pq->queue = pq->queue_tail = pq->queue_head = array;
}


//$$$$*API*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 
void send0(VFI handler, int val)
{
	QUEUEVALUE x[2];
	x[0]=(QUEUEVALUE)handler;
	x[1]=val;
	queue_put(&(queue[0]),(void*)x,2);
}

void send1(VFI handler,int command_type,int length, int *val)
{
	int i;
	QUEUEVALUE x[MSG_PACKET_LENGTH];
	
	x[0]=(QUEUEVALUE)handler;
	x[1]=(QUEUEVALUE)command_type;
	
	if(!length)
	{
		x[2]=(QUEUEVALUE)val;
		queue_put(&(queue[0]),(void*)x,3);		
	}
	else
	{
		for(i=0;i<length;i++)
			x[i+2]=val[i];
		queue_put(&(queue[0]),(void*)x,length+2);
	}
}


