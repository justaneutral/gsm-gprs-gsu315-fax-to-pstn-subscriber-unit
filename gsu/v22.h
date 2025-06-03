/****************************************************************************/
/* File: "v22.h" 															*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for v22.		*/
/****************************************************************************/

#ifndef V22_INCLUSION_
#define V22_INCLUSION_

	/**** state_ID definitions ****/

#define TX_V22_MOD_ID				0x2200

#define TX_V22A_SILENCE1_ID			0x2201
#define TX_V22A_ANS_ID				0x2202
#define TX_V22A_SILENCE2_ID			0x2203
#define TX_V22A_UB1_ID				0x2204
#define TX_V22A_S1_ID				0x2205
#define TX_V22A_SCR1_ID				0x2206
#define TX_V22A_SB1_R2_ID			0x2207
#define TX_V22A_MESSAGE_ID			(0x2200|MESSAGE_ID)

#define TX_V22C_SILENCE_ID			0x2281
#define TX_V22C_S1_ID				0x2282
#define TX_V22C_SCR1_ID				0x2283
#define TX_V22C_SB1_R2_ID			0x2284
#define TX_V22C_MESSAGE_ID			(0x2280|MESSAGE_ID)

#define RX_V22A_START_DETECT_ID 	0x2201
#define RX_V22A_TRAIN_LOOPS_ID		0x2202
#define RX_V22A_TRAIN_EQ_ID 		0x2203
#define RX_V22A_MESSAGE_ID 			(0x2200|MESSAGE_ID)
#define RX_V22A_RC_RESPOND_ID 		0x2205
#define RX_V22A_RC_INITIATE_ID 		0x2206

#define RX_V22C_START_DETECT_ID 	0x2281
#define RX_V22C_TRAIN_LOOPS_ID		0x2282
#define RX_V22C_TRAIN_EQ_ID 		0x2283
#define RX_V22C_MESSAGE_ID 			(0x2280|MESSAGE_ID)
#define RX_V22C_RC_RESPOND_ID 		0x2285
#define RX_V22C_RC_INITIATE_ID 		0x2286
										  
	/**** memory ****/

struct TX_ANS_BLOCK
		{
		TX_CONTROL_BLOCK;
		int frequency;
		unsigned int osc_memory;
		int osc_scale;
		};

struct TX_V22_BLOCK
		{
		TX_CONTROL_BLOCK;
		TX_APSK_MOD_BLOCK;
		int Scounter;
		unsigned int guard_memory;
		int guard_scale;
		};

struct RX_V22_BLOCK
		{
		RX_CONTROL_BLOCK;
		RX_APSK_DMOD_BLOCK;
		int data_Q1;		
		int pattern_detect;      	
		int S1_memory;		
		int S1_nm1;			
		int Dcounter;		
		int SNR_est_coef;
		int SNR_thr_coef;
		int F1800;
		};

	/**** functions ****/

#ifdef XDAIS_API
extern void V22_MESI_TxInitV22A(struct START_PTRS *);
extern void V22_MESI_TxInitV22A_ANS(struct START_PTRS *);
extern void V22_MESI_TxV22ARetrain(struct START_PTRS *);
extern void V22_MESI_TxInitV22C(struct START_PTRS *);
extern void V22_MESI_TxV22CRetrain(struct START_PTRS *);
#else
extern void Tx_init_v22A(struct START_PTRS *);
extern void Tx_init_v22A_ANS(struct START_PTRS *);
extern void Tx_v22A_retrain(struct START_PTRS *);
extern void Tx_init_v22C(struct START_PTRS *);
extern void Tx_v22C_retrain(struct START_PTRS *);
#endif

/****************************************************************************/

#endif	/* inclusion */
	
