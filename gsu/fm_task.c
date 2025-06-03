//C settings:	DUMP_LEN=0;FARC_MODE;TI_DSP
//Asm settings: V32_DEMO;SHOW_GLOBAL;OVERLAY_MODE=0;ON_CHIP_COEFFICIENTS=1;FARC_MODE

void TimeDebugDump(void);

#ifdef _MODEMS_

#include "su.h"
#include "modems.h"
#include "fr_frames.h"

volatile ioport unsigned VOICE_DATA_SWITCH;

Queue Rx_data_queue;
Queue Tx_data_queue;
QUEUEVALUE ar_Rx_data_queue[QLRx_data_queue];
QUEUEVALUE ar_Tx_data_queue[QLTx_data_queue];

int fm_flag = 0;
char fm_modem_type = 0;
DATA_MODE fm_state = COMMAND;
int fm_connected = 0;
DATA_MODE fax_mode = COMMAND;
DATA_FORMAT_TYPE fm_data_format = V14_RT;
int preambula_counter;

VFV	modem_BSP_0_rx_isr = 0;
VFV	modem_BSP_0_tx_isr = 0;
IFV modem_continue_func = 0;
VFI	set_tx_data = vxx_set_tx_data;
IFV	get_tx_nbits = vxx_get_tx_nbits;
IFV get_rx_data = vxx_get_rx_data;
IFV	get_rx_nbits = vxx_get_rx_nbits;
IFV	tx_modem_ready = vxx_tx_modem_ready;
VFV mv_v14_Rx_data_as_8_to_Rx_queue = v32_mv_v14_Rx_data_as_8_to_Rx_queue;


char derive_modem_type(void)
{
	switch(synchronize_bearer())
	{
	case 4800:
		return '4';
	case 2400:
		return '3';
	case 1200:
		return '1';
	}
	
	return '5';
}

int synchronize_bearer(void)
{
	int bearer_baud_rate = 9600;
	char *flashdata;
	
	if((flashdata = ReadFlashData("DATABEARER")) != NULL)
	{
		switch(flashdata[8])
		{
		case '6': //v.32bis
			bearer_baud_rate = 4800;
			break;
		case '4': //v.22bis
			bearer_baud_rate = 2400;
			break;
		case '2': //v.22
			bearer_baud_rate = 1200;
			break;
		}
	}		
	
	return bearer_baud_rate;
}

void voice_init(void)
{
 	DeactiveBsp0();
	//fpga voice/data switch to voice.
	if(WSU003_Config)
		VOICE_DATA_SWITCH = 0;
	else
		VoiceModeSwitch();
		
	//unlink modem.
	modem_continue_func = 0;
	//install modem's isrs.
	modem_BSP_0_rx_isr = 0;
	modem_BSP_0_tx_isr = 0;
}

int fm_started = 0;

VFI fm_tsk(int param)
{
	static int uart_fc_modified=0;
		
	int m_standard = 32;	//v.32bis
	char m_direction = 'a'; //answer modem
	int m_baud = 9600;		//9600 bps.
	
	switch(param)
	{
	case FM_incoming_fax:
		fax_mode = ON_LINE;
		send0(FR,FR_incoming);
		DebugPrintf(":FM_incoming_fax\n\r");
		break;
	case FM_outgoing_fax:
		//send0(CC,AT_connect);
		fax_mode = DATA;
		send0(FR,FR_outgoing);
		send0(FM,FM_start);
		break;
	case FM_incoming_data:
		fm_flag = param;
		fm_modem_type = derive_modem_type();
		DebugPrintf1("FM incoming data call type = ",(long)fm_flag);
		break;
	case FM_outgoing_data:
		fm_flag = param;
		DebugPrintf1("FM outgoing data call type = ",(long)fm_flag);
		break;
	case FM_modem_type:
		fm_modem_type = (char)queue_get(&(queue[0]));
		DebugPrintf1("FM modem type = ",(long)fm_modem_type);
		//send0(FM,FM_start);
		break;
	case FM_start:
		if(fm_started)
			break;
		switch(fm_modem_type) // v.32 9600 baud
		{
		case '5':
			m_baud = 9600; //v.32bis 9600
			break;
		case '4': //v.32bis 4800
			m_baud = 4800;
			break;
		case '3': //v.22bis 2400, v.26ter 2400
			m_standard = 22;
			m_baud = 2400;
			break;
		case '1': //v.22 1200
			m_standard = 22;
			m_baud = 1200;
			break;
		default: //voice or fax.
			m_standard = 0;
			break;
		}	
		
		if(m_standard || fax_mode != COMMAND)
		{
			fm_started = 1;
			INTR_DISABLE(INT1); //uart b int disable.
			Rx_and_Tx_data_queue_init();
			StopPeriodicPASCheck(0);
			StopPeriodicCREGCheck(0);
			off_echo_cancel(0);
		}
		
		if(fax_mode != COMMAND)
		{
			fax_mode = DATA;
			send0_delayed(100,FR,FR_start);
			DebugPrintf(":FM_start\n\r");
		}
		else
		if(m_standard)
		{
			v14init();
			fm_connected = 0;
			fm_state = ON_LINE;
			setup_uart_a_for_modem();
			uart_fc_modified = 1;

			if(fm_flag == FM_incoming_data)
			{
				m_direction = 'c'; //calling modem by standard.
			}

	    	data_modem_init(m_standard,m_direction,m_baud);
		}	
		break;
	case FM_stop:
		fm_started = 0;
		if(uart_fc_modified)
		{
		    enable_uart_a_autoRTSCTS();
		    //uart_a_init(UART_BAUD_19200);
			//uart_b_init(UART_BAUD_115200);
			uart_fc_modified = 0;
		}
		send0(FR,FR_stop);
//		TimeDebugSPrintf("FR_STOP");
		fax_mode = COMMAND;
		fm_flag = 0;
		fm_modem_type = 0;
		fm_state = COMMAND;
//		TimeDebugSPrintf("COMMAND 0");
		v14init();
		voice_init();
		//Rx_and_Tx_data_queue_init();      
		INTR_ENABLE(INT1); //uart b int enable.
		DebugPrintf("fm task is stopped\n\r");
		///uart_a_rate(144);
		///uart_a_set_FCR; //to flush the FIFOs.
	}
	return 0;
}





// check if GSM is fed up.
//  if it is - we need to send Xoff(0x13) to the local DTE via our soft modem.
//  else if it was fed up, and now it has consumed the characters from the queue,
//	we send Xon(0x11) to the local DTE.
// but in any case we place the character to the output queue to GSM module

int Rx_data_queue_fed_up = 0x00;
MODEM_DATA_QUEUE_STATE write_Rx_data_queue(int *buf, int len)
{
	MODEM_DATA_QUEUE_STATE ret_val = UNCHANGED;
	
	int i;
	QUEUEVALUE qv[40];
	
	if(len)
	{
		if((!Rx_data_queue_fed_up) && (!queue_ready_amaunt(&Rx_data_queue,QLRx_data_queue_LOW))) //GSM fed up.
		{
			Rx_data_queue_fed_up = 0xff;
			ret_val = HIGH;
		}
		else 
		if(Rx_data_queue_fed_up && queue_ready_amaunt(&Rx_data_queue,QLRx_data_queue_HIGH)) //GSM is ready again.
		{
			Rx_data_queue_fed_up = 0x00;
			ret_val = LOW;
		}
	
		if(queue_empty(&Rx_data_queue))
		{
		   ret_val = EMPTY;
		}
		else
		{
			if(!(queue_ready_amaunt(&Rx_data_queue,len+1)))
			{
				return FULL;
			}
		}
		
		for(i=0;i<len;i++)
		{
			qv[i] = buf[i];
		}
		
		queue_put(&Rx_data_queue,qv,len);
	}
		
	return ret_val;
}


MODEM_DATA_QUEUE_STATE Rx_data_queuep(int *buf, int len)
{
	MODEM_DATA_QUEUE_STATE fcstate;
	
	fcstate = write_Rx_data_queue(buf,len);

/*	switch(fcstate)
	{
	//case EMPTY:
	case HIGH:
		//Xoff(0x13) can be sent here or RTS cleared for our software modem if necessary and implemented.
		//here we can initiate retrain or rate reneg to give the system to take the symbols from the queue.
		break;
	case LOW:
	//case FULL:
		//Xon(0x11) can be sent here or RTS set for our software modem if necessary and implemented.
	}
*/
	return fcstate;
}


/*****************************/
void call_delayed(int *th, short period, timer_service_func function, void *parameter)
{
   	if(*th == -1)
   		if(!timer_add(th, 1,// One_shot!!
   			(timer_service_func)function,(void *)parameter,0))
		{
			DebugPrintf("Unable to create timer\r\n");
			*th = -1;
		}
		
	if(*th != -1)
		if(!timer_enable(*th,(unsigned int)period))
			DebugPrintf("Unable to enable timer\r\n");
}


/*****************************/

void mv_Rx_queue_to_uart_a(void)
{
	static unsigned int plus_cntr = 0;
	static char val;
	static char val_dep = 0;
	static int esth = -1;
	static unsigned int estf = 0;
	
	
	if(val_dep)
	{
		if(fr_uart_a_fputc(0xff & (val)))
		{
			val_dep = 0;		
		}
		else
		{
			return;
		}
	}
	
	
	if(!estf && plus_cntr == 3 && queue_empty(&Rx_data_queue))
	{
		plus_cntr=0;
		call_delayed(&esth, 50 /* 500 ms */,(timer_service_func)fr_uart_a_fputc,(void *)'+');
		if(esth != -1)
		{
			estf=1;
		}
	
	}
	
	if(estf && !(queue_empty(&Rx_data_queue)))
	{
		estf=0;
		timer_disable(esth);
	}

	while(!queue_empty(&Rx_data_queue))
	{
		
		val = (char)queue_get(&Rx_data_queue);
		switch(val)
		{
		case '+':
			if(plus_cntr < 3)
				plus_cntr++;
			if(!fr_uart_a_fputc(0xff & (val)))
			{
				val_dep = 1;
				return;
			};
			break;	
		default:
			plus_cntr=0;
			if(!fr_uart_a_fputc(0xff & (val)))
			{
				val_dep = 1;
				return;
			};
		}
	}
	
}

int Tx_data_queue_fed_up = 0;
MODEM_DATA_QUEUE_STATE write_Tx_data_queue(char val)
{
	MODEM_DATA_QUEUE_STATE ret_val = UNCHANGED;
	
	if((!Tx_data_queue_fed_up) && (!queue_ready_amaunt(&Tx_data_queue,QLTx_data_queue_LOW)))
	{
		ret_val = HIGH;
		Tx_data_queue_fed_up = 0xff;
		UART_A_MCR_REG &= ~RTS;
    }
	else
	if((Tx_data_queue_fed_up) && (queue_ready_amaunt(&Tx_data_queue,QLTx_data_queue_HIGH)))
	{
		ret_val = LOW;
		Tx_data_queue_fed_up = 0x0;
		UART_A_MCR_REG |= RTS;
	}

	if(queue_ready_amaunt(&Tx_data_queue,2))
	{
		queue_put(&Tx_data_queue,(void*)(val),0);
	}
	else
	{
		ret_val = FULL;
	}

	return ret_val;
}


void mv_uart_a_to_Tx_queue(char val)
{
	switch(write_Tx_data_queue(val))
	{
	case FULL:
	case HIGH:
		UART_A_MCR_REG &= ~RTS;
		//INTR_DISABLE(INT2);
		//uart_a_data_interrupt(0);
//		break;
//	case FULL:
		//leds_update(2,dark,0);
		//leds_update(3,dark,0);
	}
}


void Rx_and_Tx_data_queue_init(void)                                            
{
	queue_init(&Rx_data_queue,ar_Rx_data_queue,QLRx_data_queue);
	queue_init(&Tx_data_queue,ar_Tx_data_queue,QLTx_data_queue);
	queue_flush(&Rx_data_queue);
	queue_flush(&Tx_data_queue);
	Rx_data_queue_fed_up = 0;
	Tx_data_queue_fed_up = 0;
}

int uart_c_fputs(char *buf)
{
	int cnt = 0;
#if 0
	DebugPrintf("*uc:*");
	DebugPrintf(buf);
#endif	
	for(;*buf;buf++)
	{
		uart_c_fputc(*buf);
		cnt++;
	}
	
	return cnt;
}

int rcvintr(int rc)
{
	switch(rc)
	{
	case 1:
		INTR_ENABLE(RINT0);
    	INTR_DISABLE(XINT0);
    	return 1;
    case -1:
    	INTR_DISABLE(RINT0);
    	INTR_DISABLE(XINT0);
    	break;
	case 0:
		INTR_ENABLE(XINT0);
   		INTR_DISABLE(RINT0);
    }
	return 0;	
}

int receive =0;
int modem_symbol_counter = 0;
int uart_c_fputc(char val) //the bytes from FR to the software fax modem.
{
	static int ind_ata;
	static int ind_fth3;
	static int ind_atd;
	static int ind_frh3;
	static int ind_ftm96;
	static int ind_frm96;
	static int ind_ftm72;
	static int ind_frm72;
	static int ind_ftm48;
	static int ind_frm48;
	static int ind_ftm24;
	static int ind_frm24;
    static int ind_fclass1;
    static int ind_fts;
    static int ind_frs;
    static int ind_at;
    
	static int high_speed_modem = 0;
	int command_interp_state;
	
	
	//char debug_string[16]="uart_c_fputc: \r"
	//debug_string[13] = val;
	//DebugPrintf(debug_string);
	
	command_interp_state = fm_state;
	
	if(receive)
	{
	  command_interp_state =  COMMAND;
	}
	
	switch(command_interp_state)
	{
	case COMMAND: //look for AT command
		modem_symbol_counter = 0;
		if(fr_parse(val,"AT+FTH=3\r",9,&ind_fth3))
		{
//			TimeDebugSPrintf("FTH=3");
			INTR_GLOBAL_DISABLE;
			v21_modem_init('t');
			hdlc_init();
			modem_continue_func = silence_75ms_continue_func;
		    if(receive || high_speed_modem)
		    	preambula_counter = 0;
		    fm_state = DATA;
		    receive = rcvintr(0);
		    high_speed_modem = 0;
			INTR_GLOBAL_ENABLE;
//			write_string_to_fr_tsk("\nCONNECT\r");
//			TimeDebugSPrintf("CON 0");
		}
        else
		if(fr_parse(val,"ATA\r",4,&ind_ata))
		{
//            TimeDebugSPrintf("ATA");
			INTR_GLOBAL_DISABLE;
			//ced_init();
			v21_modem_init('t');
			hdlc_init();
			//modem_continue_func = ced_modem_continue_func;
			modem_continue_func = silence_75ms_continue_func;
		    preambula_counter = 0;
		    fm_state = DATA;
		    receive = rcvintr(0);
		    high_speed_modem = 0;
			INTR_GLOBAL_ENABLE;
		}
        else
		if(fr_parse(val,"AT+FRH=3\r",9,&ind_frh3))
		{
			//if(modem_continue_function != fth3_modem_continue_func)
			{
//				TimeDebugSPrintf("FRH");
				//queue_flush(&(queue[1]));
				frh3_init(1);
				receive = rcvintr(1);
				high_speed_modem = 0;
			}
		}
        else
		if(fr_parse(val,"ATD\r",4,&ind_atd))
		{
//			TimeDebugSPrintf("ATD");
			frh3_init(1);
			receive = rcvintr(1);
			high_speed_modem = 0;
		}
        else
		if(fr_parse(val,"AT+FTM=96\r",10,&ind_ftm96))
		{
//			TimeDebugSPrintf("FTM=96");
			INTR_GLOBAL_DISABLE;
			v29_v27_modem_init('t',29,9600);
			raw_init();
			modem_continue_func = silence_v29_75ms_cf;
		    fm_state = ON_LINE;
			receive = rcvintr(0);
			high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
        else
		if(fr_parse(val,"AT+FRM=96\r",10,&ind_frm96))
		{
//			TimeDebugSPrintf("FRM=96");
			INTR_GLOBAL_DISABLE;
			///v29_modem_init('r');
			v29_v27_modem_init('r',29,9600);
			raw_init();
			modem_continue_func = frmXX_modem_continue_func;
		    fm_state = ON_LINE;
		    receive = rcvintr(1);
		    high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
		else
		if(fr_parse(val,"AT+FTM=72\r",10,&ind_ftm72))
		{
//			TimeDebugSPrintf("FTM=72");
			INTR_GLOBAL_DISABLE;
			v29_v27_modem_init('t',29,7200);
			///v29_modem_init('t');
			raw_init();
			modem_continue_func = silence_v29_75ms_cf;
		    fm_state = ON_LINE;
			receive = rcvintr(0);
			high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
        else
		if(fr_parse(val,"AT+FRM=72\r",10,&ind_frm72))
		{
//			TimeDebugSPrintf("FRM=72");
			INTR_GLOBAL_DISABLE;
			///v29_modem_init('r');
			v29_v27_modem_init('r',29,7200);
			raw_init();
			modem_continue_func = frmXX_modem_continue_func;
		    fm_state = ON_LINE;
		    receive = rcvintr(1);
		    high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
		else
		if(fr_parse(val,"AT+FTM=48\r",10,&ind_ftm48))
		{
//			TimeDebugSPrintf("FTM=48");
			INTR_GLOBAL_DISABLE;
			v29_v27_modem_init('t',27,4800);
			raw_init();
			modem_continue_func = silence_v29_75ms_cf;
		    fm_state = ON_LINE;
			receive = rcvintr(0);
			high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
		else
		if(fr_parse(val,"AT+FRM=48\r",10,&ind_frm48))
		{
//			TimeDebugSPrintf("FRM=48");
			INTR_GLOBAL_DISABLE;
			///v27_modem_init('r');
			v29_v27_modem_init('r',27,4800);
			raw_init();
			modem_continue_func = frmXX_modem_continue_func;
		    fm_state = ON_LINE;
		    receive = rcvintr(1);
		    high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
        else
		if(fr_parse(val,"AT+FTM=24\r",10,&ind_ftm24))
		{
//			TimeDebugSPrintf("FTM=24");
			INTR_GLOBAL_DISABLE;
			v29_v27_modem_init('t',27,2400);
			raw_init();
			modem_continue_func = silence_v29_75ms_cf;
		    fm_state = ON_LINE;
			receive = rcvintr(0);
			high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
		else
		if(fr_parse(val,"AT+FRM=24\r",10,&ind_frm24))
		{
//			TimeDebugSPrintf("FRM=24");
			INTR_GLOBAL_DISABLE;
			///v27_modem_init('r');
			v29_v27_modem_init('r',27,2400);
			raw_init();
			modem_continue_func = frmXX_modem_continue_func;
		    fm_state = ON_LINE;
		    receive = rcvintr(1);
		    high_speed_modem = 1;
			INTR_GLOBAL_ENABLE;
		}
        else
        if(fr_parse(val,"AT+FCLASS=1\r",12,&ind_fclass1))
        {
			rx_bsp0_interrupt_counter = 0;
			tx_bsp0_interrupt_counter = 0;
			rx_bsp0_interrupt_counter_max = 0;
			tx_bsp0_interrupt_counter_max = 0;

        	faxmodem_init();
        	write_string_to_fr_tsk("\nOK\r");
        	receive = rcvintr(-1);
        	high_speed_modem = 0;
        }
        else
        if(fr_parse(val,"AT+FTS=8\r",9,&ind_fts))
        {
        	write_string_to_fr_tsk("\nOK\r");
        }
        else
        if(fr_parse(val,"AT+FRS=8\r",9,&ind_frs))
        {
        	write_string_to_fr_tsk("\nOK\r");
        }
        else
        if(fr_parse(val,"AT\r",3,&ind_at))
        {
//        	TimeDebugSPrintf("AT");
			INTR_GLOBAL_DISABLE;
			modem_continue_func = 0;
		    fm_state = COMMAND;
			receive = rcvintr(-1);
			high_speed_modem = 0;
			preambula_counter = 0;
			INTR_GLOBAL_ENABLE;
        	write_string_to_fr_tsk("\nOK\r");
        }
		break;
	case ON_LINE: //abort the command and go back to command mode.
		//modem_symbol_counter = 0;
		//send0(FM,FM_stop);
		//fm_state = COMMAND;
		break;
    case DATA: //transmit data
		switch(write_Tx_data_queue(val))
		{
		case FULL:
//			TimeDebugSPrintf2("Mdm Tx Qu. FULL -data lost\n\r");
			DebugPrintf("Mdm Tx Qu. FULL - data lost\n\r");
			return 0;
		case HIGH:
//			TimeDebugSPrintf2("Mdm Tx Qu. HIGH\n\r");
			break;
		case LOW:
//			TimeDebugSPrintf2("Mdm Tx Qu. LOW\n\r");
			break;
		case EMPTY:
//			TimeDebugSPrintf2("Mdm Tx Qu. EMPTY\n\r");
			break;
		}
	}
	return 1;
}

int uart_c_fputc_non_block(char val) //the bytes from FR to the software fax modem.
{
	if(Tx_data_queue.nodes_taken >= 32)
		return 0;
		
	queue_put_fast(&Tx_data_queue,(void*)(val));
	return 1;
}

#endif // _MODEMS_
