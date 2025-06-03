;****************************************************************************
;* Filename: v29.asm
;* Date: 04-10-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for V.29.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"v29.inc"

	;**** modulator ****

V29_AUTO_RATE		  		.set	0
V29_RATE_4800		  		.set	4800
V29_RATE_7200		  		.set	7200
V29_RATE_9600		  		.set	9600

TX_FIR_TAPS					.set	5
TX_INTERP			  		.set	2*DEC2400 
TX_DEC				 		.set	INTERP2400
TX_COEF_LEN					.set	(TX_FIR_TAPS*TX_INTERP+TX_DEC)		
TX_V29_SCALE_HIGH	  		.set	16625
TX_V29_SCALE_MID			.set	26076
TX_V29_SCALE_LOW			.set	20340	

SCRAMBLER_SEED		 		.set	2ah
TX_PHASE_LEN				.set	24
TX_CARRIER			 		.set	17

TX_TEP_LEN			 		.set	480
TX_SEGMENT1_LEN				.set	48
TX_LONG_SEGMENT2_LEN		.set    128
TX_RESYNC_SEGMENT2_LEN		.set    64 
TX_LONG_SEGMENT3_LEN 		.set    384
TX_RESYNC_SEGMENT3_LEN 		.set    64 
TX_SEGMENT4_LEN				.set	48 
TX_SEGMENT6_LEN				.set	3

 .if $isdefed("XDAIS_API")
	.global _V29_MESI_TxInitV29
	.global V29_MESI_TxInitV29
 .else
	.global _Tx_init_v29
	.global Tx_init_v29
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_APSKmodulator
	.asg	COMMON_MESI_APSKmodulator, APSK_modulator
	.global RXTX_MESI_TxInitSilence
	.asg	RXTX_MESI_TxInitSilence, Tx_init_silence
	.global RXTX_MESI_TxStateReturn
	.asg	RXTX_MESI_TxStateReturn, Tx_state_return
 .else
	.global APSK_modulator
	.global Tx_init_silence
	.global Tx_state_return
 .endif										;* "XDAIS_API endif

	;**** demodulator ****

;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;RX_FIR_TAPS					.set	16 
;RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
;RX_INTERP			  		.set	INTERP2400
;RX_DEC				 		.set	DEC2400
;RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
;RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
;RX_BAUD_RATE				.set	2400
;RX_V29_CARRIER_FREQ			.set	23210   ;* (1700*65536/4800))
;LO_PHASE_ADJ				.set	580	 	;* (65536/8000)*(1700/RX_INTERP)
;
;V29_EQ_LEN			 		.set	53 
;ACQ_EQ_2MU			 		.set	1024	
;TRK_EQ_2MU			 		.set	256 
;ACQ_AGC_K			  		.set	512
;TRK_AGC_K			  		.set	128
;EQ_TRAIN_SEED		  		.set	0aah	;* seed for equalizer training reference generator
;ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
;TRK_TIMING_THR		 		.set	3584	;* (32768*OP_POINT*7.0)
;TAN22				  		.set	13572
;SLICE_1						.set	512	 	;* OP_POINT*32768
;SLICE_707			  		.set	362	
;
;;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;WHAT						.set	512	 	;* (Pavg/(1)^2)*32768/64
;;
;;SLICE48_3			  		.set	512	 	;* OP_POINT*32768
;;WHAT48_30			  		.set	512	 	;* (Pavg/(1)^2)*32768/64
;;
;;SLICE72_1			  		.set	218	 	;* (1/SIGavg)*OP_POINT*32768
;;SLICE72_2			  		.set	437	 	;* (2/SIGavg)*OP_POINT*32768
;;SLICE72_3			  		.set	655	 	;* (3/SIGavg)*OP_POINT*32768
;;WHAT72_11			  		.set	1408	;* (Pavg/(1^2+1^2))*32768/64
;;WHAT72_30			  		.set	313	 	;* (Pavg/3^2)*32768/64
;;
;;SLICE96_1			  		.set	139	 	;* (1/SIGavg)*OP_POINT*32768
;;SLICE96_134					.set	187	 	;* (1.34/SIGavg)*OP_POINT*32768
;;SLICE96_2			  		.set	279	 	;* (2/SIGavg)*OP_POINT*32768
;;SLICE96_3			  		.set	418	 	;* (3/SIGavg)*OP_POINT*32768
;;SLICE96_4			  		.set	557	 	;* (4/SIGavg)*OP_POINT*32768
;;SLICE96_5			  		.set	697	 	;* (5/SIGavg)*OP_POINT*32768
;;WHAT96_11			  		.set	3456	;* (Pavg/(1^2+1^2)*32768/16
;;WHAT96_30			  		.set	768	 	;* (Pavg/(3^2))*32768/16
;;WHAT96_33			  		.set	384	 	;* (Pavg/(3^2+3^2))*32768/16
;;WHAT96_50			  		.set	276	 	;* (Pavg/(5^2))*32768/16
;;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
; .if $isdefed("SQUARE_ROOT_WHAT")
;WHAT						.set	512		;* OP_POINT*SIG_AVG/SIG_11
;
;SLICE48_3			  		.set	512	 	;* OP_POINT*32768
;WHAT48_30			  		.set	512		;* OP_POINT*SIG_AVG/SIG_11
;
;SLICE72_1			  		.set	218	 	;* (1/SIGavg)*OP_POINT*32768
;SLICE72_2			  		.set	437	 	;* (2/SIGavg)*OP_POINT*32768
;SLICE72_3			  		.set	655	 	;* (3/SIGavg)*OP_POINT*32768
;WHAT72_11			  		.set	849		;* OP_POINT*SIG_AVG72/SIG72_11
;WHAT72_30			  		.set	400     ;* OP_POINT*SIG_AVG72/SIG72_30
;
;SLICE96_1			  		.set	139	 	;* (1/SIGavg)*OP_POINT*32768
;SLICE96_134					.set	187	 	;* (1.34/SIGavg)*OP_POINT*32768
;SLICE96_2			  		.set	279	 	;* (2/SIGavg)*OP_POINT*32768
;SLICE96_3			  		.set	418	 	;* (3/SIGavg)*OP_POINT*32768
;SLICE96_4			  		.set	557	 	;* (4/SIGavg)*OP_POINT*32768
;SLICE96_5			  		.set	697	 	;* (5/SIGavg)*OP_POINT*32768
;WHAT96_11			  		.set	1330    ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_11
;WHAT96_30			  		.set	627     ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_30
;WHAT96_33			  		.set	443     ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_33
;WHAT96_50			  		.set	376     ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_50
;
; .else		;* SQUARE_ROOT_WHAT
; 
;WHAT						.set	512	 	;* (Pavg/(1)^2)*32768/64
;
;SLICE48_3			  		.set	512	 	;* OP_POINT*32768
;WHAT48_30			  		.set	512	 	;* (Pavg/(1)^2)*32768/64
;
;SLICE72_1			  		.set	218	 	;* (1/SIGavg)*OP_POINT*32768
;SLICE72_2			  		.set	437	 	;* (2/SIGavg)*OP_POINT*32768
;SLICE72_3			  		.set	655	 	;* (3/SIGavg)*OP_POINT*32768
;WHAT72_11			  		.set	1408	;* (Pavg/(1^2+1^2))*32768/64
;WHAT72_30			  		.set	313	 	;* (Pavg/3^2)*32768/64
;
;SLICE96_1			  		.set	139	 	;* (1/SIGavg)*OP_POINT*32768
;SLICE96_134					.set	187	 	;* (1.34/SIGavg)*OP_POINT*32768
;SLICE96_2			  		.set	279	 	;* (2/SIGavg)*OP_POINT*32768
;SLICE96_3			  		.set	418	 	;* (3/SIGavg)*OP_POINT*32768
;SLICE96_4			  		.set	557	 	;* (4/SIGavg)*OP_POINT*32768
;SLICE96_5			  		.set	697	 	;* (5/SIGavg)*OP_POINT*32768
;WHAT96_11			  		.set	3456	;* (Pavg/(1^2+1^2)*32768/16
;WHAT96_30			  		.set	768	 	;* (Pavg/(3^2))*32768/16
;WHAT96_33			  		.set	384	 	;* (Pavg/(3^2+3^2))*32768/16
;WHAT96_50			  		.set	276	 	;* (Pavg/(5^2))*32768/16
; .endif		;* SQUARE_ROOT_WHAT
;;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;
;V29_ACQ_LOOP_K1				.set	6178
;V29_ACQ_LOOP_K2				.set	582
;V29_TRK_LOOP_K1				.set	1544
;V29_TRK_LOOP_K2				.set	36
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
RX_FIR_TAPS					.set	16 
RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
RX_INTERP			  		.set	INTERP2400
RX_DEC				 		.set	DEC2400
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_BAUD_RATE				.set	2400
RX_V29_CARRIER_FREQ			.set	23210   ;* (1700*65536/4800))
LO_PHASE_ADJ				.set	580	 	;* (65536/8000)*(1700/RX_INTERP)

V29_EQ_LEN			 		.set	53 
ACQ_EQ_2MU			 		.set	247
TRK_EQ_2MU			 		.set	(ACQ_EQ_2MU/4)
ACQ_AGC_K			  		.set	64 
TRK_AGC_K			  		.set	16 
EQ_TRAIN_SEED		  		.set	0aah	;* seed for equalizer training reference generator
ACQ_TIMING_THR		 		.set	2048  	;* (32768*OP_POINT*0.5)
TRK_TIMING_THR		 		.set	28672	;* (32768*OP_POINT*7.0)
TAN22				  		.set	13572
SLICE_1						.set	4096	;* OP_POINT*32768
SLICE_707			  		.set	2896	
WHAT						.set	4096	;* OP_POINT*SIG_AVG/SIG_11

SLICE48_3			  		.set	4096	;* OP_POINT*32768
WHAT48_30			  		.set	4096	;* OP_POINT*SIG_AVG/SIG_11

SLICE72_1			  		.set	1747	;* (1/SIGavg)*OP_POINT*32768
SLICE72_2			  		.set	3493	;* (2/SIGavg)*OP_POINT*32768
SLICE72_3			  		.set	5240	;* (3/SIGavg)*OP_POINT*32768
WHAT72_11			  		.set	6792	;* OP_POINT*SIG_AVG72/SIG72_11
WHAT72_30			  		.set	3202    ;* OP_POINT*SIG_AVG72/SIG72_30

SLICE96_1			  		.set	1115  	;* (1/SIGavg)*OP_POINT*32768
SLICE96_134					.set	1494  	;* (1.34/SIGavg)*OP_POINT*32768
SLICE96_2			  		.set	2230  	;* (2/SIGavg)*OP_POINT*32768
SLICE96_3			  		.set	3344  	;* (3/SIGavg)*OP_POINT*32768
SLICE96_4			  		.set	4459  	;* (4/SIGavg)*OP_POINT*32768
SLICE96_5			  		.set	5574  	;* (5/SIGavg)*OP_POINT*32768
WHAT96_11			  		.set	10642   ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_11
WHAT96_30			  		.set	5017    ;* 32768.0*OP_POINT*SIG_AVG96/SIG96_30
WHAT96_33			  		.set	3547	;* 32768.0*OP_POINT*SIG_AVG96/SIG96_33
WHAT96_50			  		.set	3010	;* 32768.0*OP_POINT*SIG_AVG96/SIG96_50

V29_ACQ_LOOP_K1				.set	772
V29_ACQ_LOOP_K2				.set	72 
V29_TRK_LOOP_K1				.set	193
V29_TRK_LOOP_K2				.set	4  
 .else      ;* OP_POINT=8
RX_FIR_TAPS					.set	16 
RX_OVERSAMPLE		  		.set	OVERSAMPLE2400
RX_INTERP			  		.set	INTERP2400
RX_DEC				 		.set	DEC2400
RX_COEF_LEN					.set	(RX_FIR_TAPS*RX_INTERP+RX_DEC)
RX_COEF_SAMPLE_RATE			.set	(8000*RX_INTERP) 
RX_BAUD_RATE				.set	2400
RX_V29_CARRIER_FREQ			.set	23210   ;* (1700*65536/4800))
LO_PHASE_ADJ				.set	580	 	;* (65536/8000)*(1700/RX_INTERP)

V29_EQ_LEN			 		.set	53 
ACQ_EQ_2MU			 		.set	2048
TRK_EQ_2MU			 		.set	(ACQ_EQ_2MU/4)
ACQ_AGC_K			  		.set	512
TRK_AGC_K			  		.set	128
EQ_TRAIN_SEED		  		.set	0aah	;* seed for equalizer training reference generator
ACQ_TIMING_THR		 		.set	256	 	;* (32768*OP_POINT*0.5)
TRK_TIMING_THR		 		.set	3584	;* (32768*OP_POINT*7.0)
TAN22				  		.set	13572
SLICE_1						.set	512	 	;* OP_POINT*32768
SLICE_707			  		.set	362	
WHAT						.set	512	 	;* (Pavg/(1)^2)*32768/64

SLICE48_3			  		.set	512	 	;* OP_POINT*32768
WHAT48_30			  		.set	512	 	;* (Pavg/(1)^2)*32768/64

SLICE72_1			  		.set	218	 	;* (1/SIGavg)*OP_POINT*32768
SLICE72_2			  		.set	437	 	;* (2/SIGavg)*OP_POINT*32768
SLICE72_3			  		.set	655	 	;* (3/SIGavg)*OP_POINT*32768
WHAT72_11			  		.set	1408	;* (Pavg/(1^2+1^2))*32768/64
WHAT72_30			  		.set	313	 	;* (Pavg/3^2)*32768/64

SLICE96_1			  		.set	139	 	;* (1/SIGavg)*OP_POINT*32768
SLICE96_134					.set	187	 	;* (1.34/SIGavg)*OP_POINT*32768
SLICE96_2			  		.set	279	 	;* (2/SIGavg)*OP_POINT*32768
SLICE96_3			  		.set	418	 	;* (3/SIGavg)*OP_POINT*32768
SLICE96_4			  		.set	557	 	;* (4/SIGavg)*OP_POINT*32768
SLICE96_5			  		.set	697	 	;* (5/SIGavg)*OP_POINT*32768
WHAT96_11			  		.set	3456	;* (Pavg/(1^2+1^2)*32768/16
WHAT96_30			  		.set	768	 	;* (Pavg/(3^2))*32768/16
WHAT96_33			  		.set	384	 	;* (Pavg/(3^2+3^2))*32768/16
WHAT96_50			  		.set	276	 	;* (Pavg/(5^2))*32768/16

V29_ACQ_LOOP_K1				.set	6178
V29_ACQ_LOOP_K2				.set	582
V29_TRK_LOOP_K1				.set	1544
V29_TRK_LOOP_K2				.set	36
 .endif		;* OP_POINT=8
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 

RX_LONG_SEGMENT3_LEN 		.set    TX_LONG_SEGMENT3_LEN
RX_RESYNC_SEGMENT3_LEN 		.set    TX_RESYNC_SEGMENT3_LEN
TRAIN_LOOPS_TIMEOUT			.set	128+120
START_EQ_TIMEOUT			.set	128+120
TRAIN_EQ_TIMEOUT			.set	384+20
SCR1_TIMEOUT 				.set    TX_SEGMENT4_LEN

 .if $isdefed("XDAIS_API")
	.global _V29_MESI_RxInitV29
	.global V29_MESI_RxInitV29
 .else
	.global _Rx_init_v29
	.global Rx_init_v29
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
	.global COMMON_MESI_RxTrainLoops
	.asg	COMMON_MESI_RxTrainLoops, Rx_train_loops
	.global COMMON_MESI_RxDetectEQ
	.asg	COMMON_MESI_RxDetectEQ, Rx_detect_EQ
	.global COMMON_MESI_agcGainEstimator
	.asg	COMMON_MESI_agcGainEstimator, agc_gain_estimator

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
 .else
	.global Rx_init_detector
	.global APSK_demodulator
	.global no_timing
	.global sgn_timing
	.global APSK_timing
	.global Rx_train_loops
	.global Rx_detect_EQ
	.global agc_gain_estimator

	.global _RCOS2400_f1800
	.global _Rx_timing2400
	.global Rx_state_return
	.global slicer_return
	.global timing_return
	.global decoder_return
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global Tx_v29_TEP
	.global Tx_v29_segment1
	.global Tx_v29_segment2
	.global Tx_v29_segment3
	.global Tx_v29_segment4
	.global Tx_v29_message
	.global Tx_v29_segment6
	.global v29_abs_encoder				
	.global v29_diff_encoder				
	.global v29_scrambler					
	.global Rx_v29_train_loops
	.global Rx_v29_detect_EQ
	.global Rx_EQ_detected
	.global Rx_v29_train_EQ
	.global Rx_v29_scr1
	.global Rx_v29_message
	.global v29_mes_while_loop
	.global v29_descrambler
	.global v29_slicer48
	.global v29_EQslicer48
	.global v29_slicer72
	.global slicer72_onQ
	.global slicer72_onI
	.global v29_EQslicer72
	.global v29_slicer96
	.global slicer96_step2					
	.global slicer96_step3					
	.global slicer96_step4					
	.global slicer96_step5					
	.global slicer96_step6					
	.global v29_EQslicer96
	.global v29_diff_decoder
 .endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

;****************************************************************************
;* tables and coefficients
;****************************************************************************

	.sect	"vcoefs"

 .if TX_V29_MODEM=ENABLED
Tx_v29_phase_map:							
	.word 3,0,6,9,18,21,15,12

Tx_v29_scale_table:
	.word 19661,32767,9269,27805

Tx_v29_amp_table:
	.word  32767,	  0
	.word  31652,  8480
	.word  28378, 16384
	.word  23170, 23170
	.word  16384, 28378
	.word   8480, 31652

	.word	   0, 32767
	.word  -8480, 31652
	.word -16384, 28378
	.word -23170, 23170
	.word -28378, 16384
	.word -31652,  8480

	.word -32767,	  0
	.word -31652, -8480
	.word -28378,-16384
	.word -23170,-23170
	.word -16384,-28378
	.word  -8480,-31652

	.word	   0,-32767
	.word   8480,-31652
	.word  16384,-28378
	.word  23170,-23170
	.word  28378,-16384
	.word  31652, -8480
 .endif

 .if RX_V29_MODEM=ENABLED
Rx_v29_hard_map:
	.word   0,1,3,2,7,6,4,5

Rx_v29_phase_map:
	.word   1,0,2,3,7,6,4,5
 .endif

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_v29(struct START_PTRS *)
;* void Rx_init_v29(struct START_PTRS *)
;*
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TX_V29_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v29:
;* C function call: void Tx_init_v29(struct START_PTRS *)
;* Initializes Tx_block for V29 modulator.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v29:					
_V29_MESI_TxInitV29:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;*	reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	 Tx_init_v29
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v29:
;* Initializes Tx_block[] workspace for v29
;****************************************************************************

Tx_init_v29:
V29_MESI_TxInitV29:
	MVDK	Tx_start_ptrs,AR0				;* AR0=start_ptrs
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
	ST		#Tx_v29_phase_map,Tx_map_ptr		
	ST		#Tx_v29_amp_table,Tx_amp_ptr		

	LD		*AR0(Tx_fir_start),B
	STL		B,Tx_fir_head					;* Tx_fir_head=&Tx_fir[0]
	STL		B,Tx_fir_tail					;* Tx_fir_tail=&Tx_fir[0]
	ST		#TX_FIR_LEN,Tx_fir_len

	MVDK	Tx_fir_head,AR0
	STM		#(TX_FIR_LEN-1),BRC
	RPTB	v29_init_Tx_fir_loop
v29_init_Tx_fir_loop:
	 STL	A,*AR0+							;* Tx_fir[*++]=0

	ST		#(TX_FIR_TAPS-1),Tx_fir_taps		
	STL		A,Tx_coef_ptr	
	STL		A,Tx_phase		
	STL		A,Tx_amp_acc;		
	STL		A,Tx_symbol_counter	
	STL		A,Tx_sample_counter	
	ST		#SCRAMBLER_SEED,Tx_Sreg			

	;**** rate-dependent	initialization	****

	LD		Tx_rate,A
	SUB		#V29_RATE_4800,A,B
	BC_		Tx_init_v29_elseif,BNEQ		
	ST		#TX_V29_SCALE_LOW,Tx_fir_scale;
	ST		#2,Tx_Nbits
	ST		#3h,Tx_Nmask
	B_		Tx_init_v29_endif
Tx_init_v29_elseif:
	SUB		#V29_RATE_7200,A,B
	BC_		Tx_init_v29_else,BNEQ		
	ST		#TX_V29_SCALE_MID,Tx_fir_scale;
	ST		#3,Tx_Nbits
	ST		#7h,Tx_Nmask
	B_		Tx_init_v29_endif
Tx_init_v29_else:
	ST		#TX_V29_SCALE_HIGH,Tx_fir_scale;
	ST		#4,Tx_Nbits
	ST		#0fh,Tx_Nmask
	ST		#V29_RATE_9600,Tx_rate;
Tx_init_v29_endif:

	ST		#TX_SEGMENT1_LEN,Tx_terminal_count
	STPP	#Tx_v29_segment1,Tx_state,B
	ST		#TX_V29_SEGMENT1_ID,Tx_state_ID	  

	;**** check for	TEP	enable	****

	LD		Tx_mode,B
	AND		#TX_TEP_FIELD,B
	RC_		BEQ								;* return if !TEP
	ST		#TX_TEP_LEN,Tx_terminal_count
	STPP	#Tx_v29_TEP,Tx_state,B
	RETD_
	 ST		#TX_V29_TEP_ID,Tx_state_ID		 

;****************************************************************************
;* Tx_v29_TEP: 200 msec. unmodulated carrier
;****************************************************************************

Tx_v29_TEP:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** generate TEP	tone	****

	LD		#0,A							;* amp=0
	STM		#18,AR7							;* phase=18
	CALL_	v29_abs_encoder	

	;**** check for	end	of	segment	****

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT	
	ST		#0,Tx_symbol_counter	
	ST		#0,Tx_sample_counter	
	ST		#TX_SEGMENT1_LEN,Tx_terminal_count
	STPP	#Tx_v29_segment1,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_SEGMENT1_ID,Tx_state_ID	  

;****************************************************************************
;* Tx_v29_segment1:	silence
;****************************************************************************

Tx_v29_segment1:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	MVDK	Tx_fir_len,BK
	 MVDK	Tx_fir_head,AR7					;*	AR7=Tx_fir_head
	ST		#0,*AR7+%
	ST		#0,*AR7+%
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if sample_counter<LEN
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
	ST		#0,Tx_phase		
	ST		#0,Tx_amp_acc	
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	BITF	Tx_mode,#TX_LONG_RESYNC_FIELD
	 ST		#TX_LONG_SEGMENT2_LEN,Tx_terminal_count	
	XC		2,TC							;* if mode != RESYNC ...
	 ST		#TX_RESYNC_SEGMENT2_LEN,Tx_terminal_count	
	STPP	#Tx_v29_segment2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_SEGMENT2_ID,Tx_state_ID	
	
;****************************************************************************
;* Tx_v29_segment2:	phase alternations
;****************************************************************************

Tx_v29_segment2:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** generate "ABAB..."	****

	LD		Tx_rate,B
	SUB		#V29_RATE_7200,B,A
	SUB		#V29_RATE_9600,B
	 STM	#0,AR6							;* amp=0
	 STM	#18,AR7							;* phase=18
	XC		2,BEQ
	 STM	#3,AR6							;* if V29_RATE_9600, amp=3
	XC		2,BEQ
	 STM	#21,AR7							;* if V29_RATE_9600, phase=21
	LD		Tx_symbol_counter,B
	AND		#1,B							;* B=Tx_symbol_counter&1
	XC		2,AEQ
	 STM	#2,AR6							;* if V29_RATE_7200, amp=2
	XC		2,AEQ
	 STM	#21,AR7							;* if V29_RATE_7200, phase=21
	XC		1,BNEQ
	 STM	#0,AR6							;* if sym_cntr&1!=0, amp=0
	XC		2,BNEQ
	 STM	#12,AR7							;* if sym_cntr&1!=0, phase=12
	LDM		AR6,A							;* A=amp
	CALL_	v29_abs_encoder	

	;**** check for	end	of	segment	****

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT	
	ST		#0,Tx_symbol_counter	
	ST		#0,Tx_sample_counter	
	BITF	Tx_mode,#TX_LONG_RESYNC_FIELD
	 ST		#TX_LONG_SEGMENT3_LEN,Tx_terminal_count	
	XC		2,TC							;* if mode != RESYNC ...
	 ST		#TX_RESYNC_SEGMENT3_LEN,Tx_terminal_count	
	STPP	#Tx_v29_segment3,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_SEGMENT3_ID,Tx_state_ID	

;****************************************************************************
;* Tx_v29_segment3:	Equalizer training pattern 
;****************************************************************************

Tx_v29_segment3:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** generate random	"CD"	equalizer pattern ****

	LD		Tx_rate,B
	SUB		#V29_RATE_9600,B
	 STM	#0,AR6							;* amp=0
	 STM	#6,AR7							;* phase=18
	XC		2,BEQ
	 STM	#3,AR6							;* if V29_RATE_9600, amp=3
	XC		2,BEQ
	 STM	#9,AR7							;* if V29_RATE_9600, phase=21
	LD		Tx_Sreg,-5,B					;* B=Sreg>>5
	XOR		B,-1,B							;* B=(Sreg>>5)^(Sreg>>6)
	AND		#1,B							;* B=((Sreg>>5)^(Sreg>>6))&1
	LD		Tx_Sreg,1,A						;* A=Sreg<<1
	OR		A,B
	LD		Tx_rate,A
	SUB		#V29_RATE_7200,A
	 STL	B,Tx_Sreg						;* Sreg=(Sreg<<1)|k
	 AND	#80h,B	
	XC		2,AEQ
	 STM	#2,AR6							;* if V29_RATE_7200, amp=2
	XC		2,AEQ
	 STM	#9,AR7							;* if V29_RATE_7200, phase=21
	XC		1,BEQ
	 STM	#0,AR6							;* if Sreg&0x80=0, amp=0
	XC		2,BEQ
	 STM	#0,AR7							;* if Sreg&0x80=0, phase=0
	LDM		AR6,A							;* A=amp
	CALL_	v29_abs_encoder	

	;**** check for	end	of	segment	****

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT
	ST		#0,Tx_Sreg			
	ST		#0,Tx_Sreg_low
	ST		#TX_SEGMENT4_LEN,Tx_terminal_count
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	STPP	#Tx_v29_segment4,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_SEGMENT4_ID,Tx_state_ID
			
;****************************************************************************
;* Tx_v29_segment4:	Scrambled	binary	ones
;****************************************************************************

Tx_v29_segment4:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	;**** generate scrambled	ones	****

	CALLD_	v29_scrambler
	 LD		Tx_Nmask,A
	 LD		Tx_Nbits,T
	LD		Tx_rate,A
	SUB		#V29_RATE_4800,A
	BC_		v29_scr1_endif,ANEQ				;* branch if !4800
	SFTL	B,1								;* B=symbol<<1
	 SUB	#6,B,A							;* A=symbol<<1-6
	 NOP
	XC		1,BEQ
	 LD		#1,B							;* if symbol=0, symbol=1
	XC		1,AEQ
	 LD		#7,B							;* if symbol=6, symbol=7
v29_scr1_endif:

	;**** differential	encoder	****

	CALLD_	v29_diff_encoder
	 LD		B,A								;* A=symbol
	 NOP

	;**** check for	end	of	segment	****

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT
	ST		#0,Tx_symbol_counter	
	ST		#0,Tx_sample_counter	
	ST		#-1,Tx_terminal_count	
	STPP	#Tx_v29_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_MESSAGE_ID,Tx_state_ID
	
;****************************************************************************
;* Tx_v29_message: data	segment
;****************************************************************************

Tx_v29_message:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BCD_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail
	 MVDK	Tx_data_tail,AR7
	
	;**** scramble data	****

	MVDK	Tx_data_len,BK
	CALLD_	v29_scrambler
	 LD		*AR7+%,A
	 LD		Tx_Nbits,T
	LD		Tx_rate,A
	SUB		#V29_RATE_4800,A
	BC_		v29_mes_endif,ANEQ				;* branch if !4800
	SFTL	B,1								;* B=symbol<<1
	 SUB	#6,B,A							;* A=symbol<<1-6
	 NOP
	XC		1,BEQ
	 LD		#1,B							;* if symbol=0, symbol=1
	XC		1,AEQ
	 LD		#7,B							;* if symbol=6, symbol=7
v29_mes_endif:
 
	;**** differential	encoder	****

	LD		B,A								;* A=symbol
	CALLD_	v29_diff_encoder
	 MVKD	AR7,Tx_data_tail

	;**** check terminal_count	****

	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if terminal_count<0
	SUB		Tx_symbol_counter,B
	BC_		Tx_state_return,BGEQ			;* return if symbol_counter<=LEN
	ST		#0,Tx_symbol_counter	
	ST		#0,Tx_sample_counter	
	ST		#TX_SEGMENT6_LEN,Tx_terminal_count	
	STPP	#Tx_v29_segment6,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V29_SEGMENT6_ID,Tx_state_ID		

;****************************************************************************
;* Tx_v29_segment6:	3	symbols	of	silence	to flush out remaining symbols
;****************************************************************************

Tx_v29_segment6:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	MVDK	Tx_fir_len,BK
	 MVDK	Tx_fir_head,AR7					;*	AR7=Tx_fir_head
	ST		#0,*AR7+%
	ST		#0,*AR7+%
	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT				;* return if symbol_counter<LEN
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
	CALL_	Tx_init_silence					;* switch to silence
	B_		Tx_state_return

;****************************************************************************
;* v29_abs_encoder:	
;* Absolute phase/amplitude	encoder	puts	the signalling element at the 
;* specified amplitude	and	phase	where:						
;*			amp is 2	bit	address	into	amp_mod_table				 
;*			 Tx_scale_table[4]={3/5,	5/5, sqrt(2)/5, 3*sqrt(2)/5}			
;*			phase is	24*(phase/2pi)	in	pi/12 increments			
;* Expects the following	on	entry:
;*	AR7=phase
;*	A=amp
;****************************************************************************

v29_abs_encoder:				
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STM		#TX_PHASE_LEN,BK;
	MVDK	Tx_carrier,AR0
	MVDK	Tx_phase,AR6
	MAR		*AR6+0%							;* AR6=(Tx_phase+Tx_carrier)%TX_PHASE_LEN
	MVKD	AR6,Tx_phase					;* update Tx_phase
	MVMM	AR7,AR0							;* AR0=phase
	MAR		*AR6+0%							;* AR6=(Tx_phase+phase)%TX_PHASE_LEN

	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	ADD		#Tx_v29_scale_table,A			;* A=&scale_table[amp]
	STLM	A,AR0
	 LDM	AR6,A							;* A=k
	 SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*k
	STLM	A,AR6							;* AR0=amp_ptr+2*phase
	 LD		Tx_fir_scale,T					;* T=scale
	 MPY	*AR0,A							;* A=scale_table*scale
	LD	*(AH),16,A
	MPYA	*AR6+							;* B=scale* *(Tx->amp_ptr+2*k)
	STH		B,*AR7+%
	MPYA	*AR6							;* B=scale* *(Tx->amp_ptr+2*k+1)
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
 .else
	STM		#TX_PHASE_LEN,BK;
	MVDK	Tx_carrier,AR0
	MVDK	Tx_phase,AR6
	MAR		*AR6+0%							;* AR6=(Tx_phase+Tx_carrier)%TX_PHASE_LEN
	MVKD	AR6,Tx_phase					;* update Tx_phase
	MVMM	AR7,AR0							;* AR0=phase
	MAR		*AR6+0%							;* AR6=(Tx_phase+phase)%TX_PHASE_LEN

	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	ADD		#Tx_v29_scale_table,A			;* A=&scale_table[amp]
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7							;* Tx_fir[]=scale_table[amp]
	LD		*AR7,T							;* T=scale_table[amp]

	LDM		AR6,A							;* A=k
	SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*k
	READA	*AR7+%							;* Tx_fir[]=*(Tx->amp_ptr+2*k)
	ADD		#1,A							;* A=amp_ptr+2*phase+1
	READA	*AR7-%							;* Tx_fir[]=*(Tx->amp_ptr+2*k+1)
	MPY		Tx_fir_scale,A					;* A=scale_table*scale
	MPYA	*AR7							;* B=scale* *(Tx->amp_ptr+2*k)
	STH		B,*AR7+%
	MPYA	*AR7							;* B=scale* *(Tx->amp_ptr+2*k+1)
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
 .endif

;****************************************************************************
;* v29_diff_encoder:	differentially	encodes the data bits for v29.
;* Expects the following	on	entry:
;*	A=B=symbol
;****************************************************************************

v29_diff_encoder:				
 .if ON_CHIP_COEFFICIENTS=ENABLED
	AND		#7,A,B							;* B=symbol&7
	ADDS	Tx_map_ptr,B					;* B=map_ptr+(symbol&7)
 	STLM	B,AR7							;* AR7=(map_ptr+symbol)
	 NOP
	 STM	#TX_PHASE_LEN,BK
	MVDK	*AR7,AR0						;* AR0=remap

	MVDK	Tx_amp_acc,AR6
	MAR		*AR6+0%							;* AR6=(amp_acc+remap)%TX_PHASE_LEN
	MVKD	AR6,Tx_amp_acc					;* update amp_acc
	
	MVDK	Tx_carrier,AR6
	MAR		*AR6+0%							;* AR6=(Tx_carrier+remap)%TX_PHASE_LEN
	MVDK	Tx_phase,AR0
	MAR		*AR6+0%							;* AR6=(phase+carrier+remap)%TX_PHASE_LEN
	MVKD	AR6,Tx_phase					;* update Tx_phase

	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	MVDK	Tx_fir_len,BK
	SFTL	A,-3,A							;* A=symbol>>3
	AND		#1,A							;* A=(symbol>>3)&1
	LD		Tx_amp_acc,1,B					;* B=amp_acc*2
	AND		#2,B							;* B=(amp_acc*2)&2
	OR		B,A								;* A=(amp_acc*2)&2|((symbol>>3)&1)
	ADD		#Tx_v29_scale_table,A
	STLM	A,AR0
	 LD		Tx_phase,1,A					;* A=2*Tx_phase
	 ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*phase
	STLM	A,AR6							;* AR6=amp_ptr+2*phase
	 LD		Tx_fir_scale,T					;* T=scale
	 MPY	*AR0,A							;* A=scale_table*scale
	MPYA	*AR6+							;* B=scale* *(Tx->amp_ptr+2*k)
	STH		B,*AR7+%
	MPYA	*AR6							;* B=scale* *(Tx->amp_ptr+2*k+1)
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
 .else
	AND		#7,A
	ADDS	Tx_map_ptr,A					;* A=map_ptr+(symbol&7)
	STM		AR0,AR7							;* AR7=&AR0
	READA	*AR7							;* AR0= remap=*(map_ptr+(symbol&7))
	STM		#TX_PHASE_LEN,BK
	 MVDK	Tx_amp_acc,AR6
	MAR		*AR6+0%							;* AR6=(amp_acc+remap)%TX_PHASE_LEN
	MVKD	AR6,Tx_amp_acc					;* update amp_acc
	
	MVDK	Tx_carrier,AR6
	MAR		*AR6+0%							;* AR6=(Tx_carrier+remap)%TX_PHASE_LEN
	MVDK	Tx_phase,AR0
	MAR		*AR6+0%							;* AR6=(phase+carrier+remap)%TX_PHASE_LEN
	MVKD	AR6,Tx_phase					;* update Tx_phase

	MVDK	Tx_fir_head,AR7					;* AR7=Tx_fir_head
	MVDK	Tx_fir_len,BK
	 SFTL	B,-3,A							;* A=symbol>>3
	AND		#1,A							;* A=(symbol>>3)&1
	LD		Tx_amp_acc,1,B					;* B=amp_acc*2
	AND		#2,B							;* B=(amp_acc*2)&2
	OR		B,A								;* A=(amp_acc*2)&2|((symbol>>3)&1)
	ADD		#Tx_v29_scale_table,A
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7
	LD		*AR7,T							;* T=scale_table[amp]

	LD		Tx_phase,1,A					;* A=2*Tx_phase
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*phase
	READA	*AR7+%							;* Tx_fir[]=*(Tx->amp_ptr+2*phase)
	ADD		#1,A							;* A=amp_ptr+2*phase+1
	READA	*AR7-%							;* Tx_fir[]=*(Tx->amp_ptr+2*phase+1)
	MPY		Tx_fir_scale,A					;* A=scale_table*scale
	MPYA	*AR7							;* B=scale* *(Tx->amp_ptr+2*k)
	STH		B,*AR7+%
	MPYA	*AR7							;* B=scale* *(Tx->amp_ptr+2*k+1)
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head					;* update Tx_fir_head
 .endif


;****************************************************************************
;* v29_scrambler: V29	scrambler	
;* Expects the following	on	entry:
;*	A=in
;*	T=Nbits
;* On exit:
;*	B=out
;*	A=Sreg
;****************************************************************************

v29_scrambler:					
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
 .endif

	;**************************
	;**** receiver modules	****
	;**************************

 .if RX_V29_MODEM=ENABLED
;****************************************************************************
;* _Rx_init_v29:
;* C function call:	void	Rx_init_v29(struct START_PTRS *)
;* Initializes Rx_block	for	V29	demodulator.
;****************************************************************************

 .if COMPILER=ENABLED
_Rx_init_v29:					
_V29_MESI_RxInitV29:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;*	reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	 Rx_init_v29
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_v29:
;* Initializes Rx_block[]	workspace	for	v29 demodulator operation.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_v29:					
V29_MESI_RxInitV29:
	MVDK	Rx_start_ptrs,AR0			
	LD		#0,A
	ST		#_Rx_timing2400,Rx_timing_start	
	ST		#_RCOS2400_f1800,Rx_coef_start	
	ST		#RX_DEC,Rx_decimate
	ST		#(2*RX_INTERP),Rx_interpolate
	ST		#(RX_DEC/2),Rx_sym_clk_phase	
	ST		#RX_OVERSAMPLE,Rx_oversample
	LD		Rx_data_head,B
	STL		B,Rx_data_tail
	STPP	#v29_diff_decoder,decoder_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	STL		A,Rx_coef_ptr		

	ST		#(RX_FIR_TAPS-1),Rx_fir_taps	
	STL		A,EQ_MSE		
	ST		#EQ_DISABLED,EQ_2mu	
	ST		#(V29_EQ_LEN-1),EQ_taps
	MVDK	*AR0(Rx_fir_start),AR3
	MVKD	AR3,Rx_fir_ptr			
	STM		#(RX_FIR_LEN-1),BRC
	RPTB	v29_init_Rx_fir_loop
v29_init_Rx_fir_loop:
	 STL	A,*AR3+				

	BITF	Rx_mode,#RX_LONG_RESYNC_FIELD
	BC_		v29_init_EQ_coef_end,TC			;* branch if != LONG_TRAIN mode
	MVDK	*AR0(EQ_coef_start),AR3
	STM		#(2*V29_EQ_LEN-1),BRC
	RPTB	v29_init_EQ_coef_loop
v29_init_EQ_coef_loop:
	 STL	A,*AR3+				
	ST		#EQ_DISABLED,EQ_2mu	
v29_init_EQ_coef_end:
	
	ST		#V29_ACQ_LOOP_K1,loop_K1
	ST		#V29_ACQ_LOOP_K2,loop_K2
	STL		A,PJ1_coef						;* disable PJ1 resonator
	STL		A,PJ2_coef                      ;* disable PJ2 resonator
	ST		#ACQ_AGC_K,agc_K		
	STL		A,Rx_sym_clk_memory		
	ST		#ACQ_TIMING_THR,timing_threshold	
	STL		A,coarse_error		
	ST		#(V29_EQ_LEN/2),Rx_baud_counter	
	STL		A,Rx_sample_counter	
	STL		A,Rx_symbol_counter	
	STL		A,LO_memory					
	STL		A,loop_memory_low
	STL		A,vco_memory	
	STL		A,loop_memory	
	STL		A,frequency_est
	STL		A,LOS_counter	
	STL		A,LOS_monitor	
	ST		#WHAT,What		
	STL		A,Rx_Dreg		
	STL		A,Rx_Dreg_low
	STL		A,Rx_pattern_reg
	STL		A,data_Q1
	STL		A,Rx_map_shift
	STL		A,hard_sym_nm1

	ST		#RX_V29_CARRIER_FREQ,LO_frequency					
	ST		#LO_PHASE_ADJ,LO_phase					
	STL		A,Ihat
	STL		A,Qhat
	STL		A,frequency_est

	;**** rate-dependent initialization ****

	STPP	#v29_slicer96,slicer_ptr,B
	ST		#4,Rx_Nbits
	ST		#0fh,Rx_Nmask
	CMPM	Rx_rate,#V29_RATE_7200
	BC_		Rx_init_v29_elseif1,NTC
	STPP	#v29_slicer72,slicer_ptr,B
	ST		#3,Rx_Nbits
	ST		#7,Rx_Nmask
Rx_init_v29_elseif1:
 .if (RX_V29_MODEM_4800=ENABLED)
	CMPM	Rx_rate,#V29_RATE_4800
	BC_		Rx_init_v29_endif1,NTC
	STPP	#v29_slicer48,slicer_ptr,B
	ST		#2,Rx_Nbits
	ST		#3,Rx_Nmask
	ST		#-1,Rx_map_shift
 .endif
Rx_init_v29_endif1:

	CALL_	agc_gain_estimator
	MPY		agc_gain,#COS_PI_BY_4,B
	STH		B,agc_gain
	MVKD	AR2,Rx_sample_tail
	MVKD	AR2,Rx_sample_ptr

	;**** switch states	to	train_loops	****

	ST		#0,Rx_status
	STPP	#Rx_v29_train_loops,Rx_state,B
	RETD_
	 ST		#RX_V29_TRAIN_LOOPS_ID,Rx_state_ID		

;****************************************************************************
;* Rx_v29_train_loops:	carrier,symbol,agc	loop training.
;****************************************************************************

Rx_v29_train_loops:
	CALL_	Rx_train_loops
	LD		Rx_symbol_counter,B
	BCD_	Rx_state_return,AEQ				;* return if A=0
	 SUB	#2,A
	BCD_	loops_stable_detected,AEQ		;* branch if return=2
	 SUB	#TRAIN_LOOPS_TIMEOUT,B			;* Rx_symbol_counter-TRAIN_LOOPS_TIMEOUT

	;**** check for	timeout	****

	BC_		Rx_state_return,BLEQ			;* branch if symbol_counter<=TIMEOUT
	CALL_	 Rx_init_detector
	BD_		Rx_state_return
	 ST		#TRAIN_LOOPS_FAILURE,Rx_status	;* set status to FAILURE

loops_stable_detected:
	STPP	#Rx_v29_detect_EQ,Rx_state,B
	BD_		Rx_state_return
	  ST	#RX_V29_DETECT_EQ_ID,Rx_state_ID	

;****************************************************************************
;* Rx_v29_detect_EQ: wait for start of training
;****************************************************************************

Rx_v29_detect_EQ:
	CALL_	Rx_detect_EQ
	LD		Rx_symbol_counter,B
	BCD_	Rx_state_return,AEQ				;* return if A=0
	 SUB	#2,A
	BCD_	Rx_EQ_detected,AEQ			 	;* branch if return=2
	 SUB	#START_EQ_TIMEOUT,B				;* Rx_symbol_counter-START_EQ_TIMEOUT

	;**** check for	timeout	****

	BC_		start_EQ_timeout_endif,BLEQ	 	;* branch if symbol_counter<=TIMEOUT
	CALL_	Rx_init_detector
	ST		#START_EQ_FAILURE,Rx_status		;* set status to FAILURE
	B_		Rx_state_return
start_EQ_timeout_endif:

	;**** closed-eye detection ****

	LD		Ihat,A
	SUB		Ihat_nm2,A
	ADD		Qhat,A
	SUB		Qhat_nm2,A
	 LD		#1,B
	 ADD	Rx_sample_counter,B
	XC		1,ANEQ							;* if Ihat-Ihat_nm2+Qhat-Qhat_nm2!=0
	 LD		#0,B
	SUB		EQ_taps,-2,B,A				 	;* sample_counter-EQ_taps>>2
	BCD_	CW_detect_endif,ALEQ
	 STL	B,Rx_sample_counter
	 LDU	vco_memory,B
	ADD		#FOURTY_FIVE_DEGREES,B
	STL		B,vco_memory					;* vco_memory+=FOURTY_FIVE_DEGREES
	ST		#0,coarse_error				 	;* coarse_error=0
	ST		#0,Rx_sample_counter			;* sample_counter=0
CW_detect_endif:

	;**** coarse symbol timing error calculation ****

	LD		Iprime,A
	SUB		Inm2,A						 	;* A=Iprime-Inm2
	ABS		A,A								;* A=abs(Iprime-Inm2)
	LD		Inm1,B
	SUB		Inm3,B						 	;* B-=Inm3
	ABS		B,B								;* B=abs(Inm1-Inm3)
	SUB		B,A								;* A=abs(Iprime-Inm2)-abs(Inm1-Inm3)
	LD		Qprime,B
	SUB		Qnm2,B						 	;* B=Qprime-Qnm2
	ABS		B,B								;* B=abs(Qprime-Qnm2)
	ADD		B,A								;* A=abs(Qprime-Qnm2)
	LD		Qnm1,B
	SUB		Qnm3,B						 	;* B=Qnm1
	ABS		B,B								;* B=abs(Qnm1-Qnm3)
	SUB		B,A								;* A-=abs(Qnm1-Qnm3)
;++++#ifndef MESI_INTERNAL 03-13-2001 OP_POINT8 MODS
	SFTA	A,-4,A							;* A>>=4
;++++#endif  MESI_INTERNAL 03-13-2001 OP_POINT8 MODS 
	ADD		coarse_error,A				 	;* A+=coarse_error
	STL		A,coarse_error				 	;* update coarse_error

	SUB		#COARSE_THR,A,B					;* B=A-COARSE_THR
	 ADD	#COARSE_THR,A				 	;* A=A+COARSE_THR
	XC		2,BGT
	 ST		#0,coarse_error					;* if error>THR, coarse_error=0
	XC		2,ALT
	 ST		#0,coarse_error					;* if error<-THR, coarse_error=0
	XC		2,ALT
	 ST		#1,Rx_baud_counter			 	;* if error<-THR, baud_counter=1
	B_		Rx_state_return

Rx_EQ_detected:
	BITF	Rx_mode,#RX_LONG_RESYNC_FIELD
	BC_		Rx_EQ_detected_endif,TC		;* branch if not LONG_TRAIN	
	STPP	#v29_EQslicer96,slicer_ptr,A
	CMPM	Rx_rate,#V29_RATE_7200
	BC_		detect_EQ_elseif,NTC
	STPP	#v29_EQslicer72,slicer_ptr,A
detect_EQ_elseif:
 .if (RX_V29_MODEM_4800=ENABLED)
	CMPM	Rx_rate,#V29_RATE_4800
	BC_		detect_EQ_endif1,NTC
	STPP	#v29_EQslicer48,slicer_ptr,A
 .endif
detect_EQ_endif1:
	STPP	#no_timing,timing_ptr,B
	ST		#0,agc_K			
	ST		#ACQ_EQ_2MU,EQ_2mu	
Rx_EQ_detected_endif:

	ST		#EQ_TRAIN_SEED,Rx_Dreg	
	ST		#0,Rx_symbol_counter	
	STPP	#Rx_v29_train_EQ,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V29_TRAIN_EQ_ID,Rx_state_ID

;****************************************************************************
;* Rx_v29_train_EQ:	equalizer	training.
;****************************************************************************

Rx_v29_train_EQ:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BC_		Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail
	
	LDU		Rx_data_head,B
	STL		B,Rx_data_tail					;* data_tail=data_head
	
	;**** odd octant detection and correction ****
	
	LD		Rx_pattern_reg,3,B				;* B=pattern_reg<<3
	OR		Phat,B
	STL		B,Rx_pattern_reg				;* pattern_reg=(pattern_reg<<3)|Phat
	LD		#0,A							;* i=0
	CMPM	Rx_pattern_reg,#1861h
	BCD_	Rx_v29_train_EQ_endif1,NTC
	 CMPM	Rx_pattern_reg,#3cf3h
	LD		#1,A							;* i=1	
Rx_v29_train_EQ_endif1:
	BCD_	Rx_v29_train_EQ_endif2,NTC
	 CMPM	Rx_pattern_reg,#5145h
	LD		#1,A							;* i=1	
Rx_v29_train_EQ_endif2:
	BCD_	Rx_v29_train_EQ_endif3,NTC
	 CMPM	Rx_pattern_reg,#75d7h
	LD		#1,A							;* i=1	
Rx_v29_train_EQ_endif3:
	BCD_	Rx_v29_train_EQ_endif4,NTC
	 LD		#8192,B
	LD		#1,A							;* i=1	
Rx_v29_train_EQ_endif4:
	
	ADDS	vco_memory,B	 				;* B=vco_memory+8192
	XC		1,ANEQ							;* if 1!=0 ...
	 STL	B,vco_memory					;* ... vco_memory+=8192
	
	;**** check for end of training segment ****
	 
	LD		Rx_symbol_counter,B
	BITF	Rx_mode,#RX_LONG_RESYNC_FIELD
	BCD_	Rx_v29_train_EQ_else,TC			;* branch if mode!=LONG
	 SUB	#RX_LONG_SEGMENT3_LEN,B,A
	BC_		Rx_state_return,ALT				;* return if counter<SEGMENT_LEN
	B_		Rx_v29_train_EQ_endif5
Rx_v29_train_EQ_else:
	SUB		#RX_RESYNC_SEGMENT3_LEN,B,A
	BC_		Rx_state_return,ALT				;* return if counter<SEGMENT_LEN
;++++#ifndef MESI_INTERNAL 03-06-2001
;++++#else   MESI_INTERNAL 03-06-2001
;++++#endif  MESI_INTERNAL 03-06-2001
	B_		Rx_v29_train_EQ_endif5
Rx_v29_train_EQ_endif5:

	;**** rotate constellation if needed at 4800 ****
	
 .if (RX_V29_MODEM_4800=ENABLED)
	CMPM	Rx_rate,#V29_RATE_4800
	BC_		Rx_v29_train_EQ_endif6,NTC		;* branch if rate!=4800
	BITF	Rx_pattern_reg,#1
	BCD_	Rx_v29_train_EQ_endif6,NTC		;* branch if pattern_reg&1==0
	 LD		#8192,B
	ADDS	vco_memory,B	 				;* B=vco_memory+8192
	STL		B,vco_memory					;* vco_memory+=8192
Rx_v29_train_EQ_endif6:
 .endif

	;**** switch to	decision-based slicer ****

	STPP	#v29_slicer96,slicer_ptr,A
	LD		Rx_rate,B
	SUB		#V29_RATE_7200,B,A
	BC_		train_EQ_elseif,ANEQ
	STPP	#v29_slicer72,slicer_ptr,A
train_EQ_elseif:
 .if (RX_V29_MODEM_4800=ENABLED)
	SUB		#V29_RATE_4800,B
	BC_		train_EQ_endif,BNEQ
	STPP	#v29_slicer48,slicer_ptr,A
train_EQ_endif:
 .endif

	LD		#0,A
	STPP	#APSK_timing,timing_ptr,B
	ST		#TRK_TIMING_THR,timing_threshold	
	STL		A,Rx_sym_clk_memory				;* sym_clk_memory=0
	ST		#TRK_EQ_2MU,EQ_2mu				
	ST		#V29_TRK_LOOP_K1,loop_K1		 
	ST		#V29_TRK_LOOP_K2,loop_K2		 
	STL		A,Rx_Dreg						;* Dreg=0
	STL		A,Rx_Dreg_low					;* Dreg_low=0
	STL		A,Rx_symbol_counter				;* symbol_counter=0
	STPP	#Rx_v29_scr1,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V29_SCR1_ID,Rx_state_ID

;****************************************************************************
;* Rx_v29_scr1: v29	scr1 data.
;****************************************************************************

Rx_v29_scr1:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_	Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail
	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_tail,AR4				;* AR4=Rx_data_head

	;**** run descrambler and wait for end of SCR1 ****

	CALLD_	v29_descrambler
	 LD		*AR4,A
	 LD		Rx_Nbits,T
	STL		B,*AR4+%
	LD		Rx_symbol_counter,B
	SUB		#SCR1_TIMEOUT,B			
	BCD_	Rx_state_return,BLT				;* return if counter<TIMEOUT
	 MVKD	AR4,Rx_data_tail				;* update Rx_data_tail

	;**** end of SCR1 detected, switch to message ****

	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
	ST		#0,Rx_symbol_counter	
	STPP	#Rx_v29_message,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_V29_MESSAGE_ID,Rx_state_ID

;****************************************************************************
;* Rx_v29_message: v29 message data.
;****************************************************************************

Rx_v29_message:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_ptr,B
	BCD_	Rx_state_return,BEQ				;* branch if Rx_data_head==Rx_data_tail
	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_head,AR0				;* AR0=Rx_data_head
	LD		LOS_monitor,B
	SUB		#UNLOCKED,B
	BCD_	v29_mes_while_loop,BNEQ
	 MVDK	Rx_data_ptr,AR4					;*	AR4=Rx_data_ptr
	CALL_	 Rx_init_detector
	ST		#LOSS_OF_LOCK,Rx_status			;* set status to LOSS_OF_LOCK
	B_		Rx_state_return

v29_mes_while_loop:
	CALLD_	v29_descrambler
	 LD		*AR4,A
	 LD		Rx_Nbits,T
	STL		B,*AR4+%
	CMPR	1,AR4							;* Rx_data_ptr-Rx_data_head
	BC_		v29_mes_while_loop,TC				;* branch if data_tail!=data_ptr
	BD_		Rx_state_return			
	 MVKD	AR4,Rx_data_ptr					;*	update Rx_data_ptr

;****************************************************************************
;* v29_descrambler:	V29	descrambler
;* Expects the following	on	entry:
;*	A=in
;*	T=Nbits
;* On exit:
;*	 B=out
;****************************************************************************

v29_descrambler:
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

;****************************************************************************
;* v29_slicer48: slicer	for	4800	bits/sec.
;****************************************************************************

 .if (RX_V29_MODEM_4800=ENABLED)
v29_slicer48:
	STM		#Rx_v29_hard_map,AR5			;* AR5=&Rx_v29_hard_map[k]
	ST		#WHAT48_30,What
	LD		Iprime,A
	ABS		A
	LD		Qprime,B
	ABS		B
	SUB		B,A
	BC_		slicer48_onI,ALEQ				;* branch if abs(Iprime)<=abs(Qprime)
	LD		Iprime,B
	 ST		#0,Qhat			
	 ST		#-SLICE48_3,Ihat			
	XC		2,BGEQ
	 ST		#SLICE48_3,Ihat					;* if Iprime>=0, Ihat=SLICE48_3
	XC		2,BLT
	 MAR	*+AR5(6)						;* if Iprime<0, j+=6
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer48_onI:
	LD		Qprime,B
	 ST		#0,Ihat			
	 ST		#-SLICE48_3,Qhat			
	XC		2,BGEQ
	 ST		#SLICE48_3,Qhat					;* if Qprime>=0, Qhat=SLICE48_3
	MAR		*+AR5(3)
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Qprime<0, j+=5
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif
 .endif

;****************************************************************************
;* v29_EQslicer48: equalizer	assisted	slicer for 4800 bits/sec.
;****************************************************************************

 .if (RX_V29_MODEM_4800=ENABLED)
v29_EQslicer48:
	LD		Rx_Dreg,-5,B					;* B=Dreg>>5
	XOR		B,-1,B							;* B=(Dreg>>5)^(Dreg)>>6
	AND		#1,B
	LD		Rx_Dreg,1,A						;* A=Dreg<<1
	OR		A,B
	STL		B,Rx_Dreg						;* Dreg=(Dreg<<1)|k
	AND		#80h,B
	ST		#SLICE48_3,Ihat
	ST		#0,Qhat
	BCD_		slicer_return,BEQ
	 ST		#WHAT48_30,What
	ST		#0,Ihat
	BD_		slicer_return
	 ST		#SLICE48_3,Qhat
 .endif

;****************************************************************************
;* v29_slicer72: slicer	for	7200	bits/sec.
;****************************************************************************

v29_slicer72:
	STM		#Rx_v29_hard_map,AR5			;* AR5=&Rx_v29_hard_map[k]
	ST		#0,data_Q1
	LD		Iprime,A
	ABS		A
	SUB		#SLICE72_2,A,B					;* abs(Iprime)-SLICE72_2
	BC_		slicer72_onQ,BGEQ				;* branch if abs(Iprime)>=SLICE72_2
	LD		Qprime,B
	ABS		B
	SUB		#SLICE72_2,B					;* abs(Qprime)-SLICE72_2
	BC_		slicer72_onQ,BGEQ				;* branch if abs(Qprime)>=SLICE72_2
	LD		Iprime,A
	LD		Qprime,B
	 ST		#-SLICE72_1,Ihat			
	 ST		#-SLICE72_1,Qhat			
	XC		2,AGEQ
	 ST		#SLICE72_1,Ihat					;* if Iprime>=0, Ihat=SLICE72_1
	XC		2,BGEQ
	 ST		#SLICE72_1,Qhat					;* if Qprime>=0, Qhat=SLICE72_1
	XC		2,ALT
	 MAR	*+AR5(2)						;* if Iprime<0, j+=2
	LD		Iprime,T
	MPY		Qprime,A
	ST		#WHAT72_11,What
	XC		2,BLT
	 MAR	*+AR5(4)						;* if Qprime<0, j+=4
	XC		2,AGEQ
	 MAR	*+AR5(1)						;* if Iprime*Qprime>=0, j+=1
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer72_onQ:
	LD		Qprime,B
	ABS		B
	SUB		A,B								;* abs(Qprime)-abs(Iprime)
	BC_		slicer72_onI,BLEQ				;* branch if abs(Qprime)<=abs(Iprime)	
	LD		Qprime,B
	 ST		#0,Ihat			
	 ST		#-SLICE72_3,Qhat			
	XC		2,BGEQ
	 ST		#SLICE72_3,Qhat					;* if Qprime>=0, Qhat=SLICE72_3
	ST		#WHAT72_30,What
	MAR		*+AR5(3)
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Qprime<0, j+=5
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer72_onI:
	LD		Iprime,B
	 ST		#0,Qhat			
	 ST		#-SLICE72_3,Ihat			
	XC		2,BGEQ
	 ST		#SLICE72_3,Ihat					;* if Iprime>=0, Ihat=SLICE72_3
	ST		#WHAT72_30,What
	XC		2,BLT
	 MAR	*+AR5(6)						;* if Iprime<0, j+=6
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

;****************************************************************************
;* v29_EQslicer72: equalizer	assisted	slicer for 7200 bits/sec.
;****************************************************************************

v29_EQslicer72:
	LD		Rx_Dreg,-5,B					;* B=Dreg>>5
	XOR		B,-1,B							;* B=(Dreg>>5)^(Dreg)>>6
	AND		#1,B
	LD		Rx_Dreg,1,A						;* A=Dreg<<1
	OR		A,B
	STL		B,Rx_Dreg						;* Dreg=(Dreg<<1)|k
	AND		#80h,B
	ST		#SLICE72_3,Ihat
	ST		#0,Qhat
	BCD_		slicer_return,BEQ
	 ST		#WHAT72_30,What
	ST		#-SLICE72_1,Ihat
	ST		#SLICE72_1,Qhat
	BD_		slicer_return
	 ST		#WHAT72_11,What

;****************************************************************************
;* v29_slicer96: slicer	for	9600	bits/sec.
;****************************************************************************

v29_slicer96:
	STM		#Rx_v29_hard_map,AR5			;* AR5=&Rx_v29_hard_map[k]
	LD		Iprime,A
	ABS		A
	SUB		#SLICE96_4,A,B					;* abs(Iprime)-SLICE96_4
	BC_		slicer96_step2,BLEQ				;* branch if abs(Iprime)<=SLICE96_4
	LD		Qprime,B
	ABS		B
	SUB		B,A								;* abs(Iprime)-abs(Qprime)
	BC_		slicer96_step2,ALEQ				;* branch if abs(Iprime)<=abs(Qprime)
	LD		Iprime,B
	 ST		#0,Qhat			
	 ST		#-SLICE96_5,Ihat			
	XC		2,BGEQ
	 ST		#SLICE96_5,Ihat					;* if Iprime>=0, Ihat=SLICE96_5
	ST		#WHAT96_50,What
	ST		#8,data_Q1
	XC		2,BLT
	 MAR	*+AR5(6)						;* if Iprime<0, j+=6
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer96_step2:					
	LD		Qprime,A
	ABS		A
	SUB		#SLICE96_4,A,B					;* abs(Qprime)-SLICE96_4
	BC_		slicer96_step3,BLEQ				;* branch if abs(Qprime)<=SLICE96_4
	LD		Iprime,B
	ABS		B
	SUB		B,A								;* abs(Qprime)-abs(Iprime)
	BC_		slicer96_step3,ALEQ				;* branch if abs(Qprime)<=abs(Iprime)
	LD		Qprime,B
	 ST		#0,Ihat			
	 ST		#-SLICE96_5,Qhat			
	XC		2,BGEQ
	 ST		#SLICE96_5,Qhat					;* if Qprime>=0, Qhat=SLICE96_5
	ST		#WHAT96_50,What
	ST		#8,data_Q1
	MAR		*+AR5(3)
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Qprime<0, j+=5
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer96_step3:					
	LD		Iprime,A
	ABS		A
	SUB		#SLICE96_2,A,B					;* abs(Iprime)-SLICE96_2
	BC_		slicer96_step4,BGEQ				;* branch if abs(Iprime)>=SLICE96_2
	LD		Qprime,B
	ABS		B
	SUB		#SLICE96_2,B					;* abs(Qprime)-SLICE96_2
	BC_		slicer96_step4,BGEQ				;* branch if abs(Qprime)>=SLICE96_2
	LD		Iprime,A
	LD		Qprime,B
	 ST		#-SLICE96_1,Ihat			
	 ST		#-SLICE96_1,Qhat			
	XC		2,AGEQ
	 ST		#SLICE96_1,Ihat					;* if Iprime>=0, Ihat=SLICE96_1
	XC		2,BGEQ
	 ST		#SLICE96_1,Qhat					;* if Qprime>=0, Qhat=SLICE96_1
	XC		2,ALT
	 MAR	*+AR5(2)						;* if Iprime<0, j+=2
	XC		2,BLT
	 MAR	*+AR5(4)						;* if Qprime<0, j+=4
	LD		Iprime,T
	MPY		Qprime,A
	 ST		#WHAT96_11,What
	 ST		#0,data_Q1
	XC		2,AGEQ
	 MAR	*+AR5(1)						;* if Iprime*Qprime>=0, j+=1
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer96_step4:					
	LD		Iprime,A
	ABS		A
	SUB		#SLICE96_134,A					;* abs(Iprime)-SLICE96_134
	BC_		slicer96_step5,AGEQ				;* branch if abs(Iprime)>=SLICE96_134
	LD		Qprime,B
	 ST		#0,Ihat			
	 ST		#-SLICE96_3,Qhat			
	XC		2,BGEQ
	 ST		#SLICE96_3,Qhat					;* if Qprime>=0, Qhat=SLICE96_3
	ST		#WHAT96_30,What
	ST		#0,data_Q1
	MAR		*+AR5(3)
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Qprime<0, j+=5
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer96_step5:					
	LD		Qprime,A
	ABS		A
	SUB		#SLICE96_134,A					;* abs(Qprime)-SLICE96_134
	BC_		slicer96_step6,AGEQ				;* branch if abs(Qprime)>=SLICE96_134
	LD		Iprime,B
	 ST		#0,Qhat			
	 ST		#-SLICE96_3,Ihat			
	XC		2,BGEQ
	 ST		#SLICE96_3,Ihat					;* if Iprime>=0, Ihat=SLICE96_3
	ST		#WHAT96_30,What
	ST		#0,data_Q1
	XC		2,BLT
	 MAR	*+AR5(6)						;* if Iprime<0, j+=6
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

slicer96_step6:					
	LD		Iprime,A
	LD		Qprime,B
	 ST		#-SLICE96_3,Ihat			
	 ST		#-SLICE96_3,Qhat			
	XC		2,AGEQ
	 ST		#SLICE96_3,Ihat					;* if Iprime>=0, Ihat=SLICE96_3
	XC		2,BGEQ
	 ST		#SLICE96_3,Qhat					;* if Qprime>=0, Qhat=SLICE96_3
	XC		2,ALT
	 MAR	*+AR5(2)						;* if Iprime<0, j+=2
	XC		2,BLT
	 MAR	*+AR5(4)						;* if Qprime<0, j+=4
	LD		Iprime,T
	MPY		Qprime,A
	 ST		#WHAT96_33,What
	 ST		#8,data_Q1
	XC		2,AGEQ
	 MAR	*+AR5(1)						;* if Iprime*Qprime>=0, j+=1
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v29_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v29_hard_map[k]
 .else
	 LDM	AR5,A							;* A=&Rx_v29_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v29_hard_map[k]
 .endif

;****************************************************************************
;* v29_EQslicer96: equalizer	assisted	slicer for 9600 bits/sec.
;****************************************************************************

v29_EQslicer96:
	LD		Rx_Dreg,-5,B					;* B=Dreg>>5
	XOR		B,-1,B							;* B=(Dreg>>5)^(Dreg)>>6
	AND		#1,B
	LD		Rx_Dreg,1,A						;* A=Dreg<<1
	OR		A,B
	STL		B,Rx_Dreg						;* Dreg=(Dreg<<1)|k
	AND		#80h,B
	ST		#SLICE96_3,Ihat
	ST		#0,Qhat
	BCD_		slicer_return,BEQ
	 ST		#WHAT96_30,What
	ST		#-SLICE96_3,Ihat
	ST		#SLICE96_3,Qhat
	BD_		slicer_return
	 ST		#WHAT96_33,What

;****************************************************************************
;* v29_diff_decoder:	hard	decision	symbol differential decoder for v29
;* On entry, it expects	the	following:
;*	AR7=Rx_data_head
;*	BK=Rx_data_len
;*	A=Phat
;****************************************************************************

v29_diff_decoder:
 .if ON_CHIP_COEFFICIENTS=ENABLED
	ADD		#8,A							;* A=Phat+8
	SUB		Rx_phase,A						;* A=Phat+8-Rx_phase
	AND		#7,A							;* A&=7
	ADD		#Rx_v29_phase_map,A				;* A=&Rx+phase_map[(Phat+8-Rx_phase&7]
	STLM	A,AR0	
	 LD		Phat,B
	 STL	B,Rx_phase						;* Rx_phase=Phat
	LD		*AR0,A							;* A=Rx_v29_phase_map[*]
	OR		data_Q1,A						;* A=Rx_v29_phase_map[*]|data_Q1
	STL		A,temp0
	LD		Rx_map_shift,T
	LD		temp0,TS,A						;* A=Rx_v29_phase_map[*]>>map_shift
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_v29_phase_map[*]
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .else										
	ADD		#8,A							;* A=Phat+8
	SUB		Rx_phase,A						;* A=Phat+8-Rx_phase
	AND		#7,A							;* A&=7
	ADD		#Rx_v29_phase_map,A				;* A=&Rx+phase_map[(Phat+8-Rx_phase&7]
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7							;* *AR7=Rx_v29_phase_map[*]
	LD		Rx_map_shift,T
	LD		*AR7,A
	OR		data_Q1,A						;* A=Rx_v29_phase_map[*]|data_Q1
	STL		A,*AR7				
	LD		*AR7,TS,A						;* R0=Rx_v29_phase_map[*]>>map_shift
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_v29_phase_map[*]
	LD		Phat,B
	STL		B,Rx_phase						;* Rx_phase=Phat
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .endif										

;****************************************************************************
 .endif

;****************************************************************************
	.end
