/*file duart_ti16c752.h*/
#define u16 unsigned int
#define s16 int
/*THR ready timeout factor*/
#define _UART_THR_MAX_TIME_OUT_ 250 /*for 9600 bps*/


/*interrupts*/
///#define UART_B_INTR_FLAG 0x0002 //INT1
///#define UART_A_INTR_FLAG 0x0004 //INT2

/*uart register addresses*/
#ifdef _ti16c752_ports_
/*uart A*/
volatile ioport u16 		port1000;
volatile ioport u16 		port1001;
volatile ioport u16 		port1002;
volatile ioport u16 		port1003;
volatile ioport u16 		port1004;
volatile ioport u16 		port1005;
volatile ioport u16 		port1006;
volatile ioport u16 		port1007;
/*uart B*/
volatile ioport u16 		port2000;
volatile ioport u16 		port2001;
volatile ioport u16 		port2002;
volatile ioport u16 		port2003;
volatile ioport u16 		port2004;
volatile ioport u16 		port2005;
volatile ioport u16 		port2006;
volatile ioport u16 		port2007;
#else
extern volatile ioport u16 		port1000;
extern volatile ioport u16 		port1001;
extern volatile ioport u16 		port1002;
extern volatile ioport u16 		port1003;
extern volatile ioport u16 		port1004;
extern volatile ioport u16 		port1005;
extern volatile ioport u16 		port1006;
extern volatile ioport u16 		port1007;
/*uart B*/
extern volatile ioport u16 		port2000;
extern volatile ioport u16 		port2001;
extern volatile ioport u16 		port2002;
extern volatile ioport u16 		port2003;
extern volatile ioport u16 		port2004;
extern volatile ioport u16 		port2005;
extern volatile ioport u16 		port2006;
extern volatile ioport u16 		port2007;
#endif

/*#define __UART_DEBUG_B__*/
#ifndef __UART_DEBUG_B__
/*ports*/
#define UART_A_RHR_REG 		port1000
#define UART_A_THR_REG 		port1000
#define UART_A_IER_REG 		port1001
#define UART_A_FCR_REG 		port1002
#define UART_A_IIR_REG 		port1002
#define UART_A_LCR_REG 		port1003
#define UART_A_MCR_REG 		port1004          
#define UART_A_LSR_REG 		port1005
#define UART_A_MSR_REG 		port1006
#define UART_A_SPR_REG 		port1007
/*for these registers ensure DLAB=1 (b7 of LCR) before accessing*/
#define UART_A_DLL_REG 		port1000
#define UART_A_DLH_REG 		port1001
#define UART_A_EFR_REG 		port1002
#define UART_A_Xon1_REG 	port1004
#define UART_A_Xon2_REG		port1005
#define UART_A_Xoff1_REG	port1006
#define UART_A_Xoff2_REG 	port1007
#define UART_A_TCR_REG 		port1006
#define UART_A_TLR_REG 		port1007
#define UART_A_FIFO_Rdy_REG port1007
#else
#define UART_A_RHR_REG 		port2000
#define UART_A_THR_REG 		port2000
#define UART_A_IER_REG 		port2001
#define UART_A_FCR_REG 		port2002
#define UART_A_IIR_REG 		port2002
#define UART_A_LCR_REG 		port2003
#define UART_A_MCR_REG 		port2004          
#define UART_A_LSR_REG 		port2005
#define UART_A_MSR_REG 		port2006
#define UART_A_SPR_REG 		port2007
/*for these registers ensure DLAB=1 (b7 of LCR) before accessing*/
#define UART_A_DLL_REG 		port2000
#define UART_A_DLH_REG 		port2001
#define UART_A_EFR_REG 		port2002
#define UART_A_Xon1_REG 	port2004
#define UART_A_Xon2_REG		port2005
#define UART_A_Xoff1_REG	port2006
#define UART_A_Xoff2_REG 	port2007
#define UART_A_TCR_REG 		port2006
#define UART_A_TLR_REG 		port2007
#define UART_A_FIFO_Rdy_REG port2007
#endif



/*accessible when LCR =  0xxxxxxx */
#define UART_B_RHR_REG 		port2000
#define UART_B_THR_REG 		port2000
#define UART_B_IER_REG 		port2001
#define UART_B_FCR_REG 		port2002
#define UART_B_IIR_REG 		port2002
#define UART_B_LCR_REG 		port2003
#define UART_B_MCR_REG 		port2004
#define UART_B_LSR_REG 		port2005
#define UART_B_MSR_REG 		port2006
#define UART_B_SPR_REG 		port2007
/*accessable when LCR =  1xxxxxxx */
#define UART_B_DLL_REG 		port2000
#define UART_B_DLH_REG 		port2001
/*accessible when LCR=10111111 */
#define UART_B_EFR_REG 		port2002
#define UART_B_Xon1_REG 	port2004
#define UART_B_Xon2_REG		port2005
#define UART_B_Xoff1_REG	port2006
#define UART_B_Xoff2_REG 	port2007
/*accessible when EFR = xxx1xxxx 
			 and  MCR = x1xxxxxx */
#define UART_B_TCR_REG 		port2006
#define UART_B_TLR_REG 		port2007
/*accessible when CSA=CSB=0
			 and  MCR = xxx0x1xx */
#define UART_B_FIFO_Rdy_REG port2007


//$$$$$$$$$$$ Accessible directly.

//RHR	UART_A_RHR_REG	UART_B_RHR_REG
//THR	UART_A_THR_REG  UART_B_THR_REG

//IER   UART_A_IER_REG  UART_B_IER_REG
#define CTS_int_enable			0x80
#define RTS_int_enable			0x40
#define Xoff_sleep_mode			0x20
#define X_sleep_mode			0x10
#define Modem_status_int		0x08
#define Rx_line_status_int		0x04
#define THR_empty_int			0x02
#define Rx_data_available_int	0x01

//FCR - write only UART_A_FCR_REG UART_B_FCR_REG
#define Rx_trigger_8    0x00
#define Rx_trigger_16   0x40
#define Rx_trigger_56   0x80
#define Rx_trigger_60   0xC0
#define Tx_trigger_8    0x00
#define Tx_trigger_16   0x10
#define Tx_trigger_32   0x20
#define Tx_trigger_56   0x30
#define DMA_mode_select 0x08
#define Reset_Tx_FIFO   0x04
#define Reset_Rx_FIFO   0x02
#define Enable_FIFOs    0x01
#define set_FCR (Rx_trigger_8|Tx_trigger_56|Enable_FIFOs|Reset_Tx_FIFO|Reset_Rx_FIFO)
#define uart_a_set_FCR UART_A_FCR_REG=set_FCR;
#define uart_b_set_FCR UART_B_FCR_REG=set_FCR;

//IIR - read only and reading destroys it. UART_A_IIR_REG UART_B_IIR_REG
#define FCR0							0xC0
#define _Receiver_line_status_error_	0x06
#define _Receiver_timeout_int_    		0x0C
#define _RHR_int_                 		0x04
#define _THR_int_                 		0x02
#define _Modem_int_               		0x00
#define _Received_Xoff_ 				0x10
#define _CTS_RTS_high_                  0x20

//LCR UART_A_LCR_REG UART_B_LCR_REG
#define DLAB_and_EFR_enable		0xbf
#define Break_control_bit		0x40
#define Set_parity				0x20
#define Parity_type_select		0x10
#define Parity_enable			0x08
#define _FORCE_PARITY_			Set_parity
#define _ODD_PARITY_    		Parity_enable
#define _EVEN_PARITY_   		(Parity_enable|Parity_type_select)
#define _NO_PARITY_     		0x00
#define Stop_bits_2				0x04
#define Stop_bits_1				0x00
#define Word_length_8			0x03
#define Word_length_7			0x02
#define Word_length_6			0x01
#define Word_length_5			0x00

//MCR - UART_A_MCR_REG UART_B_MCR_REG
#define clock				0x80
#define TCR_and_TLR_enable	0x40
#define Xon_any				0x20
#define Enable_loopback		0x10
#define IRQ_enable			0x08
#define FIFO_Rdy_enable		0x04
#define RTS					0x02
#define DTR					0x01

//LSR - read only  UART_A_LSR_REG UART_B_LSR_REG
#define Error_in_Rx_FIFO		0x80
#define THR_and_TSR_empty	0x40
#define THR_empty			0x20
#define Break_int			0x10
#define Framing_error		0x08
#define Parity_error			0x04
#define Overrun_error		0x02
#define Data_in_receiver		0x01

//MSR - read only UART_A_MSR_REG UART_B_MSR_REG
#define CD			0x80
#define RI			0x40
#define DSR			0x20
#define CTS			0x10
#define dCD			0x08
#define dRI			0x04
#define dDSR		0x02
#define dCTS		0x01

//SPR - does not need something special


//$$$$$$$$$$$ Accessible if LCR = DLAB_and_EFR_enable;

//DLL, DHL
#define uart_a_baud_rate(baud_rate)\
	UART_A_DLL_REG=(unsigned char)(0xFF & (baud_rate));\
	UART_A_DLH_REG=(unsigned char)(0xFF & ((baud_rate)>>8));
#define uart_b_baud_rate(baud_rate)\
	UART_B_DLL_REG=(unsigned char)(0xFF & (baud_rate));\
	UART_B_DLH_REG=(unsigned char)(0xFF & ((baud_rate)>>8));

#define UART_BAUD_300	6827
#define UART_BAUD_600	3413
#define UART_BAUD_1200	1707
#define UART_BAUD_1800  1138
#define UART_BAUD_2000  1024
#define UART_BAUD_2400   853
#define UART_BAUD_3600   569
#define UART_BAUD_4800   427
#define UART_BAUD_7200   284
#define UART_BAUD_9600   213
#define UART_BAUD_14400  142
#define UART_BAUD_19200  107
#define UART_BAUD_38400   53
#define UART_BAUD_57600   36
#define UART_BAUD_64000   32
#define UART_BAUD_76800   27
#define UART_BAUD_115200  18
#define UART_BAUD_128000  16
#define UART_BAUD_153600  13

//EFR	UART_A_EFR_REG UART_B_EFR_REG
#define AutoCTS							0x80
#define AutoRTS							0x40
#define Special_character_detect		0x20
#define Enable_enhanced_functions		0x10
#define _NO_TRANSMIT_FLOW_CONTROL_		0x00
#define _TRANSMIT_XON1_XOFF1_			0x80
#define _TRANSMIT_XON2_XOFF2_			0x40
#define _NO_RECEIVE_FLOW_CONTROL_		0x00
#define _RECEIVER_COMPARES_XON1_XOFF1	0x20
#define _RECEIVER_COMPARES_XON2_XOFF2	0x01

//Xon1	UART_A_Xon1_REG UART_B_Xon1_REG
//Xoff1	UART_A_Xoff1_REG UART_B_Xoff1_REG
//Xon2	UART_A_Xon1_REG UART_B_Xon1_REG
//Xoff2	UART_A_Xoff1_REG UART_B_Xoff1_REG



//$$$$$$$$$$$ Accessible if MCR |= TCR_and_TLR_enable;


//TCR UART_A_TCR_REG UART_B_TCR_REG
#define uart_a_TCR UART_A_TCR_REG=0x1f;
#define uart_b_TCR UART_B_TCR_REG=0x1f;

//TLR UART_A_TLR_REG UART_B_TLR_REG
#define uart_a_TLR UART_A_TLR_REG=0x00;
#define uart_b_TLR UART_B_TLR_REG=0x00;



//$$$$$$$$$$$ Accessible if MCR |= FIFO_Rdy_enable;
//						and	MCR &=~Enable_loopback;

//FIFO_Rdy read only UART_A_FIFO_Rdy_REG UART_B_FIFO_Rdy_REG
#define RX_FIFO_B_Status	0x20
#define RX_FIFO_A_Status	0x10
#define TX_FIFO_B_Status	0x02
#define TX_FIFO_A_Status	0x01



//$$$$$$$$$$$ Primitives.
/*init*/
void uart_a_init(int);
void uart_b_init(int);
/*fputc*/
void uart_b_fputc(const int c);
void uart_a_fputc(const int c);
void uart_b_fputs(const char* pBuf);
void uart_a_fputs(const char* pBuf);
/*blocked/unblocked output*/
int uart_b_fputc_(const int c, int blocked);
int uart_a_fputc_(const int c, int blocked);
int fr_uart_a_fputc(const int c);
int fr_uart_a_fputs(const char* pBuf);
/*isrs*/
//interrupt void UART_A_isr(void);
//interrupt void UART_B_isr(void);
/*serial cable*/
/*set parameters*/
extern int msr_store;
extern int uart_b_isr_occured;
void uart_b_isr_deferred_part(void);
void uart_b_modem_interrupt(int set);
void check_serial_cable_state(int *msr);
void check_serial_cable_state_timer(int msr);
void enable_uart_a_autoRTSCTS(void);
/*autoboud*/
void autoboudA(int rc);
/*EOF*/

