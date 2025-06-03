#include "su.h"

#define MAX_AT_TIMERS 15
//typedef void (*timer_service_func)(void *);
typedef struct delayed_message_tag
{
	int in_use;
	int timer_id;
	QUEUEVALUE message[MSG_PACKET_LENGTH];
	int length;
} delayed_message;

delayed_message messages[MAX_AT_TIMERS];

void erase_delayed_messages(void)
{
	int i;
	for(i=0;i<MAX_AT_TIMERS;i++)
		messages[i].in_use = 0;
}

timer_service_func exec_post(void *p)
{
	//p is representing the index.
	queue_put(&(queue[0]),messages[(int)p].message,messages[(int)p].length);
	timer_delete(messages[(int)p].timer_id);
	messages[(int)p].in_use = 0;
	return 0;
}


send1_delayed(unsigned short delay, VFV handler, int command_type,int length, int *val)
{
	int index,i;
	
	for(index=0;index<MAX_AT_TIMERS;index++)
		if(!messages[index].in_use)
			break;
	
	if(index>=MAX_AT_TIMERS)
		return 0;
	
	messages[index].in_use = 1;
	messages[index].message[0]=(QUEUEVALUE)handler;
	messages[index].message[1]=(QUEUEVALUE)command_type;	
	
	if(length<0)
	{
		messages[index].length = 2;	
	}
	else
	if(length==0)
	{
		messages[index].length = 3;
		messages[index].message[2]=(QUEUEVALUE)(val);
	}
	else
	{
		messages[index].length = length+2;
		for(i=0;i<length;i++)
		{
			messages[index].message[i+2]=(QUEUEVALUE)(val[i]);
		}
	}
	
	if(!timer_add(&(messages[index].timer_id),
	   1,//one_shot
	   (timer_service_func)exec_post, 
	   (void *)index, 
	   delay))
	{
		messages[index].in_use = 0;
	 	return 0;
	}
		
	if(!timer_enable(messages[index].timer_id, delay))
	{
		timer_delete(messages[index].timer_id);
		messages[index].in_use = 0;
	 	return 0;
	}

	return 1; 	
}

