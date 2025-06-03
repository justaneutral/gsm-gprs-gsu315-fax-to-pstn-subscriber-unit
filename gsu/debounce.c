#include "su.h"

int msr_store;
int uart_b_isr_occured = 0;

void uart_b_isr_deferred_part(void)
{
	if(uart_b_isr_occured)
	{
		check_serial_cable_state(&msr_store);
		uart_b_isr_occured = 0;
	}
}

///#pragma CODE_SECTION(check_serial_cable_state,".dbgtext")
void check_serial_cable_state(int *msr)
{
#if(0)
	char str[80]="";
	
	sprintf(str,"uart B msr = %X",(*msr));
	puts(str);
#endif	

#ifndef _UART_B_DSR_ //CD - cable connect/disconnect
    if((*msr) & CD)
    {
    	if(dce_state != DCE_STATE_NORMAL)
#else 				//DSR - on/off
    if(!((*msr) & DSR))
	{
		if(dce_state == DCE_STATE_PUMP)
#endif
		{
			intializeRS232Setigs();
			send1(DCE,DCE_change_state,0,(int*)DCE_STATE_NORMAL);
		    send0(SLAC,SLAC_dce_normal);
		    uart_a_tx_count = 0;
		    uart_b_tx_count = 0;
		    uart_a_tx_over_threshold = 0;
		    uart_b_tx_over_threshold = 0;
	    }
	}
	else
	{
		if(dce_state != DCE_STATE_PUMP)
		{
		    fast_clock_mode(); // to wake-up from power saving
		    restoreUserParams();
		    send1(DCE,DCE_change_state,0,(int*)DCE_STATE_PUMP);
			send0(SLAC,SLAC_dce_pump);
			uart_a_tx_count = 0;
		    uart_b_tx_count = 0;
		    uart_a_tx_over_threshold = 0;
		    uart_b_tx_over_threshold = 0;
		}
	}
}

