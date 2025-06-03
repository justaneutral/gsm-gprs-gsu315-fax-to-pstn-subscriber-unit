#include <math.h>
#include "su.h"
#include "modems.h"

void ClearInterruptRegs(void);

// Globals
int SLAC_isr_flag=0;
DIALINGPARAMETERS DialingParams;
BOOLEAN bRingBackTest = FALSE;

// for debug msg strings
static char strBuf[80];

// Statics
static BOOLEAN bOnHook = TRUE;
static BOOLEAN bFirstDigit = FALSE;
static BOOLEAN bfirstRing;
static BOOLEAN bValid = TRUE;
static BOOLEAN bDtmfCidDone = FALSE;

static BYTE IntRegData;

static BOOLEAN bTwoWayCall = TRUE;
static BOOLEAN bcall_flashed = FALSE;

static int	pulse_count = 0;

static SUPV_EVENTS supv_events;
static SUPV_ACTION *actions;
static int SupvBlocked = NOT_BLOCKED;
static BOOLEAN bCallConnected = FALSE;

// Externs
extern int SIM_present;
extern int PIN_not_required;
extern int PUK_not_required;
extern int REG_status;

extern int CC_current_state;
extern int into_caller_id;
extern int waiting_CID;
extern int PolingWavecom;
extern char SMS_message[];
extern int bAwake;
extern int chargingEnabled;
extern FLASH_PARAMS FlashRecords[MAX_RECORDS];


//volume control.
//    Phone         slic   slac ch 1  slac ch 2     Wavecom
//     
//                 OOOOOO  Tx 111111  222222 Rx  Tx WWWWW 
//   Microphine -->OOOOOO-----111111=>222222------->WWWWW
//                 OOOOOO     111111  222222        WWWWW
//                 OOOOOO  Rx 111111  222222 Tx  Rx WWWWW
//   Telephone <---OOOOOO<----111111<=222222<-------WWWWW
//                 OOOOOO     111111  222222        WWWWW
//
unsigned short rx_gain_table_b1[13]=
	{0x20,0x24,0x28,0x2d,0x32,0x39,0x40,0x47,0x50,0x5a,0x65,0x72,0x7f};
unsigned short rx_gain_table_b2[13]=
	{0x26,0x13,0x7a,0x6a,0xf5,0x2c,0x26,0xfa,0xc3,0x9d,0xac,0x14,0xff};
unsigned short tx_gain_table_b1[13]=
	{0xe0,0xe3,0xe8,0xed,0xf2,0xf8,0xff,0x07,0x10,0x1a,0x25,0x31,0x3f};
unsigned short tx_gain_table_b2[13]=
	{0x00,0xe8,0x4a,0x34,0xb8,0xe8,0xda,0xa3,0x61,0x30,0x31,0x8a,0x64};

void WriteSlac1RxGain(int gain)
{
	if(!WSU003_Config)
  	{
  		//WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  		//WriteSingleByte(ENABLE_CHNL_1_RW);
  		WriteSingleByte(WRITE_RECEIVE_GAIN);
  		WriteSingleByte(rx_gain_table_b1[gain]);
  		WriteSingleByte(rx_gain_table_b2[gain]);
  		WriteSingleByte(WRITE_RECEIVE_GAIN+1);
  		DebugPrintf1("Slac 1 Rx gain byte 1: ",ReadSingleByte());
  		DebugPrintf1("Slac 1 Rx gain byte 2: ",ReadSingleByte());
  	}
}  

void WriteSlac1TxGain(int gain)
{
	if(!WSU003_Config)
  	{
		WriteSingleByte(WRITE_TRANSMIT_GAIN);
  		WriteSingleByte(tx_gain_table_b1[gain]);
  		WriteSingleByte(tx_gain_table_b2[gain]);
  		WriteSingleByte(WRITE_TRANSMIT_GAIN+1);
  		DebugPrintf1("Slac 1 Tx gain byte 1: ",ReadSingleByte());
  		DebugPrintf1("Slac 1 Tx gain byte 2: ",ReadSingleByte());
  	}
}

void WriteSlac2RxGain(int gain)
{
	if(!WSU003_Config)
  	{
  		WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  		WriteSingleByte(ENABLE_CHNL_2_RW);
  		WriteSingleByte(WRITE_RECEIVE_GAIN);
  		WriteSingleByte(rx_gain_table_b1[gain]);
  		WriteSingleByte(rx_gain_table_b2[gain]);
  		WriteSingleByte(WRITE_RECEIVE_GAIN+1);
  		DebugPrintf1("Slac 2 Rx gain byte 1: ",ReadSingleByte());
  		DebugPrintf1("Slac 2 Rx gain byte 2: ",ReadSingleByte());
  		WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  		WriteSingleByte(ENABLE_CHNL_1_RW);
  	}
}  

void WriteSlac2TxGain(int gain)
{
	if(!WSU003_Config)
  	{
  		WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  		WriteSingleByte(ENABLE_CHNL_2_RW);
		WriteSingleByte(WRITE_TRANSMIT_GAIN);
  		WriteSingleByte(tx_gain_table_b1[gain]);
  		WriteSingleByte(tx_gain_table_b2[gain]);
  		WriteSingleByte(WRITE_TRANSMIT_GAIN+1);
  		DebugPrintf1("Slac 2 Tx gain byte 1: ",ReadSingleByte());
  		DebugPrintf1("Slac 2 Tx gain byte 2: ",ReadSingleByte());
  		WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  		WriteSingleByte(ENABLE_CHNL_1_RW);
  	}
}

int volume_control_monitor(char dch)
{
	static int state_index = 0;
	static int device;
	static int tx;
	int ret_val;
	
	static char at_com[14]="at+vgr=000\r\n";
	static int idval=0;
	
	ret_val=0;
	
	switch(state_index)
	{
		case 0: //#
		case 1: //##
		case 2: //###
		case 4: //###d#
		case 6: //###d#s#
			if(dch == '#')
			{
				state_index++;
			}
			else
				state_index=0;
			break;
		case 3: //###d
			device = dch-'1';
			switch(device)
			{
				case 0: //slac #1
				case 1: //slac #2
				case 2: //wavecom
					state_index=4;
					break;
				default:
					state_index=0;
			}
			break;		
		case 5: //###d#s
			tx = dch-'1';
			switch(tx)
			{
				case 0: //rx
				case 1: //tx
					strcpy(at_com,"at+vgr=000\r\n");
					idval=0;
					state_index=6;
					break;
				default:
					state_index=0;
			}
			break;
		case 7:
		case 8:
		case 9:
			at_com[state_index++]=dch;
			idval = idval*10 + dch - 0x30;
			if(state_index==9)
			{
				if(device==0 && tx)
				{
					WriteSlac1TxGain(idval);
					state_index=0;
					ret_val=1;
				}
				else if(device==0 && !tx)
				{
					WriteSlac1RxGain(idval);
					state_index=0;
					ret_val=1;
				}
				else if(device==1 && tx)
				{
					WriteSlac2TxGain(idval);
					state_index=0;
					ret_val=1;
				}
				else if(device==1 && !tx)
				{
					WriteSlac2RxGain(idval);
					state_index=0;
					ret_val=1;
				}
			}
			else if(device == 2 && state_index ==10)
			{
				if(tx)
				{
					at_com[5]='t';
				}
				uart_a_fputs(at_com);
				DebugPrintf(at_com);
				state_index=0;
				ret_val=1;
			}
			else if(state_index>=10)
			{
				state_index=0;
			}
			break;
		default:
			state_index=0;
	}
	return ret_val;
}


CB	timer_control_block =
{ 
	{ -1, -1, -1, -1}
};


const STATE_CONTEXT state_table[MAX_STATES]=
{
	{
		initial,	//modified
		{off_timers,check_waiting_CID,slac_restart,on_hook_to_cc,set_to_valid},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_ring,SLAC_init,SLAC_cid_messages,0,0,0,0},
		{valid_hook,ring0,initial,cid_messages,0,0,0,0}
	},
	{
		valid_hook,	//added
		{start_ignore_slac_interrupt,0,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_disable_interrupt_timeout,0,0,0,0,0,0,0},
		{initial,off_hook,0,0,0,0,0,0,0}
	},
	{
		off_hook,	// Now, handles single & held calls
		{off_hook_to_cc,check_gsm_state,clear_dialing_parameters,set_to_valid,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_service_available,SLAC_connect,SLAC_disconnect,SLAC_ring_back,SLAC_no_service,SLAC_no_sim,SLAC_dce_pump,0},
		{initial,dial,connectstate,/*networkbusy*/disconnect,ringback,nosbusy,nosimbusy,pumpbusy,0}
	},
	{
		dial,
		{off_timers,slac_restart,off_hook_to_cc,first_digit_timer,on_dial_tone},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_network_busy,0,0,0},
		{pulse_detect,collectdigits,disconnect,networkbusy,pumpbusy,networkbusy,0,0,0}
	},
	{
		disconnect,	//modified
		{off_timers,slac_restart,on_disconnect_timer,transient_func,Mute},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_disconnect,SLAC_unconditional_branch,SLAC_dce_pump,SLAC_call_active,0,0,0,0},
		{flash_detect,networkbusy,disconnect1,pumpbusy,connectstate,0,0,0,0}
	},
	{
		disconnect1,
		{on_busy_timer,on_busy,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy_timeout,SLAC_disconnect_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_call_active,0,0,0},
		{flash_detect,disconnect2,disconnect3,networkbusy,pumpbusy,connectstate,0,0,0}
	},
	{
		disconnect2,
		{on_busy_timer,off_generator_tone,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy_timeout,SLAC_disconnect_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_call_active,0,0,0},
		{flash_detect,disconnect1,disconnect3,networkbusy,pumpbusy,connectstate,0,0,0}
	},
	{
		disconnect3,
		{off_timers,slac_restart,start_slac_off_timer,transient_func,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_unconditional_branch,SLAC_dce_pump,0,0,0,0,0,0},
		{initial,disconnect4,pumpbusy,0,0,0,0,0,0}
	},
	{
		disconnect4,
		{start_roh_timer,on_roh,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_roh_timeout,SLAC_slic_off_timeout,SLAC_dce_pump,0,0,0,0,0},
		{initial,disconnect5,disconnectslic,pumpbusy,0,0,0,0,0}
	},
	{
		disconnect5,
		{off_generator_tone,start_roh_timer,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_roh_timeout,SLAC_slic_off_timeout,SLAC_dce_pump,0,0,0,0,0},
		{initial,disconnect4,disconnectslic,pumpbusy,0,0,0,0,0}
	},
	{
		disconnectslic,
		{off_timers,slac_restart,DisconnectSlic,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_dce_pump,0,0,0,0,0,0,0},
		{initial,pumpbusy,0,0,0,0,0,0,0}
	},
	{
		collectdigits,
		{off_timers,off_generator_tone,start_digit_timer,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_network_busy,0,0,0},
		{initial,collectdigits,callprogress,networkbusy,pumpbusy,networkbusy,0,0,0}
	},
	{
		callprogress,
		{off_timers,slac_restart,send_digits_to_cc,Communication,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_ring_back,SLAC_connect,SLAC_network_busy,SLAC_SS_confirm,SLAC_SS_failure,SLAC_neg_status,0},
		{flash_detect,disconnect,ringback,connectstate,networkbusy,ssconfirm,ssfailure,ssnegstatus,0}
	},
	{
		ringback,
		{on_ring_back,start_ring_back_on_timer,Mute,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_ring_back_on_timeout,SLAC_busy,SLAC_connect,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_SS_failure,0},
		{flash_detect,ringback1,disconnect,connectstate,networkbusy,networkbusy,pumpbusy,ssfailure,0}
	},
	{
		ringback1,
		{off_generator_tone,start_ring_back_off_timer,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_ring_back_off_timeout,SLAC_busy,SLAC_connect,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_SS_failure,0},
		{flash_detect,ringback,disconnect,connectstate,networkbusy,networkbusy,pumpbusy,ssfailure,0}
	},
	{
		connectstate,
		{off_timers,slac_restart,off_hook_to_cc,Communication,connect_to_cc},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_call_waiting,SLAC_call_held,0,0},
		{flash_detect,disconnect,networkbusy,
#ifndef _PAYPHONE_
											 off_hook
#else
											 disconnect
#endif
													   ,pumpbusy,handle_call_waiting,ss_prompt0,0,0}
	},
	{
		handle_call_waiting, // modified for CWCID
		{ten_sec_call_waiting_sas,on_call_waiting_sas_delay,CallWaitSetUp,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_call_waiting_sas_on,0,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,call_waiting_cas0,0,0,0}
	},
	{
		call_waiting_sas0,  // modified for CWCID - currently not used
		{On_CallWaitingTone,off_call_waiting_sas,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_call_waiting_sas_off,0,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,call_waiting_sas1,0,0,0}
	},
	{
		call_waiting_sas1,  // modified for CWCID - currently not used
		{off_generator_tone,on_cas_delay,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_cas_on,0,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,call_waiting_cas0,0,0,0}
	},
	{
		call_waiting_cas0,
		{on_CasTone,off_cas,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_cas_off,0,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,call_waiting_idle,0,0,0}
	},
	{
		call_waiting_sas2,
		{On_CallWaitingTone,off_call_waiting_sas,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_call_waiting_sas_off,0,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,connectstate,0,0,0}
	},
	{
		call_waiting_idle, // modified for CWCID & ss svce improvements
		{off_generator_tone,mute_codec_delay,0,0,0},
		{0,(void *)mute_delay_fail_timeout,0,0,0},
		{SLAC_on_hook,SLAC_busy,SLAC_network_busy,SLAC_disconnect,SLAC_dce_pump,SLAC_ten_sec_cw_sas_on,SLAC_call_waiting_released,0,0},
		{flash_detect,disconnect,networkbusy,dial,pumpbusy,call_waiting_sas2,connectstate,0,0}
	},
	{
		ring0,
		{on_ringing,start_ring_on_timer,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_ring_timeout,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{connectstate,ring1,initial,dcepump,0,0,0,0,0}
	},
	{
		ring1,
		{off_ring,start_ring_off_timer,SetOhtState,start_caller_id_timer,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_ring_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_send_dtmf_cid,0,0,0,0},
		{connectstate,ring0,initial,dcepump,cid_dtmf0,0,0,0,0}
	},
	{
		networkbusy,
		{off_timers,slac_restart,start_disconnect_timer,transient_func,off_echo_cancel},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_unconditional_branch,SLAC_dce_pump,0,0,0,0,0,0},
		{flash_detect,networkbusy1,pumpbusy,0,0,0,0,0,0}
	},
	{
		networkbusy1,
		{start_network_busy_on_timer,on_network_busy_signal,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_network_busy_timeout,SLAC_disconnect_timeout,SLAC_dce_pump,0,0,0,0,0},
		{flash_detect,networkbusy2,disconnect3,pumpbusy,0,0,0,0,0}
	},
	{
		networkbusy2,
		{start_network_busy_off_timer,off_generator_tone,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_network_busy_timeout,SLAC_disconnect_timeout,SLAC_dce_pump,0,0,0,0,0},
		{flash_detect,networkbusy1,disconnect3,pumpbusy,0,0,0,0,0}
	},
	{
		ssconfirm,
		{off_timers,slac_restart,start_ssconfirm_timer,on_confirm_tone,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_ssconfirm_timeout,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{initial,off_hook,networkbusy,pumpbusy,0,0,0,0,0}
	},
	{
		ssfailure,
		{off_timers,slac_restart,start_ssfailure_timer,on_failure_tone,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_ssfailure_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_crssfailure,0,0,0,0},
		{initial,off_hook,networkbusy,pumpbusy,connectstate,0,0,0,0}
	},
	{
		ssnegstatus,
		{off_timers,slac_restart,start_ssneg_status_timer,on_neg_status_tone,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_neg_status_timeout,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{initial,off_hook,networkbusy,pumpbusy,0,0,0,0,0}
	},
	{
		dcepump,
		{off_timers,slac_restart,0,0,0},
		{0,0,0,0,0},
		{SLAC_dce_normal,SLAC_off_hook,0,0,0,0,0,0,0},
		{initial,off_hook,0,0,0,0,0,0,0}
	},
	{
		pumpbusy,
		{off_timers,slac_restart,on_dial_tone,transient_func,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_dce_normal,SLAC_unconditional_branch,0,0,0,0,0,0},
		{initial,off_hook,pumpbusy1,0,0,0,0,0,0}
	},
	{
		pumpbusy1,
		{on_busy_timer,slac_restart,OffCommunication,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy_timeout,SLAC_dce_normal,0,0,0,0,0,0},
		{initial,pumpbusy2,off_hook,0,0,0,0,0,0}
	},
	{
		pumpbusy2,
		{on_busy_timer,on_busy,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_busy_timeout,SLAC_dce_normal,0,0,0,0,0,0},
		{initial,pumpbusy1,off_hook,0,0,0,0,0,0}
	},
	{	nosimbusy,
		{off_timers,off_hook_to_cc,first_digit_timer,transient_func,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_unconditional_branch,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosimbusy1,0,0}
    },
	{
		nosimbusy1,
		{start_no_sim_on_timer,on_no_sim_signal,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_no_sim_timeout,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosimbusy2,0,0}
	},
	{
		nosimbusy2,
		{start_no_sim_off_timer,off_generator_tone,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_no_sim_timeout,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosimbusy1,0,0}
	},
	{	nosbusy,
		{off_timers,off_hook_to_cc,first_digit_timer,transient_func,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_unconditional_branch,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosbusy1,0,0}
    },
	{
		nosbusy1,
		{start_network_busy_on_timer,on_network_busy_signal,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_network_busy_timeout,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosbusy2,0,0}
	},
	{
		nosbusy2,
		{start_network_busy_off_timer,off_generator_tone,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_first_digit_timeout,SLAC_disconnect,SLAC_dce_pump,SLAC_service_available,SLAC_network_busy_timeout,0,0},
		{initial,collectdigits,disconnect,networkbusy,pumpbusy,dial,nosbusy1,0,0}
	},
	{
		pulse_detect,	//  now invalid pulse check for valid flash
		{off_generator_tone,start_pulse_break_timer,reset_pulse_count,0,0},
		{0,0,0,0,0},
		{SLAC_pulse_break_timer,SLAC_off_hook,SLAC_disconnect,0,0,0,0,0,0},
		{flash_detect,pulse_make,initial,0,0,0,0,0,0}
	},
	{
		pulse_make,	//modified
		{off_cadence_timer,start_inter_pulse_timer,increment_pulse_count,set_to_valid,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_inter_pulse_timer,0,0,0,0,0,0,0},
		{pulse_break,collect_pulses,0,0,0,0,0,0,0}
	},
	{   // Added - off_timers,off_generator_tone,
		flash_detect,
		{off_timers,off_generator_tone, start_flash_timer,set_call_flashed,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_flash_timer,0,0,0,0,0,0,0},
		{off_hook/*connectstate*/,valid_flash_detect,0,0,0,0,0,0,0}
	},
	{	// Added
		valid_flash_detect,
		{start_invalid_flash_timer,0,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_invalid_flash_timer,0,0,0,0,0,0,0},
		{flash,initial,0,0,0,0,0,0}
	},
	{   
		flash, // ready for flash deactivation & ss svce improvements
		{off_timer_1,OffCommunication,clear_dialing_parameters,start_ss_cancel_timer,check_call_flashed},
		{0,0,0,(void *)flash_cancel_timout_value,0},
		{SLAC_digit,SLAC_on_hook,SLAC_disconnect,SLAC_ss_cancel_timeout,0,0,0,0,0},
		{ss_service,ss_pulse_detect,disconnect,ssfailure,0,0,0,0,0}
	},
	{
		pulse_break,
		{start_pulse_break_timer,0,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_pulse_break_timer,SLAC_inter_pulse_timer,0,0,0,0,0,0},
		{pulse_make,initial,initial,0,0,0,0,0,0}
	},
	{
		collect_pulses,
		{store_pulse_digit,start_digit_timer,0,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit_timeout,0,0,0,0,0,0,0},
		{pulse_detect,callprogress,0,0,0,0,0,0,0}
	},
	{
		ss_service,  // modified for ss svce improvements
		{ss_service_handler,clear_dialing_parameters,set_ss_prompt_count,start_ss_cancel_timer,0},
		{0,0,(void *)3,(void *)ss_cancel_timeout_value,0},
		{SLAC_on_hook,SLAC_call_active,SLAC_call_held,SLAC_disconnect,SLAC_SS_failure,SLAC_ss_cancel_timeout,0,0,0},
		{ss_pulse_detect,connectstate,ss_prompt1,dial,ssfailure,ssfailure,0,0,0}
	},
	{
		ss_pulse_detect,
		{off_generator_tone,start_pulse_break_timer,reset_pulse_count,Communication,0},
		{0,0,0,0,0},
		{SLAC_pulse_break_timer,SLAC_off_hook,0,0,0,0,0,0,0},
		{ss_flash_detect,ss_pulse_make,0,0,0,0,0,0,0}
	},
	{
		ss_pulse_make,	//modified
		{off_cadence_timer,start_inter_pulse_timer,increment_pulse_count,set_to_valid,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_inter_pulse_timer,0,0,0,0,0,0,0},
		{ss_pulse_break,ss_collect_pulses,0,0,0,0,0,0,0}
	},
	{
		ss_pulse_break,
		{start_pulse_break_timer,0,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_pulse_break_timer,SLAC_inter_pulse_timer,0,0,0,0,0,0},
		{ss_pulse_make,ss_flash_detect,ss_flash_detect,0,0,0,0,0,0}
	},
	{
		ss_collect_pulses,
		{store_pulse_digit,transient_func,0,0,0},
		{0,0,0,0,0},
		{SLAC_unconditional_branch,0,0,0,0,0,0,0,0},
		{ss_service,0,0,0,0,0,0,0,0}
	},
	{	// if ss_service, then, failure
		ss_flash_detect,
		{start_flash_timer,off_generator_tone,set_ss_prompt_count,0,0},
		{0,0,(void *)3,(void *)2,0},
		{SLAC_off_hook,SLAC_flash_timer,SLAC_disconnect,0,0,0,0,0,0},
		{ss_service,valid_ss_flash_detect,disconnect,0,0,0,0,0,0}
	},
	{	// if ss_service, then, failure
		valid_ss_flash_detect,
		{start_invalid_flash_timer,0,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_invalid_flash_timer,0,0,0,0,0,0,0},
		{ss_service,initial,0,0,0,0,0,0}
	},
	{
		ss_prompt0,
		{start_ss_prompt_delay_timer,OffCommunication,clear_dialing_parameters,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_disconnect,SLAC_ss_prompt_timeout,0,0,0,0,0},
		{flash_detect,ss_collectdigits,disconnect,ss_prompt1,0,0,0,0,0}
	},
	{
		ss_prompt1,
		{start_ss_prompt_on_timer,on_ss_prompt_signal,OffCommunication,clear_dialing_parameters,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_disconnect,SLAC_ss_prompt_timeout,0,0,0,0,0},
		{flash_detect,ss_collectdigits,disconnect,ss_prompt2,0,0,0,0,0}
	},
	{
		ss_prompt2,
		{start_ss_prompt_off_timer,off_generator_tone,decrement_ss_prompt_count,0,0},
		{0,0,0,0,0},
		{SLAC_on_hook,SLAC_digit,SLAC_disconnect,SLAC_ss_prompt_timeout,SLAC_ss_prompted,0,0,0,0},
		{flash_detect,ss_collectdigits,disconnect,ss_prompt1,dial,0,0,0,0}
	},
	{
		ss_collectdigits,
		{off_timers,off_generator_tone,start_digit_timer,set_ss_prompt_count,0},
		{0,0,0,(void *)3,(void *)2},
		{SLAC_on_hook,SLAC_digit,SLAC_digit_timeout,SLAC_disconnect,SLAC_dce_pump,0,0,0,0},
		{flash_detect,ss_collectdigits,callprogress,networkbusy,pumpbusy,0,0,0,0}
	},
	{
		cid_messages,
		{on_ringing,short_ring_delay,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_short_ring_timeout,SLAC_ring,0,0,0,0,0,0},
		{valid_hook,cid_send0,ring0,0,0,0,0,0,0}
	},
	{
		cid_send0,
		{off_generator_tone,SetOhtState,start_cid_message_timer,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_send_cid_message,SLAC_ring,0,0,0,0,0},
		{valid_hook,cid_send1,ring0,0,0,0,0,0}
	},
	{
		cid_send1,
		{off_timers,SendCallerIdMsg0,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_init,SLAC_ring,0,0,0,0,0,0},
		{valid_hook,initial,ring0,0,0,0,0,0,0}
	},
	{
		cid_dtmf0,
		{SetUpForDtmf,transient_func,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_unconditional_branch,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{connectstate,cid_dtmf1,initial,dcepump,0,0,0,0,0}
	},
	{
		cid_dtmf1,
		{SetDtmfGenB,dtmf_setup_gen_delay,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_delay_dtmf_gen,SLAC_disconnect,SLAC_dce_pump,SLAC_done_dtmf_cid,0,0,0,0},
		{connectstate,cid_dtmf2,initial,dcepump,cid_dtmf4,0,0,0,0}
	},
	{
		cid_dtmf2,
		{off_slac_timer,SendDtmf,dtmf_cid_delay_message,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_delay_dtmf_cid,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{connectstate,cid_dtmf3,initial,dcepump,0,0,0,0,0}
	},
	{ 
		cid_dtmf3,
		{off_slac_timer,off_generator_tone,dtmf_cid_delay_message,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_delay_dtmf_cid,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{connectstate,cid_dtmf1,initial,dcepump,0,0,0,0,0}
	},
	{
		cid_dtmf4,
		{off_generator_tone,RestoreFromDtmf,0,0,0},
		{0,0,0,0,0},
		{SLAC_off_hook,SLAC_ring_timeout,SLAC_disconnect,SLAC_dce_pump,0,0,0,0,0},
		{connectstate,ring0,initial,dcepump,0,0,0,0,0}
	},
};

/**************************************************
Caller ID Functions
**************************************************/
//#define CALL_WAITING_AVAL
state_service_func SendCallerIdMsg0(void *p)
{
  CID_SendMessage();
  post_cid_message_delay(0);
  if(!waiting_CID)
    bAwake = 0;		// go back to slow clock mode
    
  SetSlicState(STANDBY);
  SetSlicState2(STANDBY);
  return 0;
}
state_service_func SendCallerIdMsg1(void *p)
{
  off_generator_tone(0);
  CID_SendMessage();
  return 0;
}

state_service_func SetUpForDtmf(void *p)
{
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_7_Mask);
  WriteSingleByte(ALL_BITS_MASKED);
  if(WSU003_Config)
    VOICE_DATA_SWITCH=1;
  else
    DataModeSwitch();

  WriteSingleByte(WRITE_ACTIVATE_OP_MODE);
  return 0;
}

state_service_func RestoreFromDtmf(void *p)
{
  if(WSU003_Config)
    VOICE_DATA_SWITCH=0;
  else
    VoiceModeSwitch();
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_7_Mask);
  WriteSingleByte(NDIG_BIT_UNMASKED);
    return 0;
}

state_service_func SetDtmfGenB(void *p)
{
  static int count = 0;
    
//    sprintf(strBuf,"digit: %c[%d] \r\n",DialingParams.DialingParams[count], count);
//    DebugPrintf(strBuf);
    
    switch(DialingParams.DialingParams[count++])
    {
      case '1':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x44);
      break;
    
      case '2':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x45);
      break;
    
      case '3':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x46);
      break;
    
      case '4':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x54);
      break;
    
      case '5':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x55);
      break;
    
      case '6':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x56);
      break;
    
      case '7':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x64);
      break;
    
      case '8':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x65);
      break;
    
      case '9':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x66);
      break;
    
      case '0':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x75);
      break;
    
      case '*':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x74);
      break;
    
      case '#':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x76);
      break;
    
      case 'A':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x47);
      break;
     
      case 'B':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x57);
      break;
    
      case 'C':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x67);
      break;
    
      case 'D':
        WriteSingleByte(WRITE_SIG_GEN_B_DTMF_CONTROL);
        WriteSingleByte(0x77);
      break;
    
      default:
        bDtmfCidDone = TRUE;
        count = 0;
        clear_dialing_parameters(0);
      break;
    } // end switch
  return 0;
}

state_service_func SendDtmf(void *p)
{
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);
  return 0;
}

/**************************************************
**************************************************/

static int	state = initial;
static int previous_state = initial;

#ifdef nDebug
const char debugstring[MAX_STATES][20]={
  "initial","valid_hook","off_hook","dial",
  "disconnect","disconnect1","disconnect2","disconnect3","disconnect4","disconnect5",
  "disconnectslic","collectdigits","callprogress","ringback","ringback1","connectstate",
  "handle_call_waiting","call_waiting_sas0","call_waiting_sas1","call_waiting_cas0",
  "call_waiting_sas2","call_waiting_idle",
  "ring0","ring1","networkbusy","networkbusy1","networkbusy2","ssconfirm","ssfailure","ssnegstatus",
  "dcepump","pumpbusy","pumpbusy1","pumpbusy2",
  "nosbusy","nosbusy1","nosbusy2",
  "nosimbusy","nosimbusy1","nosimbusy2",
  "pulse_detect","pulse_make","flash_detect","valid_flash_detect",
  "flash","pulse_break","collect_pulses",
  "ss_service","ss_pulse_detect","ss_pulse_make","ss_pulse_break","ss_collect_pulses",
  "ss_flash_detect","valid_ss_flsh_dtect",
  "ss_prompt0","ss_prompt1", "ss_prompt2","ss_collectdigits",
  "cid_messages","cid_send0","cid_send1",
  "cid_dtmf0","cid_dtmf1","cid_dtmf2","cid_dtmf3","cid_dtmf4"
}; 
#endif

/**************************************************
SLAC Interrupt Processing Routine
**************************************************/
void ClearInterruptRegs(void)
{
  INTR_DISABLE(SLAC_INTR_FLAG); 
  
  WriteSingleByte(READ_GLOBAL_DEVICE_STATUS_REG);
  ReadSingleByte();                              // Must read the device to clear the register 
  
  WriteSingleByte(READ_UNLOCK_SIG_REG);
  ReadSingleByte();
  ReadSingleByte();
  
  INTR_ENABLE(SLAC_INTR_FLAG);
} 

INTERRUPT_TYPE ProcessSlacInterrupt(void)
{ 
  WORD SigRegData;
  INTERRUPT_TYPE IntType;
  BYTE Glob;
  BYTE Val;
  
  if(0x80 & IntRegData)                     // is Global interrupt
  { // An interruption of Global nature has occurred 
    WriteSingleByte(READ_GLOBAL_DEVICE_STATUS_REG);
    Glob = 0xF0 & ReadSingleByte();                        // Must read the device to clear the register 
    sprintf(strBuf, "Glob interrupt = 0x%4X \r\n", Glob);
    DebugPrintf(strBuf);
    
    if(Glob & 0x10)
    {
      DebugPrintf("CFAIL bit set. \r\n");
      WriteSingleByte(CALIBRATE_ANALOG_CIRCUITS);
      WriteSingleByte(READ_CHNL_CONFIG_REG_2);
      Val = 0x20 & ReadSingleByte();
      do
      {
        WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
        WriteSingleByte(READ_CHNL_CONFIG_REG_2);
        Val = 0x20 & ReadSingleByte();
      }
      while(!Val);
    }
    IntType = GLOBALERROR;
  }
  else if(0x30 & IntRegData)                // is Channel 2 interrupt
  { // An invalid Channel has been detected
    IntType = OTHERERROR;
    DebugPrintf("Chnl 2 interrupt. \r\n");
  }
  else                                      // is valid Channel 1 interrupt
  {
    if(0x01 & IntRegData)                   // is Signaling interrupt
    { // Read and unlock the signaling register
      WriteSingleByte(READ_UNLOCK_SIG_REG);
      
      // then concatentate the data into a single WORD
      SigRegData = ReadSingleByte() << 8;
      SigRegData |= (ReadSingleByte() & 0x00FF);

/*    Debug Stuff
      sprintf(strBuf, "\t Signaling Reg Data = %4X \n\r", SigRegData);
      DebugPrintf(strBuf); */

      if(0x0008 & SigRegData)                // is valid digit
      {
        DialingParams.LastDigitDialed = (SigRegData >> 4) & 0x000F;
        CollectDigits(DialingParams.LastDigitDialed);
        IntType = DIGIT;
      }
      else                                  // is signaling error
      {
        IntType = OTHERERROR;
/*      Debug Stuff        
        sprintf(strBuf, "Sig interrupt = 0x%4X  ", SigRegData);
        DebugPrintf(strBuf);
        WriteSingleByte(READ_GLOBAL_DEVICE_STATUS_REG);		// READ_GLOBAL_DEVICE_STATUS_REG
        Glob = ReadSingleByte();                              // Must read the device to clear the register 
        sprintf(strBuf, "Glob interrupt = 0x%4X \r\n", Glob);
        DebugPrintf(strBuf); */
      }
    }
    else                                    // is hook status
    {
      IntType = OTHERERROR;
/*    Debug Stuff
      sprintf(strBuf, "Not Glob, Ch2, Sig; but SigReg=0x%4X GlobReg=0x%4X \r\n", SigRegData, Glob);
      DebugPrintf(strBuf); */
    }  /* end else {!0x01 & IntRegData)} */
  }  /* end else valid Channel 1 interrupt */
  return IntType;
}

interrupt void SLAC_isr(void)
{
  INTR_DISABLE(SLAC_INTR_FLAG);
  
  SLAC_isr_flag = 0;

  WriteSingleByte(READ_INTERRUPT_REG);
  IntRegData = ReadSingleByte();
  
  if(IntRegData & 0x40)
  {
    IntRegData &= 0x00B9;
    SLAC_isr_flag = 1;
  }
/*  debug stuff
  else  //For dial tone debug 
 	uart_b_fputs("No active Slac interrupt. \r\n");
*/
  INTR_ENABLE(SLAC_INTR_FLAG);
}

void SLAC_isr_1(void)
{ /* POLLING HOOK, INTERRUPTS NOT ACTIVE */
  INTERRUPT_TYPE IntType;
  BYTE HookBit;
  static int ck_interval = 0;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  
  /* first, respond to a valid interrupt */  
  if(SLAC_isr_flag)
  {
    SLAC_isr_flag=0;
  
    IntType = ProcessSlacInterrupt();
    if(DIGIT == IntType)
    {
      //DebugPrintf("\t Signaling interrupt. \n\r");
      if(TRUE == GetRohStatus())
      {  /* if in Receiver off-hook (ROH) state do not process DTMF digits */
        INTR_ENABLE(SLAC_INTR_FLAG);
        return;
      }
      send0(SLAC,SLAC_digit);
    }
    else if((OTHERERROR == IntType) | (GLOBALERROR == IntType))
    { // error: signaling, global, and ?? interrupt
      DebugPrintf("\t error interrupt. \n\r");
      send0(SLAC,SLAC_error);
    }
    else
  	  DebugPrintf("what slac error is this? \r\n");
  }

  /* Otherwise, check for a change in hook status */
  if(0x00F == ck_interval)
  {
    WriteSingleByte(READ_GlOBAL_SUPERVISION);
    HookBit = 0x02 & ReadSingleByte();
    ck_interval = 0;
  
  
/*   Debug Stuff
    sprintf(strBuf, "GlobSupvReg: 0x%X \r\n", HookBit);
    DebugPrintf(strBuf); */
    if((POLLING_OFF_HOOK == HookBit) && (TRUE == bOnHook))
    { //  software debounce stuff
      if(TRUE == bValid)
      {
        WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
        WriteSingleByte(ENABLE_CHNLS_1_AND_2_RW);
        WriteSingleByte(WRITE_ACTIVATE_OP_MODE);	// Activate CODEC - wake-up Chnls 1 & 2
        WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
        WriteSingleByte(ENABLE_CHNL_1_RW);
        if(1 == into_caller_id)
        {
          into_caller_id = 0;
          DeactiveBsp0();
        }
        bValid = FALSE;
        bOnHook = FALSE;
/*      Debug Stuff
        DebugPrintf("\t QUICK: Off-hook interrupt. \n\r"); */
        SetSlicState(AUTO_BATT_SWITCH | ACTIVE);
        update_call_metering_info(0);
        send0(SLAC,SLAC_off_hook);
      } 
    }
    else if((HookBit == POLLING_ON_HOOK) && (FALSE == bOnHook))
    { //  software debounce stuff
      if(TRUE == bValid)
      {
        bOnHook = TRUE;
/*      Debug Stuff
        DebugPrintf("\t QUICK: On-hook interrupt. \n\r"); */

        send0(SLAC,SLAC_on_hook);
        if(chargingEnabled && bCallConnected)
          SupvBlocked = HOOK_BLOCKED;
        else	// valid on-hook || (!bCallConnected)
        {
          bCallConnected = FALSE;
          SetSlicState(STANDBY);
          if(!WSU003_Config)
          {
            SetSlicState2(STANDBY);
          }
        }
      }
    }
  }
  else
  {
    ck_interval++;
  }
  INTR_ENABLE(SLAC_INTR_FLAG);
}

/**************************************************
Codec interface functions
**************************************************
state_service_func mute_codec(void *p)
{
  UpdateCodec(CODEC_MUTE_VALUE);
  return 0;
}*/

/**************************************************
Muting functions
**************************************************/
state_service_func Mute(void *p)
{
  if(WSU003_Config)
  {
    UpdateCodec(CODEC_MUTE_VALUE);
  }
  else
  {
    BYTE DataVal;

    WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    WriteSingleByte(ENABLE_CHNL_1_RW);
  
    WriteSingleByte(READ_CHNL_CONFIG_REG_1);
    DataVal = ReadSingleByte();
  
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_1);
    WriteSingleByte(DataVal | DISABLED_PCM_TRANSMIT);
    DisablePcmReceive();
  }
  return 0;
}

state_service_func UnMuteSlacTx(void *p)
{
  BYTE DataVal;

  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_1_RW);
  
  WriteSingleByte(READ_CHNL_CONFIG_REG_1);
  DataVal = ReadSingleByte();
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_1);
  WriteSingleByte(DataVal & ~DISABLED_PCM_TRANSMIT);
  
  EnablePcmReceive();
  return 0;
}

void EnablePcmReceive(void)
{
  BYTE DataVal;
  
  WriteSingleByte(READ_CHNL_CONFIG_REG_1);
  DataVal = ReadSingleByte();
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_1);
  WriteSingleByte(DataVal & ~DISABLED_PCM_RECEIVE);
}

void DisablePcmReceive(void)
{
  BYTE DataVal;
  
  WriteSingleByte(READ_CHNL_CONFIG_REG_1);
  DataVal = ReadSingleByte();
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_1);
  WriteSingleByte(DataVal | DISABLED_PCM_RECEIVE);
}

/**************************************************
Slic State Functions
**************************************************/
BYTE GetSlicState(void)
{ // Get the current slic state
  BYTE DataVal;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  WriteSingleByte(READ_ISLIC_STATE_REG);
  DataVal = (ReadSingleByte() & 0x003F);
  INTR_ENABLE(SLAC_INTR_FLAG);
  
//  sprintf(strBuf, "\t GetSlicState: State = 0x%X \n\r", DataVal);
//  DebugPrintf(strBuf);
  return DataVal;
}

void SetSlicState(BYTE SlicState)
{ // Set the new slic state 

  INTR_DISABLE(SLAC_INTR_FLAG);
  WriteSingleByte(WRITE_ISLIC_STATE_REG);
  WriteSingleByte(SlicState & 0x00FF);
  INTR_ENABLE(SLAC_INTR_FLAG);
}

state_service_func SetOhtState(void *p)
{ // sets up slic state for call id xmt
  
  if(bfirstRing)
  {
    INTR_DISABLE(SLAC_INTR_FLAG);
    WriteSingleByte(WRITE_ISLIC_STATE_REG);
    WriteSingleByte(HI_BATT_ENABLE | ON_HOOK_TRANSMISSON);
    INTR_ENABLE(SLAC_INTR_FLAG);

//    DebugPrintf("SetOht 1st ring only. \r\n");
  }
  return 0;
}

void SetSlicState2(BYTE SlicState)
{
    WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    WriteSingleByte(ENABLE_CHNL_2_RW);
    SetSlicState(SlicState);

    WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    WriteSingleByte(ENABLE_CHNL_1_RW);
    
    if(ACTIVE == SlicState)
    {  //  Place Chnl 2 into Active state
      EnablePcmReceive();
    }
    if(STANDBY == SlicState)
    {  //  Place Chnl 2 into STANDBY
      DisablePcmReceive();
    }
}

void SetSlicRevPolarity(BYTE reversed)
{
	if(reversed)
		SetSlicState(AUTO_BATT_SWITCH |  POLREV | ACTIVE);
	else
		SetSlicState(AUTO_BATT_SWITCH | (GetSlicState() & ~POLREV) | ACTIVE);
}


void SetSlicLineSupv(BYTE freq, BYTE ramp, BYTE rev)
{
  static BYTE last_state;
  BYTE val = 0;
  
  if(freq == 2)
  {
    SetSlicState(last_state);
  }
  else
  {
    last_state = GetSlicState();
    val = (freq<<6) | (ramp<<5) | (rev<<4);
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_5);
    WriteSingleByte(val);
    SetSlicState(GetSlicState() | TELETAX);
//    SetSlicState(HI_BATT_ENABLE | TELETAX);
  }
}

unsigned short ascii2hex(char c)
{
	if(c >= '0' && c <= '9')
		return (c - '0');
	if(c >= 'A' && c <= 'F')
		return (0xA + c - 'A');
	if(c >= 'a' && c <= 'f')
		return (0xA + c - 'a');
	return 0xFF;
}

int LoadLineSupervParams(void)
{
  unsigned short type, duration, val;
  int i, j;
  int x = 0;
  int retval = 0;
  
  for(i = 0; i < MAX_RECORDS; i++)
  {
    if(0 == strcmp("CONNSUPV", FlashRecords[i].NameParam)
    && (0 != strlen(FlashRecords[i].DataParam)))
    	actions = &supv_events.conn_supv[0];
    else if(0 == strcmp("DISCONSUPV", FlashRecords[i].NameParam)
    && (0 != strlen(FlashRecords[i].DataParam)))
    	actions = &supv_events.disconn_supv[0];
    else if(0 == strcmp("AOCSUPV", FlashRecords[i].NameParam)
    && (0 != strlen(FlashRecords[i].DataParam)))
    	actions = &supv_events.aoc_supv[0];
    else
    	continue;
    	
    retval = 1;
    x = 0;
    for(j = 0; j < (sizeof(FlashRecords[i].DataParam) - 1); j+=4)
    {
      	/* construct 16-bit value out of 4 ascii hex digits */
      	val = (ascii2hex(FlashRecords[i].DataParam[j] ) << 12 |
      		  ascii2hex(FlashRecords[i].DataParam[j+1]) << 8 |
      		  ascii2hex(FlashRecords[i].DataParam[j+2]) << 4 |
      		  ascii2hex(FlashRecords[i].DataParam[j+3]) << 0);
      
      	type = (val >> 10) & 0x3F;		/* 6 bits */
      	duration = val & 0x3FF;			/* 10 bits */
		actions[x].type = type;
		actions[x].duration = duration;
		x++;
		if(x > MAX_ACTIONS)
		{
			retval = 0;
        	break;
        }
    }
    
    /* on failure, stop processing records */
    if(retval == 0)
    	break;
  }
  return retval;  
}

void LineSupervision(int command_type, int cntr_val)
{
  static SUPV_ACTION *current_supv_actions;
  static int ndx, val;
  int i, begin;
  BYTE freq, ramp, rev;
  int act_type, act_subtype;
  
  if(!chargingEnabled)
    return;
    
  begin = 0;
  switch(command_type)
  {
    case SLAC_supv_conn:
      SupvBlocked = BLOCKED;
      off_timer(SLAC_TIMER_SUPV);
      actions = &supv_events.conn_supv[0];
    break;
    
    case SLAC_supv_disconn:
      if(!bCallConnected)
        return;
      bCallConnected = FALSE;
      off_timer(SLAC_TIMER_SUPV);
      actions = &supv_events.disconn_supv[0];
    break;
    
    case SLAC_supv_aoc:
      if(HOOK_BLOCKED == SupvBlocked)
        return;
      else if(BLOCKED == SupvBlocked)
      {
        send0(SLAC, SLAC_supv_aoc);
        return;
      }
      val = cntr_val;
      off_timer(SLAC_TIMER_SUPV);
      actions = &supv_events.aoc_supv[0];
    break;
    
    case SLAC_supv_continue:
      actions = current_supv_actions;
      begin = ndx + 1;
    break;
  }
    
  for(i = begin; i < MAX_ACTIONS; i++)
  {
    act_type = (actions[i].type & 0x30) >> 4;
    act_subtype = actions[i].type & 0xF;

    if((act_type) == NO_ACTION)
    {
      if(val && --val)
      {
        LineSupervision(SLAC_supv_aoc, val);
      }
      else
      {
        ndx = 0;
        if(SupvBlocked == HOOK_BLOCKED)
        {
          if(bOnHook)
          {
            bCallConnected = FALSE;
            SetSlicState(STANDBY);
            if(!WSU003_Config)
            {
              SetSlicState2(STANDBY);
            }
          }
        }
        SupvBlocked = NOT_BLOCKED;	
      }
      break;
    }
    else if(act_type == SUPV_DELAY)
    {
      ndx = i;
      current_supv_actions = actions;
      on_supv_delay_timer((void*)actions[i].duration);
      break;
    }
    else if(act_type == SUPV_LINE)
    {
      /* perform the requested action */
	  if(act_subtype & SUPV_SUBTYPE_REV_MASK)
	  	rev = 1;
	  else
	    rev = 0;

      if(act_subtype & SUPV_SUBTYPE_RAMP_MASK)
      	ramp = 1;
      else
      	ramp = 0;
      	
      if(act_subtype & SUPV_SUBTYPE_12KHz_MASK)
      	freq = 0;
      else if(act_subtype & SUPV_SUBTYPE_16KHz_MASK)
      	freq = 1;
      else if(!rev)	// return to normal (original) condition
        freq = 2;
      else			// teletax polarity reversal pulses req'd
        freq = 0;

	  SetSlicLineSupv(freq, ramp, rev);
	  if(actions[i].duration != 0)
      {
      	ndx = i;
      	current_supv_actions = actions;
      	on_supv_delay_timer((void*)actions[i].duration);
      	break;
      }
    }
    else if(act_type == SUPV_HARD_REV)
    {
	  if(act_subtype & SUPV_SUBTYPE_REV_MASK)
	  	rev = 1;
	  else
	    rev = 0;
	    
	  SetSlicRevPolarity(rev);  
	  if(actions[i].duration != 0)
      {
      	ndx = i;
      	current_supv_actions = actions;
      	on_supv_delay_timer((void*)actions[i].duration);
      	break;
      }
    }
  }
}
/**************************************************
Communication state functions
**************************************************/
state_service_func Communication(void *p)
{
  BYTE DataVal;
  
  /* so we can return on-hook, a bizzare occurance, but can happen */
  set_to_valid(0);
  
  if(fax_mode != COMMAND)
  {
  	if(!WSU003_Config)
    {
      UnMuteSlacTx(0);
    }
  	return; //we need to be connected to modems in data transmitting mode
  }
  
  if(WSU003_Config)
  {
    /* increase codec input/output gains to nominal values */
    UpdateCodec(CODEC_DEMUTE_VALUE);
  
    VOICE_DATA_SWITCH = 0;   // Set to voice,  Fix for the CID.
  }
  else
  {
    VoiceModeSwitch();
    UnMuteSlacTx(0);
  }
  
  //SetSlicState(AUTO_BATT_SWITCH | ACTIVE);
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  /* 1st, get the current value of the register */
  WriteSingleByte(READ_CHNL_CONFIG_REG_3);
  DataVal = (ReadSingleByte() & 0x00FF);
  
  /* Set to continuous echo adaptation while in communication
     by setting the appropriate bit */
  if(bTwoWayCall)
  {
  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_3);
  	WriteSingleByte((DataVal | B_FILTER_ADAPTIVE_MODE_ON) & 0x00FF);
  }
  
  INTR_ENABLE(SLAC_INTR_FLAG);
	
  if(!WSU003_Config)
  {
    SetSlicState2(ACTIVE);
  }
  
  bTwoWayCall = TRUE;

  return 0;
}

state_service_func OffCommunication(void *p)
{
  if(WSU003_Config)
    VOICE_DATA_SWITCH=0;   // Set to voice,  Fix for the CID.
  else
    VoiceModeSwitch();
  
  /* so we can return on-hook, a bizzare occurance, but can happen */
  set_to_valid(0);
  
  off_echo_cancel(0);
  
  if(WSU003_Config)
    UpdateCodec(CODEC_MUTE_VALUE);
  else  
    Mute(0);					
					
  SetSlicState(AUTO_BATT_SWITCH | ACTIVE);

  return 0;
}

/**************************************************
Adaptive Echo Cancellation Functions
**************************************************/
state_service_func on_echo_cancel(void *p)
{
  BYTE DataVal;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  
  WriteSingleByte(READ_CHNL_CONFIG_REG_3);
  DataVal = (ReadSingleByte() & 0x00FF);
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_3);
  WriteSingleByte((DataVal | B_FILTER_ADAPTIVE_MODE_ON) & 0x00FF);
  
  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

state_service_func off_echo_cancel(void *p)
{
  BYTE DataVal;
  
  INTR_DISABLE(SLAC_INTR_FLAG);

  /* 1st, get the current value of the register */
  WriteSingleByte(READ_CHNL_CONFIG_REG_3);
  DataVal = (ReadSingleByte() & 0x00FF);
  
  /* Then, turn off echo adaptation by reseting the appropriate bit */
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_3);
  WriteSingleByte((DataVal & B_FILTER_ADAPTIVE_MODE_OFF) & 0x00FF);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
Slac Hook functions
**************************************************/
state_service_func set_to_valid(void *p)
{
  bValid = TRUE;
  return 0;
}

state_service_func on_hook_to_cc(void *p)
{
  extern int fr_task_on;
  extern int fr_direction;
  
  bcall_flashed = FALSE;
  
  send0(CC,SLAC_on_hook);
  if(fr_task_on)
  {
  	if(fr_direction == FR_outgoing)
  		send0(FR_IN, FR_ERROR_IN);
  	else
  		send0(FR_OUT, FR_ERROR_OUT);
  }
  return 0;
}

state_service_func off_hook_to_cc(void *p)
{
  send0(CC,SLAC_off_hook);
  return 0;
}

state_service_func connect_to_cc(void *p)
{
  bCallConnected = TRUE;
  send0(CC,SLAC_connect);
  return 0;
}

BOOLEAN OnHookStatus(void)
{
 return bOnHook;
}
/**************************************************
Slac Restart function
**************************************************/
state_service_func slac_restart(void *p)
{
  bfirstRing = TRUE;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
   
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_A_B_OFF);
  
  INTR_ENABLE(SLAC_INTR_FLAG);
  
  off_echo_cancel(p);
  
//  if(WSU003_Config)
    //UpdateCodec(CODEC_MUTE_VALUE); 
//  else
  //Mute(0);

  if(RINGING == GetSlicState())
    SetSlicState(STANDBY);
  
  return 0;
}


/**************************************************
Ring functions
**************************************************/
void setup_ring(void)
{ // setup the ring
  SetUpSigGenA(SLAC_ring);
}

state_service_func on_ringing(void *p)
{ // turn on the ring
  setup_ring();
  
  INTR_DISABLE(SLAC_INTR_FLAG); 
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_A_ON);
  
  INTR_ENABLE(SLAC_INTR_FLAG);
  
  SetSlicState(RINGING); 
//  DebugPrintf("on_ringing\r\n");
  return 0;
}

state_service_func off_ring(void *p)
{ // turn off the ring
  INTR_DISABLE(SLAC_INTR_FLAG); 

  SetSlicState(STANDBY);
  SetSlicState2(STANDBY);
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_A_B_OFF);
  
  INTR_ENABLE(SLAC_INTR_FLAG); 
  // DebugPrintf("off_ringing\r\n");

  return 0;
}
/**************************************************
Dial Tone functions
**************************************************/
void setup_dial_tone(void)
{ // setup the dial tone 

  SetUpSigGenB(DIAL_TONE);
  delay(500);  ///against corrupted tones.
}

state_service_func on_dial_tone(void *p)
{
  setup_dial_tone();
  
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  
  memset(DialingParams.DialingParams, 0,MAX_DIGIT_COUNT);
  bFirstDigit = TRUE; // make it true
  return 0;
}

/**************************************************
Ringback Tone functions
**************************************************/
void set_up_ring_back(void)
{ //setup ringback tone
  SetUpSigGenB(SLAC_ring_back);
} 

state_service_func on_ring_back(void *p)
{ // turn on ringback tone
  set_up_ring_back();
  
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0; 
}

/**************************************************
Busy Tone functions
**************************************************/
void set_up_busy(void)
{ // setup busy tone
  SetUpSigGenB(SLAC_busy);
}

state_service_func on_busy(void *p)
{ // turn on busy tone
  set_up_busy();
  
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
Network Busy Tone functions
**************************************************/
void setup_network_busy_signal(void)
{
  SetUpSigGenB(SLAC_all_trunks_busy);
}

state_service_func on_network_busy_signal(void *p)
{
  setup_network_busy_signal();
    
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
Ringer Off-Hook (ROH) Tone functions
**************************************************/
void setup_roh_signal(void)
{ // setup the ROH tone
  SetUpSigGenA(RECEIVER_OFF_HOOK);
  SetUpSigGenB(RECEIVER_OFF_HOOK);
}

state_service_func on_roh(void *p)
{ // turn on the ROH tone
  setup_roh_signal();
  
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON | GENERATOR_A_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
Turn off all tone types both Gen A and Gen B
**************************************************/
state_service_func off_generator_tone(void *p)
{
  INTR_DISABLE(SLAC_INTR_FLAG); 
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_A_B_OFF);
  
  // DebugPrintf("off_tones\r\n");
  INTR_ENABLE(SLAC_INTR_FLAG); 
  return 0;
}

/**************************************************
SS Prompt Tone functions
**************************************************/
void setup_ss_prompt_signal(void)
{
  SetUpSigGenB(SS_CONFIRMATION_TONE);
}

state_service_func on_ss_prompt_signal(void *p)
{
  setup_ss_prompt_signal();
    
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

static int ss_prompt_count;

state_service_func set_ss_prompt_count(void *p)
{
	ss_prompt_count = (int)p;
	if(ss_prompt_count == 0)
		send0(SLAC, SLAC_ss_prompted);	
	return 0;
}

state_service_func decrement_ss_prompt_count(void *p)
{
	if(ss_prompt_count > 0)
		ss_prompt_count--;

	if(ss_prompt_count == 0)
		send0(SLAC, SLAC_ss_prompted);
		
	return 0;
}

/**************************************************
CRSS Failed Tone functions
**************************************************/

/**************************************************
SS Disconnect Tone functions
**************************************************/

/**************************************************
Confirmation/Failure Tone functions
**************************************************/
void SetUpConfirmTone(void)
{
  SetUpSigGenB(SS_CONFIRMATION_TONE);
}

state_service_func on_confirm_tone(void *p)
{
  SetUpSigGenB(SS_CONFIRMATION_TONE);
  
  INTR_DISABLE(SLAC_INTR_FLAG);    

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);
  
  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

int failure_tone_state = 0;
state_service_func on_failure_tone(void *p)
{
	static int fth = -1;
	
	if(failure_tone_state == 0)
	{
		if(fth == -1)
		{
    		if(!timer_add(&fth, 0,// Not one_shot!!
	   			(timer_service_func)on_failure_tone,(void*)0,0))
			{
				fth = -1;
				DebugPrintf("ssf_e1\r\n");
				return (state_service_func)0;
			}
			DebugPrintf("ssf+\r\n");
		}
		
		
		if(fth != -1)
		{
			if(!timer_enable(fth,(unsigned int)25))
			{
  				DebugPrintf("ssf_e2\r\n");
  				return (state_service_func)0;
  			}
  	    	DebugPrintf("ssf*\r\n");
  	    }
		
  	}	
	else
	if(failure_tone_state == 3)
	{
		if(fth != -1)
			timer_disable(fth);

		failure_tone_state = 0;
		
	  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
	  	WriteSingleByte(GENERATOR_A_B_OFF  );
		
		DebugPrintf("ssf-\r\n");
		
		return (state_service_func)1;
	}
	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
	WriteSingleByte(GENERATOR_A_B_OFF  );
    
    delay(10);
  	SetUpSigGenB(NO_SUCH_NUMBER_TONE_1 + failure_tone_state);
  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  	WriteSingleByte(GENERATOR_B_ON);
	
	failure_tone_state++;

	DebugPrintf1("ssft=",(int)(NO_SUCH_NUMBER_TONE_1 + failure_tone_state));
	
	return (state_service_func)1;
}

int neg_status_tone_state = 0;
state_service_func on_neg_status_tone(void *p)
{
	static int fth = -1;
	
	if(neg_status_tone_state == 0)
	{
		if(fth == -1)
		{
    		if(!timer_add(&fth, 0,// Not one_shot!!
	   			(timer_service_func)on_neg_status_tone,(void*)0,0))
			{
				fth = -1;
				DebugPrintf("nsf_e1\r\n");
				return (state_service_func)0;
			}
			DebugPrintf("nsf+\r\n");
		}
		
		
		if(fth != -1)
		{
			if(!timer_enable(fth,(unsigned int)25))
			{
  				DebugPrintf("nsf_e2\r\n");
  				return (state_service_func)0;
  			}
  	    	DebugPrintf("nsf*\r\n");
  	    }
		
  	}	
	else if(neg_status_tone_state == 2)
	{
		if(fth != -1)
			timer_disable(fth);

		neg_status_tone_state = 0;
		
	  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
	  	WriteSingleByte(GENERATOR_A_B_OFF);  
		
		DebugPrintf("ssf-\r\n");
		
		return (state_service_func)1;
	}

  	
  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  	WriteSingleByte(GENERATOR_A_B_OFF);  
    
    delay(10);
  	SetUpSigGenB(NO_SUCH_NUMBER_TONE_3 - neg_status_tone_state);
  	WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  	WriteSingleByte(GENERATOR_B_ON);
	
	neg_status_tone_state++;

	DebugPrintf1("nsft=",(int)(NO_SUCH_NUMBER_TONE_3 - neg_status_tone_state));
	
	return (state_service_func)1;
}

/**************************************************
NO Sim Tone functions
**************************************************/
void setup_no_sim_signal(void)
{
  SetUpSigGenB(SLAC_no_sim);
}

state_service_func on_no_sim_signal(void *p)
{
  setup_no_sim_signal();
    
  INTR_DISABLE(SLAC_INTR_FLAG);

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);

  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
Call Waiting Tone & interface functions
**************************************************/
state_service_func CallWaitSetUp(void *p)
{
  Mute(0);
  SetSlicState(HI_BATT_ENABLE | ACTIVE);
  return 0;
}

state_service_func On_CallWaitingTone(void *p)
{

  SetUpSigGenB(CALL_WAITING_TONE);
  INTR_DISABLE(SLAC_INTR_FLAG);
    
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
  WriteSingleByte(GENERATOR_B_ON);
    
  INTR_ENABLE(SLAC_INTR_FLAG);

  return 0;
}

state_service_func on_CasTone(void *p)
{
  SetUpSigGenB(CPE_ALERTING_SIGNAL);
  
  INTR_DISABLE(SLAC_INTR_FLAG);
    
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_2);
    WriteSingleByte(GENERATOR_B_ON);
  
  INTR_ENABLE(SLAC_INTR_FLAG);
  return 0;
}

/**************************************************
DisconnectSlic
**************************************************/
state_service_func DisconnectSlic(void *p)
{ 
  SetSlicState(ACTIVE);				
  return 0;
}

/**************************************************
Call waiting message passed functions
**************************************************/
state_service_func ss_service_handler(void *p)
{
  int digit;
  
  off_slac_timer(0);
  digit = DialingParams.DialingParams[0];
  if(CC_current_state == 5 || CC_current_state == 7 || held_call_exists())
  {
	  switch(digit)
	  {
	  	case 1:
	 		send0(AT, AT_call_accept_waiting);
	  		break;
	  	case 2:
	  		 send0(AT, AT_call_hold);
	  		break;
	  	case 3:
	  		if(num_current_calls > 1 && num_mpty_calls != num_current_calls)
	  		{
	  			send0(AT, AT_call_mpty);
	  			bTwoWayCall = FALSE;
	  		}
	  		else
	  			start_failure_timer(0);
	  		break;
	  	case 4:
	  	  	if(num_current_calls > 1)
	  			send0(AT, AT_call_ect);
	  		else
				start_failure_timer(0); 
	  		break;
	  	default:
				start_failure_timer(0);
	    	break;
	   }
   }
   else
   	start_failure_timer(0);
 
  return 0;
}

/**************************************************
CollectDigits
**************************************************/
state_service_func clear_dialing_parameters(void *p)
{ 
  memset(DialingParams.DialingParams,0,MAX_DIGIT_COUNT);
  DialingParams.ndx = 0;
  return 0;
}

void CollectDigits(BYTE digit)
{
#ifdef SUPV_DEBUG
  int i;
  static int val = 0;
#endif
  if(TRUE == bFirstDigit)
  {  // stop INTER_DIGIT_TIMEOUT (from the dtmf interrupt detection)
     DialingParams.ndx = 0;
     bFirstDigit = FALSE;
  }
  if(MAX_DIGIT_COUNT > DialingParams.ndx)
  {
    DialingParams.DialingParams[DialingParams.ndx++] = digit;
  }
  if('D' == dialing_symbol(digit))
  { // cid on cw ack to SPCS
    send0(SLAC, SLAC_call_waiting_ack);
  }
#ifdef SUPV_DEBUG
  if('#' == dialing_symbol(digit))
  {
    sprintf(strBuf,"+CCCM: \"%X\"\r\n" ,val+=5);
    for(i=0;i<strlen(strBuf);i++)
      send1(AT, AT_response,0 ,(int*)strBuf[i]);
    DebugPrintf(strBuf);
  }
#endif
  sprintf(strBuf,"digit collected: %c\r\n",dialing_symbol(digit));
  DebugPrintf(strBuf);
  
  //volume control sequences.
  if(dce_state == DCE_STATE_DEBUG && volume_control_monitor(dialing_symbol(digit)))
  {
	DialingParams.ndx=0;
  }
}


void SetVolume(void)
{
	char *flashdata = ReadFlashData("VOLUME");
	int idval;
	char atstr[22]="AT+VGR=006;+VGT=000\n\r";
	
	idval=(int)(flashdata[0])-(int)'0';
	idval=idval*10+((int)(flashdata[1])-(int)'0');
    WriteSlac1RxGain(idval);
    
	idval=(int)(flashdata[2])-(int)'0';
	idval=idval*10+((int)(flashdata[3])-(int)'0');
    WriteSlac2TxGain(idval);

	atstr[7]=flashdata[4];
	atstr[8]=flashdata[5];
	atstr[9]=flashdata[6];
	
	idval=(int)(flashdata[7])-(int)'0';
	idval=idval*10+((int)(flashdata[8])-(int)'0');
    WriteSlac1TxGain(idval);
    
   	idval=(int)(flashdata[9])-(int)'0';
	idval=idval*10+((int)(flashdata[10])-(int)'0');
    WriteSlac2RxGain(idval);

	atstr[16]=flashdata[11];
	atstr[17]=flashdata[12];
	atstr[18]=flashdata[13];

	send_at_command(atstr,0);
}

int SaveVolume(int vindx)
{
	char volstr[8][8]=
	{
		"0500006", //1
		"0800006", //2
		"1200006", //3 default
		"1204006", //4
		"1207006", //5
		"0606000", //mic 1
		"0808000", //mic 2 default
		"0811000", //mic 3
	};
	
	char tmpstr[16];
	char *flashdata = ReadFlashData("VOLUME");
	
	strcpy(tmpstr,flashdata);
	
	if(vindx>=1 && vindx <=5) //speaker volume.
	{
		strncpy(tmpstr,volstr[vindx-1],7);
	}
	else 
	if(vindx>=6 && vindx <=8) //mic. volume.
	{
		strncpy(&tmpstr[7],volstr[vindx-1],7);
	}
	else
	{
		return 0;
	}
	
	if(UpdateFlashData("VOLUME",tmpstr))
	{
		return 0; //could not write to flash.
	}
	
	SetVolume();
	return 1;
}


/**************************************************
Pulse Dialing functions
**************************************************/
state_service_func reset_pulse_count(void *p)
{
	pulse_count = 0;
	return 0;
}

state_service_func increment_pulse_count(void *p)
{
	pulse_count++;
	return 0;
}

state_service_func store_pulse_digit(void *p)
{
  if(pulse_count < 1 || pulse_count > 16)
	return 0;
  
  if(TRUE == bFirstDigit)
  {
    DialingParams.ndx = 0;
    bFirstDigit = FALSE;
  }
  if(MAX_DIGIT_COUNT > DialingParams.ndx)
  {
    DialingParams.DialingParams[DialingParams.ndx++] = pulse_count;
  }
  sprintf(strBuf,"pulse digit collected: %d\r\n",pulse_count);
  DebugPrintf(strBuf);
  
  reset_pulse_count(NULL);
  return 0;
}

/**************************************************
Convert Digits to ASCII symbols
**************************************************/
char dialing_symbol(int value)
{
	char *val_str;
	
	val_str="1234567890*#ABCD";
	return val_str[value?(value-1):15];
}

/**************************************************
send_digits_to_cc
**************************************************/
state_service_func send_digits_to_cc(void *p)
{
  send0(CC,SLAC_dialing_params);
  return 0;
}

/**************************************************
SetUpSigGenA
**************************************************/
void SetUpSigGenA(BYTE ToneType)
{
  DWORD LongHexFreq;
  int HexAmplitude, HexDcBias;
  WORD Frequency;
  double Amplitude;
  double DcBias;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  WriteSingleByte(WRITE_SIG_GEN_A_PARAMS);
  INTR_ENABLE(SLAC_INTR_FLAG);
      
  switch(ToneType)
  {
    case SLAC_ring:
    { 										// 22 Hz Trapezoidal Waveform
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte(TEST_TIME_TRAPEZOID_1);
      WriteSingleByte(TEST_TIME_TRAPEZOID_2);
      WriteSingleByte(SLOPE_TRAPEZOID_1);
      WriteSingleByte(SLOPE_TRAPEZOID_2);
      WriteSingleByte(SLOPE_TRAPEZOID_3);
      WriteSingleByte(SLOPE_TRAPEZOID_4);
      WriteSingleByte(AMPLITUDE_RING_1);			// - 200 V
      WriteSingleByte(AMPLITUDE_RING_2);			// - 200 V
      WriteSingleByte(DC_BIAS_RING_1);
      WriteSingleByte(DC_BIAS_RING_2);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
    
    case RECEIVER_OFF_HOOK:
    {
      Frequency = 2060;
      Amplitude = 0.5;
      DcBias = 0.0;
      LongHexFreq = Hertz2LongHex(Frequency);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      HexDcBias = Decimal2ShortHex(DcBias / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
       WriteSingleByte(TEST_TIME_ONE_SHOT);
      WriteSingleByte(TEST_TIME_ONE_SHOT);
      WriteSingleByte((LongHexFreq >> 24) & 0xFF);
      WriteSingleByte((LongHexFreq >> 16) & 0xFF);
      WriteSingleByte((LongHexFreq >> 8) & 0xFF);
      WriteSingleByte(LongHexFreq & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((HexDcBias >> 8) & 0xFF);
      WriteSingleByte(HexDcBias & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
    
    default:
      break;
  }
}

/**************************************************
SetUpSigGenB
**************************************************/
void SetUpSigGenB(BYTE ToneType)
{
  int HexAmplitude;
  int ShortHexFreq1, ShortHexFreq2;
  WORD Frequency1, Frequency2;
  double Amplitude;	
  
  INTR_DISABLE(SLAC_INTR_FLAG); 
  WriteSingleByte(WRITE_SIG_GEN_B_PARAMS);
  INTR_ENABLE(SLAC_INTR_FLAG);

  switch(ToneType)
  {
    case RECEIVER_OFF_HOOK:
    {
      Frequency1 = 2450;
      Frequency2 = 2600;
      Amplitude = 0.5;
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      ShortHexFreq2 = Hertz2ShortHex(Frequency2);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((ShortHexFreq2 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq2 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);     
    }
    break;
    
    case DIAL_TONE:
    {

      Frequency1 = 350;
      Frequency2 = 440;
      Amplitude = 0.1;
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      ShortHexFreq2 = Hertz2ShortHex(Frequency2);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG);
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((ShortHexFreq2 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq2 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }      
    break;
    
//    case CALL_WAITING_TONE:
    case SLAC_ring_back:
    {
      Frequency1 = 480;
      Frequency2 = 440;
      Amplitude = 0.1;      
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      ShortHexFreq2 = Hertz2ShortHex(Frequency2);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((ShortHexFreq2 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq2 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }      
    break;
    
    case SLAC_busy:
    case SLAC_all_trunks_busy:
    case SLAC_no_sim:
    {
      Frequency1 = 480;
      Frequency2 = 620;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      ShortHexFreq2 = Hertz2ShortHex(Frequency2);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((ShortHexFreq2 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq2  & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
    
    case CPE_ALERTING_SIGNAL:
    {
      Frequency1 = 2130;
      Frequency2 = 2750;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      ShortHexFreq2 = Hertz2ShortHex(Frequency2);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte((ShortHexFreq2 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq2 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
     
    case CALL_WAITING_TONE: 
    {
      Frequency1 = 440;
      Amplitude = 0.1;      
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);

      INTR_DISABLE(SLAC_INTR_FLAG); 

      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);

      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
     
    case SS_CONFIRMATION_TONE: 
    {
      Frequency1 = 1000;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
    
    case NO_SUCH_NUMBER_TONE_1:
    {
      Frequency1 = 950;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
    
       INTR_ENABLE(SLAC_INTR_FLAG);	
    }
    break;
    
    case NO_SUCH_NUMBER_TONE_2:
    {
      Frequency1 = 1400;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;
    
    case NO_SUCH_NUMBER_TONE_3:
    {
      Frequency1 = 1800;
      Amplitude = 0.1; 
      ShortHexFreq1 = Hertz2ShortHex(Frequency1);
      HexAmplitude = Decimal2ShortHex(Amplitude / 2.0);
      
      INTR_DISABLE(SLAC_INTR_FLAG); 
      
      WriteSingleByte((ShortHexFreq1 >> 8) & 0xFF);
      WriteSingleByte(ShortHexFreq1 & 0xFF);
      WriteSingleByte((HexAmplitude >> 8) & 0xFF);
      WriteSingleByte(HexAmplitude & 0xFF);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      WriteSingleByte(0x00);
      
      INTR_ENABLE(SLAC_INTR_FLAG);
    }
    break;

    default:
      break;
  }  
}

/**************************************************
SLAC Utility Functions
**************************************************/
DWORD Hertz2LongHex(WORD Freq)
{
  DWORD HexFreq;
  double Value;
  
  Value = sin((double) PI * (double) Freq / (double) FS);
  HexFreq = (DWORD) (Value * (pow(2.0, 31.0)));
  return HexFreq; 
}

int Decimal2ShortHex(double Decimal)
{
  int IntValue;
  
  IntValue = (int) (Decimal * MAX_INT);
  return IntValue;
}

int Hertz2ShortHex(WORD Freq)
{
  double Value;
  int IntValue;
  
  Value = sin((double) PI * (double) Freq / (double) FS);
  IntValue = Decimal2ShortHex(Value);
  return IntValue; 
}

/**************************************************
SLAC Voice/Data Utility Functions
**************************************************/
void VoiceModeSwitch(void)
{
  VOICE_DATA_SWITCH = 0;
   
  WriteSingleByte(WRITE_TX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_2);
  
  WriteSingleByte(WRITE_RX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_0);
  
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_2_RW);
  
  WriteSingleByte(WRITE_TX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_0);
  
  WriteSingleByte(WRITE_RX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_2);
  
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_1_RW);
  
  if(autobaud_enabled)
  {
  	VOICE_DATA_SWITCH = 0x3;
  }
}

void DataModeSwitch(void)
{
  VOICE_DATA_SWITCH = 1;
  
  WriteSingleByte(WRITE_TX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_0);
  
  WriteSingleByte(WRITE_RX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_0);
  
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_2_RW);
   
  WriteSingleByte(WRITE_TX_TIME_SLOT);
  WriteSingleByte(TIMESLOT_2);
  
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_1_RW);
  
  EnablePcmReceive();
}

void DataCommunication(void)
{
  off_echo_cancel(0);
  SetSlicState2(STANDBY);
  DataModeSwitch();
}

/**************************************************
Low-Level SLAC Driver Functions
**************************************************/
void WriteSingleByte(BYTE byte)
{
  while(!ReadSlacAvail);
  IO_SLAC_ADDR = byte & 0x00FF;
}

BYTE ReadSingleByte(void)
{
  while(!ReadSlacAvail);
  
  return (0x00FF & IO_SLAC_ADDR);
}
/**************************************************
**************************************************/

void InitSlac(void) 
{
  USHORT ndx = 0;
  BYTE RevCode;
  char *memdata;
  
  INTR_DISABLE(SLAC_INTR_FLAG);
  
  WriteSingleByte(WRITE_HARDWARE_RESET);
  
  WriteSingleByte(WRITE_DEV_CONFIG_REG_1);
  WriteSingleByte(MCLK_4096_MHZ);
  
  WriteSingleByte(READ_GLOBAL_DEVICE_STATUS_REG);
  while(0x00 != ReadSingleByte())
  {
    ndx++;
    WriteSingleByte(READ_GLOBAL_DEVICE_STATUS_REG);
  }
  WriteSingleByte(WRITE_CALIBRATE_ANALOG_CKTS);
  delay(110);									// wait >= 10 ms for calib completion
  WriteSingleByte(READ_REVISON_CODE);
  RevCode = ReadSingleByte();
#if 0
  sprintf(strBuf, "(bfor)SLAC rev: 0x%X; GlobDevStat read: %d \r\n", RevCode, ndx);
  uart_b_fputs(strBuf);
#endif
  if(0xC1 != RevCode)
  {
    uart_b_fputs("resetting (S) ... \r\n");
    reset();
  }
  WriteSingleByte(WRITE_DEV_CONFIG_REG_2);
  WriteSingleByte(DEFAULT_ZEROS);
  
  WriteSingleByte(WRITE_TX_RX_CLK_SLOT_SELECT);
  WriteSingleByte(DEFAULT_ZEROS | XE_PCLK_POS);
  
  WriteSingleByte(WRITE_GLBL_DEV_STATUS_MSK_REG);
  WriteSingleByte(LOW_PWR_AND_CLK_FAIL);
  
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_1_RW);
  
  if(WSU003_Config)
  {
    WriteSingleByte(WRITE_TX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_0);
  
    WriteSingleByte(WRITE_RX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_0);
  }
  else
  {
    WriteSingleByte(WRITE_TX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_2);
  
    WriteSingleByte(WRITE_RX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_0);
  } 
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_3);
  WriteSingleByte(LINEAR_PCM);
  
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_4);
  WriteSingleByte(0x08);

#ifdef HOOK_INTERRUPTS_ACTIVE
  WriteSingleByte(WRITE_CHNL_CONFIG_REG_6_Mask);
  WriteSingleByte(/*0x41*/HOOK_BIT_UNMASKED);
#endif

  WriteSingleByte(WRITE_CHNL_CONFIG_REG_7_Mask);
  WriteSingleByte(/*0xF4*/NDIG_BIT_UNMASKED);

  if(WSU003_Config)
  {
    WriteSingleByte(WRITE_RECEIVE_GAIN);
    WriteSingleByte(WSU003_RX_GAIN_BYTE_1);
    WriteSingleByte(WSU003_RX_GAIN_BYTE_2);
  
    WriteSingleByte(WRITE_TRANSMIT_GAIN);
    WriteSingleByte(WSU003_TX_GAIN_BYTE_1);
    WriteSingleByte(WSU003_TX_GAIN_BYTE_2);
  }
  else
  {
    WriteSingleByte(WRITE_RECEIVE_GAIN);
    WriteSingleByte(RX_GAIN_BYTE_1);
    WriteSingleByte(RX_GAIN_BYTE_2);
  
    WriteSingleByte(WRITE_TRANSMIT_GAIN);
    WriteSingleByte(TX_GAIN_BYTE_1);
    WriteSingleByte(TX_GAIN_BYTE_2);
    
    DisablePcmReceive();
  }
  
  WriteSingleByte(WRITE_Z_FLTR_COEFFS);
  WriteSingleByte(0x80);
  WriteSingleByte(0x40);
  WriteSingleByte(0x00);
  WriteSingleByte(0x77);
  WriteSingleByte(0x1C);
  WriteSingleByte(0x23);
  WriteSingleByte(0x0A);
  WriteSingleByte(0x8A);
  WriteSingleByte(0xF5);
  WriteSingleByte(0xEF);
  WriteSingleByte(0xF3);
  WriteSingleByte(0x04);
  WriteSingleByte(0x02);
  WriteSingleByte(0x0A); 
  
  WriteSingleByte(WRITE_R_FLTR_COEFFS);
  WriteSingleByte(0x83);
  WriteSingleByte(0x2C);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x40);
  WriteSingleByte(0x00);
  
  WriteSingleByte(WRITE_X_FLTR_COEFFS);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x00);
  WriteSingleByte(0x40);
  WriteSingleByte(0x00);
  
  WriteSingleByte(WRITE_B_FLTR_COEFFS_7_TAPS);
  WriteSingleByte(0x00);
  WriteSingleByte(0x33);
  WriteSingleByte(0x00);
  WriteSingleByte(0x6F);
  WriteSingleByte(0x00);
  WriteSingleByte(0x0B);
  WriteSingleByte(0xFF);
  WriteSingleByte(0x51);
  WriteSingleByte(0xF8);
  WriteSingleByte(0x9C);
  WriteSingleByte(0xF1);
  WriteSingleByte(0x72);
  WriteSingleByte(0xFA);
  WriteSingleByte(0x90);
  
  WriteSingleByte(WRITE_B_FLTR_COEFFS_6_TAPS);
  WriteSingleByte(0x00);
  WriteSingleByte(0x28);
  WriteSingleByte(0x82);
  WriteSingleByte(0xD1);
  WriteSingleByte(0x00);
  WriteSingleByte(0x31);
  WriteSingleByte(0x00);
  WriteSingleByte(0x2A);
  WriteSingleByte(0x00);
  WriteSingleByte(0x2B);
  WriteSingleByte(0x00);
  WriteSingleByte(0x36);
  WriteSingleByte(0x00);
  WriteSingleByte(0x36);
   
  WriteSingleByte(WRITE_ERROR_LEVEL_THRESHOLD);
  WriteSingleByte(DEFAULT_ZEROS);
  
  WriteSingleByte(WRITE_ECHO_GAIN);
  WriteSingleByte(ECHO_GAIN);
  
  WriteSingleByte(WRITE_ADPATIVE_B_FLTR_CNTRL);
  WriteSingleByte(LST);
  WriteSingleByte(DCR1);
  WriteSingleByte(DCR2);
  WriteSingleByte(DPB);
  
  WriteSingleByte(WRITE_ANALOG_GAIN_DISN);
  WriteSingleByte(DISN_GAIN);
  
  WriteSingleByte(WRITE_LOOP_SUPV_PARMS);
  WriteSingleByte(GROUND_KEY_THRESHOLD);
  WriteSingleByte(GROUND_KEY_INTEGRATION);
  WriteSingleByte(CURRENT_SPIKE_THRESHOLD);
  WriteSingleByte(ZERO_CROSS_THRESHOLD);
  WriteSingleByte(SWITCH_HOOK_TRESHOLD);
  WriteSingleByte(DIAL_PULSE_MAKE_VOLTAGE);
  WriteSingleByte(SWITCH_HOOK_DEBOUNCE);
  WriteSingleByte(STANDBY_LOOP_THRESHOLD);
  
  WriteSingleByte(WRITE_DC_FEED_PARMS);
  WriteSingleByte(BATT_SWITCH_VOLTAGE);
  WriteSingleByte(VAS);
  WriteSingleByte(V1);
  WriteSingleByte(DEFAULT_ZEROS);
  WriteSingleByte(LOOP_CURRENT_LIMIT);
  WriteSingleByte(RESISTANCE_FEED);
  WriteSingleByte(RFD_SAT);
  WriteSingleByte(HI_BATT_FAILURE_THRESHOLD);
  WriteSingleByte(LO_BATT_FAILURE_THRESHOLD);
  WriteSingleByte(POS_BATT_FAILURE_THRESHOLD);
  WriteSingleByte(DC_FAULT_CURRENT_THRESHOLD);
  WriteSingleByte(AC_FAULT_CURRENT_THRESHOLD);
  WriteSingleByte(RTLL);
  WriteSingleByte(RTSL);
  
  WriteSingleByte(WRITE_METER_TARGET_LIM);
  WriteSingleByte(MTR_PEAK_VOLT);
  WriteSingleByte(MTR_TARGET_PK_CURRENT);

  if(!WSU003_Config)
  {
    /* Enable and set up channel 2 */
    WriteSingleByte(DEACTIVATE_CHNNL);
  
    WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
    WriteSingleByte(ENABLE_CHNL_2_RW);
  
    WriteSingleByte(WRITE_TX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_0);
  
    WriteSingleByte(WRITE_RX_TIME_SLOT);
    WriteSingleByte(TIMESLOT_2);
  
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_3);
    WriteSingleByte(LINEAR_PCM);
  
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_6_Mask);
    WriteSingleByte(ALL_BITS_MASKED);
  
    WriteSingleByte(WRITE_CHNL_CONFIG_REG_7_Mask);
    WriteSingleByte(ALL_BITS_MASKED);
  
    WriteSingleByte(WRITE_RECEIVE_GAIN);
    WriteSingleByte(CHNL_2_RX_GAIN_1);
    WriteSingleByte(CHNL_2_RX_GAIN_2);
  
    WriteSingleByte(WRITE_TRANSMIT_GAIN);
    WriteSingleByte(CHNL_2_TX_GAIN_1);
    WriteSingleByte(CHNL_2_TX_GAIN_2);
  }
  
  /* Enable both channels before device activation */
  WriteSingleByte(WRITE_ENABLE_CHANNL_REG);
  WriteSingleByte(ENABLE_CHNL_1_RW);

  SetSlicState(STANDBY);
  
  WriteSingleByte(READ_REVISON_CODE);
  RevCode = ReadSingleByte();
#if 0
  sprintf(strBuf, "(aftr)SLAC rev: 0x%X; GlobDevStat read: %d \r\n", RevCode, ndx);
  uart_b_fputs(strBuf);
#endif

  if(0xC1 != RevCode)
  {
#if 0
    uart_b_fputs("Slac did not init properly, resetting board ...");
#endif
    reset();
  } 
  WriteSingleByte(WRITE_ACTIVATE_OP_MODE);
  
  INTR_ENABLE(SLAC_INTR_FLAG);

  ClearInterruptRegs();
  
  memdata = ReadFlashDataImage("CHARGING");
  if('1' == memdata[0])
    LoadLineSupervParams();
  
  // setup the serial ports
  int_bsps();
  
  if(WSU003_Config)
  {
    ConfigCodec();
    UpdateCodec(CODEC_MUTE_VALUE);
  }
}

/*****************************************************
External state input Support functions
*****************************************************/
state_service_func set_call_flashed(void *p)
{
  switch(previous_state)
  {
    case disconnect:
    case disconnect1:
    case disconnect2:
    case networkbusy:
    case networkbusy1:
    case networkbusy2:
      bcall_flashed = TRUE;
      break;
  }
  return 0;
}

state_service_func check_call_flashed(void *p)
{
  if(bcall_flashed && held_call_exists())
    send0(CC, SLAC_call_flashed);
  bcall_flashed = FALSE;
  
  return 0;
}

state_service_func check_gsm_state(void *p)
{
	
	if(!SIM_present || !PIN_not_required || !PUK_not_required)
	{
		send0(SLAC,SLAC_no_sim);
		return 0;
	}

	if(REG_status != 1 && REG_status != 5)
	{
		send0(SLAC, SLAC_no_service);
		return 0;
	}
	
	if(dce_state == DCE_STATE_PUMP)
	{
		send0(SLAC, SLAC_dce_pump);
		return 0;
	}
	
	if(PolingWavecom == 1)
	{
		send0(SLAC, SLAC_no_service);
		return 0;
	}
		
	switch(CC_current_state)
	{
		case 0:	/* idle */
			break;
		case 1:	/* setup */
			send0(SLAC, SLAC_service_available);
			break;
		case 2:	/* ringing */
			send0(SLAC, SLAC_ring);
			break;
		case 3:	/* answering */
			send0(SLAC, SLAC_connect);
			break;
		case 4:	/* disconnected */
			send0(SLAC, SLAC_disconnect);
			break;
		case 5:	/* connected */
			send0(SLAC, SLAC_connect);
			break;
		case 6:	/* alerting */
			send0(SLAC, SLAC_ring_back);
			break;
		case 7:	/* on hold */
			send0(SLAC, SLAC_connect);
			break;
		default:
			break;
	}
	
	send0(SLAC, SLAC_service_available);
			
	return 0;
}

state_service_func check_waiting_CID(void *p)
{
	if(waiting_CID)
	{
		if(SMS_message[0] != '\0')
			handleSMS();
		else
			handleInfoDisplay();

		CID_OffhookPrintf();
		waiting_CID--;
			
	}
	else if(bRingBackTest)
	{
		bRingBackTest = FALSE;
		CID_Name_Number("","1234567890", "");
		send0_delayed(100,SLAC, SLAC_ring);
	}
	else
	{
		bAwake = 0;
	    ClearCidParams();
	    clearSMSParams();
	    clearINFOparams();
 	}
	return 0;
}

 
state_service_func update_call_metering_info(void *p)
{
  if(state == ring0 || state == ring1 || state == initial)
    ReadAllCallData();
   
   return 0;
}

/*****************************************************
Slac Timer functions
*****************************************************/
timer_service_func timer_func(void *p)
{
	send0(SLAC,(int)p);
	return 0;
}

timer_service_func digitTimeout(void *p)
{
	if(held_call_exists())  
		send0(AT,AT_call_hold);
	send0(SLAC,SLAC_first_digit_timeout);
	return 0;
}

timer_service_func transient_func(void *p)
{
	send0(SLAC,SLAC_unconditional_branch);
	return 0;
}

int init_slac_timers(void)
{
	int i;
	static int tid;
	int	status;
	
	for(i = 0; i < MAX_SLAC_TIMERS; i++)
	{
		if(timer_control_block.timer_id[i] == -1)
		{
			status = timer_add(&tid,1,NULL,NULL,0);
			if(!status)
				// indicate error - TBD
				return 0;
			timer_control_block.timer_id[i] = tid;
		}
	}
	return 1;
}

//timer functions
state_service_func off_timer(int timer_index)
{
	int tid;
	tid = timer_control_block.timer_id[timer_index];
	timer_disable(tid);
	return 0;
}
state_service_func off_timers(void *p)
{
	off_timer(SLAC_TIMER_CADENCE);
	off_timer(SLAC_TIMER_0);
	return 0;
}
state_service_func off_cadence_timer(void *p)
{
	return off_timer(SLAC_TIMER_CADENCE);
}
state_service_func off_slac_timer(void *p)
{
	return off_timer(SLAC_TIMER_0);
}
state_service_func off_timer_1(void *p)
{
	return off_timer(SLAC_TIMER_1);
}
state_service_func on_timer(mbx_typ message, int timeout_value,int timer_index)
{
	int tid;
	tid = timer_control_block.timer_id[timer_index];
	timer_disable(tid);
	timer_set_callback(tid, (timer_service_func)timer_func, (void *)message);
	timer_enable(tid, timeout_value);
	return 0;
}

state_service_func on_timer_functionCall(timer_service_func (*timerFunction)(void *), int timeout_value,int timer_index)
{
	int tid;
	tid = timer_control_block.timer_id[timer_index];
	timer_disable(tid);
	timer_set_callback(tid, (timer_service_func)timerFunction, (void *)0);
	timer_enable(tid, timeout_value);
	return 0;
}

state_service_func on_supv_delay_timer(void *p)
{
	int time_out_value = (int)p;
	return on_timer_functionCall(supv_timer_function,time_out_value,SLAC_TIMER_SUPV);
}

state_service_func supv_timer_function(void *p)
{
	send0(SLAC, SLAC_supv_continue);
	return 0;
}

state_service_func on_digit_timer(void *p)
{
	return on_timer(SLAC_digit_timeout,digit_timeout_value,SLAC_TIMER_0);
}

state_service_func first_digit_timer(void *p)
{
	return on_timer_functionCall(digitTimeout,first_digit_timeout_value,SLAC_TIMER_0);
}

state_service_func on_disconnect_timer(void *p)
{
	return on_timer(SLAC_disconnect_timeout,disconnect_timeout_value,SLAC_TIMER_0);
}

state_service_func on_busy_timer(void *p)
{
	return on_timer(SLAC_busy_timeout,busy_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_slac_off_timer(void *p)
{
	return on_timer(SLAC_slic_off_timeout,slac_off_timeout_value,SLAC_TIMER_0);
}

state_service_func start_disconnect_timer(void *p)
{
	return on_timer(SLAC_disconnect_timeout,disconnect_timeout_value,SLAC_TIMER_0);
}

state_service_func start_roh_timer(void *p)
{
	return on_timer(SLAC_roh_timeout,roh_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_digit_timer(void *p)
{
	return on_timer(SLAC_digit_timeout,digit_timeout_value,SLAC_TIMER_0);
}

state_service_func start_ring_back_on_timer(void *p)
{
	return on_timer(SLAC_ring_back_on_timeout,ring_back_on_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ring_back_off_timer(void *p)
{
	return on_timer(SLAC_ring_back_off_timeout,ring_back_off_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ring_on_timer(void *p)
{
	return on_timer(SLAC_ring_timeout,ring_on_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ring_off_timer(void *p)
{
	return on_timer(SLAC_ring_timeout,ring_off_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_network_busy_on_timer(void *p)
{
	return on_timer(SLAC_network_busy_timeout,network_busy_on_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_network_busy_off_timer(void *p)
{
	return on_timer(SLAC_network_busy_timeout,network_busy_off_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ss_prompt_on_timer(void *p)
{
	return on_timer(SLAC_ss_prompt_timeout,ss_prompt_on_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ss_prompt_off_timer(void *p)
{
	return on_timer(SLAC_ss_prompt_timeout,ss_prompt_off_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func start_ss_prompt_delay_timer(void *p)
{
	return on_timer(SLAC_ss_prompt_timeout,ss_prompt_delay_timeout_value,SLAC_TIMER_CADENCE);
}
state_service_func start_no_sim_on_timer(void *p)
{
	return on_timer(SLAC_no_sim_timeout,no_sim_on_timeout_value,SLAC_TIMER_CADENCE);
}
state_service_func start_no_sim_off_timer(void *p)
{
	return on_timer(SLAC_no_sim_timeout,no_sim_off_timeout_value,SLAC_TIMER_CADENCE);
}


state_service_func start_ssconfirm_timer(void *p)
{
	return on_timer(SLAC_ssconfirm_timeout,ssconfirm_timeout_value,SLAC_TIMER_0);
}
state_service_func start_ssfailure_timer(void *p)
{
	mbx_typ msgVal;
	
	switch(previous_state)
	{
	  case callprogress:
	  case ringback:
	  case ringback1:
        msgVal = SLAC_ssfailure_timeout;
        break;
        
      case flash:
      case ss_service:
      case ss_flash_detect:
      case valid_ss_flash_detect:
        if(held_call_exists())
          msgVal = SLAC_crssfailure;
        else
          msgVal = SLAC_ssfailure_timeout;
        break;
	}
	return on_timer(msgVal,ssfailure_timeout_value,SLAC_TIMER_0);
}
state_service_func start_ssneg_status_timer(void *p)
{
	return on_timer(SLAC_neg_status_timeout,ssneg_status_timeout_value,SLAC_TIMER_0);
}

state_service_func start_pulse_break_timer(void *p)
{
	if(held_call_exists())
		return on_timer(SLAC_pulse_break_timer,pulse_break_timeout_value,SLAC_TIMER_CADENCE);
	else
		return on_timer(SLAC_disconnect,pulse_break_timeout_value,SLAC_TIMER_CADENCE);
}
/*
state_service_func start_pulse_break_timer(void *p)
{
	return on_timer(SLAC_pulse_break_timer,pulse_break_timeout_value,SLAC_TIMER_CADENCE);
}
*/
state_service_func start_inter_pulse_timer(void *p)
{
	return on_timer(SLAC_inter_pulse_timer,inter_pulse_timeout_value,SLAC_TIMER_0);
}

state_service_func start_flash_timer(void *p)
{
	return on_timer(SLAC_flash_timer,flash_timeout_value,SLAC_TIMER_0);
}

state_service_func start_invalid_flash_timer(void *p)
{
	return on_timer(SLAC_invalid_flash_timer,invalid_flash_timeout_value,SLAC_TIMER_1);
}

state_service_func start_ss_cancel_timer(void *p)
{
	int val;
	
	val = (int)p;
	if(FALSE)  /* Enable this in order to disable to FLASH detection */
	{
		off_timers(0);
		send0(SLAC, SLAC_connect);
		return 0;
	}
	return on_timer(SLAC_ss_cancel_timeout,val,SLAC_TIMER_0);
}

state_service_func start_failure_timer(void *p)
{
	return on_timer(SLAC_SS_failure,ss_failure_timeout_value,SLAC_TIMER_CADENCE);
}

state_service_func on_call_waiting_sas_delay(void *p)
{
	return on_timer(SLAC_call_waiting_sas_on,cw_sas_delay_on_timeout_value,SLAC_TIMER_0);
}

state_service_func off_call_waiting_sas(void *p)
{
	return on_timer(SLAC_call_waiting_sas_off,cw_sas_off_timeout_value,SLAC_TIMER_0);
}   

state_service_func ten_sec_call_waiting_sas(void *p)
{
	return on_timer(SLAC_ten_sec_cw_sas_on,ten_sec_cw_sas_on_timeout_value,SLAC_TIMER_1);
}

state_service_func on_cas_delay(void *p)
{
	return on_timer(SLAC_cas_on,cas_delay_on_timeout_value,SLAC_TIMER_0);
}

state_service_func off_cas(void *p)
{
	return on_timer(SLAC_cas_off,cas_off_timeout_value,SLAC_TIMER_0);
}

state_service_func on_cidcw_delay(void *p)
{
	return on_timer(SLAC_cidcw_transmit,cidcw_transmit_timeout_value,SLAC_TIMER_0);
}
  
state_service_func start_caller_id_timer(void *p)
{
	int delay_value;
	
	if(bfirstRing)
	{
		if('2' == GetCidModulation())	// ETSI_DTMF
			delay_value = dtmf_caller_id_delay;
		else							// BELL_202 or ETSI_V23 FSK
			delay_value = caller_id_delay;
	}
	return on_timer(SLAC_caller_id,delay_value,SLAC_TIMER_0);
}

state_service_func mute_codec_delay(void *p)
{
	int val;
	
	val = (int)p;

	return on_timer(SLAC_call_wait_unmute,val,SLAC_TIMER_CADENCE);
}

state_service_func start_ignore_slac_interrupt(void *p)
{
	return on_timer(SLAC_disable_interrupt_timeout,disable_interrupt_timeout,SLAC_TIMER_1);
}

state_service_func short_ring_delay(void *p)
{
  return on_timer(SLAC_short_ring_timeout,short_ring_timeout_value,SLAC_TIMER_0);
}

state_service_func start_cid_message_timer(void *p)
{
	return on_timer(SLAC_send_cid_message,cid_message_delay,SLAC_TIMER_0);
}

state_service_func post_cid_message_delay(void *p)
{
	return on_timer(SLAC_init,post_cid_message_timeout,SLAC_TIMER_0);
}

state_service_func dtmf_cid_delay_message(void *p)
{
	return on_timer(SLAC_delay_dtmf_cid,dtmf_inter_message_delay,SLAC_TIMER_0);
}

state_service_func dtmf_setup_gen_delay(void *p)
{
	if(bDtmfCidDone)
	{
		bDtmfCidDone = FALSE;
		return on_timer(SLAC_done_dtmf_cid,dtmf_gen_delay,SLAC_TIMER_0);
	}
	
	return on_timer(SLAC_delay_dtmf_gen,dtmf_gen_delay,SLAC_TIMER_0);
}

/*******************************************************
Slac State Machine
*******************************************************/
int slac_state_machine(int event)
{
   	int index,trnind,j;
   	//go to the current state context
   	//find the table entry.
   	for(index=0;index<MAX_STATES && state_table[index].state != state;index++);
   	if(index>=MAX_STATES)
		return -1; //no entry in the table.
   
	//find state transition.
	for(trnind=0;trnind<MAX_TRANSITIONS;trnind++)
		if(state_table[index].transition[trnind] == event)
		/*
		if(state_table[index].transition[trnind] == event || 
		   state_table[index].transition[trnind] == SLAC_unconditional_branch)
		*/
		   break;
	if(trnind>=MAX_TRANSITIONS)
		return 0; //no transitions.

    //process state transition.
    previous_state = state;
	state = state_table[index].next_state[trnind];
	
#ifdef nDebug
	strcpy(strBuf,"ssm state: ");
	strcat(strBuf,debugstring[state]);
	strcat(strBuf,"\r\n");
	DebugPrintf(strBuf);
#endif

	//delay(50); //dbg
	
	//find the next table entry.
   	for(index=0;index<MAX_STATES && state_table[index].state != state;index++);
    if(index>=MAX_STATES)
   		return -1; //no entry in the table.

	//perform the new state functions
	for(j=0;j<MAX_FUNCTIONS && state_table[index].service_func[j];j++)
		state_table[index].service_func[j](
		state_table[index].arg[j]
		//(void *)0
		);

#if 0				
	//look for unconditional branch in the new state.
	for(j=0;j<MAX_TRANSITIONS && state_table[index].transition[j];j++)
		if(state_table[index].transition[j] == SLAC_unconditional_branch)
			return slac_state_machine(0); 
#endif
	return 1;			 
}


BOOLEAN GetRohStatus(void)
{
	BOOLEAN ret;
	
	ret = FALSE;
	
	switch(state)
	{
		case networkbusy:
		case networkbusy1:
		case networkbusy2:
		case disconnect:
		case disconnect1:
		case disconnect2:
		case disconnect3:
		case disconnect4:
		case disconnect5:
		case disconnectslic:
			ret = TRUE;
			break;
		default:
			break;
	}
	
	return ret;
}

int GetCurrentSlacState(void)
{
	return state;
}

void slac_tsk(int command_type)
{ //process slac command.
   int delay_const, cntr_val;
   void *p = NULL;
   
    switch(command_type)
    {
      case SLAC_caller_id:
      	  if(bfirstRing)
    		  caller_id();
    	  bfirstRing = FALSE;
      break;

      case SLAC_call_waiting_ack:
    	  off_timer(SLAC_TIMER_CADENCE); 	// stop failure mute delay timer
    	  on_cidcw_delay(p);  				//start delay before sending cidcw data
      break;
      
      case SLAC_cidcw_transmit:
          cw_caller_id(p);
          delay_const = mute_delay_success_timeout;
          mute_codec_delay((void *)delay_const);  // start success mute delay timer
      break;
      	  
      case SLAC_call_wait_unmute:
      	if(WSU003_Config)
        	UpdateCodec(CODEC_DEMUTE_VALUE);
	      else
    	    UnMuteSlacTx(0);
      break;
      
      case SLAC_supv_aoc:
        cntr_val = queue_get(&(queue[0]));
        LineSupervision(command_type, cntr_val);
      break;
      
      case SLAC_supv_conn:
      case SLAC_supv_disconn:
      case SLAC_supv_continue:
        LineSupervision(command_type, 0);
      break;
      
      default:
           slac_state_machine(command_type);
    }
}

#if 0
state_service_func SlacTest(void *p)
{
//  REGISTER_DATA *DataReg;
//  READ_REGISTER_DATA *RdDataReg;
  int DataVal;
  double fVal;
  double Beta = 0.01;
  double RloopStepSz = 0.244;
  double StepSz = 31.13e-6;
  
  delay(50);
  
  WriteSingleByte(0xA7);					// READ_VAB
  DataVal = ((0x00FF & ReadSingleByte()) << 8);
  DataVal |= 0x00FF & ReadSingleByte();
  fVal = StepSz * (double)DataVal / Beta;
  sprintf(strBuf,"\tSLAC Test:  Loop Volatage = %2.3f V\r\n", fVal);
  DebugPrintf(strBuf);
  
  WriteSingleByte(0xA9);					// READ_VIMT
  DataVal = ((0x00FF & ReadSingleByte()) << 8);
  DataVal |= 0x00FF & ReadSingleByte();
  fVal = StepSz * (double)DataVal/* / 3010*/;
  sprintf(strBuf,"\tSLAC Test:  Loop Current = %3.3f mA\r\n", fVal*100/*1e6*/);
  DebugPrintf(strBuf);
  
  WriteSingleByte(0xAD);					// READ_RLOOP
  DataVal = ((0x00FF & ReadSingleByte()) << 8);
  DataVal |= 0x00FF & ReadSingleByte();
  fVal = RloopStepSz * DataVal;
  sprintf(strBuf,"\tSLAC Test:  Loop Resistance = %4.2f Ohms\r\n", fVal);
  DebugPrintf(strBuf);
  
  WriteSingleByte(0xAB);					// READ_VILG
  DataVal = ((0x00FF & ReadSingleByte()) << 8);
  DataVal |= 0x00FF & ReadSingleByte();
  fVal = StepSz * DataVal / 6040; /* RLG in ohms */
  sprintf(strBuf,"\tSLAC Test:  Longitudinal Current = %4.2f mA\r\n", fVal);
  DebugPrintf(strBuf);
  return 0;
}
#endif
