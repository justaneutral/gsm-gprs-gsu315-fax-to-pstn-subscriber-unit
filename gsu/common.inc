;****************************************************************************
;* Filename: common.inc
;* Date: 02-04-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros for the 
;*		common assembly file
;****************************************************************************

   ;**** APSK modulator common offsets ****

	.asg  TX_CONTROL_LEN,x
	INT	Tx_fir_head					  
	INT	Tx_fir_tail					  
	INT	Tx_fir_len					   
	INT	Tx_fir_taps					  
	INT	Tx_coef_start					
	INT	Tx_coef_ptr					  
	INT	Tx_interpolate				   
	INT	Tx_decimate					  
	INT	Tx_sym_clk_offset				
	INT	Tx_sym_clk_memory				
	INT	Tx_sym_clk_phase				 
	INT	Tx_carrier					   
	INT	Tx_map_ptr					   
	INT	Tx_amp_ptr					   
	INT	Tx_phase						 
	INT	Tx_Sreg						  
	INT	Tx_Sreg_low					  
	INT	Tx_fir_scale					 
	INT	Ereg							 
TX_COMMON_LEN		  		.set	x-TX_CONTROL_LEN

	;**** APSK demodulator common offsets ****
					
	.asg  RX_CONTROL_LEN,x
 .if FARC=ENABLED
	FAR_INT	decoder_ptr	  
 .else
	INT	decoder_ptr	  
 .endif
 .if FARC=ENABLED
	FAR_INT	slicer_ptr	   
 .else
	INT	slicer_ptr	   
 .endif
 .if FARC=ENABLED
	FAR_INT	timing_ptr	   
 .else
	INT	timing_ptr	   
 .endif
	INT	Rx_baud_counter	 
	INT	Rx_data_ptr	  
	INT	Rx_sample_ptr	
	INT	Rx_fir_taps	  
	INT	Rx_coef_start	
	INT	Rx_coef_ptr	  
	INT	Rx_sym_clk_phase 
	INT	Rx_interpolate   
	INT	Rx_decimate	  
	INT	Rx_oversample	
	INT	Rx_timing_start										  
	INT	I				
	INT	Q				
	INT	IEQ			  
	INT	QEQ			  
	INT	Iprime		   
	INT	Qprime		   
	INT	Inm1			 
	INT	Qnm1			 
	INT	Inm2			 
	INT	Qnm2			 
	INT	Inm3			 
	INT	Qnm3			 
	INT	Ihat			 
	INT	Qhat			 
	INT	Ihat_nm2		 
	INT	Qhat_nm2		 
	INT	What			 
	INT	Rx_fir_ptr	   
	INT	IEQprime_error   
	INT	QEQprime_error   
	INT	EQ_MSE		   
	INT	EQ_2mu		   
	INT	COS			  
	INT	SIN			  
	INT	LO_memory		
	INT	LO_frequency	 
	INT	LO_phase		 
	INT	vco_memory	   
	INT	phase_error	  
	INT	loop_memory	  
	INT	loop_memory_low  
	INT	loop_K1		  
	INT	loop_K2		  
	INT	PJ1_coef
	INT	PJ1_dnm2
	INT	PJ1_dnm1
	INT	PJ2_coef
	INT	PJ2_dnm2
	INT	PJ2_dnm1
	INT	agc_gain		 
	INT	agc_K			
	INT	frequency_est	
	INT	Rx_sym_clk_memory
	INT	timing_threshold 
	INT	coarse_error	 
	INT	LOS_counter	  
	INT	LOS_monitor	  
	INT	Rx_map_shift	 
	INT	Phat			 
	INT	Rx_phase		 
	INT	EQ_taps		  
	INT	Rx_Dreg		  
	INT	Rx_Dreg_low	  
	INT	Rx_pattern_reg   
	INT	demod_delay_ptr  
	INT	trace_back_ptr   
	INT	signal_map_ptr   
	INT	EC_fir_ptr	   
	INT	EC_sample_ptr	
	INT	EC_2mu		   
	INT	EC_MSE		   
	INT	EC_taps		  
	INT	EC_shift		 
	INT	Rx_RTD		   
RX_COMMON_LEN		  		.set	x-RX_CONTROL_LEN
	
;++++#ifndef MESI_INTERNAL 03-22-2001
;++++#else   MESI_INTERNAL 03-22-2001
	;**** FSK modulator common offsets ****

	.asg  TX_CONTROL_LEN,x
	INT	TxFSK_coef_ptr 
	INT	TxFSK_interpolate 
	INT	TxFSK_decimate 
	INT	TxFSK_carrier 
	INT	TxFSK_tone_scale 
	INT	TxFSK_vco_memory 
	INT	TxFSK_frequency
	INT	TxFSK_frequency_shift
TX_FSK_BLOCK_LEN	  		.set	x-TX_CONTROL_LEN

	;****FSK demodulator common offsets ****

	.asg  RX_CONTROL_LEN,x
	INT	RxFSK_mark_coef
	INT	RxFSK_space_coef
	INT	RxFSK_sym
	INT	RxFSK_coef_len
	INT	RxFSK_coef_ptr
	INT	RxFSK_interpolate
	INT	RxFSK_decimate
	INT	RxFSK_sym_nm1
	INT	RxFSK_sym_hat
	INT	RxFSK_sym_hat_nm2
	INT	RxFSK_sym_clk_memory
	INT	RxFSK_baud_counter
	INT	RxFSK_sym_level
	INT	RxFSK_LOS_threshold
	INT	RxFSK_LOS_memory
RX_FSK_BLOCK_LEN	  		.set	x-RX_CONTROL_LEN
;++++#endif  MESI_INTERNAL 03-22-2001

;++++#ifndef MESI_INTERNAL 02-26-2001
;++++#else   MESI_INTERNAL 02-26-2001
	;**** set up modem operating point, OP_POINT ****

OP_POINT64 			        .set    512		;* (1/64.0)*32768
OP_POINT32 			        .set    1024    ;* (1/32.0)*32768
OP_POINT16 			        .set    2048    ;* (1/16.0)*32768
OP_POINT8 			        .set    4096    ;* (1/8.0)*32768 
OP_POINT4 			        .set    8192    ;* (1/4.0)*32768 
OP_POINT2 			        .set    16384   ;* (1/2.0)*32768 

 .if !$isdefed("OP_POINT")
;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
OP_POINT 			        .set	OP_POINT64	
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
;OP_POINT 			        .set	OP_POINT8
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
 .endif

 .if OP_POINT == OP_POINT64
OP_POINT_SHIFT				.set    6
 .endif
 .if OP_POINT == OP_POINT32
OP_POINT_SHIFT		        .set    5
 .endif
 .if OP_POINT == OP_POINT16
OP_POINT_SHIFT		        .set    4
 .endif
 .if OP_POINT == OP_POINT8
OP_POINT_SHIFT		        .set    3
;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
 .if !$isdefed("SQUARE_ROOT_WHAT")
SQUARE_ROOT_WHAT			.set	ENABLED
 .endif
;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
 .endif
 .if OP_POINT == OP_POINT4
OP_POINT_SHIFT		        .set    2
 .endif
 .if OP_POINT == OP_POINT2
OP_POINT_SHIFT		        .set    1
 .endif
;++++#endif  MESI_INTERNAL 02-26-2001

		   
	;**** RCOS filter coefficients ****
			 
ROLL600						.set	0.75
OVERSAMPLE600		  		.set	1
INTERP600			  		.set	(3*OVERSAMPLE600)
DEC600				 		.set	(20*OVERSAMPLE600)
TAPS600						.set	60
RCOS600_LEN					.set	(TAPS600*INTERP600+DEC600)
RX_RCOS600_LEN		 		.set	(TAPS600*INTERP600+DEC600)
FS600				  		.set	(8000*INTERP600)
TX_TAPS600			 		.set	3
TX_RCOS600_LEN		 		.set	(TX_TAPS600*2*DEC600+INTERP600+1) 

ROLL1200					.set	0.75
OVERSAMPLE1200		 		.set	4
INTERP1200			 		.set	(3*OVERSAMPLE1200)
DEC1200						.set	(10*OVERSAMPLE1200)
TAPS1200					.set	18
RCOS1200_LEN				.set	(TAPS1200*INTERP1200+DEC1200)
FS1200				 		.set	(8000*INTERP1200)

ROLL1600					.set	0.75
OVERSAMPLE1600		 		.set	2
INTERP1600			 		.set	(6*OVERSAMPLE1600)
DEC1600						.set	(15*OVERSAMPLE1600)
TAPS1600					.set	18
RCOS1600_LEN				.set	(TAPS1600*INTERP1600+DEC1600)
FS1600				 		.set	(8000*INTERP1600)

ROLL2400					.set	0.75
OVERSAMPLE2400		 		.set	8
INTERP2400			 		.set	(3*OVERSAMPLE2400)
DEC2400						.set	(5*OVERSAMPLE2400)
TAPS2400					.set	16
RCOS2400_LEN				.set	(TAPS2400*INTERP2400+DEC2400)
FS2400				 		.set	(8000*INTERP2400)

ROLL3000					.set	0.45
OVERSAMPLE3000		 		.set	8
INTERP3000			 		.set	(3*OVERSAMPLE3000)
DEC3000						.set	(4*OVERSAMPLE3000)
TAPS3000					.set	16
RCOS3000_LEN				.set	(TAPS3000*INTERP3000+DEC3000)
FS3000				 		.set	(8000.0*INTERP3000)

ROLL3200					.set	0.25
OVERSAMPLE3200		 		.set	8
INTERP3200			 		.set	(4*OVERSAMPLE3200)
DEC3200						.set	(5*OVERSAMPLE3200)
TAPS3200					.set	20 
RCOS3200_LEN				.set	(TAPS3200*INTERP3200+DEC3200)
FS3200				 		.set	(8000.0*INTERP3200)

	;**** APSK modulator parameters ***

SYM_CLK_THRESHOLD	  		.set	16384
					  
	;**** APSK demodulator parameters ****
					
;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;COARSE_THR			 		.set	8688	;* 6*(2*OP_POINT*SQRT(2))
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
COARSE_THR			 		.set	2048
 .else
COARSE_THR			 		.set	512
 .endif
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
TIMING_TABLE_LEN			.set	(4*4)
COEF_INCR			  		.set	(0*4)
COEF_DECR			  		.set	(1*4)
FIR_INCR					.set	(2*4)
FIR_DECR					.set	(3*4)

REV_CORR_LEN				.set	16
REV_CORR_DELAY		 		.set	(4*4)	;* 2 symbols*2x baud*(REV_CORR_LEN/4)
;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;LOS_THRESHOLD		  		.set	128		;* (32768*OP_POINT*0.25)
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
LOS_THRESHOLD		  		.set	1024	;* (32768*OP_POINT*0.25)
 .else		;* OP_POINT=8
LOS_THRESHOLD		  		.set	128		;* (32768*OP_POINT*0.25)
 .endif		;* OP_POINT=8
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
LOS_COUNT			  		.set	4
LOCKED				 		.set	400
UNLOCKED					.set	0

EQ_COEF_SEED				.set	4096
;++++#ifndef MESI_INTERNAL 03-05-2001 (OP_POINT _SHIFT MODS)
;EQ_2MU_SCALE				.set	8
;++++#endif  MESI_INTERNAL 03-05-2001 (OP_POINT _SHIFT MODS)

EQ_DISABLED					.set	-1
EQ_UPDATE_DISABLED	 		.set	0
EQ_FIR_ENABLED		 		.set	0

MSE_B0				 		.set	328	 	;* (32768*0.01)
MSE_A1				 		.set	32440   ;* (32768-MSE_B0)
AGC_REF						.set	512	 	;* (32768*OP_POINT*OP_POINT*64) 
AGC_EST_SEED				.set	20287
AGC_EST_STEP				.set	16423

COS_PI_BY_4					.set	23170   ;* 32768*cos(pi/4)
TWENTY_SIX_DEGREES	 		.set	4836
FOURTY_FIVE_DEGREES			.set	8192
NINETY_DEGREES		 		.set	16384
ONE_EIGHTY_DEGREES	 		.set	32768
RX_DET_LEN			 		.set	80

	;**** phase jitter resonator coefficients ****

;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;PJ_COEF_A					.set	1024   
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
PJ_COEF_A					.set	(2<<OP_POINT_SHIFT)
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
PJ50_COEF600_B				.set    -28378	;* 32768*cos(2*pi*50/600) 
PJ60_COEF600_B				.set    -26510	;* 32768*cos(2*pi*60/600) 
PJ50_COEF1200_B				.set    -31651	;* 32768*cos(2*pi*50/1200)
PJ60_COEF1200_B				.set    -31164	;* 32768*cos(2*pi*60/1200)
PJ50_COEF1600_B				.set    -32138	;* 32768*cos(2*pi*50/1600)
PJ60_COEF1600_B				.set    -31863	;* 32768*cos(2*pi*60/1600)
PJ50_COEF2400_B				.set    -32488	;* 32768*cos(2*pi*50/2400)
PJ60_COEF2400_B				.set    -32365	;* 32768*cos(2*pi*60/2400)
PJ50_COEF3000_B				.set    -32588  ;* 32768*cos(2*pi*50/3000)
PJ60_COEF3000_B				.set    -32510  ;* 32768*cos(2*pi*60/3000)
PJ50_COEF3200_B				.set    -32610  ;* 32768*cos(2*pi*50/3200)
PJ60_COEF3200_B				.set    -32541  ;* 32768*cos(2*pi*60/3200)

ATTENUATE_2DB				.set	26029	;* 32768*10exp(-2 dB/20)	
ATTENUATE_3DB				.set	23198	;* 32768*10exp(-3 dB/20)	
ATTENUATE_6DB				.set	16423	;* 32768*10exp(-6 dB/20)	
ATTENUATE_9DB				.set	11627	;* 32768*10exp(-9 dB/20)	
ATTENUATE_10DB				.set	10362	;* 32768*10exp(-10 dB/20)	
ATTENUATE_12DB				.set	8231	;* 32768*10exp(-12 dB/20)	
ATTENUATE_16DB				.set	5192	;* 32768*10exp(-16 dB/20)	
ATTENUATE_20DB				.set	3277	;* 32768*10exp(-20 dB/20)	
ATTENUATE_28DB				.set	1305	;* 32768*10exp(-28 dB/20)	
ATTENUATE_30DB				.set	1036	;* 32768*10exp(-30 dB/20)	

;****************************************************************************
