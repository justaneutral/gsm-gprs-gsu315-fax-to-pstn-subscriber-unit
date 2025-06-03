#include <std.h>
#include "su.h"
#include "modems.h"
#include "duart_ti16c752.h"           
#include "fr_frames.h"


#define TEST

//char frame_DCS[6] = {0x5,0x13,0x83,0x0,0x06,0x78};

static int ind_OK_IN=0;
static int ind_CONNECT_IN = 0;
static int ind_NOCAR_IN = 0;
static int ind_ERROR_IN = 0;

static int ind_OK_OUT=0;
static int ind_CONNECT_OUT = 0;
static int ind_NOCAR_OUT = 0;
static int ind_ERROR_OUT = 0;
static int ind_BUSY_OUT = 0;

static int ind_WIND2 = 0;
static int stop_flag = 0;


int fr_state_in = 0;
int fr_state_out = 0;

extern int clcc_check_enabled;
extern int pas_check_enabled;

int	uart_a_in_event;
int	uart_c_in_event;

int fr_direction;

extern int fr_task_on;
extern int pump_page;
extern int fr_stop_flag;
extern int page_recv_flag;


MODEM In;
MODEM Out;


void check_and_set_uart_a_rts()
{
	if(queue0_fed_up && (queue_ready_amaunt(&(queue[0]),QLMAIN_HIGH)))  ///can be a bug, but must be checked now. 2/8/2002
	{
//		TimeDebugSPrintf2("Main queue low - RTS set\n\r");
		queue0_fed_up = 0;
		UART_A_MCR_REG |= RTS;
	}
}



char to_up_case(char symb)
{
	if ((symb >= 'a') && (symb <= 'z')) return (symb - ('a' - 'A')); 
	return (symb);		
}

int fr_parse(char symb, char *at_mask, int msk_len,int *index)
{
  int t;
  
  if((*index)<0)
  	(*index)=0;  
  
  if(at_mask[*index]==to_up_case(symb))
  {
  	t = *index;
  	t++;
  	*index = t;
  }
  else
  	(*index)=0;
  	
  if((*index)>=msk_len)
  {
  	(*index)=0;
  	return 1;
  } 

  return 0;
}

void fr_tsk(int command_type)
{ 
    //INTR_GLOBAL_DISABLE;

#if 0   
    char debug_string_in[80]="    ";
	char debug_string_out[80]="a:>";    
#endif
    switch(command_type)
    {
    case FR_incoming:
    		fr_direction = FR_incoming;
			uart_a_in_event = FR_IN_modem_char;
  
			In.char_out = (IFI)&uart_a_fputc;
//			In.char_out_non_block = (IFI)&fr_uart_a_fputc;
			In.char_out_non_block = (IFI)&uart_a_fputc;
			In.string_out = (IFPC)&uart_a_fputs;
			In.last_char = 0;
			In.current_char = 0;
			In.i_stack_driver = 0;
			In.in_flag[0] = 0;
			In.driver[0] = (VFI)&fr_stm_in; 	
  
			uart_c_in_event = FR_OUT_modem_char;

			Out.char_out = (IFI)&uart_c_fputc;
			//Out.char_out_non_block = (IFI)&uart_c_fputc;
			Out.char_out_non_block = (IFI)&uart_c_fputc_non_block;
			Out.string_out = (IFPC)&uart_c_fputs;
			Out.last_char = 0;
			Out.current_char = 0;
			Out.i_stack_driver = 0;
			Out.in_flag[0] = 0;
			Out.driver[0] = (VFI)&fr_stm_out; 	
	        DialingParams.ndx = 0;
	        
			DebugPrintf(":FR_incoming\n\r");
//			send0(FR, FR_init);

//			send0(FR, FR_FAX_CALL);


		break;
    case FR_outgoing:
    	fr_direction = FR_outgoing;
		uart_c_in_event = FR_IN_modem_char;

		In.char_out = (IFI)&uart_c_fputc;
		//In.char_out_non_block = (IFI)&uart_c_fputc;
		In.char_out_non_block = (IFI)&uart_c_fputc_non_block;
		In.string_out = (IFPC)&uart_c_fputs;
		In.last_char = 0;
		In.current_char = 0;
		In.i_stack_driver = 0;
		In.in_flag[0] = 0;
		In.driver[0] = (VFI)&fr_stm_in; 	

		uart_a_in_event = FR_OUT_modem_char;

		Out.char_out = (IFI)&uart_a_fputc;
		Out.char_out_non_block = (IFI)&fr_uart_a_fputc;
		Out.string_out = (IFPC)&uart_a_fputs;
		Out.last_char = 0;
		Out.current_char = 0;
		Out.i_stack_driver = 0;
		Out.in_flag[0] = 0;
		Out.driver[0] = (VFI)&fr_stm_out; 	
		
//		send0(FR, FR_init);
//		send0(FR, FR_FAX_CALL);

		break;

	case FR_start:
		fr_task_on = 1;
		send0(FR, FR_init);
		send0(FR_IN, FR_FAX_CALL);
		fr_init_structs();
		InitTimeDebug();
		DebugPrintf(":FR_start\n\r");
		break;
    case FR_init:
		stop_flag = 0;    
        fr_state_in = FR_STATE_SLEEP;
        fr_state_out = o0;
		uart_a_fputs("ATE0V1;+WIND=2;+CR=0;+CRC=0\r");
		DebugPrintf(":FR_init\n\r");
//		a_uart_flag = 0;
    	break;
    case FR_finished:
		TimeDebugSPrintf("FR_finished\r\n");
		dropcall();
		TimeDebugDump();
		fr_task_on = 0;    
		fr_state_out = o0;
		clcc_check_enabled = 1;
		pas_check_enabled = 1;
		stop_flag = 0;
		break;
    case FR_stop:
    	if(stop_flag)
    		break;
		stop_flag = 1;    
    	TimeDebugSPrintf("FR_stop");
		dropcall();
//		send_at_command("ATH\r",100);
	    send_at_command("ATE0V0\r",100);
    	if (fr_state_out == o0 ||
    		(fr_state_out == oo1) ||
    		(fr_state_in == ii1))
    	{
	    	if(fr_task_on && (dce_state != DCE_STATE_PUMP))
	    	{
	    		send_at_command("ATE0V0;+WIND=255;+CR=1;+CRC=1\r",200);
				DebugPrintf(":FR_stop\n\r");
			}
			send0_delayed(200,FR,FR_finished); // -hakan
	    	fr_state_in = FR_STATE_SLEEP;
    	}
		else
		{
			fr_stop_flag = 1;
			TimeDebugSPrintf("fr_stop_flag");
		}   
    	break;
    case FR_PUMP_OUT:
    	Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
    	break;
    case FR_PUMP_IN:
    	In.driver[In.i_stack_driver](FR_PUMP_IN);
    	break;    
	case FR_CONNECT_IN:
		In.driver[In.i_stack_driver](FR_CONNECT_IN);
		break;
	case FR_OK_IN:
		In.driver[In.i_stack_driver](FR_OK_IN);
		break;
    case FR_IN_modem_char:
		
		check_and_set_uart_a_rts();
		
    	In.current_char = queue_get(&(queue[0]));
#if 0
    	if(stop_flag)
    	{
    		debug_string_in[0]=In.current_char;
    		TimeDebugSPrintf(debug_string_in);
    	}
#endif
//    	if (pump_page)
//    	{
//			send0(FR,FR_PUMP_OUT);
//		}
    	if (In.in_flag[In.i_stack_driver] == 0)
    	{
#if 0
    		if(debug_string_in_len<80)
    		{
    			debug_string_in[debug_string_in_len++] = In.current_char;
    			debug_string_in[debug_string_in_len] = 0;
    		}
    		if(In.current_char == '\r' || In.current_char == '\n')
    		{
    			DebugPrintf(debug_string_in);
				debug_string_in_len = 3;
			}
#endif
			if (fr_parse(In.current_char,"OK",2,&ind_OK_IN))
			{
//				TimeDebugSPrintf("FRTSK-OK-in");
				In.driver[In.i_stack_driver](FR_OK_IN);
				break;				
			}
			if (fr_parse(In.current_char,"NNECT\r",6,&ind_CONNECT_IN))
			{
//				TimeDebugSPrintf("FRTSK-CONN-in");
				In.driver[In.i_stack_driver](FR_CONNECT_IN);
				break;				
			}
			if (fr_parse(In.current_char,"RROR",4,&ind_ERROR_IN))
			{   
//				TimeDebugSPrintf("FRTSK-ERROR-in");			
				In.driver[In.i_stack_driver](FR_ERROR_IN);
				break;				
			}
			if (fr_parse(In.current_char,"CARRIER",7,&ind_NOCAR_IN))
			{
//				TimeDebugSPrintf("FRTSK-NOCARR-in");
				In.driver[In.i_stack_driver](FR_NO_CARRIER_IN);
				break;				
			}
		}
		if ((In.last_char == DLE) && (In.current_char == ETX))
		{	// FR_DLE_ETX
			In.driver[In.i_stack_driver](FR_DLE_ETX_IN);
			In.last_char = 0;
			break;
		}
		if (In.in_flag[In.i_stack_driver] == 1) 
				In.driver[In.i_stack_driver](FR_IN_modem_char);
		if ((In.last_char == DLE) && (In.current_char == DLE))
			In.current_char = 0;
		In.last_char = In.current_char;
    	break;
    case FR_OUT_modem_char:
    	
    	check_and_set_uart_a_rts(); 
    	   
    	Out.current_char = queue_get(&(queue[0]));
    	
    	if (Out.in_flag[Out.i_stack_driver] == 0)
    	{
#if 0
    		if(debug_string_out_len<80)
    		{
    			debug_string_out[debug_string_out_len++] = Out.current_char;
				debug_string_out[debug_string_out_len]=0;    			
    		}
    		if(Out.current_char == '\r' || Out.current_char == '\n')
    		{
    			DebugPrintf(debug_string_out);
				debug_string_out_len = 3;    
			}
#endif
			if (fr_parse(Out.current_char,"OK",2,&ind_OK_OUT))
			{
//				TimeDebugSPrintf("FRTSK-OK");
				Out.driver[Out.i_stack_driver](FR_OK_OUT);
				break;				
			}
			if (fr_parse(Out.current_char,"+WIND: 2",7,&ind_WIND2))
			{
//				TimeDebugSPrintf("FRTSK- +WIND: 2");
				send0(FR_OUT,FR_RINGBACK);
				break;				
			}
			if (fr_parse(Out.current_char,"NNECT\r",6,&ind_CONNECT_OUT))
			{
//				TimeDebugSPrintf("FRTSK-CONNECT");
				Out.driver[Out.i_stack_driver](FR_CONNECT_OUT);
				break;				
			}
			if (fr_parse(Out.current_char,"RROR",4,&ind_ERROR_OUT))
			{
//				TimeDebugSPrintf("FRTSK_ERROR");
				Out.driver[Out.i_stack_driver](FR_ERROR_OUT);
				//send0(CC,AT_error);
				break;				
			}
			if (fr_parse(Out.current_char,"CARRIER",7,&ind_NOCAR_OUT))
			{
//				TimeDebugSPrintf("FRTSK-NOCARR");
				Out.driver[Out.i_stack_driver](FR_NO_CARRIER_OUT);
				//send0(CC,AT_disconnect);
				break;				
			}
			if (fr_parse(Out.current_char,"BUSY",4,&ind_BUSY_OUT))
			{
//				TimeDebugSPrintf("FRTSK-BUSY");
				send0(FR, FR_stop);
				send0_delayed(300,CC,AT_busy);
				break;				
			}
			
		}
		if ((Out.last_char == DLE) && (Out.current_char == ETX))
		{	// FR_DLE_ETX
			Out.driver[Out.i_stack_driver](FR_DLE_ETX_OUT);
			break;
		}
		if (Out.in_flag[Out.i_stack_driver] == 1) 
				Out.driver[Out.i_stack_driver](FR_OUT_modem_char);
		if ((Out.last_char == DLE) && (Out.current_char == DLE))
			Out.current_char = 0;
		Out.last_char = Out.current_char;
    	break;
    default:
		fr_stm_in(command_type);
	}
}

