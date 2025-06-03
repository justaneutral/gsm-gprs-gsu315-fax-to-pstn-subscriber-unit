#include "su.h"
#include "modems.h"

#define RECEIVE_HDLC_INIT_VAL 6

typedef enum HDLC_STATUS_TAG
{
	HDLC_NONE,
	HDLC_FLAG,
	HDLC_DATA,
	HDLC_ABORT
} 
HDLC_STATUS;


typedef enum HDLC_STATE_TAG
{
	INITIAL,
	STARTING_FLAGS,
	DATA_RECEIVED,
	STOPPING_FLAG
} HDLC_STATE;

static HDLC_STATE hdlc_receiver_state = INITIAL;
static int ones_counter = 0;
static int receive_hdlc_c5_counter = RECEIVE_HDLC_INIT_VAL;
unsigned short received_hdlc_crc;
static unsigned short hdlc_val;

void hdlc_init()
{
	//init hdlc receiver.
	receive_hdlc_c5_counter = 0;//RECEIVE_HDLC_INIT_VAL;
	hdlc_receiver_state = INITIAL;
	hdlc_val = 0xFFFF;

	//receive hdlc transmitter.
	ones_counter = 0;
	//init raw data tramsmitter. It is used by hdlc transmitter.
	raw_init();
}


static int flag_cnt;
static int bit_cnt;

int hdlc_receive(int inp_val, int *hdlc_buf, int *hdlc_buf_len)
{
	static unsigned short hdlc_bit_cnt;
	int i;
	int bit;
	int idx;
	
	int ret = 0;
	
	idx = hdlc_buf_len[0];
	
	
	for(i=1<<(get_rx_nbits()-1);i;i>>=1)
	{
		bit = (i & inp_val) ? 0x1 : 0x0;
		bit_cnt++;
		
		switch(hdlc_receiver_state)
		{
			case INITIAL:
				/* searching for openning flag(s) */
				hdlc_val <<= 1;
				hdlc_val |= bit;
				hdlc_val &= 0xFF;
				if(hdlc_val == 0x7E)
				{
					hdlc_receiver_state = STARTING_FLAGS;
					hdlc_val = 0;
					hdlc_bit_cnt = 0;
					receive_hdlc_c5_counter = 0;
				}
				break;
			case STARTING_FLAGS:
				received_hdlc_crc = 0xFFFF;
				flag_cnt++;
			case DATA_RECEIVED:
				if(!bit)
				{
					if(receive_hdlc_c5_counter == 5)
					{
						hdlc_receiver_state = DATA_RECEIVED;
						receive_hdlc_c5_counter = 0;
						break;
					}
					else if(receive_hdlc_c5_counter == 6)
					{
						if(hdlc_receiver_state == DATA_RECEIVED)
						{
							if(hdlc_bit_cnt != 7)
								ret |= HDLC_NON_OCTET_ALIGN;
							ret |= HDLC_FRAME_END;
							if(received_hdlc_crc != 0x1D0F)
								ret |= HDLC_CRC_ERROR;
						}
						hdlc_receiver_state = STARTING_FLAGS;
						hdlc_bit_cnt = 0;
						receive_hdlc_c5_counter = 0;
						hdlc_val = 0;
						break;
					}
				}
				else
				{
					if(receive_hdlc_c5_counter == 6)
					{
						if(hdlc_receiver_state == DATA_RECEIVED)
						{
							ret |= HDLC_FRAME_END;
						}
						ret |= HDLC_ABORT;
						hdlc_receiver_state = INITIAL;
						break;
					}
				}
				
				hdlc_val <<= 1;
				hdlc_val |= bit;
				hdlc_val &= 0xFF;
				receive_hdlc_c5_counter += bit;
				receive_hdlc_c5_counter *= bit;
				hdlc_bit_cnt++;
				if(hdlc_bit_cnt == 8)
				{
					hdlc_bit_cnt = 0;
					updateCRC(&received_hdlc_crc,hdlc_val);
					reverse((char *)&hdlc_val);
					hdlc_buf[idx] = hdlc_val;
					hdlc_buf_len[0]++;
					hdlc_val = 0;
					hdlc_receiver_state = DATA_RECEIVED;	
				}
				break;
			default:
				break;
		} /* end switch */				
	} /* end for */
	
	return ret;
}
				
				
				


int hdlc_put_(char val,int *outval)
{
	int i;
	int mask;
	int len = 8;
	int hdlc_val = 0;
	mask = 0x0080;
	
	for(i=0;i<8;i++)
	{
		hdlc_val<<=1;
		
		if(mask & val)
		{
			ones_counter++;
			hdlc_val |= 1;
			if(ones_counter >= 5)
			{
				ones_counter=0;
				len += 1;
				hdlc_val<<=1;
			}
		}
		else
		{
			ones_counter = 0;
		}
		
		mask >>= 1;
	}	
	
	*outval = hdlc_val;
	return len;	
}


void hdlc_put(char val)
{
	int outval,len;
	len=hdlc_put_(val,&outval);
	raw_write(outval,len);
}

void hdlc_put_reversed(char val)
{
	int outval,len;
	char reversed_val;
	int i;
	reversed_val = 0;
	for(i=0x80;i;i>>=1)
	{
		reversed_val<<=1;
		if(val & i)
		{
			reversed_val|=1;
		}
		
	}
	
	len=hdlc_put_(val,&outval);
	raw_write(outval,len);
}

