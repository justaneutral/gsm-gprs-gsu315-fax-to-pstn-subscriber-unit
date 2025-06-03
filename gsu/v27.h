/****************************************************************************/
/* File: "v27.h" 															*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for v22.		*/
/****************************************************************************/

#ifndef V27_INCLUSION_
#define V27_INCLUSION_

	/**** state_ID definitions ****/

#define TX_V27_MOD_ID 				0x2700
#define TX_V27_SEGMENT1_ID 			0x2701
#define TX_V27_SEGMENT2_ID 			0x2702
#define TX_V27_SEGMENT3_ID 			0x2703
#define TX_V27_SEGMENT4_ID 			0x2704
#define TX_V27_SEGMENT5_ID 			0x2705
#define TX_V27_MESSAGE_ID 			(0x2700|MESSAGE_ID)
#define TX_V27_SEGMENTA_ID 			0x2707
#define TX_V27_SEGMENTB_ID 			0x2708

#define RX_V27_TRAIN_LOOPS_ID 		0x2701
#define RX_V27_DETECT_EQ_ID 		0x2702
#define RX_V27_TRAIN_EQ_ID 			0x2703
#define RX_V27_SCR1_ID 				0x2704
#define RX_V27_MESSAGE_ID 			(0x2700|MESSAGE_ID)

	/**** mode definitions ****/

#define STARTUP 					0
#define TURNAROUND 					1

	/**** memory ****/

struct TX_V27_BLOCK
		{
		TX_CONTROL_BLOCK;
		TX_APSK_MOD_BLOCK;
		int Sguard;			
		int Sinv;			
		};

struct RX_V27_BLOCK
		{
		RX_CONTROL_BLOCK;
		RX_APSK_DMOD_BLOCK;
		int Dguard;
		int Dinv;
		int hard_sym_nm1;
		int train_EQ_timeout;
		};

	/**** functions ****/

#ifdef XDAIS_API
extern void V27_MESI_TxInitV27(struct START_PTRS *);
extern void V27_MESI_RxInitV27(struct START_PTRS *);
#else
extern void Tx_init_v27(struct START_PTRS *);
extern void Rx_init_v27(struct START_PTRS *);
#endif

	/**** macros ****/

#define Tx_init_v27_2400(ptrs) \
				{set_Tx_rate(2400,ptrs); \
				enable_Tx_v27_TEP(ptrs); \
			 	Tx_init_v27(ptrs);}
#define Tx_init_v27_2400_TEP(ptrs) Tx_init_v27_2400(ptrs)
#define Tx_init_v27_4800(ptrs) \
				{set_Tx_rate(4800,ptrs); \
				enable_Tx_v27_TEP(ptrs); \
			 	Tx_init_v27(ptrs);}
#define Tx_init_v27_4800_TEP(ptrs) Tx_init_v27_4800(ptrs)

#define enable_Tx_v27_startup_seq(ptrs) \
	set_Tx_mode(~TX_LONG_RESYNC_FIELD&get_Tx_mode(ptrs),ptrs)
#define enable_Tx_v27_turnaround_seq(ptrs) \
	set_Tx_mode(TX_LONG_RESYNC_FIELD|get_Tx_mode(ptrs),ptrs)
#define enable_Tx_v27_TEP(ptrs) \
	{set_Tx_mode(TX_TEP_FIELD|get_Tx_mode(ptrs),ptrs); \
	Tx_init_v27(ptrs);}
#define disable_Tx_v27_TEP(ptrs) \
	{set_Tx_mode(~TX_TEP_FIELD&get_Tx_mode(ptrs),ptrs); \
	Tx_init_v27(ptrs);}

#define Rx_init_v27_2400(ptrs) \
				{set_Rx_rate(2400,ptrs); \
			 	Rx_init_v27(ptrs);}
#define Rx_init_v27_4800(ptrs) \
				{set_Rx_rate(4800,ptrs); \
			 	Rx_init_v27(ptrs);}

#define enable_Rx_v27_startup_seq(ptrs) \
	set_Rx_mode(~RX_LONG_RESYNC_FIELD&get_Rx_mode(ptrs),ptrs)
#define enable_Rx_v27_turnaround_seq(ptrs) \
	set_Rx_mode(RX_LONG_RESYNC_FIELD|get_Rx_mode(ptrs),ptrs)

#define enable_Tx_v27_resync(ptrs) enable_Tx_v27_turnaround_seq(ptrs)
#define enable_Rx_v27_resync(ptrs) enable_Rx_v27_turnaround_seq(ptrs)

/****************************************************************************/

#endif	/* inclusion */

