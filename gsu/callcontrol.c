#include "su.h"

#ifdef _MODEMS_
#include "modems.h"
#endif

/* SendMessag parameter lists for all StateMachine branches */

static struct SendMsgList s0_to_s1[2] = 	{{AT,	AT_disable_incoming_call},
										 	{NOBODY, NOTYPE}
											};
static struct ToDoList do_s0_to_s1[2] =		{{SendMessageBunch, s0_to_s1},
											{0,0}
					  						};										
/*....*/
static struct SendMsgList s0_to_s2[2] = 	{{SLAC, SLAC_ring},
								  			{NOBODY, NOTYPE}
											};
static struct ToDoList do_s0_to_s2[4] =		{{StartPeriodicPASCheck,(void*)CPAS_PERIOD},
										 	{StopPeriodicCREGCheck,(void*)0},	
										 	{SendMessageBunch, s0_to_s2},
											{0,0}
					  						};										
/*--------------------------------------*/
static struct SendMsgList s1_to_s6[2] = 	{{AT,AT_dialing_params},
								  			{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s1_to_s6[3] =		{{StartPeriodicCLCCCheck, (void*)CPAS_PERIOD},
										 	{SendMessageBunch,s1_to_s6},
											{0,0}
					  						};
 static struct SendMsgList s1_to_s0[2] = 	{{AT,AT_enable_incoming_call},
								  			{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s1_to_s0[4] =		{{StopPeriodicPASCheck, 0},
											{StartPeriodicCREGCheck, 0},
											{SendMessageBunch,s1_to_s0},
											{0,0}
					  						};					  				
/*--------------------------------------*/
static struct SendMsgList s6_to_s0[4] = 	{{FM, FM_stop},
											{AT,	AT_stop_callmetering},
											{AT,AT_enable_incoming_call},
								  			{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s6_to_s0[5] =		{{StopPeriodicPASCheck, 0},
											{StartPeriodicCREGCheck, 0},
											{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s0},
											{0,0}
					  						};										

static struct SendMsgList s6_to_s1_0[2] = 	{{SLAC,	SLAC_SS_confirm},
											{NOBODY,NOTYPE}
											};
static struct ToDoList do_s6_to_s1_0[3] = 	{{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s1_0},
											{0,0}
					  			   			};

static struct SendMsgList s6_to_s1_1[2] = 	{{SLAC,	SLAC_SS_failure},
											{NOBODY,NOTYPE}
											};
static struct ToDoList do_s6_to_s1_1[3] = 	{{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s1_1},
											{0,0}
					  			   			};
					  			   																							
static struct SendMsgList s6_to_s1_2[2] = 	{{SLAC,	SLAC_neg_status},
											{NOBODY,NOTYPE}
											};
static struct ToDoList do_s6_to_s1_2[3] = 	{{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s1_2},
											{0,0}
					  			   			};
					  			   			
static struct SendMsgList s6_to_s5[3] =  	{{SLAC,	SLAC_connect},
								   			{SLAC,	SLAC_supv_conn},
								   			{NOBODY,	NOTYPE}
											};																				
static struct ToDoList do_s6_to_s5[5] =   { {StartPeriodicPASCheck,(void*)CPAS_PERIOD},
											{StopPeriodicCREGCheck,(void*)0},
											{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s5},
											{0,					0}	
					  			   			};

static struct SendMsgList s6_to_s5_1[2] = {{SLAC,	SLAC_call_active},
											{NOBODY,NOTYPE}
											};
static struct ToDoList do_s6_to_s5_1[3] = 	{{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s5_1},
											{0,0}
					  			   			};

static struct SendMsgList s6_to_s4_0[4] = 	{{FM, FM_stop},
											{SLAC,	SLAC_busy},
											{AT,	AT_stop_callmetering},
								  			{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s6_to_s4_0[3] = 	{{Reset_SS_setup, 0},
											{SendMessageBunch, s6_to_s4_0},
											{0,0}
					  			   			};										

static struct SendMsgList s6_to_s4_1[4] = 	{{FM, FM_stop},
											{SLAC,	SLAC_network_busy},
											{AT,	AT_stop_callmetering},
											{NOBODY, NOTYPE}
											};
static struct ToDoList do_s6_to_s4_1[3] = 	{{SendMessageBunch, s6_to_s4_1},
											{Reset_SS_setup, 0},
											{0,0}
					  			   			};
/*
static struct SendMsgList s6_to_s7[2] = 	{{SLAC,	SLAC_disconnect_held},
											{NOBODY, NOTYPE}
											};
static struct ToDoList do_s6_to_s7[3] = 	{{SendMessageBunch, s6_to_s7},
											{Reset_SS_setup, 0},
											{0,					0}
					  			   			};										

static struct SendMsgList s6_to_s6_5[2] = {{SLAC,	SLAC_ring_back},
									{NOBODY,NOTYPE}
										};
static struct ToDoList do_s6_to_s6_5[2] = {{&SendMessageBunch, s6_to_s6_5},
									{0,0}
					  			   };										
*/

/*--------------------------------------*/
static struct SendMsgList s2_to_s0[2] = 	{{SLAC,		SLAC_disconnect},
								  			{NOBODY,	NOTYPE}
											};
static struct ToDoList do_s2_to_s0[4] = 	{{StopPeriodicPASCheck,0},
										 	{StartPeriodicCREGCheck,0},
										 	{SendMessageBunch, s2_to_s0},
											{0,0}
					  			   			};										
static struct SendMsgList s2_to_s7[2] = 	{{SLAC,		SLAC_disconnect_held},
								  			{NOBODY,	NOTYPE}
											};
static struct ToDoList do_s2_to_s7[3] = 	{{SendMessageBunch, s2_to_s7},
											{0,0}
					  			   			};
static struct SendMsgList s2_to_s3[2] = 	{{AT, 		AT_answer},
											{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s2_to_s3[2] = 	{{SendMessageBunch, s2_to_s3},
											{0,0}
					  			   			};										

/*--------------------------------------*/
static struct SendMsgList s3_to_s0[3] = 	{{FM, FM_stop},
											{AT, 	AT_enable_incoming_call},
											{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s3_to_s0[4] = 	{{StopPeriodicPASCheck,0}, 
										 	{StartPeriodicCREGCheck,0},
										 	{SendMessageBunch, s3_to_s0},
											{0,0}
					  			   			};										

static struct SendMsgList s3_to_s4[3] = 	{{FM, FM_stop},
											{SLAC, 		SLAC_network_busy},
											{NOBODY, 	NOTYPE}
											};
static struct ToDoList do_s3_to_s4[4] = 	{{StopPeriodicPASCheck,0},
										 	{StartPeriodicCREGCheck,0},
										 	{SendMessageBunch, s3_to_s4},
											{0,0}
					  			   			};										

static struct SendMsgList s3_to_s5[2] = {{SLAC, 	SLAC_supv_conn},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s3_to_s5[2] = {{SendMessageBunch, s3_to_s5},
										{0,0}
					  			   		};										
static struct SendMsgList s3_to_s7[2] = {{SLAC, 	SLAC_disconnect_held},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s3_to_s7[3] = {{SendMessageBunch, s3_to_s7},
										{0,0}
					  			   		};
/*--------------------------------------*/
static struct SendMsgList s4_to_s0[2] = {{AT,AT_enable_incoming_call},
										{NOBODY,NOTYPE}
										};
static struct ToDoList do_s4_to_s0[4] = {{StopPeriodicPASCheck, 0},
										{StartPeriodicCREGCheck, 0},
										{SendMessageBunch, s4_to_s0},
										{0,0}
					  			   		};
static struct SendMsgList s4_to_s7[1] = {{NOBODY, 	NOTYPE}};
static struct ToDoList do_s4_to_s7[2] = {{SendMessageBunch, s4_to_s7},
										{0,0}
										};										

/*--------------------------------------*/
static struct SendMsgList s5_to_s0[5] = {{FM, 	FM_stop},
										 {SLAC, SLAC_supv_disconn},
										 {AT,	AT_stop_callmetering},
										 {AT,	AT_enable_incoming_call},
										 {NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s0[4] = {{StopPeriodicPASCheck,0},
										 {StartPeriodicCREGCheck, 0},
										 {SendMessageBunch, s5_to_s0},
										{0,0}
					  			   		};										
static struct SendMsgList s5_to_s7_1[2] = {{SLAC, 	SLAC_call_held},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s7_1[2] = {{SendMessageBunch, s5_to_s7_1},
										{0,0}
										};
static struct SendMsgList s5_to_s7_2[2] = {{SLAC, 	SLAC_disconnect_held},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s7_2[2] = {{SendMessageBunch, s5_to_s7_2},
										{0,0}
										};
static struct SendMsgList s5_to_s1[5] = {{FM, 		FM_stop},
										{SLAC, 		SLAC_supv_disconn},
										{SLAC, 		SLAC_disconnect},
										{AT,		AT_stop_callmetering},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s1[4] = {{StopPeriodicPASCheck,0},
										 {StartPeriodicCREGCheck,0},
										 {SendMessageBunch, s5_to_s1},
										{0,0}
										};										
static struct SendMsgList s5_to_s5[3] = {{SLAC, 	SLAC_call_active},
										{AT, 		AT_start_callmetering},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s5[2] = {{SendMessageBunch, s5_to_s5},
										{0,0}
										};
static struct SendMsgList s5_to_s4_1[5] = {{FM, 	FM_stop},
										{SLAC, 		SLAC_supv_disconn},
										{SLAC, 		SLAC_network_busy},
										{AT,		AT_stop_callmetering},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s4_1[4] = {{StopPeriodicPASCheck,0},
										{StartPeriodicCREGCheck,0},
										{SendMessageBunch, s5_to_s4_1},
										{0,0}
										};
static struct SendMsgList s5_to_s4_2[5] = {{FM, 	FM_stop},
										{SLAC, 		SLAC_supv_disconn},
										{SLAC, 		SLAC_busy},
										{AT,		AT_stop_callmetering},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s5_to_s4_2[4] = {{StopPeriodicPASCheck,0},
										{StartPeriodicCREGCheck,0},
										{SendMessageBunch, s5_to_s4_2},
										{0,0}
										};										
static struct SendMsgList ml_fm[2] = {{FM, FM_start},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_fm[2] = {{SendMessageBunch, ml_fm},
										{0,0}
										};
static struct SendMsgList s7_to_s6[2] = {{AT,AT_dialing_params},
								  {NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s7_to_s6[3] =	{{StartPeriodicCLCCCheck, (void*)CPAS_PERIOD},
										{SendMessageBunch,s7_to_s6},
										{0,0}
					  					};
static struct SendMsgList s7_to_s1[3] = {{AT,		AT_stop_callmetering},
										{SLAC, 		SLAC_disconnect},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s7_to_s1[4] = {{StopPeriodicPASCheck,0},
										 {StartPeriodicCREGCheck,0},
										 {SendMessageBunch, s7_to_s1},
										{0,0}
										};
static struct SendMsgList s7_to_s5[2] = {{SLAC, SLAC_call_active},
										{NOBODY, 	NOTYPE}};
static struct ToDoList do_s7_to_s5[2] = {{SendMessageBunch, s7_to_s5},
										{0,0}
					  			   		};
static struct SendMsgList s7_to_s0[3] = {{AT,	AT_stop_callmetering},
										{AT,AT_enable_incoming_call},
										{NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s7_to_s0[4] = {{StopPeriodicPASCheck,0},
										 {StartPeriodicCREGCheck,0},
										 {SendMessageBunch, s7_to_s0},
										{0,0}
					  			   		};
static struct SendMsgList s7_to_s4[3] = {{AT,		AT_stop_callmetering},
										{SLAC, 		SLAC_network_busy},
									    {NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s7_to_s4[4] = {{StopPeriodicPASCheck,0},
										 {StartPeriodicCREGCheck,0},
										 {SendMessageBunch, s7_to_s4},
										{0,0}
					  			   		};
static struct SendMsgList s7_to_s7[2] = {{SLAC, 	SLAC_call_held},
									    {NOBODY, 	NOTYPE}
										};
static struct ToDoList do_s7_to_s7[3] = {{SendMessageBunch, s7_to_s7},
										{0,0}
					  			   		};
static struct StateTable idle[3]  = {	{CC,	SLAC_off_hook,		1,	do_s0_to_s1},
										{CC,	AT_ringing,			2,	do_s0_to_s2},
										{NOBODY,NOTYPE,				0,	0}
								   		};
static struct StateTable setup[3] = {	{CC,	SLAC_dialing_params, 6,	do_s1_to_s6},
										{CC,	SLAC_on_hook, 		0,	do_s1_to_s0},
										{NOBODY,   	NOTYPE,	0,	0}
										};
static struct StateTable ringing[4]={	{CC,	AT_disconnect, 		0,	do_s2_to_s0},
										{CC,	AT_disconnect_held, 7,	do_s2_to_s7},
										{CC,	SLAC_off_hook,		3,	do_s2_to_s3},
										{NOBODY,			NOTYPE,     		0, 	0}
										};
static struct StateTable answering[7] ={{CC,	SLAC_on_hook,		0,	do_s3_to_s0},
								 		{CC,	AT_error,			4,	do_s3_to_s4},
								 		{CC,	AT_disconnect,		4,	do_s3_to_s4},
								 		{CC,	AT_disconnect_held,	7,	do_s3_to_s7},
								 		{CC,	AT_connect,			5,	do_s3_to_s5},
								 		{CC,	SLAC_connect,		3,	do_fm},
								 		{NOBODY,    	NOTYPE,     		0, 	0}
										};
static struct StateTable disconnected[3]= {{CC,	SLAC_on_hook,	0,	do_s4_to_s0},
										{CC, SLAC_call_flashed,	7,	do_s4_to_s7},
								   		{NOBODY,    		NOTYPE,     	0, 	0}
										};
static struct StateTable connected[9]=	{{CC,	SLAC_connect, 		5,	do_fm},
										{CC,	SLAC_on_hook,		0,	do_s5_to_s0},
										{CC,	AT_disconnect,		1,	do_s5_to_s1},
										{CC,	AT_disconnect_held,	7,	do_s5_to_s7_2},
								 		{CC,	AT_call_held,		7,	do_s5_to_s7_1},
								 		{CC,	AT_call_active,		5,	do_s5_to_s5},
								 		{CC,	AT_error,			4,	do_s5_to_s4_1},
								 		{CC,	AT_busy,			4,	do_s5_to_s4_2},
								 		{NOBODY,		NOTYPE,				0,	0}
									   	};
static struct StateTable alerting[11]= 	{{CC,	SLAC_on_hook,		0,	do_s6_to_s0},
								 		{CC,	AT_connect,			5,	do_s6_to_s5},
								 		{CC,	AT_call_active,		5,	do_s6_to_s5_1},
								 		{CC,	AT_busy,			4,	do_s6_to_s4_0},
								 		{CC,	AT_error,			4,	do_s6_to_s4_1},
								 		{CC,	AT_disconnect,		4,	do_s6_to_s4_1},
								 		{CC,	AT_disconnect_held,	4,	do_s6_to_s4_1},
								 		{CC,	AT_SS_confirm,		1,	do_s6_to_s1_0},
								 		{CC,	AT_SS_failure,		1,	do_s6_to_s1_1},
								 		{CC, 	AT_SS_neg_status,	1,	do_s6_to_s1_2},
								 		{NOBODY,    NOTYPE,     0, 0}
								  		};
static struct StateTable on_hold[7] = {	{CC,	SLAC_dialing_params, 6,	do_s7_to_s6},
										{CC,	AT_call_active,		 5, do_s7_to_s5},
										{CC,	AT_disconnect,		 1, do_s7_to_s1},
										{CC,	AT_error,		 	 4, do_s7_to_s4},
										{CC,	SLAC_on_hook, 		0,	do_s7_to_s0},
										{CC,	SLAC_connect, 		7,	do_s7_to_s7},
										{NOBODY,   	NOTYPE,	0,	0}
									};

static struct StateList stsList[10] = { 		idle,    		/* 0 - state*/
						 				setup,        /* 1 - state*/
						 				ringing,		/* 2 - state*/
						 				answering,    /* 3 - state*/
						 				disconnected, /* 4 - state*/
						 				connected,    /* 5 - state*/
						 				alerting,     /* 6 - state*/
						 				on_hold,      /* 7 */
						 				0
									  };
int CC_current_state;									  

void tsk_callcontrol(int inputMsgobj)
{
	int iSrc;
    int jfunct;
    switch(inputMsgobj)
    {
    default:
    	/*state machine starts here*/
    	iSrc = 0;
		while (stsList[CC_current_state].start[iSrc].dst != NOBODY)
		{
			/* Scan current State table */
			if ((stsList[CC_current_state].start[iSrc].type == inputMsgobj))
			{ 	
				/* Entry in State Table was found */
				jfunct = 0;
				/* Do All Function from ToDoList */  
				while(stsList[CC_current_state].start[iSrc].functlst[jfunct].service_func != 0)
				{
					stsList[CC_current_state].start[iSrc].functlst[jfunct].service_func(
							stsList[CC_current_state].start[iSrc].functlst[jfunct].arg);
					jfunct++;
				}
				/* Change State to nextState */
				CC_current_state = stsList[CC_current_state].start[iSrc].nextState;
#ifdef __DEBUG__
				DebugPrintf1("cc: state = ",(int)CC_current_state);
#endif
				break;			
			}
			iSrc++;
		}    
   	}
}


state_service_func SendMessageBunch(void *p)
{
	SendMsgList *list;
	int i;
	
	list = p;
	
	for(i=0;list[i].dst;i++)
		send0(list[i].dst,list[i].type);

	return 0;
}

