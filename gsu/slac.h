#ifndef __SLAC_H__
#define __SLAC_H__

/* Variable Types */
typedef unsigned char  BYTE;
typedef unsigned short USHORT;
typedef unsigned int   WORD;
typedef unsigned long  DWORD;
typedef unsigned int BOOLEAN;
typedef BYTE INTERRUPT_TYPE;

#define ReadSlacAvail (!(ISLAC_BUSY & IO_SLAC_AVAIL_REG))

#define NULL 0

#ifndef STD_
enum BOOLEAN {FALSE, TRUE};
#endif

enum INTERRUPT_TYPE {GLOBALERROR, OTHERERROR, DIGIT/*, ON_HOOK, OFF_HOOK*/};

/****** Advice of Charge line Supervision Params *****/
#define MAX_ACTIONS								5
typedef struct supervision_action
{
  int type;
  int duration;
} SUPV_ACTION;

typedef struct supervision_events
{
  SUPV_ACTION conn_supv[MAX_ACTIONS];
  SUPV_ACTION disconn_supv[MAX_ACTIONS];
  SUPV_ACTION aoc_supv[MAX_ACTIONS];
} SUPV_EVENTS;

enum {NO_ACTION, SUPV_DELAY, SUPV_LINE, SUPV_HARD_REV};

enum {NOT_BLOCKED, BLOCKED, HOOK_BLOCKED};

#define	SUPV_SUBTYPE_RAMP_MASK		0x01
#define SUPV_SUBTYPE_12KHz_MASK		0x02
#define SUPV_SUBTYPE_16KHz_MASK		0x04
#define SUPV_SUBTYPE_REV_MASK		0x08


/* SLAC Interrupt Unmasking Bit */
#define SLAC_INTR_FLAG          				INT0

#define ISLAC_BUSY 								0x1

/* Call Progress Tones and Ringing Tone defines */
#define DIAL_TONE								0xff10
#define RECEIVER_OFF_HOOK						0xff11
#define NO_SUCH_NUMBER_TONE_1					0xff12
#define NO_SUCH_NUMBER_TONE_2					0xff13
#define NO_SUCH_NUMBER_TONE_3					0xff14
#define CPE_ALERTING_SIGNAL						0xff15
#define CALL_WAITING_TONE						0xff16
#define SS_CONFIRMATION_TONE					0xff17
#define SS_FAILURE_TONE							0xff18

/**********************************************************
SLAC utils.c defines
**********************************************************/ 

#define PI									3.141592653
#define FS									8000
#define MAX_INT								32768

/* SLAC i/o */

#define IO_SLAC_ADDR							port7000 
#define IO_SLAC_AVAIL_REG						port6000
ioport BYTE IO_SLAC_ADDR;
ioport BYTE IO_SLAC_AVAIL_REG;

/* Slac task initializer functions */
void slac_tsk(int);
void InitSlac(void);

/* SLAC interrupt */
//interrupt void SLAC_isr(void); 

/* SLAC low level dirver functions */
void WriteSingleByte(BYTE byte);
BYTE ReadSingleByte(void);

/* slac driver functions. */
BOOLEAN GetRohStatus(void);
BYTE GetDialedDigit(void);
BYTE GetSlicState(void);
void SetSlicState(BYTE SlicState);
void SetSlicState2(BYTE SlicState);
void SetSlicRevPolarity(BYTE reversed);
void DisablePcmReceive(void);
void EnablePcmReceive(void);
void setup_ring(void);
void setup_dial_tone(void);
void set_up_ring_back(void);
void set_up_busy(void);
void setup_network_busy_signal(void);
void setup_roh_signal(void);
void CollectDigits(BYTE digit);
void SetUpSigGenA(BYTE ToneType);
void SetUpSigGenB(BYTE ToneType);
BOOLEAN OnHookStatus(void);
int GetCurrentSlacState(void);
void VoiceModeSwitch(void);
void DataModeSwitch(void);
void DataCommunication(void);
void LineSupervision(int command_type, int delay_val);
int LoadLineSupervParams(void);

/* SLAC timer functions */
state_service_func transient_func(void *p);
state_service_func on_busy_timer(void *p);
state_service_func start_slac_off_timer(void *p);
state_service_func start_roh_timer(void *p);
state_service_func start_digit_timer(void *p);
state_service_func start_ring_back_on_timer(void *p);
state_service_func start_ring_back_off_timer(void *p);
state_service_func start_ring_on_timer(void *p);
state_service_func start_ring_off_timer(void *p);
state_service_func start_disconnect_timer(void *p);
state_service_func start_network_busy_on_timer(void *p);
state_service_func start_network_busy_off_timer(void *p);
state_service_func start_ss_prompt_on_timer(void *p);
state_service_func start_ss_prompt_off_timer(void *p);
state_service_func start_ss_prompt_delay_timer(void *p);
state_service_func start_crss_failed_on_timer(void *p);
state_service_func start_crss_failed_off_timer(void *p);
state_service_func start_ss_cancel_timer(void *p);
state_service_func start_failure_timer(void *p);
state_service_func start_no_sim_on_timer(void *p);
state_service_func start_no_sim_off_timer(void *p);
state_service_func start_ssconfirm_timer(void *p);
state_service_func start_ssfailure_timer(void *p);
state_service_func start_ssneg_status_timer(void *p);
state_service_func start_pulse_break_timer(void *p);
state_service_func start_inter_pulse_timer(void *p);
state_service_func start_flash_timer(void *p);
state_service_func start_invalid_flash_timer(void *p);
state_service_func on_call_waiting_sas_delay(void *p);
state_service_func off_call_waiting_sas(void *p);
state_service_func ten_sec_call_waiting_sas(void *p);
state_service_func on_cas_delay(void *p);
state_service_func off_cas(void *p);
state_service_func on_cidcw_delay(void *p);
state_service_func start_caller_id_timer(void *p);
state_service_func mute_codec_delay(void *p);
state_service_func start_ignore_slac_interrupt(void *p);
state_service_func short_ring_delay(void *p);
state_service_func start_cid_message_timer(void *p);
state_service_func post_cid_message_delay(void *p);
state_service_func dtmf_cid_delay_message(void *p);
state_service_func dtmf_setup_gen_delay(void *p);

state_service_func start_fax_ring_on_timer(void *p);
state_service_func start_fax_ring_off_timer(void *p);

state_service_func clear_dialing_parameters(void *p);
state_service_func ss_service_handler(void *p);

state_service_func on_disconnect_timer(void *p);
state_service_func first_digit_timer(void *p);

state_service_func off_timer(int timer_index);
state_service_func off_timers(void *p);
state_service_func off_cadence_timer(void *p);
state_service_func off_slac_timer(void *p);
state_service_func off_timer_1(void *p);
state_service_func on_timer_functionCall(timer_service_func (*timerFunction)(void *), int timeout_value,int timer_index);
timer_service_func digitTimeout(void *p);
state_service_func on_supv_delay_timer(void *p);
state_service_func supv_timer_function(void *p);

/* Slac state machine driver functions */ 
state_service_func on_dial_tone(void *p);
state_service_func on_busy(void *p);
state_service_func on_roh(void *p);
state_service_func on_ring_back(void *p); 
state_service_func on_ringing(void *p);
state_service_func off_ring(void *p);
state_service_func fax_ringing(void *p);
state_service_func on_network_busy_signal(void *p);
state_service_func on_no_sim_signal(void *p);
state_service_func DisconnectSlic(void *p);
state_service_func Communication(void *p);
state_service_func OffCommunication(void *p);
state_service_func on_echo_cancel(void *p);
state_service_func off_echo_cancel(void *p); 
state_service_func store_pulse_digit(void *p);
char dialing_symbol(int value);
state_service_func increment_pulse_count(void *p);
state_service_func reset_pulse_count(void *p);
state_service_func set_ss_prompt_count(void *p);
state_service_func decrement_ss_prompt_count(void *p);
state_service_func on_ss_prompt_signal(void *p);
state_service_func set_crss_failed_count(void *p);
state_service_func decrement_crss_failed_count(void *p);
state_service_func on_crss_failed_signal(void *p);

state_service_func mute_codec(void *p);
state_service_func Mute(void *p);
state_service_func MuteSlacTx(void *p);
state_service_func UnMuteSlacTx(void *p);
state_service_func CallWaitSetUp(void *p);
//state_service_func SetSendToneVal(void *p);
state_service_func On_CallWaitingTone(void *p);
state_service_func on_CasTone(void *p);
state_service_func flash_handler(void *p);
state_service_func cw_caller_id(void *p);
state_service_func off_generator_tone(void *p); 
state_service_func on_confirm_tone(void *p);
state_service_func off_confirm_tone(void *p);
state_service_func on_failure_tone(void *p);
state_service_func off_failure_tone(void *p);
state_service_func on_neg_status_tone(void *p);
state_service_func off_neg_status_tone(void *p);
state_service_func check_gsm_state(void *p);
state_service_func update_call_metering_info(void *p);
state_service_func SetOhtState(void *p);
state_service_func set_to_valid(void *p);
state_service_func SendCallerIdMsg0(void *p);
state_service_func SendCallerIdMsg1(void *p);
state_service_func check_waiting_CID(void *p);
state_service_func check_call_flashed(void *p);
state_service_func set_call_flashed(void *p);
state_service_func SetUpForDtmf(void *p);
state_service_func RestoreFromDtmf(void *p);
state_service_func SetDtmfGenB(void *p);
state_service_func SendDtmf(void *p);

/* Slac utility/debug-test functions */
DWORD Hertz2LongHex(WORD Freq);
int Decimal2ShortHex(double Decimal);
int Hertz2ShortHex(WORD Freq);

state_service_func SlacTest(void *p);
state_service_func BatteryTest(void *p);

//messaging functions
state_service_func on_hook_to_cc(void *p);
state_service_func off_hook_to_cc(void *p);
state_service_func connect_to_cc(void *p);
state_service_func send_digits_to_cc(void *p);

#define	MAX_STATES	66		//61		//65
#define MAX_FUNCTIONS 5
#define MAX_TRANSITIONS 9

typedef struct _state_context
{
	int					state;
	state_service_func	(*service_func[MAX_FUNCTIONS])(void *);
	void				*arg[MAX_FUNCTIONS];
	int					transition[MAX_TRANSITIONS];
	int					next_state[MAX_TRANSITIONS];
} STATE_CONTEXT;


//states
enum slac_states_tag
{
  initial,valid_hook,off_hook,dial,
  disconnect,disconnect1,disconnect2,disconnect3,disconnect4,disconnect5,
  disconnectslic,collectdigits,callprogress,ringback,ringback1,connectstate,
  handle_call_waiting,call_waiting_sas0,call_waiting_sas1,call_waiting_cas0,
  call_waiting_sas2,call_waiting_idle,
  ring0,ring1, networkbusy,networkbusy1,networkbusy2,ssconfirm,ssfailure,ssnegstatus,
  dcepump,pumpbusy,pumpbusy1,pumpbusy2,
  nosbusy,nosbusy1,nosbusy2,
  nosimbusy,nosimbusy1,nosimbusy2,
  pulse_detect,pulse_make,flash_detect,valid_flash_detect,
  flash,pulse_break,collect_pulses,
  ss_service,ss_pulse_detect,ss_pulse_make,ss_pulse_break,ss_collect_pulses,
  ss_flash_detect, valid_ss_flash_detect, 
  ss_prompt0, ss_prompt1, ss_prompt2,ss_collectdigits,
  cid_messages,cid_send0,cid_send1,
  cid_dtmf0,cid_dtmf1,cid_dtmf2,cid_dtmf3,cid_dtmf4
};

#define	MAX_SLAC_TIMERS	(4)

typedef struct _control_block
{
	int		timer_id[MAX_SLAC_TIMERS];
} CB;


#define	SLAC_TIMER_0		(0)
#define SLAC_TIMER_CADENCE 	(1)
#define SLAC_TIMER_1		(2)
#define SLAC_TIMER_SUPV		(3)

#define	busy_timeout_value 				 50
#define	network_busy_on_timeout_value    20
#define network_busy_off_timeout_value	 30
#define	digit_timeout_value 			500   /* 5 s timeout */
#define	first_digit_timeout_value	   2000 /* start of Busy  */
#define	disconnect_timeout_value 	   2000 /* start of ROH   */
#define	ring_back_on_timeout_value 	 	200
#define	ring_back_off_timeout_value  	400
#define	ring_on_timeout_value 		 	200
#define	ring_off_timeout_value 		 	400
#define	roh_timeout_value 			 	 10
#define	slac_off_timeout_value 		   2000 /* start of disconnect slac */ 
#define ssconfirm_timeout_value		 	 50
#define ssfailure_timeout_value		 	100
#define ssneg_status_timeout_value		100
#define pulse_break_timeout_value	 	  8
#define inter_pulse_timeout_value	 	 50
///////////////////////////////////////////
// flash detect spec states 300 ms to 1 sec
#define flash_timeout_value			 	 35		//48		//70
#define invalid_flash_timeout_value		 100	//22		//60
///////////////////////////////////////////
#define cw_sas_delay_on_timeout_value	 10
#define cw_sas_off_timeout_value         25
#define cas_delay_on_timeout_value		  4
#define cas_off_timeout_value			  8
#define call_waiting_timeout_value	 	 10
#define cidcw_transmit_timeout_value	 10
#define ten_sec_cw_sas_on_timeout_value	1000
#define caller_id_delay				 	 25
/////////  dtmf cid delay consts  //////////
#define dtmf_caller_id_delay			 50
#define dtmf_inter_message_delay		  7
#define dtmf_gen_delay					  1
///////////////////////////////////////////
// failure delay=160 ms; success delay 100 ms
#define mute_delay_fail_timeout			 16
#define mute_delay_success_timeout		 10
///////////////////////////////////////////
#define no_sim_on_timeout_value			 20
#define no_sim_off_timeout_value         100
#define ss_prompt_on_timeout_value	     7
#define ss_prompt_off_timeout_value      5
#define ss_prompt_delay_timeout_value    100
////////////////////////////////////////////
// failed invalid supplementary svce attempt = 1 sec
// digit timeout after flash = 5 sec
// unsuccessful supplementary svce attempt = 15 sec
#define ss_failure_timeout_value		 100
#define flash_cancel_timout_value		 500
#define	ss_cancel_timeout_value			 1500
////////////////////////////////////////////
#define crss_failed_on_timeout_value	 100	//20
#define crss_failed_off_timeout_value    4
#define fax_on_ring_delay_timeout		50
#define fax_off_ring_delay_timeout		40
#define disable_interrupt_timeout		10
#define short_ring_timeout_value		40
#define cid_message_delay				25
#define post_cid_message_timeout		25

state_service_func slac_restart(void *p);

#define ON_HOOK								0x00
#define OFF_HOOK							0x08
#define POLLING_ON_HOOK						0x00
#define POLLING_OFF_HOOK					0x02


/* SLAC READ register addresses */
#define READ_GlOBAL_SUPERVISION				0x21
#define READ_GLOBAL_DEVICE_STATUS_REG		0x3D
#define READ_INTERRUPT_REG					0x3F
#define READ_REVISON_CODE					0x73
#define READ_UNLOCK_SIG_REG					0x4F
#define READ_ISLIC_STATE_REG				0x57
#define READ_CHNL_CONFIG_REG_1				0x63
#define READ_CHNL_CONFIG_REG_2				0x65
#define READ_CHNL_CONFIG_REG_3				0x61
#define READ_LOW_BATT_VOLT					0xAF
#define READ_HI_BATT_VOLT					0xB1
#define READ_POS_BATT_VOLT					0xB3
#define READ_METER_TARGET_LIM				0xD1


/* SLAC WRITE register addresses */ 
#define CALIBRATE_ANALOG_CIRCUITS			0x7C

#define DEACTIVATE_CHNNL					0x00
#define WRITE_HARDWARE_RESET				0x04
#define WRITE_DEV_CONFIG_REG_1				0x46
#define WRITE_DEV_CONFIG_REG_2				0x36
#define WRITE_TX_RX_CLK_SLOT_SELECT			0x44
#define WRITE_GLBL_DEV_STATUS_MSK_REG		0x34
#define WRITE_ENABLE_CHANNL_REG				0x4A
#define WRITE_TX_TIME_SLOT					0x40
#define WRITE_RX_TIME_SLOT					0x42
#define WRITE_CHNL_CONFIG_REG_1				0x62
#define WRITE_CHNL_CONFIG_REG_2				0x64
#define WRITE_CHNL_CONFIG_REG_3				0x60
#define WRITE_CHNL_CONFIG_REG_4				0x68
#define WRITE_CHNL_CONFIG_REG_5				0x6A
#define WRITE_CHNL_CONFIG_REG_6_Mask		0x6C
#define WRITE_CHNL_CONFIG_REG_7_Mask		0x6E
#define WRITE_RECEIVE_GAIN					0x82
#define WRITE_TRANSMIT_GAIN					0x80
#define WRITE_Z_FLTR_COEFFS					0x84
#define WRITE_R_FLTR_COEFFS					0x8A
#define WRITE_X_FLTR_COEFFS					0x88
#define WRITE_B_FLTR_COEFFS_7_TAPS			0x86
#define WRITE_B_FLTR_COEFFS_6_TAPS			0x96
#define WRITE_ERROR_LEVEL_THRESHOLD			0x8E
#define WRITE_ECHO_GAIN						0x8C
#define WRITE_ADPATIVE_B_FLTR_CNTRL			0x90
#define WRITE_ANALOG_GAIN_DISN				0xCA
#define WRITE_LOOP_SUPV_PARMS				0xC2
#define WRITE_DC_FEED_PARMS					0xC6
#define WRITE_ISLIC_STATE_REG				0x56
#define WRITE_ACTIVATE_OP_MODE				0x0E
#define WRITE_SIG_GEN_A_PARAMS				0xD2
#define WRITE_SIG_GEN_B_PARAMS				0xD4
#define WRITE_SIG_GEN_B_DTMF_CONTROL		0x66
#define WRITE_CALIBRATE_ANALOG_CKTS			0x7C
#define WRITE_METER_TARGET_LIM				0xD0


/* Register settings */ 
#define DEFAULT_ZEROS							0x00
#define TIMESLOT_0								0x00
#define TIMESLOT_2								0x02
#define TIMESLOT_4								0x04
#define MCLK_2048_MHZ							0x93
#define MCLK_4096_MHZ							0x97
#define XE_PCLK_POS								0x40
#define LOW_PWR_AND_CLK_FAIL					0x8F
#define ENABLE_CHNL_1_RW						0x01
#define ENABLE_CHNL_2_RW						0x02
#define ENABLE_CHNLS_1_AND_2_RW					ENABLE_CHNL_1_RW | ENABLE_CHNL_2_RW
#define ENABLE_BOTH_CHNLS_RW					0x03
#define LINEAR_PCM								0x01
#define HOOK_BIT_UNMASKED						0x7F
#define NDIG_BIT_UNMASKED						0xF7
#define ALL_BITS_MASKED							0xFF
#define DISABLED_PCM_RECEIVE					0x01
//#define ENABLED_PCM_RECEIVE					0x00
#define DISABLED_PCM_TRANSMIT					0x40
//#define ENABLED_PCM_TRANSMIT					0x00

#define B_FILTER_ADAPTIVE_MODE_ON				0x80
#define B_FILTER_ADAPTIVE_MODE_OFF				0x7F

/**** WSU003 only gains ****/
#define WSU003_RX_GAIN_BYTE_1					0x7F	//   0 dB
#define WSU003_RX_GAIN_BYTE_2					0xFF	//   0 dB
#define WSU003_TX_GAIN_BYTE_1					0xE0	//   0 dB
#define WSU003_TX_GAIN_BYTE_2					0x00	//   0 dB
/***************************/

/**** WSU004 chnl 1  gains ****/
#define RX_GAIN_BYTE_1							0x7F	//   0 dB			0x20	// -12 dB
#define RX_GAIN_BYTE_2							0xFF	//   0 dB			0x26	// -12 dB
#define TX_GAIN_BYTE_1							0xFF	//	+6 dB			0xE0	//   0 dB			0x1A	// + 9 dB
#define TX_GAIN_BYTE_2							0xDA	//	+6 dB			0x00	//   0 dB			0x30	// + 9 dB

/**** WSU00  - chnl 2 gains ****/
#define CHNL_2_RX_GAIN_1						0x2D	// - 9 dB			0x40	// - 6 dB
#define CHNL_2_RX_GAIN_2						0x6A	// - 9 dB			0x26	// - 6 dB
#define CHNL_2_TX_GAIN_1						0xE0	//   0 dB
#define CHNL_2_TX_GAIN_2						0x00	//   0 dB

#define DISN_GAIN								0x04
#define ECHO_GAIN								0x04
#define LST										0x20
#define DCR1									0x1C
#define DCR2									0x02
#define DPB										0x00

/* Turn on/off Signal Generator defs */
#define GENERATOR_A_B_OFF						0x00
#define GENERATOR_A_ON							0x04
#define GENERATOR_B_ON							0x08

/* Signal Generator A for Ringing */       	  // 22 Hz Trapezoidal Waveform
#define TEST_TIME_TRAPEZOID_1					0xFF
#define TEST_TIME_TRAPEZOID_2					0x4B
#define SLOPE_TRAPEZOID_1						0xFD
#define SLOPE_TRAPEZOID_2						0x2F
#define SLOPE_TRAPEZOID_3						0x1A
#define SLOPE_TRAPEZOID_4						0xA0
#define AMPLITUDE_RING_1						0x7F	// 200 V
#define AMPLITUDE_RING_2						0xFF    // 200 V
#define DC_BIAS_RING_1							0x00
#define DC_BIAS_RING_2							0x00

#define TEST_TIME_ONE_SHOT						0x00						

/* Loop Supervision Constants */
#define GROUND_KEY_THRESHOLD					0x0D
#define GROUND_KEY_INTEGRATION					0x64
#define CURRENT_SPIKE_THRESHOLD					0x0D
#define ZERO_CROSS_THRESHOLD					0x01
#define SWITCH_HOOK_TRESHOLD					0x30	// 3000 ohms		0x12	// 1125 ohms
#define DIAL_PULSE_MAKE_VOLTAGE					0x00
#define SWITCH_HOOK_DEBOUNCE					0x24	// 18mA debounce
#define STANDBY_LOOP_THRESHOLD					0x0D	// 10.15625 mA
 
/* DC Feed Parameters */                        
#define BATT_SWITCH_VOLTAGE						0x0F	// 11.71875 V		0x39	// 44.453125 V
#define VAS										0x08	// 6.25 V
#define V1										0x48	// 56.25 V
#define RTSLP									0x00	// 44 ms
#define LOOP_CURRENT_LIMIT						0x20	// 25 mA
#define RESISTANCE_FEED							0x26	// 593.75 Ohms
#define RFD_SAT									0x24	// 140.76 Ohms
#define HI_BATT_FAILURE_THRESHOLD				0x29	// 32.03125 V
#define LO_BATT_FAILURE_THRESHOLD				0x17	// 17.96875 V
#define POS_BATT_FAILURE_THRESHOLD				0x00	// 0 V
#define DC_FAULT_CURRENT_THRESHOLD				0x0D	// 10.15625 mA
#define AC_FAULT_CURRENT_THRESHOLD				0x1A	// 50 mA
#define RTLL									0x19	// 9.765 mA
#define RTSL									0x07 	// 2.17 W

/* ISLIC States */
#define STANDBY									0x00
#define TIP_OPEN								0x01
#define ACTIVE									0x02
#define TELETAX									0x03
#define RESERVED								0x04
#define ON_HOOK_TRANSMISSON						0x05
#define DISCONNECT								0x06
#define RINGING									0x07

#define POLREV									0x08
#define AUTO_BATT_SWITCH						0x10
#define HI_BATT_ENABLE							0x20 

/* Metering and Polarity reversal metering */
#define METER_FREQ_16							0x40
#define METER_FREQ_12							0x00
#define ABRUPT_METER_RAMP						0x20
#define SMOOTH_METER_RAMP						0x00
#define REVERSE_METER							0x10
#define NORM_METER								0x00
#define NOT_APPLICABLE							0x00

#define MTR_PEAK_VOLT							0x3F	// 1020 mVrms
#define MTR_TARGET_PK_CURRENT					0x14	// 10.34 mA peak

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////    Caller ID declarations  ////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
/* external func prototypes */
void CID_SendMessage(void);
void CID_Name_Number(char *pName, char *pNumber, char *pDateTime);
void CID_Name(char *pName);
void CID_Number(char *pNumber);
int CID_SetDateTime(char *pDateTime);

/* internal func prototypes */
void TX_DTMF_Number(void);
void TX_NameData(void);
void TX_NumberData(void);
void TX_NameNumberData(void);
void TX_MessageIndication(void);
void ClearCidParams(void);
char GetCidModulation(void);

#endif //__SLAC_H__

