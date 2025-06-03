#include "su.h"
#include "watchdog.h"

#ifdef _MODEMS_
#include "modems.h"
#endif

#define Number_of_commands 177
extern int dce_state;
extern int fr_task_on;

#ifdef __DEBUG__

#ifdef _USE_SCREEN_AREA_IN_MEMORY_

#define SCREEN_Y 32
#define SCREEN_X 16
char screen[SCREEN_Y][SCREEN_X];
int sx=0;
int sy=0;

void advance_sy(void)
{
	int x,y;
	if((++sy)>=(SCREEN_Y-1))
	{
		sy=(SCREEN_Y-1);
		for(y=0;y<(SCREEN_Y-1);y++)
			for(x=0;x<SCREEN_X;x++)
				screen[y][x]=screen[y+1][x];
		for(x=0;x<SCREEN_X;x++)
			screen[SCREEN_Y-1][x]=' ';
	}	
}

void putscreen(char c)
{
	
	switch(c)
	{
	case '\r':
		sx=0;
		break;
	case '\n':
		advance_sy();
		break;
	default:
		screen[sy][sx]=c;
		if((++sx)>=80)
		{
			sx=0;
			advance_sy();
		}
	}
}

void putscreens(char *str)
{
	while(*str)
		putscreen(*str++);
}

#endif // _USE_SCREEN_AREA_IN_MEMORY_


static char Command_name[Number_of_commands][32]=
{
	"NOTYPE",
	"SLAC_interrupt", 
    "SLAC_all_trunks_busy", 
    "SLAC_error",
	"SLAC_on_hook",
    "SLAC_off_hook",
    "SLAC_dialing_params",
    "SLAC_init",
    "SLAC_unconditional_branch",
	"SLAC_ring",
	"SLAC_digit",
	"SLAC_busy",
	"SLAC_ring_back",
	"SLAC_connect",
	"SLAC_network_busy",
	"SLAC_busy_timeout",
	"SLAC_network_busy_timeout",
	"SLAC_no_sim_timeout",
	"SLAC_digit_timeout",
	"SLAC_first_digit_timeout",
	"SLAC_disconnect_timeout",
	"SLAC_ring_back_on_timeout",
	"SLAC_ring_back_off_timeout",
	"SLAC_ring_timeout",
	"SLAC_short_ring_timeout",
	"SLAC_roh_timeout",
	"SLAC_slic_off_timeout",
	"SLAC_SS_confirm",
	"SLAC_SS_failure",
	"SLAC_ssconfirm_timeout",
	"SLAC_ssfailure_timeout",
	"SLAC_crssfailure",
	"SLAC_disconnect",
	"SLAC_disconnect_held",
	"SLAC_dce_normal",
	"SLAC_dce_pump",
	"SLAC_no_service",
	"SLAC_no_sim",
    "SLAC_service_available",
    "SLAC_flash_timer",
    "SLAC_invalid_flash_timer",
    "SLAC_pulse_break_timer",
    "SLAC_inter_pulse_timer",
    "SLAC_call_waiting",
    "SLAC_call_waiting_sas_on",
    "SLAC_call_waiting_sas_off",
    "SLAC_cas_on",
    "SLAC_cas_off",
    "SLAC_ten_sec_cw_sas_on",
    "SLAC_caller_id",
    "SLAC_call_waiting_ack",
    "SLAC_cidcw_transmit",
    "SLAC_call_waiting_released",
    "SLAC_call_wait_unmute",
    "SLAC_call_held",
    "SLAC_call_active",
    "SLAC_ss_cancel",
    "SLAC_ss_prompt_timeout",
    "SLAC_ss_prompted",
    "SLAC_ss_cancel_timeout",
    "SLAC_crss_failed_timeout",
    "SLAC_ss_disc_ttone_timeout",
    "SLAC_neg_status",
    "SLAC_neg_status_timeout",
    "SLAC_disable_interrupt_timeout",
    "SLAC_cid_messages",
    "SLAC_send_cid_message",
    "SLAC_call_flashed",
 	"SLAC_send_dtmf_cid",
 	"SLAC_delay_dtmf_cid",
    "SLAC_done_dtmf_cid",
    "SLAC_delay_dtmf_gen",
    "SLAC_supv_conn",
    "SLAC_supv_disconn",
    "SLAC_supv_aoc",
    "SLAC_supv_continue",
	"AT_answer",
	"AT_busy",
	"AT_connect",
	"AT_dialing_params",
	"AT_disconnect",
	"AT_disconnect_held",
	"AT_disable_incoming_call",
	"AT_enable_incoming_call",
	"AT_error",
    "AT_ringing",
    "AT_SS_confirm",
    "AT_SS_failure",
    "AT_SS_neg_status",
    "AT_response",
    "AT_init_normal",
    "AT_init_data",
    "AT_restart",
    "AT_data_mode",
    "AT_command_mode",
    "AT_call_accept_waiting",
    "AT_call_hold",
    "AT_call_mpty",
    "AT_call_ect",
    "AT_call_active",
    "AT_call_held",
    "AT_inactive_disconnect",
    "AT_SS_flag",
    "AT_start_callmetering",
    "AT_stop_callmetering",
    "AT_show_callmetering",
    "DCE_change_state",
    "DCE_uart",
    "DCE_at_command",
    "DCE_pump",
    "FM_incoming_data",
    "FM_incoming_fax",
    "FM_outgoing_data",
    "FM_outgoing_fax",
    "FM_modem_type",
    "FM_start",
    "FM_stop",
	"FM_AT_FTH_3",
	"FM_AT_FRH_3",
	"FM_AT_FTS_8",
	"FM_AT_FTM_96",
	"FM_AT_FRM_96",
	"FR_init",
	"FR_start",
	"FR_outgoing",
	"FR_incoming",
    "FR_IN_modem_char",
    "FR_OUT_modem_char",
    "FR_OK_IN",
    "FR_OK_OUT",
    "FR_ERROR_IN",
    "FR_ERROR_OUT",
    "FR_CONNECT_IN",
    "FR_CONNECT_OUT",
    "FR_NO_CARRIER_IN",
    "FR_NO_CARRIER_OUT",
    "FR_DLE_ETX_IN",
    "FR_DLE_ETX_OUT",
    "FR_stop",
    "FR_COUNTER_OK",
    "FR_RINGBACK",
    "FR_BUSY",
    "FR_COUNTER_OVER",
    "FR_FAX_CALL",
    "FR_NSF_IN",
    "FR_NSF_OUT",
    "FR_CSI_IN",
    "FR_CSI_OUT",
    "FR_DIS_IN",
    "FR_DIS_OUT",
    "FR_TSI_IN",
    "FR_DCS_IN",
    "FR_DCN_IN",
    "FR_DEFAULT_IN",
    "FR_UNKNOWN_IN",
    "FR_TRANING_END",
    "FR_CFR_OUT",
    "FR_MCF_OUT",
    "FR_RTN_OUT",
    "FR_RTP_OUT",
    "FR_FTT_OUT",
    "FR_DEFAULT_OUT",
    "FR_PUMP_OUT",
    "FR_PUMP_IN",
    "FR_MPS_IN",
    "FR_EOM_IN",
    "FR_EOP_IN",
    "FR_TRANING_END_IN",
	"FR_TR_IN_OK",
	"FR_TRANING_IN_FAULT",
	"FR_FRS_OUT_START",
	"FR_NOT_LASTPAGE",
	"FR_BCS_START_OUT",
	"FR_TIMEOUT_IN",
	"FR_TIMEOUT_OUT",
    "FR_finished",
    "SYSTEM"
};
/*
void DebugPrintfCommandName(int cmd)
{
	if(cmd >= 0 && cmd < Number_of_commands)
	{
		DebugPrintf((char*)Command_name[cmd]);
	}
}
*/
#define TIME_EVENTS_DIM 128
typedef struct TIME_EVENTS_TAG
{
	int index;
	char descr[TIME_EVENTS_DIM][20];
	unsigned long time[TIME_EVENTS_DIM];
}
TIME_EVENTS;

TIME_EVENTS time_events;

int time_debug_initialized = 0;
void InitTimeDebug(void)
{
	time_debug_initialized = 1;
	time_events.index = 0;
	clock_();	
}

void TimeDebugSPrintf_(char *descr)
{
	int i;

	if(time_events.index >= TIME_EVENTS_DIM)
	{
		time_events.index = 0;
	}
	
	for(i=0;descr[i];i++)
	{
		time_events.descr[time_events.index][i] = descr[i];
	}
	time_events.descr[time_events.index][i]=0;
	
	time_events.time[time_events.index]=clock_();

	time_events.index++;
}


void TimeDebugDump(void)
{
	int i;
	
	if(time_debug_initialized)
	{
		time_debug_initialized = 0;
	
		if(dce_state == DCE_STATE_DEBUG)
		{
			for(i=0; i < time_events.index; i++)
			{
				DebugPrintf1("\n\rat ",time_events.time[i]);
				DebugPrintf(" : ");
				DebugPrintf(time_events.descr[i]);
			}
		}
	}
}




void DebugPrintf(char *str)
{
#ifdef _FAR_MODE_
	watchdog_feed
#endif
	
	if(dce_state ==	DCE_STATE_DEBUG)
	{
		uart_b_fputs(str);
	}
}

#ifdef _FAR_MODE_
#define UPPER_TETRAD 28
#else
#define UPPER_TETRAD 12
#endif

void DebugPrintf1(char *str, QUEUEVALUE val)
{
	int i;
	QUEUEVALUE j;
	const char hdsmbl[16]="0123456789abcdef";
	char s[80];
	for(i=0;s[i]=str[i];i++);
	for(j=UPPER_TETRAD;(j+4)!=0;j-=4)
		s[i++] = hdsmbl[(val>>j)& 0xf];
	s[i++]='\r';
	s[i++]='\n';
	s[i]=0;
	DebugPrintf(s);
}


char *handler_name(VFI handler)
{
	if(handler == CC)
		return "C:";
	if(handler == DCE)
		return "D:";
	if(handler == AT)
		return "A:";
	if(handler == SLAC)
		return "S:";
	if(handler == FM)
		return "M:";
	if(handler == FR)
		return "R:";
	if(handler == FR_IN)
		return "I:";
	if(handler == FR_OUT)
		return "O:";
		
	return 0;
}

void DebugMainMsgEntry(void)
{
	static VFI last_good_handler = 0;
	static int last_good_command = 0;
	
	VFI handler;
	int command;
	
	if(!queue_empty(&(queue[0])))
	{
		handler = (VFI)queue_get(&(queue[0]));
		
		if(handler_name(handler))
		{
			command = (int)queue_get(&(queue[0]));
	        
	        if
	        (	0
	        	//handler == CC
	        	//||handler == DCE
	        	//|| handler == SLAC
	        	//|| handler == AT
	        	//|| handler == FM
				//|| handler == FR
	        )
	        {   
		        DebugPrintf(handler_name(handler));
		        DebugPrintf((char*)Command_name[command]);
		        DebugPrintf("\r\n");
			}	        
	        
	        last_good_handler = handler;
	        last_good_command = command;
	        handler(command);
       	}
       	else
       	{
			INTR_GLOBAL_DISABLE;
			DebugPrintf1("wrong handler = ",(QUEUEVALUE)handler);
			if(last_good_command && last_good_handler)
			{
				DebugPrintf("Last good handler/command:\n\r");
		    	DebugPrintf(handler_name(last_good_handler));
		    	DebugPrintf((char*)Command_name[last_good_command]);
		    }
		    DebugPrintf("\r\nQueue Dump:\n\r");
			while(!queue_empty(&(queue[0])))
			{
				DebugPrintf1("# ",(QUEUEVALUE)(queue[0].nodes_taken));
				DebugPrintf1("> ",queue_get(&(queue[0])));
			}
			
	    	if(fr_task_on && (dce_state != DCE_STATE_PUMP))
    		{
				TimeDebugDump();
			}
			
			for(;;); //wait for watchdog to reset the module.
			//INTR_GLOBAL_ENABLE;
		}
	}
	//asm("	IDLE	1");
}

#endif // __DEBUG__

