#define u16 unsigned int
#define s16 int

#include "su.h"
#include "modems.h"

#pragma CODE_SECTION (UART_A_isr, "pump")
#pragma CODE_SECTION (UART_B_isr, "pump")

#pragma CODE_SECTION (pump_uart_a_fputc, "pump")
#pragma CODE_SECTION (pump_uart_b_fputc, "pump")


#define _ti16c752_ports_
#include "duart_ti16c752.h"
#undef  _ti16c752_ports_

#ifdef _FAR_MODE_
#include "watchdog.h"
#endif

extern int fax_mode;
extern int local_connection_on; 
extern int manualIPR;
extern unsigned short last_known_uart_b_rate;

//#undef _DUART_INIT_
#define _DUART_INIT_
#ifdef _DUART_INIT_

#define	PUMP_HIGH_THRESHOLD	(128)
#define	PUMP_LOW_THRESHOLD	(32)
#define	PUMP_MARGIN			(32)

static int no_auto_fc;

unsigned long countBfromPC=0;
unsigned long countAfromWavecom=0;
unsigned long countBtoPC=0;
unsigned long countAtoWavecom=0;


int	uart_a_tx_count;
int	uart_b_tx_count;
int uart_a_tx_over_threshold;
int uart_b_tx_over_threshold;

int fr_task_on = 0;

void uart_b_modem_interrupt(int set)
{
	if(set)
	{
		UART_B_IER_REG |= Modem_status_int;
	}
	else
	{
		UART_B_IER_REG &= ~Modem_status_int;
	}
}

/*init*/
void uart_a_init(int baud)
{
	volatile unsigned int s;
	
	no_auto_fc = 0;
		
	INTR_DISABLE(INT2);
	UART_A_LCR_REG = DLAB_and_EFR_enable;
	UART_A_MCR_REG = 0;
		//DLL,DLH
			uart_a_baud_rate(baud);
		//EFR	
    		UART_A_EFR_REG = Enable_enhanced_functions;
    	//Xon,Xoff
    		UART_A_Xon1_REG=0x00;
    		UART_A_Xon2_REG=0x60;
    		UART_A_Xoff1_REG=0x30;
    		UART_A_Xoff2_REG=0x00;
    UART_A_MCR_REG = TCR_and_TLR_enable;
		//TCR,TLR
    		uart_a_TCR
    		UART_A_TLR_REG = 0x00;
	UART_A_LCR_REG = Word_length_8;   //parity,wrd len,stop bits.
	UART_A_MCR_REG = IRQ_enable | DTR;
	    //FCR
    		uart_a_set_FCR
		//IER
			UART_A_IER_REG = Modem_status_int|
				Rx_data_available_int;//THR_empty_int|
		//EFR
		UART_A_LCR_REG = DLAB_and_EFR_enable;	
    		UART_A_EFR_REG = AutoCTS|AutoRTS|Enable_enhanced_functions;
		UART_A_LCR_REG = Word_length_8;   //parity,wrd len,stop bits.
		//final reset.
			UART_A_THR_REG=0;
		s =	UART_A_RHR_REG;
    	s =	UART_A_IIR_REG;
    	s =	UART_A_LSR_REG;
    	s =	UART_A_MSR_REG;
        
	//system INT2.
	INTR_ENABLE(INT2);
}

void uart_b_init(int baud)
{
	volatile unsigned int s;
	
	no_auto_fc = 0;
		
	INTR_DISABLE(INT1);
	UART_B_LCR_REG = DLAB_and_EFR_enable;
	UART_B_MCR_REG = 0;
		//DLL,DLH
			uart_b_baud_rate(baud);
		//EFR	
    		UART_B_EFR_REG = Enable_enhanced_functions;
    	//Xon,Xoff
    		UART_B_Xon1_REG=0x00;
    		UART_B_Xon2_REG=0x60;
    		UART_B_Xoff1_REG=0x30;
    		UART_B_Xoff2_REG=0x00;
    UART_B_MCR_REG = TCR_and_TLR_enable;
		//TCR,TLR
    		uart_b_TCR
    		UART_B_TLR_REG = 0x00;
	UART_B_LCR_REG = Word_length_8;   //parity,wrd len,stop bits.
	UART_B_MCR_REG = IRQ_enable;
	    //FCR
    		uart_b_set_FCR
		//IER
			UART_B_IER_REG = Modem_status_int|Rx_data_available_int;//THR_empty_int|
		//EFR
		UART_B_LCR_REG = DLAB_and_EFR_enable;	
    		UART_B_EFR_REG = AutoCTS|AutoRTS|Enable_enhanced_functions;
		UART_B_LCR_REG = Word_length_8;   //parity,wrd len,stop bits.
		//final reset.
			UART_B_THR_REG=0;
		s =	UART_B_RHR_REG;
    	s =	UART_B_IIR_REG;
    	s =	UART_B_LSR_REG;
    	s =	UART_B_MSR_REG;

	//system INT1.
	INTR_ENABLE(INT1);
}

int uart_a_last_baud_rate = 0;
void uart_a_rate(int baud)
{
	int	temp;
	INTR_DISABLE(INT2);
	uart_a_last_baud_rate = baud;
//	UART_A_IER_REG &= ~X_sleep_mode;
	temp = UART_A_LCR_REG & 0xff;
	UART_A_LCR_REG = DLAB_and_EFR_enable;
	uart_a_baud_rate(baud);
	UART_A_LCR_REG = temp;
//	UART_A_IER_REG |= X_sleep_mode;
	INTR_ENABLE(INT2);
}

int uart_b_last_baud_rate = 0;
void uart_b_rate(int baud)
{
	int	temp;
	INTR_DISABLE(INT1);
	uart_b_last_baud_rate = baud;
//	UART_B_IER_REG &= ~X_sleep_mode;
	temp = UART_B_LCR_REG & 0xff;
	UART_B_LCR_REG = DLAB_and_EFR_enable;
	uart_b_baud_rate(baud);
	UART_B_LCR_REG = temp;
//	UART_B_IER_REG |= X_sleep_mode;
	INTR_ENABLE(INT1);
}

void set_duart_word_len(int word_len)
{
	unsigned int valA, valB;
	
	INTR_DISABLE(INT1 | INT2);
	valA = UART_A_LCR_REG;
	valB = UART_B_LCR_REG;	
	switch(word_len)
	{
	/*	case 5:	// Wavecom only support 7 and 8 bit data
			UART_B_LCR_REG = valB & 0xFC;
			UART_A_LCR_REG = valA & 0xFC;
			break;
		case 6:
			UART_B_LCR_REG = (valB & 0xFD) | Word_length_6;
			UART_A_LCR_REG = (valA & 0xFD) | Word_length_6;
			break;
	*/	case 7:
			UART_B_LCR_REG = (valB & 0xFE) | Word_length_7;
			UART_A_LCR_REG = (valA & 0xFE) | Word_length_7;
			break;
		case 8:
			UART_B_LCR_REG |= Word_length_8;
			UART_A_LCR_REG |= Word_length_8;
			break;
	}
	INTR_ENABLE(INT1 | INT2);
}

void set_duart_stop_bits(int num_bits)
{
	unsigned int valA, valB;
	
	INTR_DISABLE(INT1 | INT2);
	valA = UART_A_LCR_REG;
	valB = UART_B_LCR_REG;
	switch(num_bits)
	{
		case 1:
			UART_B_LCR_REG = valB & 0xFB;
			UART_A_LCR_REG = valA & 0xFB;
			break;
		case 2:		// 1.5 for 5 data bits, otherwise 2 stop bits
			UART_B_LCR_REG |= Stop_bits_2;
			UART_A_LCR_REG |= Stop_bits_2;
			break;
	}
	INTR_ENABLE(INT1 | INT2);
}

void set_duart_parity(int parity_type)
{
	unsigned int valA, valB;
	
	INTR_DISABLE(INT1 | INT2);
	valA = UART_A_LCR_REG;
	valB = UART_B_LCR_REG;
	switch(parity_type)
	{
		case 0:	// odd parity
			UART_B_LCR_REG = (valB & 0xC7) | _ODD_PARITY_;
			UART_A_LCR_REG = (valA & 0xC7) | _ODD_PARITY_;		
			break;
		case 1:	// even parity
			UART_B_LCR_REG = (valB & 0xC7) | _EVEN_PARITY_;
			UART_A_LCR_REG = (valA & 0xC7) | _EVEN_PARITY_;
			break;
		case 2:	// force parity to 1 (Mark)
			UART_B_LCR_REG = (valB & 0xC7) | _FORCE_PARITY_ | _ODD_PARITY_;
			UART_A_LCR_REG = (valA & 0xC7) | _FORCE_PARITY_ | _ODD_PARITY_;
			break;
		case 3:	// force parity to 0 (Space)
			UART_B_LCR_REG = (valB & 0xC7) | _FORCE_PARITY_ | _EVEN_PARITY_;
			UART_A_LCR_REG = (valA & 0xC7) | _FORCE_PARITY_ | _EVEN_PARITY_;
			break;
		case 4:	// no parity (None)
			UART_B_LCR_REG &= 0xC7;
			UART_A_LCR_REG &= 0xC7;
			break;
	}
	INTR_ENABLE(INT1 | INT2);
}


void setup_uarts_for_pump(void)
{
	int	temp;
	int	msr;

	no_auto_fc = 1;
	
	INTR_DISABLE(INT1 | INT2);	
	/* setup uarts */
	uart_a_tx_count = 0;
    uart_b_tx_count = 0;
    uart_a_tx_over_threshold = 0;
    uart_b_tx_over_threshold = 0;
    /* cancel auto RTS */
    temp = UART_B_LCR_REG & 0xff;
    UART_B_LCR_REG = DLAB_and_EFR_enable; //unnecessary
    UART_B_EFR_REG = AutoCTS|Enable_enhanced_functions; //0;
    UART_B_LCR_REG = temp;
    uart_b_set_FCR;
    temp = UART_A_LCR_REG & 0xff;
    UART_A_LCR_REG = DLAB_and_EFR_enable;
    UART_A_EFR_REG = AutoCTS|Enable_enhanced_functions; //0;
    UART_A_LCR_REG = temp;
    uart_a_set_FCR;
    
        
    msr = UART_B_MSR_REG;
    if(msr & DSR)
		UART_A_MCR_REG |= DTR;
	else
		UART_A_MCR_REG &= ~DTR;
	
	if(msr & CTS)
		UART_A_MCR_REG |= RTS;
	else
		UART_A_MCR_REG &= ~RTS;
	
	
	if(UART_A_MSR_REG & CTS)
	{
		UART_B_MCR_REG |= RTS;
	}
	else
	{
		UART_B_MCR_REG &= ~RTS;
	}
	INTR_ENABLE(INT1 | INT2);

}

void
setup_uart_a_for_modem(void)
{
	int	temp;

	no_auto_fc = 1;
	
	INTR_DISABLE(INT2);	
	/* setup uarts */
	uart_a_tx_count = 0;
    uart_a_tx_over_threshold = 0;
    /* cancel auto RTS/CTS */
    temp = UART_A_LCR_REG & 0xff;
    UART_A_LCR_REG = DLAB_and_EFR_enable;
    UART_A_EFR_REG = 0;
    UART_A_LCR_REG = temp;
    uart_a_set_FCR;
	UART_A_MCR_REG |= RTS;
    
	INTR_ENABLE(INT2);
}


void enable_uart_a_autoRTSCTS(void)
{
    int	temp;
    
    INTR_DISABLE(INT2);	
    	
    temp = UART_A_LCR_REG & 0xff;
    UART_A_LCR_REG = DLAB_and_EFR_enable;
    UART_A_EFR_REG = AutoCTS|AutoRTS|Enable_enhanced_functions;
    UART_A_LCR_REG = temp;
    
    INTR_ENABLE(INT2);
}

#endif //_DUART_INIT_

//void uart_a_init_transmitting(void)
//{
//	if(!queue_empty(&(queue[1])))
//		while(!(UART_A_LSR_REG&THR_empty));
//	UART_A_THR_REG=0xff&queue_get(&(queue[1]));	
//}
//void uart_b_init_transmitting(void)
//{
//	if(!queue_empty(&(queue[2])))
//		while(!(UART_B_LSR_REG&THR_empty));
//	UART_B_THR_REG=0xff&queue_get(&(queue[2]));	
//}


//int uart_a_fgetc(int *val)
//{
//	if(UART_A_LSR_REG & DATA_in_
//} 

int fr_uart_a_fputc(const int c)
{
	static int fifo_cnt;
	
	if(UART_A_LSR_REG & THR_empty)
		fifo_cnt = 64;
	
	if(fifo_cnt > 0)
	{
		UART_A_THR_REG = c;
		fifo_cnt--;
		return 1;
	}
	
	return 0;
}

int fr_uart_a_fputs(const char* pBuf)
{
	int cnt = 0;
	
	while(*pBuf != '\0')
	{
		if(uart_a_fputc_(*pBuf++, 1))	/* blocking */
			cnt++;
		else
			break;
	}
	
	return cnt;
}

int pump_uart_a_fputc(const int c)
{
	
	static int fifo_a_cnt;
	
	if(UART_A_LSR_REG & THR_empty)
		fifo_a_cnt = 64;
	
	if(fifo_a_cnt > 0)
	{
		countAtoWavecom++;
		UART_A_THR_REG = c;
		fifo_a_cnt--;
		if(no_auto_fc)
		{	
			INTR_GLOBAL_DISABLE;
			if(uart_a_tx_count > 0)
				uart_a_tx_count--;
			INTR_GLOBAL_ENABLE;
			
			if(uart_a_tx_count <= PUMP_LOW_THRESHOLD)
			{
				uart_a_tx_over_threshold = 0;
				if(UART_A_MSR_REG & CTS)
					UART_B_MCR_REG |= RTS;
			}				
		}
		return 1;
	}
	
	return 0;
}

int pump_uart_b_fputc(const int c)
{
	
	static int fifo_b_cnt;
	
	if(UART_B_LSR_REG & THR_empty)
		fifo_b_cnt = 64;
	
	if(fifo_b_cnt > 0)
	{
		countBtoPC++; 
		UART_B_THR_REG = c;
		fifo_b_cnt--;
		if(no_auto_fc)
		{
			INTR_GLOBAL_DISABLE;
			if(uart_b_tx_count > 0)
				uart_b_tx_count--;
			INTR_GLOBAL_ENABLE;
			
			if(uart_b_tx_count <= PUMP_LOW_THRESHOLD)
			{
				uart_b_tx_over_threshold = 0;
				if(UART_B_MSR_REG & CTS)
					UART_A_MCR_REG |= RTS;
			}
		} 
		return 1;
	}
	
	return 0;
}

void
uart_b_discard_char(void)
{
	if(no_auto_fc)
	{
		INTR_GLOBAL_DISABLE;
		if(uart_b_tx_count > 0)
			uart_b_tx_count--;
		INTR_GLOBAL_ENABLE;
		
		if(uart_b_tx_count <= PUMP_LOW_THRESHOLD)
		{
			uart_b_tx_over_threshold = 0;
			if(UART_B_MSR_REG & CTS)
				UART_A_MCR_REG |= RTS;
		}
	} 
}


/*fputc*/
void uart_a_fputc(const int c)
{
    uart_a_fputc_(c,1);
}

extern void modem_process(void);

int uart_a_fputc_(const int c,  int blocked)
{
	
	//queue_put(&(queue[1]),(void*)c,0);
	//uart_a_init_transmitting();
	
	//unsigned long i;
	//for(i=0;!(UART_A_LSR_REG & THR_empty) && i<_UART_THR_MAX_TIME_OUT_;i++);
	//UART_A_THR_REG = 0xff & c;

	while(!(UART_A_LSR_REG & THR_empty))
	{
#ifdef _FAR_MODE_		
		watchdog_feed
#endif
		if(!blocked)
			return 0;
		
		modem_process();
	}

	UART_A_THR_REG = 0xff & c;

if(no_auto_fc){	
	INTR_GLOBAL_DISABLE;
	if(uart_a_tx_count > 0)
		uart_a_tx_count--;
	INTR_GLOBAL_ENABLE;
	
	if(uart_a_tx_count <= PUMP_LOW_THRESHOLD)
	{
		uart_a_tx_over_threshold = 0;
		if(UART_A_MSR_REG & CTS)
			UART_B_MCR_REG |= RTS;
	}
		
}
	return 1;
}


void uart_b_fputc(const int c)
{
	uart_b_fputc_(c,1);
}

int uart_b_fputc_(const int c, int blocked)
{
	//queue_put(&(queue[2]),(void*)c,0);
	//uart_b_init_transmitting();
	
	//unsigned long i;
	//for(i=0;!(UART_B_LSR_REG & THR_empty) && i<_UART_THR_MAX_TIME_OUT_;i++);
	//UART_B_THR_REG = 0xff & c;

	while(!(UART_B_LSR_REG & THR_empty))
	{
#ifdef _FAR_MODE_		
		watchdog_feed
#endif
		if(!blocked)
			return 0;

		modem_process();
				
		if(UART_B_MSR_REG & CD) //terminal not connected.
		{
			break;
		}
    }

    UART_B_THR_REG = 0xff & c;
if(no_auto_fc){
	INTR_GLOBAL_DISABLE;
	if(uart_b_tx_count > 0)
		uart_b_tx_count--;
	INTR_GLOBAL_ENABLE;
	
	if(uart_b_tx_count <= PUMP_LOW_THRESHOLD)
	{
		uart_b_tx_over_threshold = 0;
		if(UART_B_MSR_REG & CTS)
			UART_A_MCR_REG |= RTS;
	}
}    
    return 1;
}

int string_length(const char *ps)
{
	int i;
	for(i=0;ps[i];i++);
	return i;
}

void uart_a_fputs(const char* pBuf)
{
 	while(*pBuf != '\0')
    {
		uart_a_fputc(*pBuf++);
	}
}

void uart_b_fputs(const char* pBuf)
{
    //queue_put(&(queue[2]),(void*)pBuf,string_length(pBuf));
    //uart_b_init_transmitting();
    
    if(autobaud_enabled)
    {
    	uart_b_discard_char();
    	disable_autobaud();
    	uart_b_rate(last_known_uart_b_rate);
    }
    while(*pBuf != '\0')
	{
		uart_b_fputc(*pBuf++);
	}
}

//int UART_B_isr_flag = 0; 
//void UART_B_isr_deferred(void) //INT1
//{
//	UART_B_isr_flag = 1;	
//}
interrupt void UART_B_isr(void) //INT1
{

	int ipt;
	unsigned short lsr;
	unsigned short val;
	
//	if(!UART_B_isr_flag)
//		return;
		
//	UART_B_isr_flag = 0;	
	
	ipt = 0x3f & UART_B_IIR_REG;
	
  	switch(ipt)    
  	{
    	case _Receiver_line_status_error_: /*r.l.stat.err.*/
    	case _Receiver_timeout_int_:  /*R.T.O. interrupt*/
    	case _RHR_int_:/*R.D.Int*/
            //uart_a_fputc(0xff&UART_B_RHR_REG);
            //break;
	    	switch(dce_state)
	    	{
#ifdef MON
	    	case DCE_STATE_DEBUG:
	    		send1(MON,DCE_uart,0,0xff&UART_B_RHR_REG);
	    		break;
#else //MON
	    	case DCE_STATE_DEBUG:
#endif //MON	    		
	    	case DCE_STATE_PUMP:
	    		while((lsr = UART_B_LSR_REG & 0xff) & Data_in_receiver)
	    		{
	    			val = UART_B_RHR_REG & 0xff;
	    			if(lsr & (Framing_error | Overrun_error | Break_int))
	    			{
	   					if(!manualIPR && data_mode == COMMAND)
	   					{
	   						//enable_autobaud();
	    				}
	    				continue;	// ignore errored characters
	    			}
	    		
					if(no_auto_fc)
					{
		    			if(uart_a_tx_count >= PUMP_HIGH_THRESHOLD + PUMP_MARGIN)
		    				break;
		    	   		uart_a_tx_count++;
		    	   		if(uart_a_tx_count >= PUMP_HIGH_THRESHOLD)
		    	   		{
		    	   			uart_a_tx_over_threshold = 1;
		    	   			UART_B_MCR_REG &= ~RTS;
		    	   		}
					}
	    				
	    	   		countBfromPC++;
	    	   		send1(DCE,DCE_uart,0,(int*)val);

	    	   	}
	        }
      		break;
    	case _THR_int_:
    		/*
			if(!queue_empty(&(queue[2])))
				UART_B_THR_REG=0xff&queue_get(&(queue[2]));
			*/
			if(dce_state == DCE_STATE_PUMP && data_mode == COMMAND && !manualIPR)
			{
				while(!(UART_B_LSR_REG & THR_and_TSR_empty))
				{
				}
				UART_B_IER_REG &= ~0x2;
				enable_autobaud();
			}	
    		break;
    	case _Modem_int_: //Modem.Int
			msr_store = UART_B_MSR_REG & 0xff;
			uart_b_isr_occured = 1;

			if(dce_state == DCE_STATE_PUMP)
			{	
				if(msr_store & DSR)
				{
					UART_A_MCR_REG |= DTR;
					if(!manualIPR && data_mode == COMMAND)
					{
						enable_autobaud();
					}
				}
				else
				{
					UART_A_MCR_REG &= ~DTR;
					disable_autobaud();
				}
				
				if(msr_store & CTS)
				{
					if(!uart_b_tx_over_threshold)
	   					UART_A_MCR_REG |= RTS;
	   			}
	   			else
	   			{
	   				UART_A_MCR_REG &= ~RTS;
				}
			}
	}
} 

//int UART_A_isr_flag = 0; 
//void UART_A_isr_deferred(void) //INT2
//{
//	UART_A_isr_flag = 1;	
//}
interrupt void UART_A_isr(void) //INT2
{
	int ipt;
	unsigned short val;
	unsigned short lsr;
	unsigned short msr;
	

//	if(!UART_A_isr_flag)
//		return;
		
//	UART_A_isr_flag = 0;	
  	
  	ipt = 0x3f & UART_A_IIR_REG;
  	//while(!(ipt & 0x1))
  	//{
  	switch(ipt)    
  	{
    	case _Receiver_line_status_error_: /*r.l.stat.err.*/
    	case _Receiver_timeout_int_:  /*R.T.O. interrupt*/
    	case _RHR_int_:/*R.D.Int*/

			
			while((lsr = UART_A_LSR_REG & 0xff) & Data_in_receiver)
			{    			
		    	val = (0xff&UART_A_RHR_REG);
		    	if(!(lsr & (Framing_error | Parity_error | Overrun_error | Break_int)))
		    	{	

		    		if(dce_state!=DCE_STATE_PUMP)
		    		{
//		    			if(fax_mode == DATA)
						if(fr_task_on)
		    			{  
		    				
							send1(FR,uart_a_in_event,0,(int*)val); ///fax
	    			   		
	    			   		//clear RTS if queue is full.
	    			   		if((!queue0_fed_up) && (!queue_ready_amaunt(&(queue[0]),QLMAIN_LOW)))
	    			   		{
//	    			   			TimeDebugSPrintf2("Main queue high - RTS reset\n\r");
	    			   			queue0_fed_up = 1;
	    			   			UART_A_MCR_REG &= ~RTS;		
							}
		    			}
		    			else
						if(fax_mode != DATA)
		    			{
		    				if(data_mode == DATA)
	    					{
	    						mv_uart_a_to_Tx_queue(val);
	    						////uart_b_fputc_(val,0);
	    					}
	    					else
                    		{
	    						send1(AT,AT_response,0,(int*)val);
		    				}
		    			}
                
                	}
                	else
                	{
if(no_auto_fc){
		    			//flow control handling for pump mode.
		    			if(uart_b_tx_count >= PUMP_HIGH_THRESHOLD + PUMP_MARGIN)
	    					break;
		    			uart_b_tx_count++;
		    			if(uart_b_tx_count >= PUMP_HIGH_THRESHOLD)
			    		{
			    			uart_b_tx_over_threshold = 1;
		    				UART_A_MCR_REG |= RTS;
			    		}
}
	    				countAfromWavecom++;
	    				send1(DCE,AT_response,0,(int*)val);
	    			}
	    		}
      		}
      		break;
/*    	case _THR_int_:
			if(!queue_empty(&(queue[1])))
				UART_A_THR_REG=0xff&queue_get(&(queue[1]));	
    		break;
*/    	case _Modem_int_: //Modem.Int
			msr = UART_A_MSR_REG & 0xff;
			if(msr & dDSR)
    		{
    			if(msr & DSR)	
				{
				    data_mode = ON_LINE;
				    DebugPrintf("Uart A: DSR low - data/online mode\n\r");
				    if(DCE_STATE_PUMP != dce_state)
				    {
				    	send0(AT,AT_data_mode);
					}
					else
					{
						UART_B_MCR_REG |= DTR;
					}
				}
				else
				{
				    data_mode = COMMAND;
				    DebugPrintf("Uart A: DSR high - command mode\r\n");
				    if(DCE_STATE_PUMP != dce_state)
				    {
				    	send0(AT,AT_command_mode);
				    	uart_a_set_FCR //to flush the FIFOs.
				    }
				    else
					{
						//UART_B_MCR_REG &= ~DTR;
						UART_B_MCR_REG |= DTR;
					}
		    	}
    		}
    		
			if(dce_state == DCE_STATE_PUMP)
			{
				if(msr & CTS)
    			{
    				if(!uart_a_tx_over_threshold)
    					UART_B_MCR_REG |= RTS;
    			}
    			else
    			{
    				UART_B_MCR_REG &= ~RTS;
    			}
    		}
    		else
    		{
				if(msr & CTS)
    			{
//   					TimeDebugSPrintf2("CTS high\r\n");
   					//UART_C_MCR_REG |= RTS;
    			}
    			else
    			{
//    				TimeDebugSPrintf2("CTS low\r\n");
    				//UART_C_MCR_REG &= ~RTS;
    			}
    		}
    		
    		if((msr & dCD) || (msr & dRI))
    		{    
    			 /* Always write CD and RI together to HPI
    			    GPIOSR = |x|x|RI|x|x|CD|x|x|
    			    MSR    = |CD|x|RI|x|x|x|x|x| 
    			    Due to V.24-RS232 conversion RI and CD needs to be inverted.
    			 */ 
       			 GPIOSR = ~(((msr & RI) >> 1) | ((msr & CD) >> 5));
       		}	
    		break;
    	default:
    		break;
    		//DebugPrintf("UART A unexpected int\n\r");
	}
  	//ipt = 0x3f & UART_A_IIR_REG;
	//}
}

void autoboudA(int rc)
{
	//	if(rc > 0xF0)
	//		uart_b_rate(UART_BAUD_115200);
	//	else
		if(rc == 0x06)
		{
			uart_b_rate(UART_BAUD_9600);
			uart_a_rate(UART_BAUD_9600);
		}
		else
		if(rc == 0x78)
		{
			uart_b_rate(UART_BAUD_4800);
			uart_a_rate(UART_BAUD_4800);
		}
		else
		if(rc == 0x80)
		{
			uart_b_rate(UART_BAUD_2400);
			uart_a_rate(UART_BAUD_2400);
		}
	
	leds_update(2,dark,0);

}

/*EOF*/ 


