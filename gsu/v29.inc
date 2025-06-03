;****************************************************************************
;* Filename: v29.inc
;* Date: 04-10-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****
			
TX_V29_MOD_ID				.set	2900h
TX_V29_TEP_ID				.set	2901h
TX_V29_SEGMENT1_ID			.set	2902h
TX_V29_SEGMENT2_ID			.set	2903h
TX_V29_SEGMENT3_ID			.set	2904h
TX_V29_SEGMENT4_ID			.set	2905h
TX_V29_MESSAGE_ID 			.set	(2900h|MESSAGE_ID)
TX_V29_SEGMENT6_ID			.set	2907h

RX_V29_TRAIN_LOOPS_ID 		.set	2901h
RX_V29_DETECT_EQ_ID 		.set	2902h
RX_V29_TRAIN_EQ_ID 			.set	2903h
RX_V29_SCR1_ID 				.set	2904h
RX_V29_MESSAGE_ID 			.set	(2900h|MESSAGE_ID)

	;**** Tx_v29 spare offsets ****

	.asg  TX_CONTROL_LEN+TX_COMMON_LEN,x
	INT	Tx_amp_acc
TX_V29_WORKSPACE_LEN	  	.set	x-TX_COMMON_LEN

	;**** Rx_v29 spare offsets ****

	.asg  RX_CONTROL_LEN+RX_COMMON_LEN,x
	INT	data_Q1				 
	INT	hard_sym_nm1			
RX_V29_WORKSPACE_LEN		.set	x-RX_COMMON_LEN
							 
;****************************************************************************
