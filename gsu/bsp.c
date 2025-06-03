#include <std.h>
#include <regs54xx.h>
#include "su.h"

#ifdef _MODEMS_
#include "modems.h"

#pragma CODE_SECTION (BSP_0_rx_isr, "vtext")
#pragma CODE_SECTION (BSP_0_tx_isr, "vtext")
#endif // _MODEMS_

#pragma CODE_SECTION (BSP_1_rx_isr, "vtext_pump")

//int cntr_flg=0;
int bsp0_tx_flag=0;


//  For Conexant Chip Set Solution only
//static unsigned short	slip_buf0[4];
//static unsigned short	slip_buf0_insert_idx = 0;
//static unsigned short	slip_buf0_remove_idx = 0;

//static unsigned short	slip_buf1[4];
//static unsigned short	slip_buf1_insert_idx = 0;
//static unsigned short	slip_buf1_remove_idx = 0;


//autobaud data
int	autobaud_enabled;
int run_length_counts[16];
//int run_length_data_bits[16];
int run_length_idx;
int run_length_last_bit;
int run_length_bit_counter;
unsigned int run_length_min_pulse = 0xFFFF;
int run_length_bit_lookup[16] =
{
	0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1
};
int baud_rate_found;
char symbol_decoded;
unsigned short last_known_uart_b_rate = UART_BAUD_19200;



void int_bsps(void)
{
	INTR_DISABLE(RINT0);
	INTR_DISABLE(XINT0);
	INTR_DISABLE(RINT1);
	INTR_DISABLE(XINT1);
	
    // init serial port 0
	MCBSP_SUBREG_WRITE(0, SPCR1_SUBADDR, 0x4000);
	MCBSP_SUBREG_WRITE(0, SPCR2_SUBADDR, 0x0200);
	MCBSP_SUBREG_WRITE(0, PCR_SUBADDR, 0x0001);
	MCBSP_SUBREG_WRITE(0, RCR1_SUBADDR, 0x0040);
	MCBSP_SUBREG_WRITE(0, RCR2_SUBADDR, 0x0001);
	MCBSP_SUBREG_WRITE(0, XCR1_SUBADDR, 0x0040);
	MCBSP_SUBREG_WRITE(0, XCR2_SUBADDR, 0x0001);
	
	// init serial port 1
	if(WSU003_Config) // for Codec DCSI config
	{
		MCBSP_SUBREG_WRITE(1, SPCR1_SUBADDR, 0x4000);
		MCBSP_SUBREG_WRITE(1, SPCR2_SUBADDR, 0x0200);
		MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x0000);
		MCBSP_SUBREG_WRITE(1, RCR1_SUBADDR, 0x0040);
		MCBSP_SUBREG_WRITE(1, RCR2_SUBADDR, 0x0001);
		MCBSP_SUBREG_WRITE(1, XCR1_SUBADDR, 0x0040);
		MCBSP_SUBREG_WRITE(1, XCR2_SUBADDR, 0x0001);
	}
	else
	{
		MCBSP_SUBREG_WRITE(1, SPCR1_SUBADDR, 0x0000); // was 0x4000
		MCBSP_SUBREG_WRITE(1, SPCR2_SUBADDR, 0x0200);
		MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x0000);
		MCBSP_SUBREG_WRITE(1, RCR1_SUBADDR, 0x0000); //was 0x0040 (16 bit wrd)
		MCBSP_SUBREG_WRITE(1, RCR2_SUBADDR, 0x0000); // was 0x4 frame ignore
		MCBSP_SUBREG_WRITE(1, XCR1_SUBADDR, 0x0000);
		MCBSP_SUBREG_WRITE(1, XCR2_SUBADDR, 0x0000);
	}
	
	INTR_DISABLE(RINT0);
	INTR_DISABLE(XINT0);
	INTR_DISABLE(RINT1);
	INTR_DISABLE(XINT1);
}

void ActivateBsp0(int mask)
{
	INTR_DISABLE(XINT0);
	INTR_DISABLE(RINT0);
	
	// bring rcvrs and xmtrs out of reset
	if(mask)
	{
		MCBSP_SUBREG_BITWRITE(0, SPCR1_SUBADDR, RRST, RRST_SZ, 1); 
	}
	
	MCBSP_SUBREG_BITWRITE(0, SPCR2_SUBADDR, XRST, XRST_SZ, 1);
	
	delay(4);
	if(mask)
	{
		INTR_ENABLE(RINT0);
	}
	
	INTR_ENABLE(XINT0);

	// kick-off transmit
	DXR10 = 0x5555;
}

void DeactiveBsp0(void)
{
	INTR_DISABLE(XINT0);
    INTR_DISABLE(RINT0);
    
	// place xmtrs into reset
	MCBSP_SUBREG_BITWRITE(0, SPCR1_SUBADDR, RRST, RRST_SZ, 0);
	MCBSP_SUBREG_BITWRITE(0, SPCR2_SUBADDR, XRST, XRST_SZ, 0); 
}

interrupt void BSP_0_rx_isr(void)
{
#ifdef _MODEMS_
	if((unsigned long)modem_BSP_0_rx_isr != 0)
	{
		modem_BSP_0_rx_isr();
	}
#endif
}

extern int into_caller_id;
interrupt void BSP_0_tx_isr(void)
{
	if(into_caller_id)
			bsp0_tx_flag = 1;
	else
	if((unsigned long)modem_BSP_0_tx_isr != 0)
	{
		modem_BSP_0_tx_isr();
	}
}



interrupt void BSP_1_tx_isr(void)
{
	INTR_GLOBAL_DISABLE;
/*	For Conexant Chip Set Solution only
	if(slip_buf0_remove_idx == slip_buf0_insert_idx)
		DXR11 = 0x0000;
	else
	{
		DXR11 = (0xFFFF & slip_buf0[slip_buf0_remove_idx++]);
		slip_buf0_remove_idx &= 0x3;
	}
*/	
	INTR_GLOBAL_ENABLE;	
}

/* Configures the Codec using DSP serial port */
void ConfigCodec(void)
{
  unsigned int XRdy = 0;
  
  MCBSP_SUBREG_BITWRITE(1, SPCR2_SUBADDR, XRST, XRST_SZ, 1);
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = CODEC_REG1_VALUE;
  
  XRdy = 0;
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = CODEC_REG2_VALUE;
  
  XRdy = 0; 
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = CODEC_REG3_VALUE;
  
  XRdy = 0;
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = CODEC_REG4_VALUE;
  
  XRdy = 0;
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = 0xFFFF;
  
//  delay(500);
  
//  MCBSP_SUBREG_BITWRITE(1, SPCR2_SUBADDR, XRST, XRST_SZ, 0);    
   	  

}

void  UpdateCodec(unsigned int CodecVal)
{
  unsigned int XRdy = 0;
  //DebugPrintf1("UpdateCodec, val=",CodecVal);
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = CodecVal;
  
  XRdy = 0;
  
  while(!(XRdy))
  	XRdy = MCBSP_SUBREG_BITREAD(1, SPCR2_SUBADDR, XRDY, XRDY_SZ);

  DXR11 = 0xFFFF;

}

void enable_autobaud(void)
{
	INTR_GLOBAL_DISABLE;
	VOICE_DATA_SWITCH = 0x0;
	
	// reset state variables
	run_length_idx = -1;
	run_length_last_bit = -1;
	run_length_min_pulse = 0xFFFF;
	run_length_bit_counter = 0;
	baud_rate_found = 0;
	symbol_decoded = 0;
	memset(run_length_counts, 0, sizeof(run_length_counts));
	//memset(run_length_data_bits, 0, sizeof(run_length_data_bits));
	
	MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 0);
	
	// configure as McBSP
	MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x0000);

	// take BSP1 receiver out of reset
	DRR11;
	MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 1);
	DRR11;
	INTR_ENABLE(RINT1);
	
	// arm the falling edge detector in the FPGA
	VOICE_DATA_SWITCH = 0x3;
	
	uart_b_rate(0xFFFF);
	UART_B_FCR_REG |= Reset_Tx_FIFO|Reset_Rx_FIFO;
		
	autobaud_enabled = 1;
	INTR_GLOBAL_ENABLE;
}

void disable_autobaud(void)
{
	INTR_DISABLE(RINT1);
	
	// then, put UART B in loopback and flush its fifos
	//UART_B_MCR_REG |= Enable_loopback;
	UART_B_FCR_REG |= Reset_Tx_FIFO|Reset_Rx_FIFO;
		
	// hold BSP1 receiver in reset
	MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 0);
	
	// disarm the falling edge detector in the FPGA
	VOICE_DATA_SWITCH = 0x0;
	
	autobaud_enabled = 0;
}

typedef struct _divider
{
	int	idx;
	unsigned short div;
} BAUD_RATE_DIVIDER;


BAUD_RATE_DIVIDER divider_map[9] =
{
	{1,   UART_BAUD_115200},
	{2,   UART_BAUD_57600},
	{3,   UART_BAUD_38400},
	{6,   UART_BAUD_19200},
	{12,  UART_BAUD_9600},
	{24,  UART_BAUD_4800},
	{48,  UART_BAUD_2400},
	{96,  UART_BAUD_1200},
	{384, UART_BAUD_300},
};


#define smb_prx_tolerance 1
int smb[3][3]={/*a*/{1,4,2},/*A*/{1,5,1},/*<CR>*/{1,1,2}};
char decodedsmb[3]={'a','A',0xD};
int smb_prx[3];
#define BAUD_RATE_MAXIND 80
int baud_rate_array[BAUD_RATE_MAXIND];
int baud_rate_index = 0;
char decoded_symbol_array[BAUD_RATE_MAXIND];
interrupt void BSP_1_rx_isr(void)
{
	unsigned short tmp;
	int i,j,k;
	unsigned long cnt;
	unsigned short div;
	
	INTR_GLOBAL_DISABLE;
	// perform autobaud processing here
	// extract 4 middle sample bits from the received data
	tmp = run_length_bit_lookup[(DRR11) & 0xF];
	if(!(run_length_idx == -1 && tmp))
	{
		// update run length counters
		if(run_length_last_bit != tmp)
		{
			if(run_length_idx != -1 && run_length_min_pulse > run_length_counts[run_length_idx])
				run_length_min_pulse = run_length_counts[run_length_idx];
			run_length_idx++;
			run_length_counts[run_length_idx]++;
			run_length_last_bit = tmp;
		}
		else
		{
			run_length_counts[run_length_idx]++;
		}
	
		run_length_bit_counter++; 
		if((run_length_idx >= 3 && run_length_bit_counter >= ((run_length_min_pulse << 3) + (run_length_min_pulse >> 1))) || run_length_bit_counter >= (400 * 9))
		{
			
			if(run_length_counts[0] > 1)
			{
				// got the start bit and 7 data bits
				// compare to the known patterns
				for(i=0;i<3;i++)
					smb_prx[i] = 0;
				
				for(i = 0; i < 3; i++)
				{
					for(j=0;j<3;j++)
					{
						smb_prx[j] += 
							abs(run_length_counts[i+1] - smb[j][i]*run_length_min_pulse);
					}
				}
			
				for(i=0;i<3;i++)
				{
					if(smb_prx[i] <= run_length_min_pulse/2)
					{
						//character smb[i] has been identified
						// baud rate has been detected
						baud_rate_found = run_length_min_pulse/*>>1*/;
						symbol_decoded = decodedsmb[i];
													
						// configure McBSP as GPIO register
						MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 0);
						MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x1000);
						
						// find the closest divider
						div = UART_BAUD_115200 * baud_rate_found;
						for(k = 0; k < 9; k++)
						{
							if(abs(baud_rate_found - divider_map[k].idx) <= (baud_rate_found >> 2))
							{
								div = divider_map[k].div;
								break;
							}
						}
						
						//wait for the edge of stop bit
						for(tmp=0,cnt=0;!(tmp) && (cnt < 0x3FFFFF); cnt++ )
						{
							tmp = MCBSP_SUBREG_BITREAD(1,PCR_SUBADDR,DR_STAT,1);
						}
						
						if(tmp)
						{
							//init uarts a and b
							uart_b_rate(div);
							disable_autobaud();
							uart_a_rate(div);
							last_known_uart_b_rate = div;
							///debugautobaud
							//if(baud_rate_index<BAUD_RATE_MAXIND)
							//{
							//	decoded_symbol_array[baud_rate_index] = symbol_decoded;
							//	baud_rate_array[baud_rate_index++]=baud_rate_found;
							//}
							///enable_autobaud();
							///~debugautobaud
							send1(DCE,DCE_uart,0,(int*)symbol_decoded);
						}
						else
						{
							// configure as McBSP
							MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x0000);
							baud_rate_found = 0;
						}
						break;
					}
				}
			}
			
			if(!baud_rate_found)
			{
				//baud_rate_found = -1; //temporary value to wait for mark

				// hold BSP1 receiver in reset
				MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 0);
				MCBSP_SUBREG_WRITE(1, PCR_SUBADDR, 0x0000);
		
				// disarm the falling edge detector in the FPGA
				VOICE_DATA_SWITCH = 0x0;

				// reset state variables
				run_length_idx = -1;
				run_length_last_bit = -1;
				run_length_min_pulse = 0xFFFF;
				run_length_bit_counter = 0;
				baud_rate_found = 0;
				symbol_decoded = 0;
				memset(run_length_counts, 0, sizeof(run_length_counts));
				//memset(run_length_data_bits, 0, sizeof(run_length_data_bits));
				// take BSP1 receiver out of reset
				MCBSP_SUBREG_BITWRITE(1, SPCR1_SUBADDR, RRST, RRST_SZ, 1);
				DRR11;
				INTR_ENABLE(RINT1);
		
				// arm the falling edge detector in the FPGA
				VOICE_DATA_SWITCH = 0x3;
			}
		}
	}
	else
	{
		VOICE_DATA_SWITCH = 0x0;
		run_length_idx = -1;
		run_length_last_bit = -1;
		run_length_min_pulse = 0xFFFF;
		run_length_bit_counter = 0;
		baud_rate_found = 0;
		symbol_decoded = 0;
		VOICE_DATA_SWITCH = 0x3;
	}
	INTR_GLOBAL_ENABLE;	
}
