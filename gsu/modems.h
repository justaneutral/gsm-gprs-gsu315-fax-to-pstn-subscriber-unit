#ifndef _MODEMS_H_
#define _MODEMS_H_

//modem include files.
#include "c54x.h"
#include "common.h"
#include "vmodem.h"
#include "gendet.h"

typedef enum DATA_MODE_TAG
{
	COMMAND,
	ON_LINE,
	DATA,
	TRAINING
} DATA_MODE;

extern DATA_MODE data_mode;
extern DATA_MODE fax_mode;
extern DATA_MODE fm_state;

typedef enum DATA_FORMAT_TYPE_TAG
{
	V14_RT,
	V14_R,
	V14_T,
	HDLC_RT,
	HDLC_R,
	HDLC_T,
	RAW_RT,
	RAW_R,
	RAW_T
}
DATA_FORMAT_TYPE;

extern DATA_FORMAT_TYPE fm_data_format;

typedef enum MODEM_DATA_QUEUE_STATE_TAG
{
	EMPTY,
	LOW,	
	UNCHANGED,
	HIGH,
	FULL
}
MODEM_DATA_QUEUE_STATE;

extern struct START_PTRS start_ptrs_table;
extern int Tx_block[];
extern CIRC Tx_sample[];
extern CIRC Tx_data[];
extern CIRC Tx_fir[];
extern int Rx_block[];
extern CIRC Rx_sample[];
extern CIRC Rx_data[];
extern CIRC Rx_fir[];
extern int EQ_coef[];
extern int EC_coef[];
extern int decoder[];
extern CIRC trace_back[];


#define ptrs (&start_ptrs_table)
#define FM (VFI)fm_tsk

//BSP0 - connected to SLAC PCI signal.
#define BSP0_RX_INTR	4 //mask=0x10
#define BSP0_TX_INTR    5 //mask=0x20
//BSP1 - connected to GSM PCI signal.
#define BSP1_RX_INTR	10 //mask=0x400
#define BSP1_TX_INTR    11 //mask=0x800

#define HW_SYSTEM_DELAY 0 

#define VOICE_DATA_SWITCH port5000
extern volatile ioport unsigned VOICE_DATA_SWITCH;

void	bsp0_rx_voice_handler(void);
void	bsp0_tx_voice_handler(void);
void	bsp0_rx_modem_handler(void);
void	bsp0_tx_modem_handler(void);
void	voice_init(void);
void 	data_modem_init(int modem_type, char call_direction, int baud_rate);
void	vxx_modem_init(int modem_type);
void	init_vxx_tx_silence(void);
int		silence_v27_2400_75ms_cf(void);
void	v32_mv_v14_Rx_data_as_8_to_Rx_queue(void);
void	vxx_mv_v14_Rx_data_as_8_to_Rx_queue(void);
extern	VFV	mv_v14_Rx_data_as_8_to_Rx_queue;
void	send_hdlc_Rx_data_as_8_to_fr_tsk(void);
void	v32_BSP_0_rx_isr(void);
void	v22_BSP_0_rx_isr(void);
void	vxx_BSP_0_rx_isr(void);
void	v32_BSP_0_tx_isr(void);
void	v22_BSP_0_tx_isr(void);
void	vxx_BSP_0_tx_isr(void);
extern	VFV	modem_BSP_0_rx_isr;
extern	VFV	modem_BSP_0_tx_isr;
int		data_modem_continue_func(void);
int		ced_modem_continue_func(void);
int		silence_75ms_continue_func(void);
int		silence_v29_75ms_cf(void);
int		fth3_modem_continue_func(void);
int		frh3_modem_continue_func(void);
int		ftmXX_modem_continue_func(void);
int		frmXX_modem_continue_func(void);

void v21_receive_debug_init(void);
int v21_receive_debug(void);
void fax_transmitter_init(void);
int fax_transmitter(void);


extern	IFV modem_continue_func;
void	v32_set_tx_data(int val);
void	vxx_set_tx_data(int val);
extern	VFI	set_tx_data;
int		v32_get_tx_nbits(void);
int		vxx_get_tx_nbits(void);
extern	IFV	get_tx_nbits;
int		v32_get_rx_data(void);
int		vxx_get_rx_data(void);
extern	IFV get_rx_data;
int		v32_get_rx_nbits(void);
int		vxx_get_rx_nbits(void);
extern	IFV	get_rx_nbits;
int		v14_receive(int *rec_buf, int inp_val);
extern	IFV	tx_modem_ready;
int		v32_tx_modem_ready(void);
int		vxx_tx_modem_ready(void);
int		synchronize_bearer(void);

#define QLRx_data_queue 128
#define QLRx_data_queue_HIGH 54
#define QLRx_data_queue_LOW 32

#define QLTx_data_queue 2048 //128
#define QLTx_data_queue_HIGH 2040 //54
#define QLTx_data_queue_LOW 1020//32

extern Queue Rx_data_queue;
extern Queue Tx_data_queue;
extern QUEUEVALUE ar_Rx_data_queue[QLRx_data_queue];
extern QUEUEVALUE ar_Tx_data_queue[QLTx_data_queue];

#define Tx_data_queueEMPTY (queue_empty(&Tx_data_queue))
#define Tx_data_queueGET	((char)queue_get(&Tx_data_queue))

extern int Rx_data_queue_fed_up;
extern int Tx_data_queue_fed_up;
void Rx_and_Tx_data_queue_init(void);
MODEM_DATA_QUEUE_STATE Rx_data_queuep(int *buf, int len);
void v14put(char val);
void mv_Tx_queue_as_v14_to_Tx_data(void);
void mv_Tx_queue_as_hdlc_to_Tx_data(void);
int mv_Tx_queue_as_raw_to_Tx_data(void);
void mv_Rx_queue_to_uart_a(void);
int write_Tx_data_queue(char val);
void mv_uart_a_to_Tx_queue(char val);

//Fax Relay supporting functions and variables.
extern int uart_a_in_event;
extern int uart_c_in_event;
extern int receive;
extern int rx_bsp0_interrupt_counter;
extern int tx_bsp0_interrupt_counter;
extern int rx_bsp0_interrupt_counter_max;
extern int tx_bsp0_interrupt_counter_max;


VFI		fm_tsk(int param);
int uart_c_fputs(char *buf);
int uart_c_fputc(char val); //the bytes to the software fax modem.
int uart_c_fputc_non_block(char val); //the bytes to the software fax modem.
void write_string_to_fr_tsk(const char *buf);

extern int modem_symbol_counter;
extern int preambula_counter;
void init_tx_data(void);
void updateCRC(unsigned short *crc, unsigned char c);
void reverse(char *val);
void fm_at_fth_3(void);
extern unsigned short received_hdlc_crc;
void fm_at_frh_3(void);
void fm_at_fts_8(void);
void fm_at_ftm_96(void);
void fm_at_frm_96(void);

#define	HDLC_FRAME_END			0x01
#define	HDLC_FRAME_ABORT		0x10
#define HDLC_NON_OCTET_ALIGN	0x20
#define HDLC_CRC_ERROR			0x40
#define HDLC_FRAME_LAST			0x02

#endif // _MODEMS_H_
