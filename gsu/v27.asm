;****************************************************************************
;* Filename: v27.asm
;* Date: 04-01-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for V.27ter.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"v27.inc"

V27_RATE_2400		  		.set	2400	 
V27_RATE_4800		  		.set	4800	 

	;**** modulator ****

TX_FIR_TAPS24		  		.set	3
TX_INTERP24					.set	2*DEC1200 
TX_DEC24					.set	INTERP1200
TX_COEF24_LEN		  		.set	(TX_FIR_TAPS24*TX_INTERP24+TX_DEC24)	
TX_V27_SCALE24		 		.set	24572   ;* scaled for Rx_coef[] 
TX_CARRIER24				.set	2

TX_FIR_TAPS48		  		.set	3
TX_INTERP48					.set	2*DEC1600
TX_DEC48					.set	INTERP1600
TX_COEF48_LEN		  		.set	(TX_FIR_TAPS48*TX_INTERP48+TX_DEC48)		
TX_V27_SCALE48		 		.set	18426   ;* scaled for Rx_coef[] 
TX_CARRIER48				.set	1

SCRAMBLER_SEED		 		.set	3ch
TX_SEGMENT1_LEN				.set	1600	;* 200 msec carrier
TX_SEGMENT2_LEN				.set	160	 	;* 20 msec. silence 
TX_SEGMENT3_LEN				.set	50	  	;* 50 symbols of ACAC...(180 deg revs)
TX_LONG_SEGMENT3_LEN		.set	50	  	;* 50 symbols of ACAC...(180 deg revs)
TX_SHORT_SEGMENT3_LEN  		.set	14	  	;* 14 symbols of ACAC...(180 deg revs)
TX_SEGMENT4_LEN				.set	1074	;* 1074 scrambled 2-phase Eq pattern
TX_LONG_SEGMENT4_LEN		.set	1074	;* 1074 scrambled 2-phase Eq pattern
TX_SHORT_SEGMENT4_LEN  		.set	58	  	;* 58 scrambled 2-phase Eq pattern
TX_SEGMENT5_LEN				.set	8	   	;* 8 symbols scrambled 1
TX_SEGMENTA_LEN				.set	8	   	;* 8 symbols scrambled 1
TX_SEGMENTB_LEN				.set	160	 	;* 20 msec. silence

 .if $isdefed("XDAIS_API")
	.global _V27_MESI_TxInitV27					
	.global V27_MESI_TxInitV27
 .else
	.global _Tx_init_v27					
	.global Tx_init_v27
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

;++++#ifndef MESI_INTERNAL 03-09-2001 OP_POINT8 MODS
;RX_FIR_TAPS24		  		.set	TAPS1200
;RX_OVERSAMPLE24				.set	OVERSAMPLE1200
;RX_INTERP24					.set	INTERP1200
;RX_DEC24					.set	DEC1200
;RX_COEF24_LEN		  		.set	(RX_FIR_TAPS24*RX_INTERP24+RX_DEC24)
;RX_COEF_SAMPLE_RATE24  		.set	(8000*RX_INTERP24)
;RX_V27_CARRIER_FREQ24  		.set	49152   ;* (1800*65536/2400)
;LO_PHASE_ADJ24		 		.set	1229	;* (65536/8000)*(1800/RX_INTERP24)
;
;RX_FIR_TAPS48		  		.set	TAPS1600
;RX_OVERSAMPLE48				.set	OVERSAMPLE1600
;RX_INTERP48					.set	INTERP1600
;RX_DEC48					.set	DEC1600
;RX_COEF48_LEN		  		.set	(RX_FIR_TAPS48*RX_INTERP48+RX_DEC48)
;RX_COEF_SAMPLE_RATE48  		.set	(8000*RX_INTERP48)
;RX_V27_CARRIER_FREQ48  		.set	36864   ;* (1800*65536/3200)
;LO_PHASE_ADJ48		 		.set	1229
;								 
;V27_EQ_LEN			 		.set	15 
;ACQ_EQ_2MU			 		.set	2185
;TRK_EQ_2MU			 		.set	1024  
;EQ_TRAIN_SEED		  		.set	1e3h
;ACQ_AGC_K			  		.set	1024
;
;WHAT						.set	512	 	;* 32768.0*OP_POINT
;SLICE_383			  		.set	192	 	;* 32768*sin(22.5)*OP_POINT
;SLICE_707			  		.set	362	 	;* 32768*cos(45)*OP_POINT
;SLICE_924			  		.set	473	 	;* 32768*cos(22.5)*OP_POINT
;SLICE_10					.set	512	 	;* 32768*cos(0)*OP_POINT
;
;ACQ24_LOOP_K1		  		.set	12355	
;ACQ24_LOOP_K2		  		.set	2330	
;TRK24_LOOP_K1		  		.set	3088	
;TRK24_LOOP_K2		  		.set	145	
;ACQ48_LOOP_K1		  		.set	9266	
;ACQ48_LOOP_K2		  		.set	1310	
;TRK48_LOOP_K1		  		.set	2316	
;TRK48_LOOP_K2		  		.set	81	
;
;ACQ_TIMING_THR 				.set    256		;* 32768.0*0.5*OP_POINT
;TRK_TIMING_THR	 			.set    3584	;* 32768.0*7.0*OP_POINT
;++++#else   MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
RX_FIR_TAPS24		  		.set	TAPS1200
RX_OVERSAMPLE24				.set	OVERSAMPLE1200
RX_INTERP24					.set	INTERP1200
RX_DEC24					.set	DEC1200
RX_COEF24_LEN		  		.set	(RX_FIR_TAPS24*RX_INTERP24+RX_DEC24)
RX_COEF_SAMPLE_RATE24  		.set	(8000*RX_INTERP24)
RX_V27_CARRIER_FREQ24  		.set	49152   ;* (1800*65536/2400)
LO_PHASE_ADJ24		 		.set	1228	;* (65536/8000)*(1800/RX_INTERP24)

RX_FIR_TAPS48		  		.set	TAPS1600
RX_OVERSAMPLE48				.set	OVERSAMPLE1600
RX_INTERP48					.set	INTERP1600
RX_DEC48					.set	DEC1600
RX_COEF48_LEN		  		.set	(RX_FIR_TAPS48*RX_INTERP48+RX_DEC48)
RX_COEF_SAMPLE_RATE48  		.set	(8000*RX_INTERP48)
RX_V27_CARRIER_FREQ48  		.set	36864   ;* (1800*65536/3200)
LO_PHASE_ADJ48		 		.set	1228
								 
V27_EQ_LEN			 		.set	15 
ACQ_EQ_2MU			 		.set	546
TRK_EQ_2MU			 		.set	273  
EQ_TRAIN_SEED		  		.set	1e3h
ACQ_AGC_K			  		.set	128

WHAT						.set	4096	;* 32768.0*OP_POINT
SLICE_383			  		.set	1567	;* 32768*sin(22.5)*OP_POINT
SLICE_707			  		.set	2896	;* 32768*cos(45)*OP_POINT
SLICE_924			  		.set	3784	;* 32768*cos(22.5)*OP_POINT
SLICE_10					.set	4096	;* 32768*cos(0)*OP_POINT

ACQ24_LOOP_K1		  		.set	1544	
ACQ24_LOOP_K2		  		.set	291    
TRK24_LOOP_K1		  		.set	386    
TRK24_LOOP_K2		  		.set	18  
ACQ48_LOOP_K1		  		.set	1158   
ACQ48_LOOP_K2		  		.set	163    
TRK48_LOOP_K1		  		.set	289    
TRK48_LOOP_K2		  		.set	10  

ACQ_TIMING_THR 				.set    2048	;* 32768.0*0.5*OP_POINT
TRK_TIMING_THR	 			.set    28672	;* 32768.0*7.0*OP_POINT
 .else      ;* OP_POINT=8
RX_FIR_TAPS24		  		.set	TAPS1200
RX_OVERSAMPLE24				.set	OVERSAMPLE1200
RX_INTERP24					.set	INTERP1200
RX_DEC24					.set	DEC1200
RX_COEF24_LEN		  		.set	(RX_FIR_TAPS24*RX_INTERP24+RX_DEC24)
RX_COEF_SAMPLE_RATE24  		.set	(8000*RX_INTERP24)
RX_V27_CARRIER_FREQ24  		.set	49152   ;* (1800*65536/2400)
LO_PHASE_ADJ24		 		.set	1228	;* (65536/8000)*(1800/RX_INTERP24)

RX_FIR_TAPS48		  		.set	TAPS1600
RX_OVERSAMPLE48				.set	OVERSAMPLE1600
RX_INTERP48					.set	INTERP1600
RX_DEC48					.set	DEC1600
RX_COEF48_LEN		  		.set	(RX_FIR_TAPS48*RX_INTERP48+RX_DEC48)
RX_COEF_SAMPLE_RATE48  		.set	(8000*RX_INTERP48)
RX_V27_CARRIER_FREQ48  		.set	36864   ;* (1800*65536/3200)
LO_PHASE_ADJ48		 		.set	1229
								 
V27_EQ_LEN			 		.set	15 
ACQ_EQ_2MU			 		.set	2185
TRK_EQ_2MU			 		.set	1024  
EQ_TRAIN_SEED		  		.set	1e3h
ACQ_AGC_K			  		.set	1024

WHAT						.set	512	 	;* 32768.0*OP_POINT
SLICE_383			  		.set	192	 	;* 32768*sin(22.5)*OP_POINT
SLICE_707			  		.set	362	 	;* 32768*cos(45)*OP_POINT
SLICE_924			  		.set	473	 	;* 32768*cos(22.5)*OP_POINT
SLICE_10					.set	512	 	;* 32768*cos(0)*OP_POINT

ACQ24_LOOP_K1		  		.set	12355	
ACQ24_LOOP_K2		  		.set	2330	
TRK24_LOOP_K1		  		.set	3088	
TRK24_LOOP_K2		  		.set	145	
ACQ48_LOOP_K1		  		.set	9266	
ACQ48_LOOP_K2		  		.set	1310	
TRK48_LOOP_K1		  		.set	2316	
TRK48_LOOP_K2		  		.set	81	

ACQ_TIMING_THR 				.set    256		;* 32768.0*0.5*OP_POINT
TRK_TIMING_THR	 			.set    3584	;* 32768.0*7.0*OP_POINT
 .endif		;* OP_POINT=8
;++++#endif  MESI_INTERNAL 03-09-2001 OP_POINT8 MODS 

DESCRAMBLER24_SEED	 		.set	0aa0h
DESCRAMBLER48_SEED	 		.set	3fh
EQ_START_PATTERN			.set	3ff0h
SCR24_PATTERN		  		.set	9aa0h
SCR48_PATTERN		  		.set	0a03fh
RX_PATTERN_LEN		 		.set	8
RX_MAP24_SHIFT		 		.set	1
RX_MAP48_SHIFT		 		.set	0

TRAIN_LOOPS_TIMEOUT			.set	50+20
START_EQ_TIMEOUT			.set	50+120
TRAIN_EQ_TIMEOUT			.set	1074+20
SCR1_PAT_LEN				.set	8
LONG_SEGMENT3_LEN	  		.set	TX_LONG_SEGMENT3_LEN	   
SHORT_SEGMENT3_LEN	 		.set	TX_SHORT_SEGMENT3_LEN 
LONG_SEGMENT4_LEN	  		.set	TX_LONG_SEGMENT4_LEN 
SHORT_SEGMENT4_LEN	 		.set	TX_SHORT_SEGMENT4_LEN 
SHORT_SCR1_TIMEOUT	 		.set	(SHORT_SEGMENT4_LEN+SCR1_PAT_LEN-1)
LONG_SCR1_TIMEOUT	  		.set	(LONG_SEGMENT4_LEN+SCR1_PAT_LEN-1)

 .if $isdefed("XDAIS_API")
	.global V27_MESI_RxInitV27
 .else
	.global Rx_init_v27
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

	.global	_VCOEF_MESI_RCOS1200f1800
	.asg	_VCOEF_MESI_RCOS1200f1800, _RCOS1200_f1800
	.global	_VCOEF_MESI_RCOS1600f1800
	.asg	_VCOEF_MESI_RCOS1600f1800, _RCOS1600_f1800
	.global	_VCOEF_MESI_RxTiming1200
	.asg	_VCOEF_MESI_RxTiming1200, _Rx_timing1200
	.global	_VCOEF_MESI_RxTiming1600
	.asg	_VCOEF_MESI_RxTiming1600, _Rx_timing1600
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

	.global _RCOS1200_f1800
	.global _RCOS1600_f1800
	.global _Rx_timing1200
	.global _Rx_timing1600
	.global Rx_state_return
	.global slicer_return
	.global timing_return
	.global decoder_return
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global Tx_v27_segment1
	.global Tx_v27_segment2
	.global Tx_v27_segment3
	.global Tx_v27_segment4
	.global Tx_v27_segment5
	.global Tx_v27_message
	.global Tx_v27_segmentA
	.global Tx_v27_segmentB
	.global v27_scrambler					
	.global v27_diff_encoder				
	.global Rx_v27_train_loops
	.global Rx_v27_detect_EQ
	.global Rx_v27_train_EQ
	.global Rx_v27_message
	.global v27_descrambler
	.global v27_slicer12
	.global v27_EQslicer12
	.global v27_slicer24
	.global v27_slicer48
	.global v27_diff_decoder
 .endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

;****************************************************************************
;* tables and coefficients
;****************************************************************************

	.sect	"vcoefs"

 .if (TX_V27_MODEM_2400=ENABLED)
Tx_v27_phase_map24:
	.word 0,1,3,2

Tx_v27_amp_table24:
		.word  23170, 23170
		.word -23170, 23170
		.word -23170,-23170
		.word  23170,-23170

 .endif

 .if (TX_V27_MODEM_4800=ENABLED)
Tx_v27_phase_map48:
	.word 1,0,2,3,6,7,5,4

Tx_v27_amp_table48:
		.word  30274, 12540
		.word  12540, 30274
		.word -12540, 30274
		.word -30274, 12540
		.word -30274,-12540
		.word -12540,-30274
		.word  12540,-30274
		.word  30274,-12540
 .endif

 .if (RX_V27_MODEM=ENABLED)
Rx_v27_hard_map:
	.word	 0,1,3,2,7,6,4,5

Rx_v27_phase_map:
	.word	 1,0,2,3,7,6,4,5
 .endif

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_v27(struct START_PTRS *)
;* void Rx_init_v27(struct START_PTRS *)
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TX_V27_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_v27:
;* C function call: void Tx_init_v27(struct START_PTRS *)
;* Initializes Tx_block for V27 modulator.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_v27:					
_V27_MESI_TxInitV27:
	STLM	A,AR0						   	;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v27
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v27:
;* Initializes Tx_block[] workspace for v27.
;* On entry it expects:
;*	AR0=&start_ptrs
;*	DP=&Tx_block
;****************************************************************************

Tx_init_v27:
V27_MESI_TxInitV27:
	MVDK	Tx_start_ptrs,AR0			   	;* AR0=start_ptrs
	LD		#0,A
	STL		A,Tx_sym_clk_offset			  	;* Tx_sym_clk_offset=0;
	STL		A,Tx_sym_clk_memory			  	;* Tx_sym_clk_memory=0;
	STL		A,Tx_sym_clk_phase			   	;* Tx_sym_clk_phase=0;

	LD		*AR0(Tx_fir_start),B
	STL		B,Tx_fir_head					;* Tx_fir_head=&Tx_fir[0]
	STL		B,Tx_fir_tail					;* Tx_fir_tail=&Tx_fir[0]
	ST		#TX_FIR_LEN,Tx_fir_len

	MVDK	Tx_fir_head,AR0
	STM		#(TX_FIR_LEN-1),BRC
	RPTB	v27_init_Tx_fir_loop
v27_init_Tx_fir_loop:
	 STL	A,*AR0+						 	;* Tx_fir[*++]=0

	STL		A,Tx_coef_ptr	
	STL		A,Tx_phase		
	STL		A,Tx_symbol_counter   
	STL		A,Tx_sample_counter   
	STL		A,Sguard
	STL		A,Sinv  
	ST		#SCRAMBLER_SEED,Tx_Sreg

	;**** rate-dependent initialization ****

	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	BC_		Tx_init_v27_else,BNEQ			;* if rate!=2400, Tx_init_v27_4800
 .if (TX_V27_MODEM_2400=ENABLED)
	ST		#(_RCOS1200_f1800+(RX_COEF24_LEN-TX_COEF24_LEN)),Tx_coef_start
	ST		#(TX_FIR_TAPS24-1),Tx_fir_taps		
	ST		#TX_V27_SCALE24,Tx_fir_scale	
	ST		#(2*TX_INTERP24),Tx_interpolate		
	ST		#(2*TX_DEC24),Tx_decimate			
	ST		#TX_CARRIER24,Tx_carrier		
	ST		#Tx_v27_phase_map24,Tx_map_ptr	
	ST		#Tx_v27_amp_table24,Tx_amp_ptr	
	ST		#2,Tx_Nbits
	ST		#3,Tx_Nmask
	B_		Tx_init_v27_endif
 .endif
Tx_init_v27_else:
 .if (TX_V27_MODEM_4800=ENABLED)
	ST		#(_RCOS1600_f1800+(RX_COEF48_LEN-TX_COEF48_LEN)),Tx_coef_start	
	ST		#(TX_FIR_TAPS48-1),Tx_fir_taps		
	ST		#TX_V27_SCALE48,Tx_fir_scale	
	ST		#(2*TX_INTERP48),Tx_interpolate		
	ST		#(2*TX_DEC48),Tx_decimate			
	ST		#TX_CARRIER48,Tx_carrier		
	ST		#Tx_v27_phase_map48,Tx_map_ptr	
	ST		#Tx_v27_amp_table48,Tx_amp_ptr	
	ST		#3,Tx_Nbits
	ST		#7,Tx_Nmask
	ST		#V27_RATE_4800,Tx_rate			
 .endif
Tx_init_v27_endif:

	;**** check for TEP enable ****

	ST		#TX_SEGMENT2_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment2,Tx_state,B
	ST		#TX_V27_SEGMENT2_ID,Tx_state_ID   
	LD		Tx_mode,B
	AND		#TX_TEP_FIELD,B
	RC_		BEQ							   	;* return if !TEP

	ST		#TX_SEGMENT1_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment1,Tx_state,B
	RETD_
	 ST		#TX_V27_SEGMENT1_ID,Tx_state_ID   

;****************************************************************************
;* Tx_v27_segment1: 200 msec unmodulated carrier (1800 Hz).
;****************************************************************************

Tx_v27_segment1:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	 LD		#1,A
	CALLD_	v27_diff_encoder
	XC		1,BEQ
	 LD		#0,A							;* if rate=2400, A=1

	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if sample_counter<LEN
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	ST		#TX_SEGMENT2_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment2,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENT2_ID,Tx_state_ID
	
;****************************************************************************
;* Tx_v27_segment2: 20 msec. silence.
;****************************************************************************

Tx_v27_segment2:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7				 	;* AR7=Tx_fir_head
	ST		#0,*AR7+%
	ST		#0,*AR7+%
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT			  	;* return if sample_counter<LEN
	 MVKD	AR7,Tx_fir_head			   		;* update Tx_fir_head

	LD		Tx_mode,B
	AND		#TX_LONG_RESYNC_FIELD,B
	 ST		#0,Tx_symbol_counter
	 ST		#0,Tx_sample_counter
	ST		#TX_SHORT_SEGMENT3_LEN,Tx_terminal_count
	XC		2,BEQ
	 ST		#TX_LONG_SEGMENT3_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment3,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENT3_ID,Tx_state_ID

;****************************************************************************
;* Tx_v27_segment3: 180 degree phase reversals (50/14 symbols).
;****************************************************************************

Tx_v27_segment3:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	LD		Tx_Nmask,A
	CALL_	v27_diff_encoder

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if sample_counter<LEN
	LD		Tx_mode,B
	AND		#TX_LONG_RESYNC_FIELD,B
	 ST		#0,Tx_symbol_counter
	 ST		#0,Tx_sample_counter
	ST		#TX_SHORT_SEGMENT4_LEN,Tx_terminal_count
	XC		2,BEQ
	 ST		#TX_LONG_SEGMENT4_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment4,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENT4_ID,Tx_state_ID	

;****************************************************************************
;* Tx_v27_segment4: 2-phase equalizer training (1074/58 symbols).
;****************************************************************************

Tx_v27_segment4:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail
 
	LD		#1,A
	CALL_	 v27_scrambler				  	;* scramble first bit
	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	BC_		Tx_v27_seg4_else1,BEQ
	LD		A,B
	 LD		#7,A							;* symbol=7
	CALLD_	v27_diff_encoder
	XC		1,BEQ					
	 LD		#1,A							;* if k=0, symbol=1
	B_		Tx_v27_seg4_endif1

Tx_v27_seg4_else1:
	LD		A,B
	 LD		#3,A							;* symbol=3
	CALLD_	v27_diff_encoder
	XC		1,BEQ
	 LD		#0,A							;* if k=0, symbol=0
Tx_v27_seg4_endif1:

	LD		#1,A
	CALL_	 v27_scrambler			
	LD		#1,A
	CALL_	 v27_scrambler			

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if sample_counter<LEN
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	ST		#TX_SEGMENT5_LEN,Tx_terminal_count
	STPP	#Tx_v27_segment5,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENT5_ID,Tx_state_ID	

;****************************************************************************
;* Tx_v27_segment5: scrambled ONEs (8 symbols).
;****************************************************************************

Tx_v27_segment5:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B				 
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail
 
	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	LD		#1,A
	CCD_		 v27_scrambler,BNEQ			  	;* scramble first bit
	 XC		1,BEQ	
	 LD		#0,A							;* if rate=2400, A=0
	CALLD_	v27_scrambler			
	 STL	A,Ereg						  	;* Ereg=symbol
	 LD		#1,A
	LD		Ereg,1,B
	OR		B,A							   	;* A=scrambler()|symbol<<1
	CALLD_	v27_scrambler			
	 STL	A,Ereg						  	;* Ereg=symbol
	 LD		#1,A
	CALLD_	v27_diff_encoder
	 LD		Ereg,B
	 OR		B,1,A							;* A=scrambler()|symbol<<1

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if symbol_counter<LEN
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	ST		#-1,Tx_terminal_count
	STPP	#Tx_v27_message,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_MESSAGE_ID,Tx_state_ID

;****************************************************************************
;* Tx_v27_message: Scrambled v27 message data.
;****************************************************************************

Tx_v27_message:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail

	MVDK	Tx_data_len,BK
	MVDK	Tx_data_tail,AR4
	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	LD		*AR4,-2,A						;* A=bits>>2
	CCD_		 v27_scrambler,BNEQ			  	;* scramble first bit
	 XC		1,BEQ	
	 LD		#0,A							;* if rate=2400, A=0
	STL		A,Ereg						   	;* Ereg=symbol
	CALLD_	v27_scrambler			
	 LD		*AR4,-1,A						;* A=bits>>1
	LD		Ereg,B
	OR		B,1,A							;* A=scrambler()|symbol<<1
	CALLD_	v27_scrambler			
	 STL	A,Ereg						  	;* Ereg=symbol
	 LD		*AR4+%,A						;* A=bits
	CALLD_	v27_diff_encoder
	 LD		Ereg,B
	 OR		B,1,A							;* A=scrambler()|symbol<<1
	MVKD	AR4,Tx_data_tail				;* update Tx_data_tail

	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if terminal_count<0
	SUB		Tx_symbol_counter,B
	BC_		Tx_state_return,BGT			   	;* return if symbol_counter<LEN
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	ST		#TX_SEGMENTA_LEN,Tx_terminal_count
	STPP	#Tx_v27_segmentA,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENTA_ID,Tx_state_ID

;****************************************************************************
;* Tx_v27_segmentA: scrambled ONEs (8 symbols).
;****************************************************************************

Tx_v27_segmentA:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail
 
	LD		Tx_rate,B
	SUB		#V27_RATE_2400,B
	LD		#1,A
	CCD_		 v27_scrambler,BNEQ			  	;* scramble first bit
	 XC		1,BEQ	
	 LD		#0,A							;* if rate!=2400, A=0
	CALLD_	v27_scrambler			
	 STL	A,Ereg						  	;* Ereg=symbol
	 LD		#1,A
	LD		Ereg,1,B
	OR		B,A							   	;* A=scrambler()|symbol<<1
	CALLD_	v27_scrambler			
	 STL	A,Ereg						  	;* Ereg=symbol
	 LD		#1,A
	CALLD_	v27_diff_encoder
	 LD		Ereg,B
	 OR		B,1,A							;* A=scrambler()|symbol<<1

	LD		Tx_symbol_counter,B
	SUB		Tx_terminal_count,B
	BC_		Tx_state_return,BLT			   	;* return if sample_counter<LEN
	ST		#0,Tx_symbol_counter
	ST		#0,Tx_sample_counter
	ST		#TX_SEGMENTB_LEN,Tx_terminal_count
	STPP	#Tx_v27_segmentB,Tx_state,B
	BD_		Tx_state_return
	 ST		#TX_V27_SEGMENTB_ID,Tx_state_ID

;****************************************************************************
;* Tx_v27_segmentB: 20 msec. silence.
;****************************************************************************

Tx_v27_segmentB:
	CALLD_	APSK_modulator
	 MVDK	Tx_fir_len,BK				  	;* BK=TX_FIR_LEN
	LDU		Tx_fir_head,B
	SUBS	Tx_fir_tail,B
	BC_		Tx_state_return,BNEQ			;* branch if Tx_fir_head!=Tx_fir_tail
 
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7				 	;* AR7=Tx_fir_head
	ST		#0,*AR7+%
	ST		#0,*AR7+%
	LD		Tx_sample_counter,B
	SUB		Tx_terminal_count,B
	BCD_	Tx_state_return,BLT			  	;* return if sample_counter<LEN
	 MVKD	AR7,Tx_fir_head			   		;* update Tx_fir_head
	CALL_	Tx_init_silence				 	;* switch to silence
	B_		Tx_state_return

;****************************************************************************
;* v27_scrambler: scrambler with repeating sequence guards.
;* Expects the following on entry:
;*	A=in
;* Modifies:
;*	A,B
;* On exit:
;*	A=out
;****************************************************************************

v27_scrambler:					
	AND		#1,A							;* A=in&1
	LD		Tx_Sreg,-5,B					;* B=Sreg>>5
	XOR		B,A							  	;* A=in^(Sreg>>5)
	SFTL	B,-1							;* B=Sreg>>6
	XOR		B,A							  	;* A=in^(Sreg>>5)^(Sreg>>6)
	XOR		Sinv,A						   	;* A=in^(Sreg>>5)^(Sreg>>6)^Sinv
	AND		#1,A							;* A&=1

	LD		Tx_Sreg,-7,B					;* B=Sreg>>7
	XOR		A,B							  	;* B=out^(Sreg>>7)
	STL		B,Tx_Sreg_low					;* low=out^(Sreg>>7)
	LD		Tx_Sreg,-8,B					;* B=Sreg>>8
	XOR		A,B							  	;* B=out^(Sreg>>8)
	AND		Tx_Sreg_low,B					;* B&=out^(Sreg>>8)
	STL		B,Tx_Sreg_low					;* low=out^(Sreg>>7)
	LD		Tx_Sreg,-11,B					;* B=Sreg>>11
	XOR		A,B							  	;* B=out^(Sreg>>11)
	AND		Tx_Sreg_low,B					;* B&=out^(Sreg>>8)
	AND		#1,B							;* B&=1 (B=k)
	STL		B,Tx_Sreg_low					;* low=k

	LD		Tx_Sreg,1,B					   	;* B=Sreg<<1
	OR		A,B							   	;* B=(Sreg<<1)|out
	STL		B,Tx_Sreg						;* update Sreg

	LD		Tx_Sreg_low,B
	OR		Sinv,B							;* B=k|Sinv
	SUB		#1,B							;* compare with 1
	BC_		v27_scrambler_else,BNEQ
	ST		#0,Sinv						   	;* Sinv=0
	RETD_
	 ST		#0,Sguard						;* Sguard=0

v27_scrambler_else:
	LD		Sguard,B						;* B=Sguard
	SUB		#32,B
	 ADDM	#1,Sguard					  	;* ++Sguard
	 NOP
	XC		2,BEQ
	 ST		#1,Sinv						  	;* if Sguard=32, Sinv=1
	RET_

;****************************************************************************
;* v27_diff_encoder: differentially encodes the data bits for v.27.
;* Expects the following on entry:
;*	A=symbol
;****************************************************************************

v27_diff_encoder:				
 .if ON_CHIP_COEFFICIENTS=ENABLED
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7				 	;* AR7=Tx_fir_head
	ADDS	Tx_map_ptr,A					;* A=map_ptr+symbol
	STLM	A,AR0							;* AR0=(map_ptr+symbol)
	 LD		Tx_phase,A
	 ADD	Tx_carrier,A					;* A=Tx_phase+Tx_carrier
	ADD		*AR0,A						   	;* A=phase+carrier+*(map_ptr+sym)
	AND		Tx_Nmask,A					   	;* A&=Tx_mask
	STL		A,Tx_phase					   	;* update Tx_phase
	SFTL	A,1							 	;* A=2*phase
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*phase
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
	MVDK	Tx_fir_len,BK
	MVDK	Tx_fir_head,AR7				 	;* AR7=Tx_fir_head
	ADDS	Tx_map_ptr,A					;* A=map_ptr+symbol
	READA	*AR7						   	;* Tx_fir[]=*(map_ptr+symbol)
	LD		Tx_phase,A
	ADD		Tx_carrier,A					;* A=Tx_phase+Tx_carrier
	ADD		*AR7,A						   	;* A=phase+carrier+*(map_ptr+sym)
	AND		Tx_Nmask,A					   	;* A&=Tx_mask
	STL		A,Tx_phase					   	;* update Tx_phase
	SFTL	A,1							 	;* A=2*phase
	ADDS	Tx_amp_ptr,A					;* A=amp_ptr+2*phase
	READA	*AR7						   	;* Tx_fir[]=real
	LD		Tx_fir_scale,T			
	MPY		*AR7,B
	STH		B,*AR7+%
	ADD		#1,A							;* A=amp_ptr+2*phase+1
	READA	*AR7						   	;* Tx_fir[]=real
	MPY		*AR7,B
	STH		B,*AR7+%
	RETD_
	 MVKD	AR7,Tx_fir_head			   		;* update Tx_fir_head
 .endif

;****************************************************************************
 .endif

	;**************************
	;**** receiver modules ****
	;**************************

 .if RX_V27_MODEM=ENABLED
;****************************************************************************
;* Rx_init_v27:
;* Initializes Rx_block[] workspace for v27 demodulation.
;* On entry it expects:
;*	 DP=&Rx_block
;****************************************************************************

Rx_init_v27:						
V27_MESI_RxInitV27:
	MVDK	Rx_start_ptrs,AR0			
	LD		#0,A
	LD		Rx_data_head,B
	STL		B,Rx_data_tail
	STPP	#v27_diff_decoder,decoder_ptr,B
	STPP	#v27_slicer12,slicer_ptr,B
	STPP	#sgn_timing,timing_ptr,B
	STL		A,Rx_coef_ptr			

	STL		A,EQ_MSE			
	ST		#EQ_DISABLED,EQ_2mu		
	ST		#(V27_EQ_LEN-1),EQ_taps
	MVDK	*AR0(Rx_fir_start),AR3
	MVKD	AR3,Rx_fir_ptr			
	STM		#(RX_FIR_LEN-1),BRC
	RPTB	v27_init_Rx_fir_loop
v27_init_Rx_fir_loop:
	 STL	A,*AR3+				

	MVDK	*AR0(EQ_coef_start),AR3
	STM		#(2*V27_EQ_LEN-1),BRC
	RPTB	v27_init_EQ_coef_loop
v27_init_EQ_coef_loop:
	 STL	A,*AR3+				
	
	ST		#ACQ_AGC_K,agc_K			
	STL		A,Rx_sym_clk_memory		
	ST		#ACQ_TIMING_THR,timing_threshold	
	STL		A,coarse_error		
	ST		#(V27_EQ_LEN/2),Rx_baud_counter	
	STL		A,Rx_sample_counter	
	STL		A,Rx_symbol_counter	
	STL		A,vco_memory		
	STL		A,loop_memory		
	STL		A,loop_memory_low
	STL		A,frequency_est
	STL		A,LOS_counter		
	STL		A,LOS_monitor		
	ST		#WHAT,What			
	STL		A,Rx_pattern_reg
	STL		A,Rx_Dreg
	STL		A,Rx_Dreg+1
	STL		A,LO_memory					
	STL		A,Ihat
	STL		A,Qhat
	STL		A,frequency_est

	;**** rate-dependent initialization ****

	LD		Rx_rate,B
	SUB		#V27_RATE_2400,B
	BC_		Rx_init_v27_else,BNEQ		
 .if (RX_V27_MODEM_2400=ENABLED)
	ST		#_Rx_timing1200,Rx_timing_start	
	ST		#_RCOS1200_f1800,Rx_coef_start	
	ST		#(RX_FIR_TAPS24-1),Rx_fir_taps	
	ST		#RX_OVERSAMPLE24,Rx_oversample
	ST		#(2*RX_INTERP24),Rx_interpolate
	ST		#RX_DEC24,Rx_decimate
	ST		#RX_DEC24/2,Rx_sym_clk_phase	
	ST		#LO_PHASE_ADJ24,LO_phase					
	ST		#-RX_MAP24_SHIFT,Rx_map_shift		
	ST		#V27_RATE_2400,Rx_rate			
	ST		#RX_V27_CARRIER_FREQ24,LO_frequency	
	ST		#ACQ24_LOOP_K1,loop_K1			
	ST		#ACQ24_LOOP_K2,loop_K2			
	STL		A,PJ1_coef						;* disable PJ1 resonator
	STL		A,PJ2_coef                      ;* disable PJ2 resonator
	ST		#2,Rx_Nbits		
	ST		#03h,Rx_Nmask		
	B_		Rx_init_v27_endif
 .endif
Rx_init_v27_else:
 .if (RX_V27_MODEM_4800=ENABLED)
	ST		#_Rx_timing1600,Rx_timing_start	
	ST		#_RCOS1600_f1800,Rx_coef_start	
	ST		#(RX_FIR_TAPS48-1),Rx_fir_taps	
	ST		#RX_OVERSAMPLE48,Rx_oversample
	ST		#(2*RX_INTERP48),Rx_interpolate
	ST		#RX_DEC48,Rx_decimate
	ST		#RX_DEC48/2,Rx_sym_clk_phase	
	ST		#LO_PHASE_ADJ48,LO_phase					
	ST		#-RX_MAP48_SHIFT,Rx_map_shift		
	ST		#V27_RATE_4800,Rx_rate			
	ST		#RX_V27_CARRIER_FREQ48,LO_frequency	
	ST		#ACQ48_LOOP_K1,loop_K1			
	ST		#ACQ48_LOOP_K2,loop_K2			
	STL		A,PJ1_coef						;* disable PJ1 resonator
	STL		A,PJ2_coef                      ;* disable PJ2 resonator
	ST		#V27_RATE_4800,Rx_rate
	ST		#3,Rx_Nbits	  
	ST		#07h,Rx_Nmask 
 .endif
Rx_init_v27_endif:

	CALL_	 agc_gain_estimator

	MVKD	AR2,Rx_sample_tail
	MVKD	AR2,Rx_sample_ptr

	;**** switch states to train_loops ****

	ST		#0,Rx_status
	STPP	#Rx_v27_train_loops,Rx_state,B
	 ST		#RX_V27_TRAIN_LOOPS_ID,Rx_state_ID
	RET_

;****************************************************************************
;* Rx_v27_train_loops: carrier,symbol,agc loop training.
;****************************************************************************

Rx_v27_train_loops:
	CALL_	Rx_train_loops
	LD		Rx_symbol_counter,B
	BCD_		Rx_state_return,AEQ			  	;* return if A=0
	 SUB	#2,A
	BCD_		loops_stable_detected,AEQ		;* branch if return=2
	 SUB	#TRAIN_LOOPS_TIMEOUT,B		  	;* Rx_symbol_counter-TRAIN_LOOPS_TIMEOUT

	;**** check for timeout ****

	BC_		Rx_state_return,BLEQ			;* branch if symbol_counter<=TIMEOUT
	CALL_	 Rx_init_detector
	BD_		Rx_state_return
	 ST		#TRAIN_LOOPS_FAILURE,Rx_status  ;* set status to FAILURE

loops_stable_detected:
	STPP	#Rx_v27_detect_EQ,Rx_state,B
	BD_		Rx_state_return
	  ST	#RX_V27_DETECT_EQ_ID,Rx_state_ID;* Rx_state_ID=RX_V27_DETECT_EQ_ID

;****************************************************************************
;* Rx_v27_detect_EQ: wait for start of training
;****************************************************************************

Rx_v27_detect_EQ:
	CALL_	Rx_detect_EQ
	LD		Rx_symbol_counter,B
	BCD_		Rx_state_return,AEQ			  	;* return if A=0
	 SUB	#2,A
	BCD_		Rx_fir_detected,AEQ			  	;* branch if return=2
	 SUB	#START_EQ_TIMEOUT,B			 	;* Rx_symbol_counter-START_EQ_TIMEOUT

	;**** check for timeout ****

	BC_		Rx_state_return,BLEQ			;* branch if symbol_counter<=TIMEOUT
	CALL_	 Rx_init_detector
	ST		#START_EQ_FAILURE,Rx_status	   	;* set status to FAILURE
	B_		Rx_state_return

Rx_fir_detected:
	STPP	#v27_EQslicer12,slicer_ptr,B
	STPP	#no_timing,timing_ptr,A
	LD		#0,A
	ST		#EQ_TRAIN_SEED,Rx_Dreg	
	ST		#ACQ_EQ_2MU,EQ_2mu	
	STL   	A,agc_K			
	STL   	A,Rx_symbol_counter	

	LD		Rx_mode,B
	AND		#RX_LONG_RESYNC_FIELD,B
	ST		#SHORT_SCR1_TIMEOUT,train_EQ_timeout
	STPP	#Rx_v27_train_EQ,Rx_state,A
	XC		2,BEQ
	 ST		#LONG_SCR1_TIMEOUT,train_EQ_timeout
	BD_		Rx_state_return
	  ST	#RX_V27_TRAIN_EQ_ID,Rx_state_ID

;****************************************************************************
;* Rx_v27_train_EQ: equalizer training.
;****************************************************************************

Rx_v27_train_EQ:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	BCD_	Rx_state_return,BEQ			  	;* branch if Rx_data_head==Rx_data_tail
	 LD		Rx_symbol_counter,A
	 SUB	train_EQ_timeout,A
	ADD		#(8+4),A
	LDU		Rx_data_head,B
	BCD_	EQ_endif,ANEQ				   	;* branch if counter!=timeout-12
	 STL	B,Rx_data_tail				  	;* data_tail=data_head
	 LD		Rx_symbol_counter,A
	ST		#0,Rx_sym_clk_memory
	ST		#TRK_TIMING_THR,timing_threshold
	STPP	#APSK_timing,timing_ptr,B
	LD		Rx_rate,B
	SUB		#V27_RATE_4800,B
	BC_		EQ_else,BNEQ					;* branch if rate!=4800
	LDU		vco_memory,B
	ADD		#4096,B
	STL		B,vco_memory					;* vco_memory+=4096
	STPP	#v27_slicer48,slicer_ptr,B
	ST		#TRK48_LOOP_K1,loop_K1		
	BD_		 EQ_endif				
	 ST		#TRK48_LOOP_K2,loop_K2		
EQ_else:
	LDU		vco_memory,B
	ADD		#8192,B
	STL		B,vco_memory					;* vco_memory+=8192
	STPP	#v27_slicer24,slicer_ptr,B
	ST		#TRK24_LOOP_K1,loop_K1			
	ST		#TRK24_LOOP_K2,loop_K2			
EQ_endif:

	;**** check for end of SCR1 ****

	SUB		train_EQ_timeout,A
	BC_		Rx_state_return,ALT			   	;* branch if counter<SCR1_TIMEOUT
	ST		#TRK_EQ_2MU,EQ_2mu				;* EQ_2mu=TRK_EQ_2MU
	LD		Rx_data_head,B
	STL		B,Rx_data_ptr
	ST		#0,Dguard			
	ST		#0,Dinv			
	LD		Rx_rate,B
	SUB		#V27_RATE_4800,B
	 ST		#DESCRAMBLER24_SEED,Rx_Dreg
	 ST		#0,Rx_symbol_counter	
	XC		2,BEQ 
	 ST		 #DESCRAMBLER48_SEED,Rx_Dreg		  
	STPP	#Rx_v27_message,Rx_state,B
	BD_		Rx_state_return
	  ST	#RX_V27_MESSAGE_ID,Rx_state_ID

;****************************************************************************
;* Rx_v27_message: v27 message data.
;****************************************************************************

Rx_v27_message:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_ptr,B
	BCD_	Rx_state_return,BEQ			  	;* branch if Rx_data_head==Rx_data_tail
	 MVDK	Rx_data_len,BK
	MVDK	Rx_data_head,AR0				;* AR0=Rx_data_head
	LD		LOS_monitor,B
	SUB		#UNLOCKED,B
	BCD_		mes_while_loop,BNEQ
	 MVDK	Rx_data_ptr,AR4					;* AR4=Rx_data_ptr
	CALL_	 Rx_init_detector
	ST		#LOSS_OF_LOCK,Rx_status		   	;* set status to LOSS_OF_LOCK
	B_		Rx_state_return

mes_while_loop:
	LD		Rx_rate,B
	SUB		#V27_RATE_4800,B
	LD		*AR4,-2,A						;* A=j>>2 (scrambled bits)
	CCD_		 v27_descrambler,BEQ			;* descramble first bit
	 XC		1,BNEQ	
	 LD		#0,A							;* if rate!=4800, A=0
	STL		A,temp0						  	;* temp=symbol
	CALLD_	v27_descrambler			
	 LD		*AR4,-1,A						;* A=bits>>1
	LD		temp0,B
	OR		B,1,A							;* A=descrambler()|symbol<<1
	CALLD_	v27_descrambler			
	 STL	A,temp0						 	;* temp0=symbol
	 LD		*AR4,A						   	;* A=bits
	LD		temp0,B
	OR		B,1,A							;* A=descrambler()|symbol<<1
	STL		A,*AR4+%						;* Rx_data[*++%]=symbol
	CMPR	1,AR4						   	;* Rx_data_ptr-Rx_data_head
	BC_		mes_while_loop,TC				;* branch if data_tail!=data_ptr
	BD_		Rx_state_return			
	 MVKD	AR4,Rx_data_ptr					;* update Rx_data_ptr

;****************************************************************************
;* v27_descrambler: descrambler with repeating sequence guards.
;* Expects the following on entry:
;*	A=in
;* Modifies:
;*	A,B
;* On exit:
;*	A=out
;****************************************************************************

v27_descrambler:
	AND		#1,A							;* A&=1
	LD		Rx_Dreg,-7,B					;* B=Dreg>>7
	XOR		A,B							  	;* B=in^(Dreg>>7)
	STL		B,Rx_Dreg_low					;* low=in^(Dreg>>7)
	LD		Rx_Dreg,-8,B					;* B=Dreg>>8
	XOR		A,B							  	;* B=in^(Dreg>>8)
	AND		Rx_Dreg_low,B					;* B&=in^(Dreg>>8)
	STL		B,Rx_Dreg_low					;* low=in^(Dreg>>7)
	LD		Rx_Dreg,-11,B					;* B=Dreg>>11
	XOR		A,B							  	;* B=in^(Dreg>>11)
	AND		Rx_Dreg_low,B					;* B&=in^(Dreg>>8)
	AND		#1,B							;* B&=1 (B=k)
	STL		B,Rx_Dreg_low					;* low=k

	LD		Rx_Dreg,1,B				
	OR		A,B							   	;* B=(Dreg<<1)|in
	STL		B,Rx_Dreg						;* update Dreg

	SFTL	B,-6							;* B=Dreg>>5
	XOR		B,A							  	;* A=in^(Dreg>>5)
	SFTL	B,-1							;* B=Dreg>>6
	XOR		B,A							  	;* A=in^(Dreg>>5)^(Dreg>>6)
	XOR		Dinv,A						   	;* A=in^(Dreg>>5)^(Dreg>>6)^Dinv
	AND		#1,A							;* A=out

	LD		Rx_Dreg_low,B
	OR		Dinv,B							;* B=k|Dinv
	SUB		#1,B							;* compare with 1
	BC_		v27_descrambler_else,BNEQ
	ST		#0,Dinv						   	;* Dinv=0
	RETD_
	 ST		#0,Dguard						;* Dguard=0

v27_descrambler_else:
	LD		Dguard,B						;* B=Dguard
	SUB		#32,B
	 ADDM	#1,Dguard					  	;* ++Dguard
	 NOP
	XC		2,BEQ
	 ST		#1,Dinv						  	;* if Dguard=32, Dinv=1
	RET_

;****************************************************************************
;* v27_slicer12: BPSK slicer
;****************************************************************************

v27_slicer12:
	LD		Iprime,B
	 ST		#-SLICE_10,Ihat			
	 ST		#3,Phat		
	XC		2,BGEQ
	 ST		#SLICE_10,Ihat				   	;* if Iprime>=0, Ihat=SLICE_10
	XC		2,BGEQ
	 ST		#0,Phat						  	;* if Iprime>=0, Phat=0
	BD_		slicer_return
	 ST		#0,Qhat			

;****************************************************************************
;* v27_EQslicer12: BPSK reference train slicer
;****************************************************************************

v27_EQslicer12:
	LD		Rx_Dreg,-3,B					;* B=Dreg>>(6-3)
	XOR		#7,B,A						   	;* A=7 ^ Dreg>>(6-3)
	SFTL	B,-1							;* B=Dreg>>(7-3)
	XOR		B,A							  	;* A=7 ^ Dreg>>(6-3) ^ Dreg>>(7-3)
	AND		#7,A							;* A&=7
	LD		Rx_Dreg,3,B					   	;* B=Dreg<<3
	OR		A,B							   	;* B=(Dreg<<3)|out
	STL		B,Rx_Dreg						;* update Dreg
	XOR		Rx_pattern_reg,A				;* A=pattern_reg^k
	AND		#4,A							;* A=(pattern_reg^k&4
	 STL	A,Rx_pattern_reg				;* update pattern_reg
	 ST		#-SLICE_10,Ihat			
	XC		2,AEQ
	 ST		#SLICE_10,Ihat				   	;* if pattern_reg=0, B=SLICE_10
	BD_		slicer_return
	 ST		#0,Qhat			

;****************************************************************************
;* v27_slicer24: QPSK slicer
;****************************************************************************

v27_slicer24:
	LD		Iprime,B
	 ST		#-SLICE_707,Ihat			
	 STM	#Rx_v27_hard_map,AR5			;* AR5=&Rx_v27_hard_map[k]
	XC		2,BGEQ
	 ST		#SLICE_707,Ihat				  	;* if Iprime>=0, Ihat=SLICE_707
	LD		Qprime,A
	 ST		#-SLICE_707,Qhat			
	 XC		2,BLT
	  MAR	*+AR5(2)					   	;* if Iprime<0, AR5=&hard_map[2] 
	XC		2,AGEQ
	 ST		#SLICE_707,Qhat				  	;* if Qprime>=0, Qhat=SLICE_707
	LD		Iprime,T
	MPY		Qprime,B						;* B=Iprime*Qprime
	XC		2,ALT
	 MAR	*+AR5(4)						;* if Qprime<0, AR5=&hard_map[4] 
	XC		2,BLT
	 MAR	*+AR5(1)						;* if Iprime*Qprime<0, AR5+=1
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v27_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v27_hard_map[k]
 .else
	 LDM	AR5,A						   	;* A=&Rx_v27_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v27_hard_map[k]
 .endif
 
;****************************************************************************
;* v27_slicer48: OPSK slicer
;****************************************************************************

v27_slicer48:
	LD		Iprime,A
	LD		Qprime,B
	ABS		A								;* A=abs(Iprime)
	ABS		B								;* B=abs(Qprime)
	SUB		B,A							  	;* abs(Iprime)-abs(Qprime)
	BCD_		OPSK_1256,ALEQ				   	;* branch if abs(Iprime<=abs(Qprime
	 LD		Iprime,B
	 LD		Qprime,A
	ST		#SLICE_924,Ihat			
	ST		#SLICE_383,Qhat			
	STM		#Rx_v27_hard_map,AR5			;* AR5=&Rx_v27_hard_map[k]
	XC		2,BLT
	 ST		#-SLICE_924,Ihat				;* if Iprime<0, Ihat=-SLICE_924
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Iprime<0, AR5=&hard_map[2] 
	XC		2,ALT
	 ST		#-SLICE_383,Qhat				;* if Qprime<0, Qhat=-SLICE_383
	XC		2,ALT
	 MAR	*+AR5(4)						;* if Qprime<0, AR5=&hard_map[4] 
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v27_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v27_hard_map[k]
 .else
	 LDM	AR5,A						   	;* A=&Rx_v27_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v27_hard_map[k]
 .endif

OPSK_1256:
	ST		#SLICE_383,Ihat			
	ST		#SLICE_924,Qhat			
	STM		#Rx_v27_hard_map,AR5			;* AR5=&Rx_v27_hard_map[k]
	MAR		*+AR5(1)						;* AR5=&Rx_v27_hard_map[1]
	XC		2,BLT
	 ST		#-SLICE_383,Ihat				;* if Iprime<0, Ihat=-SLICE_383
	XC		2,BLT
	 MAR	*+AR5(2)						;* if Iprime<0, AR5=&hard_map[2] 
	XC		2,ALT
	 ST		#-SLICE_924,Qhat				;* if Qprime<0, Qhat=-SLICE_924
	XC		2,ALT
	 MAR	*+AR5(4)						;* if Qprime<0, AR5=&hard_map[4] 
	BD_		slicer_return
 .if ON_CHIP_COEFFICIENTS=ENABLED
	 LD		*AR5,A							;* A=Rx_v27_hard_map[k]
	 STL	A,Phat							;* Phat=Rx_v27_hard_map[k]
 .else
	 LDM	AR5,A						   	;* A=&Rx_v27_hard_map[k]
	 READA 	Phat							;* Phat=Rx_v27_hard_map[k]
 .endif

;****************************************************************************
;* v27_diff_decoder: hard decision symbol differential decoder for QPSK/OPSK
;* On entry, it expects the following:
;*	AR7=Rx_data_head
;*	BK=Rx_data_len
;*	A=Phat
;****************************************************************************

v27_diff_decoder:
 .if ON_CHIP_COEFFICIENTS=ENABLED
	ADD		#8,A							;* A=Phat+8
	SUB		Rx_phase,A					   	;* A=Phat+8-Rx_phase
	AND		#7,A							;* A&=7
	ADD		#Rx_v27_phase_map,A			  	;* A=&Rx+phase_map[(Phat+8-Rx_phase&7]
	STLM	A,AR0	
	 LD		Phat,B
	 STL	B,Rx_phase					   	;* Rx_phase=Phat
	LD		Rx_map_shift,T
	LD		*AR0,TS,A						;* A=Rx_v27_phase_map[*]>>map_shift
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_v27_phase_map[*]
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head			   	;* update Rx_data_head	
 .else
	ADD		#8,A							;* A=Phat+8
	SUB		Rx_phase,A					   	;* A=Phat+8-Rx_phase
	AND		#7,A							;* A&=7
	ADD		#Rx_v27_phase_map,A				;* A=&Rx+phase_map[(Phat+8-Rx_phase&7]
	LDM		AL,A							;* clear upper 16 bits for READA
	LD		Phat,B
	STL		B,Rx_phase					   	;* Rx_phase=Phat
	READA	*AR7						   	;* *AR7=Rx_v27_phase_map[*]
	LD		Rx_map_shift,T
	LD		*AR7,TS,A						;* R0=Rx_v27_phase_map[*]>>map_shift
	STL		A,*AR7+%						;* Rx_data[*++%]=Rx_v27_phase_map[*]
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	
 .endif

;****************************************************************************
 .endif

;****************************************************************************
	.end
