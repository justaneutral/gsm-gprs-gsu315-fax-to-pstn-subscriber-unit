#include "su.h"
#include "modems.h"
#include "fr_frames.h"


//data
int raw_discontinuity = 1;
int sync_raw_discontinuity = 1;

//API
void raw_init(void);
void raw_send(int val_in, int length, int mode);
void raw_write(int val, int bit_length);
void sync_raw_write(int val, int bit_length);


void raw_init(void)
{
	raw_discontinuity = 1;
	sync_raw_discontinuity = 1;
	raw_receive((int*)0,0); //init receiving v.14 framer.
}

void raw_put(char val)
{
	raw_write((int)val,8);
}

void sync_raw_put(char val)
{
	char rev_val = val;
	reverse(&rev_val);
	sync_raw_write((int)rev_val,8);
}

void v21_raw_write(int val, int bit_length)
{
	unsigned int outval;
	int i;
	
	for(i = 0; i < bit_length; i++)
	{
		outval = (val >> (bit_length - i - 1)) & 0x1;
		set_tx_data(outval);		
	}
}

void sync_raw_write(int val, int bit_length)
{
	if(sync_raw_discontinuity)
	{
		/* first symbol */
		raw_send(val,bit_length,1);
		sync_raw_discontinuity = 0;		
	}
	else
	{
		raw_send(val,bit_length,2);
	}
}

void raw_write(int val, int bit_length)
{
	if(Tx_data_queueEMPTY)
	{
		if(raw_discontinuity)
		{
			raw_send(val,bit_length,0); //single symbol
		}
		else
		{
			raw_send(val,bit_length,3); //last symbol in the stream
			raw_discontinuity=1;
		}
	}
	else
	{
		if(raw_discontinuity)
		{
    		raw_send(val,bit_length,1); //first symbol in the stream
			raw_discontinuity=0;
		}
		else
		{
			raw_send(val,bit_length,2); //next but not the last symbol in the stream.
		}
	}
}

void raw_send(int val_in, int length, int mode)
{
//mode describes the stream handling situation.
//	0: send single symbol.
//	1: send first symbol in the stream.
//	2: send next but not the last symbol in the stream.
//	3: send the last symbol in the stream.
//  4: finish the last symbol with marks.

	int i;
	
	static int outval=0xffff;
	static int next;
	
	switch(mode)
	{
	case 0: //send single symbol.
	case 1: //send first symbol in the stream.
		next=1<<(get_tx_nbits()-1);
	case 2: //send next but not the last symbol in the stream.
	case 3: //send the last symbol in the stream.
		//send data.
		for(i=0;i<length;i++)
		{
			if(val_in & (1<<(length-i-1)))
				outval |= next;
			else
				outval &= ~next; 
			next>>=1;
			if(!next)
			{
				set_tx_data(outval);
				next = 1<<(get_tx_nbits()-1);
			}
		}
		if(mode==1 || mode ==2)
			break;
	case 4:	//finish the last symbol with marks.
		if(next != (1<<(get_tx_nbits()-1)))
		{
			while(next)
			{
				outval |= next;
				next>>=1;
			}
			set_tx_data(outval);
			next = 1<<(get_tx_nbits()-1);
    	}
    }
}


int raw_receive(int *raw_buf, int inp_val)
{
	int raw_buf_len = 0;
	int i;

	static int next=8;
	static char raw_val=0;
	    	
	if(!raw_buf)
	{
		next=8;
		raw_val=0;
	}
	else
	{
		for(i=1<<(get_rx_nbits()-1);i;i>>=1)
		{
    		raw_val>>=1;
    		if(i & inp_val)
    			raw_val |= 0x80;
			if((--next)<=0)
			{
				raw_buf[raw_buf_len++] = raw_val;
				raw_buf[raw_buf_len] = 0;
				raw_val = 0;
				next=8;
	   		}
   		}
   	}
   	return raw_buf_len;
}


extern int v29_count;

int mv_Tx_queue_as_raw_to_Tx_data(void)
{
	static int last = 0;
	
	char val;
	
	int ret_val = 0;
	
	while((!Tx_data_queueEMPTY) && (tx_modem_ready()))
	{
		val=Tx_data_queueGET;

		if(last == DLE)
		{
			last = 0;
			
			switch(val)
			{
			case DLE:
				sync_raw_put(DLE);
				break;
			case ETX:
				ret_val = 1;
				break;
			default:
				asm("	nop");
				break;
			}
		}
		else
		{
			last = val;
			if(val != DLE)
			{
				sync_raw_put(val);
				v29_count++;
			}
		}
#if 0 //moved to uart_c_fputc.
		if((Tx_data_queue_fed_up) && (queue_ready_amaunt(&Tx_data_queue,QLTx_data_queue_HIGH)))
		{
			Tx_data_queue_fed_up = 0x0;
		}
#endif
	}
	
	return ret_val;
}
