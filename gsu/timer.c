#include "su.h"
#define PERIODIC_TIMER_FUNC_PERIOD 20

#ifdef _FAR_MODE_
#include "watchdog.h"
#endif

unsigned long delay_counter = 0;

//system clock functions.
static unsigned long system_clock = 0l;
unsigned long clock_(void)
{
	unsigned long retval;
	
	retval = system_clock;
	system_clock = 0;
	return retval;
}

void timer_init(int timer)
{
   	unsigned int mask;
   	unsigned long counter;
   	
   	switch(timer)
   	{
   	case 0:
   		mask = TINT0;
   		counter = 49151l;		// was 49999l, now adjusted for 98.304 MHz clk
   		break;
   	case 1:
   		mask = TINT1;
   		counter = 5555l;
   	}
   	
   	INTR_DISABLE(mask);
    
    TCR(timer) = 0x0010; //stop.
    PRD(timer) = counter;
    TCR(timer) = 0x0020; //start & reload.
    
    INTR_ENABLE(mask);
}

unsigned int tint_1_count = 0;
interrupt void TINT_1_isr(void)
{
	tint_1_count++;
}

int periodic_timer_counter=PERIODIC_TIMER_FUNC_PERIOD;
interrupt void TINT_0_isr(void)
{
	//periodic timer
	if(periodic_timer_counter)
		periodic_timer_counter--;
	
	system_clock++;
}


/* delay = 1ms * duration */
void delay(unsigned long duration)
{
	int i; 
	unsigned long delay_counter;
	
	for(delay_counter=duration;delay_counter;delay_counter--)
	{
#ifdef _FAR_MODE_		
		watchdog_feed
#endif
		for(i=0;i<79;i++);
	}
	
}
 
#define	MAX_TIMERS	20 

typedef struct _timer_context
{
	unsigned	one_shot;
	unsigned	enabled;
	unsigned	valid;
	timer_service_func	service_func;
	void				*arg;
	unsigned short		timeout;
	unsigned short		period;
} TIMER_CONTEXT;


TIMER_CONTEXT	timer_table[MAX_TIMERS];
int				num_timers = 0;

void init_timer_table(void)
{
	memset(&timer_table[0], 0, sizeof(timer_table));
}

int timer_add(int *ret_timer_id,int one_shot,timer_service_func func,void *arg,unsigned short timeout)
{
	int				timer_id;
	TIMER_CONTEXT	*context;
	int				i, found;
		
	if(num_timers >= MAX_TIMERS)
	{
		return 0;
	}

	found = 0;
	for(i = 0; i < MAX_TIMERS; i++)
	{
		if(!timer_table[i].valid)
		{
			timer_id = i;
			found = 1;
			break;
		}
	}
	
	if(!found)
	{
		return 0;
	}	


	//INTR_DISABLE(TINT0);   
    num_timers++;
    
	context = &timer_table[timer_id];
	
	context->one_shot = one_shot;
	context->service_func = func;
	context->arg = arg;
	context->timeout = timeout; 
	context->period = timeout;

	context->enabled = 0; 
	context->valid = 1;
	
	if(ret_timer_id)
		(*ret_timer_id) = timer_id;

	//INTR_ENABLE(TINT0);      

    //DebugPrintf1("Timers used = ", num_timers);
		
	return 1;
}

int timer_delete(unsigned int timer_id)
{
		
	if(timer_id >= MAX_TIMERS)
	{
		return 0;
	}
	
	if(num_timers <= 0)
	{
		return 0;
	}

	//INTR_DISABLE(TINT0);   	
	timer_table[timer_id].valid = 0;
	num_timers--;
	//INTR_ENABLE(TINT0);	
	
    //DebugPrintf1("Timers used = ", num_timers);
	
	return 1;
}

int timer_enable(unsigned int timer_id, unsigned int timeout)
{
	
	if(timer_id >= MAX_TIMERS)
		return 0;

	if(!timer_table[timer_id].valid)
		return 0;
	
	//INTR_DISABLE(TINT0);   
	timer_table[timer_id].enabled = 1;
	timer_table[timer_id].timeout = timeout;
	timer_table[timer_id].period = timeout;
	//INTR_ENABLE(TINT0);
	
	return 1;
}

int timer_disable(unsigned int timer_id)
{	
	if(timer_id >= MAX_TIMERS)
		return 0;

	if(!timer_table[timer_id].valid)
		return 0;
			
	//INTR_DISABLE(TINT0);   
	timer_table[timer_id].enabled = 0;
	//INTR_ENABLE(TINT0);
	
	return 1;
}

int timer_set_callback(unsigned int timer_id, timer_service_func func, void *arg)
{
	TIMER_CONTEXT	*context;
	
	if(timer_id >= MAX_TIMERS)
		return 0;

	if(!timer_table[timer_id].valid)
		return 0;

	context = &timer_table[timer_id];
	
	//INTR_DISABLE(TINT0);   
	context->service_func = func;
	context->arg = arg;
	//INTR_ENABLE(TINT0);
	return 1;
}


int timer_set_one_shot(unsigned int timer_id, int one_shot)
{
	TIMER_CONTEXT	*context;
	
	if(timer_id >= MAX_TIMERS)
		return 0;

	if(!timer_table[timer_id].valid)
		return 0;

	context = &timer_table[timer_id];
	
	//INTR_DISABLE(TINT0);   	
	context->one_shot = one_shot;
	//INTR_ENABLE(TINT0);	
	return 1;
}

void periodic_timer_func(void)
{
	int	i;
	TIMER_CONTEXT	*context;
	
	if(periodic_timer_counter)
		return;
	INTR_DISABLE(TINT0);
	periodic_timer_counter=(PRD(0)>49000) ? PERIODIC_TIMER_FUNC_PERIOD : 2;
	INTR_ENABLE(TINT0);
	
	for(i = 0; i < MAX_TIMERS; i++)
	{
		context = &timer_table[i];
		
		if(!context->valid || !context->enabled)
			continue;
		
		context->timeout--;
		
		if(!context->timeout)
		{
			//DebugPrintf1("tcbf=",(int)(*context->service_func));
			if(context->service_func)
				(*context->service_func)(context->arg);
		                                                  
		                                                  
			if(!context->one_shot)
				context->timeout = context->period;
			else
	   			context->enabled = 0;
		}
	}
}	

