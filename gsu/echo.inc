;****************************************************************************
;* Filename: echo.inc
;* Date: 09-10-99
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Definitions for echo canceller algorithms.
;****************************************************************************

ACQ_EC_2MU			 		.set	16384 
EC_TRAIN_DISABLED	  		.set	0
NEC_PEAK_OFFSET				.set	8
FEC_PEAK_OFFSET				.set	24
FEC_RTD_SCALE		  		.set	27307   ;* 3.3333/4

;****************************************************************************
