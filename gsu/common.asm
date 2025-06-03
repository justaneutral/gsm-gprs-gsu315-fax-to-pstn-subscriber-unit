;****************************************************************************		
;* Filename: common.asm
;* Date: 02-14-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: APSK modulator, APSK demodulator, other common modules
;****************************************************************************
			
	.include	 "vmodem.inc"
	.include	 "common.inc"
	.include	 "config.inc"

	;**** global functions ****

 .if $isdefed("XDAIS_API")
	.global COMMON_MESI_APSKmodulator
	.global COMMON_MESI_APSKdemodulator
	.global COMMON_MESI_slicerReturn
	.global COMMON_MESI_timingReturn
	.global COMMON_MESI_decoderReturn
	.global COMMON_MESI_noTiming
	.global COMMON_MESI_sgnTiming
	.global COMMON_MESI_APSKTiming
	.global COMMON_MESI_RxTrainLoops
	.global COMMON_MESI_RxDetectEQ
	.global COMMON_MESI_agcGainEstimator
	.global COMMON_MESI_RxFirAutocorrelator
 .else
	.global APSK_modulator
	.global APSK_demodulator
	.global slicer_return
	.global timing_return
	.global decoder_return
	.global no_timing
	.global sgn_timing
	.global APSK_timing
	.global Rx_train_loops
	.global Rx_detect_EQ
	.global agc_gain_estimator
	.global Rx_fir_autocorrelator
	.global init_fir_site
	.global fir_site_pmad
	.global fir_site
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global _VCOEF_MESI_sinTable
	.asg	_VCOEF_MESI_sinTable, _sin_table
	.global _VCOEF_MESI_DFTCoef
	.asg	_VCOEF_MESI_DFTCoef, _DFT_coef
 .else
	.global _sin_table
	.global _DFT_coef
 .endif										;* "XDAIS_API endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global check_baud_counter
	.global phase_detector
	.global hard_decision
	.global Itiming
	.global Qtiming
	.global ItimingOPSK
	.global QtimingOPSK
	.global end_timing
	.global coarse_timing
	.global check_advance
	.global check_retard
	.global LOS_detector
	.global Rx_fir_else
	.global Rx_fir_autocorrelator
	.global	IIR_resonator
 .endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif
 .if IQ_DAC_WRITE=ENABLED
 .if $isdefed("XDAIS_API")
 	.global USER_MESI_IQDACwrite
 	.asg USER_MESI_IQDACwrite, IQ_DAC_write
 .else
 	.global  IQ_DAC_write         
 .endif                         
 .endif

	.sect		"vtext"

;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TX_COMMON_MODEM=ENABLED
;****************************************************************************
;* APSK_modulator: M-PSK modulator
;* This routine generates num_samples of M-PSK modulated samples from 
;* the bits in Tx_data[] and places them in Tx_sample[]. Both of these buffers
;* are CIRCULAR buffers and must be positioned correctly in memory.
;* 
;* Expects the following setup on entry:
;* On entry it expects:
;*	DP=&Tx_block
;*	AR2=Tx_sample_head
;*	BK=Tx_fir_len
;* Modifies the following:
;*	
;****************************************************************************

APSK_modulator:					
COMMON_MESI_APSKmodulator:
 .if ON_CHIP_COEFFICIENTS=ENABLED
	MVDK	Tx_fir_tail,AR4					;* AR4= DAG1=Tx_fir_tail
	LD		Tx_coef_start,A			
	ADD		Tx_coef_ptr,A			
	ADD		Tx_sym_clk_phase,A				;* A=coef_start+coef_ptr+sym_clk_phase
	STLM	A,AR3							;* AR3= DAG2=coef_ptr
	MVDK	Tx_interpolate,AR0
	MVDK	Tx_fir_taps,BRC
	RPTBD	Tx_interpolate_loop
	LD		#0,B
	MPY		*AR3+,*AR4+,A					;* dummy, AR3=DAG1+1, AR4=DAG2+1
	 MAS	*AR3-,*AR4-,B					;* B-=*(DAG1+1) * *(DAG2+1)
	 MAC	*AR3+,*AR4,B					;* B+=*(DAG1) * *(DAG2)
	 MAR	*AR4-%							;* (--DAG1)%LEN
Tx_interpolate_loop:
	 MAR	*AR3+0							;* DAG2+=Tx_interpolate
 .else
	MVDK	Tx_start_ptrs,AR1				;* AR1=start_ptrs
	MVDK	*AR1(Tx_block_start),AR5
	MAR		*+AR5(Tx_interpolate)			;* AR5=&Tx_interpolate
	LD		#0,B
	STM		#-2,AR0							;* AR0=Tx_fir_ptr stride

	MVDK	Tx_fir_tail,AR4					;* AR4=Tx_fir_tail
	MAR		*+AR4(1)						;* imag fir
	LD		#1,16,A							;* offset to imag coefs
	ADD		Tx_coef_ptr,16,A				;* A=Tx_coef_start+Tx_coef_ptr
	ADD		Tx_sym_clk_phase,16,A			
	ADD		Tx_coef_start,16,A			

	CALLD_	fir_site
	MVDK	Tx_fir_taps,BRC
	NEG		B
	MVDK	Tx_fir_tail,AR4					;* AR4=Tx_fir_tail
	LD		Tx_coef_ptr,16,A				;* A=Tx_coef_start+Tx_coef_ptr
	ADD		Tx_sym_clk_phase,16,A			
	ADD		Tx_coef_start,16,A			
	CALLD_	fir_site
	MVDK	Tx_fir_taps,BRC
 .endif										;* ON_CHIP_COEFFICIENTS endif		

	;**** update coef_ptr and check for new symbol ****
	
	MVDK	Tx_interpolate,BK			
	MVDK	Tx_coef_ptr,AR5
	MVDK	Tx_decimate,AR0
	MAR		*AR5+0%
	MVKD	AR5,Tx_coef_ptr					;* (Tx_coef_ptr+Tx_decimate)%Tx_interpolate
	MVDK	Tx_sample_len,BK				;* BK=TX_SAMPLE_LEN
	ADD		#4000h,1,B,A					;* B=rounded sample
	MPYA	Tx_scale						;* B=sample*scale
	STH		B,*AR2+%						;* Tx_sample[*++]=sample
	LD		Tx_coef_ptr,B
	SUB		Tx_decimate,B
	RCD_	BGEQ							;* return if Tx_coef_ptr>=Tx_decimate	
	 ADDM	#1,Tx_sample_counter
	 
	MVDK	Tx_fir_len,BK					;* BK=TX_FIR_LEN
	ADDM	#1,Tx_symbol_counter			;* Tx_symbol_counter++
	MVDK	Tx_fir_tail,AR4					;* AR4=Tx_fir_tail
	 LD		*+AR4(2)%,A						;* AR4+=2%

	;**** Tx symbol clock offset ****

	LD		Tx_sym_clk_offset,B			 
	RCD_	BEQ								;* return if offset=0
	 MVKD	AR4,Tx_fir_tail					;* Tx_fir_tail=(Tx_fir_tail+2)%LEN
	LD		Tx_sym_clk_memory,B
	ADD		Tx_sym_clk_offset,B
	STL		B,Tx_sym_clk_memory				;* sym_clk_memory+=sym_clk_offset
Tx_advance:
	SUB		#SYM_CLK_THRESHOLD,B,A
	BCD_	Tx_retard,ALEQ					;* branch if clk_nm1 <=THR
	 ADD	#SYM_CLK_THRESHOLD,B,A
	ST		#0,Tx_sym_clk_memory			
	LD		Tx_sym_clk_phase,B
	SUB		#2,B
	STL		B,Tx_sym_clk_phase				;* Tx_sym_clk_phase-=2
	RCD_	BGEQ							;* return if Tx_sym_clk_phase>=0
	 MVDK	Tx_interpolate,BK			
	 MAR	*+AR4(-2)%						;* AR4-=2%
	MVKD	AR4,Tx_fir_tail					;* Tx_fir_tail=(Tx_fir_tail-2)%LEN
	MVDK	Tx_coef_ptr,AR5
	MVDK	Tx_decimate,AR0
	MAR		*AR5-0%
	MVKD	AR5,Tx_coef_ptr					;* (Tx_coef_ptr-Tx_decimate)%Tx_interpolate
	MVKD	AR0,Tx_sym_clk_phase			;* sym_clk_phase=dec
	RETD_
	 ADDM	#-1,Tx_symbol_counter			;* Tx_symbol_counter--
Tx_retard:
	RC_		 AGEQ							;* return if clk_nm1>=-THR
	ST		#0,Tx_sym_clk_memory			
	LD		Tx_sym_clk_phase,B
	ADD		#2,B
	STL		B,Tx_sym_clk_phase				;* Tx_sym_clk_phase+=2
	SUB		Tx_decimate,B,A
	RCD_	ALEQ							;* return if Tx_sym_clk_phase<=Tx_decimate
	 MVDK	Tx_interpolate,BK			
	MVDK	Tx_coef_ptr,AR5
	MVDK	Tx_decimate,AR0
	MAR		*AR5+0%
	MVKD	AR5,Tx_coef_ptr					;* (Tx_coef_ptr+Tx_decimate)%Tx_interpolate
	RETD_
	 ST		#0,Tx_sym_clk_phase				;* sym_clk_phase=0

;****************************************************************************
 .endif

	;**************************
	;**** receiver modules ****
	;**************************

 .if RX_COMMON_MODEM=ENABLED
;****************************************************************************
;* APSK_demodulator: M-PSK demodulator 
;* Expects the following setup on entry:
;*	DP=&Rx_block
;*	AR2=Rx_sample_head
;*	BK=Rx_sample_len
;*	
;* Modifies the following:
;*	
;****************************************************************************

APSK_demodulator:				
COMMON_MESI_APSKdemodulator:
	LDM		AR2,A
	SUBS	Rx_sample_stop,A				;* Rx_sample_tail-Rx_sample_stop
	RCD_	AEQ								;* return if Rx_sample_stop==Rx_sample_tail
	 LDM	AR2,A
	 SUBS	Rx_sample_ptr,A					;* Rx_sample_tail-Rx_sample_ptr
	BCD_	APSK_demodulator,ANEQ			;* branch if Rx_sample_ptr!=Rx_sample_tail
	 MAR	*AR2+%							;* Rx_sample_tail++%
	 LD		Rx_coef_ptr,T					;* T=Rx_coef_ptr

	;**** interpolator-decimator low pass filters ****

 .if ON_CHIP_COEFFICIENTS=ENABLED
	MVDK	Rx_sample_ptr,AR4				;* AR4= DAG1=Rx_sample_ptr
	MPY		Rx_oversample,A					;* A=oversample*coef_ptr<<1
	ADD		Rx_sym_clk_phase,1,A			;* A=(oversample*ptr+offset)<<1
	ADD		Rx_coef_start,A					;* A+=Rx_coef_start
	STLM	A,AR3							;* AR3= DAG2=coef_start+(oversample*ptr+offset)<<1
	LD		#4000h,1,A						;* initialize round	in A
	LD		A,B								;* initialize round in B
	MVDK	Rx_fir_taps,BRC
	RPTBD	Rx_interpolate_loop
	MVDK	Rx_interpolate,AR0
	 MAC	*AR3+,*AR4,A					;* A+=*(DAG1) * *(DAG2)
	 MAC	*AR3-,*AR4,B					;* B+=*(DAG1) * *(DAG2)
	 MAR	*AR4-%							;* (--DAG1)%LEN
Rx_interpolate_loop:
	 MAR	*AR3+0							;* DAG2+=Rx_interpolate
	STH		A,I
	STH		B,Q
 .else

	MPY		Rx_oversample,B					;* B=oversample*coef_ptr<<1
	ADD		Rx_sym_clk_phase,1,B			;* B=(oversample*ptr+offset)<<1
	ADD		Rx_coef_start,B					;* B+=Rx_coef_start
	STL		B,temp1							;* temp1=coef_start+(oversample*ptr+offset)<<1
	LD		temp1,16,A
	MVDK	Rx_start_ptrs,AR1				;* AR1=start_ptrs
	MVDK	*AR1(Rx_block_start),AR5
	MAR		*+AR5(Rx_interpolate)			;* AR5=&Rx_interpolate 
	STM		#-1,AR0							;* AR0=stride=1

	MVDK	Rx_sample_ptr,AR4				;* AR4=Rx_sample_ptr
	LD		#4000h,1,B						;* initialize round
	CALLD_	fir_site
	 MVDK	Rx_fir_taps,BRC
	STH		B,I
	LD		temp1,16,A						;* temp1=oversample*ptr+offset
	ADD		#1,16,A
	MVDK	Rx_sample_ptr,AR4				;* AR4=Rx_sample_ptr
	LD		#4000h,1,B						;* initialize round
	CALLD_	fir_site
	 MVDK	Rx_fir_taps,BRC
	STH		B,Q
 .endif										;* ON_CHIP_COEFFICIENTS endif		

	;**** Rx_sample_ptr and Rx_coef_ptr update ****

 .if ON_CHIP_COEFFICIENTS=ENABLED
	LD		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_INCR,A,B					;* B=(timing_ptr+coef_ptr+FIR_INCR)
	STLM	B,AR4
	ADD		#COEF_INCR,A					;* B=*(timing_ptr+coef_ptr+COEF_INCR)
	STLM	A,AR5
	 MVDK	*AR4,AR0
	 MVDK	Rx_sample_ptr,AR7		
	MAR		*AR7+0%							;* AR7=(AR7+AR0)%
	MVKD	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LD		*AR5,A
	STL		A,Rx_coef_ptr					;* update Rx_coef_ptr
 .else
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_INCR,A						;* B=(timing_ptr+coef_ptr+FIR_INCR)
	READA	temp0					 
	MVDK	temp0,AR0						;* AR0=*(timing_ptr+coef_ptr+FIR_INCR)
	MVDK	Rx_sample_ptr,AR7		
	MAR		*AR7+0%							;* AR7=(AR7+AR0)%
	MVKD	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#COEF_INCR,A					;* B=*(timing_ptr+coef_ptr+COEF_INCR)
	READA	Rx_coef_ptr						;* update Rx_coef_ptr	
 .endif

	;**** carrier rotator ****

	LDU		LO_memory,A						;* A=LO_memory
	ADD		LO_frequency,A					;* A=vco_memory+LO_frequency
	STL		A,LO_memory						;* LO_memory+=LO_frequency
	LDU		LO_memory,A						;* A=LO_memory
	SFTL	A,-SIN_TABLE_SHIFT				;* A=vco_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[vco_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR5
	STM		#(SIN_TABLE_LEN/4),AR0
	 LD		#0,B							;* B=0
	 LD		I,16,A
	MASA	*AR5+0							;* B=-I*sin, T=sin
	MPY		Q,A								;* A=Q*sin
	LD		*AR5,T
	MAC		I,A								;* A=I*cos+Q*sin
	MAC		Q,B								;* B=Q*cos-I*sin
	STH		A,I								;* I=I*cos+Q*sin
	STH		B,Q								;* Q=Q*cos-I*sin
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	temp0							;* temp0=sin
	ADD		#(SIN_TABLE_LEN/4),A
	READA	temp1							;* temp1=cos
	LD		temp1,T							;* T=cos
	MPY		I,A								;* A=I*cos
	MPY		Q,B								;* B=Q*cos
	LD		temp0,T							;* T=sin
	MAC		Q,A								;* A=I*cos+Q*sin
	MAS		I,B								;* B=Q*cos-I*sin
	STH		A,I
	STH		B,Q
 .endif

	;**** AGC multiply ****

	MVDK	Rx_fir_ptr,AR4
	STM		#RX_FIR_LEN,BK
	LD		agc_gain,T
	MPY		I,B								;* B=I*agc_gain
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	STH		B,2,*AR4+%						;* Rx_fir[*++]=I*agc_gain*4
;	STH		B,2,I
;	MPY		Q,B								;* B=Q*agc_gain
;	STH		B,2,*AR4+%						;* Rx_fir[*++]=Q*agc_gain*4
;	STH		B,2,Q
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	STH		B,(8-OP_POINT_SHIFT),*AR4+%		;* Rx_fir[*++]=I*agc_gain (scaled for OP_POINT)
	STH		B,(8-OP_POINT_SHIFT),I
	MPY		Q,B								;* B=Q*agc_gain
	STH		B,(8-OP_POINT_SHIFT),*AR4+%		;* Rx_fir[*++]=Q*agc_gain (scaled for OP_POINT)
	STH		B,(8-OP_POINT_SHIFT),Q
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	MVKD	AR4,Rx_fir_ptr

	LD		EQ_2mu,B
	BCD_	EQ_fir_else,BLT					;* branch if EQ_2mu<0
	 MAR	*AR4-%
	 MAR	*AR4-%

	;**** fractionally spaced adaptive equalizer ****

	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(EQ_coef_start),AR3
	STM		#-2,AR0
	MVDK	EQ_taps,BRC						;* for EQ_LEN times ...
	RPTBD	EQ_fir_loop
	LD		#0,A
	LD		#0,B
	 MAC	*AR3+,*AR4+,A					;* A+=fir[j]*coef[4*i], T=coef
	 MAC	*AR4,B							;* B+=fir[j+1]*coef[4*i]
	 MAS	*AR3+,*AR4-,A					;* A-=fir[j+1]*coef[4*i+2], T=coef
EQ_fir_loop:	
	 MAC	*AR4+0%,B						;* B+=fir[j]*coef[4*i+2]
	ADD		#800h,1,A						;* round
	STH		A,3,IEQ							;* scale up by 8
	ADD		#800h,1,B						;* round
	BD_		EQ_fir_end
	 STH	B,3,QEQ							;* scale up by 8

EQ_fir_else: 
	MVDK	EQ_taps,AR0						;* AR0=EQ_TAPS-1
	MAR		*AR4-0%							;* adjust to proper delay position
	LD		*AR4+%,A
	STL		A,IEQ
	LD		*AR4-%,B
	STL		B,QEQ
EQ_fir_end:

	;**** symbol delay line ****

	LD		Inm2,B
	STL		B,Inm3							;* Inm3=Inm2
	LD		Inm1,B
	STL		B,Inm2                          ;* Inm2=Inm1
	LD		Iprime,B
	STL		B,Inm1							;* Inm1=Iprime
	
	LD		Qnm2,B
	STL		B,Qnm3							;* Qnm3=Qnm2
	LD		Qnm1,B
	STL		B,Qnm2                          ;* Qnm2=Qnm1
	LD		Qprime,B
	STL		B,Qnm1							;* Qnm1=Qprime

	;**** VCO ****

	LDU		vco_memory,A					;* A=vco_memory
	ADD		frequency_est,A					;* A=vco_memory+frequency_est
	STL		A,vco_memory					;* vco_memory+=frequency_est
	LDU		vco_memory,A					;* A=vco_memory
	SFTL	A,-SIN_TABLE_SHIFT				;* A=vco_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[vco_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR5
	STM		#(SIN_TABLE_LEN/4),AR0
	 LD		#0,B							;* B=0
	 LD		QEQ,16,A

	;**** phase rotation ****

	MASA	*AR5+0							;* B=-QEQ*sin, T=sin
	MVKD	T,SIN	
	MPY		IEQ,A							;* A=IEQ*sin
	LD		*AR5,T
	MVKD	T,COS	
	MAC		IEQ,B							;* B=IEQ*cos+QEQ*sin
	MAC		QEQ,A							;* A=QEQ*cos-IEQ*sin
	STH		B,Iprime						;* Iprime=IEQ*cos+QEQ*sin
	STH		A,Qprime						;* Qprime=QEQ*cos-IEQ*sin
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	SIN
	ADD		#(SIN_TABLE_LEN/4),A
	READA	COS

	;**** phase rotation ****

	LD		COS,T	
	MPY		IEQ,A							;* A=IEQ*cos
	MPY		QEQ,B							;* B=QEQ*cos
	LD		SIN,T	
	MAS		QEQ,A							;* A=IEQ*cos-QEQ*sin
	MAC		IEQ,B							;* B=QEQ*cos+IEQ*sin
	STH		A,Iprime
	STH		B,Qprime
 .endif

	;**** decrement and check baud counter ****

check_baud_counter:
	ADDM	#-1,Rx_baud_counter
	LD		Rx_baud_counter,B
	BCD_	APSK_demodulator,BGT			;* loop if baud_counter>0
	 MVDK	Rx_sample_len,BK
	ST		#2,Rx_baud_counter				;* Rx_baud_counter=2

	;**** slicer ****

	LDPP	slicer_ptr,B
	LD		Ihat,A
	STL		A,Ihat_nm2						;* Ihat_nm2=Ihat
	BACCD_	B						 		;* branch to *slicer_ptr
	 LD		Qhat,A
	 STL	A,Qhat_nm2						;* Qhat_nm2=Qhat
slicer_return:
COMMON_MESI_slicerReturn:

	;**** AGC update ****

	LD		agc_K,B
	BCD_	end_agc_update,BEQ				;* branch if agc_K==0	
	 SQUR	Ihat,A							;* A=Ihat^2
	 SQURA 	Qhat,A							;* A=+Qhat^2
	SQURS	Iprime,A						;* A-=Iprime^2
	SQURS	Qprime,A						;* A-=Qprime^2
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	SFTA	A,6								;* R0=(Ihat^2+Qhat^2-I^2-Q^2)*64 
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	SFTA	A,OP_POINT_SHIFT				;* A=(Ihat^2+Qhat^2-I^2-Q^2)<<OP_POINT_SHIFT
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	MPYA	agc_K							;* B=agc_K*(A)
	ADD		#4000h,1,B						;* round
	ADD		agc_gain,16,B					;* B+=agc_gain
	 STH	B,agc_gain						;* update agc_gain
	 NOP
	XC		2,BLT			
	 ST		#0,agc_gain						;* clamp if agc_gain<0
end_agc_update:

	;**** phase detector ****

phase_detector:
;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;;	LD		What,T			
;;	MPY		Iprime,A
;;	ADD		#20h,1,A
;;	SFTA	A,9				
;;	MPYA	Qhat							;* B=Qhat*temp0
;;	LD		What,T			
;;	MPY		Qprime,A
;;	ADD		#20h,1,A
;;	SFTA	A,9								;* A= temp1=(Qprime*What+0x20)<<9
;;	MASA	Ihat							;* B=Qhat*temp0-Ihat*temp1
;;	STH		B,3,phase_error					;* update phase_error
;;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	LD		What,T			
;	MPY		Iprime,A
;	SFTA	A,OP_POINT_SHIFT				
;	MPYA	Qhat							;* B=Qhat*temp0
;	LD		What,T			
;	MPY		Qprime,A
;	SFTA	A,OP_POINT_SHIFT				;* A= temp1=(Qprime*What)<<OP_POINT_SHIFT
;	MASA	Ihat							;* B=Qhat*temp0-Ihat*temp1
;	STH		B,OP_POINT_SHIFT,phase_error	;* update phase_error
;;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
	LD		What,T			
	MPY		Iprime,A
	SFTA	A,OP_POINT_SHIFT				
	MPYA	Qhat							;* B=Qhat*temp0
	LD		What,T			
	MPY		Qprime,A
	SFTA	A,OP_POINT_SHIFT				;* A= temp1=(Qprime*What)<<OP_POINT_SHIFT
	MASA	Ihat							;* B=Qhat*temp0-Ihat*temp1
	STH		B,OP_POINT_SHIFT,phase_error	;* update phase_error
 .if $isdefed("SQUARE_ROOT_WHAT")
	SFTA	B,OP_POINT_SHIFT,A				;* A=(Qhat*temp0-Ihat*temp1)<<OP_POINT_SHIFT = temp0
	MPYA	What							;* B=What*temp0
 .endif		;* SQUARE_ROOT_WHAT
	STH		B,OP_POINT_SHIFT,phase_error	;* phase_error=(What*temp0)<<OP_POINT_SHIFT
;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS

	;**** loop filter ****

	LD		loop_memory,16,B				;* B+=loop_K2*phase_error
	ADDS	loop_memory_low,B				;* B+=loop_K2*phase_error
	SFTA	B,1
	LD		phase_error,T
	MAC		loop_K2,B						;* B+=loop_K2*phase_error
	STH		B,-1,loop_memory				;* update loop_memory
	STL		B,-1,loop_memory_low			;* update loop_memory
	MPY		loop_K1,A						;* A=loop_K1*phase_error
	 STH	A,3,frequency_est				;* scale up
	 STH	B,temp0
	 LD		temp0,16,B
	ADD		frequency_est,16,B				;* B+=loop_K1*phase_error*SCALE
	STH		B,frequency_est					;* update frequency_est

	;**** phase jitter parallel resonators ****
	
	MVDK	Rx_start_ptrs,AR7			
	MVDK	*AR7(Rx_block_start),AR7
	MVMM	AR7,AR6
	LD		phase_error,T
	CALLD_	IIR_resonator					;* returns result in AH
	 MAR	*+AR7(PJ1_coef)
	ADD		frequency_est,16,A				;* A+=frequency_est
	STH		A,frequency_est					;* update frequency_est
	MVMM	AR6,AR7							;* AR7=Rx_block_start
	LD		phase_error,T
	CALLD_	IIR_resonator					;* returns result in AH
	 MAR	*+AR7(PJ2_coef)
	ADD		frequency_est,16,A				;* A+=frequency_est
	STH		A,frequency_est					;* update frequency_est
	
	;**** differential decoder ****

	ADDM	#1,Rx_symbol_counter
	LD		Phat,A
	MVDK	Rx_data_head,AR7
	LDPP	decoder_ptr,B
	BACCD_	B						 		;* branch to *decoder_ptr
	 MVDK	Rx_data_len,BK
decoder_return:
COMMON_MESI_decoderReturn:

	;**** constellation display ****

 .if IQ_DAC_WRITE=ENABLED
	LD		Qprime,16,A
	OR		Iprime,A
;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
	SFTA	A,(OP_POINT_SHIFT-6),A
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
	CALL_ 	IQ_DAC_write
constellation_endif:
 .endif

 .if $isdefed("IQ_DUMP_WRITE")
	LD		Iprime,A
	CALL_	dump_write
	LD		Qprime,A
	CALL_	dump_write
 .endif

	;**** symbol timing ****

	LDPP	timing_ptr,B
	BACCD_	B						 		;* branch to *timing_ptr
	 LD		Ihat,16,A
	 LD		Qhat,16,B
timing_return:					
COMMON_MESI_timingReturn:

	;**** fine symbol timing adjust ****

check_advance:
	LD		Rx_sym_clk_memory,B
	SUB		timing_threshold,B,A
	BCD_	check_retard,ALEQ				;* branch if Rx_sym_clk_memory <=THR
	 LDU	LO_memory,A						;* A=STL_memory
	 SUB	LO_phase,A						;* A-=STL_phase
	STL		A,LO_memory						;* STL_memory-=STL_phase
	ADDM	#-1,Rx_sym_clk_phase
	LD		Rx_sym_clk_phase,A
	BCD_	end_timing_adj,AGEQ				;* branch if Rx_sym_clk_phase>=0
	 ST		#0,Rx_sym_clk_memory
	LDU		LO_memory,A						;* A=STL_memory
	 ADD	LO_phase,A						;* A+=STL_phase
	STL		A,LO_memory						;* STL_memory+=STL_phase
	LD		Rx_decimate,A
	STL		A,Rx_sym_clk_phase				;* Rx_sym_clk_phase=Rx_decimate
	MVDK	Rx_sample_len,BK
 .if ON_CHIP_COEFFICIENTS=ENABLED
	LD		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_DECR,A,B					;* B=(timing_ptr+coef_ptr+FIR_DECR)
	STLM	B,AR4
	ADD		#COEF_DECR,A					;* B=*(timing_ptr+coef_ptr+COEF_DECR)
	STLM	A,AR5
	 MVDK	*AR4,AR0
	 MVDK	Rx_sample_ptr,AR7		
	MAR		*AR7-0%							;* AR7=(AR7-AR0)%
	MVKD	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LD		*AR5,A
	BD_		end_timing_adj			
	 STL	A,Rx_coef_ptr					;* update Rx_coef_ptr
	 MVMM	AR7,AR2							;* update Rx_sample_tail
 .else
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_DECR,A						;* B=(timing_ptr+coef_ptr+FIR_DECR)
	READA	temp0					 
	MVDK 	temp0,AR0						;* AR0=*(timing_ptr+coef_ptr+FIR_DECR)
	MVDK 	Rx_sample_ptr,AR7		
	MAR		*AR7-0%							;* AR7=(AR7-AR0)%
	MVKD 	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#COEF_DECR,A					;* B=*(timing_ptr+coef_ptr+COEF_DECR)
	BD_		end_timing_adj			
	 READA 	Rx_coef_ptr						;* update Rx_coef_ptr	
	 MVMM	AR7,AR2							;* update Rx_sample_tail
 .endif

check_retard:
	ADD		timing_threshold,B,A
	BCD_	end_timing_adj,AGEQ				;* branch if Rx_sym_clk_memory>=-THR
	 LDU	LO_memory,A						;* A=STL_memory
	 ADD	LO_phase,A						;* A+=STL_phase
	STL		A,LO_memory						;* STL_memory+=STL_phase
	ADDM	#1,Rx_sym_clk_phase
	LD		Rx_sym_clk_phase,A
	SUB		Rx_decimate,A					;* Rx_sym_clk_phase-Rx_decimate
	BCD_	end_timing_adj,ALEQ				;* branch if Rx_sym_clk_phase<=Rx_coef_max
	 ST		#0,Rx_sym_clk_memory
	LDU		LO_memory,A						;* A=STL_memory
	SUB		LO_phase,A						;* A-=STL_phase
	STL		A,LO_memory						;* STL_memory-=STL_phase
	ST		 #0,Rx_sym_clk_phase			;* Rx_sym_clk_phase=0

	MVDK	Rx_sample_len,BK
 .if ON_CHIP_COEFFICIENTS=ENABLED
	LD		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_INCR,A,B					;* B=(timing_ptr+coef_ptr+FIR_INCR)
	STLM	B,AR4
	ADD		#COEF_INCR,A					;* B=*(timing_ptr+coef_ptr+COEF_INCR)
	STLM	A,AR5
	 MVDK	*AR4,AR0
	 MVDK	Rx_sample_ptr,AR7		
	MAR		*AR7+0%							;* AR7=(AR7+AR0)%
	MVKD	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LD		*AR5,A
	STL		A,Rx_coef_ptr					;* update Rx_coef_ptr
 .else
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#FIR_INCR,A						;* B=(timing_ptr+coef_ptr+FIR_INCR)
	READA	temp0					 
	MVDK	temp0,AR0						;* AR0=*(timing_ptr+coef_ptr+FIR_INCR)
	MVDK	Rx_sample_ptr,AR7		
	MAR		*AR7+0%							;* AR7=(AR7+AR0)%
	MVKD	AR7,Rx_sample_ptr				;* update Rx_sample_ptr
	LDU		Rx_timing_start,A
	ADD		Rx_coef_ptr,A					;* A=Rx_timing_start+Rx_coef_ptr
	ADD		#COEF_INCR,A					;* B=*(timing_ptr+coef_ptr+COEF_INCR)
	READA	Rx_coef_ptr						;* update Rx_coef_ptr	
 .endif
end_timing_adj:

	;**** compute mean squared symbol error ****

	LD		Ihat,A
	SUB		Iprime,A						;* A=Ihat-Iprime
	STL		A,IEQprime_error				;* IEQprime_error=Ihat-Iprime
	LD		Qhat,B
	SUB		Qprime,B						;* B=Qhat-Qprime
	STL		B,QEQprime_error				;* QEQprime_error=Qhat-Qprime

;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;;	LD		What,T			
;;	MPY		IEQprime_error,A				;* A=Ierror*What
;;	SFTA	A,(15-6)				
;;	MPYA	IEQprime_error					;* B=Ierror^2*What>>6
;;	LD		What,T			
;;	MPY		QEQprime_error,A				;* A=Qerror*What
;;	SFTA	A,(15-6)				
;;	MACA	QEQprime_error					;* B=Qerror^2*What>>6
;;	SFTA	B,(15-6),A						;* A=(Ierr^2+Qerr^2)*What>>6
;;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	LD		What,T			
;	MPY		IEQprime_error,A				;* A=Ierror*What
;	SFTA	A,(3+OP_POINT_SHIFT)
;	MPYA	IEQprime_error					;* B=Ierror^2*What>>(3+OP_POINT_SHIFT)
;	LD		What,T			
;	MPY		QEQprime_error,A				;* A=Qerror*What
;	SFTA	A,(3+OP_POINT_SHIFT)
;	MACA	QEQprime_error					;* B=Qerror^2*What>>(3+OP_POINT_SHIFT)
;	SFTA	B,(3+OP_POINT_SHIFT),A			;* A=(Ierr^2+Qerr^2)*What>>(3+OP_POINT_SHIFT)
;;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
	LD		What,T			
	MPY		IEQprime_error,A				;* A=Ierror*What
	SFTA	A,(3+OP_POINT_SHIFT)
	MPYA	IEQprime_error					;* B=Ierror^2*What>>(3+OP_POINT_SHIFT)
	LD		What,T			
	MPY		QEQprime_error,A				;* A=Qerror*What
	SFTA	A,(3+OP_POINT_SHIFT)
	MACA	QEQprime_error					;* B=Qerror^2*What>>(3+OP_POINT_SHIFT)
	SFTA	B,(3+OP_POINT_SHIFT),A			;* A=(Ierr^2+Qerr^2)*What>>(3+OP_POINT_SHIFT)
 .if $isdefed("SQUARE_ROOT_WHAT")
	MPYA	What							;* B=What*temp0
	SFTA	B,OP_POINT_SHIFT,A				;* A=temp0<<OP_POINT_SHIFT
 .endif		;* SQUARE_ROOT_WHAT
;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
	SAT		A								;* saturate result
	STM		#MSE_B0,T
	MPYA	A								;* A=MSE_B0*j
	MAC		EQ_MSE,#MSE_A1,A,B				;* B=A+EQ_MSE*MSE_A1
	LD		EQ_2mu,A
	BCD_	end_EQ_update,ALEQ				;* branch if EQ_2mu<=0
	 STH	B,EQ_MSE						;* update EQ_MSE
	 LD		COS,T	

	;**** rotate EQ_error and scale by EQ_2mu ****

	MPY		IEQprime_error,A				;* A=IEQprime_error*cos
	MPY		QEQprime_error,B				;* B=QEQprime_error*cos
	LD		SIN,T	
	MAC		QEQprime_error,A				;* A=IEQprime_error*cos+QEQprime_error*sin
	MAS		IEQprime_error,B				;* B=QEQprime_error*cos-IEQprime_error*sin
	STH		A,temp0							;* temp0=IEQ_error
	STH		B,temp1							;* temp1=QEQ_error
	LD		EQ_2mu,T						;* T=EQ_2mu
	MPY		temp0,A							;* A=IEQ_error*EQ_2mu
;++++#ifndef MESI_INTERNAL 03-07-2001 OP_POINT8 MODS
;	SFTA	A,(EQ_2MU_SCALE-1)				;* scale 
;	STH		A,temp0							;* temp0=IEQ_error*2mu
;	MPY		temp1,B							;* B=QEQ_error*EQ_2mu
;	SFTA	B,(EQ_2MU_SCALE-1)				;* scale 
;	STH		B,temp1							;* temp1=QEQ_error*2mu
;++++#else   MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 
	STH		A,OP_POINT_SHIFT,temp0			;* temp0=IEQ_error*2mu<<OP_POINT_SHIFT
	MPY		temp1,B			   				;* B=QEQ_error*EQ_2mu
	STH		B,OP_POINT_SHIFT,temp1			;* temp1=QEQ_error*2mu<<OP_POINT_SHIFT
;++++#endif  MESI_INTERNAL 03-07-2001 OP_POINT8 MODS 

	;**** LMS-Steepest Descent equalizer update ****

	STM		#RX_FIR_LEN,BK
	MVDK	Rx_fir_ptr,AR4					;* AR4=Rx_fir_ptr
	MVDK	Rx_start_ptrs,AR0			
	MVDK	*AR0(Rx_block_start),AR5
	MAR		*+AR5(temp1)					;* AR5=&temp1 (Qerr)
	MVDK	*AR0(EQ_coef_start),AR6
	MAR		*+AR4(-2)%						;* AR4=Rx_fir_ptr-2%
	MVDK	EQ_taps,BRC						;* for EQ_LEN times ...
	RPTBD	EQ_coef_loop
	STM		#3,AR0
	 LD		*AR6+,16,A						;* A+=coef[4*i](high)
	 LD		*AR6-,16,B						;* B+=coef[4*i+2](high)
	 MAC	*AR5 ,*AR4+,B					;* B=fir[j]*Qerr, T=Qerr
	 MAC	*AR5-,*AR4-,A					;* A=fir[j+1]*Qerr, T=Qerr
	 MACR	*AR5+,*AR4+,A					;* A+=fir[j]*Ierr, T=Ierr
	 MASR	*AR4-0%,B						;* B-=fir[j+1]*Ierr, T=Ierr
	 STH	A,*AR6+							;* coef[4*i]=high
EQ_coef_loop:
	 STH	B,*AR6+							;* coef[4*i+2]=high
end_EQ_update:

	;**** normalized loss of signal (LOS) detector ****

LOS_detector:
;++++#ifndef MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
;;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;;	LD		What,T			
;;	MPY		Iprime,A
;;	SFTA	A,(15-6)						;* A= temp0=(Iprime*What)>>6
;;	MPYA	Iprime							;* B=Qprime*temp0
;;	LD		What,T			
;;	MPY		Qprime,A
;;	SFTA	A,(15-6)						;* A= temp1=(Qprime*What)>>6
;;	MACA	Qprime							;* B=Qprime*temp0+Iprime*temp1
;;	SUB		#LOS_THRESHOLD,13,B				;* B-=THR
;;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	LD		What,T			
;	MPY		Iprime,A
;	SFTA	A,OP_POINT_SHIFT				;* A= temp0=(Iprime*What)<<OP_POINT_SHIFT
;	MPYA	Iprime							;* B=Qprime*temp0
;	LD		What,T			
;	MPY		Qprime,A
;	SFTA	A,OP_POINT_SHIFT				;* A= temp1=(Qprime*What)<<OP_POINT_SHIFT
;	MACA	Qprime							;* B=Qprime*temp0+Iprime*temp1
;	SUB		#LOS_THRESHOLD,OP_POINT_SHIFT,B	;* B-=THR
;;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;++++#else   MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
 .if $isdefed("SQUARE_ROOT_WHAT")
	LD		What,T			
	MPY		Iprime,A
	SFTA	A,OP_POINT_SHIFT				;* A= temp0=(Iprime*What)<<OP_POINT_SHIFT
	MPYA	Iprime							;* B=Qprime*temp0
	LD		What,T			
	MPY		Qprime,A
	SFTA	A,OP_POINT_SHIFT				;* A= temp1=(Qprime*What)<<OP_POINT_SHIFT
	MACA	Qprime							;* B=Qprime*temp0+Iprime*temp1
	SFTA	B,OP_POINT_SHIFT,A				;* A=temp0<<OP_POINT_SHIFT
	MPYA	What							;* B=What*temp0
	SUB		#LOS_THRESHOLD,OP_POINT_SHIFT,B	;* B-=THR
 .else		;* SQUARE_ROOT_WHAT
	LD		What,T			
	MPY		Iprime,A
	SFTA	A,OP_POINT_SHIFT				;* A= temp0=(Iprime*What)<<OP_POINT_SHIFT
	MPYA	Iprime							;* B=Qprime*temp0
	LD		What,T			
	MPY		Qprime,A
	SFTA	A,OP_POINT_SHIFT				;* A= temp1=(Qprime*What)<<OP_POINT_SHIFT
	MACA	Qprime							;* B=Qprime*temp0+Iprime*temp1
	SUB		#LOS_THRESHOLD,OP_POINT_SHIFT,B	;* B-=THR
 .endif		;* SQUARE_ROOT_WHAT
;++++#endif  MESI_INTERNAL 03-06-2001 SQUARE_ROOT_WHAT MODS
	 ADDM	#1,LOS_counter					;* LOS_counter++
	XC		2,BGEQ
	 ST		#0,LOS_counter					;* if k>=THR, counter=0
	LD		LOS_counter,A
	SUB		#LOS_COUNT,A
	 ST		#LOCKED,LOS_monitor
	RCD_	ALT								;* return if count<LIMIT
	 MVDK	Rx_sample_len,BK
	ST		#UNLOCKED,LOS_monitor			;* LOS_monitor=UNLOCKED
	LD		#RX_LOS_FIELD,A					;* A=mode&LOS_FIELD
	AND		Rx_mode,A
	OR		LOS_monitor,A
	STL		A,LOS_monitor					;* LOS_monitor|=(mode&LOS_FIELD)
	RET_
 .endif

;****************************************************************************
;* fir_site filter initialization and filter code. It copies the transversal 
;* filter code from PS into fir_site in DS. This way the FIRS pmad can 
;* be modified to step thru coefficients with stride!=1. User must change 
;* OVLY to 1 in PMST to use this filter or crash occurs.
;* There are two types of filter structures available at compile time set by
;* the FIR_SITE_INTERRUPTS constant defined in config.inc. If ENABLED, the 
;* filter adds 3 pipeline NOPs so the total fir takes 8*FIR_LEN cycles. If
;* DISABLED, the fir_site filter globally disables all interrupts and executes
;* in 5*FIR_LEN cycles.
;* fir_site structure:
;*	struct 
;*			{
;*			int filter_site[CODE_LEN];
;*			int *filter_site_pmad;
;*			} fir_site;
;* On entry fir expects:
;*	AH= coefficient starting address (pmad)
;*	B= initial filter value (typically 0)
;*	AR0=fir[] coefficient sample stride
;*	AR4=&(fir_tail+1)
;*	AR5=&interpolate value
;*	BK=fir_len
;* Modifies:
;*	A,B,AR4
;* On exit:
;*	 B=filtered sample
;****************************************************************************

 .if ON_CHIP_COEFFICIENTS!=ENABLED
 .if COMMON_MODEM=ENABLED
init_fir_site:					

	;**** copy code to DARAM fir_site[] ****

	STM		#fir_site,AR0
	STM		#fir_site,AR4
	LD		#fir_loop_entry,A
	LDM		AL,A							;* clear upper 16 bits for READA
	STM		#(fir_loop_exit-fir_loop_entry-1),BRC
	LD		#1,B
	RPTB	fir_site_init_loop
	 READA	*AR0+							;* fir_site[*++]=fir filter code
fir_site_init_loop:
	 ADD	B,A								;* A++

	;**** re-initialize RPTB arg ****

	ST		#(fir_site+(fir_loop-fir_loop_entry)),*AR4(fir_loop_rptb-fir_loop_entry+1)
	ST		#(fir_site+(fir_loop_macp-fir_loop_entry+1)),*(fir_site_pmad)
	RET_

	;**** fir filter code ****

 .if FIR_SITE_INTERRUPTS=DISABLED
fir_loop_entry:
	 MVDK	*(fir_site_pmad),AR3			;* AR3=&pmad
	PSHM	IMR
	STM		#0,IMR							;* disable interrupts
	PSHM	ST1
	RSBX	OVM								;* disable overflow for FIRS
	STH		A,*AR3							;* pmad
	ADD		*AR5,16,A
fir_loop_rptb:
	RPTBD	fir_loop
	STH		A,*AR3							;* pmad++
	LD		*AR4+0%,16,A					;* AH=fir[*-%]
fir_loop_macp:
	 FIRS	*AR3,*AR5,fir_coef_site			;* B+=fir[] * coef[], increment coef addr
fir_loop:
	 ST		A,*AR3							;* save coef address
||	 LD		*AR4+0%,A						;* AH=fir[*-%]
	SAT		B								;* saturate result
	POPM	ST1
	POPM	IMR								;* restore interrupts
	RET_
fir_loop_exit:

 .else
fir_loop_entry:
	PSHM	ST1
	RSBX	OVM								;* disable overflow for FIRS
	 MVDK	*(fir_site_pmad),AR3			;* AR3=&pmad
	ST		A,*AR3							;* save coef address
||	LD		*AR4+0%,A						;* AH=fir[*-%]
fir_loop_rptb:
	 RPTB	fir_loop
	 NOP
	 NOP
	 NOP
fir_loop_macp:
	 FIRS	*AR3,*AR5,fir_coef_site			;* B+=fir[] * coef[], increment coef addr
fir_loop:
	 ST		A,*AR3							;* save coef address
||	 LD		*AR4+0%,A						;* AH=fir[*-%]
	SAT		B								;* saturate result
	POPM	ST1
	RET_
fir_loop_exit:
 .endif

	;**** define fir_site[] ****

fir_site		.usect	"FirSite",(fir_loop_exit-fir_loop_entry)
fir_site_pmad	.usect	"FirSite",1

fir_coef_site:
	.word	1
 .endif
 .endif										;* ON_CHIP_COEFFICIENTS endif		

;****************************************************************************
;* no_timing: Just returns.
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
no_timing:
COMMON_MESI_noTiming:
	B_		timing_return
 .endif

;****************************************************************************
;* sgn_timing: Quadrature Phase Shift Keyed symbol timing error estimator.
;* On entry it expects the following setup:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
sgn_timing:
COMMON_MESI_sgnTiming:
	LD		Inm1,T
	MPY		Ihat,B							;* B=Inm1*Ihat
	MAS		Ihat_nm2,B						;* B=Inm1*(Ihat-Ihat_nm2)
	LD		Qnm1,T
	MAC		Qhat,B							;* B+=Qnm1*Qhat
	MAS		Qhat_nm2,B						;* B=Inm1*(Ihat-Ihat_nm2)+Qnm1*(Qhat-Qhat_nm2)
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;	SFTA	B,6
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	SFTA	B,OP_POINT_SHIFT				;* scale up by OP_POINT_SHIFT
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
	BD_		timing_return
	 ADD	Rx_sym_clk_memory,16,B			;* B=+Rx_sym_clk_memory
	 STH	B,Rx_sym_clk_memory				;* Rx_sym_clk_memory+=()<<OP_POINT_SHIFT
 .endif

;****************************************************************************
;* APSK_timing: Symbol timing estimator for APSK constellations. It	   
;* looks for zero crossings and then estimates where the mid-phase signal
;* should be and generates an error signal. It reverts back to sgn_timing
;* in BPSK, QPSK.									  
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AH=Ihat
;*	BH=Qhat
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
APSK_timing:
COMMON_MESI_APSKTiming:
	MPYA	Ihat_nm2						;* A=Ihat*Ihat_nm2
	BCD_	IQPSK_endif,BGEQ				;* jump if Ihat*Ihat_nm2>=0
	  LD	Ihat,A
	  LD	Ihat,B
	ADD		Ihat_nm2,B						;* B=Ihat+Ihat_nm2
	SFTA	B,-1							;* B= k=(Ihat+Ihat_nm2)/2
	SUB		Inm1,B							;* B=k-Inm1
	XC		1,AEQ
	 LD		#0,B							;* if Ihat=0, B=0
	XC		1,AGT
	 NEG	B								;* if Ihat>0, B=(Inm1-k)
	ADD		Rx_sym_clk_memory,B				;* B+=Rx_sym_clk_memory
	STL		B,Rx_sym_clk_memory				;* update Rx_sym_clk_memory
IQPSK_endif:
	LD		Qhat,T
	MPY		Qhat_nm2,B						;* A=Qhat*Qhat_nm2
	BCD_	timing_return,BGEQ				;* jump if Qhat*Qhat_nm2>=0
	  LD	Qhat,A
	  LD	Qhat,B
	ADD		Qhat_nm2,B						;* B=Qhat+Qhat_nm2
	SFTA	B,-1							;* B= k=(Qhat+Qhat_nm2)/2
	SUB		Qnm1,B
	XC		1,AEQ
	 LD		#0,B							;* if Qhat=0, B=0
	XC		1,AGT
	 NEG	B								;* if Qhat>0, B=-(Qnm1-k)
	BD_		timing_return
	 ADD	Rx_sym_clk_memory,B				;* B+=Rx_sym_clk_memory
	 STL	B,Rx_sym_clk_memory				;* update Rx_sym_clk_memory
 .endif

;****************************************************************************
;* Rx_train_loops: Waits for the output of the autocorrelator to settle
;* Expects the following on entry:
;*	 AR7=Rx_data_tail
;* Returns:	
;*	A=0 if no symbol produced
;*	A=-1 if still in progress			
;*	A=2 if autocorrelation is stable 
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
Rx_train_loops:					
COMMON_MESI_RxTrainLoops:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	RCD_	BEQ								;* branch if Rx_data_head==Rx_data_tail
	 LD		#0,A							;* return(0)
	LDU		Rx_data_head,B
	STL		B,Rx_data_tail					;* data_tail=data_head

	;**** check for valid autocorrelator output ****

	CALLD_	Rx_fir_autocorrelator			;* returns BH=autocorrelation
	 ADDM	#1,Rx_sample_counter			;* Rx_sample_counter++
	XC		2,BGEQ							;* if corr>=0 ...
	 ST		#0,Rx_sample_counter			;* ... sample_counter=0
	LD		Rx_sample_counter,B
	SUB		#(REV_CORR_LEN/2),B
	LD		#-1,A
	RC_		BLT								;* return(-1) if counter<LEN
	
	LD		#2,A							
	RETD_									;* return(2)
	 ST		#0,Rx_sample_counter			;* ... sample_counter=0
 .endif

;****************************************************************************
;* Rx_detect_EQ: detects the phase reversal marking the start of equalizer 
;* training sequence. The unrotated (I,Q) symbols are autocorrelated to
;* find the reversal without any symbol timing recovery.			
;* Expects the following on entry:
;*	 AR7=Rx_data_tail
;* Returns:																						
;*	A=0 if no symbol produced
;*	A=-1 if still in progress			
;*	A=2 if reversal is detected
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
Rx_detect_EQ:					
COMMON_MESI_RxDetectEQ:
	CALL_	APSK_demodulator
	LDU		Rx_data_head,B
	SUBS	Rx_data_tail,B
	RCD_	BEQ								;* branch if Rx_data_head==Rx_data_tail
	 LD		#0,A							;* return(0)
	LDU		Rx_data_head,B
	STL		B,Rx_data_tail					;* data_tail=data_head

	;**** autocorrelator reversal detector ****

	CALL_	Rx_fir_autocorrelator
	RCD_	ALT								;* return if autocorrelation<0
	 LD		#-1,A
	 LD		#2,A
	RET_
 .endif

;****************************************************************************
;* agc_gain_estimator: seeds agc_gain with Rx_power estimate.
;****************************************************************************

 .if RX_COMMON_MODEM=ENABLED
agc_gain_estimator:				
COMMON_MESI_agcGainEstimator:
	LD		Rx_power,B
	ST		#AGC_EST_SEED,agc_gain
	ST		#AGC_EST_STEP,temp0				;* temp0= l=AGC_EST_STEP
	ST		#-13,temp1						;* temp1=-13
	STM		#13,BRC	
	RPTB	agc_est_loop
	 LD		temp1,T							;* T=temp1
	 SUB	temp0,TS,B						;* B=sym_power-(l>>i)
	  MPY	agc_gain,#COS_PI_BY_4,A			;* A=agc_gain*COS_PI_BY_4
	 XC		1,BGT
	  STH	A,agc_gain
	 ADDM	#1,temp1
agc_est_loop:
	 LD		Rx_power,B
	RET_
 .endif

;****************************************************************************
;* Rx_fir_autocorrelator: complex autocorrelator searches for phase reversal	 
;* in Rx_fir[]. Returns the autocorrelation which will be a the negative	 
;* of the power until the reversal where it crosses zero and goes positive	
;* On entry it expects the following setup:
;*	DP=&Rx_block
;* On return:
;*	AL=BH=Rx_temp1=autocorrelation
;****************************************************************************

Rx_fir_autocorrelator:
COMMON_MESI_RxFirAutocorrelator:
	STM		#RX_FIR_LEN,BK
	 MVDK	Rx_fir_ptr,AR4
	MVDK	EQ_taps,AR0						;* AR0=EQ_TAPS-1
	MAR		*+AR4(REV_CORR_DELAY-1)%		;* AR4= k=(Rx_fir_ptr-DELAY+1)%LEN
											;* -1 to compensate EQ_taps-1
	MAR		*AR4-0%							;* AR4= k=(k-EQ_taps)%LEN
	MVMM	AR4,AR3				
	MAR		*+AR3(-2*REV_CORR_LEN)%			;* AR3= (j-2*REV_CORR_LEN%RX_FIR_LEN)
	STM		#(2*REV_CORR_LEN-1),BRC		
	LD		#0,B
	RPTBD	autocorr_loop					;* for CORR_LEN times ...
	STM		#-1,AR0
;++++#ifndef MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS)
;autocorr_loop:
;	 MAS	*AR3+0%,*AR4+0%,B				;* B+=Rx_sample[j]*Rx_sample[k]
;++++#else   MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS) OP_POINT _SHIFT MODS
 .if (6-OP_POINT_SHIFT) != 0
autocorr_loop:
	 MAS	*AR3+0%,*AR4+0%,B				;* B+=Rx_sample[j]*Rx_sample[k]
 .else
	 MAS	*AR3+0%,*AR4+0%,B				;* B+=Rx_sample[j]*Rx_sample[k]
autocorr_loop:
	 SFTA	B,-(6-OP_POINT_SHIFT)			;* B>>=(6-OPPOINT)
 .endif 	 
;++++#endif  MESI_INTERNAL 02-26-2001 (OP_POINT _SHIFT MODS) OP_POINT _SHIFT MODS
	ADD		#4000h,1,B						;* round
	STH		B,temp1							;* Rx_temp1=autocorrelation
	LD		B,-16,A							;* AL=autocorrelation
	RET_

;****************************************************************************
;* IIR_resonator: 2nd order digital resonator or sharp bandpass filter.		
;* It implements the following filter equation:								
;*		y(n)=A*x(n)-A*x(n-2)-B*y(n-1)-y(n-2)	  
;* using the IIR filter structure below:  								
;*  	d(n)=A*x-B*d(n-1)-d(n-2)								
;*  	y=d(n)-d(n-2)
;*  	d(n-2)=d(n-1)		  							
;*  	d(n-1)=d(n)			  							
;*  								
;* IIR_resonator memory is organized as:								
;* 																			
;*				|   B/2   |													
;*				| d(n-1)  |													
;*				| d(n-2)  |													
;* 																			
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR7=start of filter coefficient and memory structure
;*	T=input sample
;* On return:
;*	AH=0 if the coefficient is zero
;*	AH=filter output otherwise
;****************************************************************************

IIR_resonator:
	
	;**** return if coefficient is zero (disabled) ****

	CMPM	*AR7,#0							
	RCD_	TC	   
	 XC		1,TC							;* if coef=0 ...
	  LD	#0,A							;* ... return(0)
 
	MPY		#PJ_COEF_A,B					;* B=A*x
	LD		*AR7+,T							;* T=coef/2
	MAS		*AR7,B							;* B=A*x-B/2*coef
	MASR	*AR7+,B							;* B=A*x-B*coef (round)
	SUB		*AR7,16,B						;* B=A0*x-B*d(n-1)-d(n-2)
	SUB		*AR7-,16,B,A					;* A= y(n)=d(n)-d(n-2)
	SAT		B
	RETD_
	 DELAY	*AR7							;* d(n-2)=d(n-1)
	 STH	B,*AR7							;* d(n-1)=d(n)
	
;****************************************************************************
	.end   
