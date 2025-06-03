;****************************************************************************
;* Filename: gendet.asm
;* Date: 02-04-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: Initialization, transmitter, and receiver for generator and
;* detectors.
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"
	.include	"gendet.inc"
	.include	"filter.inc"

	;**** transmitter ****
																		
TX_GEN_SCALE				.set	7336	;* 32768*10exp(-13 dB/20)
AA_FREQUENCY				.set	14746   ;* 8.192*1800 Hz	
AC_FREQUENCY1		  		.set	4915	;* 8.192*600 Hz	
AC_FREQUENCY2		  		.set	24576   ;* 8.192*3000 Hz	
CED_FREQUENCY		  		.set	17203   ;* 8.192*2100 Hz	
CNG_FREQUENCY		  		.set	9011	;* 8.192*1100 Hz
F1_FREQUENCY				.set	19661   ;* 8.192*2400 Hz
F2_FREQUENCY				.set	21299   ;* 8.192*2600 Hz
ECSD_FREQUENCY		 		.set	17203   ;* 8.192*2100 Hz
ECSD_REV_PERIOD				.set	3600	;* 8000*0.45 sec

	;**** definitions for Brazilian call progress tones ****

 .if $isdefed("BRAZIL_PSTN")
DIAL_FREQUENCY1				.set	3491	;* 8.192*425 Hz	
DIAL_FREQUENCY2				.set	0		;* 	no second tone 
RINGBACK_FREQUENCY1			.set	3491	;* 8.192*425 Hz	
RINGBACK_FREQUENCY2			.set	0		;* 	no second tone 
RINGBACK_PERIOD				.set	40000	;* 8000*5 sec 
RINGBACK_ON_TIME			.set	8000	;* 8000*1 sec 
REORDER_FREQUENCY1	 		.set	3491	;* 8.192*425 Hz	
REORDER_FREQUENCY2	 		.set	0		;* 	no second tone 
REORDER_PERIOD		 		.set	8000	;* 8000*1.0 sec	
REORDER_ON_TIME				.set	2000	;* 8000*0.25 sec 
BUSY_FREQUENCY1				.set	3491	;* 8.192*425 Hz	
BUSY_FREQUENCY2				.set	0;* 	no second tone 
BUSY_PERIOD					.set	4000	;* 8000*0.5 sec 
BUSY_ON_TIME				.set	2000	;* 8000*0.25 sec	

	;**** definitions for United States call progress tones ****

 .else
NORTH_AMERICA_PSTN			.set	1
DIAL_FREQUENCY1				.set	2867	;* 8.192*350 Hz
DIAL_FREQUENCY2				.set	3604	;* 8.192*440 Hz
RINGBACK_FREQUENCY1			.set	3604	;* 8.192*440 Hz
RINGBACK_FREQUENCY2			.set	3932	;* 8.192*480 Hz
RINGBACK_PERIOD				.set	48000	;* 8000*6 sec
RINGBACK_ON_TIME			.set	16000	;* 8000*2 sec
REORDER_FREQUENCY1	 		.set	3932	;* 8.192*480 Hz
REORDER_FREQUENCY2	 		.set	5079	;* 8.192*620 Hz
REORDER_PERIOD		 		.set	4000	;* 8000*0.5 sec
REORDER_ON_TIME				.set	2000	;* 8000*0.25 sec
BUSY_FREQUENCY1				.set	3932	;* 8.192*480 Hz
BUSY_FREQUENCY2				.set	5079	;* 8.192*620 Hz
BUSY_PERIOD					.set	8000	;* 8000*1 sec
BUSY_ON_TIME				.set	4000	;* 8000*0.5 sec
 .endif

 .if $isdefed("XDAIS_API")
	.global GEN_MESI_TxInitToneGen
	.global _GEN_MESI_TxInitTone
	.global GEN_MESI_TxInitTone
	.global _GEN_MESI_TxInitAA
	.global GEN_MESI_TxInitAA
	.global _GEN_MESI_TxInitCED
	.global GEN_MESI_TxInitCED
	.global _GEN_MESI_TxInitCNG
	.global GEN_MESI_TxInitCNG
	.global _GEN_MESI_TxInitECSD
	.global GEN_MESI_TxInitECSD
	.global _GEN_MESI_TxInitDialtone
	.global GEN_MESI_TxInitDialtone
	.global _GEN_MESI_TxInitRingback
	.global GEN_MESI_TxInitRingback
	.global _GEN_MESI_TxInitReorder
	.global GEN_MESI_TxInitReorder
	.global _GEN_MESI_TxInitBusy
	.global GEN_MESI_TxInitBusy
	.global GEN_MESI_TxToneGenState
	.global GEN_MESI_TxToneGen
	.global GEN_MESI_TxDigitState
 .else
	.global Tx_init_tone_gen
	.global _Tx_init_tone
	.global Tx_init_tone
	.global _Tx_init_AA
	.global Tx_init_AA
	.global _Tx_init_CED
	.global Tx_init_CED
	.global _Tx_init_CNG
	.global Tx_init_CNG
	.global _Tx_init_ECSD
	.global Tx_init_ECSD
	.global _Tx_init_dialtone
	.global Tx_init_dialtone
	.global _Tx_init_ringback
	.global Tx_init_ringback
	.global _Tx_init_reorder
	.global Tx_init_reorder
	.global _Tx_init_busy
	.global Tx_init_busy
	.global Tx_tone_gen_state
	.global Tx_tone_gen
	.global Tx_digit_state
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global RXTX_MESI_TxStateReturn
	.asg	RXTX_MESI_TxStateReturn, Tx_state_return
	.global RXTX_MESI_TxInitSilence
	.asg	RXTX_MESI_TxInitSilence, Tx_init_silence
 .else
	.global Tx_state_return
	.global Tx_init_silence
 .endif										;* "XDAIS_API endif

	;**** receiver ****

RX_SNR_EST_LEN		 		.set	16
RX_SNR_EST_PERIOD	  		.set	16
RX_SNR_THR_COEF				.set	648	 	;* 32768*10exp(-10/20)*(1/SNR_EST_LEN)
RX_SNR_EST_COEF				.set	2434	;* 32678*(1/SNR_EST_LEN) + fudge

RX_ECSD_CORR_LEN			.set	16
RX_ECSD_CORR_DISP	  		.set	38
RX_ECSD_CORR_DELAY	 		.set	16
V32_ECSD_CORR_LEN	  		.set	80
V32_ECSD_CORR_DISP	 		.set	80 
V32_ECSD_CORR_DELAY			.set	1
CP_DUALTONE_SCALE	  		.set	20650
CP_DIALTONE_460_SCALE  		.set	22616   ;* (CP_DUALTONE_SCALE*1.1)
CP_REORDER_600_SCALE		.set	22616   ;* (CP_DUALTONE_SCALE*1.1)
BUSY_DETECT_LEN				.set	30	  	;* (0.3 sec.)*(100 samples/sec.) 
CP_DETECT_COUNT				.set	4
CP_UNDETECT_COUNT	  		.set	4

	;**** filter_mask_low mask definitions ****
				
F350_MASK			  		.set	0001h
F425_MASK			  		.set	0002h
F460_MASK			  		.set	0002h
F500_MASK			  		.set	0004h
F600_MASK			  		.set	0008h
F980_MASK			  		.set	0010h
F1000_MASK			 		.set	0020h
F1100_MASK			 		.set	0040h
F1180_MASK			 		.set	0080h
F1200_MASK			 		.set	0100h
F1650_MASK			 		.set	0200h
F1700_MASK			 		.set	0400h
F1750_MASK			 		.set	0800h
F1800_MASK			 		.set	1000h
F1850_MASK			 		.set	2000h

	;**** filter_mask_high mask definitions ****				

F2100_MASK			 		.set	0001h
F2225_MASK			 		.set	0002h
F2250_MASK			 		.set	0004h
F2400_MASK			 		.set	0008h
F2600_MASK			 		.set	0010h
F2850_MASK			 		.set	0020h
F2900_MASK			 		.set	0040h
F3000_MASK			 		.set	0080h

 .if $isdefed("XDAIS_API")
	.global _DET_MESI_RxInitDetector
	.global DET_MESI_RxInitDetector
	.global DET_MESI_noDigitDetector
	.global _DET_MESI_setRxDetectorMask	
	.global DET_MESI_setRxDetectorMask	
	.global _DET_MESI_setRxDigitCPmask	
	.global DET_MESI_setRxDigitCPmask	
	.global DET_MESI_RxSetFilterMask
 .else
	.global _Rx_init_detector
	.global Rx_init_detector
	.global no_digit_detector
	.global _set_Rx_detector_mask	
	.global set_Rx_detector_mask	
	.global _set_Rx_digit_CP_mask	
	.global set_Rx_digit_CP_mask	
	.global Rx_set_filter_mask
 .endif										;* "XDAIS_API endif

	;**** external global symbols ****

 .if $isdefed("XDAIS_API")
	.global RXTX_MESI_RxStateReturn
	.asg	RXTX_MESI_RxStateReturn, Rx_state_return
	.global COMMON_MESI_agcGainEstimator
	.asg	COMMON_MESI_agcGainEstimator, agc_gain_estimator
	.global FILTER_MESI_BandpassFilter
	.asg	FILTER_MESI_BandpassFilter, bandpass_filter
	.global FILTER_MESI_BroadbandEstimator
	.asg	FILTER_MESI_BroadbandEstimator, broadband_estimator
	.global FILTER_MESI_goertzelBank
	.asg	FILTER_MESI_goertzelBank, goertzel_bank
	.global DTMF_MESI_RxInitDTMF
	.asg	DTMF_MESI_RxInitDTMF, Rx_init_DTMF
	.global MF_MESI_RxInitR1
	.asg	MF_MESI_RxInitR1, Rx_init_R1
	.global MF_MESI_RxInitR2F
	.asg	MF_MESI_RxInitR2F, Rx_init_R2F
	.global MF_MESI_RxInitR2B
	.asg	MF_MESI_RxInitR2B, Rx_init_R2B
	.global V17_MESI_RxInitV17
	.asg	V17_MESI_RxInitV17, Rx_init_v17
	.global V21_MESI_RxInitV21Ch1
	.asg	V21_MESI_RxInitV21Ch1, Rx_init_v21_ch1
	.global V21_MESI_RxInitV21Ch2
	.asg	V21_MESI_RxInitV21Ch2, Rx_init_v21_ch2
	.global V22_MESI_RxInitV22C
	.asg	V22_MESI_RxInitV22C, Rx_init_v22C
	.global V27_MESI_RxInitV27
	.asg	V27_MESI_RxInitV27, Rx_init_v27
	.global V29_MESI_RxInitV29
	.asg	V29_MESI_RxInitV29, Rx_init_v29
	.global _VCOEF_MESI_sinTable
	.asg	_VCOEF_MESI_sinTable, _sin_table
 .else
	.global Rx_state_return
	.global agc_gain_estimator
	.global bandpass_filter
	.global broadband_estimator
	.global goertzel_bank
	.global Rx_init_DTMF
	.global Rx_init_R1
	.global Rx_init_R2F
	.global Rx_init_R2B
	.global Rx_init_v17
	.global Rx_init_v21_ch1
	.global Rx_init_v21_ch2
	.global Rx_init_v22C
	.global Rx_init_v27
	.global Rx_init_v29
	.global _sin_table
 .endif										;* "XDAIS_API endif

 .if DUMP_BUFFER=ENABLED
	.global dump_write
 .endif

	;**** internal functions ****

 .if $isdefed("SHOW_GLOBAL")				;* if -dSHOW_GLOBAL is in makefile
	.global oscillator1
	.global oscillator2
	.global Rx_energy_det
	.global Rx_sig_analysis
	.global v17_detector
	.global v21_ch1_detector
	.global v21_ch2_detector
	.global v22C_detector
	.global v22_USB1_detector
	.global v27_2400_detector
	.global v27_4800_detector
	.global v29_detector
	.global call_progress_detector
	.global dialtone_detector
	.global reorder_detector
	.global ringback_detector
	.global v32_automode_detector
	.global CNG_detector
	.global CED_detector
	.global TEP_1700_detector
	.global TEP_1800_detector
	.global Rx_tone_undetector_state
	.global ECSD_detector
	.global energy_undetector
	.global level_detector
	.global phase_reversal
 .endif		

	.sect		"vtext"

;****************************************************************************
;* Summary of C callable user functions.
;* 
;* void Tx_init_tone_gen(struct START_PTRS *);
;* void Tx_init_tone(int, struct START_PTRS *);
;* void Tx_init_AA(struct START_PTRS *)
;* void Tx_init_CED(struct START_PTRS *)
;* void Tx_init_CNG(struct START_PTRS *)
;* void Tx_init_ECSD(struct START_PTRS *)
;* void Tx_init_dialtone(struct START_PTRS *)
;* void Tx_init_ringback(struct START_PTRS *)
;* void Tx_init_reorder(struct START_PTRS *)
;* void Tx_init_busy(struct START_PTRS *)
;* void Tx_init_busy(struct START_PTRS *);
;* int Tx_digit_state(struct TX_BLOCK *);
;* int Tx_tone_gen(struct TX_BLOCK *);
;* int Tx_tone_gen_state(struct TX_BLOCK *);
;*
;* void Rx_init_detector(struct START_PTRS *)
;* void set_Rx_detector_mask(int, struct START_PTRS *)
;* void set_Rx_digit_CP_mask(int, struct START_PTRS *)
;* 
;****************************************************************************

	;*****************************
	;**** transmitter modules ****
	;*****************************

 .if TRANSMITTER=ENABLED
;****************************************************************************
;* Tx_init_tone_gen:
;* Initializes Tx_block[] for tone generation.
;* On entry it expects:
;*	DP=&Tx_block
;* Modifies:
;*	A
;****************************************************************************
									
Tx_init_tone_gen:				
GEN_MESI_TxInitToneGen:
	LD		#0,A
	STL		A,Tx_frequency1
	STL		A,Tx_frequency2
	STL		A,Tx_vco_memory1			
	ST		#TX_GEN_SCALE,Tx_scale1
	STL		A,Tx_frequency2
	STL		A,Tx_vco_memory2
	ST		#TX_GEN_SCALE,Tx_scale2
	STL		A,Tx_cad_memory
	STL		A,Tx_cad_period
	STL		A,Tx_on_time
	STL		A,Tx_rev_memory
	STL		A,Tx_rev_period
	STL		A,Tx_sample_counter
	ST		#-1,Tx_terminal_count
	STPP	#Tx_tone_gen_state,Tx_state,A
	RETD_
	 ST		#TX_TONE_GEN_ID,Tx_state_ID	

;****************************************************************************
;* _Tx_init_tone: 
;* C function call void Tx_init_tone(int, struct START_PTRS *)
;* Initializes Tx_block[] workspace for tone generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_tone:					
_GEN_MESI_TxInitTone:
	GET_ARG 1,AR0							;* AR0=start_ptrs (arg1)
	LD		A,B								;* B=arg0
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_tone
	POPM	ST1
	RETC_
 .endif

Tx_init_tone:					
GEN_MESI_TxInitTone:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	STL		B,Tx_frequency1
	RET_
 
;****************************************************************************
;* _Tx_init_AA: 
;* C function call void Tx_init_AA(struct START_PTRS *)
;* Initializes Tx_block[] workspace for AA generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_AA:					
_GEN_MESI_TxInitAA:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_AA
	POPM	ST1
	RETC_
 .endif

Tx_init_AA:					
GEN_MESI_TxInitAA:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#AA_FREQUENCY,Tx_frequency1
	RET_

;****************************************************************************
;* _Tx_init_CED: 
;* C function call void Tx_init_CED(struct START_PTRS *)
;* Initializes Tx_block[] workspace for CED generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_CED:					
_GEN_MESI_TxInitCED:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_CED
	POPM	ST1
	RETC_
 .endif

Tx_init_CED:					
GEN_MESI_TxInitCED:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#CED_FREQUENCY,Tx_frequency1
	ST		#TX_CED_ID,Tx_state_ID			
	RET_ 
 
;****************************************************************************
;* _Tx_init_CNG:
;* C function call void Tx_init_CNG(struct START_PTRS *)
;* Initializes Tx_block[] workspace for CNG generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_CNG:					
_GEN_MESI_TxInitCNG:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_CNG
	POPM	ST1
	RETC_
 .endif

Tx_init_CNG:					
GEN_MESI_TxInitCNG:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#CNG_FREQUENCY,Tx_frequency1
	ST		#TX_CNG_ID,Tx_state_ID			
	RET_ 

;****************************************************************************
;* _Tx_init_ECSD:
;* C function call void Tx_init_ECSD(struct START_PTRS *)
;* Initializes Tx_block[] workspace for ECSD generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_ECSD:					
_GEN_MESI_TxInitECSD:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_ECSD
	POPM	ST1
	RETC_
 .endif

Tx_init_ECSD:					
GEN_MESI_TxInitECSD:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#ECSD_FREQUENCY,Tx_frequency1
	ST		#ECSD_REV_PERIOD,Tx_rev_period
	ST		#1,Tx_rev_memory
	ST		#TX_ECSD_ID,Tx_state_ID			;* Tx_state_ID=TX_ECSD_ID
	RET_ 

 .if TX_CALL_PROGRESS=ENABLED
;****************************************************************************
;* _Tx_init_dialtone:
;* C function call void Tx_init_dialtone(struct START_PTRS *)
;* Initializes Tx_block[] workspace for dial tone generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_dialtone:				
_GEN_MESI_TxInitDialtone:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_dialtone
	POPM	ST1
	RETC_
 .endif

Tx_init_dialtone:				
GEN_MESI_TxInitDialtone:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#DIAL_FREQUENCY1,Tx_frequency1
	ST		#DIAL_FREQUENCY2,Tx_frequency2
	ST		#TX_DIALTONE_ID,Tx_state_ID			
	RET_ 

;****************************************************************************
;* _Tx_init_ringback:
;* C function call void Tx_init_ringback(struct START_PTRS *)
;* Initializes Tx_block[] workspace for ringback tone generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_ringback:				
_GEN_MESI_TxInitRingback:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_ringback
	POPM	ST1
	RETC_
 .endif

Tx_init_ringback:				
GEN_MESI_TxInitRingback:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#RINGBACK_FREQUENCY1,Tx_frequency1
	ST		#RINGBACK_FREQUENCY2,Tx_frequency2
	ST		#RINGBACK_PERIOD,Tx_cad_period
	ST		#RINGBACK_ON_TIME,Tx_on_time
	ST		#0,Tx_cad_memory		
	ST		#TX_RINGBACK_ID,Tx_state_ID		
	RET_ 

;****************************************************************************
;* _Tx_init_reorder:
;* C function call void Tx_init_reorder(struct START_PTRS *)
;* Initializes Tx_block[] workspace for reorder tone generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_reorder:				
_GEN_MESI_TxInitReorder:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_reorder
	POPM	ST1
	RETC_
 .endif

Tx_init_reorder:					
GEN_MESI_TxInitReorder:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#REORDER_FREQUENCY1,Tx_frequency1
	ST		#REORDER_FREQUENCY2,Tx_frequency2
	ST		#REORDER_PERIOD,Tx_cad_period
	ST		#REORDER_ON_TIME,Tx_on_time
	ST		#0,Tx_cad_memory		
	ST		#TX_REORDER_ID,Tx_state_ID		
	RET_ 

;****************************************************************************
;* _Tx_init_busy:
;* C function call void Tx_init_busy(struct START_PTRS *)
;* Initializes Tx_block[] workspace for busy tone generation.
;****************************************************************************

 .if COMPILER=ENABLED
_Tx_init_busy:					
_GEN_MESI_TxInitBusy:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Tx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Tx_block
	CALL_	Tx_init_busy
	POPM	ST1
	RETC_
 .endif

Tx_init_busy:					
GEN_MESI_TxInitBusy:
	CALL_	Tx_init_tone_gen				;* initialize Tx_block[] and Tx_state
	ST		#BUSY_FREQUENCY1,Tx_frequency1
	ST		#BUSY_FREQUENCY2,Tx_frequency2
	ST		#BUSY_PERIOD,Tx_cad_period
	ST		#BUSY_ON_TIME,Tx_on_time
	ST		#0,Tx_cad_memory		
	ST		#TX_BUSY_ID,Tx_state_ID		
	RET_ 
 .endif

 .if (TX_DTMF|TX_MF)=ENABLED
;****************************************************************************
;* Tx_digit_state: Generate digits from Tx_data[].
;****************************************************************************

Tx_digit_state:
GEN_MESI_TxDigitState:

	;**** get new digit ****

	LD		Tx_cad_memory,B
	BC_		Tx_cadence_endif,BNEQ			;* branch if cad_memory!=0
	MVDK	Tx_data_len,BK					;* BK=TX_DATA_LEN
	MVDK	Tx_data_tail,AR7				;* AR7=Tx_data_tail
	LD		*AR7+%,1,A						;* A=Tx_data[tail++]<<1
	MVKD	AR7,Tx_data_tail				;* update Tx_data_tial
	MVDK	Tx_sample_len,BK				;* BK=TX_SAMPLE_LEN
	AND		#1eh,A							;* A=(Tx_data[tail++]&0xf)<<1
	ADDS	Tx_digit_ptr,A					;* A=Tx_digit_ptr+F1 (unsigned)
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	 NOP
	 NOP
	LD		*AR0+,A
	STL		A,Tx_frequency1					;* Tx_frequency1=*(Tx_digit_ptr+F1)
	LD		*AR0,A
	STL		A,Tx_frequency2					;* Tx_frequency2=*(Tx_digit_ptr+F1_1)
 .else
	READA	Tx_frequency1					;* Tx_frequency1=*(Tx_digit_ptr+F1)
	ADD		#1,A
	READA	Tx_frequency2					;* Tx_frequency2=*(Tx_digit_ptr+F1_1)
 .endif
Tx_cadence_endif:

	CALL_	Tx_tone_gen
	LDU		Tx_cad_memory,A
	SUBS	Tx_cad_period,A
	BC_		Tx_symbol_endif,ANEQ			;* branch if memory!=period
	ADDM	#1,Tx_symbol_counter			;* Tx_symbol_counter++
Tx_symbol_endif:

	LD		Tx_terminal_count,B
	BCD_		Tx_state_return,BLT				;* return if TC<0
	 LD		Tx_symbol_counter,A
	 SUB	Tx_terminal_count,A
	CC_		Tx_init_silence,AGEQ			;* if counter>=TC, Tx_init_silence
	B_		Tx_state_return
 .endif

;****************************************************************************
;* Tx_tone_gen_state: generates tones.
;* When the counter expires, the state switches automatically to Tx_silence.
;* On entry it expects:
;*	 DP=&Tx_block
;*	AR2=Tx_sample_tail
;*	BK=Tx_sample_len
;****************************************************************************

Tx_tone_gen_state:
GEN_MESI_TxToneGenState:
	CALL_	Tx_tone_gen
	LD		Tx_terminal_count,16,A
	BC_		Tx_state_return,ALT				;* return if TC<0
	SUB		Tx_sample_counter,16,A			;* TC-counter
	BC_		Tx_state_return,AGT				;* return if TC>counter
	CALL_	 Tx_init_silence				;* if counter>=TC, switch to silence
	B_		Tx_state_return

;****************************************************************************
;* Tx_tone_gen: generates dual tones with cadence and phase reversals
;****************************************************************************
					
Tx_tone_gen:					
GEN_MESI_TxToneGen:
	ADDM	#1,Tx_sample_counter			;* Tx_sample_counter++

	;**** cadence ****

	ADDM	#1,Tx_cad_memory				;* Tx_cadence++
	SUB		Tx_cad_period,A					;* compare with cad_period
	LDU		Tx_cad_memory,A					;* A=(unsigned)Tx_cad_memory
	SUBS	Tx_cad_period,A					;* compare with cad_period
	 NOP
	 NOP
	XC		2,AGT							;* branch if memory<=period
	 ST		#0,Tx_cad_memory				;* Tx_cadence=0
	  LDU	Tx_cad_memory,A					;* A=(unsigned)Tx_cad_memory
	SUBS	Tx_on_time,A
	BCD_	tone_gen_end,AGT				;* branch if memory>on_time
	 LD		#0,A							;* A=Tx_sample=0
	 LD		Tx_rev_memory,B

	;**** phase reversals ****

phase_reversal:					
	BCD_	oscillator1,BEQ					;* branch if rev_memory==0
	 SUB	#1,B							;* B--
	BCD_	oscillator1,BNEQ				;* branch if rev_memory>0
	 STL	B,Tx_rev_memory	
	 LD		Tx_rev_period,B
	STL		B,Tx_rev_memory					;* rev_memory=rev_period
	LDU		Tx_vco_memory1,A				;* A=Tx_vco_memory
	ADD		#8000h,A						;* A=vco_mem+0x8000
	STL		A,Tx_vco_memory1				;* Tx_vco_memory+=0x8000

	;**** oscillator 1 ****

oscillator1:					
	LDU		Tx_vco_memory1,A				;* A=Tx_vco_memory
	ADDS	Tx_frequency1,A					;* A=vco_memory+Tx_frequency
	STL		A,Tx_vco_memory1				;* Tx_vco_memory+=Tx_frequency
	LDU		Tx_vco_memory1,A				;* mask off upper 16 bits
	SFTL	A,-SIN_TABLE_SHIFT				;* A=vco_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[vco_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	 NOP
	 LD		Tx_scale1,T						;* T=Tx_scale1
	MPY		*AR0,B							;* B=scale*Tx_sample
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR2							;* put it in Tx_sample[*]
	LD		Tx_scale1,T						;* T=Tx_scale1
	MPY		*AR2,B							;* B=scale*Tx_sample
 .endif

	;**** oscillator 2 ****

oscillator2:					
	LDU		Tx_vco_memory2,A				;* A=Tx_vco_memory
	ADDS	Tx_frequency2,A					;* A=vco_memory+Tx_frequency
	STL		A,Tx_vco_memory2				;* Tx_vco_memory+=Tx_frequency
	LDU		Tx_vco_memory2,A				;* mask off upper 16 bits
	SFTL	A,-SIN_TABLE_SHIFT				;* A=vco_mem>>SIN_TABLE_SHIFT
	ADD		#_sin_table,A					;* A=&sin[vco_mem]
 .if ON_CHIP_COEFFICIENTS=ENABLED
	STLM	A,AR0
	 NOP
	 LD		Tx_scale2,T						;* T=Tx_scale1
	MPY		*AR0,A							;* A=scale*Tx_sample
	ADD		B,A
 .else
	LDM		AL,A							;* clear upper 16 bits for READA
	READA	*AR2							;* put it in Tx_sample[*]
	LD		Tx_scale2,T						;* T=Tx_scale1
	MPY		*AR2,A							;* A=scale*Tx_sample
	ADD		B,A
 .endif
	MPYA	Tx_scale						;* B=Tx_scale*(tone1+tone2)
	LD		B,A

tone_gen_end:
	 STH	A,*AR2+%						;* Tx_sample[*++]=sin
	RET_

;****************************************************************************
 .endif

	;**************************
	;**** receiver modules ****
	;**************************

 .if RECEIVER=ENABLED
;***************************************************************************
;* _Rx_init_detector: 
;* C function call void Rx_init_detector(struct START_PTRS *)
;* Initializes Rx_det[] workspace for energy detection.
;****************************************************************************
										
 .if COMPILER=ENABLED
_Rx_init_detector:				
_DET_MESI_RxInitDetector:
	STLM	A,AR0							;* AR0=start_ptrs
	 PSHM	ST1
	 RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,A
	LD		#0,DP
	LD		AH,DP							;* DP=&Rx_block
	CALL_	Rx_init_detector
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* Rx_init_det:
;* Initializes Rx_det[] workspace for signal detection
;* On entry it expects:
;*	 DP=&Rx_block
;*	AR0=&Rx_block
;****************************************************************************

Rx_init_detector:					
DET_MESI_RxInitDetector:
	ST		#RX_ENERGY_DET_ID,Rx_state_ID
	STPP	#Rx_energy_det,Rx_state,B
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter
	LD		#0,A
	STL		A,Rx_symbol_counter
	STL		A,temp0
	ST		#RX_SNR_EST_COEF,Rx_SNR_est_coef
	ST		#RX_SNR_THR_COEF,Rx_SNR_thr_coef
	 STL	A,Rx_filter_mask_low
	STL		A,Rx_filter_mask_high
	STL		A,Rx_dialtone_counter
	STL		A,Rx_ringback_counter
	STL		A,Rx_reorder_counter
	STPP	#no_digit_detector,Rx_digit_detector,B

	;**** initialize digit DTMF detector if masked on ****

 .if RX_DTMF=ENABLED
	BITF	Rx_digit_CP_mask,DTMF_MASK		;* test DTMF_MASK in digit_CP_mask
	CC_		Rx_init_DTMF,TC					;* call init_DTMF if MASKED ON
 .endif

	;**** initialize digit MF detector if masked on ****

 .if RX_MF=ENABLED
	BITF	Rx_digit_CP_mask,R1_MASK		;* test R1_MASK in digit_CP_mask
	CC_		Rx_init_R1,TC					;* call init_R1 if MASKED ON
	BITF	Rx_digit_CP_mask,R2F_MASK		;* test R2F_MASK in digit_CP_mask
	CC_		Rx_init_R2F,TC					;* call init_R2F if MASKED ON
	BITF	Rx_digit_CP_mask,R2B_MASK		;* test R2B_MASK in digit_CP_mask
	CC_		Rx_init_R2B,TC					;* call init_R2B if MASKED ON
 .endif

	CALL_	Rx_set_filter_mask
	RET_

;****************************************************************************
;* no_digit_detector: stub for no digit detectors - just returns
;****************************************************************************

no_digit_detector:
DET_MESI_noDigitDetector:
	RET_

;****************************************************************************
;* _set_Rx_detector_mask
;* C function call: void set_Rx_detector_mask(int)
;* Copies the first arg on the C stack to Rx_block.detector_mask
;* Checks Rx_state_ID and if it is ENERGY_DET call Rx_set_filter_mask.	
;****************************************************************************

 .if COMPILER=ENABLED
_set_Rx_detector_mask:				
_DET_MESI_setRxDetectorMask:
	GET_ARG 1,AR0							;* AR0=start_ptrs (arg1)
	PSHM	ST1
	RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,B
	LD		#0,DP
	LD		BH,DP							;* DP=&Tx_block
	CALL_	set_Rx_detector_mask
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* set_Rx_detector_mask: sets Rx_detector_mask and calls set_Rx_filter_mask
;* On entry it expects:
;*	 DP=&Rx_block
;*	AL=MASK
;****************************************************************************

set_Rx_detector_mask:				
DET_MESI_setRxDetectorMask:
	STL		A,Rx_detector_mask				;* Rx_detector_mask=MASK
	CMPM	Rx_state_ID,#RX_ENERGY_DET_ID								
	CC_		Rx_set_filter_mask,TC			;* if state_ID=ENERGY_DET, call set_filter 
	CMPM	Rx_state_ID,#RX_SIG_ANALYSIS_ID								
	CC_		Rx_set_filter_mask,TC			;* if state_ID=SIG_ANALYSIS_DET, call set_filter 
	RET_

;****************************************************************************
;* _set_Rx_digit_CP_mask
;* C function call: void set_Rx_digit_CP_mask()
;* Copies the first arg on the C stack to Rx_block.digit_CP_mask
;****************************************************************************

 .if COMPILER=ENABLED
_set_Rx_digit_CP_mask:				
_DET_MESI_setRxDigitCPmask:
	GET_ARG 1,AR0							;* AR0=start_ptrs (arg1)
	PSHM	ST1
	RSBX	CPL								;* reset compiler mode
	LD		*AR0(Rx_block_start),9,B
	LD		#0,DP
	LD		BH,DP							;* DP=&Tx_block
	CALL_	set_Rx_digit_CP_mask
	POPM	ST1
	RETC_
 .endif

;****************************************************************************
;* set_Rx_digit_CP_mask: sets Rx_digit_CP_mask 
;* On entry it expects:
;*	 DP=&Rx_block
;*	AL=MASK
;****************************************************************************

set_Rx_digit_CP_mask:				
DET_MESI_setRxDigitCPmask:
	STL		A,Rx_digit_CP_mask				;* AA[Rx_digit_CP_mask]=digit_CP_mask
	RET_		

;****************************************************************************
;* Rx_set_filter_mask: configures the filter masks according to 
;* Rx_detector_mask.
;* On entry it expects:
;*	 DP=&Rx_block
;****************************************************************************

Rx_set_filter_mask:				
DET_MESI_RxSetFilterMask:
	BITF	Rx_detector_mask,AUTO_DETECT_MASK
	RC_		NTC								;* return if auto_detect not enabled

	;**** v17 (F600, F1800, F3000) ****

 .if RX_V17_MODEM=ENABLED
	BITF	Rx_detector_mask,V17_MASK
	BC_		v17_set_filter_endif,NTC
	ORM		#(F500_MASK|F600_MASK|F1000_MASK|F1200_MASK|F1800_MASK),Rx_filter_mask_low
	ORM		#(F3000_MASK),Rx_filter_mask_high
v17_set_filter_endif:
 .endif

	;**** v21 channel 1 ****

 .if RX_V21_MODEM=ENABLED
	BITF	Rx_detector_mask,V21_CH1_MASK
	BC_		v21_ch1_set_filter_endif,NTC
	ORM		#(F980_MASK|F1100_MASK|F1180_MASK),Rx_filter_mask_low
v21_ch1_set_filter_endif:
 .endif

	;**** v21 channel 2 ****

 .if RX_V21_MODEM=ENABLED
	BITF	Rx_detector_mask,V21_CH2_MASK
	BC_		v21_ch2_set_filter_endif,NTC
	ORM		#(F500_MASK|F600_MASK|F1000_MASK|F1200_MASK|F1650_MASK|F1700_MASK|F1750_MASK|F1800_MASK|F1850_MASK),Rx_filter_mask_low
	ORM		#(F2400_MASK|F2600_MASK|F2900_MASK|F3000_MASK),Rx_filter_mask_high
v21_ch2_set_filter_endif:
 .endif

	;**** v22 CALL (F2225, F2250, F2850) ****

 .if RX_V22C_MODEM=ENABLED
	BITF	Rx_detector_mask,V22_MASK
	BC_		v22_set_filter_endif,NTC
	ORM		#(F1200_MASK|F1800_MASK),Rx_filter_mask_low
	ORM		#(F2225_MASK|F2250_MASK|F2850_MASK),Rx_filter_mask_high
v22_set_filter_endif:
 .endif

	;**** v27 # 2400 (F1200 & F2400) ****

 .if RX_V27_MODEM_2400=ENABLED
	BITF	Rx_detector_mask,V27_2400_MASK
	BC_		v27_2400_set_filter_endif,NTC
	ORM		#(F500_MASK|F600_MASK|F1000_MASK|F1200_MASK),Rx_filter_mask_low
	ORM		#(F2400_MASK),Rx_filter_mask_high
v27_2400_set_filter_endif:
 .endif

	;**** v27 # 4800 (F1000 & F2600) ****

 .if RX_V27_MODEM_4800=ENABLED
	BITF	Rx_detector_mask,V27_4800_MASK
	BC_		v27_4800_set_filter_endif,NTC
	ORM		#(F500_MASK|F600_MASK|F1000_MASK|F1200_MASK),Rx_filter_mask_low
	ORM		#(F2600_MASK),Rx_filter_mask_high
v27_4800_set_filter_endif:
 .endif

	;**** v29 (F500, F1700, F2900) ****

 .if RX_V29_MODEM=ENABLED
	BITF	Rx_detector_mask,V29_MASK
	BC_		v29_set_filter_endif,NTC
	ORM		#(F500_MASK|F600_MASK|F1000_MASK|F1200_MASK|F1700_MASK),Rx_filter_mask_low
	ORM		#(F2400_MASK|F2600_MASK|F2900_MASK|F3000_MASK),Rx_filter_mask_high
v29_set_filter_endif:
 .endif

	;**** v32 automode (F600, F1800, F2100, F2250, F2850, F3000) ****

 .if V32_AUTOMODE=ENABLED
	BITF	Rx_detector_mask,V32_AUTOMODE_MASK
	BC_		v32_automode_set_filter_endif,NTC
	ORM		#(F600_MASK|F1800_MASK),Rx_filter_mask_low
	ORM		#(F2100_MASK|F2250_MASK|F2850_MASK|F3000_MASK),Rx_filter_mask_high
	ST		#0,v32_automode_counter
v32_automode_set_filter_endif:
 .endif

	;**** CNG (F1100) ****

 .if CNG_TONE=ENABLED
	BITF	Rx_detector_mask,CNG_MASK
	BC_		CNG_set_filter_endif,NTC
	ORM		#(F1100_MASK),Rx_filter_mask_low
CNG_set_filter_endif:
 .endif

	;**** CED & ECSD (F2100) ****

 .if CED_TONE=ENABLED
	BITF	Rx_detector_mask,CED_MASK
	BC_		CED_set_filter_endif,NTC
	ORM		#(F2100_MASK),Rx_filter_mask_high
CED_set_filter_endif:
 .endif

	;**** TEP (F1700+F1800) ****

 .if (TEP_1700_TONE|TEP_1800_TONE)=ENABLED
	BITF	Rx_detector_mask,TEP_MASK
	BC_		TEP_set_filter_endif,NTC
	ORM		#(F1700_MASK|F1800_MASK),Rx_filter_mask_low
TEP_set_filter_endif:
 .endif

	;**** call progress ****

 .if RX_CALL_PROGRESS=ENABLED
	BITF	Rx_detector_mask,CALL_PROGRESS_MASK
	BC_		CP_set_filter_endif,NTC
 .if $isdefed("NORTH_AMERICA_PSTN")
	ORM		#(F350_MASK|F460_MASK|F600_MASK),Rx_filter_mask_low
 .endif
 .if $isdefed("BRAZIL_PSTN")
	ORM		#(F425_MASK),Rx_filter_mask_low
 .endif
CP_set_filter_endif:
 .endif

	RET_

;****************************************************************************
;* Rx_energy_det: detects increase in SNR on the line
;* On entry it expects:
;*	 DP=&Rx_block
;*	AR2=Rx_sample_tail
;*	BK=Rx_sample_len
;****************************************************************************

Rx_energy_det:
	MVDK	Rx_sample_stop,AR0		
	CMPR	EQ,AR2							;* sample_stop-sample_tail
	BC_		Rx_state_return,TC				;* return if stop=tail

	;**** call digit detector ****

 .if (RX_DTMF|RX_MF)=ENABLED
	LDPP	Rx_digit_detector,A
	CALA_	A
 .endif
	MVMM	AR2,AR3							;* AR3=Rx_sample_tail
	LD		Rx_sample_counter,A
	SUB		#1,A							;* Rx_sample_counter--
	BCD_		Rx_energy_det,ANEQ				;* loop if sample_counter!=0
	 STL	A,Rx_sample_counter
	 MAR	*AR2+%							;* Rx_sample_tail++
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter
	LD		Rx_SNR_est_coef,T				;* T=SNR_est_coef
	CALLD_	level_detector
	 STM	 #(RX_SNR_EST_LEN-2),BRC	

	;**** check SIG result ****

	SUB		Rx_threshold,16,B
	BC_		Rx_energy_det,BLT				;* loop if SIG<threshold
	LD		Rx_state_ID,B
	SUB		#RX_ENERGY_DET_ID,B
	BC_		Rx_state_return,BNEQ			;* return if state_ID!=ENERGY_DET	

	;**** switch state to analysis ****

	ST		#RX_ANALYSIS_LEN,Rx_sample_counter
	ST		#0,Rx_symbol_counter
	STPP	#Rx_sig_analysis,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_SIG_ANALYSIS_ID,Rx_state_ID	;* Rx_state_ID=RX_SIG_ANALYSIS_ID

;****************************************************************************
;* Rx_sig_analysis: analyzes spectral content of the line.
;*	 DP=&Rx_block
;*	AR2=Rx_sample_tail
;*	BK=Rx_sample_len
;****************************************************************************
					
Rx_sig_analysis:
	MVDK	Rx_sample_stop,AR0		
	CMPR	EQ,AR2							;* sample_stop-sample_tail
	BC_		Rx_state_return,TC				;* return if stop=tail

	;**** call digit detector ****

 .if (RX_DTMF|RX_MF)=ENABLED
	LDPP	Rx_digit_detector,A
	CALA_	A
 .endif
	LD		Rx_sample_counter,A
	SUB		#1,A							;* Rx_sample_counter--
	BCD_		Rx_state_return,ANEQ			;* return if sample_counter!=0
	 STL	A,Rx_sample_counter
	 MAR	*AR2+%							;* Rx_sample_tail++
	ADDM	#1,Rx_symbol_counter			;* Rx_symbol_counter++
	ST		#RX_ANALYSIS_LEN,Rx_sample_counter
	MVMM	AR2,AR7							;* AR7=Rx_sample_tail

	;**** broadband level estimator ****

	BB_EST	BROADBAND_EST_COEF,BROADBAND_EST_LEN,AR7
	STH		A,Rx_broadband_level			;* save result

	;**** low filter group ****

	LD		#0,A			
	BITF	Rx_filter_mask_low,F350_MASK
	BPF		FILTER_350_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_350

 .if $isdefed("BRAZIL_PSTN")
	LD		#0,A			
	BITF	Rx_filter_mask_low,F425_MASK
	BPF		FILTER_425_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_425
 .endif
 .if $isdefed("NORTH_AMERICA_PSTN")
	LD		#0,A			
	BITF	Rx_filter_mask_low,F460_MASK
	BPF		FILTER_460_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_460
 .endif

	LD		#0,A			
	BITF	Rx_filter_mask_low,F500_MASK
	BPF		FILTER_500_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_500

	LD		#0,A			
	BITF	Rx_filter_mask_low,F600_MASK
	BPF		FILTER_600_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_600

	LD		#0,A			
	BITF	Rx_filter_mask_low,F980_MASK
	BPF		FILTER_980_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_980

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1000_MASK
	BPF		FILTER_1000_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1000

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1100_MASK
	BPF		FILTER_1100_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1100

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1180_MASK
	BPF		FILTER_1180_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1180

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1200_MASK
	BPF		FILTER_1200_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1200

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1650_MASK
	BPF		FILTER_1650_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1650

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1700_MASK
	BPF		FILTER_1700_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1700

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1750_MASK
	BPF		FILTER_1750_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1750

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1800_MASK
	BPF		FILTER_1800_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1800

	LD		#0,A			
	BITF	Rx_filter_mask_low,F1850_MASK
	BPF		FILTER_1850_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_1850

	;**** high filter group ****

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2100_MASK
	BPF		FILTER_2100_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2100

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2225_MASK
	BPF		FILTER_2225_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2225

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2250_MASK
	BPF		FILTER_2250_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2250

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2400_MASK
	BPF		FILTER_2400_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2400

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2600_MASK
	BPF		FILTER_2600_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2600

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2850_MASK
	BPF		FILTER_2850_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2850

	LD		#0,A			
	BITF	Rx_filter_mask_high,F2900_MASK
	BPF		FILTER_2900_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_2900

	LD		#0,A			
	BITF	Rx_filter_mask_high,F3000_MASK
	BPF		FILTER_3000_COEF,RX_ANALYSIS_LEN,AR7
	STH		A,Rx_level_3000

	;**** dump_write debugging facility - enable to view signals ****
 
 .if 0=1
	LD		Rx_broadband_level,A 
	CALL_	dump_write
	LD		Rx_level_500,A 
	CALL_	dump_write
	LD		Rx_level_600,A 
	CALL_	dump_write
	LD		Rx_level_980,A 
	CALL_	dump_write
	LD		Rx_level_1000,A
	CALL_	dump_write
	LD		Rx_level_1100,A
	CALL_	dump_write
	LD		Rx_level_1180,A
	CALL_	dump_write
	LD		Rx_level_1200,A
	CALL_	dump_write
	LD		Rx_level_1650,A
	CALL_	dump_write
	LD		Rx_level_1700,A
	CALL_	dump_write
	LD		Rx_level_1750,A
	CALL_	dump_write
	LD		Rx_level_1800,A
	CALL_	dump_write
	LD		Rx_level_1850,A
	CALL_	dump_write
	LD		Rx_level_2100,A
	CALL_	dump_write
	LD		Rx_level_2225,A
	CALL_	dump_write
	LD		Rx_level_2250,A
	CALL_	dump_write
	LD		Rx_level_2400,A
	CALL_	dump_write
	LD		Rx_level_2600,A
	CALL_	dump_write
	LD		Rx_level_2850,A
	CALL_	dump_write
	LD		Rx_level_2900,A
	CALL_	dump_write
	LD		Rx_level_3000,A
	CALL_	dump_write
	LD		#0aaaah,A
	CALL_	dump_write
 .endif

	;**** detectors ****

	BITF	Rx_detector_mask,AUTO_DETECT_MASK
	BC_		Rx_sig_analysis_end,NTC
	LD		#NOT_DETECTED,A

 .if RX_V17_MODEM=ENABLED
	BITF	Rx_detector_mask,V17_MASK
	CC_		 v17_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V21_MODEM_CH1=ENABLED
	BITF	Rx_detector_mask,V21_CH1_MASK
	CC_	v21_ch1_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V21_MODEM_CH2=ENABLED
	BITF	Rx_detector_mask,V21_CH2_MASK
	CC_	v21_ch2_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V22C_MODEM=ENABLED
	BITF	Rx_detector_mask,V22_MASK
	CC_	v22C_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V27_MODEM_2400=ENABLED
	BITF	Rx_detector_mask,V27_2400_MASK
	CC_	v27_2400_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V27_MODEM_4800=ENABLED
	BITF	Rx_detector_mask,V27_4800_MASK
	CC_	v27_4800_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if RX_V29_MODEM=ENABLED
	BITF	Rx_detector_mask,V29_MASK
	CC_	v29_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

	;**** tone detectors ****

 .if CNG_TONE=ENABLED
	BITF	Rx_detector_mask,CNG_MASK
	CC_	CNG_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if CED_TONE=ENABLED
	BITF	Rx_detector_mask,CED_MASK
	CC_	CED_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if TEP_1700_TONE=ENABLED
	BITF	Rx_detector_mask,TEP_MASK
	CC_	TEP_1700_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

 .if TEP_1800_TONE=ENABLED
	BITF	Rx_detector_mask,TEP_MASK
	CC_	TEP_1800_detector,TC
	BC_		Rx_state_return,ANEQ			;* return if !NOT_DETECTED
 .endif

	;**** v32 automode detectors ****

; .if V32_AUTOMODE=ENABLED
;	BITF	Rx_detector_mask,V32_AUTOMODE_MASK
;	CC_	v32_automode_detector,TC
; .endif

 .if RX_CALL_PROGRESS=ENABLED
	BITF	Rx_detector_mask,CALL_PROGRESS_MASK
	CC_	call_progress_detector,TC
 .endif

	;**** switch back to energy_det if no detections ****

Rx_sig_analysis_end:
	LD		Rx_state_ID,A
	SUB		#RX_SIG_ANALYSIS_ID,A	
	BC_		#Rx_state_return,ANEQ
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter
	STPP	#Rx_energy_det,Rx_state,B
	BD_		Rx_state_return
	 ST		#RX_ENERGY_DET_ID,Rx_state_ID

return_not_detected:
	LD		#NOT_DETECTED,A
	RET_
return_detected:
	LD		#DETECTED,A
	RET_

;****************************************************************************
;* v17 detector routine
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_V17_MODEM=ENABLED
v17_detector:					
	MPY		Rx_level_600,#ONE_BY_ROOT3,B	
	MAC		Rx_level_1800,#ONE_BY_ROOT3,B	;* B=(F600+F1800)/sqrt3
	MAC		Rx_level_3000,#ONE_BY_ROOT3,B	;* B=(F600+F1800+F3000)/sqrt3
	STH		B,temp0							;* temp0=Psum
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if Psum<THR
	RATIO	temp0,Rx_broadband_level,THR_3DB
	BC_		return_not_detected,BLEQ		;* returnif |Psum/Pbb|<=THR
	RATIO	Rx_level_600,Rx_level_3000,THR_20DB
	BC_		return_not_detected,BLEQ		;* return if |600/3000|<=THR
	MPY		temp0,#THR_3DB,B				;* B=Psum*THR
	SUB		Rx_level_500,16,B				;* B-=F500
	SUB		Rx_level_1000,16,B				;* B-=F1000
	SUB		Rx_level_1200,16,B				;* B-=F1200
	SUB		Rx_level_2400,16,B				;* B-=F2600
	SUB		Rx_level_2600,16,B				;* B-=F3000
	SUB		Rx_level_2900,16,B				;* B-=F2900
	BC_		return_not_detected,BLEQ		;* return if Psum*THR<=k

	;**** calculate filtered power for agc estimator ****

	SQUR	Rx_level_600,B					;* B=level_600^2
	SQURA	Rx_level_1800,B					;* B+=level_1800^2
	SQURA	Rx_level_3000,B					;* B+=level_3000^2
	STH		B,Rx_power				
	CALL_	Rx_init_v17
	LD		#DETECTED,A
	RET_
 .endif

;****************************************************************************
;* v21 detector routines
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_V21_MODEM_CH1=ENABLED
v21_ch1_detector:				
	RET_
 .endif
						
 .if RX_V21_MODEM_CH2=ENABLED
v21_ch2_detector:				
	LD		Rx_level_1650,B
	ADD		Rx_level_1850,B
	STL		B,temp0							;* temp0=Psum
	MPY		temp0,#THR_1DB,B				;* B=Psum*THR
			SUB	Rx_broadband_level,16,B		;* B=Psum*THR-Pbb
	BC_		return_not_detected,BGEQ		;* return if Psum/Pbb>=THR
	MPY		Rx_level_1650,#THR_16DB,B		;* B=F1650*THR
	SUB		Rx_level_1750,16,B				;* B=F1650*THR-F1750
	BC_		return_not_detected,BLT			;* return if F1650/F1750<THR
	LD		temp0,B							;* B=Psum
	SUB		Rx_threshold,B,A				;* compare with min level threshold
	BCD_		return_not_detected,ALT			;* return if Psum<THR
	 ADD	Rx_level_1700,B
	 ADD	Rx_level_1800,B
	STL		B,temp0							;* temp0=Psum
	MPY		temp0,#THR_12DB,B				;* B=Psum*THR
	SUB		Rx_level_500,16,B				;* B-=F500
	SUB		Rx_level_600,16,B				;* B-=F600
	SUB		Rx_level_1000,16,B				;* B-=F1000
	SUB		Rx_level_1200,16,B				;* B-=F1200
	SUB		Rx_level_2400,16,B				;* B-=F2400
	SUB		Rx_level_2600,16,B				;* B-=F2600
	SUB		Rx_level_2900,16,B				;* B-=F2900
	SUB		Rx_level_3000,16,B				;* B-=F3000
	BC_		return_not_detected,BLEQ		;* return if Psum*THR<=(F500+F600+F1000+F1200)

	;**** calculate filtered power for agc estimator ****

	SQUR	Rx_level_1650,B					;* B=level_1650^2
	SQURA	Rx_level_1850,B					;* B=level_1850
	STH		B,Rx_power
	CALL_	Rx_init_v21_ch2
	LD		#DETECTED,A
	RET_
 .endif									

;****************************************************************************
;* v22 detector routines
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_V22C_MODEM=ENABLED
v22C_detector:						 
	LD		Rx_level_2225,B
	SUB		Rx_threshold,B,A
	BC_		v22_USB1_detector,BLT			;* branch if F2225<THR
	ADD		Rx_level_1800,B					;* B=F1800+F2250
	STL		B,temp0
	RATIO	temp0,Rx_broadband_level,THR_3DB
	BC_		v22_USB1_detector,BLEQ			;* branch if |BB/(1800+2225)|<=THR
	MPY		 Rx_level_2225,#THR_3DB,B
	SUB		Rx_level_1800,16,B				;* B=F2250*THR-F1800
	BC_		v22_USB1_detector,BLEQ			;* branch if 2225*THR<=F1800

	;**** calculate filtered power for agc estimator ****
	
	SQUR	Rx_level_2225,B					;* B=level_2225^2
	STH		B,Rx_power
	CALL_	Rx_init_v22C			
	LD		#DETECTED,A
	RET_

v22_USB1_detector:
	LD		Rx_level_2250,B
	SUB		Rx_threshold,B,A
	BC_		return_not_detected,ALT			;* return if F2250<THR
	ADD		Rx_level_1800,B					;* B=F1800+F2250
	ADD		Rx_level_2850,B					;* B=F1800+F2250+F2850
	STL		B,temp0
	RATIO	temp0,Rx_broadband_level,THR_3DB
	BC_		return_not_detected,BLEQ		;* return if |BB/(1800+2250+F2850)|<=THR
	MPY		 Rx_level_2250,#THR_3DB,B
	SUB		Rx_level_1800,16,B				;* B=F2250*THR-F1800
	BC_		return_not_detected,BLEQ		;* return if 2250*THR<=F1800

	;**** calculate filtered power for agc estimator ****
	
	SQUR	Rx_level_2250,B					;* B=level_2250^2
	SQURA	Rx_level_2850,B					;* B=level_2850^2
	STH		B,Rx_power
	CALL_	Rx_init_v22C			
	LD		#DETECTED,A
	RET_
 .endif

;****************************************************************************
;* v27 detector routines
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_V27_MODEM_2400=ENABLED
v27_2400_detector:				
	MPY		Rx_level_1200,#ONE_BY_ROOT2,B
	MAC		Rx_level_2400,#ONE_BY_ROOT2,B	;* B=(F1200+F2400)/sqrt2
	STH		B,temp0							;* temp0=Psum
	RATIO	temp0,Rx_broadband_level,THR_4DB
	BC_		return_not_detected,BLEQ		;* return if |BB/(1200+2400|<=THR
	RATIO	Rx_level_1200,Rx_level_2400,THR_20DB
	BC_		return_not_detected,BLEQ		;* return if |1200/2400|<=THR
	LD		temp0,B
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if Psum<THR
	MPY		temp0,#THR_3DB,B				;* B=Psum*THR
	SUB		Rx_level_500,16,B				;* B-=F500
	SUB		Rx_level_600,16,B				;* B-=F600
	SUB		Rx_level_1000,16,B				;* B-=F1000
	SUB		Rx_level_1800,16,B				;* B-=F1800
	SUB		Rx_level_2600,16,B				;* B-=F2600
	SUB		Rx_level_2900,16,B				;* B-=F2900
	SUB		Rx_level_3000,16,B				;* B-=F3000
	BC_		return_not_detected,BLEQ		;* return if Psum*THR<=k

	;**** calculate filtered power for agc estimator ****

	SQUR	Rx_level_1200,B					;* B=level_1200^2
	SQURA	Rx_level_2400,B					;* B+=level_2400^2
	STH		B,Rx_power						;* Rx_power=P1200+P2400
	ST		#2400,Rx_rate
	CALL_	Rx_init_v27
	LD		#DETECTED,A
	RET_
 .endif

 .if RX_V27_MODEM_4800=ENABLED
v27_4800_detector:				
	MPY		Rx_level_1000,#ONE_BY_ROOT2,B	
	MAC		Rx_level_2600,#ONE_BY_ROOT2,B	;* B=(F1000+F2600)/sqrt2
	STH		B,temp0							;* temp0=Psum
	RATIO	temp0,Rx_broadband_level,THR_4DB
	BC_		return_not_detected,BLEQ		;* return if |BB/(1000+2600|<=THR
	RATIO	Rx_level_1000,Rx_level_2600,THR_20DB
	BC_		return_not_detected,BLEQ		;* return if |1000/2600|<=THR
	LD		temp0,B
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if Psum<THR
	MPY		temp0,#THR_3DB,B				;* B=Psum*THR
	SUB		Rx_level_500,16,B				;* B-=F500
	SUB		Rx_level_600,16,B				;* B-=F600
	SUB		Rx_level_1200,16,B				;* B-=F1200
	SUB		Rx_level_1800,16,B				;* B-=F1800
	SUB		Rx_level_2400,16,B				;* B-=F2400
	SUB		Rx_level_2900,16,B				;* B-=F2900
	SUB		Rx_level_3000,16,B				;* B-=F3000
	BC_		return_not_detected,BLEQ		;* return if Psum*THR<=(F500+F600+1200)

	;**** calculate filtered power for agc estimator ****

	SQUR	Rx_level_1000,B					;* B=level_1000^2
	SQURA	Rx_level_2600,B					;* B+=level_2600^2
	STH		B,Rx_power						;* Rx_power=P1000+P2600
	ST		#4800,Rx_rate
	CALL_	Rx_init_v27
	LD		#DETECTED,A
	RET_
 .endif

;****************************************************************************
;* v29 detector routines
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if RX_V29_MODEM=ENABLED
v29_detector:					
	MPY		Rx_level_500,#ONE_BY_ROOT3,B	
	MAC		Rx_level_1700,#ONE_BY_ROOT3,B	;* B=(F500+F1700)/sqrt3
	MAC		Rx_level_2900,#ONE_BY_ROOT3,B	;* B=(F500+F1700+F2900)/sqrt3
	STH		B,temp0							;* temp0=Psum
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if Psum<THR
	RATIO	temp0,Rx_broadband_level,THR_3DB
	BC_		return_not_detected,BLEQ		;* return if |BB/Psum|<=THR
	RATIO	Rx_level_500,Rx_level_2900,THR_20DB
	BC_		return_not_detected,BLEQ		;* return if |500/2900|<=THR
	MPY		temp0,#THR_3DB,B				;* B=Psum*THR
	SUB		Rx_level_600,16,B				;* B-=F600
	SUB		Rx_level_1000,16,B				;* B-=F1000
	SUB		Rx_level_1200,16,B				;* B-=F1200
	SUB		Rx_level_2400,16,B				;* B-=F2900
	SUB		Rx_level_2600,16,B				;* B-=F2600
	SUB		Rx_level_3000,16,B				;* B-=F3000
	BC_		return_not_detected,BLEQ		;* return if Psum*THR<=(F600+F1000+F1200)

	;**** calculate filtered power for agc estimator ****

	SQUR	Rx_level_500,B					;* B=level_500^2
	SQURA	Rx_level_1700,B					;* B+=level_1700^2
	SQURA	Rx_level_2900,B					;* B+=level_2900^2
	STH		B,Rx_power				
	CALL_	Rx_init_v29				
	LD		#DETECTED,A
	RET_
 .endif

;****************************************************************************

 .if RX_CALL_PROGRESS=ENABLED
call_progress_detector:			

	;**** look for dialtone (350 Hz + 440 Hz) ****

 .if $isdefed("NORTH_AMERICA_PSTN")
dialtone_detector:
	MPY		Rx_level_350,#CP_DUALTONE_SCALE,B	
	MAC		Rx_level_460,#CP_DIALTONE_460_SCALE,B ;* B=(F350+F460)*scale
	STH		B,temp0							;* temp0=Psum

	LD		Rx_state_ID,A
	SUB		#RX_DIALTONE_ID,A
	BCD_		CP_dialtone_else1,ANEQ			;* branch if !DIALTONE_ID
	 ADDM	#1,Rx_dialtone_counter			;* dialtone_counter++
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		CP_dialtone_endif2,BLEQ			;* branch if Psum<=THR
	RATIO	temp0,Rx_broadband_level,THR_2DB
	BC_		CP_dialtone_endif2,BLEQ			;* branch if Psum/Pbb<=THR
	ST		#0,Rx_dialtone_counter			;* dialtone_counter=0
CP_dialtone_endif2:
	LD		Rx_dialtone_counter,B
	SUB		#CP_UNDETECT_COUNT,B
	BC_		CP_dialtone_endif1,BLT			;* branch if counter<COUNT
	CALL_	 Rx_init_detector
	RET_

CP_dialtone_else1:
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	 NOP
	 NOP
	XC		2,BLT							;* if Psum<thr ...
	 ST		 #0,Rx_dialtone_counter			;* ... dialtone_counter=0
	RATIO	temp0,Rx_broadband_level,THR_2DB
	 NOP
	 NOP
	XC		2,BLT							;* if Psum/Pbb<THR ...
	 ST		 #0,Rx_dialtone_counter			;* ... dialtone_counter=0
	RATIO	Rx_level_350,Rx_level_460,THR_6DB
	 NOP
	 NOP
	XC		2,BLT							;* if F350/F460<THR ...
	 ST		 #0,Rx_dialtone_counter			;* ... dialtone_counter=0

	LD		Rx_dialtone_counter,B
	SUB		#CP_DETECT_COUNT,B
	BC_		CP_dialtone_endif1,BLT			;* branch if counter<COUNT
	LD		#0,A
	STL		A,Rx_reorder_counter
	STL		A,Rx_symbol_counter
	SQUR	Rx_level_350,B					;* B=level_350^2
	SQURA	Rx_level_460,B					;* B+=level_460^2
	STH		B,Rx_power						;* Rx_power=P350+P460
	ST		#RX_DIALTONE_ID,Rx_state_ID
	RET_
CP_dialtone_endif1:

	;**** look for busy/reorder (480 Hz + 620 Hz) ****

reorder_detector:
	LD		Rx_state_ID,A
	SUB		#RX_REORDER_ID,A,B
	 SUB	#RX_BUSY_ID,A
	XC		1,BEQ							;* if REORDER_ID ...
	 LD		#0,A							;* ... force true
	MPY		Rx_level_460,#CP_DUALTONE_SCALE,B	
	MAC		Rx_level_600,#CP_REORDER_600_SCALE,B ;* B=(F460+F600)*scale
	STH		B,temp0							;* temp0=Psum
	BCD_		CP_reorder_else1,ANEQ			;* branch if !REORDER_ID|BUSY_ID
	 ADDM	#1,Rx_reorder_counter			;* reorder_counter++
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		CP_reorder_endif2,BLEQ			;* branch if Psum<=THR
	RATIO	temp0,Rx_broadband_level,THR_2DB
	BC_		CP_reorder_endif2,BLEQ			;* branch if Psum/Pbb<=THR
	ST		#0,Rx_reorder_counter			;* reorder_counter=0

CP_reorder_endif2:
	LD		Rx_symbol_counter,A
	SUB		#BUSY_DETECT_LEN,A
	 LD		Rx_reorder_counter,B
	 SUB	#CP_UNDETECT_COUNT,B
	XC		2,AGT							;* if symbol_counter>BUSY_DETECT_LEN ...
	 ST		#RX_BUSY_ID,Rx_state_ID			;* ... ID=RX_BUSY_ID
	BC_		CP_reorder_endif1,BLT			;* branch if counter<COUNT
	CALL_	 Rx_init_detector
	RET_

CP_reorder_else1:
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	 NOP
	 NOP
	XC		2,BLT							;* if Psum<thr ...
	 ST		 #0,Rx_reorder_counter			;* ... reorder_counter=0
	RATIO	temp0,Rx_broadband_level,THR_2DB
	 NOP
	 NOP
	XC		2,BLT							;* if Psum/Pbb<THR ...
	 ST		 #0,Rx_reorder_counter			;* ... reorder_counter=0
	RATIO	Rx_level_460,Rx_level_600,THR_6DB
	 NOP
	 NOP
	XC		2,BLT							;* if F460/F600<THR ...
	 ST		 #0,Rx_reorder_counter			;* ... reorder_counter=0

	LD		Rx_reorder_counter,B
	SUB		#CP_DETECT_COUNT,B
	BC_		CP_reorder_endif1,BLT			;* branch if counter<COUNT
	LD		#0,A
	STL		A,Rx_reorder_counter
	STL		A,Rx_symbol_counter
	SQUR	Rx_level_460,B					;* B=level_460^2
	SQURA	Rx_level_600,B					;* B+=level_600^2
	STH		B,Rx_power						;* Rx_power=P460+P600
	ST		#RX_REORDER_ID,Rx_state_ID
	RET_
CP_reorder_endif1:

	;**** look for ringback (440 Hz + 480 Hz) ****

ringback_detector:
	LD		Rx_level_460,16,B
	LD		Rx_state_ID,A
	SUB		#RX_RINGBACK_ID,A
	BCD_		CP_ringback_else1,ANEQ			;* branch if !RINGBACK_ID
	 ADDM	#1,Rx_ringback_counter			;* ringback_counter++
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		CP_ringback_endif2,BLEQ			;* branch if Psum<=THR
	RATIO	Rx_level_460,Rx_broadband_level,THR_2DB
	BC_		CP_ringback_endif2,BLEQ			;* branch if Psum/Pbb<=THR
	ST		#0,Rx_ringback_counter			;* ringback_counter=0
CP_ringback_endif2:
	LD		Rx_ringback_counter,B
	SUB		#CP_UNDETECT_COUNT,B
	BC_		CP_ringback_endif1,BLT			;* branch if counter<COUNT
	CALL_	 Rx_init_detector
	RET_

CP_ringback_else1:
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	 NOP
	 NOP
	XC		2,BLT							;* if Psum<thr ...
	 ST		 #0,Rx_ringback_counter			;* ... ringback_counter=0
	RATIO	Rx_level_460,Rx_broadband_level,THR_2DB
	 NOP
	 NOP
	XC		2,BLT							;* if Psum/Pbb<THR ...
	 ST		 #0,Rx_ringback_counter			;* ... ringback_counter=0
	MPY		Rx_level_460,#THR_6DB,B			;* B=Psum*THR
	SUB		Rx_level_350,16,B				;* B=Psum*THR-F350
	SUB		Rx_level_600,16,B				;* B=Psum*THR-F350-F600
	 NOP
	 NOP
	XC		2,BLT							;* if F460*THR-F350-F460<THR ...
	 ST		 #0,Rx_ringback_counter			;* ... ringback_counter=0

	LD		Rx_ringback_counter,B
	SUB		#CP_DETECT_COUNT,B
	BC_		CP_ringback_endif1,BLT			;* branch if counter<COUNT
	LD		#0,A
	STL		A,Rx_reorder_counter
	STL		A,Rx_symbol_counter
	SQUR	Rx_level_460,B					;* B=level_460^2
	STH		B,Rx_power						;* Rx_power=P460
	ST		#RX_RINGBACK_ID,Rx_state_ID
	RET_
CP_ringback_endif1:

	RET_
 .endif

 .if $isdefed("BRAZIL_PSTN")

	;**** execute the detectors/undetectors ****

	LD		Rx_level_425,16,B
	BITF	Rx_CP_corr_register_low,#1
	BCD_	CP_detector_else1,NTC			;* branch if corr&1=0
	 ADDM	#1,Rx_CP_detect_counter			;* CP_detect_counter++
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	BC_		CP_detector_endif2,BLEQ			;* branch if Psum<=THR
	RATIO	Rx_level_425,Rx_broadband_level,THR_2DB
	BC_		CP_detector_endif2,BLEQ			;* branch if Psum/Pbb<=THR
	ST		#0,Rx_CP_detect_counter			;* CP_detect_counter=0
CP_detector_endif2:
	LD		Rx_CP_detect_counter,B
	SUB		#CP_UNDETECT_COUNT,B
	BC_		CP_detector_endif1,BLT			;* branch if counter<COUNT
	ST		#0,Rx_CP_detect_counter			;* CP_detect_counter=0
	BD_		CP_detector_endif1
	ANDM	#~1,Rx_CP_corr_register_low		;* CP_corr_register_low&=~1

CP_detector_else1:
	SUB		Rx_threshold,16,B				;* compare with min level threshold
	 NOP
	 NOP
	XC		2,BLT							;* if Psum<thr ...
	 ST		#0,Rx_CP_detect_counter			;* ... CP_detect_counter=0
	RATIO	Rx_level_425,Rx_broadband_level,THR_2DB
	 NOP
	 NOP
	XC		2,BLT							;* if Psum/Pbb<THR ...
	 ST		#0,Rx_CP_detect_counter			;* ... CP_detect_counter=0

	LD		Rx_CP_detect_counter,B
	SUB		#CP_DETECT_COUNT,B
	BC_		CP_detector_endif1,BLT			;* branch if counter<COUNT
	ST		#0,Rx_CP_detect_counter			;* CP_detect_counter=0
	ORM		#1,Rx_CP_corr_register_low			;* CP_corr_register_low|=1
	SQUR	Rx_level_425,B					;* B=level_425^2
	STH		B,Rx_power						;* Rx_power=P425
	CMPM	Rx_state_ID,#RX_SIG_ANALYSIS_ID
	BC_		CP_detector_endif1,NTC			;* branch if !RX_SIG_ANALYSIS_ID
	ST		#CP_SAMPLE_PERIOD/2,Rx_symbol_counter
	ST		#RX_CALL_PROGRESS_ID,Rx_state_ID
CP_detector_endif1:

	;**** subsample and correlate for signal detection ****

	LDU		Rx_symbol_counter,A
	SUB		#CP_SAMPLE_PERIOD,A
	RCD_	ALT								;* return if counter<CP_SAMPLE_PERIOD
	 CMPM	Rx_state_ID,#RX_SIG_ANALYSIS_ID
	RC_		TC								;* return if RX_SIG_ANALYSIS_ID

	ST		#0,Rx_symbol_counter
	LD		Rx_CP_corr_register,A			
	OR		Rx_CP_corr_register_low,A		;* A=corr_register
	LD		#1,B
	AND		Rx_CP_corr_register_low,B		;* B=corr_register_low&1
	OR		A,1,B							;* B|=corr_register<<1
	STH		B,Rx_CP_corr_register
	STL		B,Rx_CP_corr_register_low

	;**** check for no detection ****

	LD		Rx_CP_corr_register_low,A
	BC_		CP_detector_endif3,ANEQ			;* branch if corr_register_low!=0
	CALL_	Rx_init_detector
	RET_
CP_detector_endif3:

	;**** check for all correlation bits present ****

	BITF	Rx_CP_corr_register_low,#CORR_MSB_POSITION
	RC_		NTC								;* return if !CORR_MSB_POSITION

	ST		#RX_CALL_PROGRESS_ID,Rx_state_ID
	CMPM	Rx_CP_corr_register_low,#DIALTONE_CORR_PATTERN
	 LD		Rx_CP_corr_register_low,A
	 AND	#~0010h,A,B						;* B=corr&0x0100
	SUB		#RINGBACK_CORR_PATTERN,B
	XC		2,TC							;* if corr=DIALTONE_CORR_PATTERN ...
	 ST		#RX_DIALTONE_ID,Rx_state_ID		;* ... state_ID=RX_DIALTONE_ID
	CMPM	Rx_CP_corr_register_low,#BUSY_CORR_PATTERN
	XC		2,BEQ							;* if corr=RINGBACK_CORR_PATTERN ...
	 ST		#RX_RINGBACK_ID,Rx_state_ID		;* ... state_ID=RX_RINGBACK_ID
	SUB		#REORDER_CORR_PATTERN,A,B
	XC		2,TC							;* if corr=BUSY_CORR_PATTERN ...
	 ST		#RX_BUSY_ID,Rx_state_ID			;* ... state_ID=RX_BUSY_ID
	XC		2,BEQ							;* if corr=REORDER_CORR_PATTERN ...
	 ST		#RX_REORDER_ID,Rx_state_ID		;* ... state_ID=RX_REORDER_ID

	;**** check for no valid detections ****

	CMPM	Rx_state_ID,#RX_CALL_PROGRESS_ID
	CC_		Rx_init_detector,TC				;* if !RX_CALL_PROGRESS_ID, Rx_init_detector

	LD		#0,A
	RETD_
	 STL	A,Rx_CP_corr_register			;* Rx_CP_corr_register=0
	 STL	A,Rx_CP_corr_register_low		;* Rx_CP_corr_register_low=0
 .endif		;* endif BERAZIL

 .endif

;****************************************************************************
;* v32 automode detector routines
;* On entry it expects:
;*	DP=&Rx_block
;* Modifies:
;****************************************************************************

 .if V32_AUTOMODE=ENABLED
v32_automode_detector:				

;****************************************************************************
;* v32_automode_segment: computes generic segment
;* On entry it expects the following setup:
;* Modifies:
;* On return:
;****************************************************************************

v32_automode_segment:				
 .endif				 

;****************************************************************************
;* tone detector routines
;* On entry it expects:
;*	DP=&Rx_block
;****************************************************************************

 .if CNG_TONE=ENABLED
CNG_detector:					
	RATIO	Rx_level_1100,Rx_broadband_level,THR_1DB
	BC_		return_not_detected,BLEQ		;* return if |1100/BB|<=THR
	LD		Rx_level_1100,B
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if 1100<THR
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter	
	SQUR	Rx_level_1100,A					;* A=F1100^2
	STH		A,Rx_power		
	STPP	#Rx_tone_undetector_state,Rx_state,B
	LD		#DETECTED,A
	RETD_
	 ST		#RX_CNG_ID,Rx_state_ID			;* Rx_state_ID=RX_CNG_ID
 .endif

 .if CED_TONE=ENABLED
CED_detector:					
	RATIO	Rx_level_2100,Rx_broadband_level,THR_1DB
	BC_		return_not_detected,BLEQ		;* return if |2100/BB|<=THR
	LD		Rx_level_2100,B
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if 2100<THR
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter	
	SQUR	Rx_level_2100,A					;* A=F2100^2
	STH		A,Rx_power		
	STPP	#Rx_tone_undetector_state,Rx_state,B
	LD		#DETECTED,A
	RETD_
	 ST		#RX_CED_ID,Rx_state_ID			;* Rx_state_ID=RX_CED_ID
 .endif

 .if TEP_1700_TONE=ENABLED
TEP_1700_detector:				
	RATIO	Rx_level_1700,Rx_broadband_level,THR_1DB
	BC_		return_not_detected,BLEQ		;* return if |1700/BB|<=THR
	MPY		Rx_level_1700,#THR_14DB,B		;* B=F1700*THR
	SUB		Rx_level_1800,16,B
	BC_		return_not_detected,BLEQ		;* branch if F1700*THR<=F1800
	LD		Rx_level_1700,B

	SUB		Rx_level_1650,B,A
	BC_		return_not_detected,ALEQ		;* return if F1700-F1650<=0
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if F1700<THR
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter	
	SQUR	Rx_level_1700,A					;* A=F1700^2
	STH		A,Rx_power		
	STPP	#Rx_tone_undetector_state,Rx_state,B
	LD		#DETECTED,A
	RETD_
	 ST		#RX_TEP_1700_ID,Rx_state_ID		;* Rx_state_ID=RX_TEP_1700_ID
 .endif

 .if TEP_1800_TONE=ENABLED
TEP_1800_detector:				
	RATIO	Rx_level_1800,Rx_broadband_level,THR_1DB
	BC_		return_not_detected,BLEQ		;* return if |1800/BB|<=THR
	MPY		Rx_level_1800,#THR_14DB,B		;* B=F1800+THR
	SUB		Rx_level_1700,16,B
	BC_		return_not_detected,BLEQ		;* branch if F1800*THR<=F1700
	LD		Rx_level_1800,B
	SUB		Rx_threshold,B					;* compare with min level threshold
	BC_		return_not_detected,BLT			;* return if 1800<THR
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter	
	SQUR	Rx_level_1800,A					;* A=F1800^2
	STH		A,Rx_power		
	STPP	#Rx_tone_undetector_state,Rx_state,B
	LD		#DETECTED,A
	RETD_
	 ST		#RX_TEP_1800_ID,Rx_state_ID		;* Rx_state_ID=RX_TEP_1800_ID
 .endif

;****************************************************************************
;* Rx_tone_undetector: waits for tone to go away and checks for phase
;* reversals in 2100 Hz.
;****************************************************************************

Rx_tone_undetector_state:
	MVDK	Rx_sample_stop,AR0		
	CMPR	EQ,AR2							;* sample_stop-sample_tail
	BC_		Rx_state_return,TC				;* return if stop=tail
	MVMM	AR2,AR3							;* AR3=Rx_sample_tail
	LD		Rx_sample_counter,A
	SUB		#1,A							;* Rx_sample_counter--
	BCD_		Rx_state_return,ANEQ			;* return if sample_counter!=0
	 STL	A,Rx_sample_counter
	 MAR	*AR2+%							;* Rx_sample_tail++
	MVDK	Rx_start_ptrs,AR0
	ST		#RX_SNR_EST_PERIOD,Rx_sample_counter
	CALLD_	energy_undetector
	 STM	 #(RX_SNR_EST_LEN-2),BRC	

	;**** test result ****

	BCD_		ECSD_detector,BLEQ				;* branch if sig*SNR_thr<=noise
	 CMPM	Rx_state_ID,#RX_CED_ID
	CALL_	Rx_init_detector				;* switch back to energy_detector
	B_		Rx_state_return

	;**** ECSD (phase reversals) detector ****

ECSD_detector:					
	BCD_		Rx_state_return,NTC				;* return if not CED (2100 Hz)
	 STM	#RX_ECSD_CORR_DELAY,AR0
	MVMM	AR2,AR3							;* AR3=Rx_sample_tail
	MAR		*AR3-0%							;* AR3= l=(AR3+LEN-CORR_DELAY)%LEN
	STM		#RX_ECSD_CORR_DISP,AR0
	MVMM	AR3,AR4				
	MAR		*AR4-0%							;* AR4= k=(AR3+LEN-CORR_DISP)%LEN
	STM		#(RX_ECSD_CORR_LEN-2),BRC		
	LD		#0,B
	RPTB	ECSD_detector_loop				;* for ECSD_CORR_LEN times ...
	 LD		*AR4-%,14,A						;* A=sample[k]>>2
ECSD_detector_loop:
	 MACA	*AR3-%,B						;* B=sample[l]*sample[k]>>2
	LD		*AR4-%,14,A						;* A=sample[k]>>2
	MACAR	*AR3-%,B						;* B=sample[l]*sample[k]>>2
	BC_		Rx_state_return,BGEQ			;* return if corr>=0
	BD_		Rx_state_return
	 ST		#RX_ECSD_ID,Rx_state_ID			;* update Rx_state_ID

;****************************************************************************
;* energy_undetector: computes current average (signal estimate) and delayed
;* average (noise_estimate) and returns signal_est*THR compared to noise_est.
;* On entry it expects the following setup:
;*	AR0->Rx_start_ptrs
;*	AR3->Rx_sample[*]
;*	BK=Rx_sample_len
;*	BRC=SNR_EST_LEN-2
;* On return:
;*	BH=noise_est-signal_est*THR
;*	 ALU status reflects result in BH
;****************************************************************************

energy_undetector:				
	MVMM	AR3,AR4							;* AR4=AR3
	MVDK	*AR0(Rx_block_start),AR5		;* AR5=&Rx_block[0]
	LD		#0,B
	LD		*AR3-%,16,A						;* A=Rx_sample[*]
	STM		#2*RX_SNR_EST_LEN,AR0
	MAR		*AR4-0%							;* AR4-=2*SNR_EST_LEN
	STM		#Rx_SNR_est_coef,AR0
	MAR		*AR5+0							;* AR5=&Rx_block.SNR_est_coef
	RPTB	energy_undetector_loop
	 ABS	A
	 MASA	*AR5+,B							;* B-=|sample[l]|*SNR_est_coef
	 LD		*AR4-%,16,A						;* A=Rx_sample[*]
	 ABS	A
	 MACA	*AR5-,B							;* B+=|sample[l]|*SNR_thr_coef
energy_undetector_loop:
	 LD		*AR3-%,16,A						;* A=Rx_sample[*]
	ABS		A
	MASA	*AR5+,B							;* B-=|sample[l]|*SNR_est_coef
	LD		*AR4-%,16,A						;* A=Rx_sample[*]
	RETD_
	 ABS	A
	 MACAR	*AR5-,B							;* B+=|sample[l]|*SNR_thr_coef

;****************************************************************************
;* level_detector: estimates average signal level (envelope).
;* On entry it expects the following setup:
;*	T=SNR_est_coef
;*	AR3->Rx_sample[*]
;*	BK=Rx_sample_len
;*	BRC=SNR_EST_LEN-2
;* On return:
;*	BH=signal level estimate (SIG)
;****************************************************************************

level_detector:					
	RPTBD	level_detector_loop
	LD		#0,B
	LD		*AR3-%,16,A						;* A=Rx_sample[*]
	 ABS	A
	 MACA	T,B								;* B+=|sample[l]|*SNR_est_coef
level_detector_loop:
	 LD		*AR3-%,16,A						;* A=Rx_sample[*]
	RETD_
	 ABS	A
	 MACAR 	T,B								;* B+=|sample[l]|*SNR_est_coef

;****************************************************************************
 .endif

	.end
 
