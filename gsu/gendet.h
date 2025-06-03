/****************************************************************************/
/* File: "gendet.h" 														*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for gendet.		*/
/****************************************************************************/

#ifndef GENDET_INCLUSION_
#define GENDET_INCLUSION_

	/**** state_ID definitions ****/

#define TX_TONE_GEN_ID 				0x1200
#define TX_CNG_ID 					0x1210
#define TX_CED_ID 					0x1211
#define TX_ECSD_ID 					0x1212
#define TX_CALL_PROGRESS_ID			0x1220
#define TX_DIALTONE_ID				0x1221
#define TX_RINGBACK_ID				0x1222
#define TX_REORDER_ID				0x1223
#define TX_BUSY_ID					0x1224

#define RX_ENERGY_DET_ID 			0x1000
#define RX_SIG_ANALYSIS_ID 			0x1100
#define RX_TONE_ID 					0x1200
#define RX_CNG_ID 					0x1210
#define RX_CED_ID 					0x1211
#define RX_ECSD_ID 					0x1212
#define RX_TEP_1700_ID				0x1213
#define RX_TEP_1800_ID				0x1214
#define RX_CALL_PROGRESS_ID			0x1220
#define RX_DIALTONE_ID				0x1221
#define RX_RINGBACK_ID				0x1222
#define RX_REORDER_ID				0x1223
#define RX_BUSY_ID					0x1224

#define RX_V32_AUTOMODE_ID 			0x1500
#define RX_V32_CED_ID 				0x1501
#define RX_V32_ECSD_ID 				0x1502
#define RX_V32_AA_ID				0x1504
#define RX_V32_AC_ID				0x1508
#define RX_V32_USB1_ID				0x1510

	/**** detector_mask mask bit definitions ****/

#define AUTO_DETECT_MASK 			0x0001
#define V21_CH1_MASK 				0x0002
#define V21_CH2_MASK 				0x0004
#define V22_MASK 					0x0008
#define V27_2400_MASK 				0x0010
#define V27_4800_MASK 				0x0020
#define V29_MASK 					0x0040
#define V17_MASK 					0x0080
#define CED_MASK 					0x0100
#define CNG_MASK 					0x0200
#define TEP_MASK 					0x0400
#define CALL_PROGRESS_MASK			0x0800
#define V32_AUTOMODE_MASK 			0x1000

#define FAX_DETECT_MASK (AUTO_DETECT_MASK|CED_MASK \
								|V21_CH2_MASK \
								|TEP_MASK \
								|V27_2400_MASK|V27_4800_MASK \
								|V17_MASK \
								|V29_MASK)
#define DATA_DETECT_MASK (AUTO_DETECT_MASK|V22_MASK)

	/**** digit_CP_mask mask bit definitions ****/

#define DTMF_MASK					0x0001
#define R1_MASK						0x0002
#define R2F_MASK					0x0004
#define R2B_MASK					0x0008

	/**** memory ****/

struct TX_GEN_BLOCK
	{
	TX_CONTROL_BLOCK;
	int frequency1;
	unsigned int vco_memory1;
	int scale1;
	int frequency2;
	unsigned int vco_memory2;
	int scale2;
	unsigned int cad_memory;
	unsigned int cad_period;
	unsigned int on_time;
	unsigned int rev_memory; 
	unsigned int rev_period;
	int *digit_ptr; 
	};

struct RX_DET_BLOCK
	{
	RX_CONTROL_BLOCK;
	int SNR_est_coef;
	int SNR_thr_coef;
	int broadband_level;
	int level_350;
	int level_460;
	int level_500;
	int level_600;
	int level_980;
	int level_1000;
	int level_1100;
	int level_1180;
	int level_1200;
	int level_1650;
	int level_1700;
	int level_1750;
	int level_1800;
	int level_1850;
	int level_2100;
	int level_2225;
	int level_2250;
	int level_2400;
	int level_2600;
	int level_2850;
	int level_2900;
	int level_3000;
	int filter_mask_low;
	int filter_mask_high;
	unsigned int dialtone_counter;
	unsigned int ringback_counter;
	unsigned int reorder_counter;
	int v32_automode_counter;
	int (*digit_detector)(struct RX_BLOCK *);
	int *digit_ptr; 
	int Pbb;
	int digit_ID;
	int GF_len;
	int GF_counter;
	int num_filters;
	int max_row;
	int max_col;
	int digit_threshold;
	};

	/**** definitions for Brazilian call progress tones ****/

#ifdef BRAZIL_PSTN
#define level_425					level_350
#define CP_detect_counter			dialtone_counter
#define CP_corr_register			ringback_counter
#define CP_corr_register_low		reorder_counter

#define CP_SAMPLE_PERIOD			25		/* 250 msec at 100 Hz */
#define DIALTONE_CORR_PATTERN		0x01ff	/* continuous tone */
#define RINGBACK_CORR_PATTERN		0x01e0	/* 1 sec. ON, 4 sec. OFF */
#define BUSY_CORR_PATTERN			0x0154	/* 0.25 sec. ON, 0.25 sec. OFF */
#define REORDER_CORR_PATTERN		0x0110	/* 0.25 sec. ON, 0.75 sec. OFF */
#define CORR_MSB_POSITION			0x0100	/* MSB position for correlation pattern */
#endif

	/**** functions ****/

#ifdef XDAIS_API
extern void GEN_MESI_TxInitTone(int, struct START_PTRS *);
extern void GEN_MESI_TxInitAA(struct START_PTRS *);
extern void GEN_MESI_TxInitCED(struct START_PTRS *);
extern void GEN_MESI_TxInitCNG(struct START_PTRS *);
extern void GEN_MESI_TxInitECSD(struct START_PTRS *);
extern void GEN_MESI_TxInitDialtone(struct START_PTRS *);
extern void GEN_MESI_TxInitRingback(struct START_PTRS *);
extern void GEN_MESI_TxInitReorder(struct START_PTRS *);
extern void GEN_MESI_TxInitBusy(struct START_PTRS *);

extern void DET_MESI_RxInitDetector(struct START_PTRS *);
extern void DET_MESI_setRxDetectorMask(int, struct START_PTRS *);
extern void DET_MESI_setRxDigitCPmask(int, struct START_PTRS *);

#else
extern void Tx_init_tone(int, struct START_PTRS *);
extern void Tx_init_AA(struct START_PTRS *);
extern void Tx_init_AC(struct START_PTRS *);
extern void Tx_init_CED(struct START_PTRS *);
extern void Tx_init_CNG(struct START_PTRS *);
extern void Tx_init_ECSD(struct START_PTRS *);
extern void Tx_init_dialtone(struct START_PTRS *);
extern void Tx_init_ringback(struct START_PTRS *);
extern void Tx_init_reorder(struct START_PTRS *);
extern void Tx_init_busy(struct START_PTRS *);

extern void Rx_init_detector(struct START_PTRS *);
extern void set_Rx_detector_mask(int, struct START_PTRS *);
extern void set_Rx_digit_CP_mask(int, struct START_PTRS *);
#endif

/****************************************************************************/

#endif	/* inclusion */

