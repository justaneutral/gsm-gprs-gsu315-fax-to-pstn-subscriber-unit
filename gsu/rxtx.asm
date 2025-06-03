;****************************************************************************
;* Filename: rxtx.asm
;* Author: Peter B. Miller
;* Date: 01-29-97
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: transmitter, receiver C callable functions.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"filter.inc"

	;**** transmitter ****

 .if $isdefed("XDAIS_API")
	.global RXTX_MESI_getTxBlockStart
	.global _RXTX_MESI_TxBlockInit
	.global RXTX_MESI_TxBlockInit
	.global _RXTX_MESI_transmitter
	.global RXTX_MESI_transmitter
	.global _RXTX_MESI_TxInitSilence
	.global RXTX_MESI_TxInitSilence
	.global RXTX_MESI_TxSyncSampleBuffers
	.global RXTX_MESI_TxSilenceState
	.global RXTX_MESI_TxStateReturn
 .else
	.global get_Tx_block_start
	.global _Tx_block_init
	.global Tx_block_init
	.global _transmitter
	.global transmitter
	.global _Tx_init_silence
	.global Tx_init_silence
	.global Tx_sync_sample_buffers
	.global Tx_silence_state
	.global Tx_state_return
 .endif										;* "XDAIS_API endif

	;**** receiver ****

 .if $isdefed("XDAIS_API")
	.global RXTX_MESI_getRxBlockStart
	.global _RXTX_MESI_RxBlockInit
	.global RXTX_MESI_RxBlockInit
	.global _RXTX_MESI_receiver
	.global RXTX_MESI_receiver
	.global _RXTX_MESI_RxInitIdle
	.global RXTX_MESI_RxInitIdle
	.global RXTX_MESI_RxIdleItate
	.global RXTX_MESI_RxStateReturn
 .else
	.global get_Rx_block_start
	.global _Rx_block_init
	.global Rx_block_init
	.global _receiver
	.global receiver
	.global _Rx_init_idle
	.global Rx_init_idle
	.global Rx_idle_state
	.global Rx_state_return
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
	.global init_fir_site
 .endif										;* "XDAIS_API endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;*
;* void transmitter(struct START_PTRS *)
;* void Tx_block_init(struct START_PTRS *)			
;* void Tx_init_silence(struct START_PTRS *)
;* void receiver(struct START_PTRS *)
;* void Rx_block_init(struct START_PTRS *)			
;* void Rx_init_idle(struct START_PTRS *)		
;*
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TRANSMITTER=ENABLED
;****************************************************************************
;* get_Tx_block_start: returns start_ptrs->Tx_block_start in AR0.		  
;* On entry it expects start_ptrs in AL			   
;****************************************************************************

get_Tx_block_start:				
RXTX_MESI_getTxBlockStart:
	STLM	A,AR0					 		;* AR0=start_ptrs (arg0)
	 NOP									;* pipe
	 NOP									;* pipe
	MVDK	*AR0(Tx_block_start),AR0		;* AR0=Tx_block_start
	RET_

;****************************************************************************
;* _Tx_block_init: 
;* C function call: void Tx_block_init(struct START_PTRS *start_ptrs)
;* Initializes Tx_block[]
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_block_init:					
_RXTX_MESI_TxBlockInit:
	PSHM	ST0
	PSHM	ST1
	STLM	A,AR0					 		;* AR0=start_ptrs
	 RSBX	CPL								;* reset compiler mode
	CALL_	Tx_block_init
	POPM	ST1
	POPM	ST0
	RETC_
 .endif

;****************************************************************************
;* Tx_block_init: initializes Tx_block[]
;* On entry it expects:
;*	AR0=&start_ptrs[0]
;* Modifies:
;*	A,B,AR0,AR2,BRC,DP
;****************************************************************************

Tx_block_init:					
RXTX_MESI_TxBlockInit:
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP						 	;* DP=&Tx_block
	MVKD	AR0,Tx_start_ptrs		 		;* Tx_block[start_ptrs]=&start_ptrs
	CALL_	Tx_init_silence
	LD		#0,A
	ST		#TX_NUM_SAMPLES,Tx_num_samples	;* Tx_num_samples=TX_NUM_SAMPLES
	ST		#TX_MINUS_16DBM0,Tx_scale	 	;* Tx_scale=TX_MINUS_16DBM0
	LD		*AR0(Tx_sample_start),B
	STL		B,Tx_sample_head			 	;* Tx_sample_head=&Tx_sample[0]
	STL		B,Tx_sample_tail			 	;* Tx_sample_tail=&Tx_sample[0]
	ST		#TX_SAMPLE_LEN,Tx_sample_len	;* Tx_sample_len=TX_SAMPLE_LEN
	STL		A,Tx_call_counter				;* Tx_call_counter=0
	LD		*AR0(Tx_data_start),B
	STL		B,Tx_data_head			 		;* Tx_data_head=&Tx_data[0]
	STL		B,Tx_data_tail			 		;* Tx_data_tail=&Tx_data[0]
	ST		#TX_DATA_LEN,Tx_data_len		;* Tx_data_len=TX_DATA_LEN
	STL		A,Tx_sample_counter				;* Tx_sample_counter=0
	STL		A,Tx_symbol_counter				;* Tx_symbol_counter=0
	STL		A,Tx_rate						;* Tx_rate=0
	STL		A,Tx_mode						;* Tx_mode=0
	STL		A,Tx_system_delay				;* Tx_system_delay=0
	STL		A,Tx_terminal_count				;* Tx_terminal_count=0

	;**** initialize Tx_fir filter in DARAM ****

 .if ON_CHIP_COEFFICIENTS!=ENABLED
 .if COMMON_MODEM=ENABLED
	CALL_	init_fir_site
 .endif 
 .endif									 	;* ON_CHIP_COEFFICIENTS endif		
	RET_ 
 
;****************************************************************************
;* Tx_sync_sample_buffers: synchronizes Tx_sample_head with Rx_sample_tail
;* for echo canceller modems. It should be called from transmitter 
;* functions. It disables interrupts globally for 2 cycles, and then 
;* re-enables them.
;* On entry it expects:
;*	AR3=&start_ptrs
;* Modifies:
;*	 AR0,AR4,AR5, BK
;* On exit:
;*	BK=Rx_sample_len
;*	AR3=&start_ptrs
;*	AR4=&Rx_block
;****************************************************************************

Tx_sync_sample_buffers:			
RXTX_MESI_TxSyncSampleBuffers:
	MVDK	*AR3(Tx_block_start),AR5		;* AR5=Rx_block_start
	MVDK	*AR3(Rx_block_start),AR4		;* AR4=Rx_block_start

	;**** temporarily disable interrupts to get the pointers ****

	PSHM	IMR
	STM		#0,IMR					 		;* disable interrupts
	LDU		*AR5(Tx_sample_tail),B		
	LDU		*AR4(Rx_sample_head),A
	POPM	IMR						 		;* restore interrupts

	SUBS	*AR5(Tx_sample_head),B		
	NEG		B								;* B=Tx_sample_head-Tx_sample_tail
	 MVDK	*AR4(Rx_sample_len),BK	
	XC		2,BLT						 	;* if head<tail ...
	 ADD	*AR5(Tx_sample_len),B	 		;* ... k+=Tx_sample_len	
	SUB		*AR5(Tx_num_samples),1,B	 	;* B-=2*Tx_num_samples
	ADD		*AR5(Tx_call_counter),B		
	STLM	A,AR5
	 SUB	#1,B						 	;* B+=Tx_call_counter-1
	STLM	B,AR0
	 NOP
	 NOP
	MAR		*AR5+0%							;* AR5=(head+k)%len
	MVKD	AR5,*AR4(Rx_sample_tail)
	RET_

;****************************************************************************
;* _transmitter:
;* C function call: void transmitter(struct START_PTRS *)
;* Function call for generation of ALL transmit samples. It pushes the C
;* environment onto the stack exactly as a C function call would. 
;* transmitter calls the function pointed to by state and decrements
;* call_counter if head is num_samples or less ahead of tail.		
;* Transmitter always generates num_samples samples into Tx_sample[].
;* After num_samples calls, transmitter() returns 1 to caller.		
;* All transmitter states expect the following on entry:
;*	DP=&Tx_block
;*	AR2=Tx_sample_head (write pointer)
;*	BK=Tx_sample_len
;* Returns:	
;*	A=0 if |head-tail|>num_samples => no calls to *(state)
;*	A=1 if |head-tail|<=num_samples => Tx_samples[] were generated
;****************************************************************************

 .if COMPILER=ENABLED
_transmitter:					
_RXTX_MESI_transmitter:
	PSHM	ST0
	PSHM	ST1
	PSHM	PMST
 .if OVERLAY_MODE=ENABLED
	ORM		#OVLY,*(PMST)					;* enable PMST overlay mode
 .endif
	PSHM	AR1
	PSHM	AR6
	PSHM	AR7
	STLM	A,AR0					 		;* AR0=start_ptrs
	 RSBX	CPL								;* reset compiler mode
	 SSBX	SXM								;* set sign extension mode
	RSBX	OVM						 		;* reset overflow mode
	SSBX	FRCT							;* set fractional MPY mode
	LD		#0,ASM							;* ASM=0
 .endif
transmitter:					
RXTX_MESI_transmitter:
	LD		*AR0(Tx_block_start),9,A		;* A=Tx_block_start
	LD		#0,DP						 	;* DP=&AH (in page 0)
	LD		AH,DP						 	;* DP=&Tx_block
	 MVKD	AR0,Tx_start_ptrs				;* Tx_block[start_ptrs]=&start_ptrs

 .if TX_SAMPLE_TEST=ENABLED
	LDU		Tx_sample_head,B
	SUBS	Tx_sample_tail,B
	 LD		#0,A
	 NOP
	XC		1,BLT						 	;* if head<tail ...
	 ADD	Tx_sample_len,B			 		;* ... k+=Tx_sample_len
	SUB		Tx_num_samples,B
	BCD_	transmitter_exit,BGT		 	;* return(0) if head-tail>num_samples
 .endif
	MVDK	Tx_sample_head,AR2				;* AR2 is Tx_sample_head
	LD		Tx_num_samples,A				;* A=TX_NUM_SAMPLES
	STL		A,Tx_call_counter				;* initialize Tx_call_counter

Tx_state_loop:
	LDPP	Tx_state,A						;* A=Tx_state branch vector
	BACCD_	A								;* branch to Tx_state (delayed)
	 MVDK	Tx_sample_len,BK		 		;* BK=Tx->sample_len
Tx_state_return:
RXTX_MESI_TxStateReturn:
	LD		Tx_call_counter,A			 	;* A=Tx_call_counter
	SUB		#1,A
	STL		A,Tx_call_counter
	BCD_	Tx_state_loop, AGT
	 MVKD	AR2,Tx_sample_head		 		;* update Tx_sample_head

	LD		#1,A							;* return(1)
 .if COMPILER=ENABLED
transmitter_exit:
	POPM	AR7
	POPM	AR6
	POPM	AR1
	POPM	PMST
	POPM	ST1						 		;* save ST1
	POPM	ST0						 		;* save ST0
 .endif
	RETC_

;****************************************************************************
;* _Tx_init_silence: 
;* C function call void Tx_init_silence(struct START_PTRS *)
;* Switches transmitter state to Tx_silence.
;****************************************************************************
	
 .if COMPILER=ENABLED
_Tx_init_silence:				
_RXTX_MESI_TxInitSilence:
	STLM	A,AR0					 		;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP						 	;* DP=&Tx_block
	CALL_	Tx_init_silence
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Tx_init_silence: initializes Tx_block[] for Tx_silence.
;* On entry it expects:
;*	DP=&Tx_block
;****************************************************************************

Tx_init_silence:					
RXTX_MESI_TxInitSilence:
	ST		#0,Tx_sample_counter
	STPP	#Tx_silence_state,Tx_state,B
	RETD_
	 ST		#TX_SILENCE_ID,Tx_state_ID			

;****************************************************************************
;* Tx_silence_state(): writes a zero to *Tx_sample_head, circularly 		
;* increments *Tx_sample_head and increments Tx_sample_counter.				
;* If Tx_sample_counterexceeds Tx_terminal_count, then it is reset to		
;* zero and Tx_symbol_counter is incremented. Therefore, 					
;* Tx_symbol_counter counts intervals of Tx_terminal_count (default		
;* is 8000 samples or 1.0 sec.).											
;****************************************************************************

Tx_silence_state:
RXTX_MESI_TxSilenceState:
	ADDM	#1,Tx_sample_counter			;* ++Tx_sample_counter
	BD_		Tx_state_return
	 ST		#0,*AR2+%

;****************************************************************************
 .endif

	;******************************
	;**** receive side modules ****
	;******************************

 .if RECEIVER=ENABLED
;****************************************************************************
;* get_Rx_block_start: returns start_ptrs->Rx_block_start in AR0.		  
;* On entry it expects start_ptrs in AL			   
;****************************************************************************

get_Rx_block_start:				
RXTX_MESI_getRxBlockStart:
	STLM	A,AR0					 		;* AR0=start_ptrs (arg0)
	 NOP									;* pipe
	 NOP									;* pipe
	MVDK	*AR0(Rx_block_start),AR0		;* AR0=Rx_block_start
	RET_

;****************************************************************************
;* _Rx_block_init: 
;* C function call void Rx_block_init(struct START_PTRS *);
;****************************************************************************

 .if COMPILER=ENABLED
_Rx_block_init:					
_RXTX_MESI_RxBlockInit:
	PSHM	ST0
	PSHM	ST1
	STLM	A,AR0					 		;* AR0=start_ptrs
	 RSBX	CPL								;* reset compiler mode
	CALL_	Rx_block_init
	POPM	ST1
	POPM	ST0
	RETC_
 .endif

;****************************************************************************
;* Rx_block_init: initializes Rx_block[]
;* On entry it expects:
;*	AR0=&start_ptrs[0]
;* Modifies:
;*	A,B,AR0,AR3,AR4,BRC,DP
;****************************************************************************

Rx_block_init:					
RXTX_MESI_RxBlockInit:
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP						 	;* DP=&Rx_block

	MVKD	AR0,Rx_start_ptrs		 		;* start_ptrs=&Rx_block
	LD		#0,A
	ST		#RX_NUM_SAMPLES,Rx_num_samples	;* Rx_num_samples=RX_NUM_SAMPLES
	ST		#0,Rx_status					;* Rx_status=0
	LD		*AR0(Rx_sample_start),B
	STL		B,Rx_sample_head			 	;* Rx_sample_head=&Rx_sample[0]
	STL		B,Rx_sample_tail			 	;* Rx_sample_tail=&Rx_sample[0]
	STL		B,Rx_sample_stop			 	;* Rx_sample_stop=&Rx_sample[0]
	ST		#RX_SAMPLE_LEN,Rx_sample_len	;* Rx_sample_len=RX_SAMPLE_LEN
	STL		A,Rx_call_counter				;* Tx_call_counter=0
	LD		*AR0(Rx_data_start),B
	STL		B,Rx_data_head			 		;* Rx_data_head=&Rx_data[0]
	STL		B,Rx_data_tail			 		;* Rx_data_tail=&Rx_data[0]
	ST		#RX_DATA_LEN,Rx_data_len		;* Rx_data_len=RX_DATA_LEN
	STL		A,Rx_sample_counter				;* Rx_sample_counter=0
	STL		A,Rx_symbol_counter				;* Rx_symbol_counter=0
	STL		A,Rx_rate						;* Rx_rate=0
	STL		A,Rx_mode						;* Rx_mode=0
	ST		#THR_48DB,Rx_threshold
	STL		A,Rx_detector_mask		 		;* Rx_detector_mask=0
	STL		A,Rx_digit_CP_mask		 		;* Rx_digit_CP_mask=0
	CALL_	Rx_init_idle

	;**** initialize DFT_coef[] with scaled sinusoid ****

 .if $isdefed("XDAIS_API")
	STM		_VCOEF_MESI_DFTCoef,AR4			;* AR4=&DFT_coef[0]
 .else
	STM		_DFT_coef,AR4					;* AR4=&DFT_coef[0]
 .endif										;* "XDAIS_API endif
	PSHM	ST1
	SSBX	FRCT							;* set fractional MPY mode
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STM		#_sin_table,AR3				 	;* A=&sin[0]
	STM		#DFT_COEF,T	
	STM		#(DFT_COEF_LEN-1),BRC			;* for DFT_COEF_LEN-1 times ...
	RPTBD	init_DFT_loop
	 LD		#0,ASM
	 MPY 	*AR3+,B			 				;* B=sin[*] * DFT_COEF
init_DFT_loop:
	 ST		B,*AR4+					 		;* DFT_coef[*+]=sin[*] * DFT_COEF
||	 MPY 	*AR3+,B			 				;* B=sin[*] * DFT_COEF
 .else
	LD		#_sin_table,A				 	;* A=&sin[0]
	LDM		AL,A						 	;* clear upper 16 bits for READA
	STM		#DFT_COEF,T	
	STM		#(DFT_COEF_LEN-1),BRC			;* for DFT_COEF_LEN-1 times ...
	RPTB	init_DFT_loop
	 READA 	*AR4						 	;* DFT_coef[i]=sin[i]
	 ADD	#1,A							;* increment PS address
	 MPY 	*AR4,B			 				;* B=sin[*] * DFT_COEF
init_DFT_loop:
	 STH	B,*AR4+					 		;* DFT_coef[*+]=sin[*] * DFT_COEF
 .endif

	;**** initialize Rx_fir filter in DARAM ****

 .if ON_CHIP_COEFFICIENTS!=ENABLED
 .if COMMON_MODEM=ENABLED
	CALL_	init_fir_site
 .endif 
 .endif									 	;* ON_CHIP_COEFFICIENTS endif		
	POPM	ST1						 		;* restore ST1
	RET_ 

;***************************************************************************
;* _receiver
;* C function call: void receiver(struct START_PTRS *)
;* Function call for ALL detectors and demodulators. It pushes the C
;* environment onto the stack exactly as a C function call would. 
;* Receiver calls the function pointed to by state and decrements	 
;* call_counter if there are num_samples or more samples in Rx_sample[].
;* Sample_stop is set to sample_head because sample_head may continue	 
;* to advance during calls. After num_samples calls, receiver() returns
;* 1 to caller. Receiver functions always try to read all samples from
;* Rx_sample[], so more than num_samples samples may be processed in a
;* call to receiver().									 
;* All receiver states expect the following on entry:
;*	DP=&Rx_block
;*	AR2=Rx_sample_tail (read pointer)
;*	BK=Rx_sample_len
;* Returns:																					*/
;*	A=0 if |head-tail|<num_samples => no calls to *(state)	
;*	A=1 if |head-tail|>=num_samples	=> Rx_samples[] were processed
;****************************************************************************

 .if COMPILER=ENABLED
_receiver:						
_RXTX_MESI_receiver:
	PSHM	ST0
	PSHM	ST1
	PSHM	PMST
 .if OVERLAY_MODE=ENABLED
	ORM		#OVLY,*(PMST)					;* enable PMST overlay mode
 .endif
	PSHM	AR1
	PSHM	AR6
	PSHM	AR7
	STLM	A,AR0					 		;* AR0=start_ptrs
	 RSBX	CPL								;* reset compiler mode
	 SSBX	OVM								;* set overflow mode
	SSBX	SXM						 		;* set sign extension mode
	SSBX	FRCT							;* set fractional MPY mode
	LD			#0,ASM						;* ASM=0
 .endif
									
receiver:						
RXTX_MESI_receiver:
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP
	 MVKD	AR0,Rx_start_ptrs				;* Rx_block[start_ptrs]=&start_ptrs

 .if RX_SAMPLE_TEST=ENABLED
	LDU		Rx_sample_head,B
	SUBS	Rx_sample_tail,B
	 LD		#0,A
	 NOP
	XC		1,BLT						 	;* if head<tail ...
	 ADD	Rx_sample_len,B			 		;* ... k+=Rx_sample_len
	SUB		Rx_num_samples,B
	BCD_	receiver_exit,BLT				;* return(0) if head-tail<num_samples
 .endif
	 MVDK	Rx_sample_tail,AR2		 		;* AR2 is Rx_sample_tail
	LD		Rx_sample_head,A 
	STL		A,Rx_sample_stop			 	;* Rx_sample_stop=Rx_sample_head
	LD		Rx_num_samples,A				;* A=RX_NUM_SAMPLES
	STL		A,Rx_call_counter				;* initialize Rx_call_counter

Rx_state_loop:
	LDPP	Rx_state,A						;* A=Rx_state branch vector
	BACCD_	A								;* branch to Rx_state (delayed)
	 MVDK	Rx_sample_len,BK		 		;* BK=Rx->sample_len
Rx_state_return:
RXTX_MESI_RxStateReturn:
	LD		Rx_call_counter,A			 	;* A=Rx_call_counter
	SUB		#1,A
	STL		A,Rx_call_counter
	BCD_		Rx_state_loop, AGT
	 MVKD	AR2,Rx_sample_tail		 		;* update Rx_sample_tail

	LD		#1,A							;* return(1)
 .if COMPILER=ENABLED
receiver_exit:
	POPM	AR7
	POPM	AR6
	POPM	AR1
	POPM	PMST
	POPM	ST1						 		;* save ST1
	POPM	ST0						 		;* save ST0
 .endif
	RETC_

;****************************************************************************
;* _Rx_init_idle
;* C function call: void Rx_init_idle(struct START_PTRS *)
;* switches Rx_state to Rx_idle.
;****************************************************************************

 .if COMPILER=ENABLED
_Rx_init_idle:					
_RXTX_MESI_RxInitIdle:
	STLM	A,AR0					 		;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP						 	;* DP=&Tx_block
	CALL_	Rx_init_idle
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_idle: initializes rx_block[] for Rx_idle.
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

Rx_init_idle:					
RXTX_MESI_RxInitIdle:
	ST		#0,Rx_sample_counter		
	STPP	#Rx_idle_state,Rx_state,B
	RETD_
	 ST		#RX_IDLE_ID,Rx_state_ID			

;****************************************************************************
;* Rx_idle: reads sample from buffer and returns.
;****************************************************************************

Rx_idle_state:
RXTX_MESI_RxIdleState:
	MVDK	Rx_sample_stop,AR0
	CMPR	0,AR2					 		;* sample_tail-sample_stop
	BC_		Rx_state_return,TC				;* return if tail=stop	
	LD		*AR2+%,A						;* A=Rx_sample[*++]
	BD_		Rx_idle_state	
	 ADDM	#1,Rx_sample_counter			;* ++Rx_sample_counter

;****************************************************************************
 .endif

	.end
