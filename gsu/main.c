#include "su.h"

#ifdef _MODEMS_
#include "modems.h"
#endif // _MODEMS_

#ifdef _FAR_MODE_
#include "watchdog.h"
#endif

int WSU003_Config = 1;
int bAwake = 0;

void SLAC_isr_1(void);
void periodic_timer_func(void);
int uarts_setup(void);

extern int CC_current_state;
extern int into_caller_id;
extern int rssi_display;
extern int GSM_ready;
extern int OnBattery;

#define RESET portF000 
volatile ioport unsigned int RESET;

void reset(void)
{
	leds_write(0x15); //all red.
#ifdef _FAR_MODE_
#ifndef _NO_WATCHDOG_
	RESET = 0;
#endif
#endif
}

int modem_process(void)
{
	if(modem_continue_func && fm_state != COMMAND)
	{
		modem_continue_func();
		return 1;
	}
	return 0;
}

void main(void)
{
#ifdef _FAR_MODE_
	watchdog_feed
#endif
	
	INTR_GLOBAL_DISABLE;
	
	PMST = 0xE0;
    SWWSR =0x7276; 
    BSCR = 0x2;
	SWCR = 0x01;
	
	v32reloc();  //to settle McBSP0 isr routines.

    VOICE_DATA_SWITCH=0; //voice. 
    
    GPIOCR = 0x24; 	// Initialize GPIO Bit 2 = CD, Bit 5 =RI 
    GPIOSR = 0xFF;
        
	init_all_queues();
	leds_update(1,dark,0);
	leds_update(2,dark,0);
	leds_update(3,dark,0);
	
	timer_init(0);
	
	uart_a_init(UART_BAUD_19200);
	uart_b_init(UART_BAUD_19200);
	
	init_timer_table();
	
	//  Clear IFR 
	IFR=0xffff;
	
    INTR_GLOBAL_ENABLE;
     
    if(GetBoardVersion())		// WSU003, Rev B or later board
    	WSU003_Config = 1;
    else						// WSU004, board or later
    	WSU003_Config = 0;
    
    set_version();
	
	if(!WSU003_Config)
	{
    	// set Battery Status led to initializing state, red.
    	leds_update(1,red,0);
	}
	
	if(strcmp(VersionNumber,ReadFlashData("VERSION")))
		InitFlashParams();
		
	intializeRS232Setigs();

    //init DCE task.
    dce_tsk_init();
    
    //init SLAC task.
    InitSlac();
    init_slac_timers();
    slac_state_machine(SLAC_init);
    
    //init AT task.
    at_tsk_init();
    
    //init call control.
    CC_current_state = 0;
    
    voice_init();
    
    //Set Volume
    SetVolume();
    
	//Main queue engine.
	for(;;)
	{
		//messages.
#ifndef __DEBUG__
		if(!queue_empty(&(queue[0])))
			((VFI)queue_get(&(queue[0])))((int)queue_get(&(queue[0])));
#else
		DebugMainMsgEntry();
#endif
		
		//modulation & demodulation.
		modem_process();

		// forward fax data
		fax_pump();
	
		//deferred interrupts.
		SLAC_isr_1();
		if(queue[0].nodes_taken <= 7)
		{
			if((fm_state == COMMAND) || (dce_state == DCE_STATE_PUMP))
			{
				uart_b_isr_deferred_part();
			}
			if(!WSU003_Config)
			{
    			Set_Battery_Status();
			}
		}
		if(GSM_ready && CC_current_state == 0 && dce_state == DCE_STATE_NORMAL 
			&& !rssi_display && !bAwake && (OnBattery || ForcedPowerSaving()))
		{
			slow_clock_mode();
		}
		else
		{
			fast_clock_mode();
		}
		if(queue_empty(&(queue[0])))
		{
			if((initial == GetCurrentSlacState()) && (OnHookStatus()))
			{
				WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    			WriteSingleByte(ENABLE_CHNLS_1_AND_2_RW);
    			WriteSingleByte(DEACTIVATE_CHNNL);	// Deactivate Chnls 1 & 2
    			WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    			WriteSingleByte(ENABLE_CHNL_1_RW);
			}
		
			asm("	nop");
			asm("	nop");
			asm("	IDLE 1");
			asm("	nop");
			asm("	nop");
			asm("	nop");
		}
		periodic_timer_func();		
	}
}

