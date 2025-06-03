#include "su.h" 

#ifdef _MODEMS_
#include "modems.h"
#endif

#define NUMBER_OF_CRC_TYPES 19
#define NUMBER_OF_PIN_TYPES 15 

/* Externs */

extern int GSM_ready; 
extern int clcc_check_enabled; 
extern int rssi_display;
extern int CC_current_state;
extern int bAwake;
extern int SS_setup;
extern int pin_entry_sent;
extern int pin_entry_countdown;
extern int autopin_enabled;
extern int SMSonCID_enabled;
extern int Start_Call_timer;
extern int timeSet;
extern int timeSetUserRequest;
extern int chargingEnabled;
extern int homeUnitSF;

extern char SMS_message[];
extern char SMSindex[];

/* Globals */

char pin[MAX_PIN_LENGTH]="";
char SMSnumber[21]="";
char SMSdatetime[9];
char LASTCALL_duration[16]="";
char LASTCALL_number[21]="";
char INFO_name[SMS_LENGTH]="";
char INFO_number[21]="";

int SIM_present = 0;
int PIN_not_required = 0;
int PUK_not_required = 0;
int REG_status;
int wrong_passwd_count = 0;
int	number_of_SMS = 0;
int waiting_CID = 0;
int LASTCALL_direction = 2;
int Multi_Call_timer = 0;
int ring_count = 0;
int	num_current_calls;
int	num_held_calls;
int	num_mpty_calls;
int	skip_clcc_reading = 0;   

CURRENT_CALL	current_calls[MAX_CURRENT_CALLS];

/* Protottypes */

void handleSMS(void);
void clearSMSParams(void);
void CallDuration(void);

int held_call_exists(void);
int numberOfCIDSessions(char *msg);


char *CRC_type[]=
{
	"ASYNC",  				// asynchronous transparent
	"SYNC",					// synchronous transparent
	"REL ASYNC",			// asynchronous non-transparent
	"REL SYNC",				// synchronous non-transparent
	"FAX",					// facsimile (TS 62)
	"VOICE",				// normal voice (TS 11)
	"VOICE/ASYNC",			// voice followed by data (BS 81)
	"VOICE/SYNC",			// voice followed by data (BS 81)
	"VOICE/REL ASYNC",		// voice followed by data (BS 81)
	"VOICE/REL SYNC",		// voice followed by data (BS 81)
	"ALT VOICE/ASYNC",		// alternating voice/data, voice first (BS 61)
	"ALT VOICE/SYNC",		// alternating voice/data, voice first (BS 61)
	"ALT VOICE/REL ASYNC",	// alternating voice/data, voice first (BS 61)
	"ALT VOICE/REL SYNC",	// alternating voice/data, voice first (BS 61)
	"ALT ASYNC/VOICE",		// alternating voice/data, data first (BS 61)
	"ALT SYNC/VOICE",		// alternating voice/data, data first (BS 61)
	"ALT REL ASYNC/VOICE",	// alternating voice/data, data first (BS 61)
	"ALT REL SYNC/VOICE",	// alternating voice/data, data first (BS 61)
	"ALT VOICE/FAX",		// alternating voice/fax, voice first (TS 61)
	"ALT FAX/VOICE"		// alternating voice/fax, fax first (TS 61)
}; 

char *PIN_type[]=
{
	"READY",			// ME is not pending for any password
	"SIM PIN",			// ME is waiting SIM PIN to be given
	"SIM PUK",			// ME is waiting SIM PUK to be given
	"PH-SIM PIN",		// ME is waiting phone-to-SIM card password to be given 
	"PH-FSIM PIN",		// ME is waiting phone-to-very first SIM card password to be given
	"PH-FSIM PUK",		// ME is waiting phone-to-very first SIM card unblocking password to be given
	"SIM PIN2",			// ME is waiting SIM PIN2 to be given 
	"SIM PUK2",			// ME is waiting SIM PUK2 to be given 
	"PH-NET PIN",		// ME is waiting network personalisation password to be given
	"PH-NET PUK",		// ME is waiting network personalisation unblocking password to be given
	"PH-NETSUB PIN",	// ME is waiting network subset personalisation password to be given
	"PH-NETSUB PUK",	// ME is waiting network subset personalisation unblocking password to be given
	"PH-SP PIN",		// ME is waiting service provider personalisation password to be given
	"PH-SP PUK",		// ME is waiting service provider personalisation unblocking password to be given
	"PH-CORP PIN",		// ME is waiting corporate personalisation password to be given
	"PH-CORP PUK"		// ME is waiting corporate personalisation unblocking password to be given
};

typedef enum creg_n_tag
{
	disable_result = '0',
	enable_result,
	enable_result_location
} creg_n;

typedef enum creg_stat_tag
{
	not_registered = '0',
	home,
	searching,
	denied,
	creg_unknown,
	roaming
} creg_stat;

void process_creg_result(char *str, creg_stat stat)
{		
		strcat(str,"stat=");

		switch(stat)
		{
		case not_registered:
			strcat(str,"not registered");
			leds_update(2,red,0);
			if(SIM_present && PIN_not_required && PUK_not_required)
			{
				send0(SLAC,SLAC_no_service);
			}
			else
			{
				send0(SLAC, SLAC_no_sim);
			}
			REG_status = 0;	
		    break;
		case home:
			strcat(str,"home");
			if(!GSM_ready)
				init_at_ss();
			leds_update(2,green,0);
			SIM_present=1;
			PIN_not_required = 1;
			PUK_not_required = 1;
			send0(SLAC,SLAC_service_available);
			REG_status = 1;
		    break;
		case searching:
			strcat(str,"searching");
			leds_update(2,orange,0);
			send0(SLAC,SLAC_no_service);
			REG_status = 2;
			break;
		case denied:
			strcat(str,"denied");
			leds_update(2,red,0);
			send0(SLAC,SLAC_no_service);
			REG_status = 3;
			break;
		case creg_unknown:
			strcat(str,"unknown");
			leds_update(2,red,0);
			send0(SLAC,SLAC_no_service);
			REG_status = 4;
			break;
		case roaming:
			strcat(str,"roaming");
			if(!GSM_ready)
				init_at_ss();
			leds_update(2,green,0);
			SIM_present=1;
			PIN_not_required = 1;
			PUK_not_required = 1;
			send0(SLAC,SLAC_service_available);
			REG_status = 5;
		}	 
}
int CREG(char *n,char *stat,char *lac,char *ci)
{
	char str[80]="creg: ";
	switch((creg_n)(n[0]))
	{
	case disable_result:
		strcat(str,"disabled, ");
		process_creg_result(str,(creg_stat)(stat[0]));
		strcat(str,"\r\n");
		DebugPrintf(str);
		return 1;
	case enable_result_location:
	    strcat(str,"enabled, ");
	    strcat(str,"lac=");
	    strcat(str,lac);
	    strcat(str,", ci=");
	    strcat(str,ci);
	    strcat(str,", ");
	    process_creg_result(str,(creg_stat)(stat[0]));
	    break;
	case enable_result:
		strcat(str,"enabled, ");
		process_creg_result(str,(creg_stat)(stat[0]));
	}
	strcat(str,"\r\n");
	DebugPrintf(str);
	return 1;
}			


int CSQ(char *rssi,char *ber)
{
	int rssi_val;
	int ber_val;
	char str[15]="";
		
	rssi_val= atoi(rssi);
	ber_val= atoi(ber);
	
	/* dBm conversion according to Wavecom implementation */
	
	if( rssi_val < 31 )
	{
		if(dce_state != DCE_STATE_PUMP)
		{
			if(rssi_val < 6 )
				leds_blink(2,100);
			else
				leds_blink(2,0);
		}
		sprintf(str, "rssi:%ddBm,%d", (-113 + rssi_val *2),ber_val);
	}

	if(rssi_val == 31)
	{
		leds_blink(2,0);
		sprintf(str, "rssi:>-51dBm,%d",ber_val); //strong signal
	}
	
	if(rssi_val == 99)
	{
		sprintf(str, "rssi:not known");
		if(REG_status != 1 && REG_status != 5)
			leds_blink(2,100);
	}

	DebugPrintf(str);
	DebugPrintf("\r\n");
		
	if(rssi_display)
		CID_OnhookPrintf(str,"");
	
	return 1;
}

int CCFC(char *stat)
{
	if(SS_setup == 1)
	{
		switch(stat[0])
		{
			case '0':	/* disabled */
				send0(CC, AT_SS_neg_status);
				break;
			case '1':	/* enabled */
				send0(CC, AT_SS_confirm);
				break;
			default:
				break;			
		}
		SS_setup = 0;
	}
	return 1;
}

enum pas_val_tag
{
	ready	=	'0',
	unavailable,
	pas_unknown,
	ringing,
	call_in_progress,
	asleep
} pas_val;

int CPAS(char *pas)
{
	int	i;
	
	switch(pas[0])
	{
	case ready:
		DebugPrintf("cpas: ready\r\n");
		for(i = 0; i < MAX_CURRENT_CALLS; i++)
			current_calls[i].present = 0;
		num_mpty_calls = 0;
		num_current_calls = 0;
		ring_count = 0;
		if(SS_setup)
		{
			send0(CC, AT_SS_confirm);
			SS_setup = 0;
		}
		else
			send0(CC,AT_disconnect);
		break;
	case unavailable:
		break;
	case pas_unknown: 
	    break;
	case ringing:
		DebugPrintf("cpas: ringing\r\n");
		break;
	case call_in_progress:
		DebugPrintf("cpas: call inprogress\r\n");
		send0(CC,AT_connect);
		ring_count = 0;
	    break;
	case asleep:
		break;
	}
	return 1;
}


enum cme_err_tag
{
	phone_failure			=	0,
	no_connection_to_phone,
	phone_adaptor_link_reserved,
	operation_not_allowed,
	operation_not_supported,
	PH_SIM_PIN_required,
	PH_FSIM_PIN_required,
	PH_FSIM_PUK_required,
	SIM_not_inserted		=	10,
	SIM_PIN_reqired,
	SIM_PUK_required,
	SIM_failure,
	SIM_busy,
	SIM_wrong,
	incorrect_password,
	SIM_PIN2_required,
	SIM_PUK2_required,
	memory_full				=	20,
	invalid_index,
	not_found,
	memory_failure,
	text_string_too_long,
	invalid_characters_in_text_string,
	dial_string_too_long,
	invalid_characters_in_dial_string,
	no_network_service		=	30,
	network_timeout,
	network_not_allowed_emergency_calls_only,
	network_personalisation_PIN_required	=	40,
	network_personalisation_PUK_required,
	network_subset_personalisation_PIN_required,
	network_subset_personalisation_PUK_required,
	service_provider_personalisation_PIN_required,
	service_provider_personalisation_PUK_required,
	corporate_personalisation_PIN_required,
	corporate_personalisation_PUK_required,
	command_processing_in_progress = 515,
	retry_operator_selection = 527
};
typedef enum  cme_err_tag  cme_err;

int CME_ERROR(char *err)
{
	cme_err err_val;
	err_val = (cme_err)atoi(err);
	switch(err_val)
	{
	case phone_failure:
	case no_connection_to_phone:
	case phone_adaptor_link_reserved:
	case operation_not_allowed:
	case operation_not_supported:
		if(pin_entry_sent)
		{
			pin_entry_sent = 0;
		}
		if(SS_setup)
		{
			SS_setup = 0;
		}
		if(timeSetUserRequest)
		{
			timeSet = 0;
			timeSetUserRequest = 0;
		}
		send0(CC,AT_SS_failure);
		break;
	case PH_SIM_PIN_required:
		break;
	case PH_FSIM_PIN_required:
		break;
	case PH_FSIM_PUK_required:
		break;
	case SIM_not_inserted:
	case SIM_failure:
	case SIM_wrong:
		SIM_present = 0;
		leds_update(3,red,0);
		if(pin_entry_sent)
		{
			send0(CC,AT_SS_failure);
			pin_entry_sent = 0;
		}	
		break;
	case SIM_PIN_reqired:
	  	send0(CC,AT_SS_failure);	
		break;
	case SIM_PUK_required:
		break;
	case SIM_busy:
		break; 
	case incorrect_password:
		if(pin_entry_sent)
		{
			send0(CC,AT_SS_failure);
			pin_entry_sent = 0;
			wrong_passwd_count++;
		}
		if(SS_setup)
		{
			send0(CC,AT_SS_failure);
			SS_setup = 0;
		}
		break;
	case SIM_PIN2_required: break; case 
	SIM_PUK2_required: break; case 
	memory_full: break; case 
	invalid_index: break; case 
	not_found: break; case 
	memory_failure: break; case 
	text_string_too_long: break; case 
	invalid_characters_in_text_string: break;
	case dial_string_too_long:
	case invalid_characters_in_dial_string:
	case no_network_service:
	case network_timeout:
	case network_not_allowed_emergency_calls_only:
		if(SS_setup)
		{
			SS_setup = 0;
			send0(CC,AT_SS_failure);
		}
		break;
	case 
	network_personalisation_PIN_required: break; case 
	network_personalisation_PUK_required: break; case 
	network_subset_personalisation_PIN_required: break; case 
	network_subset_personalisation_PUK_required: break; case 
	service_provider_personalisation_PIN_required: break; case 
	service_provider_personalisation_PUK_required: break; case 
	corporate_personalisation_PIN_required: break; 
	case corporate_personalisation_PUK_required:
		break;
	case command_processing_in_progress:
		if(SS_setup)
		{
			SS_setup = 0;
			send0(CC,AT_SS_failure);
		}
		break;
	case retry_operator_selection:
		send_at_command("AT+COPS=0\r\n",900);
		break;
	default:;
	}
	return 1;
}

int CMS_ERROR(char *err)
{
	send0(CC,AT_SS_failure);
	return 1;
}

typedef enum crc_type_tag
{
 	ASYNC	= 0,  				// asynchronous transparent
 	SYNC,					// synchronous transparent
	REL_ASYNC,			// asynchronous non-transparent
	REL_SYNC,				// synchronous non-transparent
	FAX,					// facsimile (TS 62)
	VOICE,				// normal voice (TS 11)
	VOICE_ASYNC,			// voice followed by data (BS 81)
	VOICE_SYNC,			// voice followed by data (BS 81)
	VOICE_REL_ASYNC,		// voice followed by data (BS 81)
	VOICE_REL_SYNC,		// voice followed by data (BS 81)
	ALT_VOICE_ASYNC,		// alternating voice/data, voice first (BS 61)
	ALT_VOICE_SYNC,		// alternating voice/data, voice first (BS 61)
	ALT_VOICE_REL_ASYNC,	// alternating voice/data, voice first (BS 61)
	ALT_VOICE_REL_SYNC,	// alternating voice/data, voice first (BS 61)
	ALT_ASYNC_VOICE,		// alternating voice/data, data first (BS 61)
	ALT_SYNC_VOICE,		// alternating voice/data, data first (BS 61)
	ALT_REL_ASYNC_VOICE,	// alternating voice/data, data first (BS 61)
	ALT_REL_SYNC_VOICE,	// alternating voice/data, data first (BS 61)
	ALT_VOICE_FAX,		// alternating voice/fax, voice first (TS 61)
	ALT_FAX_VOICE		// alternating voice/fax, fax first (TS 61)
}crc_type;

crc_type process_crc_type(char *pas)
{
  int i;
  
  for(i=0;i<NUMBER_OF_CRC_TYPES;i++)
  {
  	if(strcmp(pas,CRC_type[i]) == 0)
  		return (crc_type)i;
  }
  
  return (crc_type)(NUMBER_OF_CRC_TYPES+1);
}

static int CID_time_display=0;

int CRING(char *pas)
{
	switch(process_crc_type(pas))
	{
	case ASYNC:
		DebugPrintf("Transparent async data call\r\n");
		send0(FM,FM_incoming_data);
		clcc_check_enabled = 0;
		break;
	case SYNC:
		DebugPrintf("Transparent Sync data call\r\n"); 
		clcc_check_enabled = 0;
		break;
	case REL_ASYNC: 
		DebugPrintf("Non-Transparent async data call\r\n");
		send0(FM,FM_incoming_data);
		clcc_check_enabled = 0;
	    break;
	case REL_SYNC:
		DebugPrintf("Non-Transparent sync data call\r\n");
		clcc_check_enabled = 0;
		break;
	case FAX:
		DebugPrintf("Fax call\r\n");
		send0(FM,FM_incoming_fax);
		clcc_check_enabled = 0;
		break;
	case VOICE:
	   	DebugPrintf("Voice call\r\n");
		break;
	}
	
   	if(timeSet && (CC_current_state == 0))
	{	send_at_command("AT+CCLK?\r\n",0);
		CID_time_display=1;
	}
	
	ring_count++;
	
//  MD - w/o this delay CID does not work while reviving from slow clock mode.
// 	send0(CC,AT_ringing);
   	send1_delayed(2,CC, AT_ringing, -1, NULL);
   		
	return 1;
}

enum
{
	CALL_active,
	CALL_held,
	CALL_dialing,
	CALL_alerting,
	CALL_incoming,
	CALL_waiting,
	CALL_TYPE_MAX
};

int CLCC(char *str)
{
	unsigned int idx, state, dir, mode, mpty;
	int i;
	int	present_calls, mpty_calls;
	int	call_counts[CALL_TYPE_MAX];
	int call_state_changed;
	CURRENT_CALL	temp_calls[MAX_CURRENT_CALLS];
	char *ptr, *ptr2;
	
	if(skip_clcc_reading)
	{
		skip_clcc_reading = 0;
		return 1;
	}

	/* clear 'present' flags for all calls */ 	
	for(i = 0; i < MAX_CURRENT_CALLS; i++)
		temp_calls[i].present = 0;
	
	ptr = str;

	while((ptr = strstr(ptr, "CLCC: ")) != NULL)
	{	
		ptr += 6;
		
		idx = *ptr - '0';
		ptr += 2;
		dir = *ptr - '0';
		ptr += 2;
		state = *ptr - '0';
		ptr += 2;
		mode = *ptr - '0';
		ptr += 2;
		mpty = *ptr - '0';
		
		/* perform validations */
		idx -= 1;
		if(idx > MAX_CURRENT_CALLS)
			return 1;
		if(dir > 1)
			return 1;
		if(state > 5)
			return 1;
		if(mode > 9)
			return 1;
		if(mpty > 1)
			return 1;
		
		if(state == 0) // active 
		{
			if(strlen(ptr)> 2 && (ptr[1] == ',')) 
			{	
				ptr += 3;
				ptr2 = strstr (ptr,"\"");
				if((ptr2-ptr) > 0)
				{
					strncpy(LASTCALL_number, ptr, (ptr2-ptr));
					LASTCALL_number[(ptr2-ptr)]='\0';
				}
			}
			else
				*LASTCALL_number='\0';

			
			LASTCALL_direction = dir;
			
		}
			
		temp_calls[idx].present = 1;
		temp_calls[idx].state = state;
		temp_calls[idx].dir = dir;
		temp_calls[idx].mode = mode;
		temp_calls[idx].mpty = mpty;
	}
	
	present_calls = 0;
	mpty_calls = 0;
	call_state_changed = 0;
	
	for(i = 0; i < CALL_TYPE_MAX; i++)
		call_counts[i] = 0;
	
	/* copy and analyze */	
	for(i = 0; i < MAX_CURRENT_CALLS; i++)
	{
		if(current_calls[i].present && !temp_calls[i].present)
		{
			/* call went away */
			if(current_calls[i].state != CALL_active)
				send0(SLAC, AT_inactive_disconnect);
			
			if(current_calls[i].state == CALL_waiting)
				send0(SLAC,SLAC_call_waiting_released);
			
			call_state_changed = 1;			
		}
		
		if(!current_calls[i].present && temp_calls[i].present)
		{
			/* new call appeared */
			call_state_changed = 1;
		}
		if(temp_calls[i].present)
		{
			present_calls++;
			
			if(current_calls[i].present)
			{
				if(temp_calls[i].state != current_calls[i].state)
					call_state_changed = 1;
				if(temp_calls[i].mpty != current_calls[i].mpty)
					call_state_changed = 1;
				
				if(temp_calls[i].state == CALL_active &&
					(current_calls[i].state == CALL_alerting ||
					 current_calls[i].state == CALL_dialing  ||
					 current_calls[i].state == CALL_incoming  ||
					 current_calls[i].state == CALL_waiting))
					send0(CC, AT_connect);
			}
				
			call_counts[temp_calls[i].state]++;
			
			if(temp_calls[i].mpty)
				mpty_calls++;
		}
		
		memcpy(&current_calls[i], &temp_calls[i], sizeof(CURRENT_CALL));		
	}
	
	if(present_calls == 0)
		send0(CC, AT_disconnect);
	else if(call_state_changed)
	{
		if(call_counts[CALL_active] != 0)
			send0(CC, AT_call_active);
		else if(call_counts[CALL_held] == present_calls)
			send0(CC, AT_call_held);
	}
	
	num_current_calls = present_calls;
	num_held_calls = call_counts[CALL_held];
	num_mpty_calls = mpty_calls;
		
	return 1;
}

typedef enum pin_type_tag
{
	READY		=0,		// ME is not pending for any password
	SIM_PIN,			// ME is waiting SIM PIN to be given
	SIM_PUK,			// ME is waiting SIM PUK to be given
	PH_SIM_PIN,			// ME is waiting phone-to-SIM card password to be given 
	PH_FSIM_PIN,		// ME is waiting phone-to-very first SIM card password to be given
	PH_FSIM_PUK,		// ME is waiting phone-to-very first SIM card unblocking password to be given
	SIM_PIN2,			// ME is waiting SIM PIN2 to be given 
	SIM_PUK2,			// ME is waiting SIM PUK2 to be given 
	PH_NET_PIN,			// ME is waiting network personalisation password to be given
	PH_NET_PUK,			// ME is waiting network personalisation unblocking password to be given
	PH_NETSUB_PIN,		// ME is waiting network subset personalisation password to be given
	PH_NETSUB_PUK,		// ME is waiting network subset personalisation unblocking password to be given
	PH_SP_PIN,			// ME is waiting service provider personalisation password to be given
	PH_SP_PUK,			// ME is waiting service provider personalisation unblocking password to be given
	PH_CORP_PIN,		// ME is waiting corporate personalisation password to be given
	PH_CORP_PUK			// ME is waiting corporate personalisation unblocking password to be given
} pin_type;

pin_type process_pin_type(char *pas)
{
  int i;
  
  for(i=0;i<NUMBER_OF_PIN_TYPES;i++)
  {
  	if(strcmp(pas,PIN_type[i]) == 0)
  		return (pin_type)i;
  }
  
  return (pin_type)(NUMBER_OF_PIN_TYPES+1);
}

int CPIN(char *pas)
{
	char str[MAX_PIN_LENGTH]="";
	int i;
	char *ptr;
	
	switch(process_pin_type(pas))
	{
	case READY:
		SIM_present = 1;
		PIN_not_required = 1;
		DebugPrintf("ME is not pending for any password\r\n");
		StopPeriodicPINCheck((void*)0);
		StartPeriodicCREGCheck((void*)0);
		leds_update(3,green,0);
		if(pin_entry_sent)
		{
			pin_entry_sent = 0;
			send0(CC, AT_SS_confirm);
			wrong_passwd_count = 0;
						
			// Incase of PUK,PIN entery
			
			if(PUK_not_required == 0 && ((ptr = strstr(pin,",")) != NULL))
			{
				strcpy(pin,++ptr);
			}
					
			UpdateFlashData("PIN",pin);
		}
		PUK_not_required = 1;
		break;
	case SIM_PIN:
		SIM_present = 1;
		PIN_not_required = 0;
		PUK_not_required = 1;
		DebugPrintf("ME is waiting SIM PIN to be given\r\n");
		send0(SLAC,SLAC_no_sim);
		leds_update(3,red,50);
		StartPeriodicPINCheck((void*)CPIN_PREIOD);
		if(autopin_enabled && (wrong_passwd_count == 0))
		{
			strcpy(pin,ReadFlashData("PIN"));
			
			pin_entry_sent = 1;
    		pin_entry_countdown = 4;
			memcpy(str,"AT+CPIN=",8);
       		for(i = 0 ; i < strlen(pin); i++)
       			str[i+8]=pin[i];
       		str[i+8]='\r';
       		str[i+9]='\0';
        	send_at_command(str,10);
		}
		if(pin_entry_sent && (--pin_entry_countdown == 0))
		{
			pin_entry_sent = 0;
			send0(CC, AT_SS_failure);
		}
		break;
	case SIM_PUK:
		SIM_present = 1;
		PIN_not_required = 1;
		PUK_not_required = 0;
		send0(SLAC,SLAC_no_sim);
		leds_update(3,red,10);
		StartPeriodicPINCheck((void*)CPIN_PREIOD);
		if(pin_entry_sent && (--pin_entry_countdown == 0))
		{
			pin_entry_sent = 0;
			send0(CC, AT_SS_failure);
		} 
		DebugPrintf("ME is waiting SIM PUK to be given\r\n");
	    break;
	case PH_SIM_PIN:
		DebugPrintf("ME is waiting phone-to-SIM card password to be given\r\n");
		break;
	case SIM_PIN2:
		DebugPrintf("ME is waiting SIM PIN2 to be given\r\n"); 
		break;
	case SIM_PUK2:
	   	DebugPrintf("ME is waiting SIM PUK2 to be given\r\n");
	   	break;
	case PH_NET_PIN:
		DebugPrintf("ME is waiting network personalisation password to be given\r\n");
		break;
	}
	return 1;
}

int CLCK(char *sercice, char *status, char *passwd)
{
	return 1;
}


int CCWA(char *number)
{
	int i;
	char CID_msg[MAX_DIGIT_COUNT]= "";
	int CID_length;
    
	/* check if response is solicited */	
	if(number[0] >= '0' && number[0] <= '9')
	{
		if(SS_setup == 1)
		{
			switch(number[0])
			{
				case '0':	/* disabled */
					send0(CC, AT_SS_neg_status);
					break;
				case '1':	/* enabled */
					send0(CC, AT_SS_confirm);
					break;
				default:
					break;			
			}
			SS_setup = 0;
		}
		return 1;
	}

	/* response is unsolicited */ 
	
	CID_length = strlen(number)-2;
	
	if(CID_length >= 0)
	{
		for(i=1;i<=CID_length;i++)
			CID_msg[i-1] = number[i];
		CID_msg[i] = '\0';
		CID_Name_Number("", CID_msg, "");
    }
    else
    {
    	/* Clear Caller ID data */
  		for(i=0;i<MAX_DIGIT_COUNT;i++)
  			CID_msg[i]= 0;
  		ClearCidParams();
  		CID_length=0;
    }

    send0(SLAC,SLAC_call_waiting);

    return 1;
}

static char CID_msg[MAX_DIGIT_COUNT]= "";

int CLIP(char *number)
{
	
	int i;
	int CID_length;
	
	if(ring_count == 1)
	{ 
		CID_length = strlen(number)-2;
		for(i=1;i<=CID_length;i++)
		 	CID_msg[i-1] = number[i];
		
		CID_msg[i] = '\0';
		
		DebugPrintf(CID_msg);
		DebugPrintf("\r\n");
		
		if(!timeSet)
		{
			CID_Name_Number("", CID_msg, "");
			memset(CID_msg,0,strlen(CID_msg));
		}
				
	}
    
    return 1;
}

int CLIR(char *number, char *status)
{
	if(SS_setup == 1)
	{
		switch(number[0])
		{
			case '0':	/* network */
				send0(CC, AT_SS_neg_status);
				break;
			case '1':	/* invoked */
				send0(CC, AT_SS_confirm);
				break;
			case '2':	/* suppressed */
				send0(CC, AT_SS_neg_status);
			default:
				break;			
		}
		SS_setup = 0;
	}

    return 1;
}

int CMGR(char *number, char *name, char *date, char *time)	//SMS Read
{
	int i;
	int SMSnumlength;
	char str[100];
		
	SMSnumlength = strlen(number)-2;
	if(number[1] == '+')
	{
		for(i=2;i<=SMSnumlength;i++)
	 		SMSnumber[i-2] = number[i];
	}
	else
	{
		for(i=1;i<=SMSnumlength;i++)
	 		SMSnumber[i-1] = number[i];
	}
		
	SMSnumber[i] = '\0';
	
	
	SMSdatetime[0]=date[4];	//month
	SMSdatetime[1]=date[5];
	
	SMSdatetime[2]=date[7];	//day
	SMSdatetime[3]=date[8];
	
	SMSdatetime[4]=time[0];	//hour
	SMSdatetime[5]=time[1];
	
	SMSdatetime[6]=time[3];	//minute
	SMSdatetime[7]=time[4];
	
	SMSdatetime[8]= '\0';	
	
	sprintf(str,"CID: %s %s %s %s \r\n",SMSnumber,SMSdatetime,date,time);

	DebugPrintf(str);		 		
	return 1;
}

int CMTI(char *SMS_index) // New SMS
{
	char CID_txt[16]="New SMS #";
	
	strcat(CID_txt,SMS_index);
	
	if (SMSonCID_enabled)
	{
		bAwake = 1;
		CID_OnhookPrintf(CID_txt,"");  
		DebugPrintf(CID_txt);
		DebugPrintf("\r\n");
	}
	return 1;
	
}
static char CID_txt[90]="SMS #";
static int CMGL_count = 0;

int CMGL(char *SMS_index, char *SMS_status) // SMS list
{
	CMGL_count++;
		
	if(CMGL_count < number_of_SMS)
	{
		strcat(CID_txt,SMS_index);
		strcat(CID_txt,",");
	}
	else
	{
		strcat(CID_txt,SMS_index);
		waiting_CID = numberOfCIDSessions(CID_txt);
		bAwake = 1;
		
		DebugPrintf(CID_txt);
		DebugPrintf("\r\n");
		
		strcpy(SMS_message,CID_txt);
		
		CMGL_count = 0;
		strcpy(CID_txt,"SMS #");
	}
	
	return 1;
}

int CPMS(char *memory, char *used, char *total)
{
	
	if(!strstr(memory,"\"SM\""))			// We are only dealing with SIM storage
	{
		send_at_command("AT+CPMS=\"SM\"\r\n",0);
		send_at_command("AT+CPMS?\r\n",30);
	}	
	else
	{
		number_of_SMS = atoi(used);
	}
	return 1;
	
}

static int start_hh=0;
static int start_mm=0;
static int start_ss=0;
static int stop_hh=0;
static int stop_mm=0;
static int stop_ss=0;

int CCLK(char *date, char *time)
{
	char hh[3]="";
	char mm[3]="";
	char ss[3]="";
	char CID_time[9]="";
	
	hh[0]=time[0];
	hh[1]=time[1];
	hh[2]='\0';
	
	mm[0]=time[3];
	mm[1]=time[4];
	mm[2]='\0';
	
	ss[0]=time[6];
	ss[1]=time[7];
	ss[2]='\0';
	
	if(CID_time_display)
	{
		
		CID_time[0]=date[4];
		CID_time[1]=date[5];
		
		CID_time[2]=date[7];
		CID_time[3]=date[8];
		
		CID_time[4]=hh[0];
		CID_time[5]=hh[1];

		CID_time[6]=mm[0];
		CID_time[7]=mm[1];
		
		CID_time[8]='\0';
		
		CID_Name_Number("", CID_msg, CID_time);

		memset(CID_msg,0,strlen(CID_msg));
		CID_time_display = 0;
		
		return 1;
	}
	
	if(Start_Call_timer)
	{
		
		if(Multi_Call_timer == 0)
		{
			start_hh= atoi(hh);
			start_mm= atoi(mm);
			start_ss= atoi(ss);
		}
		else
		{
			stop_hh= atoi(hh);
			stop_mm= atoi(mm);
			stop_ss= atoi(ss);
			
			CallDuration();	
			
			start_hh= stop_hh;
			start_mm= stop_mm;
			start_ss= stop_ss;
			
		}
		
//		UpdateFlashData("LASTNUMBER",LASTCALL_number);
		RetainDelayedParams("LASTNUMBER", LASTCALL_number);
		Multi_Call_timer =1;	
	}
	else
	{
		stop_hh= atoi(hh);
		stop_mm= atoi(mm);
		stop_ss= atoi(ss);
	
		CallDuration();	
		StoreDelayedParams();
		ClearDelayedParams();
	}
	
	Start_Call_timer = 0;
	
	return 1;
}

char *ShowAllCalls()
{
	char ALLCALLS_duration[15]="";
	
	unsigned long int totalseconds;
	unsigned long int minutes;
	unsigned long int hours;
	unsigned long int seconds;
	unsigned long int days;
	
	totalseconds=atol(ReadFlashData("ALLCALLS"));
	
	minutes= totalseconds / 60;
	seconds= totalseconds - (minutes * 60);
	hours= minutes / 60;
	minutes= minutes - (hours * 60);
	
	if(hours > 9999)
	{
		days= hours / 24;
		hours= hours - (days * 24);
		
		sprintf(ALLCALLS_duration, "ACM:   %05lu:%02lu",days,hours);
		
		return ALLCALLS_duration;
	}
	
	sprintf(ALLCALLS_duration, "ACM: %4lu:%02lu:%02lu",hours,minutes,seconds);
	
	return ALLCALLS_duration;
}

void UpdateAllCalls(int hh, int mm, int ss)
{
	unsigned long int totalseconds;
	char newtotal[20]="";
	
//	totalseconds= atol(ReadFlashData("ALLCALLS"));
	totalseconds= atol(GetAllCallData("ALLCALLS"));
		
	totalseconds= totalseconds + ss + ((mm + (hh *60)) * 60);
	
	sprintf(newtotal,"%lu",totalseconds);
	
//	UpdateFlashData("ALLCALLS",newtotal);
	RetainDelayedParams("ALLCALLS", newtotal);

}

char CallDirection()
{
	char dir;
	
	switch(LASTCALL_direction)
	{
		case 0: //MO
		dir ='>';
		break;
		case 1: //MT
		dir ='<';
		break;
		default:
		dir =' ';
	}
	
	LASTCALL_direction = 2; // Non valid direction
	
	return dir;	
}

void CallDuration(void)
{
	int hh, mm, ss;
	
	if(stop_ss >= start_ss)
		ss = stop_ss - start_ss;
	else
	{
		ss = (stop_ss + 60) - start_ss;
		if(stop_mm == 0 )
		{
			stop_mm = 59;
			if(stop_hh == 0)
				stop_hh= 23;
			else
				stop_hh--;
		}
		else
			stop_mm--;
		
	}
	
	if(stop_mm >= start_mm)
		mm = stop_mm - start_mm;
	else
	{
		mm = (stop_mm + 60) - start_mm;
		if(stop_hh == 0)
			stop_hh = 23;
		else
			stop_hh--;
	}
		
	if (stop_hh >= start_hh)
		hh = stop_hh - start_hh;
	else
		hh = (stop_hh + 24) - start_hh;  // Calls longer then 24 hour will not be reported correctly!

	sprintf(LASTCALL_duration, "CCM: %c %02d:%02d:%02d",CallDirection(),hh,mm,ss);
	
//	UpdateFlashData("LASTCALL",LASTCALL_duration);
	RetainDelayedParams("LASTCALL", LASTCALL_duration);
	
	UpdateAllCalls(hh,mm,ss);

}

static unsigned long ccm = 0;
int CCCM(char *CCM) // Current Call Meter
{
	int i;
	unsigned long int newccm;
	unsigned long int charge;
	char cccm[7];
	
	for(i=0;i<6;i++)
		cccm[i]=CCM[i+1];
	cccm[i]='\0';
		
	newccm = strtol(cccm,NULL,16);
		
	charge = (newccm - ccm) * homeUnitSF;
	
	ccm= newccm;
	
	send1(SLAC, SLAC_supv_aoc, 0, (int*)charge);
	return 1;
}

/* Wavecom Extensions */

enum wind_val_tag
{
	sim_removed	=	'0',
	sim_inserted,
	ring_back,
	at_ready_after_init,
	at_ready,
	call_idx_created,
	call_idx_released,
	emergency_mode,
	network_lost
} wind_val; 

enum wind_call_idx_tag
{
	first = '1',
	second 
} wind_call_idx;

int WIND(char *pas, char *idx)
{
	int call_idx;
	
	switch(pas[0])
	{
	case sim_removed:
		leds_update(3,red,0);
		SIM_present=0;    
		break;
	case sim_inserted:
		SIM_present=1;  
		StartPeriodicPINCheck((void*)CPIN_PREIOD);
		if(GSM_ready)
			GSM_ready = 0; // to wake-up from power saving 
		break;
	case ring_back:
		DebugPrintf("wind: ringback\r\n");
		send0(SLAC,SLAC_ring_back);
		break;
	case at_ready_after_init:
		StartPeriodicPINCheck((void*)CPIN_PREIOD); 
		StopPeriodicCREGCheck((void*)0);
	   	break;
	case at_ready:
	    DebugPrintf("AT READY\r\n");
	    if(chargingEnabled)
  			send_at_command("AT+CLIP=1;+CCWA=1;+CAOC=2\r\n",0);
  		else
  			send_at_command("AT+CLIP=1;+CCWA=1\r\n",0);
	    break;
	case call_idx_created:
        break;
	case call_idx_released:
		call_idx = atoi(idx) - 1;
		if(call_idx >= MAX_CURRENT_CALLS || call_idx < 0)
			break;
				
		if(current_calls[call_idx].present)
		{
			if(current_calls[call_idx].state == CALL_waiting)
			{   
				DebugPrintf("wind: waiting call released\r\n");
				send0(SLAC,SLAC_call_waiting_released);
			}
			else if(current_calls[call_idx].state == CALL_active)
			{
				if(num_current_calls == 1)
					send0(CC, AT_disconnect);
			}
		
			current_calls[call_idx].present = 0;
		}
		break;
	case emergency_mode:
		break;
	case network_lost:
		break;
	}
	return 1;
}

int held_call_exists(void)
{
	int i;
	int found;
	
	found = 0;
	for(i = 0; i < MAX_CURRENT_CALLS; i++)
	{
		if(current_calls[i].present && current_calls[i].state == CALL_held)
		{
			found = 1;
			break;
		}
	}

	return found;
}


void ss_call_handler(int parm)
{
	int i;
	char str_buf[32];
     
    if(parm == 1)
    {
    	if(held_call_exists())
	    {
	    	/* check if there are any incomplete outgoing calls */
	    	for(i = 0; i < MAX_CURRENT_CALLS; i++)
	    	{
	    		if(!current_calls[i].present)
	    			continue;
	    		
	    		if(current_calls[i].state != CALL_dialing && current_calls[i].state != CALL_alerting)
	    			continue;
	
				/* drop an incomplete outgoing call by call index */    		
	    		sprintf(str_buf, "AT+CHLD=1%d;+CHLD=2\r\n", i+1);
	    		send_at_command(str_buf,0);
	    		return;   		
	    	}
	     }
	     else
	    { 
	     	send_at_command("AT+CHLD=1\r\n",0);
	    	return;
	    }
	} 	
	sprintf(str_buf,"AT+CHLD=%d\r\n", parm);
	send_at_command(str_buf,0);
}
static int SMS_count = 0;

void handleSMS()
{
	int i,len;
	char SMS_buf[16]="";
	
	strncpy(SMS_buf,SMS_message,15);
	SMS_buf[15]='\0';
	SMS_count++;
	
	if(strcmp(SMSindex,""))
	{
		CID_Name_Number(SMS_buf, SMSnumber,SMSdatetime);
		sprintf(SMSnumber,"%6s%3s%4d", "",SMSindex,SMS_count+1); 
	}
	else	
		CID_Name(SMS_buf);
	
	len = strlen(SMS_message) -15;
	if(len > 0)
	{
		for(i=0;i<len;i++)
			SMS_message[i]=SMS_message[i+15];
		SMS_message[i] = '\0';
	}
	
}

void handleInfoDisplay()
{
	int i,len;
	char INFO_buf[16]="";
	
	strncpy(INFO_buf,INFO_name,15);
	INFO_buf[15]='\0';
	
	if(strcmp(INFO_number,""))
	{
		CID_Name_Number(INFO_buf, INFO_number,"");
	}
	else	
		CID_Name(INFO_buf);
	
	len = strlen(INFO_name) -15;
	if(len > 0)
	{
		for(i=0;i<len;i++)
			INFO_name[i]=INFO_name[i+15];
		INFO_name[i] = '\0';
	}
	
}

void clearINFOparams()
{
	memset(INFO_number, 0, strlen(INFO_number));
	memset(INFO_name, 0, strlen(INFO_name));
}

int numberOfCIDSessions(char *msg)
{
	int index;
	
	index= (strlen(msg)/ 15)+1;
	
	if((strlen(msg) % 15) == 0)
		index--;	
	
	return index;
}

void clearSMSParams(void)
{
	memset(SMSnumber,0,strlen(SMSnumber));
    memset(SMSdatetime,0,strlen(SMSdatetime));
    memset(SMSindex,0,strlen(SMSindex));
    memset(SMS_message,0, strlen(SMS_message));
    SMS_count = 0;
}

void setICF(char format,char parity)
{
	int parity_type;
	
	parity_type = (int)parity - 0x30;
	
	switch(format)
	{
		case 0: // not supported
			break;
		case '1': // 8 Data 2 Stop
			set_duart_word_len(8);
			set_duart_stop_bits(2);
			set_duart_parity(4); //no parity
			break;
		case '2': // 8 Data 1 parity 1 stop
			set_duart_word_len(8);
			set_duart_stop_bits(2);
			if(parity_type == 4)	//Wavecom Fix
				set_duart_parity(3);
			else
				set_duart_parity(parity_type);	
			break;
		case '3': // 8 Data 1 Stop
			set_duart_word_len(8);
			set_duart_stop_bits(1);
			set_duart_parity(4); //no parity
			break;
		case '4': // 7 Data 2 Stop
			set_duart_word_len(7);
			set_duart_stop_bits(2);
			set_duart_parity(4); //no parity
			break;
		case '5': // 7 Data 1 Parity 1 Stop
			set_duart_word_len(7);
			set_duart_stop_bits(1);
			if(parity_type == 4)	//Wavecom Fix
				set_duart_parity(3);
			else
				set_duart_parity(parity_type);
			break;
		case '6': // 7 Data 1 Stop
			set_duart_word_len(7);
			set_duart_stop_bits(1);
			set_duart_parity(4); //no parity
			break;
	}
	
}

void setIPR(char *ipr)
{
	unsigned long rate;
	
	rate= atol(ipr);
	
	switch(rate)
	{
		case 0: // Autoboud
			uart_a_rate(UART_BAUD_19200);
			//uart_a_rate(UART_BAUD_57600); // Alex
			break;
		case 300:
			uart_a_rate(UART_BAUD_300);
			uart_b_rate(UART_BAUD_300);
			break;
		case 600:
			uart_a_rate(UART_BAUD_600);
			uart_b_rate(UART_BAUD_600);
			break;
		case 1200:
			uart_a_rate(UART_BAUD_1200);
			uart_b_rate(UART_BAUD_1200);
			break;
		case 2400:
			uart_a_rate(UART_BAUD_2400);
			uart_b_rate(UART_BAUD_2400);
			break;
		case 4800:
			uart_a_rate(UART_BAUD_4800);
			uart_b_rate(UART_BAUD_4800);
			break;
		case 9600:
			uart_a_rate(UART_BAUD_9600);
			uart_b_rate(UART_BAUD_9600);
			break;
		case 19200:
			uart_a_rate(UART_BAUD_19200);
			uart_b_rate(UART_BAUD_19200);
			break;
		case 38400:
			uart_a_rate(UART_BAUD_38400);
			uart_b_rate(UART_BAUD_38400);
			break;
		case 57600:
			uart_a_rate(UART_BAUD_57600);
			uart_b_rate(UART_BAUD_57600);
			break;
		case 115200:
			uart_a_rate(UART_BAUD_115200);
			uart_b_rate(UART_BAUD_115200);
			break;
	}
}


