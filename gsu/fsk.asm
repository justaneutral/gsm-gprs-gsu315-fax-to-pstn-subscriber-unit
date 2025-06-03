;****************************************************************************
;* Filename: fsk.asm
;* Date: 04-04-00
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, modulator, and demodulator for FSK.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"fsk.inc"
	.include	"config.inc"

	;**** modulator ****

TX_FSK_SCALE 			   	.set	7336	;* 32768*10exp(-13 dB/20) ==> -16 dB(RMS)

 .if $isdefed("XDAIS_API")
	.global _FSK_MESI_TxInitFSK
	.global FSK_MESI_TxInitFSK
	.global FSK_MESI_FSKmodulator
 .else
	.global _Tx_init_FSK
	.global Tx_init_FSK
	.global FSK_modulator
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
 .else
 .endif										;* "XDAIS_API endif

	;**** demodulator ****

 .if $isdefed("XDAIS_API")
	.global _FSK_MESI_RxInitFSK
	.global FSK_MESI_RxInitFSK
	.global FSK_MESI_FSKdemodulator
 .else
	.global _Rx_init_FSK
	.global Rx_init_FSK
	.global FSK_demodulator
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global _VCOEF_MESI_sinTable
	.asg	_VCOEF_MESI_sinTable, _sin_table
	.global FILTER_MESI_BandpassFilter
	.asg	FILTER_MESI_BandpassFilter, bandpass_filter
 .else
	.global _sin_table
	.global bandpass_filter
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
;* void Tx_init_FSK(struct START_PTRS *)
;* void Rx_init_FSK(struct START_PTRS *)
;* 
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TX_FSK_MODEM=ENABLED
;****************************************************************************
;* _Tx_init_FSK:
;* C function call: void Tx_init_FSK(struct START_PTRS *)
;* Initializes Tx_block for generic FSK modulator.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_FSK:				
_FSK_MESI_TxInitFSK:
	STLM	A,AR0						   	;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_FSK
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_FSK:
;* Initializes Tx_block for generic FSK modulator.
;* On entry it expects:
;*	AR0=&start_ptrs
;*	DP=&Tx_block
;****************************************************************************

Tx_init_FSK:
FSK_MESI_TxInitFSK:
	MVDK	Tx_start_ptrs,AR0				;* AR0=start_ptrs
	LD		#0,A
	STL		A,TxFSK_coef_ptr
	STL		A,TxFSK_vco_memory
	STL		A,TxFSK_frequency
	ST		#TX_FSK_SCALE,TxFSK_tone_scale
	ST		#-1,Tx_terminal_count
	LD		#1,A
	STL		A,Tx_Nbits
	STL		A,Tx_Nmask
	STL		A,Tx_sample_counter
	STL		A,Tx_symbol_counter
	RET_

;****************************************************************************
;* FSK_modulator:  Continuous phase frequency shift keyed modulator. It 
;* consists of an oscillator and a baud rate sub-sampler.  When the sub-
;* sampler samples, a new symbol (1 bit) is extracted from Tx_data[] and
;* converted into either a MARK or SPACE frequency value.
;* Expects the following setup on entry:
;*	DP=&Tx_block
;*	AR2=Tx_sample_head
;*	BK	Tx_sample_len
;*	AR6=TxFSK_coef_ptr
;* Modifies the following:
;****************************************************************************

FSK_modulator:					
FSK_MESI_FSKmodulator 	
	MVDK	TxFSK_decimate,AR0				;* AR0=decimate
	
	;**** transmit oscillator ****

	LDU		TxFSK_vco_memory,A				;* A=TxFSK_vco_memory
	ADDS	TxFSK_frequency,A				;* A=vco_memory+TxFSK_frequency
	STL		A,TxFSK_vco_memory				;* TxFSK_vco_memory+=TxFSK_frequency
	LDU		TxFSK_vco_memory,A				;* mask off upper 16 bits
	SFTL	A,-SIN_TABLE_SHIFT				;* A=vco_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[vco_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR7
	 NOP
	 LD		TxFSK_tone_scale,T				;* T=TxFSK_tone_scale
	MPY		*AR7,A							;* A=sample*tone_scale
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR2							;* put it in Tx_sample[*]
	LD		TxFSK_tone_scale,T				;* T=TxFSK_tone_scale
	MPY		*AR2,A							;* A=sample*tone_scale
 .endif
	MPYA	Tx_scale						;* B=Tx_scale*(tone1+tone2)
	STH		B,*AR2+%						;* Tx_sample[*++]=sin

	;**** update coef_ptr and check for new symbol ****

	MVDK	TxFSK_interpolate,BK			;* BK=interpolate
	MAR		*AR6+0%							;* AR6=(ptr+dec)%interp
	MVKD	AR6,TxFSK_coef_ptr				;* update TxFSK_coef_ptr
	 
	LD		TxFSK_coef_ptr,B
	SUB		TxFSK_decimate,B
	RCD_	BGEQ							;* return if coef_ptr>=dec
	 ADDM	#1,Tx_sample_counter			;* Tx_sample_counter++

	MVDK	Tx_data_tail,AR7				;* AR7=Tx_data_tail
	MVDK	Tx_data_len,BK					;* BK=TX_DATA_LEN
	 LD		TxFSK_frequency_shift,B
	 LD		#1,A							
	AND		*AR7+%,A						;* A=Tx_data[*++]&1
	 MVKD	AR7,Tx_data_tail				;* update Tx_data_tial
	XC		1,ANEQ							;* if k!=0 ...
	 NEG	B,B								;* ... j=-j
	ADD		TxFSK_carrier,B					;* B=TxFSK_carrier+j
	STL		B,TxFSK_frequency				;* update TxFSK_frequency
	RETD_
	 ADDM	#1,Tx_symbol_counter			;* Tx_symbol_counter++

;****************************************************************************
 .endif		;* endif TX_FSK_MODEM

	;****************************
	;**** receiver functions ****
	;****************************


 .if RX_FSK_MODEM=ENABLED
;****************************************************************************
;* _Rx_init_FSK:
;* C function call: void Rx_init_FSK(struct START_PTRS *)
;* Initializes Rx_block for FSK demodulator.
;****************************************************************************

 .if COMPILER=ENABLED
_Rx_init_FSK:				
_FSK_MESI_RxInitFSK:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_FSK
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_FSK:
;* Initializes Rx_block for FSK demodulator 
;* On entry it expects:
;*	AR0=&start_ptrs
;*	DP=&Rx_block
;****************************************************************************
											 
Rx_init_FSK:					
FSK_MESI_RxInitFSK:
	LD		#0,A
	LD		Rx_data_head,B
	STL		B,Rx_data_tail					;* data_tail=data_head
	STL		A,RxFSK_coef_ptr
	STL		A,RxFSK_sym_clk_memory
	STL		A,RxFSK_baud_counter
	STL		A,RxFSK_LOS_memory
	STL		A,Rx_sample_counter
	STL		A,Rx_symbol_counter
	LD		#1,A
	STL		A,Rx_Nbits
	STL		A,Rx_Nmask

	;**** compute the symbol level for LOS detector ****

	CALLD_	Rx_compute_sym_level
	 MVDK	Rx_sample_tail,AR2
	STH		A,RxFSK_LOS_threshold
	ST		#STATUS_OK,Rx_status
	RET_

;****************************************************************************
;* Rx_compute_sym_level: Calculates the absolute level of the FSK signal	
;* present at the time of detection, and then scales it by 					
;* FSK_LOS_THRESHOLD. When the demod is running, the current FSK signal		
;* level is compared with this threshold to determine if the signal has 	
;* dropped by greater than FSK_LOS_THRESHOLD, indicating LOSS_OF_LOCK.		
;* On entry it expects:
;*	DP=&Rx_block
;*	AR2=Rx_sample_tail
;* Returns:
;*	AH=sym_level estimate
;****************************************************************************

Rx_compute_sym_level:
	MVDK	RxFSK_coef_len,BRC				;* BRC=LEN-2
	MVMM	AR2,AR3							;* AR3=Rx_sample_tail
	CALLD_	bandpass_filter
	 MVDK	RxFSK_mark_coef,AR0			
	STH		A,RxFSK_sym
	MVDK	RxFSK_coef_len,BRC				;* BRC=LEN-2
	MVMM	AR2,AR3							;* AR3=Rx_sample_tail
	CALLD_	bandpass_filter
	 MVDK	RxFSK_space_coef,AR0				
	ADD		RxFSK_sym,16,A

	STH		A,RxFSK_sym
	RETD_
	 MPY	RxFSK_sym,#FSK_LOS_THRESHOLD,A

;****************************************************************************
;* FSK_demodulator: Frequency Shift Keyed demodulator. It uses a dual 		
;* Hilbert transformer/bandpass filter ratio detection technique to demod-	
;* ulate the received signal. The demodulated bitsa are written to Rx_data.	
;* This algorithm has a Loss Of Signal (LOS) detector that reports 			
;* Rx->status=LOSS_OF_LOCK of the signal level drops by more than 6dB from 	
;* the initial level. If the user has called Rx_init_FSK() from a signal 	
;* detector where the initial signal level was correctly calculated, then	
;* the LOSS_OF_LOCK report will be valid. If the FSK_demodulator() is run-	
;* ning continuously, then the user must determine valid FSK signal 		
;* detection from the data, and then call Rx_init_FSK() to initialize the 	
;* LOS detector.	
;* Expects the following setup on entry:
;*	DP=&Rx_block
;*	AR2=Rx_sample_tail
;*	AR6=Rx_coef_ptr
;* Returns:
;*	A=0 if no symbol was produced
;*	A=1 if symbol was produced
;****************************************************************************

FSK_demodulator:					
FSK_MESI_FSKdemodulator: 	
	MVDK	Rx_sample_stop,AR0		
	CMPR	EQ,AR2							;* sample_stop-sample_tail
	LD		#0,A						
	RCD_	TC								;* return(0) if stop=tail
	 MVDK	RxFSK_decimate,AR0				;* AR0=decimate
	MAR		*AR2+%							;* sample_tail++
	MVKD	AR2,Rx_sample_tail

	;**** check for 2xbaud period ****

	MVDK	RxFSK_interpolate,BK			;* BK=interpolate
	MAR		*AR6+0%							;* AR6=(ptr+dec)%interp
	MVKD	AR6,RxFSK_coef_ptr				;* update RxFSK_coef_ptr
	 
	LD		RxFSK_coef_ptr,B
	SUB		RxFSK_decimate,B
	BCD_	FSK_demodulator,BGEQ			;* branch if coef_ptr>=dec
	 MVDK	Rx_sample_len,BK				;* BK=Rx_sample_len

	;**** mark and space frequency filters ****

	LD		RxFSK_sym,B						;* B=RxFSK_sym
	STL		B,RxFSK_sym_nm1;				;* RxFSK_sym_nm1=RxFSK_sym
	MVMM	AR2,AR3							;* AR3=RxFSK_sample_tail
	MVDK	RxFSK_coef_len,BRC				;* BRC=LEN-2
	CALLD_	bandpass_filter
	 MVDK	RxFSK_space_coef,AR0
	STH		A,RxFSK_sym_level				;* sym_level=space

	MVMM	AR2,AR3							;* AR3=RxFSK_sample_tail
	MVDK	RxFSK_coef_len,BRC				;* BRC=LEN-2
	CALLD_	bandpass_filter
	 MVDK	RxFSK_mark_coef,AR0
 .if IQ_DAC_WRITE=ENABLED
	STH		A,-1,temp0
	LD		RxFSK_sym_level,B
	STL		B,-1,temp1
 .endif
	SUB		RxFSK_sym_level,16,A,B			;* A=mark-space
	STH		B,RxFSK_sym						;* sym=mark-space
	ADD		RxFSK_sym_level,16,A,B			;* A=mark+space
	STH		B,RxFSK_sym_level				;* sym_level=mark-space

	;**** baud rate sampling ****

	ADDM	#1,RxFSK_baud_counter			;* RxFSK_baud_counter++
	CMPM	RxFSK_baud_counter,#2
	BCD_	FSK_demodulator,NTC				;* branch to while if baud_counter!=2
	 LD		RxFSK_sym,A
	 LD		RxFSK_sym_hat,B
	ST		#0,RxFSK_baud_counter			;* RxFSK_baud_counter=0
	STL		B,RxFSK_sym_hat_nm2				;* RxFSK_sym_hat_nm2=RxFSK_sym_hat
	ST		#SLICE1,RxFSK_sym_hat		
	XC		2,ALEQ
	 ST		#-SLICE1,RxFSK_sym_hat			;* if sym<=0, sym_hat=-SLICE1

	;**** constellation display ****

 .if IQ_DAC_WRITE=ENABLED
	LD		temp1,16,A
	OR		temp0,A
	CALL_	IQ_DAC_write
 .endif

	;**** data sink ****

	LD		#1,A
	LD		RxFSK_sym,B						;* B=RxFSK_sym
	 MVDK	Rx_data_head,AR7
	MVDK	Rx_data_len,BK
	XC		1,BLEQ
	 LD		#0,A							;* if sym<=0, A=0
	STL		A,*AR7+%						;* Rx_data[*++]=bit
	MVKD	AR7,Rx_data_head				;* update Rx_data_head
	ADDM	#1,Rx_symbol_counter			;* Rx_symbol_counter++

	;**** symbol timing ****
	
RxFSK_symbol_timing:				
	LD		RxFSK_sym_hat,16,A				;* A=_sym_hat
	SUB		RxFSK_sym_hat_nm2,16,A			;* A-=_sym_hat_nm2
	MPYA	RxFSK_sym_nm1					;* B=_sym_nm1*(A)
	ADD		RxFSK_sym_clk_memory,16,B		;* B+=_sym_clk_memory
	STH		B,3,RxFSK_sym_clk_memory		;* update _sym_clk_memory<<3
	MVDK	RxFSK_decimate,AR0				;* AR0=decimate
	SUB		RxFSK_sym_level,16,B,A			;* A=error-thr
	BCD_	RxFSK_advance,ALEQ				;* branch if _sym_clk_memory<=thr
	 MVDK	RxFSK_interpolate,BK			;* BK=interpolate

RxFSK_retard:						
	ST		#0,RxFSK_sym_clk_memory			;* RxFSK_sym_clk_memory=0
	MAR		*AR6+0%							;* (coef_ptr+dec)%interp
	MVKD	AR6,RxFSK_coef_ptr				;* update RxFSK_coef_ptr
	B_		RxFSK_symbol_timing_end
								
RxFSK_advance:					
	ADD		RxFSK_sym_level,16,B,A			;* A=error+thr
	BC_		RxFSK_symbol_timing_end,AGEQ	;* branch if sym_clk_memory>=-thr
	ST		#0,RxFSK_sym_clk_memory			;* RxFSK_sym_clk_memory=0
	ST		#-1,RxFSK_baud_counter		
	MAR		*AR6-0%							;* (coef_ptr-dec)%interp
	MVKD	AR6,RxFSK_coef_ptr				;* update RxFSK_coef_ptr
RxFSK_symbol_timing_end:				

	;**** Loss OF Signal (LOS) detector ****

RxFSK_LOS_detector:					
	LD		RxFSK_LOS_threshold,B
	SUB		RxFSK_sym_level,B				;* B=RxFSK_LOS_threshold-sym_level
	 ADDM	#1,RxFSK_LOS_memory				;* LOS_memory++
	XC		2,BLT							;* if sym_level_nm1<sym_level
	 ST		#0,RxFSK_LOS_memory				;* LOS_memory++
	LD		RxFSK_LOS_memory,B
	SUB		#FSK_LOS_LIMIT,B
	RCD_	BLEQ							;* return(1) if LOS_memory<=THR
	 LD		#1,A
	 NOP
	RETD_									;* return(1)
	 ST		#LOSS_OF_LOCK,Rx_status

;****************************************************************************
 .endif		;* endif RX_FSK_MODEM

;****************************************************************************
	.end
