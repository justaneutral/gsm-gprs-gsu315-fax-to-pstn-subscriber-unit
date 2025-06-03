;****************************************************************************	
;* Filename: echo.asm
;* Date: 09-10-99
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: echo canceller initialization and implementation.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"echo.inc"

	;**** global functions ****

 .if $isdefed("XDAIS_API")
	.global ECHO_MESI_echoCanceller
	.global ECHO_MESI_enableEchoCanceller
 .else
	.global echo_canceller
	.global enable_echo_canceller
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global RXTX_MESI_TxSyncSampleBuffers
	.asg	RXTX_MESI_TxSyncSampleBuffers, Tx_sync_sample_buffers
 .else
	.global Tx_sync_sample_buffers
 .endif										;* "XDAIS_API endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

	.sect	"vtext"

;****************************************************************************
;* echo_canceller: Near-end echo canceller. 
;* It operates at the sample rate on samples in Tx_sample[] and cancels the
;* samples in Rx_sample pointed to by Rx_sample_ptr. The coefficients are 
;* stored in EC_coef[].
;* On entry it expects:
;*	DP=&Rx_block
;* On return:
;*	AR1=Tx_block_start
;*	BK=Rx_sample_len
;* Modifies:
;*	A,B,T,AR0,AR1,AR3,AR4,AR5,AR6,AR6,BK,BRC
;****************************************************************************

 .if ECHO_CANCELLER=ENABLED
echo_canceller:
ECHO_MESI_echoCanceller:
	MVDK	EC_sample_ptr,AR7				;* AR7=EC_sample_ptr
	MVDK	Rx_start_ptrs,AR6				;* AR6=Rx_start_ptrs
	MVDK	*AR6(Tx_block_start),AR1		;* AR1=&Tx_block_start
 .if FEC_COEF_LEN!=0
	MPY		Rx_RTD,#FEC_RTD_SCALE,B
	SFTA	B,(2-16)						;* B=RTD*4*FEC_RTD_SCALE
	SUB		#FEC_PEAK_OFFSET,B
	STL		B,temp1							;* temp1=far_dly
 .endif

EC_while_loop:
	LDM		AR7,A
	SUBS	Rx_sample_stop,A				;* sample_stop-sample_tail
	RCD_	AEQ								;* return if sample_tail=sample_stop
	 MVDK	Rx_sample_len,BK

	;**** near-end echo canceller filter ****

	MVDK	*AR1(Tx_sample_len),BK
	MVDK	EC_fir_ptr,AR3					;* AR3=EC_fir_ptr
	MAR		*AR3+%
	MVKD	AR3,EC_fir_ptr					;* EC_fir_ptr++%
	MAR		*AR3-%							;* dummy to modify AR3
	 MVDK	*AR6(EC_coef_start),AR5
	MVDK	EC_taps,BRC
	LD		*AR7,16,A						;* A=Rx_sample[*]<<16
	RPTBD	NEC_fir_loop
	 STM	#-1,AR0
NEC_fir_loop:
	  MAS	*AR3+0%,*AR5+,A					;* A-=Tx_sample[]*EC_coef[]

	;**** far-end echo canceller filter ****
 
 .if FEC_COEF_LEN!=0
	LD		temp1,B							;* B=far_dly
	BC_		FEC_endif1,BLEQ					;* branch if far_dly<=0
	STLM	B,AR0
	MVDK	EC_fir_ptr,AR3			
	 MVDK	EC_taps,BRC
	MAR		*AR3-0%							;* AR3=(EC_fir_ptr-far_dly)%len
	RPTBD	FEC_fir_loop
	 STM	#-1,AR0
FEC_fir_loop:
	  MAS	*AR3+0%,*AR5+,A					;* A-=Tx_sample[]*EC_coef[]
FEC_endif1:
 .endif

	;**** save cancelled sample to Rx_sample[] ****

	MVDK	Rx_sample_len,BK
	 LD		EC_2mu,B
	 SUB	#EC_TRAIN_DISABLED,B
	STH		A,*AR7+%						;* Rx_sample[++%]=error
	BCD_		EC_while_loop,BEQ				;* branch if EC_2mu==EC_TRAIN_DISABLED
	 MVKD	AR7,EC_sample_ptr				;* update EC_sample_ptr

	;**** EC MSE monitor ****

	MVDK	EC_shift,T
	MVDK	*AR1(Tx_sample_len),BK
	 SFTA	A,-2							;* fraction shift + NORM fix
	NORM	A,A								;* AL= error=temp0>>EC_shift
	STL		A,temp0							;* temp0=error	
	SQUR	temp0,B							;* B=error^2
	SFTA	B,-6							;* scale 
	STLM	B,T								;* T=error^2
	MPY		#MSE_B0,B						;* B=MSE_B0*error^2
	MAC		EC_MSE,#MSE_A1,B				;* B+=EC_MSE*MSE_A1
	STH		B,EC_MSE						;* update EC_MSE

	;**** near-end tap update ****

	LD		EC_2mu,T
	MPYR	temp0,B							;* B=error*EC_2mu
	STH		B,temp0
	LD		temp0,T							;* T=error*EC_2mu	
	MVDK	*AR6(EC_coef_start),AR4			;* AR4=&EC_coef
	MVDK	*AR6(EC_coef_start),AR5			;* AR5=&EC_coef
	MVDK	EC_fir_ptr,AR3					;* AR3=EC_fir_ptr
	MVDK	EC_taps,BRC
	MAR		*+AR5(1000h)					;* put AR5 in the next DARAM block
	MAR		*AR3+0%					
	STM		#-1,AR0
	RPTBD	NEC_coef_loop			
	 MPY	*AR3+0%,A						;* A=Tx_sample[]*error
	 LMS	*AR4,*AR5						;* A+=EC_coef[*] (RND), dummy AR5
	  ST	A,*AR4+							;* EC_coef[*++]+=Tx_sample*error
||	  MPY	*AR3+0%,A						;* A=Tx_sample[]*error
NEC_coef_loop:
	 LMS	*AR4,*AR5						;* A+=EC_coef[*] (RND), dummy AR5

	;**** far-end tap update ****

 .if FEC_COEF_LEN!=0
	LD		temp1,B							;* B=far_dly
	BC_		EC_while_loop,BLEQ				;* branch if far_dly<=0
	STLM	B,AR0
	MVDK	EC_fir_ptr,AR3			
	 MVDK EC_taps,BRC
	MAR		*AR3-0%							;* AR3=(EC_fir_ptr-far_dly)%len
	STM		#-1,AR0
	RPTBD	FEC_coef_loop			
	 MPY	*AR3+0%,A						;* A=Tx_sample[]*error, dummy AR5
	 LMS	*AR4,*AR5						;* A+=EC_coef[*] (RND)
	  ST	A,*AR4+							;* EC_coef[*++]+=Tx_sample*error
||	  MPY	*AR3+0%,A						;* A=Tx_sample[]*error
FEC_coef_loop:
	 LMS	*AR4,*AR5						;* A+=EC_coef[*] (RND), dummy AR5
 .endif
	B_		EC_while_loop					;* branch if EC_2mu==EC_TRAIN_DISABLED
 .endif

;****************************************************************************
;* enable_echo_canceller: 
;* Initializes EC_2mu, EC_shift, EC_fir_ptr, and EC_sample_ptr so that echo
;* canceller training can commence.
;* On entry it expects:
;*	A=EC_fir_ptr offset
;*	AR3=&start_ptrs
;* Modifies:
;*	A,B,T,AR0,AR3,AR4,AR5,BK
;****************************************************************************

 .if ECHO_CANCELLER=ENABLED
enable_echo_canceller:
ECHO_MESI_enableEchoCanceller:
	MVDK	*AR3(Tx_block_start),AR5
	MVDK	*AR3(Rx_block_start),AR4		;* AR4=Rx_block_start
	SUB		*AR5(Tx_call_counter),A			;* A=offset+TxN-call_counter	
	ADD		*AR5(Tx_num_samples),A		
	 ADD	*AR4(Rx_num_samples),A
	 ADD	*AR5(Tx_system_delay),A
	 SUB	#NEC_PEAK_OFFSET,A
	STLM	A,AR0					
	 ST		#ACQ_EC_2MU,*AR4(EC_2mu)		;* EC_2mu=ACQ_EC_2MU
	 LD		*AR5(Tx_scale),16,A
	EXP		A								;* T=exponent
	MVDK	*AR5(Tx_sample_len),BK	
	MVDK	*AR5(Tx_sample_head),AR5
	MAR		*AR5-0%							;* Tx_sample_head-=(AR0)%
	MVKD	AR5,*AR4(EC_fir_ptr)
	
	;**** synchronize sample[] buffer pointers ****

	CALL_	Tx_sync_sample_buffers			;* AR4, T are preserved

	;**** derrive exponent for EC_scale ****

	 LD		*AR4(Rx_sample_tail),A
	 STL	A,*AR4(EC_sample_ptr)			;* EC_sample_ptr=Rx_sample_tail
	LDM		T,B					
	SUB		#15,B							;* B=exp-15
	STL		B,*AR4(EC_shift)	
	RET_
 .endif

;****************************************************************************

	.end
