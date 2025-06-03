;****************************************************************************
;* Filename "memory.asm"													  
;* Date: 05-23-98
;* Author: Peter B. Miller																
;* Company: MESi						
;*		   10909 Lamplighter Lane, Potomac, MD 20854								
;* Phone: (301) 765-9668																
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net																
;* Description: Module that declares the data memory buffers				 
;* and structures used by vmodem channels.										 
;****************************************************************************

	.include	"vmodem.inc"
	.include	"common.inc"
	.include	"config.inc"

	;**** common tables and coefficients ****

	.include	"sincoef.inc"
	.include	"rcoscoef.inc"
 .if $isdefed("XDAIS_API")
	CIRCDEF		_VCOEF_MESI_DFTCoef,"DFTcoef",DFT_COEF_LEN
 .else
	CIRCDEF		_DFT_coef,"DFTcoef",DFT_COEF_LEN
 .endif										;* "XDAIS_API endif

;++++#ifndef MESI_INTERNAL 02-23-2001
;	;**** full memory declaration for all modem coefficients ****
;
;	.sect   "vcoefs"
;
;	;**** memory declaration for single channel ****
;
; .if TX_BLOCK_LEN!=0
;_Tx_block		.usect	"TxBlk",TX_BLOCK_LEN
;	.global	_Tx_block
; .endif
; .if TX_SAMPLE_LEN!=0
;_Tx_sample		   .usect	"TxSmpl",TX_SAMPLE_LEN
;	   .global	_Tx_sample	
; .endif
; .if TX_DATA_LEN!=0
;_Tx_data		   .usect	"TxData",TX_DATA_LEN
;	   .global	_Tx_data	
; .endif
; .if TX_FIR_LEN!=0
;_Tx_fir		   .usect	"TxFir",TX_FIR_LEN
;	   .global	_Tx_fir	
; .endif
; .if RX_BLOCK_LEN!=0
;_Rx_block		   .usect	"RxBlk",RX_BLOCK_LEN
;	   .global	_Rx_block	
; .endif
; .if RX_SAMPLE_LEN!=0
;_Rx_sample		   .usect	"RxSmpl",RX_SAMPLE_LEN
;	   .global	_Rx_sample	
; .endif
; .if RX_DATA_LEN!=0
;_Rx_data		   .usect	"RxData",RX_DATA_LEN
;	.global	_Rx_data	
; .endif
; .if RX_FIR_LEN!=0
;_Rx_fir		.usect	"RxFir",RX_FIR_LEN
;	   .global	_Rx_fir	
; .endif
; .if EQ_COEF_LEN!=0
;_EQ_coef		   .usect	"EQcoef",EQ_COEF_LEN
;	   .global	_EQ_coef	
; .endif	
; .if EC_COEF_LEN!=0
;_EC_coef		   .usect	"ECcoef",EC_COEF_LEN
;	.global	_EC_coef 
; .endif
; .if ENCODER_BLOCK_LEN!=0
;_encoder		   .usect	"Encode",ENCODER_BLOCK_LEN
;	.global	_encoder 
; .endif
; .if DECODER_BLOCK_LEN!=0
;_decoder		   .usect	"Decode",DECODER_BLOCK_LEN
;	.global	_decoder 
; .endif
; .if TRACE_BACK_BUF_LEN!=0
;_trace_back	  .usect	"Trace",TRACE_BACK_BUF_LEN
;	.global	_trace_back   
; .endif
;
; .if $isdefed("PTRS_LEN")
; .else
;	.asg	1,PTRS_LEN
; .endif
; 
; .if PTRS_LEN != 0
;	.sect	"vcoefs"
;
; .if $isdefed("XDAIS_API")
;_VCOEF_MESI_startPtrsTable:	
;	 .global	_VCOEF_MESI_startPtrsTable
; .else
;_start_ptrs_table:	
;	 .global	_start_ptrs_table
; .endif										;* "XDAIS_API endif
; .if TX_BLOCK_LEN!=0
;	.word	_Tx_block
; .else
;	.word	0
; .endif
; .if TX_SAMPLE_LEN!=0
;	.word	_Tx_sample				
; .else
;	.word	0
; .endif
; .if TX_DATA_LEN!=0
;	.word	_Tx_data				
; .else
;	.word	0
; .endif
; .if TX_FIR_LEN!=0
;	.word	_Tx_fir				
; .else
;	.word	0
; .endif
; .if RX_BLOCK_LEN!=0
;	.word	_Rx_block				
; .else
;	.word	0
; .endif
; .if RX_SAMPLE_LEN!=0
;	.word	_Rx_sample				
; .else
;	.word	0
; .endif
; .if RX_DATA_LEN!=0
;	.word	_Rx_data				
; .else
;	.word	0
; .endif
; .if RX_FIR_LEN!=0
;	.word	_Rx_fir			
; .else
;	.word	0
; .endif
; .if EQ_COEF_LEN!=0
;	.word	_EQ_coef			
; .else
;	.word	0
; .endif
; .if EC_COEF_LEN!=0
;	.word	_EC_coef			
; .else
;	.word	0
; .endif	
; .if ENCODER_BLOCK_LEN!=0
;	.word	_encoder
; .else
;	.word	0
; .endif	
; .if DECODER_BLOCK_LEN!=0
;	.word	_decoder	 
; .else
;	.word	0
; .endif	
; .if TRACE_BACK_BUF_LEN!=0
;	.word	_trace_back	   
; .else
;	.word	0
; .endif	
;_start_ptrs_table_end:	
; .endif	 	;* PTRS_LEN != 0
;
; .if $isdefed("PTRS_LEN")
; .else
;	.asg	_start_ptrs_table_end-_start_ptrs_table,PTRS_LEN
; .endif
;
; .if PTRS_LEN != 0
;_start_ptrs		.usect	"Ptrs",PTRS_LEN
;	.global	_start_ptrs
; .endif		;* PTRS_LEN != 0
;++++#else  MESI_INTERNAL 02-23-2001

	;**** build a single channel in memory with no channel suffix ****
	
	BLDCHAN					;* see vmodem.inc for BLDCHAN macro definition  
	
;++++#endif MESI_INTERNAL 02-23-2001
;****************************************************************************


