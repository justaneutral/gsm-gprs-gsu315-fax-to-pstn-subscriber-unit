#ifdef _MODEMS_
//#define _BITSTREAM_FEEDBACK_TEST_
//#define _Rx_data_debug_printf_

#include "su.h"
#include "bsps.h"
#include "modems.h"
#include "v32.h"
#include "v22.h"

//prototypes.
int v22_modem_continue_func_1(void);
int v22_modem_continue_func_2(void);

static int vmdmtp = 32;
//static 
int m_cntr = 0;

void data_modem_init(int modem_type, char call_direction, int baud_rate)
{
	INTR_GLOBAL_DISABLE;
	
	/* check and set modem type*/
	vmdmtp = modem_type;
	m_cntr = 0;
	
	switch(vmdmtp)
	{
	case 32:
		/*load v.32 modem vtext section*/
		v32reloc();
		/**** initialize Tx_block[] and Rx_block[] ****/
		break;
	case 22:
		/*load v.22 modem vtext section*/
		v22reloc();
		break;
	}
    
	/**** initialize Tx_block[] and Rx_block[] ****/
	Tx_block_init(ptrs);		/* initialize Tx_block */
	Rx_block_init(ptrs);		/* initialize Rx_block 	*/
	set_Rx_detector_mask(DATA_DETECT_MASK|CED_MASK|V32_AUTOMODE_MASK, ptrs);
	Rx_init_detector(ptrs);
	set_system_delay(HW_SYSTEM_DELAY, ptrs);	/* see c54x.h */
	((struct TX_BLOCK *)(&Tx_block))->scale=16384;
	//possible baud_rates:
	//v.32:14400,12000,9600,7200,4800;
	//v.22:2400,1200
	set_Tx_rate(baud_rate, ptrs);
	
	if(modem_type == 32)
	{
		if(call_direction == 'c')
		{
			Tx_init_v32bisC(baud_rate, ptrs);
			//Tx_init_v32C(ptrs);
		}
		else
		{	
			Tx_init_v32bisA_ANS(baud_rate, ptrs);
			//Tx_init_v32bisA(baud_rate,ptrs);
			//Tx_init_v32A_ANS(ptrs);
			//Tx_init_v32A(ptrs);
		}
	}
	else if(modem_type == 22)
	{
		if(call_direction == 'c')
		{
			Tx_init_v22C(ptrs);
			//modem_continue_func = v22_modem_continue_func_1;
		}
		else
		{
			Tx_init_v22A_ANS(ptrs);
		}
	}

	//install modem's isrs.
	modem_BSP_0_rx_isr = vxx_BSP_0_rx_isr;
	modem_BSP_0_tx_isr = vxx_BSP_0_tx_isr;
    //set modem data function pointers
	set_tx_data = vxx_set_tx_data;
	get_tx_nbits = vxx_get_tx_nbits;
    get_rx_data = vxx_get_rx_data;
	get_rx_nbits = vxx_get_rx_nbits;
	tx_modem_ready = vxx_tx_modem_ready;
    mv_v14_Rx_data_as_8_to_Rx_queue = v32_mv_v14_Rx_data_as_8_to_Rx_queue;
	modem_continue_func = data_modem_continue_func;
	
	//init v.14 values.
	v14_receive(0,0);
	    
    //enable only bsp0 for modems. Bsp1 is disabled to keep 
    //GSM PCI input undesturbed.
	
	//fpga voice/data switch to data.
	if(WSU003_Config)
    	VOICE_DATA_SWITCH = 1;
	else
		DataCommunication();
	
    //make it possible to call receive/transmit.
    //timer_init(1);
    
    ActivateBsp0(1);
    
    INTR_GLOBAL_ENABLE;
}

void v32_mv_v14_Rx_data_as_8_to_Rx_queue(void)
{	
	int v14_rec_buf[4];
	int v14_rec_buf_len;
	int j;
	int max_rate = 0;
	MODEM_DATA_QUEUE_STATE fc_rxq_state = UNCHANGED;
	unsigned int ct,cr;
	
	static int need_retrain = 0;
	
	ct = ((struct TX_BLOCK *)(&Tx_block))->state_ID;
	cr = ((struct RX_BLOCK *)(&Rx_block))->state_ID;
		
	switch(ct)
	{
	case TX_V32A_AC1_ID:
		//check if we need answer modem fallback
		if(((struct TX_BLOCK *)(&Tx_block))->symbol_counter > 2400*4)
		{
			/*Probably V.22*/
			data_modem_init(22,'a',2400); //fall back to v.22
			return;
		}
		break;
	case TX_V32C_AA_ID:
		//check if we need call modem fallback to v.22bis
		if(((struct TX_BLOCK *)(&Tx_block))->symbol_counter > 2400*4)
		{
			//must fallback to v.22 call modem.
			data_modem_init(22,'c',2400);
			return;
		}
	}
	

	switch(cr) //check rates.
	{	
    case RX_V22A_MESSAGE_ID:
    case RX_V22C_MESSAGE_ID:
    	max_rate = ((struct TX_V22_BLOCK *)(&Tx_block))->rate;
    	break;
    case RX_V32A_MESSAGE_ID:
    case RX_V32C_MESSAGE_ID:
    	max_rate = ((struct TX_V32_BLOCK *)(&Tx_block))->max_rate;
	}

	
	if(max_rate) //check rates.
	{	
    	if(((struct RX_BLOCK *)(&Rx_block))->rate != ((struct TX_BLOCK *)(&Tx_block))->rate ||
    	   ((struct RX_BLOCK *)(&Rx_block))->rate != max_rate ||
    	   ((struct TX_BLOCK *)(&Tx_block))->rate != max_rate)
    	{
    		((struct RX_BLOCK *)(&Rx_block))->data_head = ((struct RX_BLOCK *)(&Rx_block))->data_tail = ptrs->Rx_data_start;
    		((struct TX_BLOCK *)(&Tx_block))->data_head = ((struct TX_BLOCK *)(&Tx_block))->data_tail = ptrs->Tx_data_start;
    		
    		j = ((struct RX_BLOCK *)(&Rx_block))->rate;
    		if(j>((struct TX_BLOCK *)(&Tx_block))->rate)
    		{
    			j = ((struct TX_BLOCK *)(&Tx_block))->rate;
    		}
    		if(j>max_rate)
    		{
    			j = max_rate;
    			((struct TX_V32_BLOCK *)(&Tx_block))->max_rate = j;
    		}
    		
    		((struct TX_BLOCK *)(&Tx_block))->rate = j;
    		((struct RX_BLOCK *)(&Rx_block))->rate = j;
    		need_retrain = 1;
    	}
    	else
    	{
    		while(((struct RX_BLOCK *)(&Rx_block))->data_head != ((struct RX_BLOCK *)(&Rx_block))->data_tail)
			{
				j = get_rx_data();
				#ifdef _BITSTREAM_FEEDBACK_TEST_
				set_tx_data(j);
				#else // _V14_ instead of _BITSTREAM_FEEDBACK_TEST_
				v14_rec_buf_len = v14_receive(v14_rec_buf,j);
        		fc_rxq_state = Rx_data_queuep(v14_rec_buf, v14_rec_buf_len);
				if(fc_rxq_state == LOW || fc_rxq_state == FULL) //let retrain.
				{
					need_retrain = 1;
				}
				#endif // _V14_ instead of _BITSTREAM_FEEDBACK_TEST_
 			}
 		}
 	}
 	
 	if(need_retrain)
 	{
 		need_retrain = 0;
		switch(cr) //check rates.
		{	
    	case RX_V32A_MESSAGE_ID:
    		Tx_v32A_retrain(ptrs);
    		break;
    	case RX_V32C_MESSAGE_ID:
	    	Tx_v32C_retrain(ptrs);
	    	break;
    	case RX_V22A_MESSAGE_ID:
   			Tx_v22A_retrain(ptrs);
    		break;
   		case RX_V22C_MESSAGE_ID:
    		Tx_v22C_retrain(ptrs);
		}
	}
}

#endif // _MODEMS_

