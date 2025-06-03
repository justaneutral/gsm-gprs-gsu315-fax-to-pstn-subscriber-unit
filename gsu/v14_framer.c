#include "su.h"
#include "modems.h"

///#define _V32_DATA_DEBUG_


//V.14 data

int v14discontinuity = 1;


//v.14 API

void v14init(void)
{
	v14discontinuity = 1;
}

int v14_receive(int *rec_buf, int inp_val)
{
	int rec_buf_len = 0;
	
	int i;

	static int next=8;
	static char v14val=0;
	    	
	if(!rec_buf)
	{
		next=8;
		v14val=0;
	}
	else
	{
		for(i=1<<(get_rx_nbits()-1);i;i>>=1)
		{
			switch(next)
			{
			case 8:
				if(!(i & inp_val))
				{
					next--;
					v14val=0;
				}
				break;
			case -1:
				if(i & inp_val)
				{
					rec_buf[rec_buf_len++] = v14val;
					rec_buf[rec_buf_len] = 0;
#ifdef _V32_DATA_DEBUG_
					uart_b_fputc_(v14val,0);
#endif	
					
				}
				next=8;
				break;
			default:
	    		next--;
	    		v14val>>=1;
	    		if(i & inp_val)
	    			v14val |= 0x80;
	   		}
   		}
   	}
   	return rec_buf_len;
}

void v14send(char v14in, int mode)
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
		for(i=0;i<10;i++)
		{
			switch(i)
			{
			case 0: //start bit.
				outval &= ~next;
				break;
			case 9: //stop bit.
				outval |= next;
				break;
			default:
				if(v14in&(1<<(i-1)))
					outval |= next;
				else
					outval &= ~next; 
			}
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


void v14put(char val)
{
#ifdef _V32_DATA_DEBUG_
	uart_b_fputc_(val,0);
#endif	
	if(Tx_data_queueEMPTY)
	{
		if(v14discontinuity)
    		v14send(val,0); //single symbol
		else
		{
			v14send(val,3); //last symbol in the stream
			v14discontinuity=1;
		}
	}
	else
	{
		if(v14discontinuity)
		{
    		v14send(val,1); //first symbol in the stream
			v14discontinuity=0;
		}
		else
		{
			v14send(val,2); //next but not the last symbol in the stream.
		}
	}
}


void mv_Tx_queue_as_v14_to_Tx_data(void)
{
	char val;
	
	while((!Tx_data_queueEMPTY) && (tx_modem_ready()))
	{
		val=Tx_data_queueGET;
		v14put(val);
		if((Tx_data_queue_fed_up) && (queue_ready_amaunt(&Tx_data_queue,QLTx_data_queue_HIGH)))
		{
			Tx_data_queue_fed_up = 0x0;
			UART_A_MCR_REG |= RTS;
			//INTR_ENABLE(INT2);
			//uart_a_data_interrupt(1);
		}
	}
}


