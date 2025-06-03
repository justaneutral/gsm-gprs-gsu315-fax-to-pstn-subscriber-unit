;****************************************************************************
;* Filename: v32.inc
;* Date: 05-06-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****
			
TX_V32A_MOD_ID 				.set	3200h
TX_V32A_SILENCE1_ID 		.set	3201h
TX_V32A_ANS_ID 				.set	3202h
TX_V32A_SILENCE2_ID 		.set	3203h
TX_V32A_AC1_ID 				.set	3204h
TX_V32A_CA_ID 				.set	3205h
TX_V32A_AC2_ID 				.set	3206h
TX_V32A_SILENCE3_ID 		.set	3207h
TX_V32A_SPECIAL_TRN1_ID 	.set	3208h
TX_V32A_S1_ID 				.set	3209h
TX_V32A_SBAR1_ID 			.set	320ah
TX_V32A_TRN1_ID 			.set	320bh
TX_V32A_R1_ID 				.set	320ch
TX_V32A_SILENCE4_ID 		.set	320dh
TX_V32A_S2_ID 				.set	320eh
TX_V32A_SBAR2_ID 			.set	320fh
TX_V32A_TRN2_ID 			.set	3210h
TX_V32A_R3_ID 				.set	3211h
TX_V32A_E_ID 				.set	3212h
TX_V32A_B1_ID 				.set	3213h
TX_V32A_MESSAGE_ID 			.set	(3200h|MESSAGE_ID)
TX_V32A_RC_PREAMBLE_ID 		.set	3215h
TX_V32A_R4_ID 				.set	3216h
TX_V32A_R5_ID 				.set	3217h
					    
TX_V32C_MOD_ID 				.set	3280h
TX_V32C_SILENCE1_ID 		.set	3281h
TX_V32C_AA_ID 				.set	3282h
TX_V32C_CC_ID 				.set	3283h
TX_V32C_SILENCE2_ID 		.set	3284h
TX_V32C_S_DELAY_ID 			.set	3285h
TX_V32C_SPECIAL_TRN1_ID 	.set	3286h
TX_V32C_S1_ID 				.set	3287h
TX_V32C_SBAR1_ID 			.set	3288h
TX_V32C_TRN1_ID 			.set	3289h
TX_V32C_R2_ID 				.set	328ah
TX_V32C_E_ID 				.set	328bh
TX_V32C_B1_ID 				.set	328ch
TX_V32C_MESSAGE_ID 			.set	(3280h|MESSAGE_ID)
TX_V32C_RC_PREAMBLE_ID 		.set	328eh
TX_V32C_R4_ID 				.set	328fh
TX_V32C_R5_ID 				.set	3290h
				    
RX_V32A_DETECT_AA_ID 		.set	3201h
RX_V32A_DETECT_AACC_ID 		.set	3202h
RX_V32A_DETECT_CC_END_ID	.set	3203h
RX_V32A_TRAIN_EC_ID 		.set	3204h
RX_V32A_S_DETECT_ID 		.set	3205h
RX_V32A_TRAIN_LOOPS_ID 		.set	3206h
RX_V32A_DETECT_EQ_ID 		.set	3207h
RX_V32A_TRAIN_EQ_ID 		.set	3208h
RX_V32A_RATE_ID 			.set	3209h
RX_V32A_B1_ID 				.set	320ah
RX_V32A_MESSAGE_ID 			.set	(3200h|MESSAGE_ID)
RX_V32A_RC_PREAMBLE_ID 		.set	320ch
RX_V32A_R4_ID				.set	320dh
RX_V32A_R5_ID				.set	320eh
			
RX_V32C_DETECT_AC_ID 		.set	3281h
RX_V32C_DETECT_ACCA_ID 		.set	3282h
RX_V32C_DETECT_CAAC_ID 		.set	3283h
RX_V32C_DETECT_AC_END_ID	.set	3284h
RX_V32C_TRAIN_EC_ID 		.set	3285h
RX_V32C_S_DETECT_ID 		.set	3286h
RX_V32C_TRAIN_LOOPS_ID 		.set	3287h
RX_V32C_DETECT_EQ_ID 		.set	3288h
RX_V32C_TRAIN_EQ_ID 		.set	3289h
RX_V32C_RATE_ID 			.set	328ah
RX_V32C_B1_ID 				.set	328bh
RX_V32C_MESSAGE_ID 			.set	(3280h|MESSAGE_ID)
RX_V32C_RC_PREAMBLE_ID 		.set	328dh
RX_V32C_R4_ID				.set	328eh
RX_V32C_R5_ID				.set	328fh

	;**** mode definitions ****

V32TCM_MODE_BIT				.set	0008h
V32BIS_MODE_BIT				.set	0010h
V32_SPECIAL_TRAIN_BIT  		.set	0020h
V32TCM_MODE					.set	V32TCM_MODE_BIT
V32BIS_MODE					.set	V32BIS_MODE_BIT|V32TCM_MODE_BIT
GSTN_CLEARDOWN_RATE_PATTERN	.set	0111h

	;**** Tx ECSD generation ****

	.asg  TX_CONTROL_LEN,x
	INT	Tx_frequency			
	INT	Tx_osc_memory		   
	INT	Tx_osc_scale			
	INT	Tx_rev_memory		   
	INT	Tx_rev_period		   

	;**** Tx_v32 spare offsets ****

	.asg  TX_CONTROL_LEN+TX_COMMON_LEN,x
 .if FARC=ENABLED
	FAR_INT	Tx_scrambler_ptr
 .else
	INT	Tx_scrambler_ptr
 .endif
	INT	Tx_rate_pattern
	INT	Tx_max_rate
TX_V32_WORKSPACE_LEN	  	.set	x-TX_COMMON_LEN

	;**** Rx_v32 spare offsets ****

	.asg  RX_CONTROL_LEN+RX_COMMON_LEN,x
	INT	data_Q1				 
 .if FARC=ENABLED
	FAR_INT Rx_descrambler_ptr	  
 .else
	INT	Rx_descrambler_ptr	  
 .endif
	INT	Rx_rate_pattern		 
	INT	Rx_pattern_detect	   
	INT	Dcounter				
	INT	RCcounter			   
RX_V32_WORKSPACE_LEN		.set	x-RX_COMMON_LEN

;****************************************************************************
