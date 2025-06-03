;****************************************************************************
;* Filename: filter.asm
;* Date: 04-19-00
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: common filter components
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
 .if (RX_DTMF|RX_MF)=ENABLED
	.include	"gendet.inc"
 .endif
	;**** global functions ****

 .if $isdefed("XDAIS_API")
	.global FILTER_MESI_BandpassFilter
	.global FILTER_MESI_BroadbandEstimator
	.global FILTER_MESI_goertzelBank
 .else
	.global bandpass_filter
	.global broadband_estimator
	.global goertzel_bank
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global _VCOEF_MESI_DFTCoef
	.asg	_VCOEF_MESI_DFTCoef, _DFT_coef
 .else
	.global _DFT_coef
 .endif										;* "XDAIS_API endif

	.sect	"vtext"

;****************************************************************************
;* bandpass_filter: Hilbert bandpass filter with magnitude computation.
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	AR3=&Rx_sample[tail]
;*	BRC=filter length-2
;*	BK=Rx_sample_len
;*	AR0=frequency coefficient (theta): (SIN_TABLE_LEN/Fs)*Fc
;*			Fs=sampling rate (8000 Hz)
;*			Fc=filter center frequency
;*			BW=filter 3dB bandwidth
;* On return:
;*	AH=magnitude
;****************************************************************************

bandpass_filter:					
FILTER_MESI_BandpassFilter:
 .if BPF_FAST_MODE=ENABLED
	 STM	_DFT_coef,AR4					;* AR4=&DFT_coef[0]
	 STM	_DFT_coef+SIN_90_DEGREES,AR5	;* AR5=&DFT_coef[0]
	RPTBD	analysis_filter
	LD		#0,A		
	LD		#0,B
	 MAR	*AR3-%							;* *AR3--%
	 MAC	*AR5+0%,*AR3,A					;* A+=sample[]*sin[]
analysis_filter:
	 MAC	*AR4+0%,*AR3,B					;* B+=sample[]*cos[]
 .else
	 STM	_DFT_coef,AR4					;* AR4=&DFT_coef[0]
	 STM	_DFT_coef+SIN_90_DEGREES,AR5	;* AR5=&DFT_coef[0]
	RPTBD	analysis_filter
	LD		#0,A		
	LD		#0,B
	 MAR	*AR3-%							;* *AR3--%
	 STM	#DFT_COEF_LEN,BK
	 NOP
	 MAC	*AR5+0%,*AR3,A					;* A+=sample[]*sin[]
	 MAC	*AR4+0%,*AR3,B					;* B+=sample[]*cos[]
	 MVDK	Rx_sample_len,BK				;* BK=Rx->sample_len
analysis_filter:
	 NOP
 .endif

	 MAR	*AR3-%							;* *AR3--%
	MACR	*AR5,*AR3,A						;* A+=sample[]*sin[](RND)
	MACR	*AR4,*AR3,B						;* B+=sample[]*cos[](RND)

	;**** compute magnitude ****

	ABS		A
	ABS		B
	STH		A,temp0
	MAX		A								;* B=max(x,y)
	 NOP						
	 NOP						
	XC		1,C								;* if A=B=max
	 LD		temp0,16,B						;* if B=max, A=min
	ADD		B,-1,A							;* B=abs(max)+abs(min)/2
	RET_

;****************************************************************************
;* broadband_estimator: Broadband level (envelope) estimator.
;* On entry it expects the following setup:
;*	DP=&Rx_block
;*	T=broadband coefficient => (1/estimator length)*(pi/2)
;*	AR3=&Rx_sample[tail]
;*	BK=Rx_sample_len
;*	BRC=estimator length -2
;* On return:
;*	AH=broadband signal level (envelope) estimate.
;****************************************************************************

broadband_estimator:				
FILTER_MESI_BroadbandEstimator:
	MAR		*AR3-%							;* AR3--%
	RPTBD	broadband_loop
	LD		#0,B
	LD		*AR3-%,16,A						;* A=Rx_sample[*]
	 ABS	A
	 MACA	T,B								;* B+=|sample[l]|*coef
broadband_loop:
	 LD		*AR3-%,16,A						;* A=Rx_sample[*]
	RETD_
	 ABS	A
	 MACAR 	T,B,A							;* A+=|sample[l]|*SNR_est_coef

;****************************************************************************
;* goertzel_bank: bank of Goertzel DFT filters
;* On entry it expects:
;*	DP=&Rx_block
;*	AR2=SP (receive sample pointer)
;*	AR3=GC (Goertzel coefficients pointer)
;*	AR4=GM (Goertzel delay memory pointer)
;*	BK=Rx_sample_len
;* On exit:
;*	not defines
;****************************************************************************

 .if (RX_DTMF|RX_MF)=ENABLED
goertzel_bank:					
FILTER_MESI_goertzelBank				.set	goertzel_bank
	MVDK	Rx_num_filters,BRC				;* BRC=NUM_FILTERS
	RPTB	goertzel_denom_loop		
	 MPY	*AR2,*AR3+,B					;* B=X*scale
	 SUB	*AR4+,16,B						;* B=X*scale-Dnm2
	 MAC	*AR4,*AR3,B						;* B=X*scale-Dnm2+Dnm1*Ck/2
	 MAC	*AR4-,*AR3+,B					;* B=X*scale-Dnm2+Dnm1*Ck
	 ST		T,*AR4+							;* Dnm2=Dnm1
goertzel_denom_loop:
	 STH	B,*AR4+							;* Dnm1=Dn

	LD		Rx_GF_counter,A
	SUB		#1,A
	STL		A,Rx_GF_counter					;* GF_counter--
	RC_		AGT								;* return if GF_counter!=0

	;**** compute amp^2 values with saturation ****

	MPY		*AR4-,*AR3-,A					;* GM--, GC--
	MVDK	Rx_num_filters,BRC				;* BRC=NUM_FILTERS
	RPTB	goertzel_num_loop
	 SQUR	*AR4,B							;* B=Dnm1^2
	 MPY	*AR4,*AR3,A						;* A=Dnm1*Ck/2
	 MAC	*AR4-,*AR3-,A					;* A=Dnm1*Ck
	 SQURA 	*AR4,B							;* B=Dnm1^2+Dnm2^2, T=Dnm2
	 MPYA	A								;* A=Dnm1*Dnm2*Ck
	 SUB	A,B								;* B-=Dnm1*Dnm2*Ck
	 ADD	#1000h,1,B
goertzel_num_loop:
	 ST		B,*AR4-							;* Dnm2=ampsq, GM--
||	 LD		*AR3-,A							;* GC--
	RET_
 .endif
;****************************************************************************
	.end

