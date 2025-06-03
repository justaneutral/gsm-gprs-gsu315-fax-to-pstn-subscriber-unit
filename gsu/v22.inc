;****************************************************************************
;* Filename: v22.inc
;* Date: 06-13-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****

TX_V22A_SILENCE1_ID			.set    2201h
TX_V22A_ANS_ID				.set    2202h
TX_V22A_SILENCE2_ID			.set    2203h
TX_V22A_UB1_ID				.set    2204h
TX_V22A_S1_ID				.set    2205h
TX_V22A_SCR1_ID				.set    2206h
TX_V22A_SB1_R2_ID			.set    2207h
TX_V22A_MESSAGE_ID			.set    (2200h|MESSAGE_ID)

TX_V22C_SILENCE_ID			.set    2281h
TX_V22C_S1_ID				.set    2282h
TX_V22C_SCR1_ID				.set    2283h
TX_V22C_SB1_R2_ID			.set    2284h
TX_V22C_MESSAGE_ID			.set    (2280h|MESSAGE_ID)

RX_V22A_START_DETECT_ID 	.set    2201h
RX_V22A_TRAIN_LOOPS_ID		.set    2202h
RX_V22A_TRAIN_EQ_ID 		.set    2203h
RX_V22A_MESSAGE_ID 			.set    (2200h|MESSAGE_ID)
RX_V22A_RC_RESPOND_ID 		.set    2205h
RX_V22A_RC_INITIATE_ID 		.set    2206h

RX_V22C_START_DETECT_ID 	.set    2281h
RX_V22C_TRAIN_LOOPS_ID		.set    2282h
RX_V22C_TRAIN_EQ_ID 		.set    2283h
RX_V22C_MESSAGE_ID 			.set    (2280h|MESSAGE_ID)
RX_V22C_RC_RESPOND_ID 		.set    2285h
RX_V22C_RC_INITIATE_ID 		.set    2286h

	;**** Tx_v22 spare offsets ****

	.asg  TX_CONTROL_LEN+TX_COMMON_LEN,x
	INT	Tx_Scounter
TX_V22_WORKSPACE_LEN	  	.set	x-TX_COMMON_LEN

	;**** Rx_v22 spare offsets ****

	.asg  RX_CONTROL_LEN+RX_COMMON_LEN,x
	INT	data_Q1				 
	INT	Rx_pattern_detect	   
	INT	S1_memory			   
	INT	S1_nm1					 
	INT	Dcounter				
	INT	SNR_est_coef			
	INT	SNR_thr_coef			
RX_V22_WORKSPACE_LEN		.set	x-RX_COMMON_LEN
							 
;****************************************************************************
