;****************************************************************************
;* Filename: v32.asm
;* Date: 05-06-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for V.32.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"v32.inc"
	.include	"filter.inc"
	.include	"echo.inc"

V32_RATE_4800		  		.set	4800
V32_RATE_7200		  		.set	7200
V32_RATE_9600		  		.set	9600
V32_RATE_12000		 		.set	12000
V32_RATE_14400		 		.set	14400

	;**** ECSD generation ****

 .if $isdefed("V32_DEBUG")
ECSD_FREQUENCY		 		.set	17203
ECSD_REV_PERIOD				.set	1000 
ECSD_SCALE			 		.set	7336 
 .else
ECSD_FREQUENCY		 		.set	17203   ;* 8.192*2100 Hz
ECSD_REV_PERIOD				.set	3600	;* 8000*0.45 sec
ECSD_SCALE			 		.set	7336	;* 32768*10exp(-13 dB/20) 
 .endif

	;**** modulator ****

TX_FIR_TAPS					.set	5
TX_INTERP			  		.set	2*DEC2400 
TX_DEC				 		.set	INTERP2400
TX_COEF_LEN					.set	(TX_FIR_TAPS*TX_INTERP+TX_DEC)		
TX_V32_SCALE				.set	30704   ;* scaled for Rx_coef[] 
TX_PHASE_LEN				.set	4
TX_CARRIER			 		.set	3

T1					 		.set	2048 
T2					 		.set	(2*T1)
T6					 		.set	(6*T1)

 .if $isdefed("V32_DEBUG")
TXA_SILENCE1_LEN			.set	0 
TXA_ANSWER_TONE_LEN			.set	307
TXC_SILENCE1_LEN			.set	0
 .else
TXA_SILENCE1_LEN			.set	14400   ;* 1.8 sec. silence
TXA_ANSWER_TONE_LEN			.set	26400   ;* 3.3 sec 2100 Hz
TXC_SILENCE1_LEN			.set	2400	;* 1 sec. silence
 .endif

TXA_SILENCE2_LEN			.set	600	 ;* 75 msec. silence
TXA_AC_LEN			 		.set	372	 ;* 155 msec AC
TXA_SILENCE3_LEN			.set	16
TX_S_LEN					.set	256
TX_SBAR_LEN					.set	16
TX_BPSK_TRAIN_LEN	  		.set	256
TX_QPSK_TRAIN_LEN	  		.set	8192-256	
TX_TRN_LEN			 		.set	8192

 .if $isdefed("V32_DEBUG")
TXA_TRN1_LEN				.set	1560
TXA_TRN2_LEN				.set	1280
TXC_TRN_LEN					.set	1560	 ;++ speed up
 .else
TXA_TRN1_LEN				.set	8192
TXA_TRN2_LEN				.set	1280
TXC_TRN_LEN					.set	8192		
 .endif

TXA_SPECIAL_TRN_LEN			.set	1280
TXC_SPECIAL_TRN_LEN			.set	1280
TX_E_LEN					.set	8
TX_B1_LEN			  		.set	128

TXA_RC_AC_LEN		  		.set	56
TXA_RC_CA_LEN		  		.set	8
TXC_RC_AA_LEN		  		.set	56
TXC_RC_CC_LEN		  		.set	8
TX_RC_RATE_LEN		 		.set	64
TX_RC_E_LEN					.set	8
TX_RC_B1_LEN				.set	24

S_DELAY_SCALE		  		.set	40	  ;* num_samples scale for S_delay

TX_V32A_AC1_TIMEOUT			.set	24000   ;* 10 sec 
TX_V32A_CA_TIMEOUT	 		.set	12000   ;* 5 sec 
TX_V32A_AC2_TIMEOUT			.set	12000   ;* 5 sec 

TX_V32C_AA_TIMEOUT	 		.set	24000   ;* 10 sec 
TX_V32C_CC_TIMEOUT	 		.set	12000   ;* 5 sec 
TX_V32C_SILENCE2_TIMEOUT	.set	24000   ;* 10 sec
TX_R4_TIMEOUT		  		.set	2400	;* 1 sec. timeout 

 .if $isdefed("XDAIS_API")
	.global _V32_MESI_TxInitV32A
	.global V32_MESI_TxInitV32A
	.global _V32_MESI_TxInitV32A_ANS
	.global V32_MESI_TxInitV32A_ANS
	.global _V32_MESI_TxV32ARetrain
	.global V32_MESI_TxV32ARetrain
	.global _V32_MESI_TxV32ARenegotiate
	.global V32_MESI_TxV32ARenegotiate

	.global _V32_MESI_TxInitV32C
	.global V32_MESI_TxInitV32C
	.global _V32_MESI_TxV32CRetrain
	.global V32_MESI_TxV32CRetrain
	.global _V32_MESI_TxV32CRenegotiate
	.global V32_MESI_TxV32CRenegotiate
	.global	_V32_MESI_setTxV32RatePattern
	.global	V32_MESI_setTxV32RatePattern
 .else
	.global _Tx_init_v32A
	.global Tx_init_v32A
	.global _Tx_init_v32A_ANS
	.global Tx_init_v32A_ANS
	.global _Tx_v32A_retrain
	.global Tx_v32A_retrain
	.global _Tx_v32A_renegotiate
	.global Tx_v32A_renegotiate

	.global _Tx_init_v32C
	.global Tx_init_v32C
	.global _Tx_v32C_retrain
	.global Tx_v32C_retrain
	.global _Tx_v32C_renegotiate
	.global Tx_v32C_renegotiate
	.global	_set_Tx_v32_rate_pattern
	.global	set_Tx_v32_rate_pattern
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_APSKmodulator
	.asg	COMMON_MESI_APSKmodulator, APSK_modulator
	.global RXTX_MESI_TxInitSilence
	.asg	RXTX_MESI_TxInitSilence, Tx_init_silence
	.global TCM_MESI_TxInitTCM
	.asg	TCM_MESI_TxInitTCM, Tx_init_TCM
	.global TCM_MESI_TCMencoder
	.asg	TCM_MESI_TCMencoder, TCM_encoder
	.global RXTX_MESI_TxSyncSampleBuffers
	.asg	RXTX_MESI_TxSyncSampleBuffers, Tx_sync_sample_buffers
	.global RXTX_MESI_TxStateReturn
	.asg	RXTX_MESI_TxStateReturn, Tx_state_return
 .else
	.global APSK_modulator
	.global Tx_init_silence
	.global Tx_init_TCM
	.global TCM_encoder
	.global Tx_sync_sample_buffers
	.global Tx_state_return
 .endif										;* "XDAIS_API endif

	;**** demodulator ****

;++++#ifndef MESI_INTERNAL 03-09-2001 OP_POINT8 MODS
;RX_FIR_TAPS					.set	16
;RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
;RX_INTERP			  		.set	INTERP2400
;RX_DEC				 		.set	DEC2400
;RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
;RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
;RX_V32_CARRIER_FREQ			.set	24576   ;* (1800*65536/4800)
;LO_PHASE_ADJ				.set	614	 	;* (65536/8000)*(1800/RX_INTERP)
;ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
;TRK_TIMING_THR		 		.set	768	 	;* (32768*OP_POINT*1.5)
;S_DETECT_THR				.set	24
;
;V32_EQ_LEN			 		.set	63
;ACQ_EQ_2MU			 		.set	512
;V32_NEC_LEN					.set	NEC_COEF_LEN
;V32_FEC_LEN					.set	FEC_COEF_LEN
;V32_EC_LEN			 		.set	V32_NEC_LEN
; .if $isdefed("V32_DEBUG")
;MIN_EC_TRAIN_LEN			.set	1281+100 ;++ speed up
; .else
;MIN_EC_TRAIN_LEN			.set	1280+4096
; .endif
;EC_MSE_TRAIN_THRESHOLD 		.set	256
;
;ACQ_AGC_K			  		.set	512 
;RETRAIN_REQUEST_THRESHOLD	.set	64
;RETRAIN_REQUEST_LEVEL  		.set	128
;RC_DETECT_THRESHOLD			.set	16
;RC_DETECT_LEVEL				.set	128	 	;* OP_POINT/4 
;
;SLICE_707			  		.set	362
;;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;WHAT_11 					.set	2560	;* ( P_AVG/(1^2+1^2) )/64)*32768
;;WHAT_13 					.set	512		;* ( P_AVG/(1^2+3^2) )/64)*32768
;;WHAT_33 					.set	284		;* ( P_AVG/(3^2+3^2) )/64)*32768
;;COS_PI_BY_8					.set	30274   ;* 32768*cos(pi/8)
;;SIN_PI_BY_8					.set	12540   ;* 32768*sin(pi/8)
;;COS_PI_BY_4					.set	23170   ;* 32768*cos(pi/4)
;;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
; .if $isdefed("SQUARE_ROOT_WHAT")
;WHAT_11 					.set	1145    ;* 32768.0*OP_POINT*SIG_AVG/SIG_11
;WHAT_13 					.set	512     ;* 32768.0*OP_POINT*SIG_AVG/SIG_13
;WHAT_33 					.set	382     ;* 32768.0*OP_POINT*SIG_AVG/SIG_33
; .else		;* SQUARE_ROOT_WHAT
;WHAT_11 					.set	2560	;* 32768.0*OP_POINT*PAVG/(1*1+1*1)
;WHAT_13 					.set	512		;* 32768.0*OP_POINT*PAVG/(1*1+3*3)
;WHAT_33 					.set	284		;* 32768.0*OP_POINT*PAVG/(3*3+3*3)
; .endif		;* SQUARE_ROOT_WHAT
;;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;SLICE1				 		.set	81	
;SLICE2				 		.set	(SLICE1*2)
;SLICE4				 		.set	(SLICE1*4)
;SLICE6				 		.set	(SLICE1*6)
;
;V32_ACQ_LOOP_K1				.set	6177
;V32_ACQ_LOOP_K2				.set	582
;V32_TRK_LOOP_K1				.set	1544
;V32_TRK_LOOP_K2				.set	36
;++++#else   MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
RX_FIR_TAPS					.set	16
RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
RX_INTERP			  		.set	INTERP2400
RX_DEC				 		.set	DEC2400
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_V32_CARRIER_FREQ			.set	24576
LO_PHASE_ADJ				.set	614
ACQ_TIMING_THR		 		.set	2048
TRK_TIMING_THR		 		.set	6144
S_DETECT_THR				.set	24

V32_EQ_LEN			 		.set	63
ACQ_EQ_2MU			 		.set	130
V32_NEC_LEN					.set	NEC_COEF_LEN
V32_FEC_LEN					.set	FEC_COEF_LEN
V32_EC_LEN			 		.set	V32_NEC_LEN
 .if $isdefed("V32_DEBUG")
MIN_EC_TRAIN_LEN			.set	1281+100 ;++ speed up
 .else
MIN_EC_TRAIN_LEN			.set	1280+4096
 .endif
EC_MSE_TRAIN_THRESHOLD 		.set	256

ACQ_AGC_K			  		.set	64
RETRAIN_REQUEST_THRESHOLD	.set	64
RC_DETECT_THRESHOLD			.set	16
RC_DETECT_LEVEL				.set	1024

SLICE_707			  		.set	2896
WHAT_11 					.set	9159	;* 32768.0*OP_POINT*PAVG/(1*1+1*1)
WHAT_13 					.set	4096	;* 32768.0*OP_POINT*PAVG/(1*1+3*3)
WHAT_33 					.set	3053	;* 32768.0*OP_POINT*PAVG/(3*3+3*3)
SLICE1				 		.set	1295
SLICE2				 		.set	(SLICE1*2)
SLICE3				 		.set	(SLICE1*3)

V32_ACQ_LOOP_K1				.set	772
V32_ACQ_LOOP_K2				.set	72 
V32_TRK_LOOP_K1				.set	193
V32_TRK_LOOP_K2				.set	4  
 .else      ;* OP_POINT=8
RX_FIR_TAPS					.set	16
RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
RX_INTERP			  		.set	INTERP2400
RX_DEC				 		.set	DEC2400
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_V32_CARRIER_FREQ			.set	24576   ;* (1800*65536/4800)
LO_PHASE_ADJ				.set	614	 	;* (65536/8000)*(1800/RX_INTERP)
ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
TRK_TIMING_THR		 		.set	768	 	;* (32768*OP_POINT*1.5)
S_DETECT_THR				.set	24

V32_EQ_LEN			 		.set	63
ACQ_EQ_2MU			 		.set	512
V32_NEC_LEN					.set	NEC_COEF_LEN
V32_FEC_LEN					.set	FEC_COEF_LEN
V32_EC_LEN			 		.set	V32_NEC_LEN
 .if $isdefed("V32_DEBUG")
MIN_EC_TRAIN_LEN			.set	1281+100 ;++ speed up
 .else
MIN_EC_TRAIN_LEN			.set	1280+4096
 .endif
EC_MSE_TRAIN_THRESHOLD 		.set	256

ACQ_AGC_K			  		.set	512 
RETRAIN_REQUEST_THRESHOLD	.set	64
RC_DETECT_THRESHOLD			.set	16
RC_DETECT_LEVEL				.set	128	 	;* OP_POINT/4 

SLICE_707			  		.set	362
 .if $isdefed("SQUARE_ROOT_WHAT")
WHAT_11 					.set	1145    ;* 32768.0*OP_POINT*SIG_AVG/SIG_11
WHAT_13 					.set	512     ;* 32768.0*OP_POINT*SIG_AVG/SIG_13
WHAT_33 					.set	382     ;* 32768.0*OP_POINT*SIG_AVG/SIG_33
 .else		;* SQUARE_ROOT_WHAT
WHAT_11 					.set	2560	;* 32768.0*OP_POINT*PAVG/(1*1+1*1)
WHAT_13 					.set	512		;* 32768.0*OP_POINT*PAVG/(1*1+3*3)
WHAT_33 					.set	284		;* 32768.0*OP_POINT*PAVG/(3*3+3*3)
 .endif		;* SQUARE_ROOT_WHAT
SLICE1				 		.set	162	
SLICE2				 		.set	(SLICE1*2)
SLICE3				 		.set	(SLICE1*3)

V32_ACQ_LOOP_K1				.set	6177
V32_ACQ_LOOP_K2				.set	582
V32_TRK_LOOP_K1				.set	1544
V32_TRK_LOOP_K2				.set	36
 .endif		;* OP_POINT=8
;++++#endif  MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 

BPF_150HZ_BW				.set	53	  	;* 8000/150
BPF_300HZ_BW				.set	27	  	;* 8000/300
BB_300HZ_BW					.set	27	  	;* 8000/300 
BB_300HZ_COEF		  		.set	1446	;* (1/BB_300HZ_BW)*fudge 

NUM_SAMPLES_COEF			.set	9830	;* 32768*(24/80)
PATH_FILTER_DELAY	  		.set	12	  	;* (RX_ANALYSIS_LEN/2)*(24/80)
PATH_WINDOW					.set	16
MODEM_TURNAROUND_DELAY 		.set	51	  	;* measured delay through demod and mod 
USB1_DETECT_LEN				.set	240	 	;* 100 msec. of USB1 at 2400 Hz 
ANS_DETECT_LEN		 		.set	30	  	;* 33.3 msec. (1/2 ANSam cycle) 

V32_SIGNAL_MASK				.set	0f111h	
V32_BIS_MASK				.set	0880h	
GSTN_CLEARDOWN_MASK			.set	0e68h	
RATE_SIGNAL_PATTERN			.set	0111h	
SIGNAL_E_PATTERN			.set	0f111h	
RATE_2400_BIT		  		.set	0800h
RATE_4800_BIT		  		.set	0400h	
RATE_7200_BIT		  		.set	0040h	
RATE_9600_BIT		  		.set	0200h	
RATE_12000_BIT		 		.set	0020h	
RATE_14400_BIT		 		.set	0008h	
TCM_BIT						.set	0080h	
V32BIS_BITS					.set	0880h
				
RATE_4800_PATTERN	  		.set	(RATE_4800_BIT|RATE_2400_BIT)
RATE_7200_PATTERN	  		.set	(V32BIS_BITS|RATE_4800_PATTERN|RATE_7200_BIT)
RATE_9600_PATTERN	  		.set	(RATE_7200_PATTERN|RATE_9600_BIT)
RATE_12000_PATTERN	 		.set	(RATE_9600_PATTERN|RATE_12000_BIT)
RATE_14400_PATTERN	 		.set	(RATE_12000_PATTERN|RATE_14400_BIT)

AA_DETECTED					.set	0001h	
AACC_LOW_DETECTED	  		.set	0002h
AACC_HIGH_DETECTED	 		.set	0004h
CC_END_DETECTED				.set	0008h

ANS_DETECTED				.set	0001h
AC_DETECTED					.set	0002h
ACCA_LOW_DETECTED	  		.set	0004h
ACCA_HIGH_DETECTED	 		.set	0008h
CAAC_LOW_DETECTED	  		.set	0010h
AC_END_DETECTED				.set	0020h
				
S_DETECTED			 		.set	0040h
SBAR_DETECTED		  		.set	0080h
TRN_DETECTED				.set	0100h
RATE_SIGNAL_DETECTED		.set	0200h
RATE_SIGNAL_ACKNOWLEDGED	.set	0400h
SIGNAL_E_DETECTED	  		.set	0800h
MESSAGE_DETECTED			.set	1000h
RC_PREAMBLE_DETECTED		.set	2000h

RX_AA_DETECT_LEN			.set	64	 ;* 37.5 msec 
RX_AC_DETECT_LEN			.set	64
CD_LEN				 		.set	16
MIN_EQ_TRAIN_LEN			.set	1280
TRN1_LEN					.set	256
RX_B1_LEN			  		.set	128
RX_RC_B1_LEN				.set	24
TRAIN_LOOPS_TIMEOUT			.set	256
DETECT_EQ_TIMEOUT	  		.set	2400	;* 1 sec. timeout
DETECT_EQ_FAILURE	  		.set	START_EQ_FAILURE
MIN_GSTN_CLEARDOWN_LEN		.set	64

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_APSKdemodulator
	.asg	COMMON_MESI_APSKdemodulator, APSK_demodulator
	.global RXTX_MESI_RxInitIdle
	.asg	RXTX_MESI_RxInitIdle, Rx_init_idle
	.global ECHO_MESI_echoCanceller
	.asg	ECHO_MESI_echoCanceller, echo_canceller
	.global ECHO_MESI_enableEchoCanceller
	.asg	ECHO_MESI_enableEchoCanceller, enable_echo_canceller
	.global COMMON_MESI_noTiming
	.asg	COMMON_MESI_noTiming, no_timing
	.global COMMON_MESI_sgnTiming
	.asg	COMMON_MESI_sgnTiming, sgn_timing
	.global COMMON_MESI_APSKTiming
	.asg	COMMON_MESI_APSKTiming, APSK_timing
	.global COMMON_MESI_RxTrainLoops
	.asg	COMMON_MESI_RxTrainLoops, Rx_train_loops
	.global COMMON_MESI_RxDetectEQ
	.asg	COMMON_MESI_RxDetectEQ, Rx_detect_EQ
	.global COMMON_MESI_agcGainEstimator
	.asg	COMMON_MESI_agcGainEstimator, agc_gain_estimator
	.global FILTER_MESI_BandpassFilter
	.asg	FILTER_MESI_BandpassFilter, bandpass_filter
	.global FILTER_MESI_BroadbandEstimator
	.asg	FILTER_MESI_BroadbandEstimator, broadband_estimator
	.global TCM_MESI_RxInitTCM
	.asg	TCM_MESI_RxInitTCM, Rx_init_TCM
	.global TCM_MESI_TCMslicer
	.asg	TCM_MESI_TCMslicer, TCM_slicer
	.global TCM_MESI_TCMDecoder
	.asg	TCM_MESI_TCMDecoder, TCM_decoder

	.global	_VCOEF_MESI_RCOS2400f1800
	.asg	_VCOEF_MESI_RCOS2400f1800, _RCOS2400_f1800
	.global	_VCOEF_MESI_RxTiming2400
	.asg	_VCOEF_MESI_RxTiming2400, _Rx_timing2400
	.global RXTX_MESI_RxStateReturn
	.asg	RXTX_MESI_RxStateReturn, Rx_state_return
	.global COMMON_MESI_slicerReturn
	.asg	COMMON_MESI_slicerReturn, slicer_return
	.global COMMON_MESI_timingReturn
	.asg	COMMON_MESI_timingReturn, timing_return
	.global COMMON_MESI_decoderReturn
	.asg	COMMON_MESI_decoderReturn, decoder_return
	.global _VCOEF_MESI_sinTable
	.asg	_VCOEF_MESI_sinTable, _sin_table
 .else
	.global APSK_demodulator
	.global Rx_init_idle
	.global echo_canceller
	.global enable_echo_canceller
	.global no_timing
	.global sgn_timing
	.global APSK_timing
	.global Rx_train_loops
	.global Rx_detect_EQ
	.global agc_gain_estimator
	.global bandpass_filter
	.global broadband_estimator
	.global Rx_init_TCM
	.global TCM_slicer
	.global TCM_decoder

	.global _RCOS2400_f1800
	.global _Rx_timing2400
	.global Rx_state_return
	.global slicer_return
	.global timing_return
	.global decoder_return
	.global _sin_table
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global Tx_v32A_silence1
	.global Tx_v32A_ANS
	.global Tx_v32A_silence2
	.global Tx_v32A_AC1
	.global Tx_v32A_CA
	.global Tx_v32A_AC2
	.global Tx_v32A_silence3
	.global Tx_v32A_special_TRN
	.global Tx_v32A_S1
	.global Tx_v32A_SBAR1
	.global Tx_v32A_TRN1
	.global Tx_v32A_R1
	.global Tx_v32A_silence4
	.global Tx_v32A_S2
	.global Tx_v32A_SBAR2
	.global Tx_v32A_TRN2
	.global Tx_v32A_R3
	.global Tx_v32A_E
	.global Tx_v32A_B1
	.global Tx_v32A_message
	.global Tx_v32A_RC_preamble
	.global Tx_v32C_silence1
	.global Tx_v32C_AA
	.global Tx_v32C_CC
	.global Tx_v32C_silence2
	.global Tx_v32C_S_delay
	.global Tx_v32C_special_TRN
	.global Tx_v32C_S1
	.global Tx_v32C_SBAR1
	.global Tx_v32C_TRN
	.global Tx_v32C_R2
	.global Tx_v32C_E
	.global Tx_v32C_B1
	.global Tx_v32C_message
	.global Tx_v32C_RC_preamble
	.global Tx_init_v32
	.global Tx_v32_silence						
	.global Tx_v32_S	
	.global Tx_v32_SBAR
	.global Tx_v32_TRN	
	.global Tx_v32_special_TRN				
	.global Tx_v32_rate
	.global Tx_v32_B1	
	.global Tx_v32_message					
	.global v32_abs_encoder					
	.global v32_diff_encoder				
	.global v32A_scrambler
	.global v32A_scrambler6
	.global set_Tx_v32_rate					
	.global set_Tx_v32_E
	.global GSTN_cleardown

	.global Rx_init_v32A					
	.global Rx_v32A_detect_AA
	.global Rx_v32A_detect_AACC
	.global AA_detect_else
	.global AA_detect_else2
	.global Rx_v32A_detect_AACC_endif
	.global Rx_v32A_detect_CC_end
	.global Rx_v32A_train_EC
	.global Rx_v32A_train_EC_endif
	.global Rx_v32A_S_detect
	.global A_S_detect_endif1
	.global Rx_v32A_train_loops
	.global Aloops_stable_detected
	.global Rx_v32A_detect_EQ
	.global Astart_EQ_timeout_endif
	.global Adetect_EQ_endif2
	.global Adetect_EQ_endif3
	.global Rx_v32A_train_EQ
	.global Rx_v32A_rate
	.global A_check_retrain_request	
	.global A_check_renegotiate_request	
	.global A_check_E_detect
	.global Rx_v32A_B1
	.global Rx_v32A_message
	.global Rx_v32A_message_endif1
	.global Rx_init_v32C					
	.global Rx_v32C_detect_AC
	.global USB1_detector_endif
	.global ANS_detector_endif
	.global ANS_detector_endif2
	.global Rx_v32C_detect_ACCA
	.global AC_detect_else1					
	.global AC_detect_else2					
	.global Rx_v32C_detect_CAAC
	.global CAAC_detect_else1				
	.global CAAC_detect_else2				
	.global Rx_v32C_detect_CAAC_endif
	.global Rx_v32C_detect_AC_end
	.global Rx_v32C_train_EC
	.global Rx_v32C_train_EC_endif
	.global Rx_v32C_S_detect
	.global C_S_detect_endif1
	.global Rx_v32C_train_loops
	.global Cloops_stable_detected
	.global Rx_v32C_detect_EQ
	.global Cstart_EQ_timeout_endif
	.global Cdetect_EQ_endif2
	.global Cdetect_EQ_endif3
	.global Rx_v32C_train_EQ
	.global Rx_v32C_rate
	.global C_rate_endif0
	.global C_check_retrain_request
	.global C_rate_endif1
	.global C_check_renegotiate_request
	.global C_rate_endif2
	.global C_check_RATE_ACK
	.global C_rate_endif3
	.global C_check_E_detect
	.global C_rate_else4
	.global C_rate_endif4
	.global C_rate_endif5
	.global Rx_v32C_B1	
	.global Rx_v32C_message
	.global Rx_v32C_message_endif1
	.global Rx_init_v32
	.global Rx_v32_S_detect				
	.global Rx_v32_train_EQ					
	.global Rx_v32_rate
	.global check_RATE_detect
	.global v32_rate_endif0
	.global v32_rate_endif1
	.global signal_E	
	.global signal_E_endif1
	.global signal_E_endif2
	.global Rx_v32_B1	
	.global Rx_v32_message					
	.global Rx_v32_RC_detector	 
	.global Rx_v32_filters					
	.global v32A_descrambler
	.global v32C_descrambler
	.global v32C_descrambler6
	.global v32_slicer48
	.global v32A_EQslicer48
	.global v32C_EQslicer48
	.global v32_slicer96
	.global v32_diff_decoder
 .endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

;****************************************************************************
;* tables and coefficients
;****************************************************************************

	.sect	"vcoefs"

 .if TX_V32_MODEM=ENABLED
Tx_v32_phase_map:							
	.word 1,0,2,3

Tx_v32_amp_table:
 .word  T2, T2, -T2, T2, -T2,-T2,  T2,-T2,  T6, T2, -T2,T6, -T6,-T2,  T2,-T6
 .word  T2, T6, -T6, T2, -T2,-T6,  T6,-T2,  T6, T6, -T6,T6, -T6,-T6,  T6,-T6
 .endif

 .if RX_V32_MODEM=ENABLED
Rx_v32_hard_map:
	.word	 0,1,3,2

Rx_v32_phase_map:
	.word	 1,0,2,3
 .endif

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_v32A(struct START_PTRS *)
;* void Tx_init_v32A_ANS(struct START_PTRS *)
;* void Tx_v32A_retrain(struct START_PTRS *)
;* void Tx_v32A_renegotiate(struct START_PTRS *)
;* void Tx_init_v32C(struct START_PTRS *)
;* void Tx_v32C_retrain(struct START_PTRS *)
;* void Tx_v32C_renegotiate(struct START_PTRS *)
;****************************************************************************

	;*****************************************
	;**** ANSWER side transmitter modules ****
	;*****************************************

 .if TX_V32A_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v32A:
;* C function call: void Tx_init_v32A(struct START_PTRS *)
;* Initializes Tx_block for v32 modulator
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v32A:					
_V32_MESI_TxInitV32A:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v32A
	POPM	ST1
	RETC_
 .endif

Tx_init_v32A:					
V32_MESI_TxInitV32A:
	CALL_	 Tx_init_v32
	STPP	#v32A_scrambler,Tx_scrambler_ptr,B
	ST		#TXA_SILENCE2_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_silence2,Tx_state,B
	ST		#TX_V32A_SILENCE2_ID,Tx_state_ID		

	;**** initialize answer side receiver ****

 .if RX_V32A_MODEM=ENABLED
	MVDK	Tx_start_ptrs,AR3
	LD		*AR3(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_v32A		
	MVDK	Rx_start_ptrs,AR3
	LD		*AR3(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
 .endif
	RET_

;****************************************************************************
;* _Tx_init_v32A_ANS:
;* C function call: void Tx_init_v32A_ANS(struct START_PTRS *)
;* Initializes Tx_block for v32 ANS (2100 Hz ECSD) generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v32A_ANS:				
_V32_MESI_TxInitV32A_ANS:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v32A_ANS
	POPM	ST1
	RETC_
 .endif

Tx_init_v32A_ANS:				
V32_MESI_TxInitV32A_ANS:
	LD		#0,A
	ST		ECSD_FREQUENCY,Tx_frequency
	ST		ECSD_REV_PERIOD,Tx_rev_period
	ST		1,Tx_rev_memory
	STL		A,Tx_osc_memory
	ST		ECSD_SCALE,Tx_osc_scale
	STL		A,Tx_sample_counter	
	ST		#TXA_SILENCE1_LEN,Tx_terminal_count
	STPP	#Tx_v32A_silence1,Tx_state,B
	RETD_ 
	 ST		#TX_V32A_SILENCE1_ID,Tx_state_ID

;****************************************************************************
;* _Tx_v32A_retrain:
;* C function call: void Tx_v32A_retrain(struct START_PTRS *)
;* Forces transmitter into retrain sequence.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v32A_retrain:				
_V32_MESI_TxV32ARetrain:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_retrain
	POPM	ST1
	RETC_
 .endif

Tx_v32A_retrain:					
V32_MESI_TxV32ARetrain:
	CALL_	set_Tx_v32_rate_pattern
	STPP	#v32A_scrambler,Tx_scrambler_ptr,B
	ST		#Tx_v32_amp_table,Tx_amp_ptr		
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	LD		#0,A
	STL		A,Tx_symbol_counter	
	STL		A,Tx_sample_counter	
	ST		#TXA_AC_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_AC1,Tx_state,B
	RETD_ 
	 ST		#TX_V32A_AC1_ID,Tx_state_ID		

;****************************************************************************
;* _Tx_v32A_renegotiate:
;* C function call: void Tx_v32A_renegotiate(struct START_PTRS *)
;* Forces transmitter into renegotiate sequence.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v32A_renegotiate:				
_V32_MESI_TxV32ARenegotiate:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_renegotiate
	POPM	ST1
	RETC_
 .endif

Tx_v32A_renegotiate:				
V32_MESI_TxV32ARenegotiate:
	CALL_	set_Tx_v32_rate_pattern
	STPP	#v32A_scrambler,Tx_scrambler_ptr,B
	ST		#Tx_v32_amp_table,Tx_amp_ptr		
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	ST		#0,Tx_sample_counter	
	ST		#-TXA_RC_AC_LEN,Tx_symbol_counter	
	ST		#TXA_RC_CA_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_RC_preamble,Tx_state,B
	RETD_ 
	 ST		#TX_V32A_RC_PREAMBLE_ID,Tx_state_ID		

;****************************************************************************
;* Tx_v32A_silence1: 1.8 sec. silence prior to ANS
;****************************************************************************

Tx_v32A_silence1:
	ADDM	#1,Tx_sample_counter			;* ++Tx_sample_counter
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT	
	 ST		#0,*AR2+%

	ST		#0,Tx_sample_counter	
	ST		#TXA_ANSWER_TONE_LEN,Tx_terminal_count   
	STPP	#Tx_v32A_ANS,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_ANS_ID,Tx_state_ID	

;****************************************************************************
;* Tx_v32A_ANS: 2100 Hz 
;****************************************************************************

Tx_v32A_ANS:

	;**** 450 msec. phase reversals ****

	ADDM	#-1,Tx_rev_memory
	LD		Tx_rev_memory,B
	BC_		 phase_rev_endif,BNEQ			;* branch if rev_memory!=0
	ADDM	#8000h,Tx_osc_memory				 
	LD		 Tx_rev_period,B
	STL		B,Tx_rev_memory					;* rev_memory=rev_period
phase_rev_endif:

	;**** 2100 Hz tone oscillator ****

	LDU		Tx_osc_memory,A					;* A=Tx_osc_memory
	ADD		Tx_frequency,A					;* A=osc_memory+Tx_frequency
	STL		A,Tx_osc_memory					;* Tx_osc_memory+=Tx_frequency
	LDU		Tx_osc_memory,A					;* A=Tx_osc_memory
	SFTL	A,-SIN_TABLE_SHIFT				;* A=osc_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[osc_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	 NOP
	 LD		Tx_osc_scale,T					;* T=scale
	MPY		*AR0,A							;* B=scale*Tx_sample
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR2							;* put it in Tx_sample[*]
	LD		Tx_osc_scale,T					;* T=Tx_scale1
	MPY		*AR2,A							;* A=scale*Tx_sample
 .endif
	MPYA	Tx_scale						;* B=Tx_scale*A
	STH		B,*AR2+%						;* Tx_sample[*++]=sin

	;**** check for end of segment ****

	ADDM	#1,Tx_sample_counter			;* Tx_sample_counter++
	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if TC<0	
	SUB		Tx_sample_counter,B
	BC_		Tx_state_return,BGT				;* return if sample_counter<TC

	CALL_	Tx_init_v32A
	B_		Tx_state_return

;****************************************************************************
;* Tx_v32A_silence2
;****************************************************************************

Tx_v32A_silence2:
	ADDM	#1,Tx_sample_counter			;* ++Tx_sample_counter
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT	
	 ST		#0,*AR2+%

	;**** synchronize sample[] buffer pointers ****

	CALLD_	Tx_sync_sample_buffers
	 MVDK	Tx_start_ptrs,AR3				;* AR3=start_ptrs
	ST		#TXA_AC_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_AC1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_AC1_ID,Tx_state_ID		

;****************************************************************************
;* Tx_v32A_AC1
;****************************************************************************

Tx_v32A_AC1:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_	Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** timeout ****/

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_symbol_counter,A
	SUB		#TX_V32A_AC1_TIMEOUT,A
	BC_		Tx_v32A_AC1_endif1,ALT
	CALL_	GSTN_cleardown					;* GSTN_cleardown			
	BC_		Tx_state_return,ANEQ			;* return if GSTN_cleardown!=0
	CALL_	Tx_init_v32A_ANS
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32A_AC1_endif1:

	BITF	*AR1(Rx_pattern_detect),#AA_DETECTED
	BCD_	Tx_AC1_continue,NTC				;* branch if !=AA_DETECTED
	 LD		#9,B							;* B=symbol=9
	 LD		Tx_symbol_counter,A
	SUB		Tx_terminal_count,A
	BC_		Tx_AC1_continue,ALT				;* branch if counter<TXA_AC_LEN

	;**** initialize path measurement in receiver ****

	LD		Tx_num_samples,16,A				;* A=Tx_num_samples
	SUB		Tx_call_counter,16,A			;* A= k=num_samples-call_counter
	ADD		#1,16,A
	ADD		*AR1(Rx_num_samples),16,A		;* A=Tx_num_samples+Rx_num_samples
	ADD		Tx_system_delay,16,A			;* A+=system_delay
	STM		#-NUM_SAMPLES_COEF,T
	MPYA	A								;* A=-k*NUM_SAMPLES_COEF
	SUB		#(64+PATH_FILTER_DELAY+TX_FIR_TAPS/2 +1),16,A
	STH		A,*AR1(Rx_symbol_counter)
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	STL		A,Tx_terminal_count				;* terminal_count=0
	STPP	#Tx_v32A_CA,Tx_state,A
	ST		#TX_V32A_CA_ID,Tx_state_ID		
	XOR		#0ch,B							;* B=symbol=5

Tx_AC1_continue:
	CALL_	v32_diff_encoder				;* B=symbol
	B_		Tx_state_return

;****************************************************************************
;* Tx_v32A_CA
;****************************************************************************

Tx_v32A_CA:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_	Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** timeout ****/

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_symbol_counter,A
	SUB		#TX_V32A_CA_TIMEOUT,A
	BC_		Tx_v32A_CA_endif1,ALT
	CALL_	Tx_init_v32A_ANS
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32A_CA_endif1:
	 
	BITF	*AR1(Rx_pattern_detect),#AACC_HIGH_DETECTED
	BCD_	Tx_CA_continue,NTC				;* branch if !=AACC_HIGH_DETECTED
	 LD		#9,B							;* B=symbol=9
	 LD		Tx_symbol_counter,A
	SUB		Tx_terminal_count,A
	BC_		Tx_CA_continue,ALT				;* branch if counter<TXA_CA_LEN
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	ST		#(2*PATH_FILTER_DELAY),Tx_terminal_count  
	STPP	#Tx_v32A_AC2,Tx_state,A
	ST		#TX_V32A_AC2_ID,Tx_state_ID		
	XOR		#0ch,B							;* B=symbol=5

Tx_CA_continue:
	CALL_	v32_diff_encoder				;* B=symbol
	B_		Tx_state_return

;****************************************************************************
;* Tx_v32A_AC2
;****************************************************************************

Tx_v32A_AC2:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_	Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** timeout ****/

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_symbol_counter,A
	SUB		#TX_V32A_AC2_TIMEOUT,A
	BC_		Tx_v32A_AC2_endif1,ALT
	CALL_	Tx_init_v32A_ANS
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32A_AC2_endif1:
	BITF	*AR1(Rx_pattern_detect),#CC_END_DETECTED
	 LD		Tx_symbol_counter,B
	 SUB	Tx_terminal_count,B
	BC_		Tx_AC2_continue,BLT				;* branch if counter<TXA_CA_LEN
	BC_		Tx_AC2_continue,NTC				;* branch if !CC_END_DETECTED
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	ST		#TXA_SILENCE3_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_silence3,Tx_state,A
	ST		#TX_V32A_SILENCE3_ID,Tx_state_ID	

Tx_AC2_continue:
	LD		#9,B							 ;* B=symbol=9
	CALL_	v32_diff_encoder
	B_		Tx_state_return
	
;****************************************************************************
;* Tx_v32A_silence3
;****************************************************************************

Tx_v32A_silence3:
	CALL_	Tx_v32_silence
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_S_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_S1,Tx_state,B
	ST		#TX_V32A_S1_ID,Tx_state_ID	
	LD		Tx_mode,B
	AND   	#V32_SPECIAL_TRAIN_BIT,B
	BC_		Tx_state_return,BEQ				;* return if special train bit not set
	ST		#TXA_SPECIAL_TRN_LEN,Tx_terminal_count	
	STPP	#Tx_v32A_special_TRN,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_SPECIAL_TRN1_ID,Tx_state_ID	   

;****************************************************************************
;* Tx_v32A_special_TRN: special training segment			
;****************************************************************************

Tx_v32A_special_TRN:
	CALL_	Tx_v32_special_TRN
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_S_LEN,Tx_terminal_count
	STPP	#Tx_v32A_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_S1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_S1
;****************************************************************************

Tx_v32A_S1:
	CALL_	Tx_v32_S
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_SBAR_LEN,Tx_terminal_count
	STPP	#Tx_v32A_SBAR1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_SBAR1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_SBAR1
;****************************************************************************

Tx_v32A_SBAR1:
	CALL_	Tx_v32_SBAR
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

 .if ECHO_CANCELLER=ENABLED
	LD		#0,A							 ;* A= offset=0
	CALLD_	enable_echo_canceller
	 MVDK	Tx_start_ptrs,AR3
 .endif
	LD		#0,A
	STL		A,Tx_Sreg						;* Tx_Sreg=0
	STL		A,Tx_Sreg_low					;* Tx_Sreg=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#MIN_EC_TRAIN_LEN,Tx_terminal_count
	STPP	#Tx_v32A_TRN1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_TRN1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_TRN1
;****************************************************************************

Tx_v32A_TRN1:
	CALL_	Tx_v32_TRN
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B,A
	BC_		Tx_state_return,ALT				;* return if symbol_counter<LEN

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(EC_MSE),A
	SUB		#EC_MSE_TRAIN_THRESHOLD,A
	 SUB	#TXA_TRN1_LEN,B					;* counter-TRN1_LEN
	XC		1,ALEQ
	 LD		#0,B							;* if MSE<=THR, cntr=LEN	
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STL		A,Tx_terminal_count				;* terminal_count=0
	LDM		AR6,A							;* A= k (returned from Tx_v32_TRN)
	ADD		Tx_phase,A						;* A=Tx_phase+k
	AND		#(TX_PHASE_LEN-1),A				;* A=k=(Tx_phase+k)&(TX_PHASE_LEN-1)
	STL		A,Tx_phase						;* update Tx_phase
	STPP	#Tx_v32A_R1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_R1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_R1
;****************************************************************************

Tx_v32A_R1:
	CALL_	Tx_v32_rate
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_rate()=0

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	BITF	*AR1(Rx_pattern_detect),#S_DETECTED
	BC_		Tx_state_return,NTC				;* return if !S_DETECTED
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B,A
	BCD_	Tx_state_return,ALT				;* return if counter<TC
	 AND	#7,B							;* B=symbol_counter&7
	BC_		Tx_state_return,BNEQ			 ;* return if symbol_counter&7!=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STL		A,Tx_terminal_count				;* terminal_count=0
	STPP	#Tx_v32A_silence4,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_SILENCE4_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_silence4
;****************************************************************************

Tx_v32A_silence4:
	CALL_	Tx_v32_silence
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	;**** look for R2 reception ****

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	BITF	*AR1(Rx_pattern_detect),#RATE_SIGNAL_DETECTED
	BC_		Tx_state_return,NTC				;* return if !RATE_SIGNAL_DETECTED

	CALL_	set_Tx_v32_rate
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_S_LEN,Tx_terminal_count
	STPP	#Tx_v32A_S2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_S2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_S2
;****************************************************************************

Tx_v32A_S2:
	CALL_	Tx_v32_S
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_SBAR_LEN,Tx_terminal_count
	STPP	#Tx_v32A_SBAR2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_SBAR2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_SBAR2
;****************************************************************************

Tx_v32A_SBAR2:
	CALL_	Tx_v32_SBAR
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_Sreg						;* Tx_Sreg=0
	STL		A,Tx_Sreg_low					;* Tx_Sreg=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TXA_TRN2_LEN,Tx_terminal_count
	STPP	#Tx_v32A_TRN2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_TRN2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_TRN2
;****************************************************************************

Tx_v32A_TRN2:
	CALL_	Tx_v32_TRN
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B,A
	BC_		Tx_state_return,ALT				;* return if symbol_counter<LEN

	BITF	Tx_rate_pattern,#GSTN_CLEARDOWN_MASK
	 STL	A,Tx_symbol_counter				;* Tx_symbol_counter=0
	 STL	A,Tx_sample_counter				;* Tx_sample_counter=0;
	STL		A,Tx_terminal_count				;* terminal_count=0
	XC		2,NTC							;* if GSTN_CLEARDOWN ...
	 ST		#MIN_GSTN_CLEARDOWN_LEN,Tx_terminal_count
	LDM		AR6,A							;* A= k (returned from Tx_v32_TRN)
	ADD		Tx_phase,A						;* A=Tx_phase+k
	AND		#(TX_PHASE_LEN-1),A				;* A=k=(Tx_phase+k)&(TX_PHASE_LEN-1)
	STL		A,Tx_phase						;* update Tx_phase
	STPP	#Tx_v32A_R3,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_R3_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_R3
;****************************************************************************

Tx_v32A_R3:
	CALL_	Tx_v32_rate
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_rate()=0

	;**** check for rate signal boundaries ****

	LD		Tx_symbol_counter,16,B
	SUB		Tx_terminal_count,16,B,A
	BCD_	Tx_state_return,ALT				;* return if counter<TC
	 AND	#7,16,B							;* B=symbol_counter&7
	BCD_	Tx_state_return,BNEQ			;* return if symbol_counter&7!=0

	;**** check for R4 timeout ****

	 CMPM	Tx_state_ID,#TX_V32A_R4_ID
	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	BC_		Tx_v32A_R3_endif1,NTC			;* branch if !TX_V32A_R4_ID
	LD		Tx_symbol_counter,A
	SUB		*AR1(Rx_RTD),A
	SUB		#TX_R4_TIMEOUT,A				;* A=symbol_counter-TIMEOUT-path
	BC_		Tx_v32A_R3_endif1,ALEQ			;* branch if counter<=TIMEOUT+path
	CALL_	Tx_v32A_retrain
	B_		Tx_state_return
Tx_v32A_R3_endif1:

	;**** look for E reception ****

	BITF	*AR1(Rx_pattern_detect),#SIGNAL_E_DETECTED
	 LD		Tx_state_ID,A
	 SUB	#TX_V32A_R3_ID,A
	XC		1,TC							;* if !SIGNAL_E_DETECTED ...
	 LD		Tx_state_ID,A
	BC_		Tx_state_return,AEQ				;* return if state_ID=TX_V32A_R3_ID

	LD		*AR1(Rx_pattern_detect),B
	AND		#(RATE_SIGNAL_DETECTED|RC_PREAMBLE_DETECTED),B
	SUB		#(RATE_SIGNAL_DETECTED|RC_PREAMBLE_DETECTED),B
	 LD		Tx_state_ID,A
	 SUB	#TX_V32A_R4_ID,A
	XC		1,BEQ
	 LD		Tx_state_ID,A
	BC_		Tx_state_return,AEQ				;* return if state_ID=TX_V32A_R4_ID

	CALL_	set_Tx_v32_rate
	CALL_	set_Tx_v32_E
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	CMPM	Tx_state_ID,#TX_V32A_R3_ID
	 ST		#TX_RC_B1_LEN,Tx_terminal_count
	XC		2,TC							;* if state_ID=TX_V32A_R3_ID ...
	 ST		#TX_B1_LEN,Tx_terminal_count	;* terminal_count=TX_B1_LEN
	STPP	#Tx_v32A_E,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_E_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_E
;****************************************************************************

Tx_v32A_E:
	CALL_	Tx_v32_rate
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_rate()=0

	;**** look for GSTN cleardown ****

	BITF	Tx_rate_pattern,#GSTN_CLEARDOWN_MASK
	LD		Tx_symbol_counter,B
	BCD_	Tx_v32A_E_endif0,TC				;* branch if !GSTN_CLEARDOWN
	 SUB	#MIN_GSTN_CLEARDOWN_LEN,B,A
	BC_		Tx_state_return,ALT				;* return if symbol_counter<LEN
	CALL_	GSTN_cleardown					;* GSTN_cleardown			
	B_		Tx_state_return
Tx_v32A_E_endif0:

	SUB		#TX_E_LEN,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN
	LD		Tx_rate,B
	SUB		#V32_RATE_9600,B,A
	SUB		#V32_RATE_14400,B
	BC_		Tx_v32A_E_endif1,ANEQ			;* branch if rate!=9600
	ST		#4,Tx_Nbits
	ST		#0fh,Tx_Nmask
Tx_v32A_E_endif1:
 .if V32BIS_MODEM=ENABLED
	BC_		Tx_v32A_E_endif,BNEQ			;* if rate=14400 ...
	 STPP	#v32A_scrambler6,Tx_scrambler_ptr,B
Tx_v32A_E_endif:
 .endif

 .if TCM_ENCODER=ENABLED
	LD		Tx_rate_pattern,B
	AND		#TCM_BIT,B,A
	CC_		 Tx_init_TCM,ANEQ				;* if pattern&TCM!=0, init_TCM
 .endif

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STPP	#Tx_v32A_B1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_B1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32A_B1
;****************************************************************************

Tx_v32A_B1:
	CALL_	Tx_v32_B1
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_B1()=0

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		#(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED),B
	AND		*AR1(Rx_pattern_detect),B
	SUB		#(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED),B
	LD		#0,A
	BCD_	Tx_v32A_B1_endif1,BNEQ	
	 STL	A,Tx_symbol_counter				;* Tx_symbol_counter=0
	 STL	A,Tx_sample_counter				;* Tx_sample_counter=0
	ANDM	#~(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED|RC_PREAMBLE_DETECTED),*AR1(Rx_pattern_detect)
Tx_v32A_B1_endif1:
 .if $isdefed("V32_STU_III")
	BITF	Tx_mode,#TX_SCRAMBLER_DISABLE_BIT
	BC_		Tx_v32A_B1_endif2,TC			;* if mode&BIT != 0 ...
	STPP	#no_scrambler,Tx_scrambler_ptr,B ;* ... ptr=no_scrambler
Tx_v32A_B1_endif2:
 .endif
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v32A_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32A_MESSAGE_ID,Tx_state_ID
	 
;****************************************************************************
;* Tx_v32A_message
;****************************************************************************

Tx_v32A_message:
	CALL_	Tx_v32_message
	B_		Tx_state_return			

;****************************************************************************
;* Tx_v32A_RC_preamble
;****************************************************************************

Tx_v32A_RC_preamble:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** if counter=0, generate reversal ****

	CMPM	Tx_symbol_counter,#0
	 LD		#9,B							;* B=symbol=9
	 LD		#0ch,A
	CALLD_	v32_diff_encoder
	 XC		1,TC
	  XOR	A,B								;* B=symbol^0xc

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if symbol_counter<LEN
	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		#0,A
	STL		A,Tx_Sreg						;* Tx_Sreg=0
	STL		A,Tx_Sreg_low					;* Tx_Sreg=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_RC_RATE_LEN,Tx_terminal_count
	BITF	*AR1(Rx_pattern_detect),#RC_PREAMBLE_DETECTED
	 ST		#TX_V32A_R4_ID,Tx_state_ID
	XC		2,TC
	 ST		#TX_V32A_R5_ID,Tx_state_ID
	STPP	#Tx_v32A_R3,Tx_state,B
	B_		Tx_state_return

;****************************************************************************
 .endif

	;***************************************
	;**** CALL side transmitter modules ****
	;***************************************

 .if TX_V32C_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v32C:
;* C function call: void Tx_init_v32C(struct START_PTRS *)
;* Initializes Tx_block for v32 modulator
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v32C:					
_V32_MESI_TxInitV32C:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v32C
	POPM	ST1
	RETC_
 .endif

Tx_init_v32C:					
V32_MESI_TxInitV32C:
	CALL_	 Tx_init_v32
	STPP	#v32C_scrambler,Tx_scrambler_ptr,B
	ST		#-1,Tx_terminal_count	
	STPP	#Tx_v32C_silence1,Tx_state,B
	ST		#TX_V32C_SILENCE1_ID,Tx_state_ID

	;**** initialize CALL side receiver ****

 .if RX_V32C_MODEM=ENABLED
	MVDK	Tx_start_ptrs,AR3
	LD		*AR3(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_v32C		
	MVDK	Rx_start_ptrs,AR3
	LD		*AR3(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
 .endif
	RET_

;****************************************************************************
;* _Tx_v32C_retrain:
;* C function call: void Tx_v32C_retrain(struct START_PTRS *)
;* Forces transmitter into retrain sequence.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v32C_retrain:				
_V32_MESI_TxV32CRetrain:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_retrain
	POPM	ST1
	RETC_
 .endif

Tx_v32C_retrain:					
V32_MESI_TxV32CRetrain:
	CALL_	set_Tx_v32_rate_pattern
	ST		#Tx_v32_amp_table,Tx_amp_ptr
	ST		#2,Tx_Nbits
	ST		#3,Tx_Nmask
	LD		#0,A
	STL		A,Tx_symbol_counter	
	STL		A,Tx_sample_counter	
	STL		A,Tx_terminal_count
	STPP	#Tx_v32C_AA,Tx_state,B
	RETD_ 
	 ST		#TX_V32C_AA_ID,Tx_state_ID

;****************************************************************************
;* _Tx_v32C_renegotiate:
;* C function call: void Tx_v32C_renegotiate(struct START_PTRS *)
;* Forces transmitter into renegotiate sequence.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_v32C_renegotiate:				
_V32_MESI_TxV32CRenegotiate:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_renegotiate
	POPM	ST1
	RETC_
 .endif

Tx_v32C_renegotiate:				
V32_MESI_TxV32CRenegotiate:
	CALL_	set_Tx_v32_rate_pattern
	STPP	#v32C_scrambler,Tx_scrambler_ptr,B
	ST		#Tx_v32_amp_table,Tx_amp_ptr		
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	ST		#0,Tx_sample_counter	
	ST		#-TXC_RC_AA_LEN,Tx_symbol_counter	
	ST		#TXC_RC_CC_LEN,Tx_terminal_count	
	STPP	#Tx_v32C_RC_preamble,Tx_state,B
	RETD_ 
	 ST		#TX_V32C_RC_PREAMBLE_ID,Tx_state_ID		

;****************************************************************************
;* Tx_v32C_silence1
;****************************************************************************

Tx_v32C_silence1:
	CALL_	Tx_v32_silence
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if termnal_count<LEN

	;**** synchronize sample[] buffer pointers ****

	CALLD_	Tx_sync_sample_buffers
	 MVDK	Tx_start_ptrs,AR3				;* AR3=start_ptrs
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STL		A,Tx_terminal_count
	STPP	#Tx_v32C_AA,Tx_state,B
	BD_		Tx_state_return			
	 ST		#TX_V32C_AA_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_AA
;****************************************************************************

Tx_v32C_AA:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_	Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** timeout ****/

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_symbol_counter,A
	SUB		#TX_V32C_AA_TIMEOUT,A
	BC_		Tx_v32C_AA_endif1,ALT
	CALL_	GSTN_cleardown					;* GSTN_cleardown			
	BC_		Tx_state_return,ANEQ			;* return if GSTN_cleardown!=0
	CALL_	Tx_init_v32C
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32C_AA_endif1:

	BITF	*AR1(Rx_pattern_detect),#ACCA_HIGH_DETECTED
	BCD_	Tx_AA_continue,NTC				;* branch if !ACCA_DETECTED
	 LD		#5,B							;* B=symbol=5
	 LD		Tx_symbol_counter,A
	SUB		Tx_terminal_count,A
	BC_		Tx_AA_continue,ALT				;* branch if counter<TC

	;**** initialize path measurement in receiver ****

	LD		Tx_num_samples,16,A				;* A=Tx_num_samples
	SUB		Tx_call_counter,16,A			;* A= k=num_samples-call_counter
	ADD		#1,16,A
	ADD		*AR1(Rx_num_samples),16,A		;* A=Tx_num_samples+Rx_num_samples
	ADD		Tx_system_delay,16,A			;* A+=system_delay
	STM		#-NUM_SAMPLES_COEF,T
	MPYA	A								;* A=k*NUM_SAMPLES_COEF
	 SUB	#(64+PATH_FILTER_DELAY+TX_FIR_TAPS/2 +1),16,A
	STH		A,*AR1(Rx_symbol_counter)
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	ST		#2400,Tx_terminal_count
	STPP	#Tx_v32C_CC,Tx_state,A
	ST		#TX_V32C_CC_ID,Tx_state_ID		
	XOR		#0ch,B							;* B=symbol^0xc

Tx_AA_continue:
	CALL_	v32_diff_encoder				;* B=symbol
	B_		Tx_state_return

;****************************************************************************
;* Tx_v32C_CC
;****************************************************************************

Tx_v32C_CC:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_	Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** timeout ****/

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_symbol_counter,A
	SUB		#TX_V32C_CC_TIMEOUT,A
	BC_		Tx_v32C_CC_endif1,ALT
	CALL_	Tx_init_v32C
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32C_CC_endif1:

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	LD		*AR1(Rx_pattern_detect),A		;* ...A=pattern_det
	XC		2,BGEQ							;* if symbol_counter>=TC ...
	 LD		#0ffffh,A						;* ... A=CAAC_LOW_DETECTED
	AND		#CAAC_LOW_DETECTED,A
	BC_		Tx_CC_endif,AEQ					;* branch if !CAAC_LOW_DETECTED
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	STL		A,Tx_terminal_count				;* Tx_terminal_count=0
	STPP	#Tx_v32C_silence2,Tx_state,A
	ST		#TX_V32C_SILENCE2_ID,Tx_state_ID
Tx_CC_endif:

	LD		#5,B							;* B=symbol=5
	CALL_	v32_diff_encoder					;* B=symbol
	B_		Tx_state_return

;****************************************************************************
;* Tx_v32C_silence2
;****************************************************************************

Tx_v32C_silence2:
	CALL_	Tx_v32_silence
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	;**** timeout ****/

	LD		Tx_symbol_counter,A
	SUB		#TX_V32C_SILENCE2_TIMEOUT,A
	BC_		Tx_v32C_silence2_endif0,ALT
	CALL_	Tx_init_v32C
	BD_		Tx_state_return
	 ST		#0,Tx_terminal_count
Tx_v32C_silence2_endif0:

	;**** look for R1 reception ****

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		#(RATE_SIGNAL_DETECTED|RATE_SIGNAL_ACKNOWLEDGED),B
	AND		*AR1(Rx_pattern_detect),B
	SUB		#RATE_SIGNAL_DETECTED,B,A
	BC_		Tx_v32C_silence2_endif1,ANEQ	;* branch if !RATE_SIGNAL_DETECTED
	ORM		#RATE_SIGNAL_ACKNOWLEDGED,*AR1(Rx_pattern_detect)
Tx_v32C_silence2_endif1:
	LD		*AR1(Rx_pattern_detect),B
	AND		#(RATE_SIGNAL_DETECTED|RATE_SIGNAL_ACKNOWLEDGED),B
	SUB		#RATE_SIGNAL_ACKNOWLEDGED,B
	BC_		Tx_state_return,BNEQ			;* return if !RATE_SIGNAL_ACKNOWLEDGED
	ST		#NOT_DETECTED,*AR1(Rx_pattern_detect)

	CALL_	set_Tx_v32_rate
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	SQUR	Tx_num_samples,A	
	STM		#S_DELAY_SCALE,T
	MPYA	A								;* A=N^2*SCALE
	ADD		*AR1(Rx_RTD),1,A
	ADD		#64,1,A							;* A=path+TC+64
	AND		#7ffeh,1,A						;* make sure it's even
	STL		A,-1,Tx_terminal_count
	STPP	#Tx_v32C_S_delay,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_S_DELAY_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_S_delay
;****************************************************************************

Tx_v32C_S_delay:
	CALL_	Tx_v32_S
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	LD		Tx_mode,A
	AND		#V32_SPECIAL_TRAIN_BIT,A
	BC_		Tx_v32C_S_delay_else,AEQ		;* branch if SPECIAL_TRAIN not set
	ST		#TXC_SPECIAL_TRN_LEN,Tx_terminal_count
	STPP	#Tx_v32C_special_TRN,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_SPECIAL_TRN1_ID,Tx_state_ID

Tx_v32C_S_delay_else:
	ST		#TX_S_LEN,Tx_terminal_count
	STPP	#Tx_v32C_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_S1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_special_TRN: special training segment			
;****************************************************************************

Tx_v32C_special_TRN:
	CALL_	Tx_v32_special_TRN
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_S_LEN,Tx_terminal_count
	STPP	#Tx_v32C_S1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_S1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_S1
;****************************************************************************

Tx_v32C_S1:
	CALL_	Tx_v32_S
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_SBAR_LEN,Tx_terminal_count
	STPP	#Tx_v32C_SBAR1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_SBAR1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_SBAR1
;****************************************************************************

Tx_v32C_SBAR1:
	CALL_	Tx_v32_SBAR
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_silence()=0

 .if ECHO_CANCELLER=ENABLED
	LD		#0,A							 ;* A= offset=0
	CALLD_	enable_echo_canceller
	 MVDK	Tx_start_ptrs,AR3
 .endif
	LD		#0,A
	STL		A,Tx_Sreg						;* Tx_Sreg=0
	STL		A,Tx_Sreg_low					;* Tx_Sreg=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#MIN_EC_TRAIN_LEN,Tx_terminal_count
	STPP	#Tx_v32C_TRN,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_TRN1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_TRN
;****************************************************************************

Tx_v32C_TRN:
	CALL_	Tx_v32_TRN
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B,A
	BC_		Tx_state_return,ALT				;* return if symbol_counter<LEN

	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		*AR1(EC_MSE),A
	SUB		#EC_MSE_TRAIN_THRESHOLD,A
	 SUB	#TXC_TRN_LEN,B					;* counter-TRN_LEN
	XC		1,ALEQ
	 LD		#0,B							;* if MSE<=THR, cntr=LEN	
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STL		A,Tx_terminal_count				;* terminal_count=0
	LDM		AR6,A							;* A= k (returned from Tx_v32_TRN)
	ADD		Tx_phase,A						;* A=Tx_phase+k
	AND		#(TX_PHASE_LEN-1),A				;* A=k=(Tx_phase+k)&(TX_PHASE_LEN-1)
	STL		A,Tx_phase						;* update Tx_phase
	STPP	#Tx_v32C_R2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_R2_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_R2
;****************************************************************************

Tx_v32C_R2:
	CALL_	Tx_v32_rate
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_rate()=0

	;**** check for rate signal boundaries ****

	LD		Tx_symbol_counter,16,B
	SUB		Tx_terminal_count,16,B,A
	BCD_	Tx_state_return,ALT				;* return if counter<TC
	 AND	#7,16,B							;* B=symbol_counter&7
	BCD_	Tx_state_return,BNEQ			;* return if symbol_counter&7!=0

	;**** check for R4 timeout ****

	 CMPM	Tx_state_ID,#TX_V32C_R4_ID
	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	BC_		Tx_v32C_R2_endif2,NTC			;* branch if !TX_V32C_R4_ID
	LD		Tx_symbol_counter,A
	SUB		*AR1(Rx_RTD),A
	SUB		#TX_R4_TIMEOUT,A				;* A=symbol_counter-TIMEOUT-path
	BC_		Tx_v32C_R2_endif2,ALEQ			;* branch if counter<=TIMEOUT+path
	CALL_	Tx_v32C_retrain
	B_		Tx_state_return
Tx_v32C_R2_endif2:

	;**** look for rate signal reception ****

	BITF	*AR1(Rx_pattern_detect),#RATE_SIGNAL_DETECTED
	 LD		Tx_state_ID,A
	 SUB	#TX_V32C_R2_ID,A
	XC		1,TC							;* if RATE_SIGNAL_DETECTED ...
	 LD		Tx_state_ID,A
	BC_		Tx_state_return,AEQ				;* return if state_ID=TX_V32C_R2_ID

	LD		*AR1(Rx_pattern_detect),B
	AND		#(RATE_SIGNAL_DETECTED|RC_PREAMBLE_DETECTED),B
	SUB		#(RATE_SIGNAL_DETECTED|RC_PREAMBLE_DETECTED),B
	 LD		Tx_state_ID,A
	 SUB	#TX_V32C_R4_ID,A
	XC		1,BEQ		
	 LD		Tx_state_ID,A
	BC_		Tx_state_return,AEQ				;* return if state_ID=TX_V32C_R4_ID

	;**** look for GSTN cleardown ****

	CMPM	Tx_state_ID,#TX_V32C_R2_ID
	LD		#0,A
	CC_		GSTN_cleardown,TC				;* if TX_V32C_R2_ID, check for GSTN_CLEARDOWN
	BC_		Tx_state_return,ANEQ			;* return if GSTN_cleardown()!=0

	CALL_	set_Tx_v32_rate
	CALL_	set_Tx_v32_E
	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	CMPM	Tx_state_ID,#TX_V32C_R2_ID
	 ST		#TX_RC_B1_LEN,Tx_terminal_count
	XC		2,TC							;* if state_ID=R2 ...
	 ST		#32767,Tx_terminal_count		;* ... terminal_count=32767
	STPP	#Tx_v32C_E,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_E_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_E
;****************************************************************************

Tx_v32C_E:
	CALL_	Tx_v32_rate
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_rate()=0

	;**** look for GSTN cleardown ****

	BITF	Tx_rate_pattern,#GSTN_CLEARDOWN_MASK
	LD		Tx_symbol_counter,B
	BCD_	Tx_v32C_E_endif0,TC				;* branch if !GSTN_CLEARDOWN
	 SUB	#MIN_GSTN_CLEARDOWN_LEN,B,A
	BC_		Tx_state_return,ALT				;* return if symbol_counter<LEN
	CALL_	GSTN_cleardown					;* GSTN_cleardown			
	B_		Tx_state_return
Tx_v32C_E_endif0:

	SUB		#TX_E_LEN,B
	BC_		Tx_state_return,BLT				;* return if symbol_counter<LEN
	LD		Tx_rate,B
	SUB		#V32_RATE_9600,B,A
	BC_		Tx_v32C_E_endif1,ANEQ			;* branch if rate!=9600
	ST		#4,Tx_Nbits
	ST		#0fh,Tx_Nmask
Tx_v32C_E_endif1:

 .if TCM_ENCODER=ENABLED
	LD		Tx_rate_pattern,B
	AND		#TCM_BIT,B,A
	CC_		 Tx_init_TCM,ANEQ				;* if pattern&TCM!=0, init_TCM
 .endif

	LD		#0,A
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	STPP	#Tx_v32C_B1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_B1_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_B1
;****************************************************************************

Tx_v32C_B1:
	CALL_	Tx_v32_B1
	BC_		Tx_state_return,AEQ				;* return if Tx_v32_B1()=0

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if symbol_counter<LEN

	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		#(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED),B
	AND		*AR1(Rx_pattern_detect),B
	SUB		#(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED),B
	LD		#0,A
	BCD_	Tx_v32C_B1_endif1,BNEQ	
	 STL	A,Tx_symbol_counter				;* Tx_symbol_counter=0
	 STL	A,Tx_sample_counter				;* Tx_sample_counter=0
	ANDM	#~(RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED|RC_PREAMBLE_DETECTED),*AR1(Rx_pattern_detect)
Tx_v32C_B1_endif1:
 .if $isdefed("V32_STU_III")
	BITF	Tx_mode,#TX_SCRAMBLER_DISABLE_BIT
	BC_		Tx_v32C_B1_endif2,TC			;* if mode&BIT != 0 ...
	STPP	#no_scrambler,Tx_scrambler_ptr,B ;* ... ptr=no_scrambler
Tx_v32C_B1_endif2:
 .endif
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v32C_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V32C_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v32C_message
;****************************************************************************

Tx_v32C_message:
	CALL_	Tx_v32_message
	B_		Tx_state_return			

;****************************************************************************
;* Tx_v32C_RC_preamble
;****************************************************************************

Tx_v32C_RC_preamble:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** if counter=0, generate reversal ****

	CMPM	Tx_symbol_counter,#0
	 LD		#5,B							;* B=symbol
	 LD		#0ch,A
	CALLD_	v32_diff_encoder
	 XC		1,TC
	  XOR	A,B								;* B=symbol^0xc

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if symbol_counter<LEN
	 MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		#0,A
	STL		A,Tx_Sreg						;* Tx_Sreg=0
	STL		A,Tx_Sreg_low					;* Tx_Sreg=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0;
	ST		#TX_RC_RATE_LEN,Tx_terminal_count
	BITF	*AR1(Rx_pattern_detect),#RC_PREAMBLE_DETECTED
	 ST		#TX_V32C_R4_ID,Tx_state_ID
	XC		2,TC
	 ST		#TX_V32C_R5_ID,Tx_state_ID
	STPP	#Tx_v32C_R2,Tx_state,B
	B_		Tx_state_return

;****************************************************************************
 .endif

	;************************************
	;**** common transmitter modules ****
	;************************************

 .if TX_V32_MODEM=ENABLED
;****************************************************************************
;* Tx_init_v32:
;* Initializes Tx_block[] workspace for v32 operation
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_v32:
	LD		#0,A
;++++#ifndef MESI_INTERNAL 02-27-2001
;	ST		#(_RCOS2400_f1800+2*(RX_COEF_LEN-TX_COEF_LEN)),Tx_coef_start
;++++#else   MESI_INTERNAL 02-27-2001
	ST		#(_RCOS2400_f1800+(RX_COEF_LEN-TX_COEF_LEN)),Tx_coef_start
;++++#endif  MESI_INTERNAL 02-27-2001
	ST		#(2*TX_INTERP),Tx_interpolate	 
	ST		#(2*TX_DEC),Tx_decimate		 
	STL		A,Tx_sym_clk_offset				;* Tx_sym_clk_offset=0;
	STL		A,Tx_sym_clk_memory				;* Tx_sym_clk_memory=0;
	STL		A,Tx_sym_clk_phase				;* Tx_sym_clk_phase=0;
	ST		#TX_CARRIER,Tx_carrier	
	ST		#Tx_v32_phase_map,Tx_map_ptr	
	ST		#Tx_v32_amp_table,Tx_amp_ptr	
	ST		#TX_V32_SCALE,Tx_fir_scale	

	MVDK	Tx_start_ptrs,AR0				;* AR0=start_ptrs
	LD		*AR0(Tx_fir_start),B
	STL		B,Tx_fir_head					;* Tx_fir_head=&Tx_fir[0]
	STL		B,Tx_fir_tail					;* Tx_fir_tail=&Tx_fir[0]
	ST		#TX_FIR_LEN,Tx_fir_len

	MVDK	Tx_fir_head,AR0
	STM		#(TX_FIR_LEN-1),BRC
	RPTB	v32_init_Tx_fir_loop
v32_init_Tx_fir_loop:
	 STL	A,*AR0+							;* Tx_fir[*++]=0

	ST		#(TX_FIR_TAPS-1),Tx_fir_taps		
	STL		A,Tx_coef_ptr	
	STL		A,Tx_phase		
	ST		#2,Tx_Nbits		
	ST		#3,Tx_Nmask		
	STL		A,Tx_symbol_counter 
	STL		A,Tx_sample_counter 
	STL		A,Tx_terminal_count 
	STL		A,Tx_Sreg		 
	STL		A,Tx_Sreg_low	 
	LD		Tx_rate,A
	STL		A,Tx_max_rate					;* max_rate=Tx_rate
	CALL_	set_Tx_v32_rate_pattern
	RET_

;****************************************************************************
;* Tx_v32_silence: generates silence
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=1 if symbol_counter>=terminal_count			
;****************************************************************************

Tx_v32_silence:						
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	STL		A,*AR7+%
	STL		A,*AR7+%
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	RC_		BLT								;* return(0) if symbol_counter<LEN 
	LD		#1,A
	RET_									;* return(1)

;****************************************************************************
;* Tx_v32_S: generates ABAB alternations
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=1 if symbol_counter>=terminal_count			
;****************************************************************************

Tx_v32_S:	
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	LD		Tx_symbol_counter,B
	AND		#1,B
	LD		#2,A							;* A=phase=2
	CALLD_	v32_abs_encoder
	 XC		1,BEQ
	  LD	#3,A							;* if counter&1=0, phase=3
	LD		#0,A
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	RC_		BLT								;* return(0) if symbol_counter<LEN 
	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Tx_v32_SBAR: generates CDCD alternations
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=1 if symbol_counter>=terminal_count			
;****************************************************************************

Tx_v32_SBAR:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail
	LD		Tx_symbol_counter,B
	 AND	#1,B
	 LD		#0,A							;* A=phase=0
	CALLD_	v32_abs_encoder
	 XC		1,BEQ
	  LD	#1,A							;* if counter&1=0, phase=1
	LD		#0,A
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	RC_		BLT								;* return(0) if symbol_counter<LEN 
	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Tx_v32_TRN: generates v32 2/4 phase training sequence.
;* It returns the absolute phase value,k in AR6
;****************************************************************************

Tx_v32_TRN:	
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	;**** call scrambler ****

	LDPP	Tx_scrambler_ptr,B				;* B=scrambler_ptr
	CALAD_	B						 		;* branch to *scrambler_ptr
	 LD		#3,A
	 LD		Tx_Nbits,T

	;**** translate symbol ****

	SUB		#0,B,A							;* A=symbol-0
	 STM	#0,AR6							;* AR6= k=0
	 NOP 
	   XC	2,AEQ				
	 MAR	*+AR6(2)						;* if symbol=0, k=2
	SUB		#1,B,A							;* A=symbol-1
	 NOP 
	 NOP 
	XC		2,AEQ				
	 MAR	*+AR6(3)						;* if symbol=1, k=3
	SUB		#2,B,A							;* A=symbol-2
	 NOP 
	 NOP 
	XC		2,AEQ				
	 MAR	*+AR6(1)						;* if symbol=3, k=1
	AND		Tx_Nbits,B						;* B=symbol&2
	 STM	#0,AR0							;* A= k(BPSK)=0
	XC		2,BEQ				
	 STM	#2,AR0							;* if (symbol&2)==0, A=k=2
	LD		Tx_symbol_counter,B
	SUB		#TX_BPSK_TRAIN_LEN,B
	LDM		AR6,A							;* A=k
	CALLD_	v32_abs_encoder
	XC		1,BLT				
	 LDM	AR0,A							;* if counter<LEN, A=AR0
	RET_

;****************************************************************************
;* Tx_v32_special_TRN: generates special TRN sequence
;****************************************************************************

Tx_v32_special_TRN:				
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	;**** AACC alternations ****

	LD		Tx_symbol_counter,B
	 AND	#2,B
	 LD		#0,A							;* A=phase=0
	CALLD_	v32_abs_encoder
	 XC		1,BEQ
	  LD	#2,A							;* if counter&2=0, phase=2
	RET_

;****************************************************************************
;* no_scrambler: just return
;* Expects the following on entry:
;*	A=in
;* On exit:
;*	B=in
;****************************************************************************

 .if $isdefed("V32_STU_III")
no_scrambler:
;++++#ifndef MESI_INTERNAL 03-14-2001
	LD		A,B								;* rturn(in)
;++++#endif  MESI_INTERNAL 03-14-2001
	RET_
 .endif
;****************************************************************************
;* Tx_v32_rate: generates rate signal
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=if symbol produced.
;****************************************************************************

Tx_v32_rate:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	;**** rate signal generation ****

	LD		Tx_symbol_counter,A
	SUB		#1,A
	SFTL	A,1								;* A=(symbol_counter-1)<<1
	AND		#0fh,A							;* A=((symbol_counter-1)<<1)&0xf
	SUB		#14,A							;* A-=14
	STLM	A,T
	 NOP
	LDPP	Tx_scrambler_ptr,B				;* B=scrambler_ptr
	LD		Tx_rate_pattern,TS,A			;* A=rate_pattern>>index
	CALAD_	B						 		;* branch to *scrambler_ptr
	 AND	Tx_Nmask,A						;* A=symbol&3
	 LD		Tx_Nbits,T
	SFTL	B,2								;* B=scrambler<<2
	CALLD_	v32_diff_encoder
	 OR		#1,B							;* B=(scrambler<<2)|1
	LD		#1,A
	RET_	  								;* return(1)

;****************************************************************************
;* Tx_v32_B1: generates scrambled binary ones 
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=if symbol produced.
;****************************************************************************

Tx_v32_B1:	
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	;**** transmit scrambled binary ones ****

	LDPP	Tx_scrambler_ptr,B				;* B=scrambler_ptr
	CALAD_	B						 		;* branch to *scrambler_ptr
	  LD	Tx_Nmask,A
	 LD		Tx_Nbits,T

 .if TCM_ENCODER=ENABLED
	LD		Tx_rate_pattern,A
	AND		#TCM_BIT,A
	BC_		Tx_v32_B1_endif1,ANEQ
 .endif
	LD		Tx_rate,A
	SUB		#V32_RATE_4800,A			
	BC_		Tx_v32_B1_endif2,ANEQ			;* branch if rate!=4800
	SFTL	B,2								;* B=k<<2
	OR		#1,B							;* B=(k<<2)|1
Tx_v32_B1_endif2:
	CALL_	v32_diff_encoder
	LD		#1,A
	RET_	  								;* return(1)

 .if TCM_ENCODER=ENABLED
Tx_v32_B1_endif1:
	CALL_	TCM_encoder
	LD		#1,A
	RET_	  								;* return(1)
 .endif

;****************************************************************************
;* Tx_v32_message: scrambles and transmits message data.
;* returns:
;*		A=0 if symbol modulation not completed yet
;*		A=if symbol produced.
;****************************************************************************

Tx_v32_message:					
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	LD		#0,A
	RC_		BNEQ							;* return(0) if head!=tail

	;**** test for Tx_data[] underflow ****

	LDU		Tx_data_head,B
	SUBS	Tx_data_tail,B
	 MVDK	Tx_data_tail,AR7
	MVDK	Tx_data_len,BK
	LD		Tx_Nmask,A				
	XC		1,BNEQ							;* if head!=tail ...
	 LD		*AR7+%,A						;* ... A=Tx_data[*++%]
	LDPP	Tx_scrambler_ptr,B				;* B=scrambler_ptr
	LD		Tx_Nbits,T
	CALAD_	B						 		;* branch to *scrambler_ptr
	 MVKD	AR7,Tx_data_tail

	;**** scramble the data ****

 .if TCM_ENCODER=ENABLED
	LD		Tx_rate_pattern,A
	AND		#TCM_BIT,A
	BC_		Tx_v32_message_endif1,ANEQ		;* branch if TCM_BIT not set
 .endif

	LD		Tx_rate,A
	SUB		#V32_RATE_4800,A			
	BC_		Tx_v32_message_endif2,ANEQ		;* branch if rate!=4800
	SFTL	B,2								;* B=k<<2
	OR		#1,B							;* B=(k<<2)|1
Tx_v32_message_endif2:
	CALL_	v32_diff_encoder
	LD		#1,A
	RET_	  								;* return(1)

 .if TCM_ENCODER=ENABLED
Tx_v32_message_endif1:
	CALL_	TCM_encoder
	LD		#1,A
	RET_	  								;* return(1)
 .endif

;****************************************************************************
;* v32_abs_encoder: 
;* Absolute phase/amplitude encoder puts the signalling element at the 
;* specified amplitude and phase where:						
;*			phase is 4*(phase/2pi) in pi/2 increments			
;* Expects the following on entry:
;*	A=phase
;****************************************************************************

v32_abs_encoder:					
 .if ON_CHIP_COEFFICIENTS=ENABLED
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	MVDK	Tx_fir_len,BK
	LD		Tx_carrier,B					;* B=Tx_carrier
	ADD		Tx_phase,B						;* B=Tx_phase+Tx_carrier
	AND		#(TX_PHASE_LEN-1),B				;* B=(Tx_phase+Tx_carrier&(TX_PHASE_LEN-1)
	STL		B,Tx_phase						;* update Tx_phase
	ADD		B,A								;* A=Tx_phase+phase
	AND		#(TX_PHASE_LEN-1),A				;* A=k=(Tx_phase+phase&(TX_PHASE_LEN-1)
	OR		#4,A							;* A= k=4|(Tx_phase+phase&(TX_PHASE_LEN-1)
	SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=Tx_amp_ptr+2*k
	STLM	A,AR0
	 NOP
	 LD		Tx_fir_scale,T				
	MPY		*AR0+,B							;* B=fir_scale*real
	STH		B,*AR7+%
	MPY		*AR0,B							;* B=fir_scale*imag
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head			   		;* update Tx_fir_head
 .else
	LD		Tx_carrier,B					;* B=Tx_carrier
	ADD		Tx_phase,B						;* B=Tx_phase+Tx_carrier
	AND		#(TX_PHASE_LEN-1),B				;* B=(Tx_phase+Tx_carrier&(TX_PHASE_LEN-1)
	STL		B,Tx_phase						;* update Tx_phase
	ADD		B,A								;* A=Tx_phase+phase
	AND		#(TX_PHASE_LEN-1),A				;* A=k=(Tx_phase+phase&(TX_PHASE_LEN-1)
	OR		#4,A							;* A= k=4|(Tx_phase+phase&(TX_PHASE_LEN-1)
	SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=Tx_amp_ptr+2*k
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	MVDK	Tx_fir_len,BK
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
;* v32_diff_encoder: differentially encodes the data bits for v32.
;* Expects the following on entry:
;*	B=symbol
;****************************************************************************

v32_diff_encoder:				
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
	 MVKD	AR7,Tx_fir_head			 		;* update Tx_fir_head
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
;* v32A_scrambler: V32 GPA scrambler 
;* v32C_scrambler: V32 GPC scrambler 
;* Expects the following on entry:
;*	A=in
;*	T=Nbits
;* On exit:
;*	B=out
;****************************************************************************

v32A_scrambler:
	LD		Tx_Sreg,-2,B					;* B=Sreg>>18
	XOR		Tx_Sreg_low,B					;* B=Sreg^Sreg>>(18)
	NORM	B								;* B=Sreg<<N ^ Sreg>>(18-N)
	SFTL	B,-5							;* B=Sreg>>(5-N) ^ Sreg>>(23-N)
	XOR		A,B								;* A=in^Sreg>>(5-N)^Sreg>>(23-N)
	AND		Tx_Nmask,B						;* A=(Sreg>>(5-N)^Sreg>>(23-N))&Nmask
	LD		Tx_Sreg,16,A
	ADDS	Tx_Sreg_low,A					;* A=Sreg
	NORM	A								;* A=(Sreg<<Nbits)
	OR		B,A								;* A=(Sreg<<Nbits)|out
	RETD_
	 STH	A,Tx_Sreg						;* update Sreg
	 STL	A,Tx_Sreg_low				

v32C_scrambler:
	LD		Tx_Sreg,16,B
	ADDS	Tx_Sreg_low,B					;* B=Sreg
	SFTL	B,-6							;* B=Sreg>>6
	NORM	B
	SFTL	B,-12							;* B=Sreg>>(18-N)
	XOR		B,-5							;* B=Sreg>>(18-N)^Sreg>>(23-N)
	XOR		A,B								;* A=in^Sreg>>(18-N)^Sreg>>(23-N)
	AND		Tx_Nmask,B						;* A=(Sreg>>(18-N)^Sreg>>(23-N))&Nmask
	LD		Tx_Sreg,16,A
	ADDS	Tx_Sreg_low,A					;* A=Sreg
	NORM	A								;* A=(Sreg<<Nbits)
	OR		B,A								;* A=(Sreg<<Nbits)|out
	RETD_
	 STH	A,Tx_Sreg						;* update Sreg
	 STL	A,Tx_Sreg_low				

;****************************************************************************
;* v32A_scrambler6: V32 GPA scrambler for Nbits=6
;* Expects the following on entry:
;*	A=in
;*	T=Nbits
;* On exit:
;*	B=out
;****************************************************************************

 .if V32BIS_MODEM=ENABLED
v32A_scrambler6:
	STLM	A,T								;* T=in
	LD		Tx_Sreg,-2,B					;* B=Sreg>>18
	XOR		Tx_Sreg_low,B					;* B=Sreg^Sreg>>(18)
	SFTL	B,1								;* B=(Sreg<<1)^(Sreg>>17)
	XOR		A,B								;* B=in^(Sreg<<1)^(Sreg>>17)
	AND		#3eh,B							;* B=((Sreg<<1)^(Sreg>>17))&0x3e
	LD		Tx_Sreg,16,A
	ADDS	Tx_Sreg_low,A					;* A=Sreg
	SFTL	A,6								;* A=Sreg<<6
	OR		B,A								;* A=(Sreg<<6)|out
	STH		A,Tx_Sreg						;* update Sreg
	STL		A,Tx_Sreg_low				

	LDM		T,A								;* A=in
	LD		Tx_Sreg,-2,B					;* B=Sreg>>18
	XOR		Tx_Sreg_low,B					;* B=Sreg^Sreg>>(18)
	SFTL	B,-5							;* B=Sreg>>(5) ^ Sreg>>(23)
	XOR		A,B								;* A=in^Sreg>>(5-N)^Sreg>>(23-N)
	AND		#1,B							;* A=(in^Sreg>>(5-N)^Sreg>>(23-N))&1
	OR		Tx_Sreg_low,B					;* B=Sreg|out
	STL		B,Tx_Sreg_low				
	RETD_
	 AND	#3fh,B							;* B=Sreg&0x3f
 .endif

;****************************************************************************
; set_Tx_v32_rate: configures Tx_rate
;* On entry it expects:
;*	DP => &Tx_block
;****************************************************************************

set_Tx_v32_rate:					
	MVDK	Tx_start_ptrs,AR1
	MVDK	*AR1(Rx_block_start),AR1
	LD		Tx_rate_pattern,A
	AND		*AR1(Rx_rate_pattern),A
	STL		A,Tx_rate_pattern				;* Tx_rate_pattern&=Rx_rate_pattern

	ST		#0,Tx_rate
	AND		#RATE_4800_BIT,A,B
	BCD_	set_rate_4800_endif,BEQ			;* branch if pattern&4800=0
	 AND	#RATE_7200_BIT,A,B
	ST		#V32_RATE_4800,Tx_rate
set_rate_4800_endif:
	BCD_	set_rate_7200_endif,BEQ			;* branch if pattern&7200=0
	 AND	#RATE_9600_BIT,A,B
	ST		#V32_RATE_7200,Tx_rate
set_rate_7200_endif:
	BCD_	set_rate_9600_endif,BEQ			;* branch if pattern&9600=0
	 AND	#RATE_12000_BIT,A,B
	ST		#V32_RATE_9600,Tx_rate
set_rate_9600_endif:
	BCD_	set_rate_12000_endif,BEQ		;* branch if pattern&12000=0
	 AND	#RATE_14400_BIT,A,B
	ST		#V32_RATE_12000,Tx_rate
set_rate_12000_endif:
	BC_		set_rate_14400_endif,BEQ		;* branch if pattern&14400=0
	ST		#V32_RATE_14400,Tx_rate
set_rate_14400_endif:
	RET_

;****************************************************************************
;* _set_Tx_v32_rate_pattern:
;* C function call: void set_Tx_v32_rate_pattern(struct START_PTRS *)
;* Sets Tx_rate_pattern from current Tx_rate
;****************************************************************************

 .if COMPILER=ENABLED
_set_Tx_v32_rate_pattern:			
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	set_Tx_v32_rate_pattern
	POPM	ST1
	RETC_
 .endif

set_Tx_v32_rate_pattern:			
	ST		#RATE_SIGNAL_PATTERN,Tx_rate_pattern
	LD		Tx_max_rate,A
	SUB		#V32_RATE_4800,A,B
	BCD_	set_pattern_4800_endif,BNEQ
	 SUB	#V32_RATE_7200,A,B
	ORM		#RATE_4800_PATTERN,Tx_rate_pattern
set_pattern_4800_endif:
	BCD_	set_pattern_7200_endif,BNEQ
	 SUB	#V32_RATE_9600,A,B
	ORM		#RATE_7200_PATTERN,Tx_rate_pattern
set_pattern_7200_endif:
	BCD_	set_pattern_9600_endif,BNEQ
	 SUB	#V32_RATE_12000,A,B
	ORM		#RATE_9600_PATTERN,Tx_rate_pattern
set_pattern_9600_endif:
	BCD_	set_pattern_12000_endif,BNEQ
	 SUB	#V32_RATE_14400,A,B
	ORM		#RATE_12000_PATTERN,Tx_rate_pattern
set_pattern_12000_endif:
	BC_		set_pattern_14400_endif,BNEQ
	ORM		#RATE_14400_PATTERN,Tx_rate_pattern
set_pattern_14400_endif:

	LD		Tx_mode,A
	AND		#V32BIS_MODE_BIT,A
	SUB		#V32BIS_MODE_BIT,A
	BC_		set_pattern_bis_endif,AEQ
	ANDM	#(~(RATE_7200_BIT|RATE_12000_BIT|RATE_14400_BIT)),Tx_rate_pattern
set_pattern_bis_endif:

	LD		Tx_mode,A
	AND		#V32TCM_MODE_BIT,A
	SUB		#V32TCM_MODE_BIT,A
	BC_		set_pattern_TCM_endif,AEQ
	ANDM	#(~TCM_BIT),Tx_rate_pattern
set_pattern_TCM_endif:

set_pattern_endif:
	RET_

;****************************************************************************
;* set_Tx_v32_E: configures Tx_rate_pattern for signal E from specified Tx_rate.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

set_Tx_v32_E:
	ANDM	#(V32BIS_BITS|RATE_SIGNAL_PATTERN),Tx_rate_pattern
	ORM		#SIGNAL_E_PATTERN,Tx_rate_pattern
	LD		Tx_rate,A
	SUB		#V32_RATE_4800,A,B
	BCD_	set_Epattern_4800_endif,BNEQ
	 SUB	#V32_RATE_7200,A,B
	ORM		#RATE_4800_BIT,Tx_rate_pattern
set_Epattern_4800_endif:
	BCD_	set_Epattern_7200_endif,BNEQ
	 SUB	#V32_RATE_9600,A,B
	ORM		#RATE_7200_BIT,Tx_rate_pattern
set_Epattern_7200_endif:
	BCD_	set_Epattern_9600_endif,BNEQ
	 SUB	#V32_RATE_12000,A,B
	ORM		#RATE_9600_BIT,Tx_rate_pattern
set_Epattern_9600_endif:
	BCD_	set_Epattern_12000_endif,BNEQ
	 SUB	#V32_RATE_14400,A,B
	ORM		#RATE_12000_BIT,Tx_rate_pattern
set_Epattern_12000_endif:
	BC_		set_Epattern_14400_endif,BNEQ
	ORM		#RATE_14400_BIT,Tx_rate_pattern
set_Epattern_14400_endif:
	RET_

;****************************************************************************
;* GSTN_cleardown(): Checks for GSTN_CLEARDOWN in Tx_rate_pattern, and upon 
;* detection switches to Tx_silence and Rx_ilde states.
;* On return:
;* 	A=0 if GSTN_CLEARDOWN is not detected
;*	A=1 if GSTN_CLEARDOWN is detected and states switched
;****************************************************************************

GSTN_cleardown:
	BITF	Tx_rate_pattern,#GSTN_CLEARDOWN_MASK
	LD		#0,A
	RC_		TC								;* return(0) if !GSTN_CLEARDOWN_MASK

	CALL_	Tx_init_silence
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_idle
	ST		#GSTN_CLEARDOWN_REQUESTED,Rx_status
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	LD		#1,A
	RET_									;* return(1)

;****************************************************************************
 .endif

	;**************************************
	;**** ANSWER side receiver modules ****
	;**************************************

 .if RX_V32A_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v32A:
;* Initializes Rx_block[] workspace for v32 ANSWER operation.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_v32A:					
	CALL_	Rx_init_v32						;* call general init
	STPP	#v32A_descrambler,Rx_descrambler_ptr,B
	ST		#NOT_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32A_detect_AA,Rx_state,B
	RETD_
	 ST		#RX_V32A_DETECT_AA_ID,Rx_state_ID		

;****************************************************************************
;* Rx_v32A_detect_AA
;****************************************************************************

Rx_v32A_detect_AA:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,B
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	;**** reject modulated signal component at 2100 Hz ****

	MVMM	AR2,AR7							;* AR7=Rx_sample_tail
	MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len
	SSBX	TC								;* enable filter
	BPF		FILTER_2100_COEF,RX_ANALYSIS_LEN,AR7
	NEG		A
	MAC		What,#(THR_14DB),A				;* A=What*THR_14DB-F2100
	 ADDM	#1,Dcounter						;* Dcounter++
	XC		2,ALT							;* if F1800*THR_14DB-F2100<0 ...
	 ST		#0,Dcounter						;* ... Dcounter=0

	;**** detect energy in 1800 Hz filter ****

	LD		What,B
	SUB		Rx_threshold,B,A				;* F1800-THR
	 STL	B,agc_gain						;* agc_gain=Psum
	 SQUR	agc_gain,B						;* B=Psum^2
	XC		2,ALT							;* if F1800<THR ...
	 ST		#0,Dcounter						;* ... Dcounter=0
	LD		Dcounter,A
	SUB		#RX_AA_DETECT_LEN,A
	BC_		Rx_state_return,ALT				;* return if Dcounter<RX_AA_DETECT_LEN
	STH		B,Rx_power						;* Rx_power=(Psum^2)/2
	ST		#0,Dcounter						;* if What<THR, Dcounter=0
	ORM		#AA_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32A_detect_AACC,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_DETECT_AACC_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_detect_AACC
;****************************************************************************

Rx_v32A_detect_AACC:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,B
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	BITF	Rx_pattern_detect,#AACC_LOW_DETECTED
	BC_		AA_detect_else,TC				;* branch if AACC_LOW_DETECTED
	LD		What,16,B
	MAC		agc_gain,#(-THR_6DB),B			;* B=What-agc_gain*THR_6DB
	BC_		Rx_state_return,BGEQ			;* return if What>=agc_gain*THR
	ST		#0,Dcounter
	ST		#32767,LOS_counter
	BD_		Rx_state_return
	 ORM	#AACC_LOW_DETECTED,Rx_pattern_detect
AA_detect_else:

	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	LD		What,B
	SUB		LOS_counter,B,A					;* What-LOS_counter
	BCD_	AA_detect_else2,AGEQ			;* branch if What>=LOS_counter
	ADDM	#1,Dcounter						;* Dcounter++
	STL		B,LOS_counter					;* LOS_counter=What
	MVKD	AR2,Rx_sample_tail
	LDU		Rx_sample_stop,B
	SUBS	Rx_sample_tail,B
	LDU		*AR1(Tx_num_samples),A		
	ADDS	Rx_sample_stop,A
	SUBS	Rx_sample_tail,A				;* A=num_samples+sample_stop-sample_tail
	XC		1,BLT							;* if sample_stop-sample_tail<0 ...
	 ADD	Rx_sample_len,A					;* ... A+=Rx_sample_len
	ADD		*AR1(Tx_system_delay),A			;* A+=system_delay
	STLM	A,T								;* T=k
	MPY		#NUM_SAMPLES_COEF,A				;* A=k*NUM_SAMPLES_COEF
	SUB		#(64-PATH_FILTER_DELAY-TX_FIR_TAPS/2 -1),16,A
	STH		A,*AR1(Tx_symbol_counter)	
	LD		Rx_symbol_counter,16,A
	STH		A,Rx_RTD
AA_detect_else2:
	LD		Dcounter,B
	SUB		#PATH_WINDOW,B
	BCD_	Rx_state_return,BLT				;* return if Dcounter<PATH_WINDOW
	 STM	#NUM_SAMPLES_COEF,T

	LD		Rx_RTD,16,A				
	MAS		*AR1(Tx_sample_len),A			;* A=RTD-Tx_sample_len*COEF
	MAC		*AR1(Tx_num_samples),A			;* A+=Tx_num_samples*COEF
	MAC		#V32_FEC_LEN,A					;* A=V32_FEC_LEN*COEF
	BC_		Rx_v32A_detect_AACC_endif,ALEQ
	ST		#0,Rx_RTD
	ST		#EXCESSIVE_RTD_STATUS,Rx_status
Rx_v32A_detect_AACC_endif:
	ST		#0,Dcounter
	ORM		 #AACC_HIGH_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32A_detect_CC_end,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32A_DETECT_CC_END_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_detect_CC_end
;****************************************************************************

Rx_v32A_detect_CC_end:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,B
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	CMPM	EC_2mu,#EC_TRAIN_DISABLED
	 LD		What,16,B
	 MAC	agc_gain,#(-THR_6DB),B			;* B=What-agc_gain*THR_6DB
	XC		1,NTC							;* if EC_2mu!=EC_TRAIN_DISABLED ...
	 LD		#0,B							;* ... B!>0
	BC_		Rx_state_return,BGT				;* return if What>agc_gain*THR
	LD		#0,A
	STL		A,Rx_pattern_reg
	STL		A,LOS_counter
	STL		A,Rx_sample_counter
	ST		#WHAT_13,What
	ORM		#CC_END_DETECTED,Rx_pattern_detect
	MVKD	AR2,EC_sample_ptr				;* EC_sample_ptr=Rx_sample_tail
	STPP	#Rx_v32A_train_EC,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32A_TRAIN_EC_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_train_EC
;****************************************************************************

Rx_v32A_train_EC:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller
 .else
	MVDK	Rx_start_ptrs,AR1				;* AR6=Rx_start_ptrs
	MVDK	*AR1(Tx_block_start),AR1		;* AR1=&Tx_block_start
 .endif

	;**** dump_write debugging facility - enable to view signals ****

;	LD		*AR2,A
;	CALL_	dump_write		

	CMPM	*AR1(Tx_state_ID),#TX_V32A_R1_ID
	BC_		Rx_v32A_train_EC_endif,NTC		;* return if Tx_state_ID!=RATE1
	LD		#0,A
	ST		#EC_TRAIN_DISABLED,EC_2mu
	STL		A,Rx_rate_pattern
	STL		A,Rx_sample_counter
	STL		A,Rx_symbol_counter
	STPP	#Rx_v32A_S_detect,Rx_state,B
	ST		#RX_V32A_S_DETECT_ID,Rx_state_ID
Rx_v32A_train_EC_endif:

	MVDK	Rx_sample_stop,AR0
	CMPR	0,AR2							;* sample_tail-sample_stop
	BC   	Rx_state_return,TC				;* return if tail=stop	
	MAR		*AR2+%							;* AR2=Rx_sample_tail++%
	BD_		Rx_state_return			
	 ADDM #1,Rx_sample_counter	

;****************************************************************************
;* Rx_v32A_S_detect
;****************************************************************************

Rx_v32A_S_detect:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_S_detect
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_S_detect()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,B
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	BITF	Rx_pattern_detect,#S_DETECTED
	BC_		A_S_detect_endif1,NTC			;* branch if !S_DETECTED
	STPP	#Rx_v32A_train_loops,Rx_state,B
	ST		#RX_V32A_TRAIN_LOOPS_ID,Rx_state_ID
A_S_detect_endif1:

	;**** look for AA pattern ****

	LD		What,16,B
	SUB		Rx_threshold,16,B,A				;* P1800-THR_43DB_RMS
	 ADDM	#1,LOS_counter					;* LOS_counter++
	XC		2,ALT
	 ST		#0,LOS_counter					;* if Psum<THR, LOS_counter=0
	MPY		What,#THR_10DB,A				;* A=P1800*THR_10DB
	SUB		Ihat,16,A						;* A=Psum*THR-P600
	SUB		Qhat,16,A						;* A=Psum*THR-P600-P3000
	 MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	XC		2,ALT							;* if P1800-Pbb<THR ...
	 ST		#0,LOS_counter					;* if Psum<THR, LOS_counter=0
	LD		LOS_counter,B
	SUB		#S_DETECT_THR,B					;* LOS_counter-THR
	BC_		Rx_state_return,BLT				;* return if counter<THR

	CALL_	Rx_init_v32A
	ST		#RETRAIN_REQUEST_THRESHOLD,Dcounter   
	ST		#RETRAIN,Rx_status
 .if TX_V32A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return

;****************************************************************************
;* Rx_v32A_train_loops
;****************************************************************************

Rx_v32A_train_loops:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_train_loops
	BCD_	Rx_state_return,AEQ				;* return if A=0
	 SUB	#2,A

	;**** check for timeout ****

	LD		Rx_symbol_counter,B
	BCD_	Aloops_stable_detected,AEQ		;* branch if return=2
	 SUB	#TRAIN_LOOPS_TIMEOUT,B			;* Rx_symbol_counter-TRAIN_LOOPS_TIMEOUT
	BC_		Rx_state_return,BLEQ			;* branch if symbol_counter<=TIMEOUT
	LD		#0,A
	STL		A,Rx_pattern_detect				;* pattern_detect=NOT_DETECTED
	STL		A,Rx_pattern_reg				;* pattern_reg=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#TRAIN_LOOPS_FAILURE,Rx_status	;* set status to FAILURE
	STPP	#Rx_v32A_S_detect,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_S_DETECT_ID,Rx_state_ID;* state_ID=RX_V32_START_DETECT_ID

Aloops_stable_detected:
	ST		#0,Dcounter
	STPP	#Rx_v32A_detect_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_DETECT_EQ_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_detect_EQ
;****************************************************************************

Rx_v32A_detect_EQ:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_detect_EQ
	LD		Rx_symbol_counter,B
	BCD_	Rx_state_return,AEQ				;* return if A=0

	;**** check for timeout ****

	 SUB	#DETECT_EQ_TIMEOUT,B			;* Rx_symbol_counter-DETECT_EQ_TIMEOUT
	BCD_	Astart_EQ_timeout_endif,BLEQ	;* branch if symbol_counter<=TIMEOUT
	 LD		Rx_pattern_reg,2,B				;* B=pattern_reg<<2
	 LD		#0,A
	STL		A,Rx_pattern_detect				;* pattern_detect=NOT_DETECTED
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#DETECT_EQ_FAILURE,Rx_status
	STPP	#Rx_v32A_S_detect,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_S_DETECT_ID,Rx_state_ID		
Astart_EQ_timeout_endif:

	;**** find SBAR transition ****

	BITF	Rx_pattern_detect,#SBAR_DETECTED
	BCD_	Adetect_EQ_endif2,TC			;* branch if SBAR_DETECTED
	 OR		Phat,B							;* B=(pattern_reg<<2)|Phat
	 STL	B,Rx_pattern_reg
	LD		temp1,A
	BC_		Rx_state_return,ALEQ			;* return if autocorrelation<=0
	ST		#0,Rx_symbol_counter
	BD_		Rx_state_return
	 ORM	 #SBAR_DETECTED,Rx_pattern_detect
Adetect_EQ_endif2:

	;**** verify CD pattern ****

	LD		Rx_pattern_reg,-4,A				;* A=pattern_reg>>4
	XOR		A,B								;* B=((Rx->pattern_reg>>4)^Rx->pattern_reg)
	AND		#3ffh,B							;* B=((Rx->pattern_reg>>4)^Rx->pattern_reg)&0x3ff
	 LD		Rx_symbol_counter,A
	 SUB	#(TX_SBAR_LEN-1),A
	XC		2,BEQ							;* if B=0 ... 
	 ORM	#TRN_DETECTED,Rx_pattern_detect  
	BC_		Rx_state_return,ANEQ			;* return if counter!=REV_CORR_LEN
	BITF	Rx_pattern_detect,#TRN_DETECTED
	BC_		Adetect_EQ_endif3,NTC			;* branch if !TRN_DETECTED
	LD		#0,A
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg=0
	ST		#TRK_TIMING_THR,timing_threshold
	STL		A,Rx_sym_clk_memory				;* Rx_sym_clk_memory=0
	STL		A,agc_K							;* agc_K=0
	ST		#ACQ_EQ_2MU,EQ_2mu			
	ST		#V32_TRK_LOOP_K1,loop_K1		
	ST		#V32_TRK_LOOP_K2,loop_K2		
	STL		A,Rx_pattern_reg				;* Rx_pattern_reg=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#v32A_EQslicer48,slicer_ptr,B
	STPP	#Rx_v32A_train_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_TRAIN_EQ_ID,Rx_state_ID
Adetect_EQ_endif3:

	;**** CD detect failure ****

	LD		#0,A
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	ST		 #NOT_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32A_S_detect,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32A_S_DETECT_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_train_EQ
;****************************************************************************

Rx_v32A_train_EQ:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_train_EQ
	SUB		#1,A
	BC_		Rx_state_return,ANEQ				;* return if Rx_v32_train_EQ()!=1
	STPP	#Rx_v32A_rate,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_RATE_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_rate
;****************************************************************************

Rx_v32A_rate:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_rate
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_rate()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Iprime,A
;	CALL_	dump_write
;	LD		Qprime,A
;	CALL_	dump_write
;	LD		Rx_state_ID,A
;	CALL_	dump_write

	LD		Iprime,B
	SUB		Inm2,B							;* B=Iprime-Inm2
	ABS		B
	LD		Qprime,A
	SUB		Qnm2,A							;* A=Qprime-Qnm2
	ABS		A
	ADD		B,A								;* A=abs(Iprime-Inm2)+abs(Qprime-Qnm2)
	CALL_	Rx_v32_RC_detector

	;**** check for loss of lock ****

	CMPM	LOS_monitor,#UNLOCKED
	BC_		A_rate_endif0,NTC				;* branch if !UNLOCKED
	CALL_	Rx_init_v32A
	ST		#LOSS_OF_LOCK,Rx_status
 .if TX_V32A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
A_rate_endif0:

	;**** check for retrain request ****

A_check_retrain_request:	
	 BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	BC_		A_rate_endif1,NTC				;* branch if !RC_PREAMBLE_DETECTED
	 LD		RCcounter,A
	SUB		#RETRAIN_REQUEST_THRESHOLD,A
	BC_		A_rate_endif1,ALT				;* branch if A<RETRAIN_REQUEST_THRESHOLD
	CALL_	Rx_init_v32A
	ST		#RETRAIN_REQUEST_THRESHOLD,Dcounter
	ST		#RETRAIN,Rx_status
 .if TX_V32A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
A_rate_endif1:

	;**** check for renegotiate rate signal ****

A_check_renegotiate_request:	
	BITF	Rx_pattern_detect,#RATE_SIGNAL_DETECTED
	BC_		A_rate_endif2,NTC				;* branch if !RATE_SIGNAL_DETECTED
	LD		Rx_state_ID,A
	SUB		#RX_V32A_RC_PREAMBLE_ID,A
	BC_		A_rate_endif2,ANEQ				;* branch if !RX_V32A_RC_PREAMBLE_ID
	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	ST		#RENEGOTIATE,Rx_status
	ST		#RX_V32A_R5_ID,Rx_state_ID
	CMPM	*+AR1(Tx_state_ID),#TX_V32A_MESSAGE_ID
	BC_		A_rate_endif2,NTC				;* branch if !TX_V32A_MESSAGE_ID
	ST		#RX_V32A_R4_ID,Rx_state_ID
 .if TX_V32A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_renegotiate
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
A_rate_endif2:

	;**** check for signal E ****

A_check_E_detect:
	BITF	Rx_pattern_detect,#SIGNAL_E_DETECTED
	BC_		Rx_state_return,NTC				;* return if !SIGNAL_E
	LD		#-(RX_RC_B1_LEN-RX_B1_LEN),B
	CMPM	Rx_state_ID,#RX_V32A_RATE_ID
	 LD		#-MODEM_TURNAROUND_DELAY,A
	 SUB	Rx_RTD,A
	XC		1,TC							;* if Rx_state=RX_V32A_RATE_ID
	 LD		A,B								;* ... B=-(MODEM_TURNAROUND_DELAY+path_delay)
	ADD		Rx_symbol_counter,B
	STL		B,Rx_symbol_counter				;* update symbol_counter
	STPP	#Rx_v32A_B1,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_B1_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32A_B1
;****************************************************************************

Rx_v32A_B1:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_B1
	SUB		#1,A
	BC_		Rx_state_return,ANEQ			;* return if Rx_v32_B1()!=1
	STPP	#Rx_v32A_message,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_MESSAGE_ID,Rx_state_ID
	
;****************************************************************************
;* Rx_v32A_message
;****************************************************************************

Rx_v32A_message:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_message
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_message()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Iprime,A
;	CALL_	dump_write
;	LD		Qprime,A
;	CALL_	dump_write

	;**** check for loss of lock ****

	CMPM	LOS_monitor,#UNLOCKED
	BC_		Rx_v32A_message_endif1,NTC		;* branch if !UNLOCKED
	CALL_	Rx_init_v32A
	ST		#LOSS_OF_LOCK,Rx_status
 .if TX_V32A_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32A_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
Rx_v32A_message_endif1:

	;**** check for RC preamble ****

	LD		Iprime,B
	SUB		Inm2,B							;* B=Iprime-Inm2
	ABS		B
	LD		Qprime,A
	SUB		Qnm2,A							;* A=Qprime-Qnm2
	ABS		A
	ADD		B,A								;* A=abs(Iprime-Inm2)+abs(Qprime-Qnm2)
	CALL_	Rx_v32_RC_detector
	BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	BC_		Rx_state_return,NTC				;* return if !RC_PREAMBLE_DETECTED	
	STPP	#v32A_descrambler,Rx_descrambler_ptr,B
	STPP	#Rx_v32A_rate,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32A_RC_PREAMBLE_ID,Rx_state_ID

;****************************************************************************
 .endif

	;************************************
	;**** CALL side receiver modules ****
	;************************************

 .if RX_V32C_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v32C:
;* Initializes Rx_block[] workspace for v32 CALL operation.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_v32C:					
	CALL_	Rx_init_v32						;* call general init
	STPP	#v32C_descrambler,Rx_descrambler_ptr,B
	ST		#NOT_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32C_detect_AC,Rx_state,B
	RETD_
	 ST		#RX_V32C_DETECT_AC_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_detect_AC: Detectors for AC, ANS, USB1, and rejects v32 	
;* modulated signals. Upon sucessful detection of AC, the sum of the 600 Hz	
;* and 3000 Hz components, Psum, is stored to Rx_agc_gain, and the state	
;* switches to Rx_v32C_detect_ACCA.  The ANS detector reports the presence 	
;* of ANS in Rx_pattern_detect, and upon detection, resets 				
;* Tx_symbol_counter to zero to ensure that the start of AA generation is 	
;* 1.0 sec after ANS.  Rx_symbol_counter is also reset upon ANS detection,	
;* and when ANS is un-detected, Rx_symbol_counter is copied to Rx_Inm1	
;* which is the duration of ANS measured at the 2400 Hz sample rate.		
;****************************************************************************

Rx_v32C_detect_AC:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** check for v22 USB1 presence ****

	MVMM	AR2,AR7							;* AR7=Rx_sample_tail
	MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len
	SSBX	TC								;* enable filter
	BPF		FILTER_2250_COEF,BPF_150HZ_BW,AR7
	STH		A,temp1				
	MPY		temp1,#24576,A
	STH		A,1,temp1						;* temp1=1.5*F2250
	LD		temp1,A
	SUB		Rx_threshold,A,B				;* compare with min level threshold
	 ADDM	#1,LOS_counter					;* LOS_counter++
	XC		2,BLT							;* if F2250-THR<0 ...
	 ST		#0,LOS_counter					;* ... LOS_counter=0
	ADD		What,A
	STL		A,temp1							;* temp1=F1800+F2250
	MPY		temp1,THR_2DB,A			
	STH		A,temp1							;* temp1=(F1800+F2250)*THR
	RATIO	temp1,agc_K,THR_3DB				;* Psum/Pbb ratio
	 NOP	
	 NOP	
	XC		2,BLT
	 ST		#0,LOS_counter					;* ... LOS_counter=0
	LD		LOS_counter,A
	SUB		#USB1_DETECT_LEN,A				;* LOS_counter-USB1_DETECT_LEN
	BC_		USB1_detector_endif,ALEQ		;* branch if LOS_counter<=USB1_DETECT_LEN
	ST		#V22_USB1_DETECTED,Rx_status
USB1_detector_endif:

	;**** check for ANS ****

	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1		;* AR1=&Tx_block
	MVMM	AR2,AR7							;* AR7=Rx_sample_tail
	MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len
	SSBX	TC								;* enable filter
	BPF		FILTER_2100_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,temp0							;* temp0=F2100
	BITF	Rx_pattern_detect,#ANS_DETECTED
	BC_		ANS_detector_endif,TC			;* branch if ANS_DETECTED
	SUB		Rx_threshold,16,A,B				;* compare with min level threshold
	 ADDM	#1,RCcounter					;* RCcounter++
	XC		2,BLT							;* if F2100-THR<0 ...
	 ST		#0,RCcounter					;* ... RCcounter=0
	RATIO	temp0,agc_K,THR_1DB				;* F2100/Pbb ratio
	 NOP
	 NOP
	XC		2,BLT
	 ST		#0,RCcounter
	LD		RCcounter,A
	SUB		#ANS_DETECT_LEN,A				;* RCcounter-ANS_DETECT_LEN
	BC_		ANS_detector_endif,ALEQ			;* branch if counter<=LEN
	ST		#V32_ANS_DETECTED,Rx_status
	LD		temp0,B							;* B=F2100
	STL		B,agc_gain						;* agc_gain=F2100
	LD		#0,A
	STL		A,RCcounter
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ORM		#ANS_DETECTED,Rx_pattern_detect
	STL		A,*AR1(Tx_symbol_counter)		;* Tx_symbol_counter=0
	ST		#TXC_SILENCE1_LEN,*AR1(Tx_terminal_count)
ANS_detector_endif:

	BITF	Rx_pattern_detect,#ANS_DETECTED
	BC_		ANS_detector_endif2,NTC			;* branch if !ANS_DETECTED
	LD		temp0,16,A						;* A=F2100
	MAC		agc_gain,#-THR_6DB,A			;* A=F2100-agc_gain*THR
	 ADDM	#1,RCcounter					;* RCcounter++
	XC		2,AGT							;* if F2100-agc_gain*THR>0 ...
	 ST		#0,RCcounter					;* ... RCcounter=0
	LD		RCcounter,B						;* B=RCcounter
	SUB		#ANS_DETECT_LEN,B
	BC_		ANS_detector_endif2,BLEQ		;* branch if counter<=LEN
	LD		Rx_symbol_counter,B
	STL		B,Inm1							;* Inm1=length of ANS
	ST		#STATUS_OK,Rx_status
	ST		#0,RCcounter
	ANDM	#~ANS_DETECTED,Rx_pattern_detect
ANS_detector_endif2:

	;**** detect energy in 600 Hz and 3000 Hz filters ****

	MPY		Ihat,#ONE_BY_ROOT3,B			;* B=P600/sqrt(3)
	MAC		What,#ONE_BY_ROOT3,B			;* B+=P1800/sqrt(3)
	MAC		Qhat,#ONE_BY_ROOT3,B			;* B+=P3000/sqrt(3)
	STH		B,temp1							;* temp1=Psum
	RATIO	temp1,agc_K,THR_6DB				;* Psum/Pbb ratio
	BCD_	Rx_state_return,BLT				;* return if Psum*THR-Pbb<0

	;**** reject modulated signal ****

	 LD		Ihat,B
	 ADD	Qhat,B				
	STL		B,temp1							;* temp1=Psum
	MPY		temp1,#THR_14DB,A
	SUB		temp0,16,A						;* A=Psum*THR-F2100
	 ADDM	#1,Dcounter
	XC		2,ALT							;* if Psum*THR-F2100<0 ...
	 ST		#0,Dcounter						;* ... Dcounter=0

	;**** detect energy in 600 and 3000 Hz filters ****

	SUB		Rx_threshold,B,A				;* A=Psum-THR
	 STL	B,temp0							;* temp0=Psum
	 SQUR	temp0,B							;* B=Psum^2, T=Psum
	XC		2,ALT							;* if Psum-THR<0 ...
	 ST		#0,Dcounter						;* ... Dcounter=0
	LD		Dcounter,A
	SUB		#RX_AC_DETECT_LEN,A
	BC_		Rx_state_return,ALT				;* return if Dcounter<THR
	ST		T,agc_gain						;* agc_gain=Psum
	STH		B,1,Rx_power					;* Rx_power=(Psum^2)/2
	LD		 #0,A
	STL		A,*AR1(Tx_terminal_count)
	STL		A,RCcounter
	STL		A,Dcounter
	STL		A,Rx_symbol_counter
	ORM		#AC_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32C_detect_ACCA,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_DETECT_ACCA_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_detect_ACCA
;****************************************************************************

Rx_v32C_detect_ACCA:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,A
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	;**** sequencer ****

	BITF	Rx_pattern_detect,#ACCA_LOW_DETECTED
	BCD_	AC_detect_else1,TC				;* branch if ACCA_LOW_DETECTED
	 LD		Ihat,16,B
	 ADD	Qhat,16,B						;* B=Psum
	MAC		agc_gain,#(-THR_6DB),B,A		;* B=Psum-agc_gain*THR_6DB
	BC_		Rx_state_return,AGT
	ST		#32767,LOS_counter
	BD_		Rx_state_return
	 ORM	 #ACCA_LOW_DETECTED,Rx_pattern_detect
AC_detect_else1:					
			SUB	LOS_counter,16,B,A			;* A=Psum-LOS_counter
	BCD_	AC_detect_else2,AGEQ			;* branch if Psum>=LOS_counter
	 MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	STH		B,LOS_counter					;* LOS_counter=Psum
	MVKD	AR2,Rx_sample_tail
	LDU		Rx_sample_stop,B
	SUBS	Rx_sample_tail,B
	LDU		*AR1(Tx_num_samples),A		
	ADDS	Rx_sample_stop,A
	SUBS	Rx_sample_tail,A				;* A=num_samples+sample_stop-sample_tail
	XC		1,BLT							;* if sample_stop-sample_tail<0 ...
	 ADD	Rx_sample_len,A					;* ... A+=Rx_sample_len
	ADD		*AR1(Tx_system_delay),A			;* A+=system_delay
	STLM	A,T								;* T=k
	MPY		#NUM_SAMPLES_COEF,A				;* B=k*NUM_SAMPLES_COEF
	SUB		#(64-PATH_FILTER_DELAY-TX_FIR_TAPS/2 -1),16,A
	STH		A,*AR1(Tx_symbol_counter)	
AC_detect_else2:					
	ADDM	#1,Dcounter						;* Dcounter++
	LD		Dcounter,B
	SUB		#PATH_WINDOW,B
	BC_		Rx_state_return,BLT				;* return if Dcounter<PATH_WINDOW
	ST		#0,Dcounter
	ORM		#ACCA_HIGH_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32C_detect_CAAC,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32C_DETECT_CAAC_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_detect_CAAC
;****************************************************************************

Rx_v32C_detect_CAAC:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,A
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	;**** look for CAAC reversal ****

	BITF	Rx_pattern_detect,#CAAC_LOW_DETECTED
	BCD_	CAAC_detect_else1,TC			;* branch if !CAAC_LOW_DETECTED
	 LD		Ihat,16,B
	 ADD	Qhat,16,B						;* B=Psum
	MAC		agc_gain,#(-THR_6DB),B,A		;* A=Psum-agc_gain*THR_6DB
	BC_		Rx_state_return,AGT				;* return if Psum<THR
	ST		#0,Dcounter
	ST		#32767,LOS_counter
	BD_		Rx_state_return			
	 ORM	#CAAC_LOW_DETECTED,Rx_pattern_detect

CAAC_detect_else1:				
	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	SUB		LOS_counter,16,B,A				;* Psum-LOS_counter
	BCD_	CAAC_detect_else2,AGEQ			;* branch if Psum>=LOS_counter
	 STM	#NUM_SAMPLES_COEF,T
	LD		Rx_symbol_counter,16,A
	STH		A,Rx_RTD
	STH		B,LOS_counter					;* LOS_counter=Psum
CAAC_detect_else2:				
	ADDM	#1,Dcounter						;* Dcounter++
	LD		Dcounter,B
	SUB		#PATH_WINDOW,B
	BC_		Rx_state_return,BLT				;* return if Dcounter<PATH_WINDOW

	LD		Rx_RTD,16,A				
	MAS		*AR1(Tx_sample_len),A			;* A=RTD-Tx_sample_len*COEF
	MAC		*AR1(Tx_num_samples),A			;* A+=Tx_num_samples*COEF
	MAC		#V32_FEC_LEN,A					;* A=V32_FEC_LEN*COEF
	BC_		Rx_v32C_detect_CAAC_endif,ALEQ
	ST		#0,Rx_RTD
	ST		#EXCESSIVE_RTD_STATUS,Rx_status
Rx_v32C_detect_CAAC_endif:
	ST		#0,Dcounter
	STPP	#Rx_v32C_detect_AC_end,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32C_DETECT_AC_END_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_detect_AC_end
;****************************************************************************

Rx_v32C_detect_AC_end:
	CALL_	Rx_v32_filters
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_filters()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,A
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write

	;**** look for drop in filtered signal energy ****

	LD		Ihat,16,B
	ADD		Qhat,16,B						;* B=Psum
	MAC		agc_gain,#(-THR_6DB),B,A		;* A=Psum-agc_gain*THR_6DB
	BC_		Rx_state_return,AGT				;* return if What>agc_gain*THR
	LD		#0,A
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_pattern_reg				;* pattern_reg=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#WHAT_13,What
	ORM		#AC_END_DETECTED,Rx_pattern_detect
	MVKD	AR2,EC_sample_ptr				;* EC_sample_ptr=Rx_sample_tail
	STPP	#Rx_v32C_S_detect,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32C_S_DETECT_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_train_EC
;****************************************************************************

Rx_v32C_train_EC:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller
 .else
	MVDK	Rx_start_ptrs,AR1				;* AR6=Rx_start_ptrs
	MVDK	*AR1(Tx_block_start),AR1		;* AR1=&Tx_block_start
 .endif

	;**** dump_write debugging facility - enable to view signals ****

;	LD		*AR2,A
;	CALL_	dump_write		

	CMPM	*AR1(Tx_state_ID),#TX_V32C_R2_ID
	BC_		Rx_v32C_train_EC_endif,NTC		;* return if Tx_state_ID!=RATE1
	LD		#0,A
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Rx_fir_start),AR1
	MVKD	AR1,Rx_fir_ptr			
	STM		#(RX_FIR_LEN-1),BRC
	RPTB	v32C_init_Rx_fir_loop
v32C_init_Rx_fir_loop:
	 STL	A,*AR1+							;* Rx_fir[*++]=0

	MVDK	*AR0(EQ_coef_start),AR1
	STM		#(2*V32_EQ_LEN-1),BRC
	RPTB	v32C_init_EQ_coef_loop
v32C_init_EQ_coef_loop:
	 STL	A,*AR1+							;* EQ_coef[*++]=0

	ST		#(V32_EQ_LEN/2),Rx_baud_counter	
	STL		A,loop_memory					;* loop_memory=0
	STL		A,loop_memory_low				;* loop_memory=0
	STL		A,Rx_sym_clk_memory				;* Rx_sym_clk_memory=0
	STL		A,coarse_error					;* coarse_error=0
	STL		A,vco_memory					;* vco_memory=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,LOS_monitor					;* LOS_monitor=0
	STPP	#sgn_timing,timing_ptr,B
	ST		#EQ_DISABLED,EQ_2mu			

	ST		#EC_TRAIN_DISABLED,EC_2mu
	STL		A,Rx_rate_pattern				;* rate_pattern=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* sample_counter=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	STPP	#Rx_v32C_S_detect,Rx_state,B
	ST		#RX_V32C_S_DETECT_ID,Rx_state_ID
Rx_v32C_train_EC_endif:

	MVDK	Rx_sample_stop,AR0
	CMPR	0,AR2							;* sample_tail-sample_stop
	BC_		Rx_state_return,TC				;* return if tail=stop	
	MAR		*AR2+%							;* AR2=Rx_sample_tail++%
	BD_		Rx_state_return			
	 ADDM	#1,Rx_sample_counter	

;****************************************************************************
;* Rx_v32C_S_detect
;****************************************************************************

Rx_v32C_S_detect:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_S_detect
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_S_detect()=0

	;**** dump_write debugging facility - enable to view signals ****

;	LD		Ihat,A
;	ADD		Qhat,A
;	CALL_	dump_write
;	LD		What,A
;	CALL_	dump_write
	;**** dump_write debugging facility - enable to view signals ****

	BITF	Rx_pattern_detect,#S_DETECTED
	BC_		C_S_detect_endif1,NTC			;* branch if !S_DETECTED
	STPP	#Rx_v32C_train_loops,Rx_state,B
	ST		#RX_V32C_TRAIN_LOOPS_ID,Rx_state_ID
C_S_detect_endif1:

	;**** look for AC pattern ****

	MPY		Ihat,#ONE_BY_ROOT2,B			;* B=P600/sqrt(2)
	MAC		Qhat,#ONE_BY_ROOT2,B			;* B+=P3000/sqrt(2)
	STH		B,temp0							;* temp0=Psum
	SUB		Rx_threshold,16,B,A				;* Psum-THR_43DB_RMS
	 ADDM	#1,LOS_counter					;* LOS_counter++
	XC		2,ALT
	 ST		#0,LOS_counter					;* if Psum<THR, LOS_counter=0
	MPY		Ihat,#THR_10DB,A				;* A=P600*THR
	MAC		Qhat,#THR_10DB,A				;* A=(P600+P3000)*THR
	SUB		What,16,A						;* A-=P1800
	 NOP
	 NOP
	XC		2,ALT							;* if (F600+F3000)*THR<F1800 ...
	 ST		#0,LOS_counter					;* ... LOS_counter=0
	LD		LOS_counter,B
	SUB		#S_DETECT_THR,B					;* LOS_counter-THR
	BC_		Rx_state_return,BLT				;* return if counter<THR
 
	CALL_	Rx_init_v32C
	ST		#RETRAIN_REQUEST_THRESHOLD,Dcounter   
	ST		#RETRAIN,Rx_status
 .if TX_V32C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return

;****************************************************************************
;* Rx_v32C_train_loops
;****************************************************************************

Rx_v32C_train_loops:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_train_loops
	BCD_	Rx_state_return,AEQ				;* return if A=0
	 SUB	#2,A

	;**** check for timeout ****

	LD		Rx_symbol_counter,B
	BCD_	Cloops_stable_detected,AEQ		;* branch if return=2
	 SUB	#TRAIN_LOOPS_TIMEOUT,B			;* Rx_symbol_counter-TRAIN_LOOPS_TIMEOUT
	BC_		Rx_state_return,BLEQ			;* branch if symbol_counter<=TIMEOUT
	LD		#0,A
	STL		A,Rx_pattern_detect				;* pattern_detect=NOT_DETECTED
	STL		A,Rx_pattern_reg				;* pattern_reg=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#TRAIN_LOOPS_FAILURE,Rx_status	;* set status to FAILURE
	STPP	#Rx_v32C_S_detect,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_S_DETECT_ID,Rx_state_ID;* state_ID=RX_V32_START_DETECT_ID

Cloops_stable_detected:
	ST		#0,Dcounter
	STPP	#Rx_v32C_detect_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_DETECT_EQ_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_detect_EQ
;****************************************************************************

Rx_v32C_detect_EQ:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_detect_EQ
	LD		Rx_symbol_counter,B
	BCD_	Rx_state_return,AEQ				;* return if A=0

	;**** check for timeout ****

	 SUB	#DETECT_EQ_TIMEOUT,B			;* Rx_symbol_counter-DETECT_EQ_TIMEOUT
	BCD_	Cstart_EQ_timeout_endif,BLEQ	;* branch if symbol_counter<=TIMEOUT
	 LD		Rx_pattern_reg,2,B				;* B=pattern_reg<<2
	 LD		#0,A
	STL		A,Rx_pattern_detect				;* pattern_detect=NOT_DETECTED
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	ST		#DETECT_EQ_FAILURE,Rx_status
	STPP	#Rx_v32C_S_detect,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_S_DETECT_ID,Rx_state_ID;* state_ID=RX_V32_START_DETECT_ID
Cstart_EQ_timeout_endif:

	;**** find SBAR transition ****

	BITF	Rx_pattern_detect,#SBAR_DETECTED
	BCD_	Cdetect_EQ_endif2,TC			;* branch if SBAR_DETECTED
	 OR		Phat,B							;* B=(pattern_reg<<2)|Phat
	 STL	B,Rx_pattern_reg
	LD		temp1,A
	BC_		Rx_state_return,ALEQ			;* return if autocorrelation<=0
	ST		#0,Rx_symbol_counter
	BD_		Rx_state_return
	 ORM	 #SBAR_DETECTED,Rx_pattern_detect
Cdetect_EQ_endif2:

	;**** verify CD pattern ****

	LD		Rx_pattern_reg,-4,A				;* A=pattern_reg>>4
	XOR		A,B								;* B=((Rx->pattern_reg>>4)^Rx->pattern_reg)
	AND		#3ffh,B							;* B=((Rx->pattern_reg>>4)^Rx->pattern_reg)&0x3ff
	   LD	Rx_symbol_counter,A
	 SUB	#(TX_SBAR_LEN-1),A
	XC		2,BEQ							;* if B=0 ... 
	 ORM	#TRN_DETECTED,Rx_pattern_detect  
	BC_		Rx_state_return,ANEQ			;* return if counter!=REV_CORR_LEN
	BITF	Rx_pattern_detect,#TRN_DETECTED
	BC_		Cdetect_EQ_endif3,NTC			;* branch if !TRN_DETECTED
	LD		#0,A
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg=0
	ST		#TRK_TIMING_THR,timing_threshold
	STL		A,Rx_sym_clk_memory				;* Rx_sym_clk_memory=0
	STL		A,agc_K							;* agc_K=0
	ST		#ACQ_EQ_2MU,EQ_2mu			
	ST		#V32_TRK_LOOP_K1,loop_K1		
	ST		#V32_TRK_LOOP_K2,loop_K2		
	STL		A,Rx_pattern_reg				;* Rx_pattern_reg=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#v32C_EQslicer48,slicer_ptr,B
	STPP	#Rx_v32C_train_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_TRAIN_EQ_ID,Rx_state_ID

	;**** CD detect failure ****

Cdetect_EQ_endif3:
	LD		#0,A
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	ST		 #NOT_DETECTED,Rx_pattern_detect
	STPP	#Rx_v32C_S_detect,Rx_state,B
	BD_		Rx_state_return			
	 ST		#RX_V32C_S_DETECT_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_train_EQ
;****************************************************************************

Rx_v32C_train_EQ:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_train_EQ
	SUB		#1,A
	BC_		Rx_state_return,ANEQ				;* return if Rx_v32_train_EQ()!=1
	STPP	#Rx_v32C_rate,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_RATE_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_rate
;****************************************************************************

Rx_v32C_rate:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_rate
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_rate()=0

	LD		Iprime,B
	ADD		Inm2,B							;* B=Iprime+Inm2
	ABS		B
	LD		Qprime,A
	ADD		Qnm2,A							;* A=Qprime+Qnm2
	ABS		A
	ADD		B,A								;* A=abs(Iprime+Inm2)+abs(Qprime+Qnm2)
	CALL_	Rx_v32_RC_detector

	;**** check for loss of lock ****

	CMPM	LOS_monitor,#UNLOCKED
	BC_		C_rate_endif0,NTC				;* branch if !UNLOCKED
	CALL_	Rx_init_v32C
	ST		#LOSS_OF_LOCK,Rx_status
 .if TX_V32C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
C_rate_endif0:

	;**** check for retrain request ****

C_check_retrain_request:
	 BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	BC_		C_rate_endif1,NTC				;* branch if !RC_PREAMBLE_DETECTED
	 LD		RCcounter,A
	SUB		#RETRAIN_REQUEST_THRESHOLD,A
	BC_		C_rate_endif1,ALT				;* branch if A<RETRAIN_REQUEST_THRESHOLD
	CALL_	Rx_init_v32C
	ST		#RETRAIN_REQUEST_THRESHOLD,Dcounter
	ST		#RETRAIN,Rx_status
 .if TX_V32C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
C_rate_endif1:

	;**** check for renegotiate rate signal ****

C_check_renegotiate_request:
	BITF	Rx_pattern_detect,#RATE_SIGNAL_DETECTED
	BC_		C_rate_endif2,NTC				;* branch if !RATE_SIGNAL_DETECTED
	LD		Rx_state_ID,A
	SUB		#RX_V32C_RC_PREAMBLE_ID,A
	BC_		C_rate_endif2,ANEQ				;* branch if !RX_V32C_RC_PREAMBLE_ID
	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	ST		#RENEGOTIATE,Rx_status
	ST		#RX_V32C_R5_ID,Rx_state_ID
	CMPM	*+AR1(Tx_state_ID),#TX_V32C_MESSAGE_ID
	BC_		C_rate_endif2,NTC				;* branch if !TX_V32C_MESSAGE_ID
	ST		#RX_V32C_R4_ID,Rx_state_ID
 .if TX_V32C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_renegotiate
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
C_rate_endif2:

	;**** look for transmitter to acknowlege RATE_SIGNAL_DETECTED ****

C_check_RATE_ACK:
	LD		Rx_pattern_detect,A
	AND		#(RATE_SIGNAL_DETECTED|RATE_SIGNAL_ACKNOWLEDGED),A
	SUB		#(RATE_SIGNAL_DETECTED|RATE_SIGNAL_ACKNOWLEDGED),A
	BC_		C_rate_endif3,ANEQ				;* branch if !ACKNOWLEDGED
	ANDM	#~RATE_SIGNAL_DETECTED,Rx_pattern_detect
	 LD		#0,A
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STPP	#Rx_v32C_train_EC,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_TRAIN_EC_ID,Rx_state_ID
C_rate_endif3:

	;**** check for signal E ****

C_check_E_detect:
	BITF	Rx_pattern_detect,#SIGNAL_E_DETECTED
	BC_		Rx_state_return,NTC				;* return if !SIGNAL_E
	MVDK	Rx_start_ptrs,AR1
	MVDK	*AR1(Tx_block_start),AR1
	BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	BC_		C_rate_else4,TC					;* branch if !RC_PREAMBLE
	ST		#0,*AR1(Tx_symbol_counter)
	ST		#TX_B1_LEN,*AR1(Tx_terminal_count)
	B_		C_rate_endif4
C_rate_else4:
	ADDM	#(RX_B1_LEN-RX_RC_B1_LEN),Rx_symbol_counter
C_rate_endif4:

 .if V32BIS_MODEM=ENABLED
	LD		Rx_rate,B
	SUB		#V32_RATE_14400,B
	BC_		C_rate_endif5,BNEQ				;* if rate=14400 ...
	STPP	#v32C_descrambler6,Rx_descrambler_ptr,B
C_rate_endif5:
 .endif
	STPP	#Rx_v32C_B1,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_B1_ID,Rx_state_ID

;****************************************************************************
;* Rx_v32C_B1
;****************************************************************************

Rx_v32C_B1:	
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_B1
	SUB		#1,A
	BC_		Rx_state_return,ANEQ			;* return if Rx_v32_B1()!=1
	STPP	#Rx_v32C_message,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_MESSAGE_ID,Rx_state_ID
	
;****************************************************************************
;* Rx_v32C_message
;****************************************************************************

Rx_v32C_message:
 .if ECHO_CANCELLER=ENABLED
	CALL_	echo_canceller		
 .endif
	CALL_	Rx_v32_message
	BC_		Rx_state_return,AEQ				;* return if Rx_v32_message()=0

	;**** check for loss of lock ****

	CMPM	LOS_monitor,#UNLOCKED
	BC_		Rx_v32C_message_endif1,NTC		;* branch if !UNLOCKED
	CALL_	Rx_init_v32C
	ST		#LOSS_OF_LOCK,Rx_status
 .if TX_V32C_MODEM=ENABLED
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_v32C_retrain
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	LD		*AR1(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
 .endif
	B_		Rx_state_return
Rx_v32C_message_endif1:

	LD		Iprime,B
	ADD		Inm2,B							;* B=Iprime+Inm2
	ABS		B
	LD		Qprime,A
	ADD		Qnm2,A							;* A=Qprime+Qnm2
	ABS		A
	ADD		B,A								;* A=abs(Iprime+Inm2)+abs(Qprime+Qnm2)
	CALL_	Rx_v32_RC_detector
	BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	BC_		Rx_state_return,NTC				;* return if !RC_PREAMBLE_DETECTED	
	STPP	#v32C_descrambler,Rx_descrambler_ptr,B
	STPP	#Rx_v32C_rate,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V32C_RC_PREAMBLE_ID,Rx_state_ID

;****************************************************************************
 .endif 

	;*********************************
	;**** common receiver modules ****
	;*********************************

 .if RX_V32_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v32: Initializes Rx_block[] workspace for v32 operation
;* On entry it expects:
;*	AR0=&Rx_block
;****************************************************************************

Rx_init_v32:
	LD		#0,A
	ST		#_Rx_timing2400,Rx_timing_start	
	ST		#_RCOS2400_f1800,Rx_coef_start	
	ST		#RX_DEC,Rx_decimate
	ST		#(2*RX_INTERP),Rx_interpolate
	ST		#(RX_DEC/2),Rx_sym_clk_phase	
	ST		#RX_OVERSAMPLE,Rx_oversample
	STL		A,Rx_pattern_reg				;* pattern_reg=0
	STL		A,data_Q1						;* data_Q1=0
	ST		#2,Rx_Nbits		
	ST		#3,Rx_Nmask		
	STL		A,Rx_map_shift					;* map_shift=0
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg=0
	STPP	#v32_diff_decoder,decoder_ptr,B
	STPP	#v32_slicer48,slicer_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	MVDK	Rx_start_ptrs,AR0			
	LD		Rx_data_head,B
	STL		B,Rx_data_tail
	STL		A,Rx_coef_ptr					;* Rx_coef_ptr=0
	LD		Rx_sample_tail,B
	STL		B,Rx_sample_ptr					;* sample_ptr=Rx_sample_tail

	ST		#(RX_FIR_TAPS-1),Rx_fir_taps		
	STL		A,EQ_MSE						;* EQ_MSE=0
	ST		#EQ_DISABLED,EQ_2mu			
	ST		#(V32_EQ_LEN-1),EQ_taps
	MVDK	*AR0(Rx_fir_start),AR3
	MVKD	AR3,Rx_fir_ptr			
	STM		#(RX_FIR_LEN-1),BRC
	RPTB	v32_init_Rx_fir_loop
v32_init_Rx_fir_loop:
	 STL	A,*AR3+							;* Rx_fir[*++]=0

	MVDK	*AR0(EQ_coef_start),AR3
	STM		#(2*V32_EQ_LEN-1),BRC
	RPTB	v32_init_EQ_coef_loop
v32_init_EQ_coef_loop:
	 STL	A,*AR3+							;* EQ_coef[*++]=0

	ST		#ACQ_AGC_K,agc_K			
	ST		#V32_ACQ_LOOP_K1,loop_K1			
	ST		#V32_ACQ_LOOP_K2,loop_K2			

	;**** initialize PJ coefficients only if non-TCM (temporary) ****

	MVDK	Rx_start_ptrs,AR3
	MVDK	*AR3(Tx_block_start),AR3
	STL		A,PJ1_coef						;* disable PJ1 resonator
	STL		A,PJ2_coef                      ;* disable PJ2 resonator
	BITF	*AR3(Tx_mode),#V32TCM_MODE_BIT
	BC_		Rx_init_v32_endif1,TC			;* branch if Tx_mode!=TCM			
	ST		#PJ50_COEF2400_B,PJ1_coef
	STL		A,PJ1_dnm1
	STL		A,PJ1_dnm2
	ST		#PJ60_COEF2400_B,PJ2_coef
	STL		A,PJ2_dnm1
	STL		A,PJ2_dnm2
Rx_init_v32_endif1:
	STL		A,Rx_sym_clk_memory				;* Rx_sym_clk_memory=0
	ST		#ACQ_TIMING_THR,timing_threshold	
	STL		A,coarse_error					;* coarse_error=0
	ST		#(V32_EQ_LEN/2),Rx_baud_counter	
	STL		A,Rx_sample_counter				;* sample_counter=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	STL		A,vco_memory					;* vco_memory=0
	STL		A,loop_memory					;* loop_memory=0
	STL		A,loop_memory_low				;* loop_memory=0
	STL		A,frequency_est					;* frequency_est=0
	STL		A,Rx_phase						;* Rx_phase=0
	STL		A,LOS_counter					;* LOS_counter=0
	STL		A,LOS_monitor					;* LOS_monitor=0
	ST		#WHAT_13,What			
	ST		#RX_V32_CARRIER_FREQ,LO_frequency					
	STL		A,LO_memory						;* LO_memory=0
	ST		#LO_PHASE_ADJ,LO_phase					
	STL		A,Dcounter						;* Dcounter=0
	STL		A,RCcounter						;* Dcounter=0

 .if ECHO_CANCELLER=ENABLED
	MVDK	*AR0(EC_coef_start),AR3
	STM		#(V32_NEC_LEN+V32_FEC_LEN-1),BRC
	RPTB	v32_init_EC_coef_loop
v32_init_EC_coef_loop:
	 STL	A,*AR3+							;* EC_coef[*++]=0
	LD		*AR0(Tx_sample_start),B			;* B=Tx_sample_start
	STL		B,EC_fir_ptr					;* EC_fir_ptr=&_Tx_sample[0];
	LD		Rx_sample_tail,B
	STL		B,EC_sample_ptr					;* EC_sample_ptr=Rx_sample_tail
	ST		#EC_TRAIN_DISABLED,EC_2mu		
	STL		A,EC_MSE						;* EC_MSE=0
	ST		#(V32_EC_LEN-1),EC_taps
	STL		A,Rx_RTD						;* RTD=0
 .endif

	STL		A,Rx_rate_pattern				;* rate_pattern=0
	ST		#V32_RATE_4800,Rx_rate		
	STL		A,Rx_status						;* status=0 (OK)
	RET_

;****************************************************************************
;* Rx_v32_S_detect
;* Returns:
;*		A=0 if not on symbol boundary
;*		A=1 if start is detected		
;*		A=-1 if still in progress		
;****************************************************************************

Rx_v32_S_detect:				
	CALL_	Rx_v32_filters					;* CALL here so POP works
	RC_		AEQ								;* return(0) if Rx_v32_filters()=0

	;**** detect start of burst ****

	MPY		Ihat,#ONE_BY_ROOT3,B			;* B=P600/sqrt(3)
	MAC		What,#ONE_BY_ROOT3,B			;* B+=P1800/sqrt(3)
	MAC		Qhat,#ONE_BY_ROOT3,B			;* B+=P3000/sqrt(3)
	STH		B,temp0							;* temp0=Psum

	;**** minimum power level test ****

	SUB		Rx_threshold,16,B,A				;* Psum-THR_43DB_RMS
	 ADDM	 #1,Dcounter					;* Dcounter++
	XC		2,ALT							;* if Psum<THR, Dcounter=0
	 ST		 #0,Dcounter			
	
	;**** test for 3 tones only ****

	RATIO	temp0,agc_K,THR_3DB				;* Psum/Pbb ratio
	 NOP	
	 NOP
	XC		2,BLT							;* if Psum/Pbb<THR, Dcounter=0
	 ST		 #0,Dcounter			

	;**** reject AA and AC ****

	LD		Ihat,A
	ADD		Qhat,A
	STL		A,temp1
	RATIO	temp1,What,THR_10DB		
	 NOP	
	 NOP	
	XC		2,BLT
	 ST		#0,Dcounter						;* if (F600+F3000)/F1800<THR, Dcounter=0

	LD		Dcounter,B
	SUB		#S_DETECT_THR,B					;* Dcounter-THR
	LD		#-1,A
	RC_		BLT								;* return(-1) if Dcounter<THR

	;**** calculate power ****

	SQUR	Ihat,B			
	SQURA	What,B			
	SQURA	Qhat,B			
	STH		B,Rx_power						;* Rx_power=P600+P1800+P3000

	;**** estimate agc_gain ****

;++++#ifndef MESI_INTERNAL 03-15-2001
;	ST		ACQ_AGC_K,agc_K
;++++#else   MESI_INTERNAL 03-15-2001 
	ST		#ACQ_AGC_K,agc_K
;++++#endif  MESI_INTERNAL 03-15-2001 
	CALL_	 agc_gain_estimator
	ST		V32_ACQ_LOOP_K1,loop_K1
	ST		V32_ACQ_LOOP_K2,loop_K2
	LD		#0,A
	ST		WHAT_13,What
	STL		A,Dcounter						;* Dcounter=0
	MVKD	AR2,Rx_sample_ptr				;* Rx_sample_ptr=Rx_sample_tail
	ANDM	#(S_DETECTED-1),Rx_pattern_detect
	ORM		#S_DETECTED,Rx_pattern_detect
	STL		A,Rx_sample_counter
	STL		A,Rx_symbol_counter
	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Rx_v32_train_EQ: equalizer training
;* Returns:
;*		A=0 if no symbol produced
;*		A=1 if min equalizer training sequence is completed
;*		A=-1 if still in progress
;****************************************************************************

Rx_v32_train_EQ:					
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	LD		#0,A
	RC_		BEQ								;* return(0) if head=tail

	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	MVDK	Rx_data_len,BK
	LD		*AR7+%,A						;* A=Rx_data[*++%] (symbol)
	MVKD	AR7,Rx_data_tail				;* update Rx_data_tail

	;**** return if still in training segment ****

	LD		Rx_symbol_counter,B
	SUB		#MIN_EQ_TRAIN_LEN,B
	LD		#-1,A	
	RC_		BLT								;* return(-1) if counter<MIN_EQ_TRAIN_LEN
	STPP	#v32_slicer48,slicer_ptr,B
	LD		#0,A
	ST		#ACQ_AGC_K,agc_K
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg=0
	STL		A,Rx_phase						;* Rx_phase=0
	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Rx_v32_rate: Searches for rate and E signal.
;* Returns:
;*		A=0 if no symbol produced
;*		A=1 if v32 rate signal is detected
;*		A=3 if signal E is detected
;*		A=-1 if still in progress
;****************************************************************************

Rx_v32_rate:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	LD		#0,A
	RC_		BEQ								;* return(0) if head=tail

	MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail
	 LD		*AR7+%,A						;* A=Rx_data[*++%] (symbol)

	;**** search for rate signal ****

check_RATE_detect:
	LDPP	Rx_descrambler_ptr,B			;* B=descrambler_ptr
	LD		Rx_Nbits,T
	CALAD_	B						 		;* branch to *descrambler_ptr
	 MVKD	AR7,Rx_data_tail				;* update Rx_data_tail
	LD		Rx_pattern_reg,2,A				;* A=pattern_reg<<2
	OR		B,A								;* A=(pattern_reg<<2)|k
	STL		A,Rx_pattern_reg				;* update pattern_reg

	BITF	Rx_pattern_detect,#RATE_SIGNAL_DETECTED
	BCD_	v32_rate_endif1,TC				;* branch if detect!=RATE_SIGNAL_DETECTED
	 AND	#V32_SIGNAL_MASK,A,B			;* B= j=pattern_reg&V32_SIGNAL_MASK
	XOR		#RATE_SIGNAL_PATTERN,B,A
	BCD_	v32_rate_endif0,ANEQ			;* branch if pattern!=RATE_SIGNAL
	 LD		Rx_symbol_counter,A
	 SUB	Dcounter,A						;* symbol_counter-Dcounter
	  LD	#RATE_SIGNAL_DETECTED,B
	  OR	Rx_pattern_detect,B
	XC		1,ANEQ							;* if symbol_counter!=Dcounter ...
	 LD		Rx_pattern_detect,B
	LD		Rx_pattern_reg,A
	SUB		Rx_rate_pattern,A				;* B=pattern_reg-rate_pattern
	 LD		Rx_pattern_reg,T
	 MVKD	T,Rx_rate_pattern				;* rate_pattern=Rx_pattern_reg
	XC		1,ANEQ							;* if symbol_counter!=Dcounter ...
	 LD		Rx_pattern_detect,B
	STL		B,Rx_pattern_detect
	LD		#8,A
	ADD		Rx_symbol_counter,A
	STL		A,Dcounter						;* Dcounter=Rx_symbol_counter+8
v32_rate_endif0:
	LD		#-1,A
	RET_	 								;* return(-1)
v32_rate_endif1:

	;**** search for signal E ****

signal_E:	
	LD		Rx_symbol_counter,A
	SUB		Dcounter,A
	BCD_	v32_rate_endif0,ANEQ			;* return(-1) if Dcouter!=symbol_counter
	 LD		#8,A
	 ADD	Rx_symbol_counter,A
	STL		A,Dcounter						;* Dcounter=Rx_symbol_counter+8
	XOR		#SIGNAL_E_PATTERN,B				;* j-SIGNAL_E_PATTERN
	BC_		v32_rate_endif0,BNEQ			;* return(-1) j!=SIGNAL_E
	ST		#0,Rx_symbol_counter
	LD		Rx_pattern_reg,B
	STL		B,Rx_rate_pattern				;* rate_pattern=pattern_reg
 
	LD		Rx_rate_pattern,A
	AND		#RATE_4800_BIT,A
	 LD		Rx_rate_pattern,B
	 AND	#RATE_7200_BIT,B
	XC		2,ANEQ
	 ST		#V32_RATE_4800,Rx_rate
	LD		Rx_rate_pattern,A
	AND		#RATE_9600_BIT,A
	XC		2,BNEQ
	 ST		#V32_RATE_7200,Rx_rate
	LD		Rx_rate_pattern,B
	AND		#RATE_12000_BIT,B
	XC		2,ANEQ
	 ST		#V32_RATE_9600,Rx_rate
	LD		Rx_rate_pattern,A
	AND		#RATE_14400_BIT,A
	XC		2,BNEQ
	 ST		#V32_RATE_12000,Rx_rate
	XC		2,ANEQ
	 ST		#V32_RATE_14400,Rx_rate

	LD		Rx_rate_pattern,A
	AND		#(RATE_14400_PATTERN&~V32BIS_BITS),A
	SUB		#RATE_4800_BIT,A
	BC_		signal_E_endif1,AEQ				;* branch if pattern&(.)==RATE_4800_BIT
	ST		#4,Rx_Nbits
	ST		#0fh,Rx_Nmask
	LDU		vco_memory,B
	SUB		#TWENTY_SIX_DEGREES,B
	STL		B,vco_memory					;* vco_memory-=TWENTY_SIX_DEGREES
	ST		#2,Rx_map_shift
	STPP	#v32_slicer96,slicer_ptr,B
signal_E_endif1:

 .if TCM_DECODER=ENABLED
	BITF	Rx_rate_pattern,#TCM_BIT
	BC_		signal_E_endif2,NTC				;* branch if TCM_BIT not set
	LD		Rx_rate_pattern,B
	AND		#(RATE_14400_PATTERN&~V32BIS_BITS),B
	SUB		#RATE_4800_BIT,B
	BC_		signal_E_endif2,BEQ				;* branch if RATE_4800_BIT
	CALL_	Rx_init_TCM
	STPP	#TCM_slicer,slicer_ptr,B
	STPP	#TCM_decoder,decoder_ptr,B
	ADDM	#-(TRACE_BACK_LEN-1),Rx_symbol_counter
signal_E_endif2:
 .endif

	ST		#EQ_UPDATE_DISABLED,EQ_2mu	
	ST		#0,Rx_pattern_reg
	STPP	#APSK_timing,timing_ptr,B
	ST		#V32_TRK_LOOP_K1,loop_K1		
	ST		#V32_TRK_LOOP_K2,loop_K2		
	ST		#TRK_TIMING_THR,timing_threshold
	ORM		#SIGNAL_E_DETECTED,Rx_pattern_detect
	LD		#3,A
	RET_	;* return(3)	

;****************************************************************************
;* Rx_v32_B1: processes B1 signal
;* Returns:
;*		A=0 if no symbol produced
;*		A=1 if end of B1 is detected
;*		A=-1 if still in progress
;****************************************************************************

Rx_v32_B1:	
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	LD		#0,A
	RC_		BEQ								;* return(0) if head=tail

	MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR7				;* update Rx_data_tail

	;**** descramble and wait for end of B1 ****

	LDPP	Rx_descrambler_ptr,B			;* B=descrambler_ptr
	CALAD_	B						 		;* branch to *descrambler_ptr
	 LD		*AR7,A							;* A=Rx_data[*] (symbol)
	 LD		Rx_Nbits,T
	STL		B,*AR7+%						;* Rx_data[*++%]=descrambled symbol
	MVKD	AR7,Rx_data_tail				;* update Rx_data_tail
	LD		Rx_symbol_counter,B
	SUB		#RX_B1_LEN,B					;* symbol_counter-Dcounter
	LD		#-1,A
	RC_		BLT								;* return(-1) if counter<LEN

	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
 .if $isdefed("V32_STU_III")
	BITF	Rx_mode,#RX_DESCRAMBLER_DISABLE_BIT
	BC_		Rx_v32_B1_endif2,TC				;* if mode&BIT != 0 ...
	STPP	#no_descrambler,Rx_descrambler_ptr,B ;* ... ptr=no_descrambler
Rx_v32_B1_endif2:
 .endif
	LD		#0,A
	STL		A,Dcounter						;* Dcounter=0
	STL		A,Rx_sample_counter				;* sample_counter=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	ST		#MESSAGE_DETECTED,Rx_pattern_detect
	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Rx_v32_message: processes message data
;* Returns:
;*		A=0 if no symbol produced
;*		A=1 if symbol(s) processed
;****************************************************************************

Rx_v32_message:					
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_ptr,B
	LD		#0,A
	RC_		BEQ								;* return(0) if head=ptr

	MVDK	Rx_data_len,BK
	MVDK	Rx_data_ptr,AR7 

mes_while_loop:
	LDPP	Rx_descrambler_ptr,B			;* B=descrambler_ptr
	CALAD_	B						 		;* branch to *descrambler_ptr
	 LD		*AR7,A							;* A=Rx_data[*] (symbol)
	 LD		Rx_Nbits,T
	STL		B,*AR7+%						;* Rx_data[*++%]=descrambled symbol
	LDM		AR7,A	
	SUBS	Rx_data_head,A					;* Rx_data_ptr-Rx_data_head
	BCD_	mes_while_loop,ANEQ				;* loop if ptr!=head
	 MVKD	AR7,Rx_data_ptr					;* update Rx_data_ptr

	LD		#1,A
	RET_	;* return(1)

;****************************************************************************
;* Rx_v32_RC_detector: looks for continuous stable signal, and switches to  
;* QPSK demod if found. The input, "signal" should be the sum of the   
;* differential Iprime and Qprime points.						 
;* Expects the following on entry:
;*	 A=signal
;****************************************************************************

Rx_v32_RC_detector:	 

	;**** IF STU_III && LOS, freeze loops ****

	BITF	Rx_mode,#RX_STU_III_BIT	
	BCD_	Rx_v32_RC_detector_endif1,NTC	;* branch if mode!=STU_III
	 ADDM	#1,RCcounter					;* RCcounter++
	LD		LOS_counter,B
	SUB		#LOS_COUNT,B
	BC_		Rx_v32_RC_detector_elseif1,BLT ;* branch if LOCS_counter<COUNT
;++++#ifndef MESI_INTERNAL 03-13-2001
;	ST		ACQ_AGC_K,agc_K
;	ST		V32_TRK_LOOP_K1,loop_K1
;	ST		V32_TRK_LOOP_K2,loop_K2
;	STPP	#APSK_timing,timing_ptr,B
;++++#else   MESI_INTERNAL 03-13-2001
	LD		#0,A
	STL		A,agc_K							;* agc_K=0
	STL		A,loop_K1						;* loop_K1=0
	STL		A,loop_K2						;* loop_K2=0
	STPP	#no_timing,timing_ptr,B
	B_		Rx_v32_RC_detector_endif1
;++++#endif  MESI_INTERNAL 03-13-2001
Rx_v32_RC_detector_elseif1:
	ST		#ACQ_AGC_K,agc_K
	ST		V32_TRK_LOOP_K1,loop_K1
	ST		V32_TRK_LOOP_K2,loop_K2
	STPP	#APSK_timing,timing_ptr,B
Rx_v32_RC_detector_endif1:

	;**** reject signal if LOS condition ****
	
	LD		LOS_counter,B
	SUB		#LOS_COUNT,B
	 SUB	#RC_DETECT_LEVEL,A
	XC		#2,BGEQ							;* if LOS_counter>=COUNT...
	 ST		#0,RCcounter					;* ... RCcounter=0

	;**** check for continuous stable signal ****

	XC		#2,AGT							;* if signal>RC_DETECT_LEVEL ...
	 ST		#0,RCcounter					;* ... RCcounter=0
	LD		RCcounter,B
	SUB		#RC_DETECT_THRESHOLD,B
	RC_		BLT								;* return if RCcounter<RC_DETECT_THRESHOLD

	;**** return if already in QPSK ****

	BITF	Rx_pattern_detect,#RC_PREAMBLE_DETECTED
	RC_		TC								;* return if !RC_PREAMBLE_DETECTED
	ANDM	#~(MESSAGE_DETECTED|RATE_SIGNAL_DETECTED|SIGNAL_E_DETECTED),Rx_pattern_detect
	ORM		#RC_PREAMBLE_DETECTED,Rx_pattern_detect
	LD		#0,A
	CMPM	Rx_rate,#V32_RATE_4800
	RCD_	TC								;* return if rate=V32_RATE_4800
	 STL	A,Dcounter						;* Dcounter=0
	 STL	A,Rx_symbol_counter				;* symbol_counter=0

	;**** switch to QPSK ****

	STPP	#v32_diff_decoder,decoder_ptr,B
	STPP	#v32_slicer48,slicer_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	LD		#0,A
	LD		Rx_data_head,B
	STL		B,Rx_data_tail
	STL		A,data_Q1						;* data_Q1=0
	ST		#2,Rx_Nbits		
	ST		#3,Rx_Nmask		
	STL		A,Rx_map_shift					;* map_shift=0
	ST		#V32_ACQ_LOOP_K1,loop_K1			
	ST		#V32_ACQ_LOOP_K2,loop_K2			
	ST		#ACQ_TIMING_THR,timing_threshold	
	STL		A,Rx_rate_pattern
	ST		#WHAT_13,What			
	RETD_
	 ST		#V32_RATE_4800,Rx_rate		

;****************************************************************************
;* Rx_v32_filters: subsamples at 2400 Hz and runs 3 band pass filters.
;* Returns:
;*		A=0 if no sub-sample filter update
;*		A=1 if filters executed
;****************************************************************************

Rx_v32_filters:					
	MVDK	Rx_sample_stop,AR0
	CMPR	0,AR2							;* sample_tail-sample_stop
	LD		#0,A
	RC_		TC								;* return(0) if tail=stop

	MAR		*AR2+%							;* Rx_sample_tail++%
	MVKD	AR2,Rx_sample_tail
	MVDK	Rx_sample_counter,AR7
	STM		#10,BK							;* modulus=10
	MAR		*+AR7(3)%						;* AR7=(Rx_sample_counter+3)%10
	MVKD	AR7,Rx_sample_counter
	LD		Rx_sample_counter,B
	SUB		#3,B
	BCD_	Rx_v32_filters,BGEQ				;* return if counter>=3
	 MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len

	;**** set up for fir band pass filters ****

	ADDM	#1,Rx_symbol_counter
	MVMM	AR2,AR7							;* AR7=Rx_sample_tail
	SSBX	TC								;* enable filter
	BPF		FILTER_600_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Ihat							;* Ihat=P600
	SSBX	TC								;* enable filter
	BPF		FILTER_1800_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,What							;* What=P1800
	SSBX	TC								;* enable filter
	BPF		FILTER_3000_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Qhat							;* Qhat=P3000
	BB_EST BROADBAND_EST_COEF,BROADBAND_EST_LEN,AR7
	STH		A,agc_K							;* agc_K=broadband_level
	LD		#1,A
	RET_	  								;* return(1)

;****************************************************************************
;* v32A_descrambler: V32 GPC descrambler 
;* v32C_descrambler: V32 GPA descrambler 
;* Expects the following on entry:
;*	DP=&Rx_block
;*	A=in
;*	T=Nbits
;* On exit:
;*	B=out
;****************************************************************************

v32A_descrambler:
	RSBX	OVM								;* reset overflow mode
	LD		Rx_Dreg,16,B
	ADDS	Rx_Dreg_low,B					;* B=Dreg
	NORM	B								;* B=(Dreg<<Nbits)
	OR		A,B								;* B=(Dreg<<Nbits)|in
	STH		B,Rx_Dreg						;* update Dreg
	STL		B,Rx_Dreg_low				
	SSBX	OVM								;* set overflow mode
	SFTL	B,-6							;* B=Dreg>>6
	SFTL	B,-12							;* B=Dreg>>(18-N)
	XOR		B,-5							;* B=Dreg>>(18-N^Dreg)>>(23-N)
	RETD_
	 XOR	A,B								;* A=in^Dreg>>(18-N)^Dreg>>(23-N)
	 AND	Rx_Nmask,B						;* A=Dreg>>(18-N)^Dreg>>(23-N)&Nmask


v32C_descrambler:
	RSBX	OVM								;* reset overflow mode
	LD		Rx_Dreg,16,B
	ADDS	Rx_Dreg_low,B					;* B=Dreg
	NORM	B								;* B=(Dreg<<N)
	OR		A,B								;* B=(Dreg<<N)|in
	STH		B,Rx_Dreg						;* update Dreg
	STL		B,Rx_Dreg_low				
	SSBX	OVM								;* set overflow mode

	SFTL	B,-5							;* B=Dreg>>(5-N)
	XOR		A,B								;* B=in^Dreg>>(5-N)
	LD		Rx_Dreg,-7,A					;* A=Dreg>>(23-N)
	RETD_
	 XOR	A,B								;* B=in^Dreg>>(5-N)^Dreg>>(23-N)
	 AND	Rx_Nmask,B						;* A=Dreg>>(18-N)^Dreg>>(23-N)&Nmask

;****************************************************************************
;* v32C_descrambler6: V32 GPC scrambler for Nbits=6
;* Expects the following on entry:
;*	A=in
;*	T=Nbits
;* On exit:
;*	B=out
;****************************************************************************

 .if V32BIS_MODEM=ENABLED
v32C_descrambler6:
	STLM	A,T								;* T=in
	LD		Rx_Dreg,-2,B					;* B=Dreg>>18
	XOR		Rx_Dreg_low,B					;* B=Dreg^Dreg>>(18)
	SFTL	B,1								;* B=(Dreg<<1)^(Dreg>>17)
	XOR		A,B								;* B=in^(Dreg<<1)^(Dreg>>17)
	AND		#3eh,B							;* B= k=((Dreg<<1)^(Dreg>>17))&0x3e
	LD		Rx_Dreg,16,A
	ADDS	Rx_Dreg_low,A					;* A=Dreg
	SFTL	A,6								;* A=Dreg<<6
	OR		*(T),A							;* A=(Dreg<<6)|in
	STH		A,Rx_Dreg						;* update Dreg
	STL		A,Rx_Dreg_low				

	LD		Rx_Dreg,-2,A					;* A=Dreg>>18
	XOR		Rx_Dreg_low,A					;* A=Dreg^Dreg>>(18)
	SFTL	A,-5							;* A=Dreg>>(5) ^ Dreg>>(23)
	XOR		*(T),A							;* A=in^Dreg>>(5-N)^Dreg>>(23-N)
	AND		#1,A							;* A=(in^Dreg>>(5-N)^Dreg>>(23-N))&1
	RETD_
	 OR		A,B								;* B=out|k
	 NOP
 .endif

;****************************************************************************
;* no_descrambler: just return
;* Expects the following on entry:
;*	A=in
;* On exit:
;*	B=in
;****************************************************************************

 .if $isdefed("V32_STU_III")
no_descrambler:
;++++#ifndef MESI_INTERNAL 03-14-2001
	LD		A,B								;* rturn(in)
;++++#endif  MESI_INTERNAL 03-14-2001
	RET_
 .endif

;****************************************************************************
;* v32_slicer48: QPSK slicer.
;****************************************************************************

v32_slicer48:
	LD		Iprime,B
	 ST		#-SLICE_707,Ihat			
	 STM	#Rx_v32_hard_map,AR5			;* AR5=&Rx_v32_hard_map[k]
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
	 LD		*AR5,A							;* A=Rx_v32_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v32_hard_map[k]
 .else
	 LDM	AR5,A						 	;* A=&Rx_v32_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v32_hard_map[k]
 .endif

;****************************************************************************
;* v32A_EQslicer48: reference assisted slicer for GPC equalizer training.
;****************************************************************************

v32A_EQslicer48:
	LD		Rx_Dreg,-5,B					;* B=Dreg>>(23-2)
	XOR		Rx_Dreg,B						;* B=Dreg>>(18-2)^Dreg>>(23-2)
	XOR		Rx_Nmask,B						;* B=3^Dreg>>(18-2)^Dreg>>(23-2)
	AND		Rx_Nmask,B						;* B=(Dreg>>(18-2)^Dreg>>(23-2))&3
	LD		Rx_Dreg,16,A
	ADDS	Rx_Dreg_low,A					;* A=Dreg
	SFTL	A,2
	OR		B,A								;* A=(Dreg<<2)|ref
	STH		A,Rx_Dreg						;* update Dreg
	STL		A,Rx_Dreg_low				

	AND		#2,B,A							;* A=ref&2
	 ST		#SLICE_707,Ihat
	 ST		#SLICE_707,Qhat
			XC	2,AEQ
	 ST		#-SLICE_707,Ihat
			XC	2,AEQ
	 ST		#-SLICE_707,Qhat
	LD		Rx_symbol_counter,A
	SUB		#TRN1_LEN,A
	BCD_	slicer_return,ALT				;* return if counter<TRN1_LEN
	 SUB	#1,B,A							;* A=ref-1
	SUB		#2,B							;* B=ref-2
	XC		2,AEQ				 
	 ST		#SLICE_707,Ihat
	XC		2,BEQ		 
	 ST		#-SLICE_707,Ihat
	B_		slicer_return

;****************************************************************************
;* v32C_EQslicer48: reference assisted slicer for GPA equalizer training.
;****************************************************************************

v32C_EQslicer48:
	LD		Rx_Dreg,-2,B					;* B=Dreg>>18
	XOR		Rx_Dreg_low,B					;* B=Dreg^Dreg>>(18)
	SFTL	B,-3							;* B=Dreg>>(5-2) ^ Dreg>>(23-2)
	XOR		Rx_Nmask,B						;* B=3^Dreg>>(5-2)^Dreg>>(23-2)
	AND		Rx_Nmask,B						;* B=(Dreg>>(5-2)^Dreg>>(23-2))&Nmask
	LD		Rx_Dreg,16,A
	ADDS	Rx_Dreg_low,A					;* A=Dreg
	SFTL	A,2
	OR		B,A								;* A=(Dreg<<2)|ref
	STH		A,Rx_Dreg						;* update Dreg
	STL		A,Rx_Dreg_low				

	AND		#2,B,A							;* A=ref&2
	 ST		#SLICE_707,Ihat
	 ST		#SLICE_707,Qhat
			XC	2,AEQ
	 ST		#-SLICE_707,Ihat
			XC	2,AEQ
	 ST		#-SLICE_707,Qhat
	LD		Rx_symbol_counter,A
	SUB		#TRN1_LEN,A
	BCD_	slicer_return,ALT				;* return if counter<TRN1_LEN
	 SUB	#1,B,A							;* A=ref-1
	SUB		#2,B							;* B=ref-2
	XC		2,AEQ				 
	 ST		#SLICE_707,Ihat
	XC		2,BEQ		 
	 ST		#-SLICE_707,Ihat
	B_		slicer_return

;****************************************************************************
;* v32_slicer96: 16 QAM slicer
;****************************************************************************

v32_slicer96:
	STM		#Rx_v32_hard_map,AR5			;* AR5=&Rx_v32_hard_map[k]					 
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
	 LD		*AR5,A							;* A=Rx_v32_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v32_hard_map[k]
 .else
	 LDM	AR5,A						 	;* A=&Rx_v32_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v32_hard_map[k]
 .endif

;****************************************************************************
;* v32_diff_decoder: hard decision symbol differential decoder for v32.
;* On entry it expects:
;*	AR7=Rx_data_head
;*	BK=Rx_data_len
;*	A=Phat
;****************************************************************************

v32_diff_decoder:
 .if ON_CHIP_COEFFICIENTS=ENABLED
	LD		Rx_map_shift,T
	ADD		#4,A							;* A=Phat+4
	SUB		Rx_phase,A						;* A=Phat+4-Rx_phase
	AND		#3,A							;* A&=7
	ADD		#Rx_v32_phase_map,A				;* A=&Rx+phase_map[(Phat+4-Rx_phase&3]
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
	ADD		#Rx_v32_phase_map,A				;* A=&Rx+phase_map[(Phat+4-Rx_phase&3]
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7							;* Rx_data[*]=Rx_v32_phase_map[*]
	LD		*AR7,TS,A						;* A=phase_map[*]<<map_shift
	OR		data_Q1,A						;* A|=data_Q1
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_phase_map[*]
	LD		Phat,B
	STL		B,Rx_phase						;* Rx_phase=Phat
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .endif

;****************************************************************************
 .endif

	.end
