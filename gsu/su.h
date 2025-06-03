#ifndef __SU_H__
#define __SU_H__

#ifdef _FAR_MODE_
#define QUEUEVALUE unsigned long
#else
#define QUEUEVALUE int
#endif

//main queue parameters.
extern int queue0_fed_up;
#define QLMAIN 2048
#define QLMAIN_HIGH 1984
#define QLMAIN_LOW	128
#define QLMAIN_EXTREMELY_LOW 20

//#define __DEBUG__
#ifdef __DEBUG__

#define AT_TSK_DEBUG
#define CC_TSK_DEBUG
#define DCE_TSK_DEBUG
#define SLAC_TSK_DEBUG

void DebugPrintf1(char *str, QUEUEVALUE val); //prototyp.

#else // __DEBUG__

#define DebugPrintf1(str,val) //remove it from the code.

#endif // __DEBUG__

typedef void (*state_service_func)(void *);
typedef void (*timer_service_func)(void *);
typedef void (*VFV)(void);
typedef void (*VFI)(int);
typedef void (*VFPC)(char *);
typedef int  (*IFV)(void);
typedef int  (*IFI)(int);
typedef int  (*IFPC)(char *);

#include <string.h>
#include <timer.h>
#include <intr.h>
#include <stdlib.h>
#include "system_states.h"
#include "slac.h"
#include "led.h" 
#include "duart_ti16c752.h"
#include "queue.h"
#include "bsps.h"
#include "callcontrol.h"
#include "flash.h"

#define MAX_DIGIT_COUNT     80
#define MAX_PIN_LENGTH		18 
#define CPIN_PREIOD			100		// 1 sec
#define CSQ_PERIOD			300	
#define	MAX_CURRENT_CALLS	8
#define MAX_RECORDS			23
#define PARAM_NAME_SZ		11
#define PARAM_DATA_SZ		21
#define SMS_LENGTH			161 	// SMS mesagge length is 160 char.		

typedef struct _current_call
{
	int		present;
	int		state;
	int		dir;
	int		mode;
	int		mpty;
} CURRENT_CALL;

extern CURRENT_CALL current_calls[];
extern int num_current_calls;
extern int num_held_calls;
extern int num_mpty_calls;

typedef struct DIALINGPARAMETERS_TAG
{
	BYTE LastDigitDialed;
	BYTE DialingParams[MAX_DIGIT_COUNT];
	USHORT ndx; // length of DialingParams message
} DIALINGPARAMETERS;

typedef struct FLASH_PARAMS_TAG
{
	char NameParam[PARAM_NAME_SZ];
	char DataParam[PARAM_DATA_SZ];
} FLASH_PARAMS;

extern DIALINGPARAMETERS DialingParams;
extern int dce_state;
extern char pin[];
extern char  SWversion[];
extern char VersionNumber[];
extern int WSU003_Config;

#define VOICE_DATA_SWITCH port5000  
extern volatile ioport unsigned int VOICE_DATA_SWITCH; 
extern int	uart_a_tx_count;
extern int	uart_b_tx_count;
extern int	uart_a_tx_over_threshold;
extern int	uart_b_tx_over_threshold;
extern int	autobaud_enabled;
 

#define MSG_PACKET_LENGTH 100
#define TIMER_0_INTR_FLAG 0x0008
/* #define RING_BACK_DELAY		250		// 2.5 seconds. */
#define CREG_PERIOD 		1000	// 10 seconds.
#define CPAS_PERIOD			100		// 1 second.
/* #define CPAS_SHORT_PERIOD	150		// 1.5 seconds.   */

#define NOBODY	(VFI)0
#define CC		(VFI)tsk_callcontrol
#define DCE		(VFI)dce_tsk
#define SLAC	(VFI)slac_tsk
#define AT		(VFI)at_tsk
#define FR		(VFI)fr_tsk
#define FR_IN	(VFI)fr_stm_in
#define FR_OUT		(VFI)fr_stm_out
//#define MON		(VFI)monitor_tsk

void c_int00(void);
void putscreen(char c);
void putscreens(char *str);
void debugfnctn(void);
void DebugMainMsgEntry(void);
//void memset(void *dst,int val,int len);
void delay(unsigned long duration);
void init_timer_table(void);
int timer_add(int *ret_timer_id,int one_shot,timer_service_func func,void *arg,unsigned short timeout);
int timer_delete(unsigned int timer_id);
int timer_enable(unsigned int timer_id, unsigned int timeout);
int timer_disable(unsigned int timer_id);
int timer_set_callback(unsigned int timer_id, timer_service_func func, void *arg);
int timer_set_one_shot(unsigned int timer_id, int one_shot);
void periodic_timer_func(void);
char derive_modem_type(void);

//function prototypes.
char *get_msg_typ_name(mbx_typ typ);
void send0(VFI handler,int val);
void send1(VFI handler,int command_type,int length, int *val);
#define send0_delayed(delay,handler,command_type) send1_delayed(delay,handler,command_type,-1,0)
void DebugPrintf(char *str);
//#define DebugPrintf(str) uart_b_fputs(str)
//#define DebugPrintf(str) putscreens(str);
unsigned long clock_(void); /*reads system clock and clears it. bits 0.5 ms.*/

void InitTimeDebug(void);
void TimeDebugSPrintf_(char *descr);
#define TimeDebugSPrintf(x) TimeDebugSPrintf_(x)	
#define TimeDebugSPrintf1(x) TimeDebugSPrintf_(x)
#define TimeDebugSPrintf2(x) TimeDebugSPrintf_(x)

void uart_a_transmit(void);
void set_version(void);
void CID_OnhookPrintf(char *txtmsg, char *number);
char *ShowAllCalls();

state_service_func StartPeriodicCREGCheck(void *p);
state_service_func StopPeriodicCREGCheck(void *p);
state_service_func StartPeriodicPASCheck(void *p);
state_service_func StopPeriodicPASCheck(void *p);
state_service_func StartPeriodicCLCCCheck(void *p);
state_service_func Reset_SS_setup(void *p);
state_service_func StartChargingIndication(void *p);
state_service_func StopChargingIndication(void *p);
void erase_delayed_messages(void);
int at_parse(char symb, char *at_mask, int msk_len,int *index);
void at_tsk_init(void);
void at_tsk(int);
void dce_tsk_init(void);
void dce_tsk(int);
void fr_tsk(int);
int fr_stm_in(int);
int fr_stm_out(int);

#endif
