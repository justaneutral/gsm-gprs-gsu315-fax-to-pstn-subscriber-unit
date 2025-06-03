;****************************************************************************
;* Filename: v22.asm
;* Date: 06-13-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for v.22.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"v22.inc"
	.include	"filter.inc"

V22_RATE_1200		  		.set	1200
V22_RATE_2400		  		.set	2400

	;**** modulator ****

TX_FIR_TAPS					.set	TX_TAPS600
TX_INTERP			  		.set	2*DEC600
TX_DEC				 		.set	INTERP600
TX_COEF_LEN					.set	(TX_FIR_TAPS*TX_INTERP+TX_DEC+1) ;* NOTE: add1, HAS to be even
TX_V22_SCALE				.set	13029   ;* -16 dB RMS power DO NOT INCREASE ! 
TX_PHASE_LEN				.set	4
TX_ONE				 		.set	8192
TX_THREE					.set	24576
GUARD_TONE					.set    DISABLED
GUARD_FREQUENCY				.set    14746	;* 8.192*1800 Hz
GUARD_SCALE					.set    9235	;* -5-6dB

 .if $isdefed("V22_DEBUG")
TXA_SILENCE1_LEN			.set	100
TXA_ANSWER_TONE_LEN			.set	100	 
 .else
TXA_SILENCE1_LEN			.set	17200   ;* 2.15 sec.
TXA_ANSWER_TONE_LEN			.set	26400   ;* 3.3 sec 2100 Hz
 .endif
TXA_SILENCE2_LEN			.set	600	 	;* 75 msec.
TXA_S1_LEN			 		.set	60 
TXA_SB1_1200_LEN			.set	459	   
TXA_SCR1_LEN				.set	300
TXA_SB1_R2_LEN		 		.set	120	   

TXC_SILENCE_LEN				.set	366	
TXC_S1_LEN			 		.set	60
TXC_SB1_1200_LEN			.set	459		
TXC_SCR1_LEN				.set	360
TXC_SB1_R2_LEN		 		.set	120

 .if $isdefed("XDAIS_API")
	.global _V22_MESI_TxInitV22A
	.global V22_MESI_TxInitV22A
	.global _V22_MESI_TxV22ARetrain
	.global V22_MESI_TxV22ARetrain
	.global _V22_MESI_TxInitV22A_ANS
	.global V22_MESI_TxInitV22A_ANS
	.global _V22_MESI_TxInitV22C
	.global V22_MESI_TxInitV22C
	.global _V22_MESI_TxV22CRetrain
	.global V22_MESI_TxV22CRetrain
 .else
	.global _Tx_init_v22A
	.global Tx_init_v22A
	.global _Tx_v22A_retrain
	.global Tx_v22A_retrain
	.global _Tx_init_v22A_ANS
	.global Tx_init_v22A_ANS
	.global _Tx_init_v22C
	.global Tx_init_v22C
	.global _Tx_v22C_retrain
	.global Tx_v22C_retrain
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_APSKmodulator
	.asg	COMMON_MESI_APSKmodulator, APSK_modulator
	.global RXTX_MESI_TxInitSilence
	.asg	RXTX_MESI_TxInitSilence, Tx_init_silence
	.global GEN_MESI_TxInitToneGen
	.asg	GEN_MESI_TxInitToneGen, Tx_init_tone_gen
	.global GEN_MESI_TxToneGen
	.asg	GEN_MESI_TxToneGen, Tx_tone_gen 
	.global GEN_MESI_TxInitCED
	.asg	GEN_MESI_TxInitCED, Tx_init_CED
	.global RXTX_MESI_TxStateReturn
	.asg	RXTX_MESI_TxStateReturn, Tx_state_return
 .else
	.global APSK_modulator
	.global Tx_init_silence
	.global Tx_init_tone_gen
	.global Tx_tone_gen
	.global Tx_init_CED
	.global Tx_state_return
 .endif										;* "XDAIS_API endif

	;**** demodulator ****

;++++#ifndef MESI_INTERNAL 03-09-2001 OP_POINT8 MODS
;RX_FIR_TAPS					.set	60	 
;RX_OVERSAMPLE		  		.set	OVERSAMPLE600
;RX_INTERP			  		.set	INTERP600
;RX_DEC				 		.set	DEC600
;RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
;RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
;RX_BAUD_RATE				.set	600
;RX_V22_CARRIER_FREQ_LO 		.set	9831
;RX_V22_CARRIER_FREQ_HI 		.set	19661
;V22_COEF_LEN				.set	60
;LO_PHASE_ADJ_1200	  		.set	3276	;* (65536/8000)*(1200/RX_INTERP)
;LO_PHASE_ADJ_2400	  		.set	6553	;* (65536/8000)*(2400/RX_INTERP)
;
;V22_EQ_LEN			 		.set	7
;EQ_TRAIN_SEED		  		.set	0e0h
;ACQ_EQ_2MU			 		.set	4096
;TRK_EQ_2MU			 		.set	256
;ACQ_AGC_K			  		.set	8192
;ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
;TRK_TIMING_THR		 		.set	ACQ_TIMING_THR		 
;
;V22_SNR_EST_LEN				.set	16
;V22_SNR_EST_SHIFT	  		.set	(4+1)
;V22_SNR_THR_COEF			.set	648	 	;* 32768*10exp(-10/20)*(1/SNR_EST_LEN)
;V22_SNR_EST_COEF			.set	2048	;* 32678*(1/SNR_EST_LEN)
;RXC_ANALYSIS_LEN			.set	8
;RXC_ANALYSIS_COEF	  		.set	410	 	;* 32768/80
;RXC_FREQUENCY_COEF	 		.set	64	  	;* (300/1200)*256
;RXC_DETECT_THRESHOLD		.set	225
;S1_THRESHOLD				.set	16
;S1_END_THRESHOLD			.set	4
;V22_BB_EST_LEN		 		.set	16
;V22_BB_EST_COEF				.set	2453
;
;SLICE_707			  		.set	362		 
;SLICE1				 		.set	162	 	;* 1*(OP_POINT/SIG_AVG)
;SLICE2				 		.set	324	 	;* 2*(OP_POINT/SIG_AVG)
;SLICE3				 		.set	486	 	;* 3*(OP_POINT/SIG_AVG)
;;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;WHAT_11 					.set	2560	;* ( P_AVG/(1^2+1^2) )/64)*32768
;;WHAT_13 					.set	512		;* ( P_AVG/(1^2+3^2) )/64)*32768
;;WHAT_33 					.set	284		;* ( P_AVG/(3^2+3^2) )/64)*32768
;;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
; .if $isdefed("SQUARE_ROOT_WHAT")
;WHAT_11 					.set	1145    ;* 32768.0*OP_POINT*PAVG/(1*1+1*1)
;WHAT_13 					.set	512     ;* 32768.0*OP_POINT*PAVG/(1*1+3*3)
;WHAT_33 					.set	382     ;* 32768.0*OP_POINT*PAVG/(3*3+3*3)
; .else		;* SQUARE_ROOT_WHAT
;WHAT_11 					.set	2560	;* ( P_AVG/(1^2+1^2) )/64)*32768
;WHAT_13 					.set	512		;* ( P_AVG/(1^2+3^2) )/64)*32768
;WHAT_33 					.set	284		;* ( P_AVG/(3^2+3^2) )/64)*32768
; .endif		;* SQUARE_ROOT_WHAT
;;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;V22_ACQ_LOOP_K1				.set	24710
;V22_ACQ_LOOP_K2				.set	9312
;V22_TRK_LOOP_K1				.set	6176
;V22_TRK_LOOP_K2				.set	580
;++++#else   MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
RX_FIR_TAPS					.set	60	 
RX_OVERSAMPLE		  		.set	OVERSAMPLE600
RX_INTERP			  		.set	INTERP600
RX_DEC				 		.set	DEC600
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_BAUD_RATE				.set	600
RX_V22_CARRIER_FREQ_LO 		.set	9831
RX_V22_CARRIER_FREQ_HI 		.set	19661
V22_COEF_LEN				.set	60
LO_PHASE_ADJ_1200	  		.set	3276	;* (65536/8000)*(1200/RX_INTERP)
LO_PHASE_ADJ_2400	  		.set	6553	;* (65536/8000)*(2400/RX_INTERP)

V22_EQ_LEN			 		.set	7
EQ_TRAIN_SEED		  		.set	0e0h
ACQ_EQ_2MU			 		.set	1170
TRK_EQ_2MU			 		.set	73  
ACQ_AGC_K			  		.set	1024
ACQ_TIMING_THR		 		.set	2048 
TRK_TIMING_THR		 		.set	6144 

V22_SNR_EST_LEN				.set	16
V22_SNR_EST_SHIFT	  		.set	(4+1)
V22_SNR_THR_COEF			.set	648	 	;* 32768*10exp(-10/20)*(1/SNR_EST_LEN)
V22_SNR_EST_COEF			.set	2048	;* 32678*(1/SNR_EST_LEN)
RXC_ANALYSIS_LEN			.set	8
RXC_ANALYSIS_COEF	  		.set	410	 	;* 32768/80
RXC_FREQUENCY_COEF	 		.set	64	  	;* (300/1200)*256
RXC_DETECT_THRESHOLD		.set	225
S1_THRESHOLD				.set	16
S1_END_THRESHOLD			.set	4
V22_BB_EST_LEN		 		.set	16
V22_BB_EST_COEF				.set	2453

SLICE_707			  		.set	2896
SLICE1				 		.set	1295
SLICE2				 		.set	2591
SLICE3				 		.set	3886
WHAT_11 					.set	9159
WHAT_13 					.set	4096
WHAT_33 					.set	3053

V22_ACQ_LOOP_K1				.set	3089
V22_ACQ_LOOP_K2				.set	1165
V22_TRK_LOOP_K1				.set	772 
V22_TRK_LOOP_K2				.set	72  
 .else      ;* OP_POINT=8
RX_FIR_TAPS					.set	60	 
RX_OVERSAMPLE		  		.set	OVERSAMPLE600
RX_INTERP			  		.set	INTERP600
RX_DEC				 		.set	DEC600
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_BAUD_RATE				.set	600
RX_V22_CARRIER_FREQ_LO 		.set	9831
RX_V22_CARRIER_FREQ_HI 		.set	19661
V22_COEF_LEN				.set	60
LO_PHASE_ADJ_1200	  		.set	3276	;* (65536/8000)*(1200/RX_INTERP)
LO_PHASE_ADJ_2400	  		.set	6553	;* (65536/8000)*(2400/RX_INTERP)

V22_EQ_LEN			 		.set	7
EQ_TRAIN_SEED		  		.set	0e0h
ACQ_EQ_2MU			 		.set	4096
TRK_EQ_2MU			 		.set	256
ACQ_AGC_K			  		.set	8192
ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
TRK_TIMING_THR		 		.set	ACQ_TIMING_THR		 

V22_SNR_EST_LEN				.set	16
V22_SNR_EST_SHIFT	  		.set	(4+1)
V22_SNR_THR_COEF			.set	648	 	;* 32768*10exp(-10/20)*(1/SNR_EST_LEN)
V22_SNR_EST_COEF			.set	2048	;* 32678*(1/SNR_EST_LEN)
RXC_ANALYSIS_LEN			.set	8
RXC_ANALYSIS_COEF	  		.set	410	 	;* 32768/80
RXC_FREQUENCY_COEF	 		.set	64	  	;* (300/1200)*256
RXC_DETECT_THRESHOLD		.set	225
S1_THRESHOLD				.set	16
S1_END_THRESHOLD			.set	4
V22_BB_EST_LEN		 		.set	16
V22_BB_EST_COEF				.set	2453

SLICE_707			  		.set	362		 
SLICE1				 		.set	162	 	;* 1*(OP_POINT/SIG_AVG)
SLICE2				 		.set	324	 	;* 2*(OP_POINT/SIG_AVG)
SLICE3				 		.set	486	 	;* 3*(OP_POINT/SIG_AVG)
 .if $isdefed("SQUARE_ROOT_WHAT")
WHAT_11 					.set	1145    ;* 32768.0*OP_POINT*PAVG/(1*1+1*1)
WHAT_13 					.set	512     ;* 32768.0*OP_POINT*PAVG/(1*1+3*3)
WHAT_33 					.set	382     ;* 32768.0*OP_POINT*PAVG/(3*3+3*3)
 .else		;* SQUARE_ROOT_WHAT
WHAT_11 					.set	2560	;* ( P_AVG/(1^2+1^2) )/64)*32768
WHAT_13 					.set	512		;* ( P_AVG/(1^2+3^2) )/64)*32768
WHAT_33 					.set	284		;* ( P_AVG/(3^2+3^2) )/64)*32768
 .endif		;* SQUARE_ROOT_WHAT
V22_ACQ_LOOP_K1				.set	24710
V22_ACQ_LOOP_K2				.set	9312
V22_TRK_LOOP_K1				.set	6176
V22_TRK_LOOP_K2				.set	580
 .endif		;* OP_POINT=8
;++++#endif  MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 

TRAIN_LOOPS_TIMEOUT			.set	6000
SB1_270_TIMEOUT				.set	162
SB1_765_TIMEOUT				.set	459			
S1_END_TIMEOUT		 		.set	264
TRAIN_EQ_TIMEOUT			.set	6000
TRAIN_EQ_SCR1_TIMEOUT  		.set	32
RC_RESPOND_TIMEOUT	 		.set	64
RC_INITIATE_TIMEOUT			.set	300			

UB1_PATTERN					.set	0aaaah
SB1_PATTERN					.set	0ffffh
NOT_DETECTED				.set	0
S1_NOT_DETECTED				.set	0
S1_DETECTED					.set	1
S1_END_DETECTED				.set	2
UB1_DETECTED				.set	3
UB1_END_DETECTED			.set	4
SB1_DETECTED				.set	5
SB1_270_DETECTED			.set	6
SB1_END_DETECTED			.set	7
R1_DETECTED					.set	8

 .if $isdefed("XDAIS_API")
	.global V22_MESI_RxInitV22C
 .else
	.global Rx_init_v22C
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global DET_MESI_RxInitDetector
	.asg	DET_MESI_RxInitDetector, Rx_init_detector
	.global COMMON_MESI_APSKdemodulator
	.asg	COMMON_MESI_APSKdemodulator, APSK_demodulator
	.global COMMON_MESI_noTiming
	.asg	COMMON_MESI_noTiming, no_timing
	.global COMMON_MESI_sgnTiming
	.asg	COMMON_MESI_sgnTiming, sgn_timing
	.global COMMON_MESI_APSKTiming
	.asg	COMMON_MESI_APSKTiming, APSK_timing
	.global COMMON_MESI_agcGainEstimator
	.asg	COMMON_MESI_agcGainEstimator, agc_gain_estimator
	.global FILTER_MESI_BandpassFilter
	.asg	FILTER_MESI_BandpassFilter, bandpass_filter
	.global FILTER_MESI_BroadbandEstimator
	.asg	FILTER_MESI_BroadbandEstimator, broadband_estimator
	.global RXTX_MESI_TxSyncSampleBuffers
	.asg	RXTX_MESI_TxSyncSampleBuffers, Tx_sync_sample_buffers
	.global	_VCOEF_MESI_TxRCOS600f1200
	.asg	_VCOEF_MESI_TxRCOS600f1200, _Tx_RCOS600_f1200
	.global	_VCOEF_MESI_TxRCOS600f2400
	.asg	_VCOEF_MESI_TxRCOS600f2400, _Tx_RCOS600_f2400
	.global	_VCOEF_MESI_RxRCOS600f1200
	.asg	_VCOEF_MESI_RxRCOS600f1200, _Rx_RCOS600_f1200
	.global	_VCOEF_MESI_RxRCOS600f2400
	.asg	_VCOEF_MESI_RxRCOS600f2400, _Rx_RCOS600_f2400
	.global	_VCOEF_MESI_RxTiming600
	.asg	_VCOEF_MESI_RxTiming600, _Rx_timing600
	.global RXTX_MESI_RxStateReturn
	.asg	RXTX_MESI_RxStateReturn, Rx_state_return
	.global COMMON_MESI_slicerReturn
	.asg	COMMON_MESI_slicerReturn, slicer_return
	.global COMMON_MESI_timingReturn
	.asg	COMMON_MESI_timingReturn, timing_return
	.global COMMON_MESI_decoderReturn
	.asg	COMMON_MESI_decoderReturn, decoder_return
 .else
	.global Rx_init_detector
	.global APSK_demodulator
	.global no_timing
	.global sgn_timing
	.global APSK_timing
	.global agc_gain_estimator
	.global bandpass_filter
	.global broadband_estimator
	.global Tx_sync_sample_buffers
	.global _Tx_RCOS600_f1200
	.global _Tx_RCOS600_f2400
	.global _Rx_RCOS600_f1200
	.global _Rx_RCOS600_f2400
	.global _Rx_timing600
	.global Rx_state_return
	.global slicer_return
	.global timing_return
	.global decoder_return
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global Tx_v22_phase_map							
	.global Tx_v22_amp_table
	.global Rx_v22_hard_map
	.global Rx_v22_phase_map
	.global Tx_v22A_silence1
	.global Tx_v22A_ANS
	.global Tx_v22A_silence2
	.global Tx_v22A_UB1
	.global Tx_v22A_SB1_1200
	.global Tx_v22A_S1
	.global Tx_v22A_SCR1
	.global Tx_v22A_SB1_R2
	.global Tx_v22A_message
	.global Tx_v22C_silence
	.global Tx_v22C_SB1_1200
	.global Tx_v22C_S1
	.global Tx_v22C_SCR1
	.global Tx_v22C_SB1_R2
	.global Tx_v22C_message
	.global Tx_init_v22
	.global Tx_v22_S1						
	.global Tx_v22_SCR_R1R2					
	.global Tx_v22_SB1					
	.global Tx_v22_message					
	.global v22_diff_encoder				
	.global v22_scrambler

	.global Rx_v22A_start_detect
	.global Rx_v22A_train_loops
	.global Rx_v22A_train_EQ
	.global Rx_v22A_message
	.global Rx_v22A_RC_respond
	.global Rx_v22A_RC_initiate
	.global Rx_v22C_start_detect
	.global Rx_v22C_train_loops
	.global Rx_v22C_train_EQ
	.global Rx_v22C_message
	.global Rx_v22C_RC_respond
	.global Rx_v22C_RC_initiate
	.global Rx_init_v22
	.global Rx_v22_train_EQ
	.global Rx_v22_message
	.global Rx_v22_RC_respond
	.global Rx_v22_RC_initiate
	.global v22_descrambler
	.global v22_slicer12
	.global v22_slicer24
	.global v22_startup_timing
	.global v22_diff_decoder
	.global v22_S1_detector
 .endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

;****************************************************************************
;* tables and coefficients
;****************************************************************************

	.sect   "vcoefs"

 .if TX_V22_MODEM=ENABLED
Tx_v22_phase_map:							
	.word 1,0,2,3

Tx_v22_amp_table:
		.word  TX_ONE,	TX_ONE
		.word -TX_ONE,	TX_ONE
		.word -TX_ONE,   -TX_ONE
		.word  TX_ONE,   -TX_ONE

		.word  TX_THREE,  TX_ONE
		.word -TX_ONE,	TX_THREE
		.word -TX_THREE, -TX_ONE
		.word  TX_ONE,   -TX_THREE

		.word  TX_ONE,	TX_THREE
		.word -TX_THREE,  TX_ONE
		.word -TX_ONE,   -TX_THREE
		.word  TX_THREE, -TX_ONE

		.word  TX_THREE,  TX_THREE
		.word -TX_THREE,  TX_THREE
		.word -TX_THREE, -TX_THREE
		.word  TX_THREE, -TX_THREE
 .endif

 .if RX_V22_MODEM=ENABLED
Rx_v22_hard_map:
	.word   0,1,3,2

Rx_v22_phase_map:
	.word   1,0,2,3
 .endif

	.sect   "vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_v22A(struct START_PTRS *)
;* void Tx_init_v22A_ANS(struct START_PTRS *);
;* void Tx_v22A_retrain(struct START_PTRS *);
;* void Tx_init_v22C(struct START_PTRS *)
;* void Tx_v22C_retrain(struct START_PTRS *);
;*
;****************************************************************************

	;*****************************************
	;**** ANSWER side transmitter modules ****
	;*****************************************

 .if TX_V22A_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v22A:
;* C function call: void Tx_init_v22A(struct START_PTRS *)
;* Initializes Tx_block for v22 modulator 
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v22A:					
_V22_MESI_TxInitV22A:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v22A
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v22A:
;* Initializes Tx_block for v22 ANSWER operation.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_v22A:					
V22_MESI_TxInitV22A:
	CALL_	 Tx_init_v22
	ST		#_Tx_RCOS600_f2400,Tx_coef_start
	ST		#TXA_SILENCE2_LEN,Tx_terminal_count
	STPP	#Tx_v22A_silence2,Tx_state,B
	ST		#TX_V22A_SILENCE2_ID,Tx_state_ID

	;**** initialize answer side receiver ***

 .if RX_V22A_MODEM=ENABLED
	MVDK	Tx_start_ptrs,AR3
	LD		*AR3(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_v22A		
	MVDK	Rx_start_ptrs,AR3
	LD		*AR3(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
 .endif
	RET_

;****************************************************************************
;* _Tx_v22A_retrain:
;* C function call: void Tx_v22A_retrain(struct START_PTRS *)
;* Initializes Tx_block for v22 modulator retrain
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v22A_retrain:				
_V22_MESI_TxV22ARetrain:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v22A_retrain
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_v22A_retrain:
;* Initializes Tx_block for v22 modulator retrain
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_v22A_retrain:					
V22_MESI_TxV22ARetrain:
	LD		#0,A
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	ST		#TXA_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22A_S1,Tx_state,B
	ST		#TX_V22A_S1_ID,Tx_state_ID

 .if RX_V22A_MODEM=ENABLED
	MVDK	Tx_start_ptrs,AR3
	MVDK	*AR3(Rx_block_start),AR3
	LD		*AR3(Rx_data_ptr),B
	STL		B,*AR3(Rx_data_tail)		
	LD		#S1_NOT_DETECTED,B
	STL		B,*AR3(Rx_pattern_detect)		;* pattern_detect=S1_NOT_DETECTED
	STL		A,*AR3(Rx_symbol_counter)		;* Rx_symbol_counter=0
	STPP	#Rx_v22A_RC_initiate,*AR3(Rx_state),B
	ST		#RX_V22A_RC_INITIATE_ID,*AR3(Rx_state_ID)
 .endif
	RET_

;****************************************************************************
;* _Tx_init_v22A_ANS:
;* C function call: void Tx_init_v22A_ANS(struct START_PTRS *)
;* Initializes Tx_block for v22 ANS (2100 Hz CED) generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v22A_ANS:				
_V22_MESI_TxInitV22A_ANS:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v22A_ANS
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v22_ANS:
;* Initializes Tx_block for v22 ANSWER operation including ECSD.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_v22A_ANS:				
V22_MESI_TxInitV22A_ANS:
	CALL_	Tx_init_tone_gen
	ST		#TXA_SILENCE1_LEN,Tx_terminal_count
	STPP	#Tx_v22A_silence1,Tx_state,B
	RETD_
	 ST		#TX_V22A_SILENCE1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_silence1:	2.15 sec silence
;****************************************************************************

Tx_v22A_silence1:
	CALL_	Tx_tone_gen
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if sample_counter<TC

	CALL_	Tx_init_CED
	ST		#TXA_ANSWER_TONE_LEN,Tx_terminal_count
	STPP	#Tx_v22A_ANS,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_ANS_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_ANS: 2100 Hz for 3.3 sec.
;****************************************************************************

Tx_v22A_ANS:
	CALL_	Tx_tone_gen

	;**** check for end of segment ****

	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT	

	CALL_	 Tx_init_tone_gen
	ST		#TXA_SILENCE2_LEN,Tx_terminal_count
	STPP	#Tx_v22A_silence2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_SILENCE2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_silence2:	75 msec silence
;****************************************************************************

Tx_v22A_silence2:
	CALL_	Tx_tone_gen
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if sample_counter<TC

	CALL_	 Tx_init_v22A

	;**** synchronize sample[] buffer pointers ****

	MVDK	Tx_start_ptrs,AR3				;* AR3=start_ptrs
	CALL_	Tx_sync_sample_buffers

	ST		#0,Tx_terminal_count
	STPP	#Tx_v22A_UB1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_UB1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_UB1: unscrambled binary 1 @ 1200 bits/sec.
;****************************************************************************

Tx_v22A_UB1:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	 LD		#0dh,B
	CALL_	v22_diff_encoder

	;**** return if counter<TC OR TC<0 ****

	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if terminal_count<0
	SUB		Tx_symbol_counter,B
	BC_		Tx_state_return,BGT				;* return if symbol_counter<LEN

	;**** if end of S1 detected, switch to generate S1 ****

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(Rx_pattern_detect),A
	SUB		#S1_END_DETECTED,A,B
	BC_		Tx_v22A_UB1_endif,BNEQ			;* branch if detect!=S1_END_DETECTED
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	ST		#TXA_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22A_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_S1_ID,Tx_state_ID
Tx_v22A_UB1_endif:

	;**** if SB1_1200 detected, switch to generate SB1_1200 ****/

	SUB		#SB1_270_DETECTED,A,B
	BC_		Tx_state_return,BNEQ			;* return if detect!=SB1_270_DETECTED
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	ST		#TXA_SB1_1200_LEN,Tx_terminal_count
	STPP	#Tx_v22A_SB1_1200,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_SB1_R2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_SB1_1200: scrambled binary 1 @ 1200
;****************************************************************************

Tx_v22A_SB1_1200:
	CALL_	Tx_v22_SB1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v22A_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_S1: S1 alternations for 100 msec.
;****************************************************************************

Tx_v22A_S1:
	CALL_	Tx_v22_S1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TXA_SCR1_LEN,Tx_terminal_count
	STPP	#Tx_v22A_SCR1,Tx_state,B
	BD_		Tx_state_return
	 ST		TX_V22A_SCR1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_SCR1: scrambled R2 dibit @ 1200
;****************************************************************************

Tx_v22A_SCR1:
	CALL_	Tx_v22_SCR_R1R2
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	;**** handle rate change to 1200 ****

	LD		#0,A
	 STL	A,Tx_sample_counter
	 STL	A,Tx_symbol_counter
	STL		A,Tx_Scounter
	LD		Tx_rate,B
	SUB		#V22_RATE_1200,B
	BCD_	Tx_v22A_SB1_endif,BNEQ		;* branch if Tx_rate!=1200
	 ST		#TXA_SB1_R2_LEN,Tx_terminal_count
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STPP	#Tx_v22A_SB1_1200,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_SB1_R2_ID,Tx_state_ID
Tx_v22A_SB1_endif:

	ST		#4,Tx_Nbits		
	ST		#0fh,Tx_Nmask		
	STPP	#Tx_v22A_SB1_R2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_SB1_R2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_SB1_R2: scrambled binary 1 @ R2 rate
;****************************************************************************

Tx_v22A_SB1_R2:
	CALL_	Tx_v22_SB1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	STL		A,Tx_Scounter
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v22A_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22A_message: transmits data
;****************************************************************************

Tx_v22A_message:
	CALL_	Tx_v22_message

	;**** switch to S1 if a rate change request is detected ****

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(Rx_pattern_detect),B
	SUB		#R1_DETECTED,B
	BC_		Tx_state_return,BNEQ			;* branch if pattern_det!=R1_DETECTED
	LD		*AR1(Rx_rate),B
	STL		B,Tx_rate						;* Tx_rate=Rx_rate
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	LD		#0,A
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	ST		#TXA_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22A_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22A_S1_ID,Tx_state_ID

;****************************************************************************
 .endif

	;***************************************
	;**** CALL side transmitter modules ****
	;***************************************

 .if TX_V22C_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v22C:
;* C function call: void Tx_init_v22C(struct START_PTRS *)
;* Initializes Tx_block for v22 modulator 
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v22C:				
_V22_MESI_TxInitV22C:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v22C
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v22C:
;* Initializes Tx_block for v22 CALL operation.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_v22C:				
V22_MESI_TxInitV22C:
	CALL_	 Tx_init_v22
	ST		#_Tx_RCOS600_f1200,Tx_coef_start
	ST		#TXC_SILENCE_LEN,Tx_terminal_count
	STPP	#Tx_v22C_silence,Tx_state,B
	RETD_
	 ST		#TX_V22C_SILENCE_ID,Tx_state_ID

;****************************************************************************
;* _Tx_v22C_retrain:
;* C function call: void Tx_v22C_retrain(struct START_PTRS *)
;* Initializes Tx_block for v22 modulator retrain
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v22C_retrain:				
_V22_MESI_TxV22CRetrain:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v22C_retrain
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_v22C_retrain:
;* Initializes Tx_block for v22 modulator retrain
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_v22C_retrain:					
V22_MESI_TxV22CRetrain:
	LD		#0,A
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	ST		#TXC_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22C_S1,Tx_state,B
	ST		#TX_V22C_S1_ID,Tx_state_ID

 .if RX_V22C_MODEM=ENABLED
	MVDK	Tx_start_ptrs,AR3
	MVDK	*AR3(Rx_block_start),AR3
	LD		*AR3(Rx_data_ptr),B
	STL		B,*AR3(Rx_data_tail)		
	LD		#S1_NOT_DETECTED,B
	STL		B,*AR3(Rx_pattern_detect)		;* pattern_detect=S1_NOT_DETECTED
	STL		A,*AR3(Rx_symbol_counter)		;* Rx_symbol_counter=0
	STPP	#Rx_v22C_RC_initiate,*AR3(Rx_state),B
	ST		#RX_V22C_RC_INITIATE_ID,*AR3(Rx_state_ID)
 .endif
	RET_

;****************************************************************************
;* Tx_v22C_silence:
;****************************************************************************

Tx_v22C_silence:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_		Tx_state_return,BNEQ		;* branch if Tx_fir_head!=Tx_fir_tail

	;**** wait until UB1_has been detected ****

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	LD		#0,A
	STL		A,*AR7+%
	STL		A,*AR7+%
	LD		*AR1(Rx_state_ID),B
	SUB		#RX_V22C_START_DETECT_ID,B
	BCD_	Tx_state_return,BLT			;* return if state<START_DETECT
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head

	;**** stay silent for TXC_SILENCE_LEN ****

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	;**** synchronize sample[] buffer pointers ****

	MVDK	Tx_start_ptrs,AR3				;* AR3=start_ptrs
	CALL_	Tx_sync_sample_buffers
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	LD		Tx_rate,B
	SUB		#V22_RATE_2400,B
	BC_		Tx_v22C_silence_endif,BEQ		;* branch if rate=2400
	ST		#TXC_SB1_1200_LEN,Tx_terminal_count
	STPP	#Tx_v22C_SB1_1200,Tx_state,B
	BD_		 Tx_state_return
	 ST		#TX_V22C_SB1_R2_ID,Tx_state_ID
Tx_v22C_silence_endif:

	ST		#TXC_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22C_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_S1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22C_SB1_1200: SB1 @ 1200
;****************************************************************************

Tx_v22C_SB1_1200:
	CALL_	Tx_v22_SB1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v22C_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22C_S1: +/- 90 degree alternations
;****************************************************************************

Tx_v22C_S1:
	CALL_	Tx_v22_S1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TXC_SCR1_LEN,Tx_terminal_count
	STPP	#Tx_v22C_SCR1,Tx_state,B
	BD_		Tx_state_return
	 ST		TX_V22C_SCR1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22C_SCR1: scrambled binary 1 at R1 rate
;****************************************************************************

Tx_v22C_SCR1:
	CALL_	Tx_v22_SCR_R1R2
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if symbol_counter<LEN

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(Rx_state_ID),B
	SUB		#RX_V22C_START_DETECT_ID,B
	BC_		Tx_v22C_SCR1_endif,BNEQ			;* branch if Rx_state_ID!=START_DETECT
	BD_		Tx_state_return			
	 ST		#0,Tx_symbol_counter
Tx_v22C_SCR1_endif:

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	;**** handle rate change to 1200 ****

	LD		#0,A
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	STL		A,Tx_Scounter
	LD		Tx_rate,B
	SUB		#V22_RATE_1200,B
	BCD_	Tx_v22C_SB1_endif,BNEQ			;* branch if Tx_rate!=1200
	 ST		#TXA_SB1_R2_LEN,Tx_terminal_count
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STPP	#Tx_v22C_SB1_1200,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_SB1_R2_ID,Tx_state_ID
Tx_v22C_SB1_endif:

	ST		#4,Tx_Nbits		
	ST		#0fh,Tx_Nmask		
	STPP	#Tx_v22C_SB1_R2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_SB1_R2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22C_SB1_R2: SB1 @ R2 rate
;****************************************************************************

Tx_v22C_SB1_R2:
	CALL_	Tx_v22_SB1
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	STL		A,Tx_Scounter
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v22C_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v22C_message: data
;****************************************************************************

Tx_v22C_message:
	CALL_	Tx_v22_message

	;**** switch to S1 if a rate change request is detected ****

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(Rx_pattern_detect),B
	SUB		#R1_DETECTED,B
	BC_		Tx_state_return,BNEQ			;* branch if pattern_det!=R1_DETECTED
	LD		*AR1(Rx_rate),B
	STL		B,Tx_rate						;* Tx_rate=Rx_rate
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	LD		#0,A
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	ST		#TXC_S1_LEN,Tx_terminal_count
	STPP	#Tx_v22C_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V22C_S1_ID,Tx_state_ID

;****************************************************************************
 .endif

	;************************************
	;**** common transmitter modules ****
	;************************************

 .if TX_V22_MODEM=ENABLED
;****************************************************************************
;* Tx_init_v22:
;* Initializes Tx_block[] workspace for v22 modulator operation.
;* On entry it expects:
;*	DP=&Tx_block
;* Modifies:
;****************************************************************************

Tx_init_v22:
	LD		#0,A
	ST		#(2*TX_INTERP),Tx_interpolate	 
	ST		#(2*TX_DEC),Tx_decimate		 
	STL		A,Tx_sym_clk_offset				;* Tx_sym_clk_offset=0;
	STL		A,Tx_sym_clk_memory				;* Tx_sym_clk_memory=0;
	STL		A,Tx_sym_clk_phase				;* Tx_sym_clk_phase=0;
	STL		A,Tx_carrier	
	ST		#Tx_v22_phase_map,Tx_map_ptr	
	ST		#Tx_v22_amp_table,Tx_amp_ptr	
	ST		#TX_V22_SCALE,Tx_fir_scale	

	MVDK	Tx_start_ptrs,AR0				;* AR0=start_ptrs
	LD		*AR0(Tx_fir_start),B
	STL		B,Tx_fir_head					;* Tx_fir_head=&Tx_fir[0]
	STL		B,Tx_fir_tail					;* Tx_fir_tail=&Tx_fir[0]
	ST		#TX_FIR_LEN,Tx_fir_len

	MVDK	Tx_fir_head,AR0
	STM		#(TX_FIR_LEN-1),BRC
	RPTB	v22_init_Tx_fir_loop
v22_init_Tx_fir_loop:
	 STL	A,*AR0+							;* Tx_fir[*++]=0

	ST		#(TX_FIR_TAPS-1),Tx_fir_taps		
	STL		A,Tx_coef_ptr	
	STL		A,Tx_phase		
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STL		A,Tx_symbol_counter 
	STL		A,Tx_sample_counter 
	STL		A,Tx_terminal_count 
	STL		A,Tx_Scounter		 

	LD		Tx_rate,B
	SUB		#V22_RATE_1200,B
	 STL	A,Tx_Sreg		 
	 STL	A,Tx_Sreg_low	 
	XC		2,BNEQ
	 ST		#V22_RATE_2400,Tx_rate
	RET_

;****************************************************************************
;* Tx_v22_S1: Generates +/- 90 degree alternations
;****************************************************************************

Tx_v22_S1:						
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	RC_		BNEQ							;* return if Tx_fir_head!=Tx_fir_tail

	;**** +/- 90 degree alternations ****

	 LD		Tx_symbol_counter,A
	 AND	#1,A							;* A=Tx_symbol_counter&1
	  LD	#0dh,B
	CALLD_	v22_diff_encoder
	 XC		1,AEQ
	  LD	#1,B							;* if (counter&1)=0, B=1
	RET_

;****************************************************************************
;* Tx_v22_SCR_R1R2: Modulates scrambled dibit R1 or R2 at 1200 bits/sec.	*/ 
;* R1 and R2 are defined as:												
;*	01 at 1200 bits/sec.													
;*	11 at 2400 bits/sec.
;****************************************************************************

Tx_v22_SCR_R1R2:					
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	RC_		BNEQ							;* return if Tx_fir_head!=Tx_fir_tail

	;**** scrambled 1 at 1200 bits/sec. ****

	LD		Tx_rate,B
	SUB		#V22_RATE_2400,B				;* rate-RATE_2400
	LD		#1,A
	CALLD_	v22_scrambler
	 XC		1,BEQ
	  LD	#3,A							;* if rate=2400, A=3
	SFTL	B,2								;* B=(symbol<<2)
	CALLD_	v22_diff_encoder
	 OR		#1,B							;* B=(symbol<<2)|1
	RET_

;****************************************************************************
;* Tx_v22_SB1: Generates scrambled 1 at specified rate
;****************************************************************************

Tx_v22_SB1:					
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	RC_		BNEQ							;* return if Tx_fir_head!=Tx_fir_tail

	;**** scrambled 1 at R2 rate ****

	LD		Tx_Nmask,A
	LD		Tx_rate,B
	SUB		#V22_RATE_2400,B				;* rate-RATE_2400
	BC_		Tx_v22_SB1_endif,BEQ			;* branch if rate=2400
	CALL_	v22_scrambler
	SFTL	B,2								;* B=(symbol<<2)
	CALLD_	v22_diff_encoder
	 OR		#1,B							;* B=(symbol<<2)|1
	RET_
Tx_v22_SB1_endif:

	CALL_	v22_scrambler
	CALL_	v22_diff_encoder
	RET_

;****************************************************************************
;* Tx_v22_message: Generates message data at specified rate
;****************************************************************************

Tx_v22_message:					
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	RC_		BNEQ							;* return if Tx_fir_head!=Tx_fir_tail

	;**** test for Tx_data[] underflow ****

	LDU		Tx_data_head,B
	SUBS	Tx_data_tail,B
	 MVDK	Tx_data_tail,AR7
	MVDK	Tx_data_len,BK
	LD		#0fh,A				
	XC		1,BNEQ							;* if head!=tail ...
	 LD		*AR7+%,A						;* ... A=Tx_data[*++%]

	;**** scramble the data ****

	LD		Tx_rate,B
	SUB		#V22_RATE_2400,B				;* rate-RATE_2400
	BCD_		Tx_v22_message_endif,BEQ	;* branch if rate=2400
	 MVKD	AR7,Tx_data_tail
	CALL_	v22_scrambler
	SFTL	B,2								;* B=(symbol<<2)
	CALLD_	v22_diff_encoder
	 OR		#1,B							;* B=(symbol<<2)|1
	RET_

Tx_v22_message_endif:
	CALL_	v22_scrambler
	CALL_	v22_diff_encoder
	RET_

;****************************************************************************
;* v22_diff_encoder: differentially encodes the data bits for v22.
;* Expects the following on entry:
;*	B=symbol
;****************************************************************************

v22_diff_encoder:				
 .if ON_CHIP_COEFFICIENTS=ENABLED
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	SFTL	B,-2,A							;* A=symbol>>2
	AND		#3,A							;* A=(symbol>>2)&3
	ADDS	Tx_map_ptr,A					;* A=map_ptr+(symbol>>2)&3
 	STLM	A,AR0							;* AR0=map_ptr+(symbol>>2)&3
	 AND	#3,B							;* B=symbol&3
	 SFTL	B,2								;* B=(symbol&3)<<2
	LD		*AR0,A							;* A=remap=*(map_ptr+(symbol>>2)&3)
	ADD		Tx_carrier,A					;* A=carrier+remap
	ADD		Tx_phase,A						;* A=phase+remap
	AND		#(TX_PHASE_LEN-1),A				;* A=(phase+remap)&(TX_PHASE_LEN-1)
	STL		A,Tx_phase						;* update phase
	OR		Tx_phase,B						;* B=phase|((symbol&3)<<2)
	SFTL	B,1,A							;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*k
	STLM	A,AR0							;* AR0=amp_ptr+2*phase
	 NOP
	 LD		Tx_fir_scale,T			
	MPY		*AR0+,B							;* B=fir_scale*real
	STH		B,*AR7+%
	MPY		*AR0,B							;* B=fir_scale*imag
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head			   		;* update Tx_fir_head
 .else
	SFTL	B,-2,A							;* A=symbol>>2
	AND		#3,A							;* A=(symbol>>2)&3
	ADDS	Tx_map_ptr,A					;* A=map_ptr+(symbol>>2)&3
	STM		#AL,AR7							;* AR7=&AL
	READA	*AR7							;* AL= remap=*(map_ptr+(symbol>>2)&3)
	ADD		Tx_carrier,A					;* A=carrier+remap
	ADD		Tx_phase,A						;* A=phase+remap
	AND		#(TX_PHASE_LEN-1),A				;* A=(phase+remap)&(TX_PHASE_LEN-1)
	STL		A,Tx_phase						;* update phase

	AND		#3,B							;* B=symbol&3
	SFTL	B,2,A							;* A=(symbol&3)<<2
	OR		Tx_phase,A						;* A=phase|((symbol&3)<<2)
	SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*k
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	READA	*AR7							;* Tx_fir[]=*(amp_ptr+2*k)
	LD		Tx_fir_scale,T				
	MPY		*AR7,B							;* B=fir_scale* *(amp_ptr+2*k)
	STH		B,*AR7+%
	ADD		#1,A				
	READA	*AR7				
	MPY		*AR7,B							;* B=fir_scale* *(amp_ptr+2*k+1)
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
 .endif

;****************************************************************************
;* v22_scrambler: scrambles according to v22.
;* Expects the following on entry:
;*	A=in
;* On exit:
;*	B=out
;****************************************************************************

v22_scrambler:
	LD		Tx_Nbits,B
	SUB		#1,B
	STLM	B,BRC							;* BRC=Tx_Nbits-1
	STLM	A,AR0							;* AR0=in
	 STM	#0,AR1							;* AR1= out=0
	RPTB	v22_scrambler_end
	
	LDM		BRC,A							;* A=i
	NEG		A
	STLM	A,T								;* T=-i
	 LD		Tx_Scounter,A
	 SUB	#64,A
	LD		*(AR0),TS,B						;* B= j=(in>>i)		
	XC		2,AEQ							;* if Scounter=64
	 XOR	#1,B							;* ... B= j^1
	XC		2,AEQ							;* if Scounter=64 ...
	 ST		#0,Tx_Scounter					;* ... Scounter=0
	LD		Tx_Sreg_low,-13,A				;* A=Sreg>>(14-1)
	XOR		Tx_Sreg,A						;* A=Sreg>>(14-1)^Sreg>>(17-1)
	XOR		A,B								;* B= k=j^Sreg>>(14-1)^Sreg>>(17-1)
	AND		#1,B							;* B= k&1
	
	LD		Tx_Sreg,16,A
	ADDS	Tx_Sreg_low,A					;* A=Sreg
	SFTL	A,1								;* A=Sreg<<1
	OR		B,A								;* A=(Sreg<<1)|k
	STH		A,Tx_Sreg						;* update Sreg
	STL		A,Tx_Sreg_low				

	LDM		BRC,A
	STLM	A,T								;* T=i
	
	SUB		#1,B,A							;* A= k-1
	 ADDM	#1,Tx_Scounter					;* Scounter++
	XC		2,ANEQ							;* if k!=1 ...
	 ST		#0,Tx_Scounter					;* ... Scounter=0
	NORM	B								;* B= (k<<i)
	OR		*(AR1),B						;* B= out|(k<<1)
v22_scrambler_end:
	STLM	B,AR1							;* AR1=out
	RET_

;****************************************************************************
 .endif

	;**************************************
	  ;**** ANSWER side receiver modules ****
	;**************************************

 .if RX_V22A_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v22A: 
;* Initializes Rx_block[] workspace for ANSWER operation as follows:
;****************************************************************************

Rx_init_v22A:					
	CALL_	Rx_init_v22
	LD		#0,A
	ST		#_Rx_RCOS600_f1200,Rx_coef_start	
	ST		#LO_PHASE_ADJ_1200,LO_phase
	ST		#NOT_DETECTED,Rx_pattern_detect
	ST		#8192,agc_gain
	ST		#V22_SNR_EST_COEF,SNR_est_coef
	ST		#V22_SNR_THR_COEF,SNR_thr_coef		
	STPP	#Rx_v22A_start_detect,Rx_state,B
	RETD_
	 ST		#RX_V22A_START_DETECT_ID,Rx_state_ID

;****************************************************************************
;* Rx_v22A_start_detect: detects startup (S1 or SB1_1200).
;****************************************************************************

Rx_v22A_start_detect:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_	Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail
	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	LD		*AR7+%,A						;* A=Rx_data[*++%] (symbol)
	MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** check for v32 AA presence ****

	MVMM	AR2,AR7							;* AR7=Rx_sample_tail
	MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len
	SSBX	TC								;* enable filter
	BPF		FILTER_1800_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,temp0							;* temp0=P1800
	SUB		Rx_threshold,A,B				;* compare with min level threshold
	BC_		AA_detector_endif,ALT			;* branch if P1800<THR

	;**** broadband estimator ****

	BB_EST V22_BB_EST_COEF,V22_BB_EST_LEN,AR7
	STH		A,temp1							;* temp0=Pbb
	SSBX	TC								;* enable filter
	BPF		FILTER_2250_COEF,RX_ANALYSIS_LEN,AR7
	LD		temp1,16,B
	STM		#ONE_BY_ROOT2,T
	MASA	T,B								;* B=Pbb-F2250*ONE_BY_ROOT2
	STH		B,temp1
	RATIO	temp0,temp1,THR_10DB			;* F1800/Pbb ratio
	 NOP	
	 NOP
	XC		2,BGT							;* if P1800/Pbb>THR ...
	 ST		#V32_AA_DETECTED,Rx_status		;* status=AA_DETECTED
AA_detector_endif:

	;**** wait to fill the filter ****

	LD		Rx_symbol_counter,B
	SUB		#(2*V22_SNR_EST_LEN),B
	BC_		Rx_state_return,BLT				;* return if counter<2*V22_SNR_EST_LEN

	;**** complex differential energy detector ****

	STM		#RX_FIR_LEN,BK
	MVDK	Rx_fir_ptr,AR4
	STM		#-2,AR0
	MAR		*AR4+0%							;* AR4=(Rx_fir_ptr-2)%LEN
	MVMM	AR4,AR3							;* AR3=AR4
	MAR		*+AR3(-4*V22_SNR_EST_LEN)%		;* AR4=(Rx_fir_ptr-2-4*V22_SNR_EST_LEN)%LEN
	STM		#(V22_SNR_EST_LEN-1),BRC
	RPTBD	Rx_v22A_detector_loop	
	LD		#4000h,1,B						;* load round value into B
	 MPY	*AR4(1),#V22_SNR_THR_COEF,A		;* A=Rx_fir[l+1]*THR_COEF
	 ABS	A
	 ADD	A,B
	 MPY	*AR4+0%,#V22_SNR_THR_COEF,A		;* A=Rx_fir[l]*THR_COEF
	 ABS	A
	 ADD	A,B
	 MPY	*AR3(1),#V22_SNR_EST_COEF,A		;* A=Rx_fir[k+1]*EST_COEF
	 ABS	A
	 SUB	A,B								;* B-=A	
	 MPY	*AR3+0%,#V22_SNR_EST_COEF,A		;* A=Rx_fir[k]*EST_COEF
	 ABS	A
Rx_v22A_detector_loop: 
	 SUB	A,B								;* B-=A	
	SFTA	B,-16
	BCD_		Rx_state_return,BLEQ		;* return if no signal is detected 

	;**** estimate power and seed agc_gain ****

	 MVDK	Rx_sample_len,BK
	MVMM	AR2,AR7				
	STM		#(V22_SNR_EST_LEN-1),BRC
	MAR		*AR7-%							;* AR7=l=Rx_sample_tail-1
	MVMM	AR7,AR6				
	LD		#0,A
	RPTBD	Rx_v22A_power_loop	
	MAR		*+AR6(-2*V22_SNR_EST_LEN)%		;* AR6=k=Rx_sample_tail-SNR_EST_LEN-1
	 SQURA	*AR7-%,A
Rx_v22A_power_loop:
	 SQURS	*AR6-%,A				

	SFTA	A,-V22_SNR_EST_SHIFT
	 STH	A,Rx_power
	   NOP
	XC		2,ALT							;* if A<0 ...
	 ST		#0,Rx_power						;* Rx_power=0
	CALL_	 agc_gain_estimator

	ST		#ACQ_AGC_K,agc_K
	STPP	#v22_startup_timing,timing_ptr,B
	LD		#0,A
	ST		#V22_ACQ_LOOP_K1,loop_K1			
	ST		#V22_ACQ_LOOP_K2,loop_K2			
	STL		A,vco_memory					;* vco_memory=0
	STL		A,loop_memory					;* loop_memory=0
	STL		A,loop_memory_low				;* loop_memory=0
	STL		A,frequency_est
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,LOS_monitor					;* LOS_monitor=0
	ST		#0aaaah,Rx_pattern_reg
	STL		A,Rx_sample_counter				;* sample_counter=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	STL		A,Rx_status						;* status=0 (OK)
	STPP	#Rx_v22A_train_loops,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22A_TRAIN_LOOPS_ID,Rx_state_ID

;****************************************************************************
;* Rx_v22A_train_loops: trains carrier and symbol timing loops.
;****************************************************************************

Rx_v22A_train_loops:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_	Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail

	;**** check for timeout failure ****

	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7
	LD		Rx_symbol_counter,B
	SUB		#TRAIN_LOOPS_TIMEOUT,B
	BC_		 v22A_train_loops_while,BLEQ	;* branch if counter<=TIMEOUT

	CALL_	 Rx_init_detector
 .if TX_V22A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_silence
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	BD_		Rx_state_return
	 ST		#TRAIN_LOOPS_FAILURE,Rx_status

v22A_train_loops_while:
	LD		*AR7,A							;* A=symbol
	CALLD_	v22_descrambler
	 STM	#(2-1),BRC					
	LD		Rx_pattern_reg,2,A
	OR		B,A
	STL		A,Rx_pattern_reg
	LD		*AR7+%,B						;* B=symbol
	LDM		AR7,A							;* A=Rx_data_tail
	SUBS	Rx_data_head,A
	BCD_	v22A_train_loops_while,ANEQ
	 MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** look for S1 end detected (2400 startup) ****

	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR1		;* AR0=Tx_block_start
	LD		*AR1(Tx_rate),A
	SUB		#V22_RATE_1200,A
	BC_		Rx_v22A_S1_detect_endif,AEQ		;* branch if Tx_rate=1200

	CALL_	v22_S1_detector
	LD		Rx_pattern_detect,B
	SUB		#S1_DETECTED,B,A
	BC_		Rx_state_return,AEQ				;* return if detect=S1_DETECTED
	SUB		#S1_END_DETECTED,B,A
	  LD	*AR1(Tx_state_ID),B
	  SUB	#TX_V22A_UB1_ID,B
	XC		2,AEQ							;* if S1_END_DETECTED ...
	 ST		#6666h,Rx_pattern_reg			;* pattern_reg=0x6666
	BC_		Rx_v22A_S1_detect_endif,BEQ		;* branch if ID=UB1

	LD		#0,A
	STPP	#sgn_timing,timing_ptr,B
	STL		A,*AR1(Tx_symbol_counter)		;* Tx_symbol_counter=0
	STL		A,S1_memory						;* S1_memory=0
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,agc_K							;* agc_K=0
	STL		A,data_Q1						;* data_Q1=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#Rx_v22A_train_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22A_TRAIN_EQ_ID,Rx_state_ID
Rx_v22A_S1_detect_endif:

	;**** look for scrambled binary 1 or 0 (1200 startup) ****

	LD		Rx_pattern_detect,B
	SUB		#SB1_DETECTED,B,A
	 NOP
	 NOP
	XC		2,AEQ							;* if pattern=SB1_DETECTED ...
	 ST		#V22_RATE_1200,*AR1(Tx_rate)	;* ... Tx_rate=1200
	BC_		Rx_v22A_SB1_detect_endif,AEQ	;* branch if detect=SB1_DETECTED
	SUB		#SB1_270_DETECTED,B,A
	BC_		Rx_v22A_SB1_detect_endif,AEQ	;* branch if detect=SB1_270_DETECTED
	LDU		Rx_pattern_reg,A
	BC_		Rx_v22A_SB1_detected,AEQ		;* branch if pattern_reg=0
	XOR		#SB1_PATTERN,A
	BC_		Rx_state_return,ANEQ			;* return if pattern_reg!=SB1_pattern
Rx_v22A_SB1_detected:
	
	STPP	#sgn_timing,timing_ptr,B
	LD		#0,A
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,agc_K							;* agc_K=0
	STL		A,data_Q1						;* data_Q1=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#V22_SB1_DETECTED,Rx_status
	BD_		Rx_state_return
	 ST		#SB1_DETECTED,Rx_pattern_detect
Rx_v22A_SB1_detect_endif:

	;**** check for SB1_270 timeout ****

	LD		Rx_symbol_counter,A
	SUB		#SB1_270_TIMEOUT,A,B
	 SUB	#SB1_765_TIMEOUT,A
	XC		2,BEQ							;* if sym_counter=270_TIMEOUT
	 ST		#SB1_270_DETECTED,Rx_pattern_detect
	BC_		Rx_state_return,ALT				;* return if counter<TIMEOUT
	
	LD		#0,A
	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
	ST		#TRK_EQ_2MU,EQ_2mu
	ST		#TRK_TIMING_THR,timing_threshold
	ST		#V22_TRK_LOOP_K1,loop_K1
	ST		#V22_TRK_LOOP_K2,loop_K2
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#Rx_v22A_message,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22A_MESSAGE_ID,Rx_state_ID
			  
;****************************************************************************
;* Rx_v22A_train_EQ: trains equalizer.
;****************************************************************************

Rx_v22A_train_EQ:
	CALL_	Rx_v22_train_EQ
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_train_EQ()!=2
	
	ST		#RX_V22A_MESSAGE_ID,Rx_state_ID
	STPP	#Rx_v22A_message,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22A_message: processes data.
;****************************************************************************

Rx_v22A_message:
	CALL_	Rx_v22_message
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_message()!=2
	
	ST		#RX_V22A_RC_RESPOND_ID,Rx_state_ID
	STPP	#Rx_v22A_RC_respond,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22A_RC_respond: responds to rate change request
;****************************************************************************

Rx_v22A_RC_respond:
	CALL_	Rx_v22_RC_respond
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_RC_respond()!=2
	
	ST		#RX_V22A_TRAIN_EQ_ID,Rx_state_ID
	STPP	#Rx_v22A_train_EQ,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22A_RC_initiate: tracks rate change initiation.
;****************************************************************************

Rx_v22A_RC_initiate:
	CALL_	Rx_v22_RC_initiate
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_RC_initiate()!=2
	
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR1		;* AR0=Tx_block_start
	ST		#0,*AR1(Tx_symbol_counter)
	ST		#RX_V22A_TRAIN_EQ_ID,Rx_state_ID
	STPP	#Rx_v22A_train_EQ,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
 .endif

	;************************************
	;**** CALL side receiver modules ****
	;************************************

 .if RX_V22C_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v22C: 
;* Initializes Rx_block[] workspace for CALL operation 
;****************************************************************************

Rx_init_v22C:					
V22_MESI_RxInitV22C:
	CALL_	Rx_init_v22
	
	;**** set up equalizer filter for signal detection ****

	ST		#EQ_FIR_ENABLED,EQ_2mu
	ST		#(RXC_ANALYSIS_LEN-1),EQ_taps
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(EQ_coef_start),AR4
	LD		#(RXC_ANALYSIS_COEF),B
	STM		#(RXC_ANALYSIS_LEN-1),BRC
	LD		#0,A
	RPTB	Rx_init_v22C_loop				;* for RXC_ANALYSIS_LEN ...
	 STL	B,*AR4+						;* ...EQ_coef[2*i]=RXC_ANALYSIS_COEF
Rx_init_v22C_loop:				
	 STL	A,*AR4+						;* ...EQ_coef[2*i+1]=0;

	;**** estimate agc_gain ****

	CALL_	 agc_gain_estimator
	 
	ST		#_Rx_RCOS600_f2400,Rx_coef_start	
	ST		#LO_PHASE_ADJ_2400,LO_phase
	ST		#UB1_DETECTED,Rx_pattern_detect
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR3		;* AR3=Tx_block_start
	ST		#0,*AR3(Tx_symbol_counter)
	STPP	#Rx_v22C_start_detect,Rx_state,B
	RETD_
	 ST		#RX_V22C_START_DETECT_ID,Rx_state_ID

;****************************************************************************
;* Rx_v22C_start_detect: searches for S1 or SB1_1200.
;****************************************************************************

Rx_v22C_start_detect:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_	Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail
	
	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	 LD		*AR7+%,A						;* A=Rx_data[*++%] (symbol)
	MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** AGC ****

	STM		#RX_FIR_LEN,BK
	MVDK	Rx_fir_ptr,AR4
	MAR		*+AR4(-2)%						;* (Rx_fir_ptr-=2)%
	SQUR	*+AR4(-1)%,A					;* A=Q^2
	SQURA	*+AR4(-1)%,A					;* A=I^2+Q^2
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	SFTA	A,6								;* A=(AGC_REF-I^2-Q^2)*64 
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	SFTA	A,OP_POINT_SHIFT				;* A=(AGC_REF-I^2-Q^2)<< OP_POINT_SHIFT
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	SUB		#AGC_REF,16,A					;* AH=AGC_REF
	STM		#-ACQ_AGC_K,T
	MPYA	B								;* B=AGC_K*(A)
	ADD		agc_gain,16,B					;* B+=agc_gain
	 LD		Rx_symbol_counter,A
	 SUB	#RXC_ANALYSIS_LEN,A
	XC		1,BLT			
	 LD		#0,B							;* clamp if agc_gain<0
	STH		B,agc_gain						;* update agc_gain
	BC_		Rx_state_return,ALT				;* return if counter<RXC_ANALYSIS_LEN

	;**** calculate filtered magnitude ****

	LD		IEQ,A
	LD		QEQ,B
	ABS		A
	ABS		B
	STH		A,temp0
	MAX		A								;* B=max(x,y)
	 NOP						
	 NOP						
	XC		1,C								;* if A=B=max
	 LD		temp0,16,B						;* if B=max, A=min
	ADD		B,-1,A							;* B=abs(max)+abs(min)/2
	SUB		#RXC_DETECT_THRESHOLD,A,B
	BC_		Rx_state_return,BLT				;* return if mag<THR

	LD		#0,A
	MVDK	Rx_start_ptrs,AR0			
	ST		#EQ_DISABLED,EQ_2mu
	ST		#(V22_EQ_LEN-1),EQ_taps
	MVDK	*AR0(EQ_coef_start),AR1
	STM		#(2*V22_EQ_LEN-1),BRC
	RPTB	v22C_init_EQ_coef_loop
v22C_init_EQ_coef_loop:
	 STL	A,*AR1+							;* EQ_coef[*++]=0

	MVDK	*AR0(EQ_coef_start),AR1
	LD		#EQ_COEF_SEED,B
	STL		B,*AR1(V22_EQ_LEN-1)			;* NOTE: different EQ implementation from vsim
	ST		#UB1_END_DETECTED,Rx_pattern_detect
	ST		#ACQ_AGC_K,agc_K
	STPP	#v22_startup_timing,timing_ptr,B
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	ST		#V22_ACQ_LOOP_K1,loop_K1			
	ST		#V22_ACQ_LOOP_K2,loop_K2			
	STL		A,vco_memory					;* vco_memory=0
	STL		A,loop_memory					;* loop_memory=0
	STL		A,loop_memory_low				;* loop_memory=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,LOS_monitor					;* LOS_monitor=0
	STPP	#Rx_v22C_train_loops,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22C_TRAIN_LOOPS_ID,Rx_state_ID

;****************************************************************************
;* Rx_v22C_train_loops: carrier loop acquisition.
;****************************************************************************

Rx_v22C_train_loops:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_		Rx_state_return,BEQ			;* branch if Rx_data_head==Rx_data_tail

	;**** check for timeout failure ****

	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7
	LD		Rx_symbol_counter,B
	SUB		#TRAIN_LOOPS_TIMEOUT,B
	BC_		 v22C_train_loops_while,BLEQ	;* branch if counter<=TIMEOUT

	CALL_	 Rx_init_detector
 .if TX_V22C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_silence
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif

	BD_		Rx_state_return
	 ST		#TRAIN_LOOPS_FAILURE,Rx_status

v22C_train_loops_while:
	LD		*AR7,A							;* A=symbol
	CALLD_	v22_descrambler
	 STM	#(2-1),BRC					
	LD		Rx_pattern_reg,2,A
	OR		B,A
	STL		A,Rx_pattern_reg
	LD		*AR7+%,B						;* B=symbol
	LDM		AR7,A							;* A=Rx_data_tail
	SUBS	Rx_data_head,A
	BCD_		v22C_train_loops_while,ANEQ
	 MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** look for S1 pattern ****

	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR1		;* AR0=Tx_block_start
	LD		*AR1(Tx_rate),A
	SUB		#V22_RATE_1200,A
	BC_		Rx_v22C_S1_detect_endif,AEQ		;* branch if Tx_rate=1200

	CALL_	v22_S1_detector
	LD		Rx_pattern_detect,B
	SUB		#S1_DETECTED,B,A
	BC_		Rx_state_return,AEQ				;* return if detect=S1_DETECTED
	SUB		#S1_END_DETECTED,B,A
	BC_		Rx_v22C_S1_detect_endif,ANEQ	;* branch if detect!=S1_DETECTED

	STPP	#sgn_timing,timing_ptr,B
	LD		#0,A
	STL		A,*AR1(Tx_symbol_counter)		;* Tx_symbol_counter=0
	STL		A,S1_memory						;* S1_memory=0
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,agc_K							;* agc_K=0
	STL		A,data_Q1						;* data_Q1=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#Rx_v22C_train_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22C_TRAIN_EQ_ID,Rx_state_ID
Rx_v22C_S1_detect_endif:

	;**** look for scrambled binary 1 (1200 bits/sec.) ****

	LD		Rx_pattern_detect,B
	SUB		#SB1_DETECTED,B
	BC_		Rx_v22C_SB1_detect_endif,BEQ	;* branch if !SB1_DETECTED
	LDU		Rx_pattern_reg,A
	XOR		#SB1_PATTERN,A
	BC_		Rx_state_return,ANEQ			;* return if pattern_reg!=SB1_pattern
	STPP	#sgn_timing,timing_ptr,B
	LD		#0,A
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,agc_K							;* agc_K=0
	STL		A,data_Q1						;* data_Q1=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#V22_SB1_DETECTED,Rx_status
	BD_		Rx_state_return
	 ST		#SB1_DETECTED,Rx_pattern_detect
Rx_v22C_SB1_detect_endif:

	;**** check for SB1_270 timeout ****

	LD		Rx_symbol_counter,A
	SUB		#SB1_270_TIMEOUT,A,B
	BC_		Rx_state_return,BLT				;* return if counter<TIMEOUT

	LD		#0,A
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR1		;* AR0=Tx_block_start
	STL		A,*AR1(Tx_symbol_counter)		;* Tx_symbol_counter=0
	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
	ST		#TRK_EQ_2MU,EQ_2mu
	ST		#TRK_TIMING_THR,timing_threshold
	ST		#V22_TRK_LOOP_K1,loop_K1
	ST		#V22_TRK_LOOP_K2,loop_K2
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#Rx_v22C_message,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V22C_MESSAGE_ID,Rx_state_ID

;****************************************************************************
;* Rx_v22C_train_EQ: trains equalizer.
;****************************************************************************

Rx_v22C_train_EQ:
	CALL_	Rx_v22_train_EQ
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_train_EQ()!=2
	
	ST		#RX_V22C_MESSAGE_ID,Rx_state_ID
	STPP	#Rx_v22C_message,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22C_message: processes data
;****************************************************************************

Rx_v22C_message:
	CALL_	Rx_v22_message
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_message()!=2
	
	ST		#RX_V22C_RC_RESPOND_ID,Rx_state_ID
	STPP	#Rx_v22C_RC_respond,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22C_RC_respond: responds to rate change request
;****************************************************************************

Rx_v22C_RC_respond:
	CALL_	Rx_v22_RC_respond
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_RC_respond()!=2
	
	ST		#RX_V22C_TRAIN_EQ_ID,Rx_state_ID
	STPP	#Rx_v22C_train_EQ,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
;* Rx_v22C_RC_initiate: processes a rate change initiation
;****************************************************************************

Rx_v22C_RC_initiate:
	CALL_	Rx_v22_RC_initiate
	SUB		#2,A,B
	BC_		Rx_state_return,BNEQ			;* return if Rx_v22_RC_initiate()!=2
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Tx_block_start),AR1		;* AR0=Tx_block_start
	ST		#0,*AR1(Tx_symbol_counter)
	
	ST		#RX_V22C_TRAIN_EQ_ID,Rx_state_ID
	STPP	#Rx_v22C_train_EQ,Rx_state,B
	B_		Rx_state_return

;****************************************************************************
 .endif

	;*********************************
	;**** common receiver modules ****
	;*********************************

 .if RX_V22_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v22:
;* Initializes Rx_block[] workspace for v22 demodulator operation.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_v22:
	LD		#0,A
	ST		#_Rx_timing600,Rx_timing_start	
	ST		#RX_DEC,Rx_decimate
	ST		#(2*RX_INTERP),Rx_interpolate
	ST		#(RX_DEC/2),Rx_sym_clk_phase	
	ST		#RX_OVERSAMPLE,Rx_oversample
	STL		A,Rx_pattern_reg				;* pattern_reg=0
	STL		A,data_Q1						;* data_Q1=0
	STL		A,Rx_map_shift					;* map_shift=0
	STPP	#v22_diff_decoder,decoder_ptr,B
	STPP	#v22_slicer12,slicer_ptr,B
	STPP	#no_timing,timing_ptr,B

	MVDK	Rx_start_ptrs,AR0			
	LD		Rx_data_head,B
	STL		B,Rx_data_tail
	STL		A,Rx_coef_ptr					;* Rx_coef_ptr=0
	LD		Rx_sample_tail,B
	STL		B,Rx_sample_ptr					;* sample_ptr=Rx_sample_tail
	ST		#(RX_FIR_TAPS-1),Rx_fir_taps		
	STL		A,EQ_MSE						;* EQ_MSE=0
	ST		#EQ_DISABLED,EQ_2mu
	ST		#(V22_EQ_LEN-1),EQ_taps
	MVDK	*AR0(Rx_fir_start),AR3
	MVKD	AR3,Rx_fir_ptr			
	STM		#(RX_FIR_LEN-1),BRC
	RPTB	v22_init_Rx_fir_loop
v22_init_Rx_fir_loop:
	 STL	A,*AR3+							;* Rx_fir[*++]=0

	MVDK	*AR0(EQ_coef_start),AR3
	STM		#(2*V22_EQ_LEN-1),BRC
	RPTB	v22_init_EQ_coef_loop
v22_init_EQ_coef_loop:
	 STL	A,*AR3+							;* EQ_coef[*++]=0
	MVDK	*AR0(EQ_coef_start),AR3
	LD		#EQ_COEF_SEED,B
	STL		B,*AR3(V22_EQ_LEN-1)			;* NOTE: different EQ implementation from vsim

	ST		#V22_ACQ_LOOP_K1,loop_K1			
	ST		#V22_ACQ_LOOP_K2,loop_K2			
	STL		A,PJ1_coef						;* disable PJ1 resonator
	STL		A,PJ2_coef                      ;* disable PJ2 resonator
	STL		A,Rx_sym_clk_memory				;* Rx_sym_clk_memory=0
	ST		#ACQ_TIMING_THR,timing_threshold	
	STL		A,coarse_error					;* coarse_error=0
	ST		#(V22_EQ_LEN/2),Rx_baud_counter	
	STL		A,Rx_sample_counter				;* sample_counter=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	STL		A,vco_memory					;* vco_memory=0
	STL		A,loop_memory					;* loop_memory=0
	STL		A,loop_memory_low				;* loop_memory=0
	STL		A,frequency_est
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,LOS_monitor					;* LOS_monitor=0
	ST		#WHAT_13,What			
	STL		A,LO_frequency
	STL		A,LO_memory
	STL		A,S1_memory
	STL		A,S1_nm1
	ST		#V22_RATE_1200,Rx_rate
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,agc_K
	ST		#2,Rx_Nbits		
	ST		#3,Rx_Nmask		
	STL		A,Rx_status						;* status=0 (OK)
	RET_

;****************************************************************************
;* Rx_v22_train_EQ: trains equalizer.
;* Returns:
;*		A=0 if no symbol produced
;*		A=1 if timed out
;*		A=2 if training completed
;*		A=3 at 450 msec after end S1 detected
;*		A=-1 if still in progress
;****************************************************************************

Rx_v22_train_EQ:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	RCD_	BEQ								;* return(-1) if Rx_data_head==Rx_data_tail
	 LD		#-1,A

	;**** check for timeout failure ****

	LD		Rx_symbol_counter,B
	SUB		#TRAIN_EQ_TIMEOUT,B
	BCD_	EQ_timeout_endif,BLEQ			;* branch if counter<=TIMEOUT
	 MVDK	Rx_data_len,BK
	CALL_	Rx_init_detector
 .if TX_V22_MODEM=ENABLED
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_silence
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	ST		#TRAIN_LOOPS_FAILURE,Rx_status
	LD		#1,A
	RET_									;* return(1) if timed out
EQ_timeout_endif:

	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	LD		*AR7+%,A						;* A=Rx_data[*] (symbol)
	LD		Rx_Nbits,B
	SUB		#1,B
	STLM	B,BRC							;* BRC=Nbits-1
	CALL_	v22_descrambler					;* B=symbol=descrambler(A)
	LD		Rx_Nbits,T						;* T=Nbits
	 MVKD	AR7,Rx_data_tail				;* update Rx_data_tail
	LD		Rx_pattern_reg,TS,A
	OR		B,A								;* A=(pattern_reg<<N)|symbol
	STL		A,Rx_pattern_reg

	;**** wait 450 msec after end S1 detected ****

	LD		Rx_symbol_counter,A
	SUB		#S1_END_TIMEOUT,A
	BC_		EQ_rate_endif0,AGEQ
	LD		#-1,A							
	RET_									;* return(-1) if counter<TIMEOUT
EQ_rate_endif0:

	;**** check for 2400 R1 or R2 rate pattern ****

	BC_		EQ_rate_endif,ANEQ				;* branch if counter!=TIMEOUT
	LDU		Rx_pattern_reg,A
	XOR		#SB1_PATTERN,A
	BC_		EQ_rate_endif,ANEQ				;* branch if pattern_reg!=SB1
	ST		#0,Rx_pattern_reg
	ST		#V22_RATE_2400,Rx_rate
	STPP	#v22_slicer24,slicer_ptr,B
	STPP	#APSK_timing,timing_ptr,B
	ST		#4,Rx_Nbits
	ST		#0fh,Rx_Nmask
	LDU		vco_memory,B	
	SUB		#TWENTY_SIX_DEGREES,B
	STL		B,vco_memory
	ST		#2,Rx_map_shift
	LD		#3,A
	RET_									;* return(3)
EQ_rate_endif:

	;**** search for scrambled 1 pattern at R2 rate ****

	SUB		Rx_Nmask,B						;* B=symbol-Nmask
	 LD		S1_memory,A		
	 ADD	Rx_Nbits,A						;* B=S1_memory+Nbits
	XC		1,BNEQ
	 LD		#0,A							;* if symbol!=Nmask ...
	STL		A,S1_memory						;* S1_memory=0
	SUB		#TRAIN_EQ_SCR1_TIMEOUT,A,B
	LD		#-1,A
	RC_		BLT								;* return(-1) if S1_memory<TRAIN_EQ_SCR1_TIMEOUT

	LD		#0,A
	STL		A,S1_memory						;* S1_memory=0
	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
	ST		#TRK_EQ_2MU,EQ_2mu
	ST		#TRK_TIMING_THR,timing_threshold
	ST		#V22_TRK_LOOP_K1,loop_K1
	ST		#V22_TRK_LOOP_K2,loop_K2
	STL		A,Rx_symbol_counter
	STL		A,Rx_status						;* status=OK
	ST		#NOT_DETECTED,Rx_pattern_detect
	LD		#2,A							
	RET_

;****************************************************************************
;* Rx_v22_message: processes message data.
;* Returns:
;*		A=0 if no symbol produced				
;*		A=1 if loss-of-lock
;*		A=2 if rate change request is detected
;*		A=-1 if still in progress				
;****************************************************************************

Rx_v22_message:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_ptr,B
	LD		#0,A
	RC_		BEQ								;* return(0) if no symbol

	MVDK	Rx_data_ptr,AR7					;* update Rx_data_tail
	LD		LOS_monitor,B
	SUB		#UNLOCKED,B
	BCD_	mes_while_loop,BNEQ				;* branch if LOS_monitor!=UNLOCKED
	 MVDK	Rx_data_len,BK
	CALL_	 Rx_init_detector
 .if TX_V22_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_silence
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	ST		#LOSS_OF_LOCK,Rx_status
	LD		#1,A
	RET_									;* return(1) if loss-of-lock

mes_while_loop:
	LDU		Rx_data_head,B
	SUBS	Rx_data_ptr,B
	RCD_		BEQ							;* return(-1) if still in progress
	 LD		#-1,A
	 LD		Rx_Nbits,B
	SUB		#1,B
	CALLD_	v22_descrambler
	 LD		*AR7,A							;* A=Rx_data[*] (symbol)
	 STLM	B,BRC							;* BRC=N-1
	LD		*AR7,A							;* A=Rx_data[*] (symbol)
	STL		B,*AR7+%						;* Rx_data[*++%]=bits

	;**** look for S1 pattern for rate change instigation ****

	LD		Rx_rate,B
	SUB		#V22_RATE_2400,B
	 MVKD	AR7,Rx_data_ptr					;* update Rx_data_ptr
	XC		1,BEQ							;* if rate=2400 ...
	 SFTL A,-2								;* ... symbol>>2
	CALLD_	v22_S1_detector
	 LD		A,B
	 NOP
	LD		Rx_pattern_detect,B
	SUB		#S1_END_DETECTED,B,A
	BC_		mes_while_loop,ANEQ				;* loop if detect!=S1_END

	MVKD	AR7,Rx_data_tail				;* Rx_data_tail=Rx_data_ptr
	LD		#0,A
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STL		A,Rx_pattern_reg				;* Rx_pattern_reg=0
	STL		A,S1_memory						;* S1_memory=0
	LD		Rx_rate,B
	SUB		#V22_RATE_2400,B				;* compare rate with RATE_2400
	BCD_		mes_while_endif2,BNEQ		;* return(2) if rate!=2400
	 ST		#RETRAIN,Rx_status

	ST		#V22_RATE_1200,Rx_rate
	STPP	#v22_slicer12,slicer_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	 ST		#2,Rx_Nbits
	 ST		#3,Rx_Nmask
	LDU		vco_memory,B	
	ADD		#TWENTY_SIX_DEGREES,B
	STL		B,vco_memory

	STL	A,Rx_map_shift						;* map_shift=0
	STL	A,data_Q1							;* data_Q1=0
mes_while_endif2:
	LD		#2,A
	RET_									;* return(2)

;****************************************************************************
;* Rx_v22_RC_respond: responds to rate change initiation.
;* Returns:
;* 		A=0 if no symbol produced		
;* 		A=1 if timed out				
;* 		A=2 if 32 R1 dibits detected
;* 		A=-1 if still in progress		
;****************************************************************************

Rx_v22_RC_respond:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	LD		#0,A
	RC_		BEQ								;* return(0) if no symbol

	;**** check for timeout failure ****

	LD		Rx_symbol_counter,B
	SUB		#RC_RESPOND_TIMEOUT,B
	BCD_		 RC_respond_continue1,BLEQ	;* branch if counter<=TIMEOUT
	 MVDK	Rx_data_len,BK
	CALL_	 Rx_init_detector
	ST		#RETRAIN_FAILURE,Rx_status
	LD		#1,A
	RET_									;* return(1) if timed out

RC_respond_continue1:
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	 LD		*AR7-%,A						;* A=Rx_data[*] (symbol)
	AND		#3,A							;* A=Rx_dat[tail]&3
	CALLD_	v22_descrambler
	 STM	#(2-1),BRC					
	LD		*AR7+%,A						;* A=symbol_nm1
	STL		B,*AR7+%						;* Rx_data[tail++%]=symbol
	MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** check for 32 consecutive R1 dibits ****

	SUB		B,A								;* symbol-symbol_nm1
	 ADDM	#1,S1_memory					;* S1_memory++
	XC		2,ANEQ							;* if symbol!=symbol_nm1 ...
	 ST		#0,S1_memory					;* ... S1_memory=0
	LD		S1_memory,A
	SUB		#32,A
	RCD_		ALT
	 LD		#-1,A							;* return(-1) if pattern_reg<32

	SUB		#3,B,A							;* A=symbol-3
	 ST		#V22_RATE_1200,Rx_rate
	XC		2,AEQ							;* if rate=3 ...
	 ST		#V22_RATE_2400,Rx_rate			;* .. rate=RATE_2400
	LD		#0,A
	STL		A,S1_memory						;* S1_memory=0
	ST		#R1_DETECTED,Rx_pattern_detect
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	LD		#2,A
	RETD_									;* return(2)
	 ST		#RETRAIN,Rx_status

;****************************************************************************
;* Rx_v22_RC_initiate: responds to a rate change initiation.
;* Returns:
;* 		A=0 if no symbol produced		
;* 		A=1 if timed out				
;* 		A=2 if S1 end is detected
;* 		A=-1 if still in progress
;****************************************************************************

Rx_v22_RC_initiate:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	LD		#0,A
	RC_		BEQ								;* return(0) if no symbol

	;**** check for timeout failure ****

	LD		Rx_symbol_counter,B
	SUB		#RC_INITIATE_TIMEOUT,B
	BCD_	RC_initiate_continue1,BLEQ	;* branch if counter<=TIMEOUT
	 MVDK	Rx_data_len,BK
	CALL_	Rx_init_detector
	ST		#RETRAIN_FAILURE,Rx_status
	LD		#1,A
	RET_									;* return(1) if timed out

RC_initiate_continue1:
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	LD		*AR7+%,B						;* A=Rx_data[*] (symbol)
	LD		Rx_rate,A
	SUB		#V22_RATE_2400,A
	MVKD	AR7,Rx_data_tail				;* update Rx_data_ptr
	CALLD_	v22_S1_detector
	 XC		1,AEQ							;* if rate=2400 ...
	  SFTL 	B,-2							;* ... symbol>>2

	LD		Rx_pattern_detect,B
	SUB		#S1_END_DETECTED,B,A
	RCD_	ANEQ
	 LD		#-1,A							;* return(-1) if detect!=S1_END_DETECTED

	LD		#0,A
	ST		#ACQ_EQ_2MU,EQ_2mu
	STL		A,agc_K							;* agc_K=0
	ST		#60,Rx_symbol_counter
	STL		A,Rx_pattern_reg				;* Rx_pattern_reg=0
	LD		Rx_rate,B
	SUB		#V22_RATE_2400,B				;* compare rate with RATE_2400
	BCD_	RC_initiate_exit,BNEQ 			;* return(2) if rate!=2400
	 ST		#RETRAIN,Rx_status
	
	ST		#V22_RATE_1200,Rx_rate
	STPP	#v22_slicer12,slicer_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	ST		#2,Rx_Nbits
	ST		#3,Rx_Nmask
	LDU		vco_memory,B	
	ADD		#TWENTY_SIX_DEGREES,B
	STL		B,vco_memory
	STL		A,Rx_map_shift					;* map_shift=0
	STL		A,data_Q1						;* data_Q1=0
RC_initiate_exit:
	LD		#2,A
	RET_									;* return(2)

;***************************************************************************
;* v22_descrambler: descrambles according to v22.
;* Expects the following on entry:
;*	A=in
;*	BRC=N-1 (N is the number of bits to scramble)
;* On exit:
;*	B=out
;* Modifies:
;*	A,B,AR0,AR1,T
;***************************************************************************

v22_descrambler:
	STLM	A,AR0							;* AR0=in
	RPTBD	v22_descrambler_end
	STM		#0,AR1							;* AR1= out=0

	LDM		BRC,A							;* A=i
	NEG		A
	STLM	A,T								;* T=-i
	 ADDM	#1,Dcounter						;* Dcounter++
	LD		*(AR0),TS,B						;* B=(in>>i)		
	AND		#1,B							;* B=  j=(in>>i)&1
	 LD		Rx_Dreg_low,-13,A				;* A=Dreg>>(14-1)
	 XOR	Rx_Dreg,A						;* A=Dreg>>(14-1)^Dreg>>(17-1)
	 XOR	B,A								;* A= k=j^Dreg>>(14-1)^Dreg>>(17-1)
	 AND	#1,A							;* A= k&1
	 STLM	A,TRN							;* TRN=k
	XC		2,BEQ							;* if j=0 ...
	 ST		#0,Dcounter						;* ... Dcounter=0
	
	LD		Rx_Dreg,16,A
	ADDS	Rx_Dreg_low,A					;* A=Dreg
	SFTL	A,1								;* A=Dreg<<1
	OR		B,A								;* A=(Dreg<<1)|j
	LD		Dcounter,B
	SUB		#64,B
	 STH	A,Rx_Dreg						;* update Dreg
	 STL	A,Rx_Dreg_low				
	 LDM	BRC,A
	 STLM	A,T								;* T=i
	 LDM	TRN,A							;* A=k
	XC		2,BEQ							;* if Dcounter=64
	 XOR	#1,A							;* ... A= k^1
	NORM	A,B								;* B= (k<<i)
	OR		*(AR1),B						;* B= out|(k<<1)
v22_descrambler_end:
	STLM	B,AR1							;* AR1=out
	RET_

;****************************************************************************
;* v22_slicer12: QPSK slicer.
;****************************************************************************

v22_slicer12:
	LD		Iprime,B
	 ST		#-SLICE_707,Ihat			
	 STM	#Rx_v22_hard_map,AR5			;* AR5=&Rx_v22_hard_map[k]
	XC		2,BGEQ
	 ST		#SLICE_707,Ihat					;* if Iprime>=0, Ihat=SLICE_707
	LD		Qprime,A
	 ST		#-SLICE_707,Qhat			
	 XC		2,BLT
	  MAR	*+AR5(1)						;* if Iprime<0, AR5=&hard_map[1] 
	XC		2,AGEQ
	 ST		#SLICE_707,Qhat					;* if Qprime>=0, Qhat=SLICE_707
	XC		2,ALT
	 MAR	*+AR5(2)						;* if Qprime<0, AR5=&hard_map[2] 
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v22_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v22_hard_map[k]
 .else
	 LDM	AR5,A						   	;* A=&Rx_v22_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v22_hard_map[k]
 .endif

;****************************************************************************
;* v22_slicer24: 16 QAM slicer
;****************************************************************************

v22_slicer24:
 .if RX_V22_MODEM_2400=ENABLED
	STM		#Rx_v22_hard_map,AR5			;* AR5=&Rx_v22_hard_map[k]					 
	LD		Iprime,B
	SUB		#SLICE2,B,A
	ST		#SLICE3,Ihat
	ST		#1,temp0
	XC		2,ALT							;* if Iprime<SLICE2 ...
	 ST		#SLICE1,Ihat
	XC		2,ALT							;* if Iprime<SLICE2 ...
	 ST		#0,temp0
	SUB		#-SLICE2,B,A
	XC		2,BLT							;* if Iprime<0 ...
	 ST		#-SLICE1,Ihat
	XC		2,ALT							;* if Iprime<-SLICE2 ...
	 ST		#-SLICE3,Ihat
	XC		2,ALT							;* if Iprime<-SLICE2 ...
	 ST		#1,temp0
	XC		2,BLT							;* if Iprime<0
	 MAR	*+AR5(1)						;* ... j=1

	LD		Qprime,B
	SUB		#SLICE2,B,A
	ST		#SLICE3,Qhat
	ST		#1,temp1
	XC		2,ALT							;* if Qprime<SLICE2 ...
	 ST		#SLICE1,Qhat
	XC		2,ALT							;* if Qprime<SLICE2 ...
	 ST		#0,temp1
	SUB		#-SLICE2,B,A
	XC		2,BLT							;* if Qprime<0 ...
	 ST		#-SLICE1,Qhat
	XC		2,ALT							;* if Qprime<-SLICE2 ...
	 ST		#-SLICE3,Qhat
	XC		2,ALT							;* if Qprime<-SLICE2 ...
	 ST		#1,temp1
	XC		2,BLT							;* if Qprime<0
	 MAR	*+AR5(2)						;* ... j=2

	;**** set inverse power estimate, What ****

	LD		temp0,A
	ADD		temp1,A							;* A=j+k
	 ST		#WHAT_33,What
	 SUB	#1,A,B							;* B=J+k-1		
	XC		2,AEQ							;* if j+k=0 ...
	 ST		#WHAT_11,What
	XC		2,BEQ							;* if j+k=1 ...
	 ST		#WHAT_13,What

	;**** calculate data_Q1 ****

	LD		Iprime,T
	MPY		Qprime,B						;* B=Iprime*Qprime
	 LD		temp0,1,A
	 OR		temp1,A							;* A=k|(j<<1)
	XC		2,BGEQ							;* if Iprime*Qprime>=0
	 LD		temp1,1,A
	XC		1,BGEQ							;* if Iprime*Qprime>=0
	 OR		temp0,A							;* A=j|(k<<1)
	STL		A,data_Q1
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v22_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v22_hard_map[k]
 .else
	 LDM	AR5,A						   	;* A=&Rx_v22_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v22_hard_map[k]
 .endif
 .else
	B_		slicer_return
 .endif

;****************************************************************************
;* v22_startup_timing: coarse and fine sgn() based timing error estimator
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=Ihat
;*	BH=Qhat
;****************************************************************************

v22_startup_timing:

	;**** coarse symbol timing error calculation ****

	LD		Iprime,A
	SUB		Inm2,A							;* A=Iprime-Inm2
	ABS		A,A								;* A=abs(Iprime-Inm2)
	LD		Inm1,B
	SUB		Inm3,B							;* B-=Inm3
	ABS		B,B								;* B=abs(Inm1-Inm3)
	SUB		B,A								;* A=abs(Iprime-Inm2)-abs(Inm1-Inm3)
	LD		Qprime,B
	SUB		Qnm2,B							;* B=Qprime-Qnm2
	ABS		B,B								;* B=abs(Qprime-Qnm2)
	ADD		B,A								;* A=abs(Qprime-Qnm2)
	LD		Qnm1,B
	SUB		Qnm3,B							;* B=Qnm1
	ABS		B,B								;* B=abs(Qnm1-Qnm3)
	SUB		B,A								;* A-=abs(Qnm1-Qnm3)
;++++#ifndef MESI_INTERNAL 03-13-2001 OP_POINT8 MODS
	SFTA	A,-4,A							;* A>>=4
;++++#endif  MESI_INTERNAL 03-13-2001 OP_POINT8 MODS 
	ADD		coarse_error,A				 	;* A+=coarse_error
	STL		A,coarse_error				 	;* update coarse_error
	
	SUB		#COARSE_THR,A,B					;* B=A-COARSE_THR
	ADD		#COARSE_THR,A					;* A=A+COARSE_THR
	XC		2,BGT
	 ST		#0,coarse_error					;* if error>THR, coarse_error=0
	XC		2,ALT
	 ST		#0,coarse_error					;* if error<-THR, coarse_error=0
	XC		2,ALT
	 ST		#1,Rx_baud_counter				;* if error<-THR, baud_counter=1
	B_		sgn_timing

;****************************************************************************
;* v22_diff_decoder: hard decision symbol differential decoder for v22.
;* On entry it expects:
;*	AR7=Rx_data_head
;*	BK=Rx_data_len
;*	A=Phat
;****************************************************************************

v22_diff_decoder:
 .if ON_CHIP_COEFFICIENTS=ENABLED
	LD		Rx_map_shift,T
	ADD		#4,A							;* A=Phat+4
	SUB		Rx_phase,A						;* A=Phat+4-Rx_phase
	AND		#3,A							;* A&=7
	ADD		#Rx_v22_phase_map,A				;* A=&Rx+phase_map[(Phat+4-Rx_phase&3]
	STLM	A,AR0	
	 LD		Phat,B
	 STL	B,Rx_phase						;* Rx_phase=Phat
	LD		*AR0,TS,A						;* A=phase_map[*]<<map_shift
	OR		data_Q1,A						;* A|=data_Q1
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_phase_map[*]
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .else
	LD		Rx_map_shift,T
	ADD		#4,A							;* A=Phat+4
	SUB		Rx_phase,A						;* A=Phat+4-Rx_phase
	AND		#3,A							;* A&=7
	ADD		#Rx_v22_phase_map,A				;* A=&Rx+phase_map[(Phat+4-Rx_phase&3]
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7							;* Rx_data[*]=Rx_v22_phase_map[*]
	LD		*AR7,TS,A						;* A=phase_map[*]<<map_shift
	OR		data_Q1,A						;* A|=data_Q1
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_phase_map[*]
	LD		Phat,B
	STL		B,Rx_phase						;* Rx_phase=Phat
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .endif
	
;****************************************************************************
;* v22_S1_detector: detects start and stop of "S1" signal.						
;* On entry it expects:
;*	 B=symbol
;****************************************************************************

v22_S1_detector:
 .if RX_V22_MODEM_2400=ENABLED
	LD		S1_nm1,A
	STLM	A,TRN							;* TRN=S1_nm1
	STL		B,S1_nm1						;* S1_nm1=symbol

	SUB		#1,B,A
	RC_		AEQ								;* return if symbol=1
	SUB		#2,B,A
	RC_		AEQ								;* return if symbol=2

	LD		Rx_pattern_detect,A
	SUB		#S1_DETECTED,A
	BC_		v22_S1_detector_else,ANEQ		;* branch if detector!=S1_DETECTED
	LDM		TRN,A
	XOR		#3,A							;* A=S1_nm1^3
	SUB		B,A								;* A=symbol-S1_nm1^3
	 ADDM	#1,S1_memory
	XC		2,AEQ							;* if symbol=S1_nm1^3 ...
	 ST		#0,S1_memory					;* ... S1_memory=0
	LD		S1_memory,A
	SUB		#S1_END_THRESHOLD,A
	RC_		ALT								;* branch if S1_memory<THR
	ST		S1_END_DETECTED,Rx_pattern_detect
	RETD_	
	 ST		#0,S1_memory

v22_S1_detector_else:
	LDM		TRN,A
	XOR		#3,A							;* A=S1_nm1^3
	SUB		B,A								;* A=symbol-S1_nm1^3
	 ADDM	#1,S1_memory
	XC		2,ANEQ							;* if symbol!=S1_nm1^3 ...
	 ST		#0,S1_memory					;* ... S1_memory=0
	LD		S1_memory,A
	SUB		#S1_THRESHOLD,A
	RC_		ALT								;* return if S1_memory<THR
	ST		S1_DETECTED,Rx_pattern_detect
	ST		#0,S1_memory
	RETD_	
	 ST		#V22_S1_DETECTED,Rx_status
 .else
	RET_
 .endif

;****************************************************************************
 .endif

;****************************************************************************
	.end
