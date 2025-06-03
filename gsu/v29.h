/****************************************************************************/
/* File: "v29.h" 															*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for v29.		*/
/****************************************************************************/

#ifndef V29_INCLUSION_
#define V29_INCLUSION_

	/**** state_ID definitions ****/

#define TX_V29_MOD_ID 				0x2900
#define TX_V29_TEP_ID 				0x2901
#define TX_V29_SEGMENT1_ID 			0x2902
#define TX_V29_SEGMENT2_ID 			0x2903
#define TX_V29_SEGMENT3_ID 			0x2904
#define TX_V29_SEGMENT4_ID 			0x2905
#define TX_V29_MESSAGE_ID  			(0x2900|MESSAGE_ID)
/*++++#ifndef MESI_INTERNAL 03-20-2001*/
/*#define TX_V29_SEGMENT6_ID			0x9607*/
/*++++#else   MESI_INTERNAL 03-20-2001*/
#define TX_V29_SEGMENT6_ID			0x2907
/*++++#endif  MESI_INTERNAL 03-20-2001*/


#define RX_V29_TRAIN_LOOPS_ID 		0x2901
#define RX_V29_DETECT_EQ_ID 		0x2902
#define RX_V29_TRAIN_EQ_ID 			0x2903
#define RX_V29_SCR1_ID 				0x2904
#define RX_V29_MESSAGE_ID 			(0x2900|MESSAGE_ID)

	/**** memory ****/

struct TX_V29_BLOCK
		{
		TX_CONTROL_BLOCK;
		TX_APSK_MOD_BLOCK;
		int amp_acc;
		};

struct RX_V29_BLOCK
		{
		RX_CONTROL_BLOCK;
		RX_APSK_DMOD_BLOCK;
		int data_Q1;		
		int hard_sym_nm1;
		};

	/**** functions ****/

#ifdef XDAIS_API
extern void V29_MESI_TxInitV29(struct START_PTRS *);
#else
extern void Tx_init_v29(struct START_PTRS *);
#endif

	/**** macros ****/

#define Tx_init_v29_9600(ptrs) \
				{set_Tx_rate(9600,ptrs); \
			 	Tx_init_v29(ptrs);}
#define Tx_init_v29_9600_TEP(ptrs) \
				{set_Tx_rate(9600,ptrs); \
				set_Tx_mode(TX_TEP_FIELD|get_Tx_mode(ptrs),ptrs); \
			 	Tx_init_v29(ptrs);}
#define Tx_init_v29_7200(ptrs) \
				{set_Tx_rate(7200,ptrs); \
			 	Tx_init_v29(ptrs);}
#define Tx_init_v29_7200_TEP(ptrs) \
				{set_Tx_rate(7200,ptrs); \
				set_Tx_mode(TX_TEP_FIELD|get_Tx_mode(ptrs),ptrs); \
			 	Tx_init_v29(ptrs);}
#define Tx_init_v29_4800(ptrs) \
				{set_Tx_rate(4800,ptrs); \
			 	Tx_init_v29(ptrs);}
#define Tx_init_v29_4800_TEP(ptrs) \
				{set_Tx_rate(4800,ptrs); \
				set_Tx_mode(TX_TEP_FIELD|get_Tx_mode(ptrs),ptrs); \
			 	Tx_init_v29(ptrs);}

#define enable_Tx_v29_TEP(ptrs) \
				{set_Tx_mode(TX_TEP_FIELD|get_Tx_mode(ptrs),ptrs); \
				Tx_init_v29(ptrs);}
#define disable_Tx_v29_TEP(ptrs) \
				{set_Tx_mode(~TX_TEP_FIELD&get_Tx_mode(ptrs),ptrs); \
				Tx_init_v29(ptrs);}

#define enable_Tx_v29_long_train(ptrs) \
	set_Tx_mode(~RX_LONG_RESYNC_FIELD&get_Tx_mode(ptrs),ptrs)
#define enable_Tx_v29_resync(ptrs) \
	set_Tx_mode(RX_LONG_RESYNC_FIELD|get_Tx_mode(ptrs),ptrs)
#define enable_Rx_v29_long_train(ptrs) \
	set_Rx_mode(~RX_LONG_RESYNC_FIELD&get_Rx_mode(ptrs),ptrs)
#define enable_Rx_v29_resync(ptrs) \
	set_Rx_mode(RX_LONG_RESYNC_FIELD|get_Rx_mode(ptrs),ptrs)

/****************************************************************************/

#endif	/* inclusion */

