#ifdef _MODEMS_
//#define _BITSTREAM_FEEDBACK_TEST_

#include "su.h"
#include "bsps.h"
#include "modems.h"
#include "gendet.h"
#include "v21.h"
#include "v27.h"
#include "v29.h"
#include "v22.h"
#include "v32.h"
#include "fr_frames.h"

#define EOP_MSB 0x1000000
#define EOP_LSB 0x000001
#define EOP_MASK 0x001001
#define EOP_BITS 0xffffff
#define REVERSED_DLE 0x08
#define REVERSED_ETX 0xc0
#define FLAG 0x7e
#define PREAMBULA_FLAGS_NUMBER 38
#define	LAST_FRAME_MASK	0x08		/* reversed */

//prototypes
int v22_modem_continue_func_1(void);
int v22_modem_continue_func_2(void);
void v32_modem_states_log(void);
void v32_fallback_trigger(void);

extern long simbols_from_vxx;

#if 0
#define STATE_INFO_LENGTH 200
typedef struct STATE_INFO_TAG
{
	int index;
	unsigned int state[STATE_INFO_LENGTH];
	unsigned int duration[STATE_INFO_LENGTH];
}
STATE_TAG;
STATE_TAG rx_state;
STATE_TAG tx_state;
#endif

void v21_modem_init(char direction);

#if 0
#define MESSAGE_DATA_LENGTH 1000

int MESSAGE_DATA_START = 0;
int MESSAGE_DATA_END = MESSAGE_DATA_LENGTH;

typedef struct MESSAGE_DATA_TAG
{
	int index;
	unsigned int message[MESSAGE_DATA_LENGTH];
}
MESSAGE_DATA;

MESSAGE_DATA message={0,{0}};
#endif

static int frh3_in_progress = 0;
static int frh3_connect_needed = 0;
static int frh3_frame_octet_cnt = 0;
static int frh3_last_frame = 0;
static int ced_seq = 0;
static int eop_flag_counter = 0;
static unsigned long eop[3] = {0,0,0};
static int frame_cnt = 0;
static int rec_frm_cnt = 0;
static int frm_xx_dle_etx_sent = 0;

int	tx_modem_type;
int	tx_modem_speed;

///////
//debug buffer to look at the bits from v.21 modem receiver.
#if 0
#define cugelnmax 256
int cugelncntr = 0;
int cugeln[cugelnmax];
void clearcugeln(void)
{
	int i;
	cugelncntr = 0;
	for(i=0;i<cugelnmax;i++)
		cugeln[i] = 0;
}

void addcugeln(int c)
{
	if(cugelncntr<cugelnmax)
	{
		cugeln[cugelncntr++] = c;
	}
	else
	{
		cugelncntr++;
	}
}
///////
#endif

void faxmodem_init(void)
{
	int i;
	
	INTR_GLOBAL_DISABLE;
	v21reloc();
	
	frh3_in_progress = 0;
	frh3_connect_needed = 0;
	frh3_frame_octet_cnt = 0;
	frh3_last_frame = 0;
	ced_seq = 0;
	eop_flag_counter = 0;
	for(i=0;i<3;i++)
		eop[i]=0;
	frame_cnt = 0;
	rec_frm_cnt = 0;
	frm_xx_dle_etx_sent = 0;
	
	Tx_block_init(ptrs);				/* initialize Tx_block */
	Rx_block_init(ptrs);				/* initialize Rx_block 	*/
	set_system_delay(HW_SYSTEM_DELAY, ptrs);	/* see c54x.h */
	//((struct TX_BLOCK *)(&Tx_block))->scale=16384;
	Rx_init_idle(ptrs);
	Tx_init_silence(ptrs);
	((struct TX_BLOCK *)(&Tx_block))->sample_counter = 0;
    
    set_Rx_detector_mask(CED_MASK|V21_CH2_MASK|V27_2400_MASK|V27_4800_MASK|V29_MASK|AUTO_DETECT_MASK, ptrs);
	Rx_init_detector(ptrs);

	//install modem's isrs.
	modem_BSP_0_rx_isr = vxx_BSP_0_rx_isr;
	modem_BSP_0_tx_isr = vxx_BSP_0_tx_isr;
    //set modem data function pointers
	set_tx_data = vxx_set_tx_data;
	get_tx_nbits = vxx_get_tx_nbits;
    get_rx_data = vxx_get_rx_data;
	get_rx_nbits = vxx_get_rx_nbits;
	tx_modem_ready = vxx_tx_modem_ready;
	//fpga voice/data switch to data.
	if(WSU003_Config)
    	VOICE_DATA_SWITCH = 1;
	else
		DataCommunication();

	ActivateBsp0(1);
	INTR_GLOBAL_ENABLE;
}


void frh3_init(int command_issued)
{
	if(modem_continue_func != frh3_modem_continue_func)
	{
		INTR_GLOBAL_DISABLE;
		v21_modem_init('r');
		hdlc_init();
		modem_continue_func = frh3_modem_continue_func;
		frh3_frame_octet_cnt = 0;
		frh3_last_frame = 0;
	    fm_state = DATA;
		INTR_GLOBAL_ENABLE;
	}

	if(command_issued)
	{
		frh3_connect_needed = 1;
	}
}

void ced_init(void)
{
	Tx_init_CED(ptrs);
	((struct TX_BLOCK *)(&Tx_block))->terminal_count= 24;//24000; ///24000;
	((struct TX_BLOCK *)(&Tx_block))->sample_counter = 0;
	ced_seq = 0;
}

void v21_modem_init(char direction)
{
    switch(direction)
    {
    case 't':
    		Tx_init_silence(ptrs);
    		((struct TX_BLOCK *)(&Tx_block))->sample_counter = 0;	
    //   	Tx_init_v21_ch2(ptrs);
    //    break;
    //case 'r':
    	//Rx_init_v21_ch2(ptrs);
    	//Tx_init_silence(ptrs);
    }
}


int v29_v27_modem_init(char direction, int modem_type, int speed)
{
	int i;

    switch(direction)
    {
    case 't':
    	tx_modem_type = modem_type;
    	tx_modem_speed = speed;
		Tx_init_silence(ptrs);
		((struct TX_BLOCK *)(&Tx_block))->sample_counter = 0;

        break;
    case 'r':
		switch(modem_type)
		{
		case 29:
			switch(speed)
			{
			case 9600:
			case 7200:
			case 4800:
				break;
			default:
				return 0;
			}
			break;
		case 27:
			switch(speed)
			{
			case 4800:
			case 2400:
				Tx_init_silence(ptrs);
				((struct TX_BLOCK *)(&Tx_block))->sample_counter = 0;
				enable_Rx_v27_startup_seq(ptrs);    /* ensure startup mode */
				break;
			default:
				return 0;
	        }
	    default:
	    	return 0;
		}
		set_Rx_rate(speed, ptrs);
    	break;
    default:
    	return 0;
    }
    
    for (i=0;i<((struct TX_BLOCK *)(&Tx_block))->data_len;i++)
    	Tx_data[i]=0;
	
	return 1;
}


void v29_modem_init(char direction)
{
	int i;
    
    switch(direction)
    {
    case 't':
		Tx_init_v29_9600(ptrs);
        break;
    case 'r':
		set_Rx_rate(9600, ptrs);
    }
    
    for (i=0;i<((struct TX_BLOCK *)(&Tx_block))->data_len;i++)
    	Tx_data[i]=0;
}

void v27_modem_init(char direction)
{
	int i;
	
    switch(direction)
    {
    case 't':
       	Tx_init_v27_4800(ptrs);
        break;
    case 'r':
		set_Rx_rate(4800, ptrs);
    }
    
    for (i=0;i<((struct TX_BLOCK *)(&Tx_block))->data_len;i++)
    	Tx_data[i]=0;
}


//
// 8kHz sample I/O interrupt service routines.
//

int rx_bsp0_interrupt_counter = 0;
int tx_bsp0_interrupt_counter = 0;
int rx_bsp0_interrupt_counter_max = 0;
int tx_bsp0_interrupt_counter_max = 0;

#pragma CODE_SECTION (vxx_BSP_0_rx_isr, "vtext")
void vxx_BSP_0_rx_isr(void)
{
	rx_bsp0_interrupt_counter++;
	*((struct RX_BLOCK *)(&Rx_block))->sample_head = DRR10;
	if (++((struct RX_BLOCK *)(&Rx_block))->sample_head >= ptrs->Rx_sample_start + ((struct RX_BLOCK *)(&Rx_block))->sample_len)
		((struct RX_BLOCK *)(&Rx_block))->sample_head = ptrs->Rx_sample_start;
}

#pragma CODE_SECTION (vxx_BSP_0_tx_isr, "vtext")
void vxx_BSP_0_tx_isr(void)
{
	tx_bsp0_interrupt_counter++;
	//INTR_GLOBAL_DISABLE;
	DXR10 = *((struct TX_BLOCK *)(&Tx_block))->sample_tail;
	if (++((struct TX_BLOCK *)(&Tx_block))->sample_tail >= ptrs->Tx_sample_start+((struct TX_BLOCK *)(&Tx_block))->sample_len)
		((struct TX_BLOCK *)(&Tx_block))->sample_tail = ptrs->Tx_sample_start;
	//INTR_GLOBAL_ENABLE;
}

void init_tx_data(void)
{
    int i;
    int *p = ptrs->Tx_data_start;
    
    for(i=0;i<((struct TX_BLOCK *)(&Tx_block))->data_len;i++)
    {
    	*p = 0xff;
    	p++;
    }
}

void vxx_set_tx_data(int val)
{
	*((struct TX_BLOCK *)(&Tx_block))->data_head = val & (((struct TX_BLOCK *)(&Tx_block))->Nmask);
	if(++((struct TX_BLOCK *)(&Tx_block))->data_head >= ptrs->Tx_data_start + ((struct TX_BLOCK *)(&Tx_block))->data_len)
		((struct TX_BLOCK *)(&Tx_block))->data_head = ptrs->Tx_data_start;
}

int vxx_get_tx_nbits(void)
{
	return	((struct TX_BLOCK *)(&Tx_block))->Nbits;
}

int vxx_get_rx_data(void)
{			
	int ret_val = *((struct RX_BLOCK *)(&Rx_block))->data_tail;

	if(++((struct RX_BLOCK *)(&Rx_block))->data_tail >= ptrs->Rx_data_start + ((struct RX_BLOCK *)(&Rx_block))->data_len)
		((struct RX_BLOCK *)(&Rx_block))->data_tail = ptrs->Rx_data_start;

	return ret_val;
}

int vxx_get_rx_nbits(void)
{
	return	((struct RX_BLOCK *)(&Rx_block))->Nbits;
}

void vxx_mv_v14_Rx_data_as_8_to_Rx_queue(void)
{	
	int v14_rec_buf[4];
	int v14_rec_buf_len;
	int j;
	
#if 0
	static int tps = 0;
	static int rps = 0;
	if(((struct TX_BLOCK *)(&Tx_block))->state_ID != tps)
	{
		tps = ((struct TX_BLOCK *)(&Tx_block))->state_ID;
		DebugPrintf1("ts=",tps);
	}
	if(((struct RX_BLOCK *)(&Rx_block))->state_ID != rps)
	{
		rps = ((struct RX_BLOCK *)(&Rx_block))->state_ID;
		DebugPrintf1("rs=",rps);
	}
#endif
	
	switch(((struct RX_BLOCK *)(&Rx_block))->state_ID)
	{	
    case RX_V21_CH1_MESSAGE_ID:
	case RX_V21_CH2_MESSAGE_ID:	
	case RX_V27_MESSAGE_ID:
	case RX_V29_MESSAGE_ID:
	case RX_V22A_MESSAGE_ID:
	case RX_V22C_MESSAGE_ID:
	case RX_V32A_MESSAGE_ID:
	case RX_V32C_MESSAGE_ID:
		while(((struct RX_BLOCK *)(&Rx_block))->data_head != ((struct RX_BLOCK *)(&Rx_block))->data_tail)
		{
			j = get_rx_data();
#ifdef _BITSTREAM_FEEDBACK_TEST_
			set_tx_data(j);
#else // _V14_ instead of _BITSTREAM_FEEDBACK_TEST_
		v14_rec_buf_len = v14_receive(v14_rec_buf,j);
        Rx_data_queuep(v14_rec_buf, v14_rec_buf_len);
#endif // _V14_ instead of _BITSTREAM_FEEDBACK_TEST_
 		}
	}
}

int  vxx_tx_modem_ready(void)
{
	int h,t,l,d;
	
	switch(((struct TX_BLOCK *)(&Tx_block))->state_ID)
	{
	case TX_V21_CH1_MESSAGE_ID:
	case TX_V21_CH2_MESSAGE_ID:	
	case TX_V27_MESSAGE_ID:
	case TX_V29_MESSAGE_ID:
	case TX_V22A_MESSAGE_ID:
	case TX_V22C_MESSAGE_ID:
	case TX_V32A_MESSAGE_ID:
	case TX_V32C_MESSAGE_ID:
		if(((struct TX_BLOCK *)(&Tx_block))->data_head == ((struct TX_BLOCK *)(&Tx_block))->data_tail)
		{
			return 2; //buffer is empty.
    	}
		h = (int)(((struct TX_BLOCK *)(&Tx_block))->data_head - ptrs->Tx_data_start);
		t = (int)(((struct TX_BLOCK *)(&Tx_block))->data_tail - ptrs->Tx_data_start);	
		l = (int)(((struct TX_BLOCK *)(&Tx_block))->data_len);
		d = (int)((10/((struct TX_BLOCK *)(&Tx_block))->Nbits)+2);
	
		t -= h;
		if(t<0)
			t += l;
		if(t >= d)
		{
			return 1; //able to get more for sending, but not empty.
    	}
	}
	return 0; //not able to get more data for sending.
}

void tx_modem_finish_transmission(void)
{
	int h,t,l;
	
	switch(((struct TX_BLOCK *)(&Tx_block))->state_ID)
	{
	case TX_V21_CH1_MESSAGE_ID:
	case TX_V21_CH2_MESSAGE_ID:	
	case TX_V27_MESSAGE_ID:
	case TX_V29_MESSAGE_ID:
	case TX_V22A_MESSAGE_ID:
	case TX_V22C_MESSAGE_ID:
	case TX_V32A_MESSAGE_ID:
	case TX_V32C_MESSAGE_ID:
		if(((struct TX_BLOCK *)(&Tx_block))->data_head == ((struct TX_BLOCK *)(&Tx_block))->data_tail)
		{
			l = 0; //buffer empty.
    	}
		else
		{
			h = (int)(((struct TX_BLOCK *)(&Tx_block))->data_head - ptrs->Tx_data_start);
			t = (int)(((struct TX_BLOCK *)(&Tx_block))->data_tail - ptrs->Tx_data_start);	
			l = (int)(((struct TX_BLOCK *)(&Tx_block))->data_len);
	
			t -= h;
			if(t<0)
				t += l;
			l -= t; //number of symbols remained in the buffer.
		}
		
		((struct TX_BLOCK *)(&Tx_block))->symbol_counter &= 0x0003;
		l += ((struct TX_BLOCK *)(&Tx_block))->symbol_counter;
		if(l < -1)
			l = -1;
		((struct TX_BLOCK *)(&Tx_block))->terminal_count = l+1;
		break;
	default:
		((struct TX_BLOCK *)(&Tx_block))->terminal_count = 0;
	}
}

int queue0_ready(void)
{
	static int last_ret_val = 1;
	
	if((!queue_ready_amaunt(&(queue[0]),QLMAIN_EXTREMELY_LOW)))
	{
		if(last_ret_val)
		{
			//TimeDebugSPrintf2("Q0 full, losing Rx data\n\r");
			last_ret_val=0;
		}
	}
	else
	{
		if(!last_ret_val)
		{
			//TimeDebugSPrintf2("Q0 avail.for Rx data\n\r");
			last_ret_val = 1;
		}
	}
	return last_ret_val;
}

long queue_full_counter;

int mv_raw_Rx_data_as_8_to_fr_tsk(void)
{	
	int ret_val = 0;
	int raw_rec_buf[4];
	int raw_rec_buf_len;
	int j,i;
	
	switch(((struct RX_BLOCK *)(&Rx_block))->state_ID)
	{	
	case RX_V27_MESSAGE_ID:
	case RX_V29_MESSAGE_ID:
		while(((struct RX_BLOCK *)(&Rx_block))->data_head != ((struct RX_BLOCK *)(&Rx_block))->data_tail)
		{
			j = get_rx_data();
#if 1
			for(i=1<<(get_rx_nbits()-1);i;i>>=1)
			{
				eop[0] = i & j	? (eop[0]<<1)|EOP_LSB : (eop[0]<<1);
				eop[1] = eop[0] & EOP_MSB 	? (eop[1]<<1)|EOP_LSB : (eop[1]<<1);
				eop[2] = eop[1] & EOP_MSB 	? (eop[2]<<1)|EOP_LSB : (eop[2]<<1);
									
				if((eop[2]&EOP_BITS) == EOP_MASK &&  
					(eop[1]&EOP_BITS) == EOP_MASK && 
					(eop[0]&EOP_BITS) == EOP_MASK)
					{
						eop[2] = eop[1] = eop[0] = 0;
						eop_flag_counter++;							
						ret_val = 1; //end of page.
					}
			}
#endif

#if 0			
			message.message[message.index++] = j;
			if(message.index >= MESSAGE_DATA_LENGTH)
			{
				message.index = 0;
			}
#endif			
			raw_rec_buf_len = raw_receive(raw_rec_buf,j);
			for(j=0;j<raw_rec_buf_len;j++)
			{
				if(raw_rec_buf[j] == DLE || queue0_ready())
				{
					//send1(FR,uart_c_in_event,0,(int*)(raw_rec_buf[j]));
					put_into_buf(raw_rec_buf[j]);
					simbols_from_vxx++;
					if(raw_rec_buf[j] == DLE)
					{
						//send1(FR,uart_c_in_event,0,(int*)(raw_rec_buf[j]));
						put_into_buf(raw_rec_buf[j]);
						simbols_from_vxx++;
					}
				}
				else if(!queue0_ready())
				{
					queue_full_counter++;
				}
        	}
 		}
	}
	return ret_val;
}


int data_modem_continue_func(void)
{
	if(fm_state != COMMAND)
	{
		receiver(ptrs);
		transmitter(ptrs);

		mv_v14_Rx_data_as_8_to_Rx_queue();
	    mv_Rx_queue_to_uart_a();
		mv_Tx_queue_as_v14_to_Tx_data();
		return 1;
	}
	return 0;
}

void write_string_to_fr_tsk(const char *buf)
{
	int i;
	for(i=0;buf[i];i++)
	{
		send1(FR,uart_c_in_event,0,(int*)(buf[i]));
	}
}

int ced_modem_continue_func(void)
{	
	switch(fm_state)
	{
	case ON_LINE:
	case DATA:
		
		

		while(transmitter(ptrs))
		{

		if(tx_bsp0_interrupt_counter_max<tx_bsp0_interrupt_counter)
		{
			tx_bsp0_interrupt_counter_max=tx_bsp0_interrupt_counter;
		}

		
		if(((struct TX_BLOCK *)(&Tx_block))->state_ID == TX_CED_ID)
		{
			ced_seq = 1;
			return 1;
		}
			
		if(((struct TX_BLOCK *)(&Tx_block))->state_ID == TX_SILENCE_ID && ced_seq == 1)
		{		
			if(((struct TX_BLOCK *)(&Tx_block))->sample_counter >= 600)
			{
				INTR_GLOBAL_DISABLE;
				Tx_init_v21_ch2(ptrs);				
				while(tx_modem_ready())
				{
					v21_raw_write(0xFF,8);
				}
				INTR_GLOBAL_ENABLE;
				//debind=0;
				hdlc_init();
				write_string_to_fr_tsk("\nCONNECT\r");
				TimeDebugSPrintf1("CON 1");
				modem_continue_func = fth3_modem_continue_func;
			}
		}
		
		}
		return 1;
	}
	return 0;
}


//char debtstdat[12] = {0xff,0x13,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,DLE,ETX};
//int debtstind = 0;
void reverse(char *val)
{
	int i;
	char reversed_val = 0;
	for(i=0x80;i;i>>=1)
	{
		reversed_val >>= 1;
		
		if(i & (*val))
		{
			reversed_val |= 0x80;
		}
		else
		{
			reversed_val &= ~0x80;
		}
		
	}
	*val = reversed_val;
}

#define SILENCE_100MS 800
#define SILENCE_75MS 600
#define SILENCE_62MS 500
#define SILENCE_50MS 400
#define SILENCE_37MS 300
#define SILENCE_25MS 200
#define SILENCE_12MS 100
#define SILENCE_1MS 1

int silence_75ms_continue_func(void)
{
	switch(fm_state)
	{
	case COMMAND:
		return 0;
	default:
		if(transmitter(ptrs))
		{
			if(tx_bsp0_interrupt_counter_max<tx_bsp0_interrupt_counter)
			{
				tx_bsp0_interrupt_counter_max=tx_bsp0_interrupt_counter;
			}
		}
		
		
		if(((struct TX_BLOCK *)(&Tx_block))->sample_counter >= SILENCE_37MS)
		{
			Tx_init_v21_ch2(ptrs);
			TimeDebugSPrintf1("SIL 75 E");
			modem_continue_func = fth3_modem_continue_func;
			write_string_to_fr_tsk("\nCONNECT\r");
//			TimeDebugSPrintf1("CON 0");
			
		}
		return 1;
	}
}

	int silence_v29_75ms_cf(void)
{
	switch(fm_state)
	{
	case COMMAND:
		return 0;
	default:
		if(transmitter(ptrs))
		{
			if(tx_bsp0_interrupt_counter_max<tx_bsp0_interrupt_counter)
			{
				tx_bsp0_interrupt_counter_max=tx_bsp0_interrupt_counter;
			}
		}

		if(((struct TX_BLOCK *)(&Tx_block))->sample_counter >= SILENCE_75MS)
		{
			switch(tx_modem_type)
			{
			case 29:
			{
				INTR_GLOBAL_DISABLE;
				switch(tx_modem_speed)
				{
				case 9600:
					Tx_init_v29_9600(ptrs);
					break;
				case 7200:
					Tx_init_v29_7200(ptrs);
					break;
				case 4800:
					Tx_init_v29_4800(ptrs);
					break;
				default:
					break;
				}
				modem_continue_func = ftmXX_modem_continue_func;
				INTR_GLOBAL_ENABLE;
				break;
			}
			case 27:
			{
				INTR_GLOBAL_DISABLE;
				switch(tx_modem_speed)
				{
				case 4800:
					Tx_init_v27_4800(ptrs);
					break;
				case 2400:
	            	Tx_init_v27_2400(ptrs);
	            	break;
	        	default:
	        		break;
				}
				disable_Tx_v27_TEP(ptrs);
				enable_Tx_v27_startup_seq(ptrs);    /* ensure startup mode */
				enable_Rx_v27_startup_seq(ptrs);    /* ensure startup mode */
				modem_continue_func = ftmXX_modem_continue_func;
				INTR_GLOBAL_ENABLE;

			}
			break;
	    	default:
	    		return 0;
			}
		}
		return 1;
	}
}

int	preamble_delay;

int fth3_modem_continue_func(void)
{
	static int frame_in_progress;
    static unsigned short crc;
    static char last_frame;
	static char last_octet = 0;
	static int octet_deferred = 0;
	static char deferred_val;
    char val;
    static int	octet_cnt, frame_octet_cnt;
    static int	frame_end;

	int ret = 1;
    
	//while
	if(transmitter(ptrs))
	{
		if(tx_bsp0_interrupt_counter_max<tx_bsp0_interrupt_counter)
		{
			tx_bsp0_interrupt_counter_max=tx_bsp0_interrupt_counter;
		}
	}
	
	{
	switch(fm_state)
	{
	case COMMAND:
		ret = 0;
		break;
	case ON_LINE:
		fm_state = DATA;
	case DATA:
        ret = 1;
		
		if(((struct TX_BLOCK *)(&Tx_block))->state_ID == TX_SILENCE_ID)
		{
			//send0(FR, FR_OK_IN);
			write_string_to_fr_tsk("\nOK\r");
			TimeDebugSPrintf1("OK fth3");
			fm_state = COMMAND;
			TimeDebugSPrintf1("COMMAND fth3");
			
			receive = rcvintr(-1);
		}
		
		while(((struct TX_BLOCK *)(&Tx_block))->state_ID == TX_V21_CH2_MESSAGE_ID && tx_modem_ready())	
		{
			if(octet_deferred)
			{
				hdlc_put(deferred_val);
				octet_deferred = 0;				
			}
			
			if(!tx_modem_ready())
				break;
			
			if(frame_end)
			{
				v21_raw_write(FLAG,8);

				frame_end = 0;
				frame_octet_cnt = 0;
				octet_deferred = 0;
				frame_in_progress = 0;
				
				hdlc_init();
				crc = 0xFFFF;
				if(last_frame)
				{
					tx_modem_finish_transmission();
					TimeDebugSPrintf1("FIN 21");
				}
				else
				{
					write_string_to_fr_tsk("\nCONNECT\r");
					TimeDebugSPrintf1("CON 2");
				}
			}
			
			if(!tx_modem_ready())
				break;
							
			if(preambula_counter < PREAMBULA_FLAGS_NUMBER)
			{
				preambula_counter++;
				v21_raw_write(FLAG,8);
				octet_cnt = 0;
				frame_octet_cnt = 0;
				frame_end = 0;
				frame_in_progress = 0;
				crc = 0xFFFF;
				
				if(!Tx_data_queueEMPTY)
					preamble_delay++;
				break;
			}
		    
		    if(!Tx_data_queueEMPTY)
		    {
		    	if(!frame_in_progress)
		    	{
		    		/* buffer a few octets to combat jitter */
		    		if(Tx_data_queue.nodes_taken < 3)
		    			break;
		    	}
		    	
	        	val = Tx_data_queueGET;
	        	
		    	frame_in_progress = 1;
		    	
		    	if(val == 0xFF)
			    		TimeDebugSPrintf1("21 FF");
		    	
	        	reverse(&val);
	        	octet_cnt++;
	        	
	        	if(val == REVERSED_DLE)
	        	{
	        		last_octet = REVERSED_DLE;
	        	}
	        	else
	        	{
	        		if(last_octet != REVERSED_DLE)
	        		{
						updateCRC(&crc, val);
						hdlc_put(val);
						if(frame_octet_cnt == 1)
							last_frame = (val & LAST_FRAME_MASK);
						frame_octet_cnt++;
					}
					else
					{
						if(val == REVERSED_ETX)
						{
							TimeDebugSPrintf1("DLE ETX 21");
							crc = ~crc;
							hdlc_put((crc >> 8) & 0xFF);

							deferred_val = crc & 0xFF;
							octet_deferred = 1;
							
	        				frame_end = 1;
	        				frame_in_progress = 0;
	        				frame_cnt++;
						}
						else if(val == REVERSED_DLE)
						{
							updateCRC(&crc, val);
							hdlc_put(val);
						}
						else
						{
							updateCRC(&crc, val);
							hdlc_put(val);							
						}
					}
					last_octet = val;	
	        	}
	        	break;
	        }
			else
			{
				if(!frame_in_progress)
				{
					v21_raw_write(FLAG,8);
					break;
				}
				else
				{
					frame_end = 1;
					frame_in_progress = 0;
					TimeDebugSPrintf1("fth3 underrun");
				}
			}

			break;
		} /*end while */
		break;
	}

	
	/* debug */
	//if(frame_cnt >= 4)
	//	asm("	nop");

	}	

	return ret;
}


int frh3_modem_continue_func(void)
{

	unsigned short hdlc_rec_buf[4];
	unsigned short hdlc_rec_buf_len;

	QUEUEVALUE q;
	
	int status;
	
    int c;
    int j;
    
	while(receiver(ptrs))
	{
		if(rx_bsp0_interrupt_counter_max<rx_bsp0_interrupt_counter)
		{
			rx_bsp0_interrupt_counter_max=rx_bsp0_interrupt_counter;
		}
		rx_bsp0_interrupt_counter -= 20;

	switch(fm_state)
	{
	case COMMAND:
		return 0;
	case ON_LINE:
		switch(((struct RX_BLOCK *)(&Rx_block))->state_ID)
		{
		case RX_CED_ID:
		case RX_V21_CH2_MESSAGE_ID:
				fm_state = DATA;
		}
		break;
	
	case DATA:
		switch(((struct RX_BLOCK *)(&Rx_block))->state_ID)
		{
		case RX_CED_ID:
		case RX_V21_CH2_MESSAGE_ID:
			if(!frh3_in_progress && frh3_connect_needed)
			{
				frh3_connect_needed = 0;
				frh3_in_progress = 1;
				write_string_to_fr_tsk("\nCONNECT\r");
				TimeDebugSPrintf("OCN 3");
			}
				
			while(((struct RX_BLOCK *)(&Rx_block))->data_head != ((struct RX_BLOCK *)(&Rx_block))->data_tail)
			{
				c = get_rx_data();
				hdlc_rec_buf_len = 0;
				status = hdlc_receive(c,hdlc_rec_buf,&hdlc_rec_buf_len); //frame finished.

				for(j=0;j<hdlc_rec_buf_len;j++)
				{
					frh3_frame_octet_cnt++;
					if(frh3_frame_octet_cnt == 2)
					{
						frh3_last_frame = hdlc_rec_buf[j] & 0x10;
					}
					queue_put(&(queue[1]),(QUEUEVALUE*)(hdlc_rec_buf[j]),0);
					if(hdlc_rec_buf[j] == 0xFF)
						TimeDebugSPrintf1("FF 21");
                }

				if(status & HDLC_FRAME_END)
				{
					rec_frm_cnt++;
					if(rec_frm_cnt >= 2)
					{
						asm("	nop");
					}
					
					queue_put(&(queue[1]),(QUEUEVALUE*)DLE,0);
					queue_put(&(queue[1]),(QUEUEVALUE*)ETX,0);
					queue_put(&(queue[1]),(QUEUEVALUE*)'\n',0);
					if(status & ~HDLC_FRAME_END)
					{
						/* errored frame received */
						queue_put(&(queue[1]),(QUEUEVALUE*)'E',0);
						queue_put(&(queue[1]),(QUEUEVALUE*)'R',0);
						queue_put(&(queue[1]),(QUEUEVALUE*)'R',0);
						queue_put(&(queue[1]),(QUEUEVALUE*)'O',0);
						queue_put(&(queue[1]),(QUEUEVALUE*)'R',0);
						TimeDebugSPrintf1("ERR");
						
					}
					else
					{
						/* correct frame received */
						queue_put(&(queue[1]),(QUEUEVALUE*)'O',0);
						queue_put(&(queue[1]),(QUEUEVALUE*)'K',0);
						TimeDebugSPrintf1("OK");
					}
					queue_put(&(queue[1]),(QUEUEVALUE*)('\r'|0xff00),0);
					frh3_frame_octet_cnt = 0;
				}
			}
			break;
		}

		while(!queue_empty(&(queue[1])) && frh3_in_progress)
		{
			q = queue_get(&(queue[1]));

			send1(FR,uart_c_in_event,0,(int*)(q & 0xFF));
			
			if(q & 0xFF00)
			{
				frh3_in_progress = 0;

				if(frh3_last_frame)
				{
					fm_state = COMMAND;
					TimeDebugSPrintf1("COMMAND frh3");
					queue_empty(&(queue[1]));///????
					return 0;
				}
			}
		}
		
	}
	
	}
	return 1;
}

//#define STATEBUFLEN 30
//unsigned int txstate[STATEBUFLEN];
//int txstateind=0;

int v29_count;
int ftmXX_modem_continue_func(void)
{
//	static int tx_state = 0;


	//while(transmitter(ptrs))
	if(transmitter(ptrs))
	{
		if(tx_bsp0_interrupt_counter_max<tx_bsp0_interrupt_counter)
		{
			tx_bsp0_interrupt_counter_max=tx_bsp0_interrupt_counter;
		}
	}

//		if(tx_state != ((struct TX_BLOCK *)(&Tx_block))->state_ID)
//		{
//			tx_state = ((struct TX_BLOCK *)(&Tx_block))->state_ID;
//			if(STATEBUFLEN>(++txstateind))
//			txstate[txstateind]=tx_state;
//		}

	switch(fm_state)
	{
	case COMMAND:
		return 0;

	case ON_LINE:
		switch(((struct TX_BLOCK *)(&Tx_block))->state_ID)
		{
		case TX_V29_MESSAGE_ID:
		case TX_V27_MESSAGE_ID:
		//default:
			fm_state = DATA;
			write_string_to_fr_tsk("\nCONNECT\r");
			TimeDebugSPrintf1("CON 4");
			v29_count=0;
		}
		//break;
	case DATA:
		switch(((struct TX_BLOCK *)(&Tx_block))->state_ID)
		{
		case TX_V29_MESSAGE_ID:
		case TX_V27_MESSAGE_ID:
			if(mv_Tx_queue_as_raw_to_Tx_data())
			{
				tx_modem_finish_transmission();
				TimeDebugSPrintf1("FIN 29");			
			}
        	break;
        case TX_SILENCE_ID:
			if(fm_state == DATA)
			{
				fm_state = COMMAND;
				TimeDebugSPrintf1("COMMAND ftm29");
				receive = rcvintr(-1);
				write_string_to_fr_tsk("\rOK\r");
				TimeDebugSPrintf1("OK 29");
			}
		}
	}
	
	//}
	return 1;
}

int rx_state[10];
int rx_state_idx;
int frmXX_modem_continue_func(void)
{
//	if(rx_state[rx_state_idx] != ((struct RX_BLOCK *)(&Rx_block))->state_ID && (((struct RX_BLOCK *)(&Rx_block))->state_ID & 0xFF00) == 0x2700)
//	{
//		rx_state_idx++;
//		if(rx_state_idx < 10)
//			rx_state[rx_state_idx] = ((struct RX_BLOCK *)(&Rx_block))->state_ID;
//	}

	if((fm_state == ON_LINE) && 
		//(((((struct RX_BLOCK *)(&Rx_block))->state_ID & 0xFF00) == 0x2900) || ((((struct RX_BLOCK *)(&Rx_block))->state_ID & 0xFF00) == 0x2700)))
		((((struct RX_BLOCK *)(&Rx_block))->state_ID == RX_V29_MESSAGE_ID) || (((struct RX_BLOCK *)(&Rx_block))->state_ID == RX_V27_MESSAGE_ID)))		
	{
		fm_state = DATA;
		frm_xx_dle_etx_sent = 0;
		write_string_to_fr_tsk("\nCONNECT\r");
		TimeDebugSPrintf1("CON 5");
	}
	if((fm_state == DATA) && 
		((((struct RX_BLOCK *)(&Rx_block))->state_ID != RX_V29_MESSAGE_ID) && (((struct RX_BLOCK *)(&Rx_block))->state_ID != RX_V27_MESSAGE_ID)))
	{
		fm_state = COMMAND;
		TimeDebugSPrintf1("COMMAND frm29");
		if(!frm_xx_dle_etx_sent)
		{
			send1(FR,uart_c_in_event,0,(int*)DLE);
			send1(FR,uart_c_in_event,0,(int*)ETX);
			frm_xx_dle_etx_sent = 1;
			TimeDebugSPrintf1("DLE,ETX");
		}
		write_string_to_fr_tsk("\nNO CARRIER\r");
		TimeDebugSPrintf1("NOC");
	}

	if(fm_state != COMMAND)
	{
		while(receiver(ptrs))
		{
			if(rx_bsp0_interrupt_counter_max<rx_bsp0_interrupt_counter)
			{
				rx_bsp0_interrupt_counter_max=rx_bsp0_interrupt_counter;
			}
			rx_bsp0_interrupt_counter -= 20;

			if(!frm_xx_dle_etx_sent && mv_raw_Rx_data_as_8_to_fr_tsk())
			{
				send1(FR,uart_c_in_event,0,(int*)DLE);
				send1(FR,uart_c_in_event,0,(int*)ETX);
				frm_xx_dle_etx_sent = 1;
				write_string_to_fr_tsk("\nOK\r");
				TimeDebugSPrintf1("DLE,ETX,OK");
				//frh3_init(0); //switch to v.21 ch 2 receiver.
				//fm_state = COMMAND;
				//return 0;
			}
		}
		return 1;
	}
	return 0;
}

#endif // _MODEMS_

