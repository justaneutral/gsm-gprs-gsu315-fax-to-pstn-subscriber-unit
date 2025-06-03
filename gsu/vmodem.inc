;****************************************************************************
;* Filename: vmodem.inc
;* Date: 01-29-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros for the 
;*		TMS320C54x implementation of the CCITT V series modems.
;****************************************************************************

	.mmregs

DISABLED					.set	0
ENABLED				 		.set	1
NOT_DETECTED				.set	0
DETECTED					.set	1

;****************************************************************************
;* FAR-mode compilation defines and macros.
;* Building objects using FAR_MODE or FARC_MODE options will allow the 
;* "vtext" code section to be placed in extended program memory. It should be 
;* noted that FAR_MODE and FARC_MODE requires ON_CHIP_COEFFICIENTS=DISABLED,
;* and the "vcoefs" section MUST be located in on-chip, overlaid
;* memory within page 0.
;****************************************************************************

	.asg  DISABLED,far
	.asg  DISABLED,farc
 .if $isdefed("FAR_MODE")
	.c_mode
	.asg  ENABLED,far
	.asg  ENABLED,farc
 .endif
 .if $isdefed("FARC_MODE")
	.c_mode
	.asg  ENABLED,farc
 .endif
FAR							.set	far
FARC						.set	farc

;****************************************************************************

   ;**** C54x bit definitions ****
   
OVLY						.set	20h

	;**** Vector and buffer lengths ****

SIN_TABLE_LEN				.set	256
SIN_90_DEGREES		  		.set	64
SIN_TABLE_SHIFT		 		.set	8
DFT_COEF					.set	572	;* 32768*(1/80) + fudge
DFT_COEF_LEN				.set	256	;* Rx DFT buffer len
DELAY_STATES				.set	8
TRACE_BACK_LEN		  		.set	16
DEMOD_DELAY_STRIDE	  		.set	4
									   
	;**** state_ID definitions ****	
									   
TX_SILENCE_ID				.set	0100h
RX_IDLE_ID			  		.set	0100h
MESSAGE_ID_FIELD			.set	007fh
MESSAGE_ID					.set	0040h

	;**** status messages ****

STATUS_OK					.set	00h
DETECT_FAILURE		  		.set	10h
SYNC_FAILURE				.set	20h
CHECKSUM_FAILURE 			.set	21h
CRC_FAILURE 				.set	22h
CID_DETECTED 				.set	23h
TRAIN_LOOPS_FAILURE	 		.set	30h
START_EQ_FAILURE			.set	30h
TRAIN_EQ_FAILURE			.set	31h
SCR1_FAILURE				.set	40h
LOSS_OF_LOCK				.set	50h
GAIN_HIT_STATUS		 		.set	51h
EXCESSIVE_MSE_STATUS		.set	52h
EXCESSIVE_RTD_STATUS		.set	53h
RETRAIN				 		.set	60h
RETRAIN_FAILURE		 		.set	61h
RENEGOTIATE			 		.set	62h
RENEGOTIATE_FAILURE	 		.set	63h
V22_USB1_DETECTED			.set	70h
V22_S1_DETECTED		 		.set	71h
V22_SB1_DETECTED			.set	72h
V32_ANS_DETECTED			.set	80h
V32_AA_DETECTED		 		.set	81h
V32_AC_DETECTED		 		.set	82h
GSTN_CLEARDOWN_REQUESTED	.set	90h
				  
	;**** Tx mode bit field definitions ****

TX_LONG_RESYNC_FIELD		.set	0001h
TX_TEP_FIELD				.set	0002h
TX_SCRAMBLER_DISABLE_BIT	.set    0040h

	;**** Rx mode bit field definitions ****

RX_LONG_RESYNC_FIELD		.set	0001h
RX_DETECTOR_DISABLE	 		.set	0002h
RX_LOS_FIELD				.set	0004h
RX_V26_MODE_FIELD			.set	0018h
RX_STU_III_BIT				.set	0008h
RX_EC_COEF_SAVE_BIT			.set	0010h
RX_EQ_COEF_SAVE_BIT			.set	0020h
RX_DESCRAMBLER_DISABLE_BIT	.set    0040h

	;**** macros for structure member definitions ****

INT		.macro	mem  
		.asg	x,:mem:
		.eval	x+1,x   
		.endm	
   
FAR_INT	.macro	mem
		.if 	(x&1)!=0
		.eval	x+1,x   
		.endif		
	INT	:mem:  
	INT	:mem:_hi  
		.endm	

	;**** struct TX_BLOCK members ****

	.asg  0,x
	INT	Tx_start_ptrs					
 .if FARC=ENABLED
	FAR_INT	Tx_state						 
 .else
	INT	Tx_state						 
 .endif
	INT	Tx_state_ID					  
	INT	Tx_rate						  
	INT	Tx_scale						 
	INT	Tx_system_delay				  
	INT	Tx_sample_head				   
	INT	Tx_sample_tail				   
	INT	Tx_sample_len					
	INT	Tx_data_head					 
	INT	Tx_data_tail					 
	INT	Tx_data_len					  
	INT	Tx_sample_counter				
	INT	Tx_symbol_counter				
	INT	Tx_call_counter				  
	INT	Tx_num_samples				   
	INT	Tx_mode						  
	INT	Tx_terminal_count				
	INT	Tx_Nbits						 
	INT	Tx_Nmask						 
	INT	Tx_bit_register
	INT	Tx_bit_register_low
	INT	Tx_bit_index
TX_CONTROL_LEN		  		.set	x
TX_SPARE_LEN				.set	4

	;**** struct RX_BLOCK members ****

	.asg  0,x
	INT	Rx_start_ptrs	  
 .if FARC=ENABLED
	FAR_INT	Rx_state		   
 .else
	INT	Rx_state		   
 .endif
	INT	Rx_state_ID		
	INT	Rx_status			
	INT	Rx_rate			
	INT	Rx_power		   
	INT	Rx_sample_head	 
	INT	Rx_sample_tail	 
	INT	Rx_sample_stop	 
	INT	Rx_sample_len	  
	INT	Rx_data_head	   
	INT	Rx_data_tail	   
	INT	Rx_data_len		
	INT	Rx_sample_counter  
	INT	Rx_symbol_counter  
	INT	Rx_call_counter	
	INT	Rx_num_samples	 
	INT	Rx_mode			
	INT	Rx_threshold	   
	INT	Rx_detector_mask   
	INT	Rx_digit_CP_mask   
	INT	temp0			  
	INT	temp1			  
	INT	Rx_Nbits		 
	INT	Rx_Nmask		 
	INT	Rx_bit_register
	INT	Rx_bit_register_low
	INT	Rx_bit_index
RX_CONTROL_LEN		  		.set	x
RX_SPARE_LEN				.set	10

	;**** defaults ****

NUM_SAMPLES			 		.set	20
TX_NUM_SAMPLES		  		.set	NUM_SAMPLES
RX_NUM_SAMPLES		  		.set	NUM_SAMPLES
TX_MINUS_16DBM0		 		.set	32767
				
	;**** struct START_PTRS members ****

Tx_block_start		  		.set	0
Tx_sample_start		 		.set	1		
Tx_data_start				.set	2		
Tx_fir_start				.set	3			
Rx_block_start		  		.set	4		
Rx_sample_start		 		.set	5		
Rx_data_start				.set	6		
Rx_fir_start				.set	7		
EQ_coef_start				.set	8	
EC_coef_start				.set	9	
encoder_start				.set	10
decoder_start				.set	11
trace_back_start			.set	12

;++++#ifdef MESI_INTERNAL 02-23-2001
;;****************************************************************************
;;* MACRO: VECDEF
;;* Defines a vector and initializes it's start pointer.
;;****************************************************************************
;
;VECDEF	  .macro	vec,section,len
;:vec:	   .usect	:section:,:len:
;			.global	:vec:
;			.endm
;
;;****************************************************************************
;;* MACRO: CIRCDEF
;;* Defines a circular buffer vector and initializes it's start pointer.
;;****************************************************************************
;
;CIRCDEF	 .macro	vec,section,len
;:vec:	   .usect	:section:,:len:
;			.global	:vec:
;			.endm
;++++#else  MESI_INTERNAL 02-23-2001
;****************************************************************************
;* MACRO: VECDEF
;* Defines a global vector if the length is non-zero
;* Arguments:
;*	vec - name of the memory vector or array
;*	section - name of the data memory section to allocate vec in
;*	len - length of vec
;****************************************************************************

VECDEF	  .macro	vec,section,len
 .if len != 0
:vec:	   .usect	:section:,:len:
			.global	:vec:
 .endif
			.endm

;****************************************************************************
;* MACRO: CIRCDEF
;* Defines a circular circular buffer if the length is non-zero
;* Arguments:
;*	vec - name of the memory vector or array
;*	section - name of the data memory section to allocate vec in
;*	len - length of vec
;****************************************************************************

CIRCDEF	 .macro	vec,section,len
 .if len != 0
:vec:	   .usect	:section:,:len:
			.global	:vec:
 .endif
			.endm
			
;****************************************************************************
;* MACRO: BLDCHAN
;* Creates the memory vectors and buffers needed for a single channel. Also 
;* conditionally creates a START_PTRS table and initializes it.	Each channel
;* created by this macro is nmed uniquely by the specified channel suffix.
;* Arguments:
;*	CHANNEL_SPEC - channel specifier name suffix 
;****************************************************************************

BLDCHAN	 	.macro	CHANNEL_SPEC
 .if $isdefed("XDAIS_API")
	CIRCDEF	_MEMORY_MESI_TxBlock:CHANNEL_SPEC:,"TxBlk:CHANNEL_SPEC:",TX_BLOCK_LEN
	CIRCDEF	_MEMORY_MESI_TxSample:CHANNEL_SPEC:,"TxSmpl:CHANNEL_SPEC:",TX_SAMPLE_LEN
	CIRCDEF	_MEMORY_MESI_TxData:CHANNEL_SPEC:,"TxData:CHANNEL_SPEC:",TX_DATA_LEN
	CIRCDEF	_MEMORY_MESI_TxFir:CHANNEL_SPEC:,"TxFir:CHANNEL_SPEC:",TX_FIR_LEN
	CIRCDEF	_MEMORY_MESI_RxBlock:CHANNEL_SPEC:,"RxBlk:CHANNEL_SPEC:",RX_BLOCK_LEN
	CIRCDEF	_MEMORY_MESI_RxSample:CHANNEL_SPEC:,"RxSmpl:CHANNEL_SPEC:",RX_SAMPLE_LEN
	CIRCDEF	_MEMORY_MESI_RxData:CHANNEL_SPEC:,"RxData:CHANNEL_SPEC:",RX_DATA_LEN
	CIRCDEF	_MEMORY_MESI_RxFir:CHANNEL_SPEC:,"RxFir:CHANNEL_SPEC:",RX_FIR_LEN
	VECDEF	_MEMORY_MESI_EQcoef:CHANNEL_SPEC:,"EQcoef:CHANNEL_SPEC:",EQ_COEF_LEN
	VECDEF	_MEMORY_MESI_ECcoef:CHANNEL_SPEC:,"ECcoef:CHANNEL_SPEC:",EC_COEF_LEN
	VECDEF	_MEMORY_MESI_encoder:CHANNEL_SPEC:,"Encode:CHANNEL_SPEC:",ENCODER_BLOCK_LEN
	VECDEF	_MEMORY_MESI_decoder:CHANNEL_SPEC:,"Decode:CHANNEL_SPEC:",DECODER_BLOCK_LEN
	CIRCDEF	_MEMORY_MESI_traceBack:CHANNEL_SPEC:,"Trace:CHANNEL_SPEC:",TRACE_BACK_BUF_LEN
 .else		;* XDAIS_API
	CIRCDEF	_Tx_block:CHANNEL_SPEC:,"TxBlk:CHANNEL_SPEC:",TX_BLOCK_LEN
	CIRCDEF	_Tx_sample:CHANNEL_SPEC:,"TxSmpl:CHANNEL_SPEC:",TX_SAMPLE_LEN
	CIRCDEF	_Tx_data:CHANNEL_SPEC:,"TxData:CHANNEL_SPEC:",TX_DATA_LEN
	CIRCDEF	_Tx_fir:CHANNEL_SPEC:,"TxFir:CHANNEL_SPEC:",TX_FIR_LEN
	CIRCDEF	_Rx_block:CHANNEL_SPEC:,"RxBlk:CHANNEL_SPEC:",RX_BLOCK_LEN
	CIRCDEF	_Rx_sample:CHANNEL_SPEC:,"RxSmpl:CHANNEL_SPEC:",RX_SAMPLE_LEN
	CIRCDEF	_Rx_data:CHANNEL_SPEC:,"RxData:CHANNEL_SPEC:",RX_DATA_LEN
	CIRCDEF	_Rx_fir:CHANNEL_SPEC:,"RxFir:CHANNEL_SPEC:",RX_FIR_LEN
	VECDEF	_EQ_coef:CHANNEL_SPEC:,"EQcoef:CHANNEL_SPEC:",EQ_COEF_LEN
	VECDEF	_EC_coef:CHANNEL_SPEC:,"ECcoef:CHANNEL_SPEC:",EC_COEF_LEN
	VECDEF	_encoder:CHANNEL_SPEC:,"Encode:CHANNEL_SPEC:",ENCODER_BLOCK_LEN
	VECDEF	_decoder:CHANNEL_SPEC:,"Decode:CHANNEL_SPEC:",DECODER_BLOCK_LEN
	CIRCDEF	_trace_back:CHANNEL_SPEC:,"Trace:CHANNEL_SPEC:",TRACE_BACK_BUF_LEN
 .endif		;* XDAIS_API

	;**** create start_ptrs_table[] and initialize ****
	 
 .if !$isdefed("START_PTRS_LEN")
	.asg	1,START_PTRS_LEN
 .endif
 
 .if START_PTRS_LEN != 0
	.sect	"vcoefs"
	
_start_ptrs_table:CHANNEL_SPEC::	
_MEMORY_MESI_ptrsTable:CHANNEL_SPEC::	
 .if $isdefed("XDAIS_API")
	 .global	_MEMORY_MESI_ptrsTable:CHANNEL_SPEC:
 .else		;* XDAIS_API
	 .global	_start_ptrs_table:CHANNEL_SPEC:
 .endif		;* XDAIS_API
 .if TX_BLOCK_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_TxBlock:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Tx_block:CHANNEL_SPEC:
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if TX_SAMPLE_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_TxSample:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Tx_sample:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if TX_DATA_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_TxData:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Tx_data:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if TX_FIR_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_TxFir:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Tx_fir:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if RX_BLOCK_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_RxBlock:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Rx_block:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if RX_SAMPLE_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_RxSample:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Rx_sample:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if RX_DATA_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_RxData:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Rx_data:CHANNEL_SPEC:				
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if RX_FIR_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_RxFir:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_Rx_fir:CHANNEL_SPEC:			
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if EQ_COEF_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_EQcoef:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_EQ_coef:CHANNEL_SPEC:			
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
 .if EC_COEF_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_ECcoef:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_EC_coef:CHANNEL_SPEC:			
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif	
 .if ENCODER_BLOCK_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_encoder:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_encoder:CHANNEL_SPEC:
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif	
 .if DECODER_BLOCK_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_decoder:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_decoder:CHANNEL_SPEC:	 
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif	
 .if TRACE_BACK_BUF_LEN!=0
 .if $isdefed("XDAIS_API")
	.word	_MEMORY_MESI_traceBack:CHANNEL_SPEC:
 .else		;* XDAIS_API
	.word	_trace_back:CHANNEL_SPEC:	   
 .endif		;* XDAIS_API
 .else
	.word	0
 .endif
_start_ptrs_table_end:CHANNEL_SPEC::	

	.asg	_start_ptrs_table_end:CHANNEL_SPEC:-_start_ptrs_table:CHANNEL_SPEC:,START_PTRS_LEN
 .if $isdefed("XDAIS_API")
	VECDEF	_MEMORY_MESI_startPtrs:CHANNEL_SPEC:,"Ptrs:CHANNEL_SPEC:",START_PTRS_LEN
 .else		;* XDAIS_API
	VECDEF	_start_ptrs:CHANNEL_SPEC:,"Ptrs:CHANNEL_SPEC:",START_PTRS_LEN
 .endif		;* XDAIS_API
 .endif		;* START_PTRS_LEN != 0
			.endm
;++++#endif MESI_INTERNAL 02-23-2001

;****************************************************************************
;* MACRO: RATIO	Smem1,Smem2,THRESHOLD
;* pass X,Y,THR. Returns BH=smaller-bigger*THR
;****************************************************************************

RATIO	   .macro	X,Y,THR
	LD	  	:X:,16,A
	LD	  	:Y:,16,B
	MAX	 	A						   	;* A=max(X,Y)
	 STM	#:THR:,T
	 MPYA   A						   	;* A=max*THR
	XC	  	1,C							;* if Y was the max ...
	 LD	 	:X:,16,B					;* ... then load X into B
; STM 	#0,AL							;* makes it agree with CSRC
	SUB	 	A,B						 	;* B=min-max*THR	
			.endm					   

;****************************************************************************
;* BPF: bandpass filter call macro
;* On entry, it expects:
;*	ALU status reflect non zero result
;* Parameters:
;*	AR=Rx_sample_tail
;*	N is the filter length
;*	Fc is the band center frequency
;* On return:
;*	AH=output magnitude
;****************************************************************************

BPF	.macro	Fc,N,AR
	STM	 	#:N:-2,BRC				  	;* RC=N-2
	MVMM	:AR:,AR3					;* AR3=Rx_sample_tail
	CCD_	bandpass_filter,TC
	  STM   #:Fc:,AR0				   	;* AR0=COEF
	.endm								
							 
;****************************************************************************
;* BB_EST: broadband estimator call macro
;* Parameters:
;*	AR=Rx_sample_tail
;*	N=estimator length
;*	K=broadband coefficient => (1/estimator length)
;* On return:
;*	AH=output magnitude
;****************************************************************************

BB_EST		.macro	K,N,AR
	MVMM	:AR:,AR3					;* AR7=Rx_sample_tail
	STM	 	#:N:-2,BRC				  	;* RC=N
	CALLD_	broadband_estimator
	 STM	#:K:,T					  	;* T=broadband_coef
	.endm								

;****************************************************************************
;* macros to implement FAR branch, call, and ret conditioned on the FAR_MODE
;* assembler command line option.	
;*
;* B_: branch to address specified by addr
;* BACC_: branch to address in specified accum
;* BC_: branch to address specified by addr on condition specified by cond
;* CALA_: call to address in specified accum
;* CALL_: call to address specified by addr
;* CC_: call to address specified by addr on condition specified by cond 
;* RC_: return from subroutine on condition specified by cond 
;* RET_: return from subroutine 
;* RETE_: return from subroutine and enable interrupts
;*
;* RETC: conditionally inserts FRET in assembly returns to C if FARC is 
;*	   defined. This allows the user to make far calls from C and have 
;*	   the C/assembly interface routines provide the matching FRET.
;* GET_ARG: fetches the specified argument from the stack and puts it in
;*		  memory specified by dmad.
;* STPP: writes the address specified by addr to the Program Space pointer at
;*		   the location spefified by ptr. The user must be careful to 
;*		   note that the FAR version of this macro modifies the accumulator 
;*		   specified by accum. LDX is LDU with a shift value allowed.
;* LDPP: loads the specified accumulator with the address specified by the 
;*		   Program Space pointer at the location specified by ptr. 
;****************************************************************************

B_		.macro	addr
 .if FAR=ENABLED
	FB		:addr:
 .else
	B		:addr:
 .endif
		.endm

BD_		.macro	addr
 .if FAR=ENABLED
	FBD		:addr:
 .else
	BD		:addr:
 .endif
		.endm

BACC_	.macro	accum
 .if FAR=ENABLED
	FBACC	:accum:
 .else
	BACC	:accum:
 .endif
		.endm

BACCD_	.macro	accum
 .if FAR=ENABLED
	FBACCD	:accum:
 .else
	BACCD	:accum:
 .endif
		.endm

BC_		.macro	addr,cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 2,:cond:
	 FB		:addr:
 .else
	BC		:addr:,:cond:
 .endif
		.endm

BCD_	.macro	addr,cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 2,:cond:
	 FBD	:addr:
 .else
	BCD		:addr:,:cond:
 .endif
		.endm

CALA_	.macro	accum
 .if FAR=ENABLED
	FCALA	:accum:
 .else
	CALA	:accum:
 .endif
		.endm

CALAD_	.macro	accum
 .if FAR=ENABLED
	FCALAD	:accum:
 .else
	CALAD	:accum:
 .endif
		.endm

CALL_	.macro	addr
 .if FAR=ENABLED
	FCALL	:addr:
 .else
	CALL	:addr:
 .endif
		.endm

CALLD_	.macro	addr
 .if FAR=ENABLED
	FCALLD	:addr:
 .else
	CALLD	:addr:
 .endif
		.endm

CC_		.macro	addr,cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 2,:cond:
	 FCALL		:addr:
 .else
	CC		:addr:,:cond:
 .endif
		.endm

CCD_	.macro	addr,cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 2,:cond:
	 FCALLD	:addr:
 .else
	CCD		:addr:,:cond:
 .endif
		.endm

RC_		.macro	cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 1,:cond:
	 FRET
 .else
	RC		:cond:
 .endif
		.endm

RCD_	.macro	cond
 .if FAR=ENABLED
	 NOP
	 NOP
	XC 1,:cond:
	 FRETD
 .else
	RCD		:cond:
 .endif
		.endm

RET_	.macro	accum
 .if FAR=ENABLED
	FRET	:accum:
 .else
	RET	:accum:
 .endif
		.endm

RETD_	.macro	accum
 .if FAR=ENABLED
	FRETD	:accum:
 .else
	RETD	:accum:
 .endif
		.endm

RETE_	.macro	accum
 .if FAR=ENABLED
	FRETE	:accum:
 .else
	RETE	:accum:
 .endif
		.endm

RETED_	.macro	accum
 .if FAR=ENABLED
	FRETED	:accum:
 .else
	RETED	:accum:
 .endif
		.endm

RETC_	.macro
 .if FARC=ENABLED
	FRET
 .else
	RET_
 .endif 
		.endm

GET_ARG .macro	argno,dmad
 .if FARC=ENABLED
	MVDK	*SP(:argno:+1),:dmad:		;* get arg with offset for far ret
 .else
	MVDK	*SP(:argno:),:dmad:			;* get arg from stack			
 .endif 
		.endm

STPP	.macro	addr,ptr,accum
 .if FAR=ENABLED
	LDX		:addr:,16,:accum:
	OR		:addr:,:accum:
	DST		:accum:,:ptr:
 .else
	ST	:addr:,:ptr:				
 .endif
		.endm

LDPP	.macro	ptr,accum
 .if FAR=ENABLED
	DLD		:ptr:,:accum:
 .else
	LD		:ptr:,:accum:
 .endif
		.endm

;****************************************************************************
