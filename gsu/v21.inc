;****************************************************************************
;* Filename: v21.inc
;* Date: 03-27-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****

TX_V21_CH1_MESSAGE_ID 		.set	(2100h|MESSAGE_ID)
TX_V21_CH2_MESSAGE_ID 		.set	(2180h|MESSAGE_ID)

RX_V21_CH1_MESSAGE_ID 		.set	(2100h|MESSAGE_ID)
RX_V21_CH2_MESSAGE_ID 		.set	(2180h|MESSAGE_ID)

;++++#ifndef MESI_INTERNAL 03-22-2001
;TX_COMMON_LEN	  			.set	TX_FSK_LEN
;RX_COMMON_LEN	  			.set	RX_FSK_LEN
;++++#else   MESI_INTERNAL 03-22-2001
;++++#endif  MESI_INTERNAL 03-22-2001

;****************************************************************************
