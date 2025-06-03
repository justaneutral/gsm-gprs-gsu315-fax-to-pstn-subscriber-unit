;****************************************************************************	
;* Filename: tcm.asm
;* Date: 04-11-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Trellis Coded Modulation encoder and decoder.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"tcm.inc"

TCM_RATE_7200		  		.set	7200
TCM_RATE_9600		  		.set	9600
TCM_RATE_12000		 		.set	12000
TCM_RATE_14400		 		.set	14400

TX_TCM_PHASE_LEN			.set	4
 
T0					 		.set	0
T1					 		.set	2048 
T2					 		.set	(2*T1)
T3					 		.set	(3*T1)
T4					 		.set	(4*T1)
T5					 		.set	(5*T1)
T6					 		.set	(6*T1)
T7					 		.set	(7*T1)
T8					 		.set	(8*T1)
T9					 		.set	(9*T1)

 .if $isdefed("XDAIS_API")
	.global TCM_MESI_TxInitTCM
	.global TCM_MESI_TCMencoder
 .else
	.global Tx_init_TCM
	.global TCM_encoder
 .endif										;* "XDAIS_API endif

Y1					 		.set	1	   
Y2					 		.set	3	   
D0					 		.set	4	   
D1					 		.set	5	   
D2					 		.set	6	   

;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;SLICE1_T1_SCALE				.set	1296 
;SLICE0				 		.set	0
;SLICE1				 		.set	81	
;SLICE2				 		.set	(SLICE1*2)
;SLICE3				 		.set	(SLICE1*3)
;SLICE4				 		.set	(SLICE1*4)
;SLICE5				 		.set	(SLICE1*5)
;SLICE6				 		.set	(SLICE1*6)
;SLICE7				 		.set	(SLICE1*7)
;SLICE8				 		.set	(SLICE1*8)
;SLICE9				 		.set	(SLICE1*9)
;SLICE10						.set	(SLICE1*10)
;SLICE12						.set	(SLICE1*12)
;GRID_SLICE_LEVEL 			.set    64
;GRID_SLICE_COEF1			.set    25891	;* GRID_SLICE_LEVEL/SLICE1
;GRID_SLICE_COEF2			.set    20736	;* 0.5*SLICE1/GRID_SLICE_LEVEL
;
;;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;;PM_SHIFT					.set	8
;;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;PM_SHIFT					.set	(20-2*OP_POINT_SHIFT)
;;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;WHAT_TABLE_COEF				.set	160
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
 .if OP_POINT == OP_POINT8
SLICE1_T1_SCALE				.set	1296 
SLICE0				 		.set	0
SLICE1				 		.set	648   
SLICE2				 		.set	1295  
SLICE3				 		.set	1943  
SLICE4				 		.set	2591  
SLICE5				 		.set	3238  
SLICE6				 		.set	3886  
SLICE7				 		.set	4533  
SLICE8				 		.set	5181  
SLICE9				 		.set	5829  
SLICE10						.set	6476  
SLICE12						.set	7124  
GRID_SLICE_LEVEL 			.set    512    
 .else      ;* OP_POINT=8
SLICE1_T1_SCALE				.set	1296 
SLICE0				 		.set	0
SLICE1				 		.set	81	
SLICE2				 		.set	(SLICE1*2)
SLICE3				 		.set	(SLICE1*3)
SLICE4				 		.set	(SLICE1*4)
SLICE5				 		.set	(SLICE1*5)
SLICE6				 		.set	(SLICE1*6)
SLICE7				 		.set	(SLICE1*7)
SLICE8				 		.set	(SLICE1*8)
SLICE9				 		.set	(SLICE1*9)
SLICE10						.set	(SLICE1*10)
SLICE12						.set	(SLICE1*12)
GRID_SLICE_LEVEL 			.set    64
 .endif		;* OP_POINT=8

GRID_SLICE_COEF1			.set    25891	;* GRID_SLICE_LEVEL/SLICE1
GRID_SLICE_COEF2			.set    20736	;* (1/2)*SLICE1/GRID_SLICE_LEVEL 
PM_SHIFT					.set	(16-(20-2*OP_POINT_SHIFT))
WHAT_TABLE_COEF				.set	160
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 

 .if $isdefed("XDAIS_API")
	.global TCM_MESI_RxInitTCM
	.global TCM_MESI_TCMslicer
	.global TCM_MESI_TCMDecoder
 .else
	.global Rx_init_TCM
	.global TCM_slicer
	.global TCM_decoder
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_slicerReturn
	.asg	COMMON_MESI_slicerReturn, slicer_return
	.global COMMON_MESI_decoderReturn
	.asg	COMMON_MESI_decoderReturn, decoder_return
 .else
	.global slicer_return
	.global decoder_return
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global TCM_slicer_endif1	
	.global TCM_slicer_endif2	
	.global TCM_slicer_endif3	
	.global TCM_slicer_endif4	
	.global TCM_slicer_endif5
	.global TCM_slicer_else6  
	.global TCM_slicer_endif6	
	.global TCM_slicer_else7	
	.global TCM_slicer_endif7	
	.global TCM_slicer_else8	
	.global TCM_slicer_endif8
	.global grid_slicer
 .endif										;* "XDAIS_API endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

;****************************************************************************
;* tables and coefficients
;****************************************************************************

	.sect   "vcoefs"

;****************************************************************************

TCM_signal_map_7200:
 .if (TCM_7200=ENABLED)
 .word  T6,-T6, -T2, T2, -T6, T6,  T2,-T2,  T6, T2, -T2,-T6, -T6,-T2,  T2, T6
 .word -T2, T6,  T6,-T2,  T2,-T6, -T6, T2, -T6,-T6,  T2, T2,  T6, T6, -T2,-T2
 .endif

TCM_signal_map_9600:
 .if (TCM_9600=ENABLED)
 .word -T8, T2,  T0,-T6,  T0, T2,  T8, T2,  T8,-T2,  T0, T6,  T0,-T2, -T8,-T2
 .word -T4, T6, -T4,-T2,  T4, T6,  T4,-T2,  T4,-T6,  T4, T2, -T4,-T6, -T4, T2
 .word -T6,-T4,  T2,-T4, -T6, T4,  T2, T4,  T6, T4, -T2, T4,  T6,-T4, -T2,-T4
 .word  T2, T8, -T6, T0,  T2, T0,  T2,-T8, -T2,-T8,  T6, T0, -T2, T0, -T2, T8
 .endif

TCM_signal_map_12000:
 .if (TCM_12000=ENABLED)
 .word  T7, T1,  T3, T5,  T7,-T7, -T5, T5,  T3,-T3, -T1, T1, -T1,-T7, -T5,-T3
 .word -T7,-T1, -T3,-T5, -T7, T7,  T5,-T5, -T3, T3,  T1,-T1,  T1, T7,  T5, T3
 .word -T1, T5, -T5, T1,  T7, T5, -T5,-T7,  T3, T1, -T1,-T3,  T7,-T3,  T3,-T7
 .word  T1,-T5,  T5,-T1, -T7,-T5,  T5, T7, -T3,-T1,  T1, T3, -T7, T3, -T3, T7
 .word -T5,-T1, -T1,-T5, -T5, T7,  T7,-T5, -T1, T3,  T3,-T1,  T3, T7,  T7, T3
 .word  T5, T1,  T1, T5,  T5,-T7, -T7, T5,  T1,-T3, -T3, T1, -T3,-T7, -T7,-T3
 .word  T1,-T7,  T5,-T3, -T7,-T7,  T5, T5, -T3,-T3,  T1, T1, -T7, T1, -T3, T5
 .word -T1, T7, -T5, T3,  T7, T7, -T5,-T5,  T3, T3, -T1,-T1,  T7,-T1,  T3,-T5
 .endif

TCM_signal_map_14400:
 .if (TCM_14400=ENABLED)
 .word -T8,-T3,  T8,-T3,  T4,-T3,  T4,-T7, -T4,-T3, -T4,-T7,  T0,-T3,  T0,-T7
 .word -T8, T1,  T8, T1,  T4, T1,  T4, T5, -T4, T1, -T4, T5,  T0, T1,  T0, T5
 .word  T8, T3, -T8, T3, -T4, T3, -T4, T7,  T4, T3,  T4, T7,  T0, T3,  T0, T7
 .word  T8,-T1, -T8,-T1, -T4,-T1, -T4,-T5,  T4,-T1,  T4,-T5,  T0,-T1,  T0,-T5
 .word  T2,-T9,  T2, T7,  T2, T3,  T6, T3,  T2,-T5,  T6,-T5,  T2,-T1,  T6,-T1
 .word -T2,-T9, -T2, T7, -T2, T3, -T6, T3, -T2,-T5, -T6,-T5, -T2,-T1, -T6,-T1
 .word -T2, T9, -T2,-T7, -T2,-T3, -T6,-T3, -T2, T5, -T6, T5, -T2, T1, -T6, T1
 .word  T2, T9,  T2,-T7,  T2,-T3,  T6,-T3,  T2, T5,  T6, T5,  T2, T1,  T6, T1
 .word  T9, T2, -T7, T2, -T3, T2, -T3, T6,  T5, T2,  T5, T6,  T1, T2,  T1, T6
 .word  T9,-T2, -T7,-T2, -T3,-T2, -T3,-T6,  T5,-T2,  T5,-T6,  T1,-T2,  T1,-T6
 .word -T9,-T2,  T7,-T2,  T3,-T2,  T3,-T6, -T5,-T2, -T5,-T6, -T1,-T2, -T1,-T6
 .word -T9, T2,  T7, T2,  T3, T2,  T3, T6, -T5, T2, -T5, T6, -T1, T2, -T1, T6
 .word -T3, T8, -T3,-T8, -T3,-T4, -T7,-T4, -T3, T4, -T7, T4, -T3, T0, -T7, T0
 .word  T1, T8,  T1,-T8,  T1,-T4,  T5,-T4,  T1, T4,  T5, T4,  T1, T0,  T5, T0
 .word  T3,-T8,  T3, T8,  T3, T4,  T7, T4,  T3,-T4,  T7,-T4,  T3, T0,  T7, T0
 .word -T1,-T8, -T1, T8, -T1, T4, -T5, T4, -T1,-T4, -T5,-T4, -T1, T0, -T5, T0
 .endif

 .if (TCM_ENCODER=ENABLED)
Tx_TCM_diff_table:
 .word  0e4h,0b1h,01eh,04bh
 .endif

 .if (TCM_DECODER=ENABLED)
Rx_TCM_diff_table:
 .word  0b4h,0e1h,04eh,01bh

;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;TCM_What_table:
; .word  32767, 20480, 10240, 6827,  5120,  4096,  3413,  2926 
; .word  2560,  2276,  2048,  1862,  1707,  1575,  1463,  1365 
; .word  1280,  1205,  1138,  1078,  1024,   975,   931,   890 
; .word	853,   819,   788,   759,   731,   706,   683,   661 
; .word	640,   621,   602,   585,   569,   554,   539,   525 
; .word	512,   500,   488,   476,   465,   455,   445,   436 
; .word	427,   418,   410,   402,   394,   386,   379,   372 
; .word	366,   359,   353,   347,   341,   336,   330,   325 
; .word	320,   315,   310,   306,   301,   297,   293,   288 
; .word	284,   281,   277,   273,   269,   266,   263,   259 
; .word	256,   253,   250,   247,   244,   241,   238,   235 
; .word	233,   230,   228,   225,   223,   220,   218,   216 
; .word	213,   211,   209
;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
 .if $isdefed("SQUARE_ROOT_WHAT")
TCM_What_table:
 .if OP_POINT == OP_POINT8
 .word  32767, 25905, 18318, 14956, 12953, 11585, 10576,  9791
 .word   9159,  8635,  8192,  7811,  7478,  7185,  6924,  6689
 .word   6476,  6283,  6106,  5943,  5793,  5653,  5523,  5402
 .word	 5288,  5181,  5080,  4985,  4896,  4811,  4730,  4653
 .word	 4579,  4510,  4443,  4379,  4318,  4259,  4202,  4148
 .word	 4096,  4046,  3997,  3951,  3905,  3862,  3820,  3779
 .word	 3739,  3701,  3664,  3627,  3592,  3558,  3525,  3493
 .word	 3462,  3431,  3402,  3373,  3344,  3317,  3290,  3264
 .word	 3238,  3213,  3189,  3165,  3141,  3119,  3096,  3074
 .word	 3053,  3032,  3011,  2991,  2972,  2952,  2933,  2915
 .word	 2896,  2878,  2861,  2843,  2827,  2810,  2793,  2777
 .word	 2762,  2746,  2731,  2716,  2701,  2686,  2672,  2658
 .word	 2644,  2630,  2617
 .else      ;* OP_POINT=8
 .word  32767,  3238,  2290,  1870,  1619,  1448,  1322,  1224
 .word   1145,  1079,  1024,   976,   935,   898,   865,   836
 .word    810,   785,   763,   743,   724,   707,   690,   675
 .word	  661,   648,   635,   623,   612,   601,   591,   582
 .word	  572,   564,   555,   547,   540,   532,   525,   519
 .word	  512,   506,   500,   494,   488,   483,   477,   472
 .word	  467,   463,   458,   453,   449,   445,   441,   437
 .word	  433,   429,   425,   422,   418,   415,   411,   408
 .word	  405,   402,   399,   396,   393,   390,   387,   384
 .word	  382,   379,   376,   374,   371,   369,   367,   364
 .word	  362,   360,   358,   355,   353,   351,   349,   347
 .word	  345,   343,   341,   339,   338,   336,   334,   332
 .word	  330,   329,   327 
 .endif		;* OP_POINT=8
 .else		;* SQUARE_ROOT_WHAT
TCM_What_table:
 .word  32767, 20480, 10240,  6827,  5120,  4096,  3413,  2926
 .word   2560,  2276,  2048,  1862,  1707,  1575,  1463,  1365
 .word   1280,  1205,  1138,  1078,  1024,   975,   931,   890
 .word	  853,   819,   788,   759,   731,   706,   683,   661
 .word	  640,   621,   602,   585,   569,   554,   539,   525
 .word	  512,   500,   488,   476,   465,   455,   445,   436
 .word	  427,   418,   410,   402,   394,   386,   379,   372
 .word	  366,   359,   353,   347,   341,   336,   330,   325
 .word	  320,   315,   310,   306,   301,   297,   293,   288
 .word	  284,   281,   277,   273,   269,   266,   263,   259
 .word	  256,   253,   250,   247,   244,   241,   238,   235
 .word	  233,   230,   228,   225,   223,   220,   218,   216
 .word	  213,   211,   209                                   
 .endif		;* SQUARE_ROOT_WHAT
;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
 
 .endif

;****************************************************************************

	.sect		"vtext"

;****************************************************************************

	;*****************************
	;**** TCM encoder modules ****
	;*****************************

 .if TCM_ENCODER=ENABLED
;****************************************************************************
;* Tx_init_TCM: initializes Tx_block for TCM encoder/diff encoder.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_TCM:					 
TCM_MESI_TxInitTCM:
	LD		Tx_rate,B
	SUB		#TCM_RATE_12000,B,A
	 ST		#TCM_signal_map_14400,Tx_amp_ptr
	 ST		#6,Tx_Nbits
	ST		#3fh,Tx_Nmask
	XC		2,AEQ  
	 ST		#TCM_signal_map_12000,Tx_amp_ptr
	XC		2,AEQ  
	 ST		#5,Tx_Nbits
	XC		2,AEQ  
	 ST		#1fh,Tx_Nmask
	SUB		#TCM_RATE_9600,B,A
	SUB		#TCM_RATE_7200,B
	 ST		#0,Ereg 
	XC		2,AEQ  
	 ST		#TCM_signal_map_9600,Tx_amp_ptr
	XC		2,AEQ  
	 ST		#4,Tx_Nbits
	XC		2,AEQ  
	 ST		#0fh,Tx_Nmask
	XC		2,BEQ  
	 ST		#TCM_signal_map_7200,Tx_amp_ptr
	XC		2,BEQ  
	 ST		#3,Tx_Nbits
	XC		2,BEQ  
	 ST		#07h,Tx_Nmask
	RET_

;****************************************************************************
;* TCM_encoder: differential encoder, convolutional encoder, and signal map	
;* Expects the following on entry:
;*	DP=&Tx_block
;*	B=symbol
;* Modifies:
;*	AR0,AR1,AR3,AR4,AR6,AR7
;****************************************************************************

TCM_encoder:					
TCM_MESI_TCMencoder:

	;**** differential encoder ****

	LD		#2,A
	SUB		Tx_Nbits,A						;* A=2-Tx_Nbits
	STLM	A,T
	STM		AR0,AR7							;* AR7=&AR0
	STLM	B,*AR7							;* AR0=symbol
	NORM	B,A								;* A=symbol>>(Nbits-2)
 .if ON_CHIP_COEFFICIENTS=ENABLED
	ADD		#Tx_TCM_diff_table,A			;* A=&TCM_diff_table[.]
	STLM	A,AR6
	 LD		Tx_phase,1,B
	 NEG	B
	LD		*AR6,A
	STL		A,*AR7(1)
 .else
	ADD		#Tx_TCM_diff_table,A			;* A=&TCM_diff_table[.]
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7(1)
	LD		Tx_phase,1,B
	NEG		B
 .endif
	STLM	B,T								;* T=-2*Tx_phase
	 NOP
	LD		*AR7(1),TS,B					;* B=diff_table[.]>>2*phase
	AND		#(TX_TCM_PHASE_LEN-1),B
	STL		B,Tx_phase						;* phase=(diff_table[.])&(PHASE_LEN-1)

	;**** convolutional encoder ****

	AND		#2,B,A				
	STL		A,-1,*AR7(Y1)					;* Y1=(phase>>1)&1
	AND		#1,B,A				
	STL		A,*AR7(Y2)						;* Y2=(phase>>0)&1
	LD		Ereg,B
	AND		#4,B,A				
	STL		A,-2,*AR7(D2)					;* D2=(Ereg>>2)&1
	AND		#2,B,A				
	STL		A,-1,*AR7(D1)					;* D1=(Ereg>>1)&1
	AND		#1,B,A				
	STL		A,*AR7(D0)						;* D0=(Ereg>>0)&1

	LD		*AR7(D1),B
	XOR		*AR7(Y2),B						;* B= k=D1^Y2
	LD		*AR7(D0),A
	AND		B,A								;* A=D0&k	
	XOR		*AR7(Y2),A						;* A=Y2^(D0&k)
	XOR		*AR7(Y1),A						;* A=Y1^Y2^(D0&k)
	XOR		*AR7(D2),A						;* A=D2^Y1^Y2^(D0&k)
	STL		A,*AR7(D1)						;* D1=D2^Y1^Y2^(D0&k)
	LD		*AR7(D0),A
	STL		A,*AR7(D2)						;* D2=D0
	STL		A,2,Ereg						;* Ereg= Y0=D2<<2
	AND		*AR7(Y1),A						;* A=Y1&D0
	XOR		B,A								;* A=k^(Y1&D0)
	STL		A,*AR7(D0)						;* D0=k^(Y1&D0)
	LD		*AR7(D1),1,A					;* A=D1<<1
	OR		Ereg,A							;* A=Y0|(D1<<1)
	OR		*AR7(D0),A						;* A=Y0|(D1<<1)|D0
	STL		A,Ereg							;* Ereg=Y0|(D1<<1)|D0

	;**** signal mapper (Y0Y1Y2Q3Q4Q5Q6) ****

	LD		Tx_Nbits,B
	SUB		#2,B
	STLM	B,T								;* T=(Nbits-2)
	AND		#4,A							;* A=Y0
	OR		Tx_phase,A						;* A=Tx_phase|Y0
	NORM	A								;* A=A<<(Nbits-2)
	LD		Tx_Nmask,-2,B
	AND		*AR7,B							;* B=symbol&(Tx_Nmask)>>2
	OR		B,A								;* A|=symbol&(Tx_Nmask)>>2
	SFTL	A,1								;* A=2*k
	ADDS	Tx_amp_ptr,A					;* A=Tx_amp_ptr+2*k
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR6
	 LD		Tx_symbol_counter,B
	 AND	#3,B							;* B=Tx_symbol_counter&3
	LD		*AR6+,A
	STL		A,*AR7							;* AR0=*(Tx->amp_ptr+2*k)
	STL		A,*AR7(3)						;* AR3=*(Tx->amp_ptr+2*k)
	LD		*AR6,A
	STL		A,*AR7(1)						;* AR1=*(Tx->amp_ptr+2*k+1)
	STL		A,*AR7(4)						;* AR4=*(Tx->amp_ptr+2*k+1)
 .else
	LD		Tx_symbol_counter,B
	AND		#3,B							;* B=Tx_symbol_counter&3
	READA	*AR7							;* AR7=*(Tx->amp_ptr+2*k)
	READA	*AR7(3)							;* AR7=*(Tx->amp_ptr+2*k)
	ADD		#1,A
	READA	*AR7(1)							;* AR7=*(Tx->amp_ptr+2*k+1)
	READA	*AR7(4)							;* AR7=*(Tx->amp_ptr+2*k+1)
 .endif

	LD		*AR7,A				
	XC		2,BEQ							;* if symbol_counter&3=0 ...
	 NEG	A					
	 STLM	A,AR4							;* AR4= -*(Tx->amp_ptr+2*k)
	LD		*AR7(1),A
	XC		1,BEQ							;* if symbol_counter&3=0 ...
	 STLM	A,AR3							;* AR3= *(Tx->amp_ptr+2*k+1)

	SUB		#1,B	
	NOP
	LD		*AR7,A				
	XC		2,BEQ							;* if symbol_counter&3=1 ...
	 NEG	A					
	 STLM	A,AR3							;* AR3= -*(Tx->amp_ptr+2*k)
	LD		*AR7(1),A
	XC		2,BEQ							;* if symbol_counter&3=1 ...
	 NEG	A					
	 STLM	A,AR4							;* AR4= -*(Tx->amp_ptr+2*k+1)
	 
	SUB		#1,B	
	NOP
	LD		*AR7,A				
	XC		1,BEQ							;* if symbol_counter&3=2 ...
	 STLM	A,AR4							;* AR4= *(Tx->amp_ptr+2*k)
	LD		*AR7(1),A
	XC		2,BEQ							;* if symbol_counter&3=2 ...
	 NEG	A					
	 STLM	A,AR3							;* AR3= -*(Tx->amp_ptr+2*k+1)

	LD		Tx_fir_scale,T
	MVDK	Tx_fir_head,AR6		
	MVDK	Tx_fir_len,BK
	MPY		*AR7(3),A						;* A=fir_scale*I
	MPY		*AR7(4),B						;* B=fir_scale*Q
	STH		A,*AR6+%						;* Tx_fir[*++%]=I*scale
	STH		B,*AR6+%						;* Tx_fir[*++%]=Q*scale
	RETD_
	 MVKD	AR6,Tx_fir_head

;****************************************************************************
 .endif

	;*****************************
	;**** TCM decoder modules ****
	;*****************************

 .if TCM_DECODER=ENABLED
;****************************************************************************
;* Rx_init_TCM: initializes Rx_block for TCM dencoder.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_TCM:					 
TCM_MESI_RxInitTCM:

	;**** initialize state_metrics[] ****
	
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(decoder_start),AR1
	MAR		*+AR1(state_metrics_start)
	STM		#(DELAY_STATES-1),BRC
	LD		#0,A
	RPTB	init_state_metrics_loop
init_state_metrics_loop:
	 STL	A,*AR1+							;* state_metrics[*++]=0

	;**** initialize trace_back[] ****

	MVDK	*AR0(trace_back_start),AR1		;* AR=&trace_back[0]
	MVKD	AR1,trace_back_ptr				;* trace_back_ptr=&trace_back[0][0]
	STM		#(TRACE_BACK_BUF_LEN-1),BRC
	RPTB	init_trace_back_loop
init_trace_back_loop:
	 STL	A,*AR1+							;* trace_back[*++]=0

	;**** initialize rate-dependent parameters ****

	LD		Rx_rate,B
	SUB		#TCM_RATE_12000,B,A
	 ST		#TCM_signal_map_14400,signal_map_ptr
	 ST		#6,Rx_Nbits
	ST		#3fh,Rx_Nmask
	XC		2,AEQ
	 ST		#TCM_signal_map_12000,signal_map_ptr
	XC		2,AEQ
	 ST		#5,Rx_Nbits
	XC		2,AEQ
	 ST		#1fh,Rx_Nmask
	SUB		#TCM_RATE_9600,B,A
	SUB		#TCM_RATE_7200,B
	 NOP
	XC		2,AEQ
	 ST		#TCM_signal_map_9600,signal_map_ptr
	XC		2,AEQ
	 ST		#4,Rx_Nbits
	XC		2,AEQ
	 ST		#0fh,Rx_Nmask
	XC		2,BEQ
	 ST		#TCM_signal_map_7200,signal_map_ptr
	XC		2,BEQ
	 ST		#3,Rx_Nbits
	XC		2,BEQ
	 ST		#07h,Rx_Nmask
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	RET_
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)

;****************************************************************************
;* TCM_slicer: Configures grid slicer parameters for constellations 		
;* corresponding to the different TCM rates, and calls grid_slicer() for 	
;* I and Q signals. For modified cross constellations (32 and 128 QAM) the 	
;* constellation is rotated by 45 degrees to map it into a rectangular grid	
;* The inverse power scale, What, is then calculated from the sliced		
;* estimates Ihat and Qhat.													
;* On entry it expects the following setup:
;*	DP=&Rx_block
;* Modifies:
;* On return:
;****************************************************************************

TCM_slicer:
TCM_MESI_TCMslicer:

	;**** set up grid slicer parameters for different rates ****
	
	ST		#2,temp0
	ST		#6*SLICE1,temp1
	CMPM	Rx_rate,#TCM_RATE_9600
	BCD_	TCM_slicer_endif1,NTC			;* branch if !TCM_RATE_9600
	 CMPM	Rx_rate,#TCM_RATE_12000
	ST		#10*SLICE1,temp1
TCM_slicer_endif1:	
	BCD_	TCM_slicer_endif2,NTC			;* branch if !TCM_RATE_12000
	 CMPM	Rx_rate,#TCM_RATE_14400
	ST		#1,temp0
	ST		#7*SLICE1,temp1
TCM_slicer_endif2:	
	BCD_	TCM_slicer_endif3,NTC			;* branch if !TCM_RATE_14400
	 LD		Iprime,A
	 LD		Qprime,B
	ST		#1,temp0
	ST		#11*SLICE1,temp1
TCM_slicer_endif3:	

	;**** rotate if modified-cross constellation ****

	CMPM	Rx_rate,#TCM_RATE_7200
	BCD_	TCM_slicer_endif4,TC			;* branch if TCM_RATE_7200
	 CMPM	Rx_rate,#TCM_RATE_12000
	BC_		TCM_slicer_endif4,TC			;* branch if TCM_RATE_12000
	SUB		Qprime,A						;* A=Iprime-Qprime
	ADD		Iprime,B						;* B=Iprime+Qprime
TCM_slicer_endif4:	

	;**** call grid slicers for I and Q ****
	
	CALLD_	grid_slicer
	 STLM	A,T								;* T=Iin
	 STL	B,Q								;* Rx_Q=Qin

	CALLD_	grid_slicer
	 LD		Q,T								;* T=Qin
	 STL	A,Ihat							;* Ihat=Iout
	LD		Ihat,B

	;**** de-rotate if modified-cross constellation ****
	
	CMPM	Rx_rate,#TCM_RATE_7200
	BCD_	TCM_slicer_endif5,TC			;* branch if TCM_RATE_7200
	 CMPM	Rx_rate,#TCM_RATE_12000
	BC_		TCM_slicer_endif5,TC			;* branch if TCM_RATE_12000
	ADD		A,B		 						;* B=Iout+Qout
	SFTA	B,-1,B                          ;* B=(Iout+Qout)>>1
	SUB		Ihat,A                          ;* A=-Iout+Qout     
	SFTA	A,-1,A                          ;* A=(-Iout+Qout)>>1
TCM_slicer_endif5:
	STL		B,Ihat	
	STL		A,Qhat	

	;**** calculate inverse power scale, What ****

 NOP
 NOP
 NOP
	SQUR	Ihat,A
	SQURA	Qhat,A							;* A=Ihat^2+Qhat^2
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	STH		A,10,temp0						;* temp0=(Ihat^2+Qhat^2)>>(15-5)
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	STH		A,(15-(17-2*OP_POINT_SHIFT)),temp0	;* temp0=(Ihat^2+Qhat^2)>>(15-(17-2*OP_POINT_SHIFT))
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	MPY		temp0,#WHAT_TABLE_COEF,A		;* (Ihat^2+Qhat^2)*WHAT_TABLE_COEF
	STH		A,What							;* What=k

	;**** compensate for 32 modified-cross corners ****

	CMPM	Rx_rate,#TCM_RATE_9600
	BCD_	TCM_slicer_endif6,NTC			;* branch if !TCM_RATE_9600
	 CMPM	What,#100
	BCD_	TCM_slicer_endif6,NTC			;* branch if k!=100
	 CMPM	Qhat,#0
	BCD_	TCM_slicer_else6,NTC			;* branch if Qhat!=0
	 ST		#68,What						;* k=68
	 
;++++#ifndef MESI_INTERNAL 03-27-2001
;	ST		#8*SLICE1,Ihat
;	ST		#2*SLICE1,Qhat
;	LD		Iprime,B
;	 LD		Ihat,A
;	 NEG	A
;	BD_		TCM_slicer_endif6
;	 XC		1,BLT  							;* if Iprime<0...
;	  STL	A,Ihat							;* ...Ihat=-Ihat
;TCM_slicer_else6:	
;	ST		#2*SLICE1,Ihat
;	ST		#8*SLICE1,Qhat
;	LD		Qprime,B
;	 LD		Qhat,A
;	 NEG	A
;	XC		1,BLT							;* if Qprime<0...
;	 STL	A,Qhat							;* ...Qhat=-Qhat
;TCM_slicer_endif6:	
;++++#else   MESI_INTERNAL 03-27-2001
	ST		#8*SLICE1,Ihat
	BD_		TCM_slicer_continue6
	 ST		#2*SLICE1,Qhat
TCM_slicer_else6:	
	ST		#2*SLICE1,Ihat
	ST		#8*SLICE1,Qhat
	
TCM_slicer_continue6:	
	LD		Iprime,B
	 LD		Ihat,A
	 NEG	A
	XC		1,BLT  							;* if Iprime<0...
	 STL	A,Ihat							;* ...Ihat=-Ihat
	LD		Qprime,B
	 LD		Qhat,A
	 NEG	A
	XC		1,BLT							;* if Qprime<0...
	 STL	A,Qhat							;* ...Qhat=-Qhat
TCM_slicer_endif6:	
;++++#endif  MESI_INTERNAL 03-27-2001
	
	;**** compensate for 128 modified-cross corners ****

	CMPM	Rx_rate,#TCM_RATE_14400
	BCD_	TCM_slicer_endif8,NTC			;* branch if !TCM_RATE_14400
	 LD		#81,B
	 SUB	What,B
	BCD_	TCM_slicer_endif8,BGT			;* branch if k<81
	 CMPM	What,#81
	BCD_	TCM_slicer_endif7,NTC			;* branch if k!=81
	 CMPM	Qhat,#0
	BCD_	TCM_slicer_else7,NTC			;* branch if Qhat!=0
	 ST		#65,What						;* k=65
	 
;++++#ifndef MESI_INTERNAL 03-27-2001
;	ST		#1*SLICE1,Qhat
;	ST		#8*SLICE1,Ihat
;	LD		Iprime,B
;	 LD		Ihat,A
;	 NEG	A
;	BD_		TCM_slicer_endif7
;	 XC		1,BLT  							;* if Iprime<0...
;	  STL	A,Ihat							;* ...Ihat=-Ihat
;TCM_slicer_else7:	
;	ST		#1*SLICE1,Ihat
;	ST		#8*SLICE1,Qhat
;	LD		Qprime,B
;	 LD		Qhat,A
;	 NEG	A
;	XC		1,BLT							;* if Qprime<0...
;	 STL	A,Qhat							;* ...Qhat=-Qhat
;TCM_slicer_endif7:	
;++++#else   MESI_INTERNAL 03-27-2001
	ST		#1*SLICE1,Qhat
	BD_		TCM_slicer_continue7
	 ST		#8*SLICE1,Ihat
TCM_slicer_else7:	
	ST		#1*SLICE1,Ihat
	ST		#8*SLICE1,Qhat

TCM_slicer_continue7:	
	LD		Iprime,B
	 LD		Ihat,A
	 NEG	A
	XC		1,BLT  							;* if Iprime<0...
	 STL	A,Ihat							;* ...Ihat=-Ihat
	LD		Qprime,B
	 LD		Qhat,A
	 NEG	A
	XC		1,BLT							;* if Qprime<0...
	 STL	A,Qhat							;* ...Qhat=-Qhat
TCM_slicer_endif7:	
;++++#endif  MESI_INTERNAL 03-27-2001

	LD		#101,B
	SUB		What,B							
	BCD_	TCM_slicer_endif8,BGT			;* branch if k<101
	 LD		Qhat,B
	 ABS	B								;* B=abs(Qhat)
	ST		#2*SLICE1,Ihat
	ST		#9*SLICE1,Qhat
	SUB		#1*SLICE1,B
	BCD_	TCM_slicer_else8,BNEQ			;* branch if abs(Qhat)!=SLICE1
	 ST		#85,What						;* k=85
	ST		#2*SLICE1,Qhat
	ST		#9*SLICE1,Ihat
TCM_slicer_else8:	

	LD		Iprime,B
	 LD		Ihat,A
	 NEG	A
	XC		1,BLT  							;* if Iprime<0...
	 STL	A,Ihat							;* ...Ihat=-Ihat
	LD		Qprime,B
	 LD		Qhat,A
	 NEG	A
	XC		1,BLT							;* if Qprime<0...
	 STL	A,Qhat							;* ...Qhat=-Qhat
TCM_slicer_endif8:	

	;**** look up What ****

	LD		What,A
	ADD		#TCM_What_table,A				;* A=&What_table[k]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	BD_		slicer_return
	 LD		*AR0,A
	 STL	A,What							;* What=TCM_What_table[k]
 .else
	BD_		slicer_return
	 LDM	AL,A							;* clear upper 16 bits for READA
	 READA What								;* What=TCM_What_table[k]
 .endif

;****************************************************************************
;* grid_slicer: Slices input signal "in" on a regular linear grid with		
;* a fixed minimum signal level point and slice level. The signal points 	
;* are mapped to odd integer coordinates and the slicer boundaries are on	
;* at even coordinates. The constellation scaling value "shift" adjusts the	
;* input/output to map to/from this convention. Once the input signal is	
;* scaled to the grid slice level, GRID_SLICE_LEVEL, the LSBs are cropped 	
;* off and a bias is applied to map back onto positive and negative odd		
;* coordinated, and then remapped. The maximum sliced level is supplied in	
;* "max", and the sliced values are saturated to this maximum level.		
;* On entry it expects:
;*	DP=&Rx_block
;*	T=in
;*	Rx_temp0=shift
;*	Rx_temp1=max
;* On return:
;*	A=out
;****************************************************************************

grid_slicer:
	MPY		#GRID_SLICE_COEF1,A				;* A=in*GRID_SLICE_COEF1
	LD		temp0,B
	NEG		B
	STLM	B,T
	 LD		#(-GRID_SLICE_LEVEL),16,B
	NORM	A								;* A=temp>>shift
	AND		B,A                             ;* A&=-GRID_SLICE_LEVEL
	ADD		#(GRID_SLICE_LEVEL/2),16,A		;* A+=GRID_SLICE_LEVEL/2
	STM		#GRID_SLICE_COEF2,T
	MPYA	A								;* A*=GRID_SLICE_COEF2
	LD		temp0,T
	 NOP
	 NOP
	NORM	A								;* A=temp<<shift
	SFTA	A,-15,A							;* MPY by 2 and shift to AL

	;**** saturate at outer boundaries ****

	SUB		temp1,A,B
	 NOP		
	 NOP
	XC		1,BGT							;* if out > max ...
	 LD		temp1,A							;* ... A=max
	ADD		temp1,A,B
	 NOP		
	 NOP
	XC		2,BLT							;* if out < -max ...
	 LD		temp1,A		
	 NEG	A								;* ... A=-max
	RET_

;****************************************************************************
;* TCM_decoder: The 4 paths entering each delay state are added to the 		
;* accumulated distances for the linked delay states, and the smallest sum	
;* is selected. The path state and it's associated delay state are stored 	
;* in the trace back table, and the accumulated distances are updated. 		
;* The trace back array elements are packed with the predecessor state and	
;* the minimum distance decision separated into sliced estimate and group	
;* as follows:																
;*		bits 14-12: S0, S1, S2												
;*		bits  11-9: Y0, Y1, Y2												
;*		bits   8-0: Q3-Q9													
;* On entry to this routine, the sliced estimates are stored in 			
;* min_distance[] and are aligned with bit 8 already. The number of bits	
;* depends on the rate.														
;* The decoded bits Y1Y2Q3..Qn are stored in "Rx->Phat".					
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

TCM_decoder:
TCM_MESI_TCMDecoder:

	;**** calculate the path metrics ****

	MVDK	Rx_start_ptrs,AR4			
	MVDK	*AR4(Rx_block_start),AR3		;* AR3=&Rx_block
	MAR		*+AR3(Iprime)					;* AR3=&Iprime
	MVDK	*AR4(decoder_start),AR5
	MAR		*+AR5(path_metrics_start)		;* AR5=&path_metrics[0]
	MVDK	*AR4(decoder_start),AR6
	MAR		*+AR6(path_bits_start)			;* AR6=&path_bits[0]
	MVDK	*AR4(Rx_block_start),AR4		;* AR4=&Rx_block
	MAR		*+AR4(temp0)					;* AR4=&temp0

	LD		#(PM_SHIFT-1),ASM
 .if (TCM_7200=ENABLED)
	LD		Rx_rate,B
	SUB		#TCM_RATE_7200,B
	CC_		path_metrics_7200,BEQ			;* if rate=7200, call path_metrics
 .endif
 .if (TCM_9600=ENABLED)
	LD		Rx_rate,B
	SUB		#TCM_RATE_9600,B
	CC_		path_metrics_9600,BEQ			;* if rate=9600, call path_metrics
 .endif
 .if (TCM_12000=ENABLED)
	LD		Rx_rate,B
	SUB		#TCM_RATE_12000,B
	CC_		path_metrics_12000,BEQ			;* if rate=12000, call path_metrics
 .endif
 .if (TCM_14400=ENABLED)
	LD		Rx_rate,B
	SUB		#TCM_RATE_14400,B
	CC_		path_metrics_14400,BEQ			;* if rate=14400, call path_metrics
 .endif
	LD		#0,ASM

	;**** dump_write debugging facility - enable to view signals ****

;	MVDK	Rx_start_ptrs,AR4			
;	MVDK	*AR4(decoder_start),AR5
;	MAR		*+AR5(path_metrics_start)
;	MVDK	*AR4(decoder_start),AR6
;	MAR		*+AR6(path_bits_start)
;	STM		#8-1,BRC
;	RPTB	dump_loop
;	LD		*AR6+,A
;	CALL_	dump_write
;	LD		*AR5+,A
;	CALL_	dump_write
;dump_loop:
;	NOP

	STM		#TRACE_BACK_BUF_LEN,BK
	MVDK	Rx_start_ptrs,AR3			
	MVDK	trace_back_ptr,AR7				;* AR7=trace_back_ptr
	MVDK	*AR3(decoder_start),AR6
	MAR		*+AR6(min_distance_start)		;* AR6= MD=&min_distance[0]
	MVDK	*AR3(decoder_start),AR5
	MAR		*+AR5(path_metrics_start)		;* AR5= PM=&path_metrics[0]
	STM		AR0,AR3							;* AR3=&AR0

	;**** delay state 0 ****

DS0: 
	MVDK	Rx_start_ptrs,AR4
	MVDK	*AR4(decoder_start),AR4
	MAR		*+AR4(state_metrics_start)		;* AR4=&state_metrics[0]	
	ADD		*AR4+,*AR5,B					;* B= Dmin=*SM++ + *(PM+0)
	ST		#0,*AR7							;* StatePath=0
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+0)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(2),16,B					;* B= Di=*SM++ + *(PM+2)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(3),16,B					;* B= Di=*SM+ + *(PM+3)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(1<<12)|(2<<9),*AR7			;* if Di<Dmin StatePath=(1<<12)|(2<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(1),16,B					;* B= Di=*SM+ + *(PM+3)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(2<<12)|(3<<9),*AR7			;* if Di<Dmin StatePath=(2<<12)|(3<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(4),16,B					;* B= Di=*SM+ + *(PM+4)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(3<<12)|(1<<9),*AR7			;* if Di<Dmin StatePath=(3<<12)|(3<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 1 ****

DS1: 
	ST		#(4<<12)|(4<<9),*AR7			;* StatePath=(4<<12)|(4<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+4)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(7),16,B					;* B= Di=*SM++ + *(PM+7)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(6),16,B					;* B= Di=*SM+ + *(PM+6)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(5<<12)|(7<<9),*AR7			;* if Di<Dmin StatePath=(5<<12)|(7<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(5),16,B					;* B= Di=*SM+ + *(PM+5)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(6<<12)|(6<<9),*AR7			;* if Di<Dmin StatePath=(6<<12)|(6<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	MVDK	Rx_start_ptrs,AR4
	MVDK	*AR4(decoder_start),AR4
	MAR		*+AR4(state_metrics_start)		;* AR4=&state_metrics[0]	
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(2),16,B					;* B= Di=*SM+ + *(PM+2)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(7<<12)|(5<<9),*AR7			;* if Di<Dmin StatePath=(7<<12)|(5<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 2 ****

DS2: 
	ST		#(0<<12)|(2<<9),*AR7			;* StatePath=(0<<12)|(2<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+2)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(0),16,B					;* B= Di=*SM++ + *(PM+0)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(1),16,B					;* B= Di=*SM+ + *(PM+1)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(1<<12)|(0<<9),*AR7			;* if Di<=Dmin StatePath=(1<<12)|(0<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(3),16,B					;* B= Di=*SM+ + *(PM+3)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(2<<12)|(1<<9),*AR7			;* if Di<Dmin StatePath=(2<<12)|(1<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(7),16,B					;* B= Di=*SM+ + *(PM+7)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(3<<12)|(3<<9),*AR7			;* if Di<Dmin StatePath=(3<<12)|(3<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 3 ****

DS3: 
	ST		#(4<<12)|(7<<9),*AR7			;* StatePath=(4<<12)|(4<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+7)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(4),16,B					;* B= Di=*SM++ + *(PM+4)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(5),16,B					;* B= Di=*SM+ + *(PM+5)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(5<<12)|(4<<9),*AR7			;* if Di<=Dmin StatePath=(5<<12)|(4<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(6),16,B					;* B= Di=*SM+ + *(PM+6)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(6<<12)|(5<<9),*AR7			;* if Di<Dmin StatePath=(6<<12)|(5<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	MVDK	Rx_start_ptrs,AR4
	MVDK	*AR4(decoder_start),AR4
	MAR		*+AR4(state_metrics_start)		;* AR4=&state_metrics[0]	
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(3),16,B					;* B= Di=*SM+ + *(PM+3)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(7<<12)|(6<<9),*AR7			;* if Di<Dmin StatePath=(7<<12)|(6<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 4 ****

DS4: 
	ST		#(0<<12)|(3<<9),*AR7			;* StatePath=(0<<12)|(3<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+2)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(1),16,B					;* B= Di=*SM++ + *(PM+1)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(0),16,B					;* B= Di=*SM+ + *(PM+0)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(1<<12)|(1<<9),*AR7			;* if Di<Dmin StatePath=(1<<12)|(1<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(2),16,B					;* B= Di=*SM+ + *(PM+2)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(2<<12)|(0<<9),*AR7			;* if Di<=Dmin StatePath=(2<<12)|(0<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(5),16,B					;* B= Di=*SM+ + *(PM+5)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(3<<12)|(2<<9),*AR7			;* if Di<Dmin StatePath=(3<<12)|(2<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 5 ****

DS5: 
	ST		#(4<<12)|(5<<9),*AR7			;* StatePath=(4<<12)|(5<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+5)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(6),16,B					;* B= Di=*SM++ + *(PM+6)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(7),16,B					;* B= Di=*SM+ + *(PM+7)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(5<<12)|(6<<9),*AR7			;* if Di<Dmin StatePath=(5<<12)|(6<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(4),16,B					;* B= Di=*SM+ + *(PM+4)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(6<<12)|(7<<9),*AR7			;* if Di<Dmin StatePath=(6<<12)|(7<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	MVDK	Rx_start_ptrs,AR4
	MVDK	*AR4(decoder_start),AR4
	MAR		*+AR4(state_metrics_start)		;* AR4=&state_metrics[0]	
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(1),16,B					;* B= Di=*SM+ + *(PM+1)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(7<<12)|(4<<9),*AR7			;* if Di<=Dmin StatePath=(7<<12)|(4<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 6 ****

DS6: 
	ST		#(0<<12)|(1<<9),*AR7			;* StatePath=(0<<12)|(1<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+1)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(3),16,B					;* B= Di=*SM++ + *(PM+3)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(2),16,B					;* B= Di=*SM+ + *(PM+2)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(1<<12)|(3<<9),*AR7			;* if Di<Dmin StatePath=(1<<12)|(3<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(0),16,B					;* B= Di=*SM+ + *(PM+0)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(2<<12)|(2<<9),*AR7			;* if Di<Dmin StatePath=(2<<12)|(2<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(6),16,B					;* B= Di=*SM+ + *(PM+6)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(3<<12)|(0<<9),*AR7			;* if Di<=Dmin StatePath=(3<<12)|(0<<9)
	MVKD	AR0,*AR6+						;* MD++=Dmin
	MAR		*AR7+							;* TB++

	;**** delay state 7 ****

DS7: 
	ST		#(4<<12)|(6<<9),*AR7			;* StatePath=(4<<12)|(6<<9)
	ST		B,*AR3							;* Dmin=*SM++ + *(PM+6)
||	LD		*AR4+,B							;* B= *SM++ 
	ADD		*AR5(5),16,B					;* B= Di=*SM++ + *(PM+5)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(4),16,B					;* B= Di=*SM+ + *(PM+4)
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(5<<12)|(5<<9),*AR7			;* if Di<Dmin StatePath=(5<<12)|(5<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 ADD	*AR5(7),16,B					;* B= Di=*SM+ + *(PM+7)
	XC		1,ALEQ
	 MVMM	AR1,AR0							;* if Di<=Dmin, Dmin=Di
	XC		2,ALEQ
	 ST		#(6<<12)|(4<<9),*AR7			;* if Di<=Dmin StatePath=(6<<12)|(4<<9)

	SUB		*AR3+,16,B,A					;* A=Di-Dmin
	 ST		B,*AR3-							;* AR1=Di
||	 LD		*AR4+,B							;* B= *SM++ 
	 MVDK	Rx_start_ptrs,AR3
	XC		1,ALT
	 MVMM	AR1,AR0							;* if Di<Dmin, Dmin=Di
	XC		2,ALT
	 ST		#(7<<12)|(7<<9),*AR7			;* if Di<Dmin StatePath=(7<<12)|(6<<7)
	MVKD	AR0,*AR6						;* MD=Dmin

	;**** OR-in the slicer estimated path bits ****

	MVDK	*AR3(decoder_start),AR3
	MAR		*+AR3(path_bits_start)			;* AR3=&path_bits[0]
	STM		#(DELAY_STATES-1),BRC
	RPTB	update_trace_back_loop
	 LD		*AR7,B							;* A=*TB
	 LD		B,-9,A
	 AND	#7,A							;* A=(*TB>>9)&7
	 STLM	A,AR0
	  NOP
	  NOP
	 MAR	*AR3+0
	 OR		*AR3-0,B						;* A=*TB|path_bits[(*TB>>9)&7]
update_trace_back_loop:
	 STL	B,*AR7-%						;* *TB--|= path_bits[(*TB>>9)&7]

	;**** find delay state with minimum accumulated distance ****

	MVDK	Rx_start_ptrs,AR1			
	MVDK	*AR1(decoder_start),AR7
	MAR		*+AR7(min_distance_start)		;* AR7= MD=&min_distance[0]
	STM		#(DELAY_STATES-2),BRC
	STM		#0,AR0							;* AR0=index=0
	STM		#1,AR1
	LD		*AR7+,16,A						;* AH=Dmin
	RPTB	min_distance_loop
	 SUB	*AR7+,16,A,B					;* BH=Dmin-*MD
	  NOP
	  NOP
	 XC		2,BGT
	  MAR	*AR7-
	  LD	*AR7+,16,A						;* AH=*MD
	 XC		2,BGT
	  MVMM AR1,AR0							;* if *MD<Dmin, AR0=i
min_distance_loop:
	  MAR	*AR1+							;* i++

	;**** normalize state_metrics[] ****

	MVDK	Rx_start_ptrs,AR1			
	MVDK	*AR1(decoder_start),AR4
	MAR		*+AR4(min_distance_start)		;* AR4= MD=&min_distance[0]
	MVDK	*AR1(decoder_start),AR3
	MAR		*+AR3(state_metrics_start)		;* AR3=&state_metrics[0]	
	NEG		A								;* A=-Dmin
	ADD		*AR4+,16,A,B					;* B=*MD++ - Dmin
	STM		#(DELAY_STATES-1),BRC
	RPTB	TCM_normalize_loop
TCM_normalize_loop:
	 ST		B,*AR3+							;* *SM++=*MD++ - Dmin
||	 ADD	*AR4+,B						;* B=*MD++ - Dmin

	;**** dump_write debugging facility - enable to view signals ****

;	MVDK	Rx_start_ptrs,AR4			
;	MVDK	*AR4(decoder_start),AR5
;	MAR		*+AR5(path_metrics_start)		;* AR5=&path_metrics[0]
;	MVDK	*AR4(decoder_start),AR6
;	MAR		*+AR6(path_bits_start)			;* AR6=&path_bits[0]
;	MVDK	trace_back_ptr,AR7				;* AR7= k=trace_back_ptr
;	STM		#DELAY_STATES-1,BRC
;	RPTB	dump_loop
;	LD		*AR5+,A
;	CALL_	dump_write
;	LD		*AR6+,A
;	CALL_	dump_write
;	LD		*AR7+,A
;	CALL_	dump_write
;dump_loop:
;	NOP

	;**** trace back over TRACE_BACK_LEN states ****

	MVDK	trace_back_ptr,AR6				;* AR6= k=trace_back_ptr
	STM		#(TRACE_BACK_LEN-2),BRC
	RPTB	trace_back_loop
	 MAR	*AR6+0							;* AR6=trace_back_ptr+index
	 LD		*AR6-0,-12,A					;* A=(trace_back[trace_back_ptr+index])>>12
	 STM	DELAY_STATES,AR0				
	 MAR	*AR6-0%							;* (AR6-DELAY_STATES)%LEN
	 STLM	A,AR0							;* AR0=index
	 NOP
trace_back_loop:
	 NOP
	MAR		*AR6+0							;* AR6=trace_back_ptr+index
	LD		*AR6-0,A						;* A=trace_back[trace_back_ptr+index]
	STL		A,temp0							;* temp0=StatePath
	MVKD	AR6,trace_back_ptr				;* update trace_back_ptr
	LD		Rx_Nbits,B
	SUB		#11,B							;* B=Nbits-11
	STLM	B,T
	 LD		Rx_Nmask,1,B					;* B=2*Nmask
	 ADD	#1,B							;* B=2*Nmask+1
	LD		temp0,TS,A						;* A= j=StatePath>>(11-Nbits)
	AND		B,A								;* A=k=(..)&(2*Nmask+1)
	STL		A,temp0							;* temp0=k
	AND		Rx_Nmask,A						;* A=k & Nmask
	STL		A,Phat							;* Phat=k&Nmask

	;**** differential decoder ****/
   
	LD		#2,B
	SUB		Rx_Nbits,B			
	STLM	B,T								;* T=-(Nbits-2)
	MVDK	Rx_data_head,AR7
	MVDK	Rx_data_len,BK
	LD		Phat,TS,B						;* B=Phat>>(N-2)
	AND		#3,B							;* B=Y1Y2
	ADD		#Rx_TCM_diff_table,B,A			;* A=&diff_table[Y1Y2]		
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	 LD		Rx_phase,1,A					;* A=2*phase
	 NEG	A,A
	STLM	A,T								;* T=-(2*phase)
	 STL	B,Rx_phase						;* Rx_phase=Y1Y2
	 LD		Rx_Nbits,B
	LD		*AR0,TS,A						;* A=diff_table[]>>(2*phase)
	AND		#3,A							;* A=Q1Q2
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR7							;* *AR7=TCM_diff_table[Y1Y2]
	LD		Rx_phase,1,A					;* A=2*phase
	NEG		A,A
	STLM	A,T								;* T=-(2*phase)
	 STL	B,Rx_phase						;* Rx_phase=Y1Y2
	 LD		Rx_Nbits,B
	LD		*AR7,TS,A						;* A=diff_table[]>>(2*phase)
	AND		#3,A							;* A=Q1Q2
 .endif
	SUB		#2,B							;* B=Nbits-2
	STLM	B,T								;* T=Nbits-2
	 LD		Rx_Nmask,-2,B					;* B=Nmask>>2
	 AND	Phat,B							;* B=Phat&(Nmask>>2)
	NORM	A								;* A=Q1Q2<<(Nbits-2)
	OR		B,A								;* A|=Phat&(Nmask>>2)
	STL		A,*AR7+%						;* Rx_data[*++%]=A
	BD_		decoder_return
	 MVKD	AR7,Rx_data_head				;* update Rx_data_head	

;****************************************************************************
;* path_metrics_7200: Computes path_metrics (minimum distances) for 7200
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR3=&I1[]. I1 and Q1 are in contiguous memory
;*	AR4=&SLICE values
;*	AR5=&path_metrics[0]
;*	AR6=&path_bits[0]
;*	AR1=&temp0
;****************************************************************************

 .if (TCM_7200=ENABLED)
path_metrics_7200:
	ST		#SLICE2,*AR4+					;* temp0=SLICE2
	ST		#SLICE4,*AR4-					;* temp1=SLICE4
	STM		#(1<<8),AR0				

	;**** group 0 (45 degrees) ****

	CALLD_	slice_7200
	 SUB	*AR3+,*AR4+,A					;* AH=I1-SLICE2
	 SUB	*AR3-,16,A						;* AH=I1-Q1-SLICE2
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	SUB		*AR5,16,A						;* A=I1-I3-SLICE2
	SQUR	A,B
	ADD		*AR3,*AR4,A						;* A=Q1+SLICE2
	ADD		*AR5,16,A						;* A=Q1+I3+SLICE2
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 1 (-135 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	CALLD_	slice_7200
	 SUB	*AR3,16,A
	 SUB	*AR4+,16,A						;* AH=-I1+Q1-SLICE2
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	ADD		*AR5,16,A						;* A=I1-(-I3-SLICE2)
	SQUR	A,B
	SUB		*AR3-,*AR4+,A					;* A=Q1-SLICE2
	SUB		*AR5,16,A						;* A=Q1-(I3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 2 (-45 degrees) ****
	
	CALLD_	slice_7200
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3+,A
	 ADD	*AR3-,16,A						;* AH=I1+Q1
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	SUB		*AR5,16,A						;* A=I1-(I3+SLICE2)
	SQUR	A,B
	ADD		*AR3-,*AR4+,A					;* A=Q1+SLICE2
	SUB		*AR5,16,A						;* A=Q1-(I3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 3 (+135 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,A
	CALLD_	slice_7200
	 ADD	*AR3-,16,A						;* AH=I1+Q1
	 NEG	A								;* AH=-I1-Q1
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	ADD		*AR5,16,A						;* A=I1-(-I3-SLICE2)
	SQUR	A,B
	SUB		*AR3,*AR4+,A					;* A=Q1-SLICE2
	ADD		*AR5,16,A						;* A=Q1-(-I3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 4 (-135 degrees) ****

	CALLD_	slice_7200
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3-,A
	 SUB	*AR3,16,A						;* AH=-I1+Q1
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	ADD		*AR5,16,A						;* A=I1-(-I3+SLICE2)
	SQUR	A,B
	SUB		*AR3-,*AR4+,A					;* A=Q1-SLICE2
	SUB		*AR5,16,A						;* A=Q1-(I3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 5 (45 degrees) ****

	CALLD_	slice_7200
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3+,A
	 SUB	*AR3-,16,A						;* AH=I1-Q1
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	SUB		*AR5,16,A						;* A=I1-(I3-SLICE2)
	SQUR	A,B
	ADD		*AR3-,*AR4,A					;* A=Q1+SLICE2
	ADD		*AR5,16,A						;* A=Q1-(-I3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 6 (135 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,A							;* AH=Q1
	ADD		*AR3-,16,A
	CALLD_	slice_7200
	 ADD	*AR4+,16,A
	 NEG	A								;* A=-I1-Q1-SLICE1
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	ADD		*AR5,16,A						;* A=I1-(-I3-SLICE2)
	SQUR	A,B
	ADD		*AR3-,*AR4,A					;* A=Q1+SLICE2
	ADD		*AR5,16,A						;* A=Q1-(-I3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2

	;**** group 7 (-45 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,A							;* AH=Q1
	CALLD_	slice_7200
	 ADD	*AR3-,16,A
	 SUB	*AR4+,16,A						;* AH=I1+Q1-SLICE1
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	SUB		*AR5,16,A						;* A=I1-(+I3+SLICE2)
	SQUR	A,B
	SUB		*AR3-,*AR4,A					;* A=Q1-SLICE2
	SUB		*AR5,16,A						;* A=Q1-(I3+SLICE2)
	SQUR	A,A
	RETD_
	ADD		B,A								;* A=I4^2+Q4^2
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3,A							;* dummy

;****************************************************************************
;* slice_7200: Slicer for 7200 bits/sec.				
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=I2
;* Modifies:
;*	 Rx->I,Rx->Q,Rx->Ihat,Rx->Qhat
;* On return:
;*	B= *AR5=I3
;*	*AR6+= *PB++=region
;****************************************************************************

slice_7200:
	LD		*AR4-,16,B						;* A=SLICE4
	 STM	#0,AR7							;* region=0
	XC		2,ALT
	 NEG	B								;* if I2<0, B=-SLICE4
	 MAR	*AR7+0							;* if I2<0, region|=(1<<8)
	STH		B,*AR5							;* *AR5=I3
	RETD_
	 MVKD	AR7,*AR6+						;* *PB++=region
 .endif

;****************************************************************************
;* path_metrics_9600: Computes path_metrics (minimum distances) for 9600
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR3=&I1[]. I1 and Q1 are in contiguous memory
;*	AR5=&path_metrics[0]
;*	AR6=&path_bits[0]
;*	AR1=&temp0
;* Modifies:
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;****************************************************************************

 .if (TCM_9600=ENABLED)
path_metrics_9600:
	ST		#SLICE2,*AR4					;* temp0=SLICE2

	;**** group 0 (180 degrees) ****

	LD		*AR3+,16,A						;* AH=I1
	CALLD_	slice_9600_0167
	 NEG	A								;* AH=-I1
	 SUB	*AR4,*AR3-,B					;* BH=-Q1+SLICE2
	 ADD	*AR3+,16,A						;* A=I1+I3
	SQUR	A,B
	SUB		*AR3-,*AR4,A					;* A=Q1-SLICE2
	ADD		QEQ,16,A						;* A=Q1-(-Q3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 1 (0 degrees) ****

	CALLD_	slice_9600_0167
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3+,A							;* AH=I1
	 ADD	*AR4,*AR3-,B					;* BH=-Q1+SLICE2
	 SUB	*AR3+,16,A						;* A=I3-I1
	SQUR	A,B
	ADD		*AR3-,*AR4,A					;* A=Q1+SLICE2
	SUB		QEQ,16,A						;* A=Q1-(Q3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 2 (-90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,B							;* BH=I1
	CALLD_	slice_9600_2345
	 NEG	B								;* BH=-I1
	 SUB	*AR3-,*AR4,A					;* AH=Q1-SLICE2
	 ADD	*AR3+,16,B,A					;* A=Q3+I1
	SQUR	A,B
	SUB		*AR3-,*AR4,A					;* A=Q1-SLICE2
	SUB		IEQ,16,A						;* A=Q1-(I3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 3 (+90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,B							;* BH=I1
	CALLD_	slice_9600_2345
	 ADD	*AR3-,*AR4,A					;* AH=Q1+SLICE2
	 NEG	A								;* AH=-Q1-SLICE2
	 SUB	*AR3+,16,B,A					;* A=Q3-I1
	SQUR	A,B
	ADD		*AR3,*AR4,A						;* A=Q1+SLICE2
	ADD		IEQ,16,A						;* A=Q1-(-I3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 4 (180 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,B							;* BH=Q1
	NEG		B								;* BH=-Q1
	CALLD_	slice_9600_2345
	 ADD	*AR3+,*AR4,A					;* AH=I1+SLICE2
	 NEG	A								;* AH=-I1-SLICE2
	 ADD	*AR3-,16,B,A					;* A=Q3+Q1
	SQUR	A,B
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	ADD		IEQ,16,A						;* A=I1-(-I3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 5 (0 degrees) ****

	CALLD_	slice_9600_2345
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3-,B							;* BH=Q1
	 SUB	*AR3+,*AR4,A					;* AH=I1-SLICE2
	 SUB	*AR3-,16,B,A					;* A=Q3-Q1
	SQUR	A,B
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	SUB		IEQ,16,A						;* A=I1-(I3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 6 (-90 degrees) ****

	CALLD_	slice_9600_0167
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3-,A							;* AH=Q1
	 SUB	*AR4,*AR3+,B					;* AH=-I1+SLICE2
	 SUB	*AR3-,16,A						;* A=I3-Q1
	SQUR	A,B
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE2
	ADD		QEQ,16,A						;* A=I1-(-Q3+SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 7 (90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	CALLD_	slice_9600_0167
	 NEG	A								;* AH=-Q1
	 ADD	*AR4,*AR3+,B					;* AH=I1+SLICE2
	 ADD	*AR3-,16,A						;* A=I3+Q1
	SQUR	A,B
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE2
	SUB		QEQ,16,A						;* A=I1-(Q3-SLICE2)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3,A							;* dummy
	RETD_
	 MVKD	AR7,*AR6+						;* *PB++=region

;****************************************************************************
;* slice_9600_0167: Slicer for orientations 0, 1, 6, and 7
;* slice_9600_2345: Slicer for orientations 2, 3, 4, and 5
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=I2
;*	BH=Q2
;* Modifies:
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;* On return:
;*	AH=IEQ=I3
;*	BH=QEQ=Q3
;*	AR7=region
;****************************************************************************

slice_9600_0167:
	STH		A,I								;* I=I2
	STH		B,Q								;* Q=Q2
	SUB		#-SLICE4,16,A,B
	SUB		#SLICE4,16,A
	 ST		#SLICE8,IEQ
	 STM	#0,AR7
	XC		2,ALT
	 ST		#SLICE0,IEQ						;* if I2<SLICE4, IEQ=SLICE0
	XC		2,ALT
	 STM	#(2<<7),AR7						;* if I2<SLICE4, region=(2<<7)
	XC		2,BLT
	 ST		#-SLICE8,IEQ					;* if I2<-SLICE4, IEQ=-SLICE8
	XC		2,BLT
	 STM	#(3<<7),AR7						;* if I2<-SLICE4, region=(3<<7)

	LD		Q,16,B							;* B=Q2
	SUB		#SLICE4,16,B,A					;* A=Q2-SLICE4
	BCD_		slice_9600_0167_end,ALEQ		;* exit if Q2<=SLICE4
	 ST		#SLICE0,QEQ
	 LD		I,16,A							;* A=I2
	ABS		A
	SUB		B,A								;* A=abs(I2)-Q2
	BC_		slice_9600_0167_end,AGEQ		;* exit if abs(I2)>=Q2
	ST		#SLICE0,IEQ
	ST		#SLICE8,QEQ
	STM		#(1<<7),AR7
slice_9600_0167_end:
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3

slice_9600_2345:
	STH		A,I								;* I=I2
	STH		B,Q								;* Q=Q2
	LD		I,16,A							;* A=I2
	LD		Q,16,B							;* B=Q2
	STM		#0,AR7
	 ST		#SLICE4,IEQ
	 ST		#SLICE4,QEQ
	XC		2,ALT
	 ST		#-SLICE4,IEQ					;* if I2<SLICE0, IEQ=-SLICE4
	XC		2,ALT
	 STM	#(1<<7),AR7						;* if I2<SLICE0, region=(1<<7)
	XC		2,BLT
	 ST		#-SLICE4,QEQ					;* if Q2<SLICE0, QEQ=-SLICE4
	XC		2,BLT
	 MAR	*+AR7(1<<8)						;* if Q2<SLICE0, region|=(1<<8)
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3
 .endif

;****************************************************************************
;* path_metrics_12000: Computes path_metrics (minimum distances) for 12000
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR3=&I1[]. I1 and Q1 are in contiguous memory
;*	AR5=&path_metrics[0]
;*	AR6=&path_bits[0]
;*	AR1=&temp0
;* Modifies:
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;****************************************************************************

 .if (TCM_12000=ENABLED)
path_metrics_12000:
	ST		#SLICE1,*AR4+					;* temp0=SLICE1
	ST		#SLICE2,*AR4					;* temp1=SLICE2

	;**** group 0 (45 degrees) ****
	
	SUB		*AR3+,*AR4-,A					;* AH=I1-SLICE2
	SUB		*AR3-,16,A						;* AH=I1-Q1-SLICE2
	CALLD_	slice_12000
	 LD		*AR3+,16,B
	 ADD	*AR3-,16,B						;* BH=I1+Q1
	ADD		A,B								;* B=I3+Q3
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE1
	SUB		B,A								;* A=I1-(Q3+I3+SLICE1)
	SQUR	A,B					
	ADD		*AR3-,*AR4+,A					;* A=Q1+SLICE1
	SUB		QEQ,16,A						;* A=Q1-Q3+SLICE1
	ADD		IEQ,16,A						;* A=Q1-(Q3-I3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 1 (-135 degrees) ****

	ADD		*AR3+,*AR4-,B					;* BH=I1+SLICE2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	SUB		*AR3-,A							;* AH=Q1-I1-SLICE2
	LD		*AR3+,16,B
	CALLD_	slice_12000
	 ADD	*AR3-,16,B						;* BH=I1+Q1
	 NEG	B								;* BH=-I1-Q1
	ADD		A,B								;* B=I3+Q3
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE1
	ADD		B,A								;* A=I1-(-Q3-I3-SLICE1)
	SQUR	A,B					
	SUB		*AR3,*AR4+,A					;* A=Q1-SLICE1
	ADD		QEQ,16,A						;* A=Q1-(-Q3+SLICE1)
	SUB		IEQ,16,A						;* A=Q1-(-Q3+I3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 2 (-45 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	ADD		*AR3+,16,A						;* AH=I1+Q1
	CALLD_	slice_12000
	 ADD	*AR3-,*AR4-,B					;* BH=Q1+SLICE2
	 SUB	*AR3,16,B						;* BH=Q1-I1+SLICE2
	SUB		A,B								;* A=Q3-I3
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE1
	ADD		B,A								;* A=I1-(I3-Q3+SLICE1)
	SQUR	A,B					
	ADD		*AR3,*AR4+,A					;* A=Q1+SLICE1
	SUB		QEQ,16,A						;* A=Q1-(Q3-SLICE1)
	SUB		IEQ,16,A						;* A=Q1-(I3+Q3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 3 (135 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	ADD		*AR3,16,A						;* AH=I1+Q1
	NEG		A								;* AH=-Q1-I1
	CALLD_	slice_12000
	 ADD	*AR3+,*AR4-,B					;* BH=I1+SLICE2
	 SUB	*AR3-,16,B						;* BH=-Q1+I1+SLICE2
	SUB		A,B								;* A=Q3-I3
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE1
	SUB		B,A								;* A=I1-(-I3+Q3-SLICE1)
	SQUR	A,B					
	SUB		*AR3,*AR4+,A					;* A=Q1-SLICE1
	ADD		QEQ,16,A						;* A=Q1-(-Q3+SLICE1)
	ADD		IEQ,16,A						;* A=Q1-(-I3-Q3+SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 4 (-135 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	SUB		*AR3,16,A						;* AH=-I1+Q1
	SUB		*AR3+,*AR4-,B					;* BH=I1-SLICE2
	CALLD_	slice_12000
	 ADD	*AR3-,16,B						;* BH=Q1+I1-SLICE2
	 NEG	B								;* BH=-Q1-I1+SLICE2
	ADD		A,B								;* A=Q3+I3
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE1
	ADD		B,A								;* A=I1-(-I3-Q3+SLICE1)
	SQUR	A,B					
	SUB		*AR3,*AR4+,A					;* A=Q1-SLICE1
	ADD		QEQ,16,A						;* A=Q1-(-Q3+SLICE1)
	SUB		IEQ,16,A						;* A=Q1-(I3-Q3+SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 5 (45 degrees) ****

	ADD		*AR3-,*AR4-,B					;* BH=Q1+SLICE2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,A							;* AH=I1
	CALLD_	slice_12000
	 SUB	*AR3-,16,A						;* AH=I1-Q1
	 ADD	*AR3,16,B						;* BH=Q1+I1+SLICE2
	ADD		A,B								;* A=Q3+I3
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE1
	SUB		B,A								;* A=I1-(I3+Q3-SLICE1)
	SQUR	A,B					
	ADD		*AR3,*AR4+,A					;* A=Q1+SLICE1
	SUB		QEQ,16,A						;* A=Q1-(Q3-SLICE1)
	ADD		IEQ,16,A						;* A=Q1-(-I3+Q3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 6 (135 degrees) ****

	ADD		*AR3-,*AR4-,B					;* BH=Q1+SLICE2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	ADD		*AR3,A							;* AH=I1+Q1+SLICE2
	NEG		A								;* AH=-I1-Q1-SLICE2
	CALLD_	slice_12000
	 LD		*AR3+,16,B						;* BH=I1
	 SUB	*AR3-,16,B						;* BH=-Q1+I1
	SUB		A,B								;* A=Q3-I3
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE1
	SUB		B,A								;* A=I1-(-I3+Q3-SLICE1)
	SQUR	A,B					
	ADD		*AR3,*AR4+,A					;* A=Q1+SLICE1
	ADD		QEQ,16,A						;* A=Q1-(-Q3-SLICE1)
	ADD		IEQ,16,A						;* A=Q1-(-I3-Q3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 7 (-45 degrees) ****

	SUB		*AR3-,*AR4-,B					;* BH=Q1-SLICE2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	ADD		*AR3+,A							;* AH=I1+Q1-SLICE2
	CALLD_	slice_12000
	 LD		*AR3-,16,B						;* BH=Q1
	 SUB	*AR3,16,B						;* BH=Q1-I1
	SUB		A,B								;* A=Q3-I3
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE1
	ADD		B,A								;* A=I1-(I3-Q3+SLICE1)
	SQUR	A,B					
	SUB		*AR3,*AR4+,A					;* A=Q1-SLICE1
	SUB		QEQ,16,A						;* A=Q1-(Q3+SLICE1)
	SUB		IEQ,16,A						;* A=Q1-(-I3-Q3-SLICE1)
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3,A							;* dummy
	RETD_
	 MVKD	AR7,*AR6+						;* *PB++=region

;****************************************************************************
;* slice_12000: Slicer for 12000 bits/sec.
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=I2
;*	BH=Q2
;* Modifies:
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;* On return:
;*	AH=IEQ=I3
;*	BH=QEQ=Q3
;*	AR7=region
;****************************************************************************

slice_12000:
	STH		A,I								;* I=I2
	STH		B,Q								;* Q=Q2
	ABS		A								;* A=abs(I2)
	SUB		#SLICE8,16,A,B		
	BCD_		slice_12000_endif,BLEQ			;* branch if abs(I2)<=SLICE8
	 LD		Q,16,B
	 ABS	B								;* B=abs(Q2)
	SUB		B,A								;* A=abs(I2)-abs(Q2)
	SUB		#SLICE4,16,A
	BC_		slice_12000_endif,ALEQ			;* branch if abs(I2)-abs(Q2)<=SLICE4
	LD		I,16,A							;* A=I2
	 ST		#SLICE0,QEQ
	 ST		#SLICE6,IEQ
	STM		#(1<<7),AR7
	XC		2,ALT
	 ST		#-SLICE6,IEQ					;* if I2<-SLICE0, IEQ=-SLICE6
	XC		2,ALT
	 MAR	*+AR7(1<<6)
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3

slice_12000_endif:
	LD		Q,16,B							;* B=Q2
	SUB		#SLICE4,16,B,A					;* A=Q2-SLICE4
	SUB		#-SLICE4,16,B					;* B=Q2-SLICE4
	 ST		#SLICE4,QEQ						;* Q3=SLICE4
	 STM	#0,AR7							;* region=0
	XC		2,ALT
	 ST		#SLICE0,QEQ						;* if Q2< SLICE4, Q3=SLICE0
	XC		2,ALT
	 STM	#(2<<7),AR7						;* if Q2< SLICE4, region=(2<<7)
	LD		I,16,A
	XC		2,BLT
	 ST		#-SLICE4,QEQ					;* if Q2< -SLICE4, Q3=-SLICE4
	XC		2,BLT
	 STM	#(3<<7),AR7						;* if Q2< -SLICE4, region=(3<<7)

	ST		#SLICE2,IEQ						;* I3=SLICE2
	XC		2,ALT
	 ST		#-SLICE2,IEQ					;* if I2< SLICE0, I3=-SLICE2
	XC		2,ALT
	 MAR	*+AR7(1<<6)						;* if I2<SLICE0, region|=(1<<6)	 
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3
 .endif
	
;****************************************************************************
;* path_metrics_14400: Computes path_metrics (minimum distances) for 14400
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR3=&I1[]. I1 and Q1 are in contiguous memory
;*	AR5=&path_metrics[0]
;*	AR6=&path_bits[0]
;*	AR4=&temp0
;* Modifies:
;* 	AR3,AR7,A,B,
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;****************************************************************************

 .if (TCM_14400=ENABLED)
path_metrics_14400:
	ST		#SLICE1,*AR4					;* temp0=SLICE1

	;**** group 0 (180 degrees) ****

	LD		*AR3+,16,A						;* AH=I1
	NEG		A								;* A=-I1
	CALLD_	slice_14400
	 ADD	*AR3-,*AR4,B					;* BH=Q1+SLICE1
	 NEG	B					
	ADD		*AR3+,16,A						;* A= I4=I1+I3
	SQUR	A,B
	ADD		*AR3-,*AR4,A					;* A=Q1+SLICE1
	ADD		QEQ,16,A						;* A= Q4=Q1+Q3+SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 1 (0 degrees) ****

	CALLD_	slice_14400
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3+,A							;* AH=I1
	 SUB	*AR3-,*AR4,B					;* BH=Q1-SLICE1
	SUB		*AR3+,16,A						;* A= I4=I1-I3
	SQUR	A,B
	SUB		*AR3-,*AR4,A					;* A=Q1-SLICE1
	SUB		QEQ,16,A						;* A= -Q4=Q1-Q3-SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 2 (90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,B							;* BH=I1
	CALLD_	slice_14400
	 ADD	*AR4,*AR3-,A					;* AH=Q1+SLICE1
	 NEG	A								;* AH=-(Q1+SLICE1)
	SUB		*AR3+,16,B,A					;* A= I4=I1-Q3
	SQUR	A,B
	ADD		*AR3-,*AR4,A					;* A=Q1+SLICE1
	ADD		IEQ,16,A						;* A= Q4=Q1+I3+SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 3 (-90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3+,B							;* BH=I1
	CALLD_	slice_14400
	 NEG	B								;* BH= Q2=-I1
	 SUB	*AR3-,*AR4,A					;* AH= I2=Q1-SLICE1
	ADD		*AR3+,16,B,A					;* A= I4=I1+Q3
	SQUR	A,B
	SUB		*AR3,*AR4,A						;* A=Q1-SLICE1
	SUB		IEQ,16,A						;* A= Q4=Q1-I3-SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 4 (0 degrees) ****

	CALLD_	slice_14400
	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3-,B							;* BH=Q1
	 SUB	*AR3+,*AR4,A					;* AH=I1-SLICE1
	SUB		*AR3-,16,B,A					;* A=Q3-Q1
	SQUR	A,B
	SUB		*AR3+,*AR4,A					;* A=I1-SLICE1
	SUB		IEQ,16,A						;* A=I1-I3-SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 5 (180 degrees) ****
	
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,B							;* BH=Q1
	NEG		B
	CALLD_	slice_14400
	 ADD	*AR3,*AR4,A						;* AH=I1-SLICE1
	 NEG	A
	ADD		*AR3+,*AR4,B					;* B=I1+SLICE1
	ADD		B,A								;* A=I3+I1+SLICE1
	SQUR	A,B
	LD		*AR3,16,A						;* A=Q1
	ADD		QEQ,16,A						;* A= Q1+Q3
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 6 (-90 degrees) ****

	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3-,A							;* AH=Q1
	CALLD_	slice_14400
	 ADD	*AR3+,*AR4,B					;* BH=I1+SLICE1
	 NEG	B
	SUB		*AR3-,16,A						;* A=I3-Q1
	SQUR	A,B
	ADD		*AR3+,*AR4,A					;* A=I1+SLICE1
	ADD		QEQ,16,A						;* A=I1+Q3+SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	MVKD	AR7,*AR6+						;* *PB++=region

	;**** group 7 (90 degrees) ****

	 ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	 LD		*AR3-,A							;* AH=Q1
	CALLD_	slice_14400
	 NEG	A
	 SUB	*AR3+,*AR4,B					;* BH= I1-SLICE1
	ADD		*AR3-,16,A						;* A=I3+Q1
	SQUR	A,B
	SUB		*AR3,*AR4,A						;* A=I1-SLICE1
	SUB		QEQ,16,A						;* A=I1-Q3-SLICE1
	SQUR	A,A
	ADD		B,A								;* A=I4^2+Q4^2
	ST		A,*AR5+							;* *PM++=(I4^2+Q4^2)>>PM_SHIFT
||	LD		*AR3,A							;* dummy
	RETD_
	 MVKD	AR7,*AR6+						;* *PB++=region

;****************************************************************************
;* slice_14400: Slicer for 14400 bits/sec.
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=I2
;*	BH=Q2
;* Modifies:
;*	A,B,AR0,AR7
;*	 Rx->I,Rx->Q,Rx->IEQ,Rx->QEQ
;* On return:
;*	AH=IEQ=I3
;*	BH=QEQ=Q3
;*	AR7=region
;****************************************************************************

slice_14400:
	STH		A,I								;* I=I2
	STH		B,Q								;* Q=Q2
	ABS		A								;* A=abs(I2)
	SUB		#SLICE6,16,A,B			
	BCD_	slice_14400_endif,BLEQ			;* branch if abs(I2)<=SLICE6
	 LD		Q,16,B
	 ABS	B								;* B=abs(Q2)
	SUB		B,A								;* A=abs(I2)-abs(Q2)
	SUB		#SLICE2,16,A
	BCD_	slice_14400_endif,ALEQ			;* branch if abs(I2)-abs(Q2)<=SLICE2
	 LD		I,16,A							;* A=I2
	 LD		Q,16,B							;* B=Q2
	ST		#SLICE8,IEQ
	ST		#SLICE2,QEQ
	XC		2,ALT
	 ST		#-SLICE8,IEQ					;* if I2<-SLICE0, IEQ=-SLICE8
	XC		2,BLT
	 ST		#-SLICE2,QEQ					;* if Q2<-SLICE2, QEQ=-SLICE2
	STM		#0,AR7
	XC		2,ALT
	 MAR	*+AR7(1<<5)
	XC		2,BLT
	 MAR	*+AR7(8<<5)						;* if Q2<-SLICE2, AR7=(8<<5)
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3
slice_14400_endif:

	LD		Q,16,B							;* B=Q2
	SUB		#SLICE4,16,B,A					;* A=Q2-SLICE4
	 ST		#SLICE6,QEQ						;* Q3=SLICE6
	 STM	#(1<<5),AR7						;* region=(1<<5)
	XC		2,ALT
	 ST		#SLICE2,QEQ						;* if Q2< SLICE4, Q3=SLICE2
	XC		2,ALT
	 STM	#(0<<5),AR7						;* region=(1<<5)
	SUB		#-SLICE4,16,B,A					;* A=Q2-SLICE4
	XC		2,BLT
	 ST		#-SLICE2,QEQ					;* if Q2< SLICE0, Q3=-SLICE2
	XC		2,BLT
	 STM	#(8<<5),AR7						;* if Q2< SLICE0, region=(8<<5)
	XC		2,ALT
	 ST		#-SLICE6,QEQ					;* if Q2<-SLICE4, Q3=-SLICE6
	XC		2,ALT
	 STM	#(9<<5),AR7						;* if Q2<-SLICE4, region=(9<<5)

	LD		I,16,B
	SUB		#SLICE2,16,B,A
	SUB		#-SLICE2,16,B
	 ST		#SLICE4,IEQ						;* I3=SLICE4
	 STM	#(4<<5),AR0						;* R=(4<<5)
	XC		2,ALT
	 ST		#SLICE0,IEQ						;* if I2< SLICE2, I3=SLICE0
	XC		2,ALT
	 STM	#(6<<5),AR0						;* if I2< SLICE2, R=(6<<5)
	XC		2,BLT
	 ST		#-SLICE4,IEQ					;* if I2<-SLICE2, I3=-SLICE4
	XC		2,BLT
	 STM	#(2<<5),AR0						;* if I2<-SLICE2, R=(2<<5)
	MAR		*AR7+0							;* AR7=region|R
	RETD_
	 LD		IEQ,16,A						;* A=I3
	 LD		QEQ,16,B						;* B=Q3
 .endif
;****************************************************************************
 .endif

;****************************************************************************
	.end
