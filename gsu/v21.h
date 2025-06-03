/****************************************************************************/
/* File: "v21.h" 															*/
/* Date: 09-18-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: Structure definitions and extern references for v21.		*/
/****************************************************************************/

#ifndef V21_INCLUSION_
#define V21_INCLUSION_

	/**** state_ID definitions ****/

#define TX_V21_CH1_MESSAGE_ID 		(0x2100|MESSAGE_ID)
#define TX_V21_CH2_MESSAGE_ID 		(0x2180|MESSAGE_ID)

#define RX_V21_CH1_MESSAGE_ID 		(0x2100|MESSAGE_ID)
#define RX_V21_CH2_MESSAGE_ID 		(0x2180|MESSAGE_ID)

	/**** FSK modulator common structure ****/

struct TX_V21_BLOCK
	{
	TX_CONTROL_BLOCK;
	TX_FSK_MOD_BLOCK;
	};


	/**** FSK demodulator common structure ****/

struct RX_V21_BLOCK
	{
	RX_CONTROL_BLOCK;
	RX_FSK_DMOD_BLOCK;
	};

	/**** functions ****/

#ifdef XDAIS_API
extern void V21_MESI_TxInitV21Ch1(struct START_PTRS *);
extern void V21_MESI_TxInitV21Ch2(struct START_PTRS *);
extern int V21_MESI_v21Modulator(struct TX_BLOCK *);
extern void V21_MESI_RxInitV21Ch1(struct START_PTRS *);
extern void V21_MESI_RxInitV21Ch2(struct START_PTRS *);
extern int V21_MESI_v21Demodulator(struct RX_BLOCK *);
#else
extern void Tx_init_v21_ch1(struct START_PTRS *);
extern void Tx_init_v21_ch2(struct START_PTRS *);
extern int v21_modulator(struct TX_BLOCK *);
extern void Rx_init_v21_ch1(struct START_PTRS *);
extern void Rx_init_v21_ch2(struct START_PTRS *);
extern int v21_demodulator(struct RX_BLOCK *);
#endif

/****************************************************************************/

#endif	/* inclusion */

