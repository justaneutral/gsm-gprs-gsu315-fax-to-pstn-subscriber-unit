;****************************************************************************
;* Filename: fsk.inc
;* Date: 04-04-00
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

;++++#ifndef MESI_INTERNAL 03-22-2001
;	;**** FSK modulator common offsets ****
;
;	.asg  TX_CONTROL_LEN,x
;	INT	TxFSK_coef_ptr 
;	INT	TxFSK_interpolate 
;	INT	TxFSK_decimate 
;	INT	TxFSK_carrier 
;	INT	TxFSK_tone_scale 
;	INT	TxFSK_vco_memory 
;	INT	TxFSK_frequency
;	INT	TxFSK_frequency_shift
;TX_FSK_LEN	  				.set	x-TX_CONTROL_LEN
;
;	;****FSK demodulator common offsets ****
;
;	.asg  RX_CONTROL_LEN,x
;	INT	RxFSK_mark_coef
;	INT	RxFSK_space_coef
;	INT	RxFSK_sym
;	INT	RxFSK_coef_len
;	INT	RxFSK_coef_ptr
;	INT	RxFSK_interpolate
;	INT	RxFSK_decimate
;	INT	RxFSK_sym_nm1
;	INT	RxFSK_sym_hat
;	INT	RxFSK_sym_hat_nm2
;	INT	RxFSK_sym_clk_memory
;	INT	RxFSK_baud_counter
;	INT	RxFSK_sym_level
;	INT	RxFSK_LOS_threshold
;	INT	RxFSK_LOS_memory
;RX_FSK_LEN	  				.set	x-RX_CONTROL_LEN
;++++#else   MESI_INTERNAL 03-22-2001
;++++#endif  MESI_INTERNAL 03-22-2001

	;**** FSK demodulator parameters ****

SLICE1  					.set	8192
FSK_LOS_LIMIT	 			.set	4		;* 4 consecutive LOS events
FSK_LOS_THRESHOLD			.set	16384	;* 32768*10exp(-6 dB/20)

;****************************************************************************

