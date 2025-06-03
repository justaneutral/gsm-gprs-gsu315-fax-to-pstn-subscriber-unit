;****************************************************************************
;* Filename: v21.asm
;* Date: 03-27-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for v21
;****************************************************************************

	.include	"vmodem.inc"
;++++#ifndef MESI_INTERNAL 03-22-2001
	.include	"common.inc"
;++++#endif  MESI_INTERNAL 03-22-2001
	.include	"config.inc"
	.include	"fsk.inc"
	.include	"v21.inc"

	;**** modulator ****

TX_CH1_CARRIER		 		.set	8847
TX_CH2_CARRIER		 		.set	14336
TX_V21_FREQ_SHIFT   		.set    819		;* (100 Hz)*8.192
TX_V21_INTERP				.set    160		;* Finterp=300*160=48kHz
TX_V21_DEC		    		.set    6		;* Fdec=48000/6=8kHz

 .if $isdefed("XDAIS_API")
	.global _V21_MESI_TxInitV21Ch1
	.global V21_MESI_TxInitV21Ch1
	.global _V21_MESI_TxInitV21Ch2
	.global V21_MESI_TxInitV21Ch2
	.global V21_MESI_TxInitV21
 .else
	.global _Tx_init_v21_ch1
	.global Tx_init_v21_ch1
	.global _Tx_init_v21_ch2
	.global Tx_init_v21_ch2
	.global Tx_init_v21
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global FSK_MESI_TxInitFSK
	.asg	FSK_MESI_TxInitFSK, Tx_init_FSK
	.global FSK_MESI_FSKmodulator
	.asg	FSK_MESI_FSKmodulator, FSK_modulator
	.global RXTX_MESI_TxInitSilence
	.asg	RXTX_MESI_TxInitSilence, Tx_init_silence
	.global RXTX_MESI_TxStateReturn
	.asg	RXTX_MESI_TxStateReturn, Tx_state_return
 .else
	.global Tx_init_FSK
	.global FSK_modulator
	.global Tx_init_silence
	.global Tx_state_return
 .endif										;* "XDAIS_API endif

	;**** demodulator ****

FILTER_980_COEF				.set	31		;* 0.032*968.8 Hz
FILTER_1180_COEF			.set    38		;* 0.032*1187.5 Hz
FILTER_1650_COEF			.set    53		;* 0.032*1656.3 Hz
FILTER_1850_COEF			.set    59		;* 0.032*1843.8 Hz

RX_V21_COEF_LEN 			.set    40
RX_V21_INTERP				.set    80  	;* Finterp=2*300*80=48kHz
RX_V21_DEC			    	.set    6   	;* Fdec=48000/6=8kHz

 .if $isdefed("XDAIS_API")
	.global _V21_MESI_RxInitV21Ch1
	.global V21_MESI_RxInitV21Ch1
	.global _V21_MESI_RxInitV21Ch2
	.global V21_MESI_RxInitV21Ch2
	.global V21_MESI_RxInitV21
 .else
	.global _Rx_init_v21_ch1
	.global Rx_init_v21_ch1
	.global _Rx_init_v21_ch2
	.global Rx_init_v21_ch2
	.global Rx_init_v21
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global FSK_MESI_RxInitFSK
	.asg	FSK_MESI_RxInitFSK, Rx_init_FSK
	.global FSK_MESI_FSKdemodulator
	.asg	FSK_MESI_FSKdemodulator, FSK_demodulator
	.global DET_MESI_RxInitDetector
	.asg	DET_MESI_RxInitDetector, Rx_init_detector
	.global RXTX_MESI_RxStateReturn
	.asg	RXTX_MESI_RxStateReturn, Rx_state_return
 .else
	.global Rx_init_FSK
	.global FSK_demodulator
	.global Rx_state_return
	.global Rx_init_detector
 .endif										;* "XDAIS_API endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

 .if IQ_DAC_WRITE=ENABLED
	.global IQ_DAC_write
 .endif

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_v21_ch1(struct START_PTRS *)
;* void Tx_init_v21_ch2(struct START_PTRS *)
;* void Rx_init_v21_ch1(struct START_PTRS *)
;* void Rx_init_v21_ch2(struct START_PTRS *)
;* 
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

;****************************************************************************
;* _Tx_init_v21_ch1:
;* C function call: void Tx_init_v21_ch1(struct START_PTRS *)
;* Initializes Tx_block for V21 channel 1 modulator.
;****************************************************************************

 .if (COMPILER=ENABLED)&(TX_V21_MODEM_CH1=ENABLED)
_Tx_init_v21_ch1:				
_V21_MESI_TxInitV21Ch1:
	STLM	A,AR0						   	;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v21_ch1
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v21_ch1:
;* Initializes Tx_v21[] workspace for v21 channel 1 operation.
;* On entry it expects:
;*	 DP=&Tx_block
;****************************************************************************

 .if (TX_V21_MODEM_CH1=ENABLED)
Tx_init_v21_ch1:					
V21_MESI_TxInitV21Ch1:
	CALL_	Tx_init_v21
	ST		#TX_V21_CH1_MESSAGE_ID,Tx_state_ID	
	ST		#TX_CH1_CARRIER,TxFSK_carrier
	RET_
 .endif

;****************************************************************************
;* _Tx_init_v21_ch2:
;* C function call: void Tx_init_v21_ch2(struct START_PTRS *)
;* Initializes Tx_block for V21 channel 2 modulator.
;****************************************************************************

 .if (COMPILER=ENABLED)&(TX_V21_MODEM_CH2=ENABLED)
_Tx_init_v21_ch2:				
_V21_MESI_TxInitV21Ch2:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_v21_ch2
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_v21_ch2:
;* Initializes Tx_v21[] workspace for v21 channel 2 operation.
;* On entry it expects:
;*	 DP=&Tx_block
;****************************************************************************

 .if (TX_V21_MODEM_CH2=ENABLED)
Tx_init_v21_ch2:					
V21_MESI_TxInitV21Ch2:
	CALL_	Tx_init_v21
	ST		#TX_V21_CH2_MESSAGE_ID,Tx_state_ID	
	ST		#TX_CH2_CARRIER,TxFSK_carrier
	RET_
 .endif

;****************************************************************************
;* Tx_init_v21:
;* Initializes Tx_v21[] workspace for v21 operation.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

 .if (TX_V21_MODEM=ENABLED)
Tx_init_v21:
V21_MESI_TxInitV21:
	CALL_	Tx_init_FSK
	ST		#TX_V21_INTERP,TxFSK_interpolate
	ST		#TX_V21_DEC,TxFSK_decimate
	ST		#TX_V21_FREQ_SHIFT,TxFSK_frequency_shift
	STPP	#Tx_v21_message,Tx_state,B
	RET_

;****************************************************************************
;* Tx_v21_message: v21 message data.
;****************************************************************************

Tx_v21_message:
	CALLD_	FSK_modulator
	MVDK	TxFSK_coef_ptr,AR6				;* AR6=TxFSK_coef_ptr

	LD		Tx_terminal_count,B
	BC_		Tx_state_return,BLT				;* return if TC<0
	SUB		Tx_symbol_counter,B
;++++#ifndef MESI_INTERNAL 03-24-2001
;	CC_		Tx_init_silence,BLEQ			;* if counter>=TC, Tx_init_silence
;++++#else   MESI_INTERNAL 03-24-2001
	CC_		Tx_init_silence,BLT				;* if counter>TC, Tx_init_silence
;++++#endif  MESI_INTERNAL 03-24-2001
	B_		Tx_state_return

;****************************************************************************
 .endif

	;****************************
	;**** receiver functions ****
	;****************************

;****************************************************************************
;* _Rx_init_v21_ch1:
;* C function call: void Rx_init_v21_ch1(struct START_PTRS *)
;* Initializes Rx_block for V21 channel 1 demodulator.
;****************************************************************************

 .if (COMPILER=ENABLED)&(RX_V21_MODEM_CH1=ENABLED)
_Rx_init_v21_ch1:				
_V21_MESI_RxInitV21Ch1:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_v21_ch1
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_v21_ch1:
;* Initializes Rx_block for V21 demodulator channel 1 operation
;* On entry it expects:
;*	AR0=&Rx_block
;****************************************************************************
											 
 .if (RX_V21_MODEM_CH1=ENABLED)
Rx_init_v21_ch1:					
V21_MESI_RxInitV21Ch1:
	ST		#FILTER_980_COEF,RxFSK_mark_coef
	ST		#FILTER_1180_COEF,RxFSK_space_coef
	CALL_	Rx_init_v21
	ST		#RX_V21_CH1_MESSAGE_ID,Rx_state_ID
	RET_
 .endif

;****************************************************************************
;* _Rx_init_v21_ch2:
;* C function call: void Rx_init_v21_ch2(struct START_PTRS *)
;* Initializes Rx_block for V21 channel 2 demodulator.
;****************************************************************************

 .if (COMPILER=ENABLED)&(RX_V21_MODEM_CH2=ENABLED)
_Rx_init_v21_ch2:				
_V21_MESI_RxInitV21Ch2:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_v21_ch2
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_v21_ch2:
;* Initializes Rx_block for V21 demodulator channel 2 operation
;* On entry it expects:
;*	AR0=&Rx_block
;****************************************************************************
										
 .if (RX_V21_MODEM_CH2=ENABLED)
Rx_init_v21_ch2:					
V21_MESI_RxInitV21Ch2:
	ST		#FILTER_1650_COEF,RxFSK_mark_coef
	ST		#FILTER_1850_COEF,RxFSK_space_coef
	CALL_	Rx_init_v21
	ST		#RX_V21_CH2_MESSAGE_ID,Rx_state_ID
	RET_
 .endif

;****************************************************************************
;* Rx_init_v21:
;* Initializes Rx_block for V21 demodulator operation.
;* On entry it expects:
;*	AR0=&Rx_block
;****************************************************************************

 .if (RX_V21_MODEM=ENABLED)
Rx_init_v21:
V21_MESI_RxInitV21:
	ST		#RX_V21_INTERP,RxFSK_interpolate
	ST		#RX_V21_DEC,RxFSK_decimate		
	ST		#RX_V21_COEF_LEN,RxFSK_coef_len	
	CALL_	Rx_init_FSK
	STPP	#Rx_v21_message,Rx_state,B
	RET_

;****************************************************************************
;* Rx_v21_message: v21 message data.
;****************************************************************************

Rx_v21_message:
	CALLD_	FSK_demodulator
	 MVDK	RxFSK_coef_ptr,AR6

	;**** check for loss of signal ****

	LD		Rx_status,B
	SUB		#LOSS_OF_LOCK,B
	BC_		Rx_state_return,BNEQ			;* return if no LOS
	LD		Rx_mode,B
	AND		#RX_LOS_FIELD,B
	CC_		 Rx_init_detector,BEQ			;* if mode&LOS=0, Rx_init_detector
	B_		Rx_state_return

;****************************************************************************
 .endif

;****************************************************************************
	.end
