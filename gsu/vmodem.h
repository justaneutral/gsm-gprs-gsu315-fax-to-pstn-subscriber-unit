#ifndef VMODEM_INCLUSION_
#define VMODEM_INCLUSION_

#define NOT_DETECTED 				0
#define DETECTED 					1
#define DISABLED 					0
#define ENABLED 					1
#define CIRC 						int				
#ifdef ADI_DSP
#define INT_PM						int pm
#else
#define INT_PM						int
#endif
struct COMPLEX {int real,imag;};

	/**** Vector and buffer lengths ****/

#define SIN_TABLE_LEN 				256
#define SIN_BUF_LEN 				  	(SIN_TABLE_LEN+SIN_TABLE_LEN/4)
#define SIN_TABLE_SHIFT 			8
#define DFT_COEF					572	/* 32768*(sqrt(2)/ANALYSIS_LEN)*fudge */
#define DFT_COEF_LEN				256
#define DELAY_STATES 				8
#define TRACE_BACK_LEN 				16
#define DEMOD_DELAY_STRIDE 			4
#define DEMOD_DELAY_LEN 			(DEMOD_DELAY_STRIDE*TRACE_BACK_LEN)
#define TRACE_BACK_BUF_LEN 			(TRACE_BACK_LEN*DELAY_STATES)

	/**** state_ID definitions ****/

#define TX_SILENCE_ID 				0x0100
#define RX_IDLE_ID 					0x0100
/*#define MESSAGE_ID_FIELD			0x007f*/
#define MESSAGE_ID					0x0040

	/**** status response messages ****/

#define STATUS_OK					0x00
#define DETECT_FAILURE 				0x10
#define SYNC_FAILURE 				0x20
#define CHECKSUM_FAILURE 			0x21
#define CRC_FAILURE 				0x22
#define TRAIN_LOOPS_FAILURE 		0x30
#define START_EQ_FAILURE 			0x30
#define TRAIN_EQ_FAILURE 			0x31
#define SCR1_FAILURE 				0x40
#define LOSS_OF_LOCK				0x50
#define GAIN_HIT_STATUS				0x51
#define EXCESSIVE_MSE_STATUS		0x52
#define EXCESSIVE_RTD_STATUS		0x53
#define RETRAIN						0x60
#define RETRAIN_FAILURE				0x61
#define RENEGOTIATE					0x62
#define RENEGOTIATE_FAILURE			0x63
#define V22_USB1_DETECTED			0x70
#define V22_S1_DETECTED				0x71
#define V22_SB1_DETECTED			0x72
#define V32_ANS_DETECTED			0x80
#define V32_AA_DETECTED				0x81
#define V32_AC_DETECTED				0x82
#define GSTN_CLEARDOWN_REQUESTED	0x90

	/**** Tx mode bit field definitions ****/

#define TX_LONG_RESYNC_FIELD		0x0001
#define TX_TEP_FIELD				0x0002
#define TX_V32TCM_MODE_BIT			0x0008
#define TX_V32BIS_MODE_BIT			0x0010
#define TX_V32_SPECIAL_TRAIN_BIT	0x0020
#define TX_SCRAMBLER_DISABLE_BIT	0x0040

	/**** Rx mode bit field definitions ****/

#define RX_LONG_RESYNC_FIELD		0x0001
#define RX_DETECTOR_DISABLE			0x0002
#define RX_LOS_FIELD				0x0004
#define RX_V26_MODE_FIELD			0x0018
#define RX_STU_III_BIT				0x0008
#define RX_EC_COEF_SAVE_BIT			0x0010
#define RX_EQ_COEF_SAVE_BIT			0x0020
#define RX_DESCRAMBLER_DISABLE_BIT	0x0040

	/**** Tx_Block control members ****/

#define TX_CONTROL_BLOCK \
	struct START_PTRS *start_ptrs; \
	int (*state)(struct TX_BLOCK *); \
	unsigned int state_ID; \
	unsigned int rate; \
	int scale; \
	int system_delay; \
	int *sample_head; \
	int *sample_tail; \
	int sample_len; \
	int *data_head; \
	int *data_tail; \
	int data_len; \
	int sample_counter; \
	int symbol_counter; \
	int call_counter; \
	int num_samples; \
	int mode; \
	int terminal_count; \
	int Nbits;\
	int Nmask; \
	int bit_register; \
	int bit_register_low; \
	int bit_index

#define TX_CONTROL_LEN				18
#define TX_COMMON_LEN				21
#define TX_SPARE_LEN				40		/*++ see v34p2.h, v34p3.h */

	/**** Rx_Block members ****/

#define RX_CONTROL_BLOCK \
	struct START_PTRS *start_ptrs; \
	int (*state)(struct RX_BLOCK *); \
	unsigned int state_ID; \
	int status; \
	unsigned int rate; \
	int power; \
	int *sample_head; \
	int *sample_tail; \
	int *sample_stop; \
	int sample_len; \
	int *data_head; \
	int *data_tail; \
	int data_len; \
	int sample_counter; \
	int symbol_counter; \
	int call_counter; \
	int num_samples; \
	int mode; \
	int threshold; \
	int detector_mask; \
	int digit_CP_mask; \
	int temp0; \
	int temp1; \
	int Nbits; \
	int Nmask; \
	int bit_register; \
	int bit_register_low; \
	int bit_index

#define RX_CONTROL_LEN				23
#define RX_COMMON_LEN				74
#define RX_SPARE_LEN				33		/*++ see v34p2.h, v34p3.h */

	/**** defaults ****/

#define NUM_SAMPLES 				20
#define TX_NUM_SAMPLES 				NUM_SAMPLES
#define RX_NUM_SAMPLES 				NUM_SAMPLES
#define TX_MINUS_16DBM0				32767
	
	/**** struct START_PTRS members ****/

#define TRANSMITTER_START_PTRS \
	int *Tx_block_start; \
	CIRC *Tx_sample_start; \
	CIRC *Tx_data_start; \
	CIRC *Tx_fir_start

#define RECEIVER_START_PTRS \
	int *Rx_block_start; \
	CIRC *Rx_sample_start; \
	CIRC *Rx_data_start; \
	CIRC *Rx_fir_start; \
	int *EQ_coef_start; \
	INT_PM *EC_coef_start; \
	int *encoder_start; \
	int *decoder_start; \
	CIRC *demod_delay_start; \
	CIRC *trace_back_start

struct START_PTRS {
	TRANSMITTER_START_PTRS;
	RECEIVER_START_PTRS;
	};

	/**** Transmitter functions and memory ****/

#ifdef XDAIS_API
extern void RXTX_MESI_TxBlockInit(struct START_PTRS *);
extern int RXTX_MESI_transmitter(struct START_PTRS *);
extern void RXTX_MESI_TxInitSilence(struct START_PTRS *);
extern void RXTX_MESI_TxSyncSampleBuffers(struct START_PTRS *);
#else
extern void Tx_block_init(struct START_PTRS *);
extern int transmitter(struct START_PTRS *);
extern void Tx_init_silence(struct START_PTRS *);
extern void Tx_sync_sample_buffers(struct START_PTRS *);
#endif

typedef struct TX_BLOCK 
	{
	TX_CONTROL_BLOCK;
	TX_APSK_MOD_BLOCK;
	int spare[TX_SPARE_LEN];   
	} TX_BLOCK;

	/**** Receiver functions and memory ****/

#ifdef XDAIS_API
extern void RXTX_MESI_RxBlockInit(struct START_PTRS *);
extern int RXTX_MESI_receiver(struct START_PTRS *);
extern void RXTX_MESI_RxInitIdle(struct START_PTRS *);
#else
extern void Rx_block_init(struct START_PTRS *);
extern int receiver(struct START_PTRS *);
extern void Rx_init_idle(struct START_PTRS *);
#endif

typedef struct RX_BLOCK 
	{
	RX_CONTROL_BLOCK;
	RX_APSK_DMOD_BLOCK;
	int spare[RX_SPARE_LEN];
	} RX_BLOCK;

	/**** macros ****/

#define get_EQ_MSE(ptr) ptr->EQ_MSE
#define ABS(a) ((a)>=0?(a):-(a))
#define LABS(a) ABS(a)
#define MPY(a,b) (int)(((((long)(a)*(long)(b)))>>15))
#define MPYR(a,b) (int)(((((long)(a)*(long)(b))+0x4000l)>>15))
#define CIRC_WRAP(ptr, base, len) \
	{ \
	if (ptr>=base+len) ptr-=len; \
	else if (ptr<base) ptr+=len; \
	}
#define CIRC_MODIFY(ptr, mod, base, len) \
	{ \
	ptr+=(mod); \
	CIRC_WRAP(ptr,base,len) \
	}
#define CIRC_INCREMENT(ptr, mod, base, len) \
	{ \
	ptr+=(mod); \
	if (ptr>=base+len) ptr-=len; \
	}
#define CIRC_DECREMENT(ptr, mod, base, len) \
	{ \
	ptr+=(mod); \
	if (ptr<base) ptr+=len; \
	}

#define positive_saturate(a,b) if (b>32767l) a=0x7fff
#define negative_saturate(a,b) if (b<-32768l) a=0x8000
#define saturate(a,b) if (b>32767l) a=0x7fff; \
		  					 if (b<-32768l) a=0x8000

/****************************************************************************/
/* Macro: SUBC(NUM,DENOM)																	*/
/* Emulates DSP Subtract Conditionally. NUM must be a long, and denom 		*/
/* must be less than NUM. NUM is not preserved. For 16 bit integer division*/
/* you call this macro 16 times.															*/
/* Results:																						*/
/*		the quotient is in the lower 16 bits of NUM									*/
/*		the remainder is in the upper 16 bits of NUM									*/
/****************************************************************************/

#define SUBC(NUM,DENOM) \
		{if ((NUM-(((long)DENOM)<<15))>=0) \
			NUM=((NUM-(((long)DENOM)<<15))<<1)+1; \
		else \
			NUM=NUM<<1;}

/****************************************************************************/
/* Macros to access Tx_block and Rx_block control section members.			*/
/****************************************************************************/

#define set_Tx_num_samples(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->num_samples=A
#define set_Tx_rate(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->rate=A
#define set_Tx_scale(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->scale=A
#define set_system_delay(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->system_delay=A
#define set_Tx_terminal_count(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->terminal_count=A
#define set_Tx_mode(A, PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->mode=A

#define get_Tx_state_ID(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->state_ID
#define get_Tx_terminal_count(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->terminal_count
#define get_Tx_mode(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->mode
#define get_Tx_sample_head(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->sample_head
#define get_Tx_sample_tail(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->sample_tail
#define get_Tx_data_head(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->data_head
#define get_Tx_data_tail(PTR) \
	((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->data_tail


#define set_Rx_num_samples(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->num_samples=A
#define set_Rx_rate(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->rate=A
#define set_Rx_mode(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->mode=A
#define get_Rx_state_ID(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->state_ID
#define get_Rx_mode(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->mode
#define get_Rx_status(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->status
#define get_Rx_sample_head(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->sample_head
#define get_Rx_sample_tail(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->sample_tail
#define get_Rx_data_head(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->data_head
#define get_Rx_data_tail(PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->data_tail

#define set_EQ_2mu(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->EQ_2mu=A
#define set_EC_2mu(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->EC_2mu=A
#define set_agc_K(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->agc_K=A
#define set_loop_K1(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->loop_K1=A
#define set_loop_K2(A, PTR) \
	((struct RX_BLOCK *)((struct START_PTRS *)PTR)->Rx_block_start)->loop_K2=A

/****************************************************************************/

#endif	/* inclusion */


