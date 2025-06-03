;****************************************************************************
;* Filename: gendet.inc
;* Date: 02-04-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

	;**** state_ID definitions ****

TX_TONE_GEN_ID		 		.set	1200h
TX_CNG_ID			  		.set	1210h
TX_CED_ID			  		.set	1211h
TX_ECSD_ID			 		.set	1212h
TX_CALL_PROGRESS_ID			.set	1220h
TX_DIALTONE_ID		 		.set	1221h
TX_RINGBACK_ID		 		.set	1222h
TX_REORDER_ID		  		.set	1223h
TX_BUSY_ID			 		.set	1224h

RX_ENERGY_DET_ID			.set	1000h
RX_SIG_ANALYSIS_ID	 		.set	1100h
RX_TONE_ID			 		.set	1200h
RX_CNG_ID			  		.set	1210h
RX_CED_ID			  		.set	1211h
RX_ECSD_ID			 		.set	1212h
RX_TEP_1700_ID		 		.set	1213h
RX_TEP_1800_ID		 		.set	1214h
RX_CALL_PROGRESS_ID			.set	1220h
RX_DIALTONE_ID		 		.set	1221h
RX_RINGBACK_ID		 		.set	1222h
RX_REORDER_ID		  		.set	1223h
RX_BUSY_ID			 		.set	1224h

RX_V32_AUTOMODE_ID	 		.set	1500h
RX_V32_CED_ID		  		.set	1501h
RX_V32_ECSD_ID		 		.set	1502h
RX_V32_AA_ID				.set	1504h
RX_V32_AC_ID				.set	1508h
RX_V32_USB1_ID		 		.set	1510h

	;**** detector_mask mask bit definitions ****
	
AUTO_DETECT_MASK			.set	0001h
V21_CH1_MASK				.set	0002h
V21_CH2_MASK				.set	0004h
V22_MASK					.set	0008h
V27_2400_MASK		  		.set	0010h
V27_4800_MASK		  		.set	0020h
V29_MASK					.set	0040h
V17_MASK					.set	0080h
CED_MASK					.set	0100h
CNG_MASK					.set	0200h
TEP_MASK					.set	0400h
CALL_PROGRESS_MASK	 		.set	0800h
V32_AUTOMODE_MASK	  		.set	1000h

FAX_TONE_MASK		  		.set	CED_MASK|TEP_MASK
FAX_DETECT_MASK				.set	(FAX_TONE_MASK|AUTO_DETECT_MASK|V21_CH2_MASK|V27_2400_MASK|V27_4800_MASK|V29_MASK)
DATA_DETECT_MASK			.set	(AUTO_DETECT_MASK|V22_MASK)

	;**** digit_CP_mask mask bit definitions ****

DTMF_MASK			  		.set	 0001h
R1_MASK						.set	 0002h
R2F_MASK					.set	 0004h
R2B_MASK					.set	 0008h

	;**** Tx_gen common offsets ****

	.asg	TX_CONTROL_LEN,x
	INT		Tx_frequency1				   
	INT		Tx_vco_memory1				  
	INT		Tx_scale1					   
	INT		Tx_frequency2				   
	INT		Tx_vco_memory2				  
	INT		Tx_scale2					   
	INT		Tx_cad_memory				   
	INT		Tx_cad_period				   
	INT		Tx_on_time					  
	INT		Tx_rev_memory				   
	INT		Tx_rev_period				   
	INT		Tx_digit_ptr					
TX_GEN_WORKSPACE_LEN		.set	x

	;****		Rx_det common offsets ****

	.asg	RX_CONTROL_LEN,x
	INT		Rx_SNR_est_coef		 
	INT		Rx_SNR_thr_coef		 
	INT		Rx_broadband_level	  
	INT		Rx_level_350			
	INT		Rx_level_460			
	INT		Rx_level_500			
	INT		Rx_level_600			
	INT		Rx_level_980			
	INT		Rx_level_1000		   
	INT		Rx_level_1100		   
	INT		Rx_level_1180		   
	INT		Rx_level_1200		   
	INT		Rx_level_1650		   
	INT		Rx_level_1700		   
	INT		Rx_level_1750		   
	INT		Rx_level_1800		   
	INT		Rx_level_1850		   
	INT		Rx_level_2100		   
	INT		Rx_level_2225		   
	INT		Rx_level_2250		   
	INT		Rx_level_2400		   
	INT		Rx_level_2600		   
	INT		Rx_level_2850		   
	INT		Rx_level_2900		   
	INT		Rx_level_3000		   
	INT		Rx_filter_mask_low	  
	INT		Rx_filter_mask_high	 
	INT		Rx_dialtone_counter	 
	INT		Rx_ringback_counter	 
	INT		Rx_reorder_counter	  
	INT		v32_automode_counter	
 .if FARC=ENABLED
	FAR_INT	Rx_digit_detector	   
 .else
	INT		Rx_digit_detector	   
 .endif
	INT		Rx_digit_ptr			
	INT		Rx_Pbb				  
	INT		Rx_digit_ID				  
	INT		Rx_GF_len					
	INT		Rx_GF_counter				
	INT		Rx_num_filters			   
	INT		Rx_max_row				   
	INT		Rx_max_col 
	INT		Rx_digit_threshold				  
RX_DET_WORKSPACE_LEN		.set	x

	;**** definitions for Brazilian call progress tones ****

 .if $isdefed("BRAZIL_PSTN")
Rx_level_425				.set	Rx_level_350
Rx_CP_detect_counter		.set	Rx_dialtone_counter
Rx_CP_corr_register			.set	Rx_ringback_counter
Rx_CP_corr_register_low		.set	Rx_reorder_counter

CP_SAMPLE_PERIOD			.set	25		;* 250 msec at 100 Hz 
DIALTONE_CORR_PATTERN		.set	01ffh	;* continuous tone 
RINGBACK_CORR_PATTERN		.set	01e0h	;* 1 sec. ON, 4 sec. OFF 
BUSY_CORR_PATTERN			.set	0154h	;* 0.25 sec. ON, 0.25 sec. OFF 
REORDER_CORR_PATTERN		.set	0110h	;* 0.25 sec. ON, 0.75 sec. OFF 
CORR_MSB_POSITION			.set	0100h	;* MSB position for correlation pattern 
 .endif

;****************************************************************************
