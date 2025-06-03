;****************************************************************************
;* Filename: v27.inc
;* Date: 04-01-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****

TX_V27_MOD_ID				.set	2700h
TX_V27_SEGMENT1_ID			.set	2701h
TX_V27_SEGMENT2_ID			.set	2702h
TX_V27_SEGMENT3_ID			.set	2703h
TX_V27_SEGMENT4_ID			.set	2704h
TX_V27_SEGMENT5_ID			.set	2705h
TX_V27_MESSAGE_ID 			.set	(2700h|MESSAGE_ID)
TX_V27_SEGMENTA_ID			.set	2707h
TX_V27_SEGMENTB_ID			.set	2708h

RX_V27_TRAIN_LOOPS_ID 		.set	2701h
RX_V27_DETECT_EQ_ID 		.set	2702h
RX_V27_TRAIN_EQ_ID 			.set	2703h
RX_V27_SCR1_ID 				.set	2704h
RX_V27_MESSAGE_ID 			.set	(2700h|MESSAGE_ID)
			
	;**** Tx_v27 spare offsets ****

	.asg  TX_CONTROL_LEN+TX_COMMON_LEN,x
	INT	Sguard
	INT	Sinv
TX_V27_WORKSPACE_LEN	  	.set	x-TX_COMMON_LEN

	;**** Rx_v27 spare offsets ****

	.asg  RX_CONTROL_LEN+RX_COMMON_LEN,x
	INT	Dguard				  
	INT	Dinv					
	INT	hard_sym_nm1			
	INT	train_EQ_timeout		
RX_V27_WORKSPACE_LEN		.set	x-RX_COMMON_LEN
							 
;****************************************************************************
