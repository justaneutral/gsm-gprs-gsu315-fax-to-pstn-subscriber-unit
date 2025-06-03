#include "su.h"

#ifdef _MODEMS_
#include "modems.h"
#endif

#undef 	ENABLE_RESULT_LOCATION 
#define	MAX_RESP_BUF	256
#define max_params 		32

/* Wavecom reset */

#define WAVECOM 	portE000  
volatile ioport u16 WAVECOM;

/* Externs */

extern int CC_current_state;
extern int skip_clcc_reading;
extern int CLCC(char *);
extern int SIM_present;
extern int PIN_not_required;
extern int PUK_not_required;
extern int wrong_passwd_count;
extern int waiting_CID;
extern char SMSnumber[];
extern char SMSdatetime[];
extern char LASTCALL_duration[];
extern char LASTCALL_number[];
extern char INFO_name[];
extern char INFO_number[];
extern int fr_task_on;
extern int LASTCALL_direction;
extern int Multi_Call_timer;
extern int fm_started;
extern int ring_count;
extern int bAwake;
extern BOOLEAN bRingBackTest;

/* Globals */

static char	resp_buf[MAX_RESP_BUF];
char *parameters[max_params];
char SMS_message[SMS_LENGTH];
char SMSindex[4]="";
char WavecomModule[20]="";

static int ccm_enabled = 0;
static int postponed_CID =0;
static int rereg_counter = 0;
int	resp_buf_idx = 0;
int WAVECOM_init =1;
int GSM_ready=0;
int SS_setup=0;
int pas_check_enabled = 0;
int clcc_check_enabled = 0;
int pin_entry_sent = 0;
int pin_entry_countdown = 0;  
int rssi_display =0;
int autopin_enabled = 0;
int SMS_read = 0;
int SMS_list = 0;
int SMSonCID_enabled = 0;
int local_connection_on = 0;
int PolingWavecom = 0;
int Start_Call_timer = 0;
int at_call_complete = 0;
int timeSet = 0;
int timeSetUserRequest = 0;
int checkModuleVersion = 0;
int chargingEnabled = 0;
int homeUnitSF = 1 ;

DATA_MODE data_mode = COMMAND;

/* Protoypes */

void dropcall(void);
void stop_gsm_init_timer(void);
void stop_gsm_recover_timer(void);
void init_at_normal(void);
void init_at_data(void);
void init_at_timers(void);
void send_at_command(char *str, unsigned short delay);
void result_received(void);
void wavecom_on(void);
void wavecom_off(void);
void wavecom_restart(void);
void set_bearer(unsigned short delay);

int ProcessDialingParameters(void);

state_service_func StartPeriodicPINCheck(void *p);
state_service_func StartPeriodicReRegistration(void *p);
state_service_func StopPeriodicReRegistration(void *p);
extern void ss_call_handler(int);

/* Wavecom UART speed detection */

static int gsm_baud_idx = 0;
int gsm_baud_table[10] =
{
UART_BAUD_19200,
UART_BAUD_9600,
UART_BAUD_115200,
UART_BAUD_57600,
UART_BAUD_38400,
UART_BAUD_4800,
UART_BAUD_2400,
UART_BAUD_1200,
UART_BAUD_600,	   
UART_BAUD_300
};

static int gsm_icf_idx = 0; 
char gsm_icf_table[12][2] =
{
	'3','4', 	//	8 1 None
	'5','0',	//	7 1 Odd
	'5','1',	//	7 1 Even
	'5','2',	//	7 1 Mark
	'5','3',	//	7 1 Space
	'2','0',	//	8 1 Odd
	'2','1',	//	8 1 Even
	'2','2',	//	8 1 Mark
	'2','3',	//	8 1 Space
	'1','4',	//	8 2 None
	'4','4',	//	7 2 None
	'6','4'		//	7 1 None
};

typedef enum at_resp_tag
{
	at_resp_ok='0', 			// "OK"
	at_resp_connect, 			// "CONNECT"
	at_resp_ring, 				// "RING"
	at_resp_no_carrier, 		// "NO CARRIER"
	at_resp_error,				// "ERROR"
	at_resp_no_dialtone = '6', 	// "NO DIALTONE"
	at_resp_busy,				// "BUSY
	at_resp_no_answer			// "NO ANSWER"
} AT_RESP;

int command_processor(char *cmd, int par_num)
{
	if(strstr("CREG",cmd))
	{   
		switch(par_num)
		{
			case 1:	return CREG("1",parameters[0],"","");
			case 2: 
#ifdef ENABLE_RESULT_LOCATION
				    return CREG("2",parameters[0],parameters[1],"");
#else
					return CREG(parameters[0],parameters[1],"","");
#endif					
			case 3: return CREG("2",parameters[0],parameters[1],parameters[2]);
			case 4: return CREG(parameters[0],parameters[1],parameters[2],parameters[3]);
		}
	}
	else 
	if(strstr("CSQ",cmd))
		return CSQ(parameters[0],parameters[1]);
	else 
	if(strstr("CPAS",cmd))
		return CPAS(parameters[0]);
	else
	if(strstr("CRING",cmd))
		return CRING(parameters[0]);	
	else
	if(strstr("CME ERROR",cmd))
		return CME_ERROR(parameters[0]);
	else
	if(strstr("WIND",cmd))
		return WIND(parameters[0],parameters[1]);
	else
	if(strstr("CPIN",cmd))
		return CPIN(parameters[0]);
	else
	if(strstr("CLCK",cmd))
		return CLCK(parameters[0]);
	else
	if(strstr("CCFC",cmd))
		return CCFC(parameters[0]);     
	else
	if(strstr("CCWA",cmd))
        return CCWA(parameters[0]);
	else
	if(strstr("CLIP",cmd))
	{
		if(strlen(parameters[0]))     
			return CLIP(parameters[0]);
	}
	else
	if(strstr("CLIR",cmd))
        return CLIR(parameters[0],parameters[1]);
    else
	if(strstr("CMGR",cmd))
	{
        SMS_read =1;
        return CMGR(parameters[1],parameters[2],parameters[3],parameters[4]);
	}
	else
	if(strstr("CMTI",cmd))
        return CMTI(parameters[1]);
    else
    if(strstr("CPMS",cmd))
        return CPMS(parameters[0],parameters[1],parameters[2]);
    else
    if(strstr("CMGL",cmd))
    {
        SMS_list =1;
        return CMGL(parameters[0],parameters[1]);
    }
    else
	if(strstr("CMS ERROR",cmd))
		return CMS_ERROR(parameters[0]);
	else
    if(strstr("CCLK",cmd))
        return CCLK(parameters[0],parameters[1]);
    else
    if(strstr("CCCM",cmd))
        return CCCM(parameters[0]);
   
	return 0;
}

int info_interpreter()
{
	int len,i,j;
	char *command = 0;
	int found = 0;
	
	for(i=0;i<max_params;i++)
		parameters[i] = (char*)0;
		
	command = resp_buf+1;
				
	/* handle special parsing case */
	if(!strncmp(command, "CLCC:", 5))
	{
		DebugPrintf(command);
		return CLCC(command);
	}
	
	for(i=0;i<strlen(command);i++)
		if(command[i] == ':')
		{   
			found = 1;
			command[i] = '\0';
			parameters[0] = &(command[i+2]);
		}
	if(!found)
		return 0;
		
	j=0;
	len = strlen(parameters[0]);
	for(i=0;i<len;i++)
	{
		if(parameters[0][i] == ',')
		{
			j++;
			parameters[0][i] = '\0';
			parameters[j] = &(parameters[0][i+1]);
		}
        else if(parameters[0][i] == '\r')
		{
			parameters[0][i] = '\0';
		}
	}
	return command_processor(command,j+1);	
}

int info_received()
{
	if(strstr(resp_buf,"+") && strstr(resp_buf,":"))
	{
		return info_interpreter();
	}
	return 0;
}

void at_tsk_init(void)
{
  erase_delayed_messages();
  init_at_timers();
  delay(5000);  // 2.5 second delay
  wavecom_on();
  SS_setup=0; 
  timeSet=0; 
}

void at_tsk(int command_type)
{	
    int	i;
    
    switch(command_type)
    {
    case AT_response:
    
      	//check for the entire string is received.
      	if(parse_at())
  		{
      		if(!info_received()) //<+<c>:<<d>>>  
      		{
				result_received();
    	  	}
      		memset(resp_buf,0,sizeof(resp_buf));
      	}
   		break; 		
	case AT_answer:
		//DebugPrintf("at->gsm: Answering...\r\n");
		if(!(fax_mode == ON_LINE))
			send_at_command("ATA\r\n",0);
		break;
	case AT_enable_incoming_call:
		//DebugPrintf("Hanging up...\n");
		if(data_mode != COMMAND)
			dropcall();
		else
			send_at_command("ATH\r\n",10);
		data_mode = COMMAND; // We need this due to early start of modems.
		send_at_command("AT+CMOD=0\r\n",20);  // set to single mode
		//Clear curent calls
		for(i = 0; i < MAX_CURRENT_CALLS; i++)
			current_calls[i].present = 0;
		num_mpty_calls = 0;
		num_current_calls = 0;
		ring_count = 0;
		if(ccm_enabled)
			send1_delayed(200, AT, AT_show_callmetering,-1,NULL);
		if(postponed_CID)
			send1_delayed(200, SLAC, SLAC_cid_messages,-1,NULL);
		break;
	case AT_disable_incoming_call:
		send_at_command("ATV0\r\n",0);    // Figure out a good way of doing this 
		break; 
	case AT_dialing_params:
		ProcessDialingParameters();
	    break;
	case AT_init_normal:
		DebugPrintf("Initializing...\r\n");
		init_at_normal();
		break;
	case AT_init_data:
	    DebugPrintf("Initializing for data...\r\n");
		init_at_data();
		break;
	case AT_restart:
		leds_update(2,dark,0);
		wavecom_restart();
		break;
	case AT_data_mode:	// GSM AT interface in data mode
		skip_clcc_reading = 1;
    	StopPeriodicCREGCheck((void*)0);
    	StopPeriodicPASCheck((void*)0);			
		break;
   	case AT_command_mode:	// GSM AT interface in command mode.
   		if(CC_current_state == 5 || CC_current_state == 7)
   		{
   			StartPeriodicPASCheck((void*)CPAS_PERIOD);
   			StopPeriodicCREGCheck((void*)0);
   		}
   		break;
   	case AT_call_accept_waiting:
   		ss_call_handler(1);
   		skip_clcc_reading = 1;
		break;
   	case AT_call_hold:
   		ss_call_handler(2);
   		skip_clcc_reading = 1;
		break;
	case AT_call_mpty:
   		ss_call_handler(3);
   		skip_clcc_reading = 1;
		break;
	case AT_call_ect:
   		ss_call_handler(4);
   		skip_clcc_reading = 1;
		break;
	case AT_SS_flag:
		SS_setup = 1;
		break;
	case AT_start_callmetering:
		Start_Call_timer = 1;
		send_at_command("AT+CCLK?\r\n",0);
		break;
    case AT_stop_callmetering:
    	if(LASTCALL_direction < 2 ) // active call 
    	{
    		at_call_complete = 1;
    		Multi_Call_timer = 0;
    		if(ccm_enabled)
    			bAwake = 1;
       		send_at_command("AT+CCLK?\r\n",0); 
        }
    	break;
    case AT_show_callmetering:
        if(at_call_complete)
    		CID_OnhookPrintf(LASTCALL_duration,ReadFlashData("LASTNUMBER"));
    	at_call_complete = 0;
		break;
	default:
		break;
    }
}

void send_at_command(char *str, unsigned short delay)
{
	int i;
    if(!str || str[0]==0)
    	return;
    
   	for(i=0;str[i];i++);
   	i++;
   	
   	if(!delay)
   		send1(DCE,DCE_at_command,i,(int*)str);
   	else
   		send1_delayed(delay,DCE,DCE_at_command,i,(int*)str);
}       

int parse_at(void)
{
	if(resp_buf_idx >= MAX_RESP_BUF)
		resp_buf_idx = 0;
	resp_buf[resp_buf_idx++] = queue_get(&(queue[0]));
	if(resp_buf[resp_buf_idx-1] == 0x0D || 
	   resp_buf[resp_buf_idx-1] == 0x0A || 
	   resp_buf[resp_buf_idx-1] == 0x00)
	{
		resp_buf[resp_buf_idx - 1] = '\0';
		if(resp_buf_idx>1)
		{
			resp_buf_idx = 0;
			return 1;	
		}
		resp_buf_idx = 0;
		return 0;
	}
	return 0;
}

void init_at_normal(void)
{
	char *flashdata;
	char atstr[80]="AT+SIDET=0;+WIND=255;+VGR=006;+VGT=000;+CR=1;+CRC=1;+CMEE=1;+CREG=1;+CGATT=1\r\n";
		
	WAVECOM_init=0;
  	GSM_ready=0;
  	rereg_counter = 0;
  	
  	send_at_command("AT&F;+IPR=19200\r\n",0);          	// Reset to factory settings
  	
   	// Verbose off
  	// Echo off 
  	// V42bis data compression off
  	// GPRS Class B
  
  	send_at_command("ATE0;V0;+DS=0,0;+CGCLASS=\"B\"\r\n",100); 

  	// Side Tone disabled
  	// Wavecom notifications
  	// Gain control -speaker -10db -microphone +(33-36)dB
  	// Service reporting control
  	// Cellular result code 
  	// Error Reporting 
  	// Single numbering scheme call -voice
  	// Enable Network Registartion notification
  	// Attach to GPRS services
    
    flashdata=ReadFlashData("VOLUME");
    atstr[26]=flashdata[4];
    atstr[27]=flashdata[5];
    atstr[28]=flashdata[6];
    atstr[35]=flashdata[11];
    atstr[36]=flashdata[12];
    atstr[37]=flashdata[13];
    send_at_command(atstr,170);
  	
  	// SIM presence check
  	  	  
  	send_at_command("AT+CPIN?\r\n", 200);      
  	
  	// Set Data Bearer
  	
  	set_bearer(250);	
    
    flashdata=ReadFlashData("AUTOPIN");
    if(flashdata != NULL)
    {
	    switch(flashdata[0])
	    {
	    case '0':
	    	autopin_enabled = 0;
	    	break;
	    case '1':
	    	autopin_enabled = 1;
	    	break;
	    default:
	    	autopin_enabled = 0;
	    }
    }
    
    flashdata=ReadFlashData("CCM");
    if(flashdata != NULL)
    {
	    switch(flashdata[0])
	    {
	    case '0':
	    	ccm_enabled = 0;
	    	break;
	    case '1':
	    	ccm_enabled = 1;
	    	break;
	    default:
	    	ccm_enabled = 0;
	    }
    }
    
    flashdata=ReadFlashData("CHARGING");
    if(flashdata != NULL)
    {
	    switch(flashdata[0])
	    {
	    case '0':
	    	chargingEnabled = 0;
	    	break;
	    case '1':
	    	chargingEnabled = 1;
	    	break;
	    default:
	    	chargingEnabled = 0;
	    }
    }
    
    if(chargingEnabled)
    {
	    flashdata=ReadFlashData("CHARGPARMS");
	    if(flashdata != NULL)
	    {
		    homeUnitSF = atoi(flashdata);
        	
	    }
    }
   
    StartPeriodicReRegistration((void*)57600); // 9,6 Minutes   	        
}

void init_at_ss(void)
{  
  
  // GSM module version check
  
  send_at_command("AT+CGMR\r\n", 0); 
  
  checkModuleVersion = 1; 
  
  // Calling line identification
  // Call Waiting
  // Advice of Charge
  
  if(chargingEnabled)
  	send_at_command("AT+CLIP=1;+CCWA=1;+CAOC=2\r\n",150);
  else
  	send_at_command("AT+CLIP=1;+CCWA=1\r\n",150);
                
  GSM_ready=1;  
         
}

void init_at_data(void)
{
    restoreUserParams();          	// Reset user settings
}
  
void result_received(void)
{

	if(WAVECOM_init && resp_buf[0] != at_resp_ok)
		return;
	
	if(SMS_read)
    {
     	strcpy(SMS_message, resp_buf);
     	waiting_CID = numberOfCIDSessions(SMS_message);
     	bAwake = 1;
     	SMS_read = 0;
     	return;
   	}
	
	if(SMS_list)
	{
		SMS_list = 0;
		return;
	}
	
	if(checkModuleVersion)
	{
		strcpy(WavecomModule, strtok(resp_buf," "));
		if(isWismo2C2(WavecomModule))
  		{
  			// Gain control for Wismo 2C2 Modules 
  			// Gain control -speaker -10dB -microphone +36dB
  			///send_at_command("AT+VGR=128;+VGT=64\r\n", 0); ///???-pfd. 
  		} 
		checkModuleVersion = 0;
		return;	
	}
						
	switch((AT_RESP)(resp_buf[0]))
	{
		case at_resp_ok:
			if(WAVECOM_init == 1)      // Initialization
			{
				stop_gsm_init_timer();
				stop_gsm_recover_timer();
				
      			if(PolingWavecom == 1)
      			{
  					restoreUserParams();
  					uart_a_fputs("AT&W\r\n");
  				}
  				
#ifndef _UART_B_DSR_ //CD - cable connect/disconnect
      			if((UART_B_MSR_REG & CD) == 0)
#else 				//DSR - on/off
      			if((UART_B_MSR_REG & DSR) != 0)
#endif
      			{
      				send1(DCE,DCE_change_state,0,(int*)DCE_STATE_PUMP);
					send0(SLAC,SLAC_dce_pump);
					send0_delayed(300,AT,AT_init_data);
					data_mode = COMMAND;
  				}
  				else
  				{
  					restoreSystemParams();	
  					dce_state = DCE_STATE_NORMAL;
  					leds_update(3,green,0);
  					send0(AT,AT_init_normal);
  					send0(SLAC,SLAC_dce_normal);
  				}
  				
  				PolingWavecom = 0;
  				WAVECOM_init = 0;			
  	   		}
      		else if(CC_current_state == 6 && SS_setup)
      		{
      			send_at_command("AT+CPAS\r\n",0);
      		//	send1_delayed(100,CC, AT_SS_confirm, -1, NULL);
      			// SS_setup = 0;
      		}		
      		break;
		case at_resp_connect:
			DebugPrintf("at resp : CONNECT\r\n");
			if(data_mode == ON_LINE)
			{
				data_mode = DATA;
			}
			send0(CC,AT_connect);
			send1(FM,FM_modem_type,0,(int*)(resp_buf[1]));
			switch(resp_buf[1])
			{
			case '0': // 300 baud.		v.21
				DebugPrintf("V.21 300 modem/fax\r\n");
				break;
			case '1': // 1200 baud.		v.22
				DebugPrintf("V.22 1200 modem\r\n");
				break;
			case '2': // 1200/75 baud	v.23
				DebugPrintf("V.23 1200/75 modem\r\n");
				break;
			case '3': // 2400 baud		v.22bis, v.26ter
				DebugPrintf("V.22bis/V.26ter 2400 modem\r\n");
				break;
			case '4': // 4800 baud		v.32
				DebugPrintf("V.32 4800 modem\r\n");
				break;
			case '5': // 9600 baud		v.32
				DebugPrintf("V.32 9600 modem\r\n");
				break;
			case '6': // 14000 baud		v.110
				DebugPrintf("V.110 14000\r\n");
				break;
			}	
    		break;
    	case at_resp_ring:
    		DebugPrintf("at resp : RING\r\n");
      		send0(CC,AT_ringing);
    		break;
    	case at_resp_no_carrier:
    		DebugPrintf("at resp : NO CARRIER\r\n");
    		if(held_call_exists())
    			send0(CC,AT_disconnect_held);
    		else
    		{ 
      			send0(CC,AT_disconnect);
      			num_current_calls = 0; 
      		}
    		break;
    	case at_resp_error:
    		DebugPrintf("at resp : ERROR\r\n");
      		if(SS_setup == 1) // Suplementary Service status  
			{
				send0(CC,AT_SS_failure);
				SS_setup=0;
				if(!SIM_present || !PIN_not_required || !PUK_not_required)
					StartPeriodicPINCheck((void *)CPIN_PREIOD);
			}
			if(pin_entry_sent)
			{
				send0(CC,AT_SS_failure);
				pin_entry_sent = 0;
				wrong_passwd_count++;
			}
      		send0(CC,AT_error);
    		break;
    	case at_resp_busy:
    		DebugPrintf("at resp : BUSY\r\n");
      		send0(CC,AT_busy);
    		break;
    	case at_resp_no_answer:
    		DebugPrintf("at resp : NO ANSWER\r\n");
    		send0(CC,AT_disconnect);
    		break;
    	default:
    		strcat(resp_buf,"\r\n");
    		DebugPrintf(resp_buf);
       		break;
   	} // end of switch
   	
   	if(GSM_ready && ((resp_buf[0] == 'O') || (resp_buf[0] == 'o') || 
   	                 (resp_buf[0] == 'E') || (resp_buf[0] == 'e') ) ) //Incase FM/FR task  doesnot revert back to numberic mode.
   	{
   		send_at_command("ATE0;V0;+WIND=255;+CR=1;+CRC=1;+CMEE=1;+CREG=1\r\n",0);
   	}	  	
} //end of function.


state_service_func Reset_SS_setup(void *p)
{
	SS_setup = 0;
	StartPeriodicCREGCheck((void *)0);
	return (state_service_func)0;
}


static int gsm_recover_timer = -1;
static int scalecounter = 0;

int iprcounter = 0;
int icfcounter = 0;
timer_service_func send_IPR(void *p)
{
	int i;
	
	PolingWavecom = 1;

	if(gsm_baud_idx & 0x1)
		leds_update(3,green,0);
	else
		leds_update(3,dark,0);
			
	if(scalecounter != 0)
	{
		scalecounter--;
		return (timer_service_func)0;
	}
	
	if (gsm_baud_table[gsm_baud_idx] > UART_BAUD_9600 && gsm_baud_table[gsm_baud_idx] < UART_BAUD_1200 )
		scalecounter = 2;
	else if(gsm_baud_table[gsm_baud_idx] > UART_BAUD_1200)
		scalecounter = 5;	
			
	uart_a_rate(gsm_baud_table[gsm_baud_idx++]);
	gsm_baud_idx =  gsm_baud_idx % 10;
	
	uart_a_set_FCR; //to flush the FIFOs.
	
	send_at_command("ATV0;E0\r\n", 0);
	
	if(gsm_baud_idx == 0)
	{
		if((iprcounter++)>2)
		{
			iprcounter = 0;
			i=gsm_icf_idx++;
			setICF(gsm_icf_table[i][0],gsm_icf_table[i][1]);
			gsm_icf_idx = gsm_icf_idx % 12;
			if(gsm_icf_idx == 0)
			{
				if((icfcounter++)>2)
				{
					icfcounter = 0;
					reset();
				}
			}  
		}
	}
	
	return (timer_service_func)0;
}
state_service_func start_gsm_recover_timer(void *p)
{
	if(gsm_recover_timer == -1)
		return 0;
		
	if(!timer_enable(gsm_recover_timer, 25))
	{
	 	return 0;
	}
	
	return (state_service_func)1; 	
}

void stop_gsm_recover_timer(void)
{  	
	if(gsm_recover_timer == -1)
		return;
	
	timer_disable(gsm_recover_timer);
}

static int gsm_init_timer = -1;
static int startup_counter = 0;

timer_service_func send_ATV0E0(void *p)
{

	send_at_command("ATV0;E0\r\n", 0);
	
	if(startup_counter++ >10)
	{
		stop_gsm_init_timer();
		start_gsm_recover_timer((void *)0);
		startup_counter = 0;
	}
	
	return (timer_service_func)0;
}

state_service_func start_gsm_init_timer(void *p)
{
	if(gsm_init_timer == -1)
		return 0;
		
	if(!timer_enable(gsm_init_timer, 100))
	{
	 	return 0;
	}
	
	return (state_service_func)1; 	
}

void stop_gsm_init_timer(void)
{  	
	if(gsm_init_timer == -1)
		return;
	
	timer_disable(gsm_init_timer);
}

static int gsm_creg_timer = -1;

timer_service_func send_CREGcheck(void *p)
{
	if(fr_task_on)
		return (timer_service_func)0;
		
	send_at_command("AT+CREG?\r\n",0);
	if(OnHookStatus() && !rssi_display )             //Do not send CSQ checking while offhook and while we are sending to CallerID box.
		send_at_command("AT+CSQ\r\n", 10);
	return 0;
}


state_service_func StartPeriodicCREGCheck(void *p)
{
	if(gsm_creg_timer == -1)
		return 0;
		
	if(!timer_enable(gsm_creg_timer, CREG_PERIOD))
	{
	 	return (state_service_func)0;
	}
	
	return (state_service_func)1; 	
}

state_service_func StopPeriodicCREGCheck(void *p)
{  	
	if(gsm_creg_timer == -1)
		return (state_service_func)0;
	
	timer_disable(gsm_creg_timer);
 	
 	return (state_service_func)1;
}

/* Timer stuff  for ringing State */

static int gsm_cpas_timer = -1;

timer_service_func send_PAScheck(void *p)
{
	if(fr_task_on)
		return (timer_service_func)0;
	if(fm_started)
		return (timer_service_func)0;
		
	if(pas_check_enabled)
		send_at_command("AT+CPAS\r\n", 0);
	if(clcc_check_enabled)
		send_at_command("AT+CLCC\r\n", 5);
	return (timer_service_func)0;
}


state_service_func StartPeriodicPASCheck(void *p)
{
    if(gsm_cpas_timer == -1)
		return 0;
		
	if(data_mode != COMMAND)  //Do not start periodic CPAS checking in data mode
		return 0;
				
	if(!timer_enable(gsm_cpas_timer, (unsigned int)p))
	{
	 	return 0;
	}
	
	pas_check_enabled = 1;
	clcc_check_enabled = 1;

	return (state_service_func)1; 
}   

state_service_func StopPeriodicPASCheck(void *p)
{  	
	timer_disable(gsm_cpas_timer);
	pas_check_enabled = 0;
	clcc_check_enabled = 0;
 	return 0;
}

state_service_func StartPeriodicCLCCCheck(void *p)
{
    if(gsm_cpas_timer == -1)
		return 0;
		
	if(data_mode != COMMAND)  //Do not start periodic CPAS checking in data mode
		return 0;
				
	if(!timer_enable(gsm_cpas_timer, (unsigned int)p))
	{
	 	return 0;
	}
	
	pas_check_enabled = 0;
	clcc_check_enabled = 1;

	return (state_service_func)1; 
}

/* Timer stuff  for pin enrty */

static int gsm_cpin_timer = -1;

timer_service_func send_PINcheck(void *p)
{
	send_at_command("AT+CPIN?\r\n", 0);
	return (timer_service_func)0;
}


state_service_func StartPeriodicPINCheck(void *p)
{
    if(gsm_cpin_timer == -1)
    {
		return 0;
	}
	
	if(data_mode != COMMAND)  //Do not start periodic CPIN checking in data mode
	{
		return 0;
	}			
	
	if(!timer_enable(gsm_cpin_timer, (unsigned int)p))
	{
	 	return 0;
	}

	return (state_service_func)1; 
}   

state_service_func StopPeriodicPINCheck(void *p)
{  	
	timer_disable(gsm_cpin_timer);
 	return 0;
}

/* Timer stuff for rssi dsiplay */

static int gsm_csq_timer = -1;

timer_service_func send_CSQcheck(void *p)
{
	if(!OnHookStatus())  //Do not send CSQ checking while offhook
		return (timer_service_func)0;

	send_at_command("AT+CSQ\r\n", 0);
	return (timer_service_func)0;
}

state_service_func StartPeriodicCSQCheck(void *p)
{
    if(gsm_csq_timer == -1)
    {
		return 0;
	}
		
	if(!timer_enable(gsm_csq_timer, (unsigned int)p))
	{
	 	return 0;
	}

	return (state_service_func)1; 
}   

state_service_func StopPeriodicCSQCheck(void *p)
{  	
	timer_disable(gsm_csq_timer);
 	return 0;
}

/* Timer stuff for reregistration */

static int gsm_rereg_timer = -1;

timer_service_func send_ReRegistration(void *p)
{
	// Periodicity : 4 Hour (25*57600)
	
	rereg_counter++;
	
	if(rereg_counter < 25 )
	{
		StopPeriodicReRegistration(0);
		StartPeriodicReRegistration((void*)57600);
	}
	else
	{
		if((OnHookStatus()) && CC_current_state == 0 && dce_state != DCE_STATE_PUMP)
		{
			send_at_command("AT+COPS=2\r\n", 0);
			send_at_command("AT+COPS=0\r\n",600);
		}
		rereg_counter = 0;
	}
	return (timer_service_func)0;
}


state_service_func StartPeriodicReRegistration(void *p)
{
    if(gsm_csq_timer == -1)
    {
		return 0;
	}
		
	if(!timer_enable(gsm_rereg_timer, (unsigned int)p))
	{
	 	return 0;
	}

	return (state_service_func)1; 
}   

state_service_func StopPeriodicReRegistration(void *p)
{  	
	timer_disable(gsm_rereg_timer);
 	return 0;
}

void init_at_timers(void)
{
	if(!timer_add(&gsm_creg_timer,
	   0,// Not one_shot!!
	   (timer_service_func)send_CREGcheck, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create creg timer\r\n");
		gsm_creg_timer = -1;
	 	return;
	} 
	
    if(!timer_add(&gsm_cpas_timer, 0,// Not one_shot!!
	   (timer_service_func)send_PAScheck, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create cpas timer\r\n");
		gsm_cpas_timer = -1;
	 	return;
	}
	
	if(!timer_add(&gsm_cpin_timer, 0,// Not one_shot!!
	   (timer_service_func)send_PINcheck, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create cpin timer\r\n");
		gsm_cpin_timer = -1;
	 	return;
	}
	
	if(!timer_add(&gsm_init_timer, 0,// Not one_shot!!
	   (timer_service_func)send_ATV0E0, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create init timer\r\n");
		gsm_recover_timer = -1;
	 	return;
	}  
	
	if(!timer_add(&gsm_recover_timer, 0,// Not one_shot!!
	   (timer_service_func)send_IPR, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create recover timer\r\n");
		gsm_recover_timer = -1;
	 	return;
	}  
	
	if(!timer_add(&gsm_csq_timer, 0,// Not one_shot!!
	   (timer_service_func)send_CSQcheck, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create csq timer\r\n");
		gsm_csq_timer = -1;
	 	return;
	} 
		
	if(!timer_add(&gsm_rereg_timer, 0,// Not one_shot!!
	   (timer_service_func)send_ReRegistration, 
	   (void *)0, 
	   0))
	{
		DebugPrintf("Unable to create reregistration timer\r\n");
		gsm_rereg_timer = -1;
	 	return;
	}    

} 

void cut_dialing_prefix(beg,len)
{
	int i;
	
	//Cut the data call prefix from the dialing number.
	
	DialingParams.ndx-=len;
	for(i=0;i<DialingParams.ndx; i++)
		DialingParams.DialingParams[i+beg]=DialingParams.DialingParams[i+beg+len];
}

int ss_prefix(char *s, int beg, int len)
{
	int i;
	for(i=0; i<len;i++)
		if(DialingParams.DialingParams[i+beg] != s[i])
			return 0;
	cut_dialing_prefix(beg,len);
	return 1;
}

void ss_processing_setup(int delay)
{
	/* stop all periodic queries */
	erase_delayed_messages();
	
	// StopPeriodicPASCheck((void *)0);
	StopPeriodicCREGCheck((void *)0);
	
	clcc_check_enabled =0;	
	
	/* set the flag at the time of message send*/
	send0_delayed(delay, AT, AT_SS_flag);
}

int ProcessDialingParameters(void)
{
   	int i,fax;
	char str[MAX_DIGIT_COUNT]="";
	char *flashdata;
	char *dn;
	
	fax =0;
    
	//numbers to symbols.
	for(i = 0; i < DialingParams.ndx; i++)  
    	DialingParams.DialingParams[i] = (int)dialing_symbol(DialingParams.DialingParams[i]);
    DialingParams.DialingParams[DialingParams.ndx] = '\0';
    
    dn = strrchr((char *)DialingParams.DialingParams, '#'); 
    dn++; // points to the start of the phone number for call setup SS.
        
    if(ss_prefix("#**",0,3))	// PIN entry
    {
    	
    	for(i = 0; i < MAX_PIN_LENGTH; i++)
    	{
    		if( i < DialingParams.ndx)
    		{
    			if(DialingParams.DialingParams[i] == '*')
    				DialingParams.DialingParams[i] = ',';
    			pin[i]= (char)DialingParams.DialingParams[i];
    		}
    		else
    		pin[i]='\0';
    	}
    	
    	pin_entry_sent = 1;
    	pin_entry_countdown = 4;	/* 4 CPIN check periods , approx 4 seconds */
    
       	memcpy(str,"AT+CPIN=",8);
       	for(i = 0 ; i < strlen(pin); i++)
       		str[i+8]=pin[i];
       	str[i+8]='\r';
       	str[i+9]='\0';
        send_at_command(str,10);
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;
    }
    else
    if(ss_prefix("#*11*",0,5))	// PIN Enable/Disable
    {
    	for(i = 0; i <  DialingParams.ndx; i++)
    	{
    		if(DialingParams.DialingParams[i] == '*')
    			DialingParams.DialingParams[i] = ',';
       	}
       	DialingParams.DialingParams[i]='\r';
       	DialingParams.DialingParams[i+1]='\0';
       	
       	memcpy(str,"AT+CLCK=\"SC\",",13);
       	strcat(str, (char*)DialingParams.DialingParams);	
        
       	DebugPrintf("PIN Enable: ");
       	DebugPrintf(str);
       	DebugPrintf("\r\n");
        	
       	ss_processing_setup(20);
      	send_at_command(str,20);
       	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    	
    }
    else
    if(ss_prefix("#*12*",0,5))	// AutoPIN Enable/Disable
    {
    	switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': //disable AutoPIN 
   	    	autopin_enabled = 0;	// Need to be stored on FLASH
   	    	if(UpdateFlashData("AUTOPIN","0"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send0(CC, AT_SS_confirm);
   	    		DebugPrintf("AutoPIN Disabled\r\n");
   	    	}
   	    	break;
   	    case '1': //enable AutoPIN
   	    	autopin_enabled = 1;	//Need to be stored on FLASH
   	    	if(UpdateFlashData("AUTOPIN","1"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{
   	    		send0(CC, AT_SS_confirm);
   	    		DebugPrintf("AutoPIN Enabled\r\n");
   	    	}
   	    	break;
   	    case '2': // Status check
   	    	flashdata=ReadFlashData("AUTOPIN");
    	 	if(flashdata != NULL)
    		{
	    		switch(flashdata[0])
		    	{
		    	case '0':
		    		send0(CC, AT_SS_neg_status);
		    		break;
				case '1':
				   	send0(CC, AT_SS_confirm);
				   	break;
				default:
				   	send0(CC, AT_SS_failure);
				}
			}
			else
				send0(CC, AT_SS_failure);
		   	break;
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
        
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0; 	
    }
    else
    if(ss_prefix("#*13*",0,5))	// CLIR Enable/Disable
    {
    		for(i = 0; i <  DialingParams.ndx; i++)
    	{
    		if(DialingParams.DialingParams[i] == '*')
    			DialingParams.DialingParams[i] = ',';
       	}
       	DialingParams.DialingParams[i]='\r';
       	DialingParams.DialingParams[i+1]='\0';
       	
       	memcpy(str,"AT+CLIR=",8);
       	strcat(str, (char*)DialingParams.DialingParams);
        
        DebugPrintf(str);
      	DebugPrintf("\r\n");
       	
       	ss_processing_setup(20);
      	send_at_command(str,20);
      	
      	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    			
    }
    else
    if(ss_prefix("#*14*",0,5))	// CCM Enable/Disable
    {
    	switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': //disable CCM 
   	    	ccm_enabled = 0;	
   	    	if(UpdateFlashData("CCM","0"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send0(CC, AT_SS_confirm);
   	    		DebugPrintf("CCM Disabled\r\n");
   	    	}
   	    	break;
   	    case '1': //enable CCM
    		flashdata = ReadFlashData("CIDMODTYPE");
    		if('2' != *flashdata)
    		{
	   	    	ccm_enabled = 1;
	   	    	at_call_complete = 0;	
	   	    	if(UpdateFlashData("CCM","1"))
	   	    	{
	   	    		send0(CC, AT_SS_failure);
	   	    	}
	   	    	else
	   	    	{
	   	    		send0(CC, AT_SS_confirm);
	   	    		DebugPrintf("CCM Enabled\r\n");
	   	    	}
   	    	} // end if !=2
   	    	else
   	    		send0(CC, AT_SS_failure);
   	    	break;
   	    case '2': // Status check
   	    	flashdata=ReadFlashData("CCM");
    	 	if(flashdata != NULL)
    		{
	    		switch(flashdata[0])
		    	{
		    	case '0':
		    		send0(CC, AT_SS_neg_status);
		    		break;
				case '1':
				   	send0(CC, AT_SS_confirm);
				   	break;
				default:
				   	send0(CC, AT_SS_failure);
				}
			}
			else
				send0(CC, AT_SS_failure);
		   	break;
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
        	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0; 	
    }
    else
    if(ss_prefix("#*15",0,4))	// Send LASTCALL duration to CID box
    {
    	flashdata = ReadFlashData("CIDMODTYPE");
    	if('2' != *flashdata)
    	{
	    	strcpy(INFO_name, ReadFlashData("LASTCALL"));
	   		strcpy(INFO_number, ReadFlashData("LASTNUMBER"));
	    	waiting_CID = numberOfCIDSessions(INFO_name);
    		bAwake = 1;
	    	 	
	    	send0(CC, AT_SS_confirm);
    	}
    	else
    		send0(CC, AT_SS_failure);
    	   
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*16",0,4))	// Send ALLCALLs duration to CID box
    {
    	flashdata = ReadFlashData("CIDMODTYPE");
    	if('2' != *flashdata)
    	{
	        strcpy(INFO_name, ShowAllCalls());
	      	waiting_CID = numberOfCIDSessions(INFO_name);
    		bAwake = 1;
	    	
	    	send0(CC, AT_SS_confirm);
    	}
    	else
    		send0(CC, AT_SS_failure);
    	   
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*17",0,4))	// Reset ALLCALL's timer
    {
        if(UpdateFlashData("ALLCALLS","0"))
    	{
    		send0(CC, AT_SS_failure);
    	}
    	else
    	{	
    		send0(CC, AT_SS_confirm);
    		DebugPrintf("ALLCALLS timer cleared.\r\n");
    	}
            	   
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*20*",0,5))
    {
    	clcc_check_enabled = 0;
    	
    	flashdata = ReadFlashData("CIDMODTYPE");
    	if(DialingParams.DialingParams[0] == '1' && *flashdata != '2') // start periodic rssi display
    	{
   			StartPeriodicCSQCheck((void*)CSQ_PERIOD);
   			send0(CC, AT_SS_confirm);
   			rssi_display=1;
       	}
    	else 
    	if(DialingParams.DialingParams[0] == '0') // stop periodic rssi display
    	{
    		StopPeriodicCSQCheck((void*)0);
    		send0(CC, AT_SS_confirm);
    		rssi_display=0;
    	}
    	else
    		send0(CC, AT_SS_failure);
    	
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*21",0,4))	// Send SWversion string to CID box
    {
    	flashdata = ReadFlashData("CIDMODTYPE");
    	if('2' != *flashdata)
    	{
	        strcpy(INFO_name, SWversion);
	        waiting_CID = numberOfCIDSessions(INFO_name);
    		bAwake = 1;
	    	
	    	send0(CC, AT_SS_confirm);
    	}
    	else
    		send0(CC, AT_SS_failure);
    	
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*22",0,4))	// Reset DTE interface settings to Factory Default
    {
        InitFlashParams();
    	
    	send0(CC, AT_SS_confirm);
    	   
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
     if(ss_prefix("#*23*",0,5))	// Reset DTE interface settings to Factory Default
    {
       	DialingParams.DialingParams[DialingParams.ndx]='\0';
       	
       	if( (strcmp((char*)DialingParams.DialingParams,"0") == 0 ) 	    ||
       	    (strcmp((char*)DialingParams.DialingParams,"300") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"600") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"1200") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"2400") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"4800") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"9600") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"19200") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"38400") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"57600") == 0 ) 	||
       		(strcmp((char*)DialingParams.DialingParams,"115200") == 0 ) )
       	{     		
       	
	       	DialingParams.DialingParams[DialingParams.ndx]='\r';
	    	DialingParams.DialingParams[DialingParams.ndx+1]='\0';
	    
	    	memcpy(str,"AT+IPR=",7);
	       	strcat(str, (char*)DialingParams.DialingParams);
	    	
		/*	 if(dce_state == DCE_STATE_PUMP)			// If we choose to process digitis in pump mode later!
		   		uart_a_fputs(str);
		*/
	    	UpdateFlashData("BAUDRATE",str);
	
		/*	if(dce_state == DCE_STATE_PUMP)
			{
		    	DialingParams.DialingParams[DialingParams.ndx]='\0';
	       	        
			    setIPR((char*)DialingParams.DialingParams);
	       
		    	uart_a_fputs("AT&W\r");
		    }
		*/         	
	       	send0(CC, AT_SS_confirm);
	     }
	     else
	     	send0(CC, AT_SS_failure);
    				   	   
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*24*",0,5)) // Caller ID box type
    {
    	switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': // ETSI V.23 FSK
   	    	if(UpdateFlashData("CIDMODTYPE", "0"))
   	    		send0(CC, AT_SS_failure);
   	    	else
   	    		send0(CC, AT_SS_confirm);
   	    	break;
   	    case '1': // Bellcore 202 FSK
   	    	if(UpdateFlashData("CIDMODTYPE", "1"))
   	    		send0(CC, AT_SS_failure);
   	    	else
   	    		send0(CC, AT_SS_confirm);
   	    	break;
   	    case '2': // DTMF
   	    	if(UpdateFlashData("CIDMODTYPE", "2"))
   	    		send0(CC, AT_SS_failure);
   	    	else
   	    		send0(CC, AT_SS_confirm);
   	    	break;
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
   	       	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;
    	return 0;
    }
    else
    if(ss_prefix("#*25*",0,5)) // Set system time and date
    {
    	flashdata = ReadFlashData("CIDMODTYPE");
    	if('2' != *flashdata)
    	{
	     	memcpy(str,"AT+CCLK=\"",9);
       		
       		str[9]=DialingParams.DialingParams[0];
       		str[10]=DialingParams.DialingParams[1];
       		str[11]='/';
       		str[12]=DialingParams.DialingParams[2];
       		str[13]=DialingParams.DialingParams[3];
       		str[14]='/';
       		str[15]=DialingParams.DialingParams[4];
       		str[16]=DialingParams.DialingParams[5];
       		str[17]=',';
       		str[18]=DialingParams.DialingParams[6];
       		str[19]=DialingParams.DialingParams[7];
       		str[20]=':';
       		str[21]=DialingParams.DialingParams[8];
       		str[22]=DialingParams.DialingParams[9];
        		
       		strcat(str,"\"\r\n");
        
        	DebugPrintf(str);
       		DebugPrintf("\r\n");
        	
       		ss_processing_setup(20);
      		send_at_command(str,20);
      		
      		timeSetUserRequest = 1;
      		timeSet = 1;  
      }
      else
      	send0(CC, AT_SS_failure);
      	
      memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
      DialingParams.ndx = 0;
   	  return 0;
    }
    else
    if(ss_prefix("#*26",0,4))	// Ringback Test
    {
   		bRingBackTest = TRUE;
   		bAwake = 1;
   		send0(CC, AT_SS_confirm);
   		
   		memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;
    }
    else
    if(ss_prefix("#*27*",0,5))	// Charging Enable/Disable
    {
    	switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': //disable CCM reporting
   	    	if(UpdateFlashData("CHARGING","0"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send_at_command("AT+CAOC=1\r\n",0);
   	    		chargingEnabled = 0;
   	    		send0(CC, AT_SS_confirm);
   	    		DebugPrintf("CCM reporting disabled\r\n");
   	    	}
   	    	break;
   	    case '1': //enable CCM reporting
    	    if(LoadLineSupervParams())
    	    {
	    	    if(UpdateFlashData("CHARGING","1"))
	   	    	{
	   	    		send0(CC, AT_SS_failure);
	   	    	}
	   	    	else
	   	    	{
	   	    		send_at_command("AT+CAOC=2\r\n",0);
		   	    	chargingEnabled = 1;
		   	    	send0(CC, AT_SS_confirm);
		   	    	DebugPrintf("CCM reporting Enabled\r\n");
	   	    	}
   	    	}
   	    	else
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	break;
   	    case '2': // Status check
   	    	flashdata=ReadFlashData("CHARGING");
    	 	if(flashdata != NULL)
    		{
	    		switch(flashdata[0])
		    	{
		    	case '0':
		    		send0(CC, AT_SS_neg_status);
		    		break;
				case '1':
				   	send0(CC, AT_SS_confirm);
				   	break;
				default:
				   	send0(CC, AT_SS_failure);
				}
			}
			else
				send0(CC, AT_SS_failure);
		   	break;
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
        
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0; 	
    }
    else
    if(ss_prefix("#*28*",0,5))		// Home Unit Scaling Factor
    {
   		int homeUnitSFTemp;
   		
   		DialingParams.DialingParams[DialingParams.ndx]='\0';
   		
   		homeUnitSFTemp = atoi((char *)DialingParams.DialingParams); 
   		if((homeUnitSFTemp > 0 && homeUnitSFTemp < 256))
   		{        	        	
        	sprintf(str,"%d",homeUnitSFTemp);
        	if(UpdateFlashData("CHARGPARMS",str))
        	{
        		send0(CC, AT_SS_failure);
        	}
        	else
        	{
        		homeUnitSF = homeUnitSFTemp;
        		send0(CC, AT_SS_confirm);		
        	}
        }
        else
        {
        	send0(CC, AT_SS_failure);
        }
              	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    	
 	
    }
    else
    if(ss_prefix("#*30*",0,5))	// SMSdisp Enable/Disable
    {
    	switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': //disable SMS on CallerID 
   	    	SMSonCID_enabled = 0;	// Need to be stored on FLASH
   	    	if(UpdateFlashData("SMSONCID","0"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send0(CC, AT_SS_confirm);
   	    		DebugPrintf("SMS on CallerID Disabled\r\n");
   	    	}
   	    	break;
   	    case '1': //enable SMS on CallerID
    		flashdata = ReadFlashData("CIDMODTYPE");
    		if('2' != *flashdata)
    		{
	   	    	SMSonCID_enabled = 1;	//Need to be stored on FLASH
	   	    	if(UpdateFlashData("SMSONCID","1"))
	   	    	{
	   	    		send0(CC, AT_SS_failure);
	   	    	}
	   	    	else
	   	    	{
	   	    		send0(CC, AT_SS_confirm);
	   	    		DebugPrintf("SMS on CallerID Enabled\r\n");
	   	    	}
   	    	}
   	    	else
   	    		send0(CC, AT_SS_failure);
   	    	break;
   	    case '2': // Status check
   	    	flashdata=ReadFlashData("SMSONCID");
    	 	if(flashdata != NULL)
    		{
	    		switch(flashdata[0])
		    	{
		    	case '0':
		    		send0(CC, AT_SS_neg_status);
		    		break;
				case '1':
				   	send0(CC, AT_SS_confirm);
				   	break;
				default:
				   	send0(CC, AT_SS_failure);
				}
			}
			else
				send0(CC, AT_SS_failure);
		   	break;
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
        
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0; 	
    }
    else
    if(ss_prefix("#*31*",0,5)) // Read SMS
    {
   		flashdata = ReadFlashData("CIDMODTYPE");
   		if('2' != *flashdata)
   		{
	    	for(i = 0; i <  DialingParams.ndx; i++)
	    	{
	    		if(DialingParams.DialingParams[i] == '*')
	    			DialingParams.DialingParams[i] = ',';
	       	}
	       	DialingParams.DialingParams[i]='\r';
	       	DialingParams.DialingParams[i+1]='\0';
	       	
	       	memcpy(str,"AT+CMGR=",8);
	       	strcat(str, (char*)DialingParams.DialingParams);
	        
	        strcpy(SMSindex, (char*)DialingParams.DialingParams);
	        for(i = 0; i<4; i++)
	        {
	        	if(SMSindex[i] == '\r')
	        		SMSindex[i] = '\0';
	        }
	        
	       	DebugPrintf(str);
	       	DebugPrintf("\r\n");
	        	
	       	ss_processing_setup(20);
	      	send_at_command(str,20);
      	}
      	else
      		send0(CC, AT_SS_failure);
      	
      	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    		
    }
    else
    if(ss_prefix("#*32*",0,5)) //Delete SMS
    {
    	for(i = 0; i <  DialingParams.ndx; i++)
    	{
    		if(DialingParams.DialingParams[i] == '*')
    			DialingParams.DialingParams[i] = ',';
       	}
       	DialingParams.DialingParams[i]='\r';
       	DialingParams.DialingParams[i+1]='\0';
       	
       	memcpy(str,"AT+CMGD=",8);
       	strcat(str, (char*)DialingParams.DialingParams);	
        
       	DebugPrintf(str);
       	DebugPrintf("\r\n");
        	
       	ss_processing_setup(20);
      	send_at_command(str,20);
      	
      	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    		
    }
    else
    if(ss_prefix("#*33*",0,5)) //List SMS
    {
   		flashdata = ReadFlashData("CIDMODTYPE");
   		if('2' != *flashdata)
   		{
	    	for(i = 0; i <  DialingParams.ndx; i++)
	    	{
	    		if(DialingParams.DialingParams[i] == '*')
	    			DialingParams.DialingParams[i] = ',';
	       	}
	       	DialingParams.DialingParams[i]='\r';
	       	DialingParams.DialingParams[i+1]='\0';
	       	
	       	send_at_command("AT+CPMS?\r\n",0);
	       	memcpy(str,"AT+CMGL=",8);
	       
	      	if(DialingParams.DialingParams[0] == '1')
	   	    {
	   	    	strcat(str, "\"ALL\"\r\n");
	   	    	
	   	    	DebugPrintf(str);
	       		DebugPrintf("\r\n");
	        	
	       		ss_processing_setup(20);
	       		send_at_command(str,20);
	      	
	        }
	        else
	        	send0(CC, AT_SS_failure);
      	}
      	else
      		send0(CC, AT_SS_failure);
      		
      	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;  
    	return 0;    		
    }
    else
    if(ss_prefix("#*71*",0,5))
    {
   		i=(int)(DialingParams.DialingParams[5])-(int)'0';

   		if(i>=1 && i <= 5 && SaveVolume(i))
   		{
   			send0(CC, AT_SS_confirm);
   			DebugPrintf1("Speaker volume is set to",i);
   		}
   		else
   		{
			send0(CC, AT_SS_failure);   		
   		}

    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*72*",0,5))
    {
   		i=(int)(DialingParams.DialingParams[5])-(int)'0';

   		if(i>=1 && i <= 3 && SaveVolume(i+5))
   		{
   			send0(CC, AT_SS_confirm);
   			DebugPrintf1("Microphone volume is set to",i);
   		}
   		else
   		{
			send0(CC, AT_SS_failure);   		
   		}

    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
	if(ss_prefix("#*81*",0,5))	//Fax
	{
		fax = 1;
		//outgoing fax call type notification
		send0(FM,FM_outgoing_fax);
		clcc_check_enabled = 0;
		DebugPrintf("Outgoing fax call\r\n");
	}
	else
    if(ss_prefix("#*82*",0,5))	//Data
	{
		//outgoing data call type notification
		send0(FM,FM_outgoing_data);
		clcc_check_enabled = 0;
		DebugPrintf("Outgoing data call\r\n");
		send1(AT,AT_response,0,(int*)at_resp_connect);
		send1(AT,AT_response,0,(int*)derive_modem_type()); //v.32 9600
	}
    else
    if(ss_prefix("#*90",0,4))
    {
    	ss_processing_setup(10);
    	
    	send_at_command("AT+CSNS=0\r\n",10); // Voice
   		leds_update(3,green,0); 
   	       	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*91",0,4))
    {
    	ss_processing_setup(10);
    	
    	send_at_command("AT+CSNS=2\r\n",10); //fax
   		leds_update(3,orange,50);
   		       	
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*92",0,4))
    {
    	ss_processing_setup(10);
    	
    	send_at_command("AT+CSNS=4\r\n",10); //modem
   		leds_update(3,orange,0);
   		       	
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*93*",0,5))
    {
        for(i = 0; i <  DialingParams.ndx; i++)
    	{
    		if(DialingParams.DialingParams[i] == '*')
    			DialingParams.DialingParams[i] = ',';
       	}
       	DialingParams.DialingParams[i]='\r';
       	DialingParams.DialingParams[i+1]='\0';
       	
       	if((DialingParams.DialingParams[0] == '0'|| //auto
       	    DialingParams.DialingParams[0] == '2'|| //v.22 1200
       	    DialingParams.DialingParams[0] == '4'|| //v.22bis 2400
       	    DialingParams.DialingParams[0] == '6'|| //v.32 4800
       	    DialingParams.DialingParams[0] == '7')&& //v.32 9600
       	   DialingParams.DialingParams[1] == ',' &&
       	   DialingParams.DialingParams[2] == '0' &&
       	   DialingParams.DialingParams[3] == ',' &&
       	   DialingParams.DialingParams[4] >= '0' &&
       	   DialingParams.DialingParams[4] <= '3')
       	{
       	    memcpy(str,"AT+CBST=0,0,1\r\n",15);
    		str[8] = DialingParams.DialingParams[0];
    		str[10] = DialingParams.DialingParams[2];
    		str[12] = DialingParams.DialingParams[4];
   	    	if(UpdateFlashData("DATABEARER",str))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{
   	    		send0(CC, AT_SS_confirm);
   	    		send_at_command(str,0);
   	    		DebugPrintf("Data Bearer is set.\r\n");
   	    	}
    	}
    	else
    		send0(CC, AT_SS_failure);
   		       	
    	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else
    if(ss_prefix("#*899*",0,6)) // Forced Power Saving Enable
    {
   	    switch(DialingParams.DialingParams[0])
   	    {
   	    case '0': 
   	    	if(UpdateFlashData("POWERSAVE","0"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send0(CC, AT_SS_confirm);
   	    	}
   	    	break;
   	    case '1': 
   	    	if(UpdateFlashData("POWERSAVE","1"))
   	    	{
   	    		send0(CC, AT_SS_failure);
   	    	}
   	    	else
   	    	{	
   	    		send0(CC, AT_SS_confirm);
   	    	}
   	    	break;
   	    case '2':
   	    	flashdata=ReadFlashData("POWERSAVE");
    	 	if(flashdata != NULL)
    		{
	    		switch(flashdata[0])
		    	{
		    	case '0':
		    		send0(CC, AT_SS_neg_status);
		    		break;
				case '1':
				   	send0(CC, AT_SS_confirm);
				   	break;
				default:
				   	send0(CC, AT_SS_failure);
				}
			}
			else
				send0(CC, AT_SS_failure);
		   	break;   	   
   	    default:
   	    	send0(CC, AT_SS_failure);
   	    }
   	       	
       	memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
       	DialingParams.ndx = 0;      	
    	return 0;
    }
    else	// GSM SS control sequence
	if((DialingParams.DialingParams[0] == '*' ||
		DialingParams.DialingParams[0] == '#' ) &&
		DialingParams.DialingParams[DialingParams.ndx - 1] == '#')
	{
		DebugPrintf((char*)DialingParams.DialingParams);
		DebugPrintf("\r\n");
		
		ss_processing_setup(20);
	}
	else	// GSM SS call setup sequence
	if((DialingParams.DialingParams[0] == '*' ||
		DialingParams.DialingParams[0] == '#' ) &&
		(DialingParams.DialingParams[1] >= '0' && DialingParams.DialingParams[1] <= '9') &&
	    (*(dn) >= '0' && *(dn) <= '9'))
	{
	
			DebugPrintf((char*)DialingParams.DialingParams);
			DebugPrintf("\r\n");
		
			ss_processing_setup(20);
	}
    else //voice call.
    {
    	//modify the dial string to be voice call.
    	DialingParams.DialingParams[DialingParams.ndx]=';';
    	DialingParams.ndx++;	
    }

	if(DialingParams.ndx && !fax)
	{
		memcpy(str,"ATD",3);
    	for(i=0;i<DialingParams.ndx;i++)
    		str[i+3]=DialingParams.DialingParams[i];
    	str[i+3]='\r';
    	str[i+4]='\0';
    	send_at_command(str,20); 
	}
	return 0;
}

void wavecom_on(void)
{
	WAVECOM=1;     			// Set Wavecom ON 
	delay(1000);  			// 0.1 second delay
	WAVECOM=3; 
	WAVECOM_init=1;
	delay(1000);            // 0.1 second delay
    send_at_command("ATV0;E0\r\n",0);
    gsm_baud_idx = 0;
    gsm_icf_idx = 0;
    start_gsm_init_timer((void *)0);
} 

void wavecom_restart(void)
{ 
	UART_A_MCR_REG |= RTS;
	WAVECOM_init=1;
	send_at_command("ATV0;E0\r\n",0);
	gsm_baud_idx = 0;
	gsm_icf_idx = 0;
	start_gsm_init_timer((void *)0);
}

void dropcall(void)
{
	int i;
	UART_A_MCR_REG |= DTR;
	for(i=0;i<3000;i++); //~30 microsecond delay
	UART_A_MCR_REG &= ~DTR;
}

void set_bearer(unsigned short delay)
{
	char *flashdata;
	
	if((flashdata = ReadFlashData("DATABEARER")) != NULL)
	{
		send_at_command(flashdata,delay);
	}
	else
	{
		send_at_command("AT+CBST=0,0,2\r\n",delay);
	}
}


int isWismo2C2(char *rev)
{
	int result;
	 
	if((strstr(rev,"2C2") != NULL) || (strstr(rev,"Q23") != NULL))			
	{
		DebugPrintf("2C2 Wavecom Module\r\n");
		result = 1; 
	}	
	else
	{
		DebugPrintf("2D Wavecom Module\r\n");
		result = 0;
	}

	return result;
	
}

