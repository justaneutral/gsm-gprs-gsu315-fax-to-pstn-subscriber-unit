;****************************************************************************
;* Filename: tcm.inc
;* Date: 04-11-97
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: This module defines all constants and macros.
;****************************************************************************

path_metrics_start	 		.set	(0*DELAY_STATES)
path_bits_start				.set	(1*DELAY_STATES)
min_distance_start	 		.set	(2*DELAY_STATES)
state_metrics_start			.set	(3*DELAY_STATES)

;****************************************************************************
;* macro: DLYSTATE OFFSET,NSP,COND: computes the following segment:			
;*	if ((Di=*SM++ + *(PM+OFFSET))<Dmin)				
;*		Dmin=Di, StatePath=NSP;					
;****************************************************************************

DMIN	 .macro	 OFFSET1,OFFSET2,NSP
	LD	  *AR4+,A					;* A= *SM++ 
	ADD	 *AR5(:OFFSET1:),16,A		;* A= Dmin=*SM+ + *(PM+0)
	ST	  #:NSP:,*AR7			 	;* StatePath=0
	ST	  A,*AR3				  	;* Dmin=*SM++ + *(PM+0)
||	LD	*AR4+,B				 		;* B= *SM++ 
	ADD	 *AR5(:OFFSET2:),16,B		;* B= Di=*SM++ + *(PM+2)
		.endm

DLYSTATE	.macro  OFFSET,NSP,COND
	SUB	 *AR3+,16,B,A				;* A=Di-Dmin
	 ST	 B,*AR3-				 	;* AR1=Di
||	 LD   *AR4+,B				 	;* B= *SM++ 
	 ADD	*AR5(:OFFSET:),16,B	 	;* B= Di=*SM+ + *(PM+OFFSET)
	XC	  1,A:COND:
	 MVMM   AR1,AR0				 	;* if Di<Dmin, Dmin=Di
	XC	  2,A:COND:
	 ST	 #:NSP:,*AR7			 	;* if Di<Dmin StatePath=NSP
		.endm

;****************************************************************************
