#include "su.h"
#include "modems.h"

#ifdef _FAR_MODE_
#include "watchdog.h"
#endif

#pragma CODE_SECTION (dce_uart, "vtext_pump")
#pragma CODE_SECTION (dce_pump, "vtext_pump")
#pragma CODE_SECTION (at_response, "vtext_pump")
#pragma CODE_SECTION (send_pump, "vtext_pump")

int dce_state;
int manualIPR=0;

extern int	uart_a_tx_count;
extern int	uart_b_tx_count;
extern int	uart_a_tx_over_threshold;
extern int	uart_b_tx_over_threshold;

static char ICF_string[13]="";
static char S0_string[9]="";
static char IPR_string[16]="";
static int pump_cnt = 0;

#if 0
static char ipr0correctstr[6]={'\b','5','7','6','0','0'};
#endif

void updateUserParamOnFlash(void);
void restoreUserParams(void);
void restoreSystemParams(void);
void intializeRS232Setigs(void);
void at_response(void);
void dce_uart(int i);
void dce_pump(void);
unsigned long dcerate(void);
void send_pump(void);

int at_parse(char symb, char *at_mask, int msk_len,int *index)
{
  if(symb == '\b')
  {
  	(*index)--;
  	return 0;
  }	
  if((*index)<0)
  	(*index)=0;
    	
  if((*index)>=msk_len)
  {
  	(*index)=0;
  	return 1;
  } 
  
  if(at_mask[*index]==toupper(symb))
  	(*index)++;
  else
  	(*index)=0;
  return 0;
}

void set_dce_state_indicator()
{  	
	switch(dce_state)
  	{
  		case DCE_STATE_PUMP:
  				queue_flush(&(queue[1]));
  				queue_flush(&(queue[2]));
  				leds_update_background(3,orange,green,100); 
  				leds_update(2,dark,0);
				StopPeriodicCREGCheck((void*)0);
				StopPeriodicCSQCheck((void*)0);
  	    	break;
  	    case DCE_STATE_NORMAL:
  				queue_flush(&(queue[1]));
  				queue_flush(&(queue[2]));
  	    		leds_update(3,green,0);
  	    		StartPeriodicCREGCheck((void*)0);
  	    	break;
  	    case DCE_STATE_DEBUG:
  				queue_flush(&(queue[1]));
  				queue_flush(&(queue[2]));
  	    		leds_update(3,green,0);
  	    		StartPeriodicCREGCheck((void*)0);
  	    	break;
  	    default:
  				queue_flush(&(queue[1]));
  				queue_flush(&(queue[2]));
  	    		leds_update(3,dark,0);
  	    		StopPeriodicCREGCheck((void*)0);
  	    	break;
  	}	
}

void dce_tsk_init(void)
{
   	leds_update(2,dark,0);
   	leds_update(3,dark,0);
}

void dce_tsk(int command_type)
{
	int i;
	
    switch(command_type)
    {
    /***all***/
    case DCE_change_state:
       	dce_state = queue_get(&(queue[0]));
       	set_dce_state_indicator();
       	//take care of GSM stack initialization.
       	switch(dce_state)
       	{
       	case DCE_STATE_NORMAL:		
			send0(AT,AT_restart);
       		break;
		case DCE_STATE_DEBUG:
			send0(AT,AT_init_normal);
			break;
		case DCE_STATE_PUMP:
			send0(FM,FM_stop);
			setup_uarts_for_pump();
			pumpreloc();
			if(!manualIPR)
				enable_autobaud();
	   		break;
		}
		break;
	/***norm***/    
 	case DCE_at_command:
		while(i=0xff&queue_get(&(queue[0])))
			if(dce_state != DCE_STATE_PUMP)
				uart_a_fputc(i);
		break;
	/***pump***debug***/
	case AT_response:
   		at_response();
   		break;
   	/***pump***/
   	case DCE_uart:
   		i=queue_get(&(queue[0]));
   		if(dce_state == DCE_STATE_PUMP)
   		{
   			dce_uart(i);	
   		}
   		break;
   	case DCE_pump:
   		dce_pump();
   		break;
   	default:
   		break;
    }
}


void updateUserParamOnFlash(void)
{
	if(strstr(S0_string,"ATS0="))
	{
		UpdateFlashData("AUTOANSWER",S0_string);
		memset(S0_string,0,strlen(S0_string));
	}
	if(strstr(ICF_string,"AT+ICF="))
	{	
		UpdateFlashData("FRAMING",ICF_string);
		memset(ICF_string,0,strlen(ICF_string));
	}
	if(strstr(IPR_string,"AT+IPR="))
	{	
		UpdateFlashData("BAUDRATE",IPR_string);
		memset(IPR_string,0,strlen(IPR_string));
	}
	
}

void restoreUserParams(void)
{
	int i;
	char *flashdata, *ptr;
	char tmpstr[10]="";
	int  atdelay; 
	unsigned int rate =dcerate();
	
	if(rate < 4800)
	{
		atdelay = 1000;
		if(rate < 1200)
			atdelay = 1500;
	}
	else
		atdelay = 500;
	
	uart_a_fputs("ATE0\r\n");
	delay(atdelay);
	
	if((flashdata = ReadFlashData("AUTOANSWER")) != NULL)
	{
		uart_a_fputs(flashdata);
	}
	delay(atdelay);
	
	if((flashdata = ReadFlashData("BAUDRATE")) != NULL)
	{
		if(strstr(flashdata,"AT+IPR="))
		{
			ptr = flashdata;
#if 0
			if(flashdata[7] == '0')
				ptr = "AT+IPR=57600\r";
#endif
			uart_a_fputs(ptr);
			delay(atdelay);
			if(flashdata[7] == '0') 
				manualIPR = 0; //Enable Autoboud
				
			else
				manualIPR =1; // Disbale Autoboud
			
								
			memset(tmpstr,0,strlen(tmpstr));
			strcpy(tmpstr,&flashdata[7]);
			
			for(i=0; i< strlen(tmpstr);i++)
			{
				if(tmpstr[i] == '\r')
					tmpstr[i]='\0';
			}
			
			delay(atdelay);
			setIPR(tmpstr);
			
		}			
	}
	
	if((flashdata = ReadFlashData("FRAMING")) != NULL)
	{
		if(strstr(flashdata,"AT+ICF="))
		{
			uart_a_fputs(flashdata);
			delay(atdelay);
			setICF(flashdata[7],flashdata[9]);
		}
	}

	delay(atdelay);
	
	uart_a_fputs("ATE1V1;+WIND=0;+CR=0;+CRC=0\r\n");
	
	delay(atdelay);

}

void restoreSystemParams(void)
{
	int atdelay; 
	unsigned int rate = dcerate();
	
	if(rate < 4800)
	{
		if(rate == 2400)
			atdelay = 1000;
		if(rate == 1200)
			atdelay = 1500;
		if(rate == 600)
			atdelay = 3000;
		if(rate == 300)
			atdelay = 6000;
	}
	else
		atdelay = 500;
	 	
	uart_a_fputs("AT+IPR=19200\r\n");
	delay(atdelay);
	
	uart_a_rate(UART_BAUD_19200);
	uart_b_rate(UART_BAUD_19200);
	
	uart_a_fputs("AT+ICF=3,4\r\n");
	delay(atdelay);
	
	setICF('3','4');

}

void intializeRS232Setigs(void)
{

	int i;
	char *flashdata;
	char tmpstr[10]="";
	
	if((flashdata = ReadFlashData("BAUDRATE")) != NULL)
	{
		if(strstr(flashdata,"AT+IPR="))
		{
			if(flashdata[7] == '0') 
				manualIPR = 0; //Enable Autoboud
			else
				manualIPR =1; // Disbale Autoboud
								
			memset(tmpstr,0,strlen(tmpstr));
			strcpy(tmpstr,&flashdata[7]);
			
			for(i=0; i< strlen(tmpstr);i++)
			{
				if(tmpstr[i] == '\r')
					tmpstr[i]='\0';
			}
			
			setIPR(tmpstr);
			
		}
		else
		{
			uart_a_rate(UART_BAUD_19200);
			uart_b_rate(UART_BAUD_19200);
		}
					
	}
	else
	{
		uart_a_rate(UART_BAUD_19200);
		uart_b_rate(UART_BAUD_19200);
	}
	

	if((flashdata = ReadFlashData("FRAMING")) != NULL)
	{
		if(strstr(flashdata,"AT+ICF="))
		{
			setICF(flashdata[7],flashdata[9]);
		}
		else
			setICF('3','4'); // 8 Data, 1 Stop, Parity None
	}
	else
		setICF('3','4'); // 8 Data, 1 Stop, Parity None

}
	
unsigned long dcerate(void)
{
	int i;
	char *flashdata;
	char tmpstr[10]="";
		
	if((flashdata = ReadFlashData("BAUDRATE")) != NULL)
	{
		memset(tmpstr,0,strlen(tmpstr));
		strcpy(tmpstr,&flashdata[7]);
		
		for(i=0; i< strlen(tmpstr);i++)
		{
			if(tmpstr[i] == '\r')
				tmpstr[i]='\0';
		}
		
		return atol(tmpstr);
	}
	else
		return 19200;
}


#if 0
void inline enqueue_character(int i)
{
		if(!queue_empty(&queue[1]))
		{
			queue_put(&(queue[1]),(QUEUEVALUE*)i,0);
	   		//send0(DCE, DCE_pump);
			send_pump();
		}
		else
		{ 
   			if((UART_A_MSR_REG & CTS))
   			{
   				if(!pump_uart_a_fputc(i))
	   			{
	   				queue_put(&(queue[1]),(QUEUEVALUE*)i,0);
	   				//send0(DCE, DCE_pump);
					send_pump();
	   			}
   			}
   			else
   			{
   				queue_put(&(queue[1]),(QUEUEVALUE*)i,0);
   				//send0(DCE, DCE_pump);
				send_pump();
   			}
		}
}

#else
#define enqueue_character(i)\
{\
		if(!queue_empty(&queue[1]))\
		{\
			queue_put(&(queue[1]),(QUEUEVALUE*)i,0);\
			send_pump();\
		}\
		else\
		{\
   			if((UART_A_MSR_REG & CTS))\
   			{\
   				if(!pump_uart_a_fputc(i))\
	   			{\
	   				queue_put(&(queue[1]),(QUEUEVALUE*)i,0);\
					send_pump();\
	   			}\
   			}\
   			else\
   			{\
   				queue_put(&(queue[1]),(QUEUEVALUE*)i,0);\
				send_pump();\
   			}\
		}\
}
#endif

int CollectSupvParams(char c)
{
	static char conn_supv[PARAM_DATA_SZ] = "";
	static char disconn_supv[PARAM_DATA_SZ] = "";
	static char aoc_supv[PARAM_DATA_SZ] = "";
	static int ndx = 0;
	static int spndx = 0;
	static int qndx  = 0;
	static int failed = 0;
	static int char_cnt = 0;
	char C;
	
	if((0x0D == c) && (-1 == qndx))
	{
		failed = UpdateFlashData("CONNSUPV", conn_supv);
		if(!failed)
			failed = UpdateFlashData("DISCONSUPV", disconn_supv);
		if(!failed)
			failed = UpdateFlashData("AOCSUPV", aoc_supv);

		memset(conn_supv,'\0',PARAM_DATA_SZ);
		memset(disconn_supv,'\0',PARAM_DATA_SZ);
		memset(aoc_supv, '\0',PARAM_DATA_SZ);
		ndx = 0;
		spndx = 0;
		qndx = 0;
		char_cnt = 0;
		if(!failed)
			return 0;
	}
	else if((0x0D == c) && (qndx != 6) && (spndx != 2) && (!(ndx%4)))
	{	// premature carrige return
		failed = 1;
	}
	else if(('"' == c))
	{	// next record
		char_cnt++;
		if(++qndx == 6)
		{
			if(0 == ((char_cnt-qndx-spndx)%4))
			{	// input sequence successfully entered
				ndx = 0;
				spndx = 0;
				qndx = -1;
				char_cnt = 0;
			}
			else	// not enough characters in the record
			{
				failed = 1;
			}
		}
	}
	else if(',' == c)
	{	// record separator
		char_cnt++;
		spndx++;
		ndx = 0;
	}
	else if((('0' <= c) && ('9' >= c)) || (('A' <= c) && ('F' >= c)) ||(('a' <= c) && ('f' >= c)))
	{	// legitimate character for storing to flash table
		if(isalpha(c))
			C = toupper(c);
		else C = c;
		switch(spndx)
		{
			case 0:
				conn_supv[ndx++] = C;
				char_cnt++;
			break;
				
			case 1:
				disconn_supv[ndx++] = C;
				char_cnt++;
			break;
			
			case 2:
				aoc_supv[ndx++] = C;
				char_cnt++;
			break;
			
			default:
				failed = 1;
			break;
		}
		if(ndx > PARAM_DATA_SZ)
			failed = 1;		
	}
	else	// invalid character typed
		failed = 1;
	if(failed)
	{	// reset all values and retry input
		failed = 0;
		memset(conn_supv,'\0',PARAM_DATA_SZ);
		memset(disconn_supv,'\0',PARAM_DATA_SZ);
		memset(aoc_supv, '\0',PARAM_DATA_SZ);
		ndx = 0;
		spndx = 0;
		qndx = 0;
		char_cnt = 0;
//		uart_b_fputs("\r\nInvalid entry, reenter.\r\n");
		return 2;
	}
	return 1; // keeps us in the loop
}


void dce_uart(int i)
{

	static int atlmnt_index = 0;
	static int atlswv_index = 0;
	static int atlrst_index = 0;
	static int atldef_index = 0;
	static int aticf_index = 0;
	static int ats0_index= 0;
	static int atw_index = 0;
	static int atipr_index = 0;
	static int supv_index = 0;
	
	static int collect_icf = 0;
	static int icfparam = 0;
	static int format = 0;
	static int parity = 0;
	static int change_icf = 0;
	
	static int collect_s0 = 0;
	static int s0param = 0;
	static int autoanswer = 0;
	
	static int collect_ipr = 0;
	static int iprparam = 0;
	static char ipr[8]="";
	static int change_ipr = 0;
	static int supv_collect = 0;
	static int supv_read_index = 0;
	
	char supv_str[80]="";
	int atdelay = 300; // 30ms
	char inputval[1];
	
	if(data_mode == COMMAND)
		{
			if(at_parse(i,"AT+LCRG=",8,&supv_index))
			{
				supv_collect = 1;
			}
			if(at_parse(i,"AT+LCRG?",8,&supv_read_index))
			{
				uart_a_fputs("\b\b\b\b\b\b\b\b");
				uart_b_fputs("\r\n");
				strcat(supv_str, "LCRG: ");
				strcat(supv_str, ReadFlashData("CONNSUPV"));
				strcat(supv_str,",");
				strcat(supv_str,ReadFlashData("DISCONSUPV"));
				strcat(supv_str,",");
				strcat(supv_str,ReadFlashData("AOCSUPV"));
				uart_b_fputs(supv_str);
				uart_b_fputs("\r\nOK\n");
			}
			if(at_parse(i,"AT+LMNT",7,&atlmnt_index))
			{
				uart_b_fputs("\r\n");
				restoreSystemParams();
				send1_delayed(100,DCE,DCE_change_state,0,(int*)DCE_STATE_DEBUG);
				send0(SLAC,SLAC_dce_normal);
   			}
			if(at_parse(i,"AT+LSWV",7,&atlswv_index))
			{
				uart_a_fputs("\b\b\b\b\b\b\b");
				uart_b_fputs("\n");
				uart_b_fputs(SWversion);
				uart_b_fputs("OK\n");
			}
			if(at_parse(i,"AT+LDEF",7,&atldef_index))
			{
				uart_a_fputs("\b\b\b\b\b\b\b");
				uart_b_fputs("\r\n");
				InitFlashParams();
				uart_b_fputs("OK\n");
			}
			if(at_parse(i,"AT+LRST",7,&atlrst_index))
			{
				uart_a_fputs("\b\b\b\b\b\b\b");
				uart_b_fputs("\r\nOK\r\n");
				delay(500);
				reset();
			}
			if(at_parse(i,"AT+ICF=",7,&aticf_index))
			{
				collect_icf = 1;
				icfparam = 0;    				
			}
			if(at_parse(i,"AT+IPR=",7,&atipr_index))
   			{
				collect_ipr = 1;
				iprparam = 0;    				
			}
			if(at_parse(i,"ATS0=",5,&ats0_index))
			{
				collect_s0 = 1;
				s0param = 0;    				
			}
			if(at_parse(i,"AT&W",4,&atw_index))
			{
				updateUserParamOnFlash();    				
			}				
			if(supv_collect)
			{
				supv_collect = CollectSupvParams(i);
				sprintf(inputval, "%c", i);
				uart_b_fputs(inputval);
				if(!supv_collect)
				{
					uart_a_fputs("\b\b\b\b\b\b\b\b\b");
					uart_b_fputs("\r\nOK\r\n");
				}
				if(supv_collect == 2)
					supv_collect = 0;
				
				if(1 == supv_collect)
					return;
			}
  			if(collect_icf)
  			{
				if(i != '\b')	
	  			{		  			
  					if(icfparam == 0 && i != '\r')
	  					format = i;
	  				if(icfparam == 2)
	  					parity = i;
	  				if(icfparam == 3 && i == '\r')
	  					change_icf = 1;
	  				icfparam++;
	  				if(icfparam >= 4) 
	  				{
	  					collect_icf = 0;
	  					icfparam = 0;
	  				}
  				}
  				
  				if(i == '\b')
  				{
  					if(icfparam != 0)
  						icfparam--;
  					else
  					{
  						i = '\r';
  						collect_icf =0;
  						icfparam = 0;
  					}
  				}
  			}
  			if(collect_ipr)
  			{
  				if(i != '\b')	
	  			{
	  				if(i !='\r')
	  				{
						ipr[iprparam]= i;		  				
	  					iprparam++;
	  				}
	  				if(i == '\r')
	  				{
	  					if(iprparam != 0)
	  					{
	  						ipr[iprparam]= '\0';
	  						change_ipr = 1;
	  						collect_ipr =0;
	  						iprparam =0;
	  					}
	  				}
	  			}
	  			if(i == '\b')
  				{
  					if(iprparam != 0)
  						iprparam--;
  					else
  					{
  						uart_a_fputc('\b');
  						i = '\r';
  						collect_ipr =0;
  						iprparam =0;	
  					}
  				}	
  			}
  			if(collect_s0)  
  			{
				if(i != '\b')	
	  			{		  			
					if(s0param == 0 && i != '\r')
	  					autoanswer = i;
	  				if(s0param ==1)
	  				{
	  					if(i == '\r')
		  				{
		  					memset(S0_string,0,strlen(S0_string));
		  					sprintf(S0_string,"ATS0=%c\r\n",autoanswer);
		  					collect_s0 =0;
		  				}
		  				else 
		  				{
		  				  uart_a_fputc('\b');
		  				  i = '\r';
		  				  collect_s0 = 0;
		  				}
		  			}
	  				s0param++;				
  				}	
  				if(i == '\b')
  				{
  					if(s0param != 0)
  						s0param--;
  					else
  					{
  						i = '\r';
  						collect_s0 =0;	
  					}
  				}
  				if(collect_s0 == 0)
  					s0param = 0;	
  			}
		
			//uart_a_fputc(i);
			//putchar(i);
		}
		
#if 0		
		if(change_ipr)
		{
			if(ipr[0] == '0')
			{
				for(iprparam=0;iprparam<8;iprparam++)
				{
					enqueue_character(ipr0correctstr[iprparam]);
				}
				iprparam=0;
			}
		}
#endif
		
		enqueue_character(i);

		//uart_a_fputc(i);
		if(change_icf == 1)
		{
			if( format != '?')
			{
   				delay(atdelay);
   				setICF(format,parity);
   				memset(ICF_string,0,strlen(ICF_string));
   				sprintf(ICF_string,"AT+ICF=%c,%c\r\n",format,parity);
   			}
			change_icf = 0;
			icfparam = 0;	
		}
		if(change_ipr == 1)
		{
			if(ipr[0] != '?')
			{
   				delay(atdelay);
   				if(ipr[0] == '0')
   					manualIPR =0;
   				else
   					manualIPR =1; // Disbale Autoboud
   				setIPR(ipr);
   				memset(IPR_string,0,strlen(IPR_string));
		  		sprintf(IPR_string,"AT+IPR=%s\r\n",ipr);
		  	}
			change_ipr = 0;
			iprparam = 0;
		}  		

}

void send_pump(void)
{
	
	if(pump_cnt == 0)
	{
		send0(DCE, DCE_pump);
		pump_cnt++;
	}
}

void dce_pump(void)
{
	int i;
	
	pump_cnt--;	
	//if(!queue_empty(&queue[1]))
	while(!queue_empty(&queue[1]))
	{
		if((UART_A_MSR_REG & CTS))
		{
			i = queue_peek(&queue[1]);
			if(pump_uart_a_fputc(i))
			{
				i = queue_get(&queue[1]);
			}
			else
			{
				//send0(DCE, DCE_pump);
				send_pump();
				break;
			}
		}
		else
		{
			//send0(DCE, DCE_pump);
			send_pump();
			break;
		}
	}
	//if(!queue_empty(&queue[2]))
	while(!queue_empty(&queue[2]))
	{
		if((UART_B_MSR_REG & CTS))
		{
			i = queue_peek(&queue[2]);
			if(pump_uart_b_fputc(i))
			{
				i = queue_get(&queue[2]);
			}
			else 
			{
				//send0(DCE, DCE_pump);
				send_pump();
				break;
			}
		}
		else
		{
			//send0(DCE, DCE_pump);
			send_pump();
			break;
		}
	}
}

extern unsigned short last_known_uart_b_rate;

void at_response(void)
{
	static int resp_ok_idx = 0;
	int i;
	
	i=queue_get(&(queue[0]));
	
	if(autobaud_enabled)
	{
		uart_b_discard_char();
/*		if(at_parse(i, "RING\r\n", 5, &resp_ring_idx))
		{
			disable_autobaud();
			uart_b_rate(last_known_uart_b_rate);
			pump_uart_b_fputc('\r');
			pump_uart_b_fputc('\n');
			pump_uart_b_fputc('R');
			pump_uart_b_fputc('I');
			pump_uart_b_fputc('N');
			pump_uart_b_fputc('G');
			pump_uart_b_fputc('\r');
			pump_uart_b_fputc('\n');
			resp_ring_idx = 0;
			UART_B_IER_REG |= 0x2;

		}
		return;
*/
		disable_autobaud();
		uart_b_rate(last_known_uart_b_rate);
	}
	
	if(dce_state != DCE_STATE_NORMAL)
	{		 
		if(!queue_empty(&queue[2]))
		{
			queue_put(&(queue[2]),(QUEUEVALUE*)i,0);
			//send0(DCE, DCE_pump);
			send_pump();
		}
		else 
		{
			if(UART_B_MSR_REG & CTS)
			{
				if(!pump_uart_b_fputc(i))
	   			{
	   				queue_put(&(queue[2]),(QUEUEVALUE*)i,0);
	   				//send0(DCE, DCE_pump);
					send_pump();
	   			}
	   		}
	   		else
	   		{
   				queue_put(&(queue[2]),(QUEUEVALUE*)i,0);
   				//send0(DCE, DCE_pump);
				send_pump();
	   		}
   		}
		/*uart_b_fputc(i);*/
		
		if(dce_state == DCE_STATE_PUMP)
		{
			if(data_mode == COMMAND && !manualIPR)
			{
				if(at_parse(i, "OK\r\n", 3, &resp_ok_idx) && queue_empty(&queue[2]))
				{
					resp_ok_idx = 0;
					UART_B_IER_REG |= 0x2;
				}
			}
		}
	}
}

