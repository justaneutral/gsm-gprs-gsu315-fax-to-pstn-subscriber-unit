/****************************************************************************/
/* File: "v32.h" 															*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: Miller Engineering Services, Inc. (MESi)						*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/* E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for v32.		*/
/****************************************************************************/

#ifndef V32_INCLUSION_
#define V32_INCLUSION_

	/**** state_ID definitions ****/

#define TX_V32A_MOD_ID 				0x3200
#define TX_V32A_SILENCE1_ID 		0x3201
#define TX_V32A_ANS_ID 				0x3202
#define TX_V32A_SILENCE2_ID 		0x3203
#define TX_V32A_AC1_ID 				0x3204
#define TX_V32A_CA_ID 				0x3205
#define TX_V32A_AC2_ID 				0x3206
#define TX_V32A_SILENCE3_ID 		0x3207
#define TX_V32A_SPECIAL_TRN1_ID 	0x3208
#define TX_V32A_S1_ID 				0x3209
#define TX_V32A_SBAR1_ID 			0x320a
#define TX_V32A_TRN1_ID 			0x320b
#define TX_V32A_R1_ID 				0x320c
#define TX_V32A_SILENCE4_ID 		0x320d
#define TX_V32A_S2_ID 				0x320e
#define TX_V32A_SBAR2_ID 			0x320f
#define TX_V32A_TRN2_ID 			0x3210
#define TX_V32A_R3_ID 				0x3211
#define TX_V32A_E_ID 				0x3212
#define TX_V32A_B1_ID 				0x3213
#define TX_V32A_MESSAGE_ID 			(0x3200|MESSAGE_ID)
#define TX_V32A_RC_PREAMBLE_ID 		0x3215
#define TX_V32A_R4_ID 				0x3216
#define TX_V32A_R5_ID 				0x3217

#define TX_V32C_MOD_ID 				0x3280
#define TX_V32C_SILENCE1_ID 		0x3281
#define TX_V32C_AA_ID 				0x3282
#define TX_V32C_CC_ID 				0x3283
#define TX_V32C_SILENCE2_ID 		0x3284
#define TX_V32C_S_DELAY_ID 			0x3285
#define TX_V32C_SPECIAL_TRN1_ID 	0x3286
#define TX_V32C_S1_ID 				0x3287
#define TX_V32C_SBAR1_ID 			0x3288
#define TX_V32C_TRN1_ID 			0x3289
#define TX_V32C_R2_ID 				0x328a
#define TX_V32C_E_ID 				0x328b
#define TX_V32C_B1_ID 				0x328c
#define TX_V32C_MESSAGE_ID 			(0x3280|MESSAGE_ID)
#define TX_V32C_RC_PREAMBLE_ID 		0x328e
#define TX_V32C_R4_ID 				0x328f
#define TX_V32C_R5_ID 				0x3290

#define RX_V32A_DETECT_AA_ID 		0x3201
#define RX_V32A_DETECT_AACC_ID 		0x3202
#define RX_V32A_DETECT_CC_END_ID 	0x3203
#define RX_V32A_TRAIN_EC_ID 		0x3204
#define RX_V32A_S_DETECT_ID 		0x3205
#define RX_V32A_TRAIN_LOOPS_ID 		0x3206
#define RX_V32A_DETECT_EQ_ID 		0x3207
#define RX_V32A_TRAIN_EQ_ID 		0x3208
#define RX_V32A_RATE_ID 			0x3209
#define RX_V32A_B1_ID 				0x320a
#define RX_V32A_MESSAGE_ID 			(0x3200|MESSAGE_ID)
#define RX_V32A_RC_PREAMBLE_ID 		0x320c
#define RX_V32A_R4_ID				0x320d
#define RX_V32A_R5_ID				0x320e

#define RX_V32C_DETECT_AC_ID 		0x3281
#define RX_V32C_DETECT_ACCA_ID 		0x3282
#define RX_V32C_DETECT_CAAC_ID 		0x3283
#define RX_V32C_DETECT_AC_END_ID 	0x3284
#define RX_V32C_TRAIN_EC_ID 		0x3285
#define RX_V32C_S_DETECT_ID 		0x3286
#define RX_V32C_TRAIN_LOOPS_ID 		0x3287
#define RX_V32C_DETECT_EQ_ID 		0x3288
#define RX_V32C_TRAIN_EQ_ID 		0x3289
#define RX_V32C_RATE_ID 			0x328a
#define RX_V32C_B1_ID 				0x328b
#define RX_V32C_MESSAGE_ID 			(0x3280|MESSAGE_ID)
#define RX_V32C_RC_PREAMBLE_ID 		0x328d
#define RX_V32C_R4_ID				0x328e
#define RX_V32C_R5_ID				0x328f

	/**** mode definitions ****/

#define V32TCM_MODE_BIT				0x0008
#define V32BIS_MODE_BIT				0x0010
#define V32_SPECIAL_TRAIN_BIT		0x0020
#define V32TCM_MODE					V32TCM_MODE_BIT
#define V32BIS_MODE					(V32BIS_MODE_BIT|V32TCM_MODE_BIT)
#define GSTN_CLEARDOWN_RATE_PATTERN	0x0111

	/**** memory ****/

struct TX_ECSD_BLOCK
		{
		TX_CONTROL_BLOCK;
		int frequency;
		unsigned int osc_memory;
		int osc_scale;
		unsigned int rev_memory; 
		unsigned int rev_period;
		};

struct TX_V32_BLOCK
		{
		TX_CONTROL_BLOCK;
		TX_APSK_MOD_BLOCK;
		int (*scrambler_ptr)(int, struct TX_BLOCK *);
		int rate_pattern;
		int max_rate;
		};

struct RX_V32_BLOCK
		{
		RX_CONTROL_BLOCK;
		RX_APSK_DMOD_BLOCK;
		int data_Q1;		
		int (*descrambler_ptr)(int, struct RX_BLOCK *);
		int rate_pattern;
		int pattern_detect;
		int Dcounter;
		int RCcounter;
		};

	/**** functions ****/

#ifdef XDAIS_API
extern void V32_MESI_TxInitV32A(struct START_PTRS *);
extern void V32_MESI_TxV32ARetrain(struct START_PTRS *);
extern void V32_MESI_TxV32ARenegotiate(struct START_PTRS *);
extern void V32_MESI_TxInitV32A_ANS(struct START_PTRS *);
extern void V32_MESI_TxInitV32C(struct START_PTRS *);
extern void V32_MESI_TxV32CRetrain(struct START_PTRS *);
extern void V32_MESI_TxV32CRenegotiate(struct START_PTRS *);
extern void V32_MESI_setTxV32RatePattern(struct START_PTRS *);
#else
extern void Tx_init_v32A(struct START_PTRS *);
extern void Tx_v32A_retrain(struct START_PTRS *);
extern void Tx_v32A_renegotiate(struct START_PTRS *);
extern void Tx_init_v32A_ANS(struct START_PTRS *);
extern void Tx_init_v32C(struct START_PTRS *);
extern void Tx_v32C_retrain(struct START_PTRS *);
extern void Tx_v32C_renegotiate(struct START_PTRS *);
extern void set_Tx_v32_rate_pattern(struct START_PTRS *);
#endif

	/**** v32 macros ****/

#define Tx_v32_enable_special_TRN(ptrs) \
					set_Tx_mode( (V32_SPECIAL_TRAIN_BIT|get_Tx_mode(ptrs)), ptrs)

#define Tx_v32_enable_TCM(ptrs) \
					set_Tx_mode( (V32TCM_MODE | get_Tx_mode(ptrs)), ptrs)

#define Tx_v32_disable_TCM(ptrs) \
					set_Tx_mode( (~V32TCM_MODE & get_Tx_mode(ptrs)), ptrs)

	/**** v32bis macros ****/

#define Tx_init_v32bisA(rate, ptrs) \
					{set_Tx_rate(rate, ptrs); \
					set_Tx_mode( (V32BIS_MODE | get_Tx_mode(ptrs)), ptrs); \
					Tx_init_v32A(ptrs);}

#define Tx_init_v32bisA_ANS(rate, ptrs) \
					{set_Tx_rate(rate, ptrs); \
					set_Tx_mode( (V32BIS_MODE | get_Tx_mode(ptrs)), ptrs); \
					Tx_init_v32A_ANS(ptrs);}

#define Tx_init_v32bisC(rate, ptrs) \
					{set_Tx_rate(rate, ptrs); \
					set_Tx_mode( (V32BIS_MODE | get_Tx_mode(ptrs)), ptrs); \
					Tx_init_v32C(ptrs);}

	/**** general macros ****/

/*#define get_EC_MSE(ptr) ptr->EC_MSE*/
#define GSTN_cleardownA(PTR) \
			if ((((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->mode&V32BIS_MODE_BIT)==0) \
				Tx_v32A_retrain(PTR); \
			else \
				Tx_v32A_renegotiate(PTR)

#define GSTN_cleardownC(PTR) \
			if ((((struct TX_BLOCK *)((struct START_PTRS *)PTR)->Tx_block_start)->mode&V32BIS_MODE_BIT)==0) \
				Tx_v32C_retrain(PTR); \
			else \
				Tx_v32C_renegotiate(PTR)

/****************************************************************************/

#endif	/* inclusion */

