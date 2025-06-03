		.title "PROCESSOR INITIALIZATION"
        
        .mmregs

		.include context.inc
		
        .global	relocate ;_c_int00
		.ref	_reset
		.ref	_SLAC_isr 
		.ref	_UART_A_isr, _UART_B_isr
		.ref	_TINT_0_isr, _TINT_1_isr
		.ref	_BSP_0_rx_isr, _BSP_0_tx_isr
		.ref	_BSP_1_rx_isr, _BSP_1_tx_isr  
        
        .global SLAC_isr, UART_B_isr, UART_A_isr
        .global TINT_0_isr, TINT_1_isr
        .global BSP_0_rx_isr, BSP_0_tx_isr
        .global BSP_1_rx_isr, BSP_1_tx_isr
        
;Interrupt Vector Table
		.align 4
		.sect ".vectors"
; 0
RESET	PSHM XPC
		FB	relocate ;_c_int00
		NOP
; 1
		.align 4
NMI		PSHM XPC
		FB	_reset
		NOP
; 2
		.align 4
SINT17	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 3
		.align 4
SINT18	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 4
		.align 4
SINT19	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 5
		.align 4
SINT20	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 6
		.align 4
SINT21	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 7
		.align 4
SINT22	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 8
		.align 4
SINT23	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 9
		.align 4
SINT24	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 10
		.align 4
SINT25	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 11
		.align 4
SINT26	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 12
		.align 4
SINT27	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 13
		.align 4
SINT28	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 14
		.align 4
SINT29	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 15
		.align 4
SINT30	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 16
		.align 4
INT0	PSHM XPC
		FB	_SLAC_isr
		NOP
; 17
		.align 4
INT1	PSHM XPC
		FB	_UART_B_isr
		NOP
; 18
		.align 4
INT2	PSHM XPC
		FB	_UART_A_isr
		NOP
; 19
		.align 4
TINT	PSHM XPC
		FB	_TINT_0_isr ;TINT_0_isr
		NOP
; 20
		.align 4
BRINT0	PSHM XPC
		FB	_BSP_0_rx_isr
		NOP
; 21
		.align 4
BXINT0	PSHM XPC
		FB	_BSP_0_tx_isr
		NOP
; 22
		.align 4
TRINT	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 23
		.align 4
TXINT	PSHM XPC
		FB	_TINT_1_isr
		NOP
; 24
		.align 4
INT3	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 25
		.align 4
HPIINT	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 26
		.align 4
BRINT1	PSHM XPC
		FB	_BSP_1_rx_isr
		NOP
; 27
		.align 4
BXINT1	PSHM XPC
		FB	_BSP_1_tx_isr
		NOP
; 28
		.align 4
BMINT0	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 29
		.align 4
BMINT1	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 30
		.align 4
RES1	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP
; 31
		.align 4
RES2	PSHM XPC
		FB	UNEXPECTED_INTR
		NOP

;###########################################################################
;# The following code is copied from TI's rts.src                          #
;###########################################################################
****************************************************************************
*                                                                          *
*   This module contains the following definitions :                       *
*                                                                          *
*         __stack    - Stack memory area                                   *
*         _c_int00   - Boot function                                       *
*         _var_init  - Function which processes initialization tables      *
*                                                                          *
****************************************************************************
	.text
	.global  _c_int00, cinit
	.global  _main, _abort, __STACK_SIZE

	.ref	_bss, _ebss

****************************************************************************
* Declare the stack.  Size is determined by the linker option -stack.  The *
* default value is 1K words.                                               *
****************************************************************************
;Trap for unexpected interrupts
UNEXPECTED_INTR:	FRETE

SLAC_isr:		context_save
                fcall _SLAC_isr
                context_restore

TINT_0_isr:		context_save
    			fcall _TINT_0_isr
				context_restore

TINT_1_isr:		context_save
    			fcall _TINT_1_isr
				context_restore
				
				.sect "pump"
UART_B_isr: 	context_save
    			fcall _UART_B_isr
				context_restore

UART_A_isr: 	context_save
    			fcall _UART_A_isr
				context_restore

				.sect "vtext"
BSP_0_rx_isr: 	context_save
				fcall _BSP_0_rx_isr
				context_restore

BSP_0_tx_isr:	context_save
    			fcall _BSP_0_tx_isr
				context_restore

                .text
BSP_1_rx_isr:	context_save
    			fcall _BSP_1_rx_isr
				context_restore

BSP_1_tx_isr:	context_save
    			fcall _BSP_1_tx_isr
				context_restore

;;;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$;;;


__stack:	.usect	".stack", 0

****************************************************************************
* FUNCTION DEF : _c_int00                                                  *
*                                                                          *
*   1) Set up stack                                                        *
*   2) Set up proper status                                                *
*   3) If "cinit" is not -1, init global variables                         *
*   4) call users' program                                                 *
*                                                                          *
****************************************************************************
_c_int01:
	SSBX	INTM		; ALK
	LD	#0, DP
	STM	#000e0h, PMST; initialize CPU
	STM	#00c00h, ST0	; initialize status registers
	STM	#02900h, ST1 
	STM	#00000h, IMR ; disable all interrupt
	STM	#0FFFFh, IFR ; clear latched interrupts
    STM #07276h, SWWSR
    STM #00002h, BSCR
    STM #00001h, 02bh ;SWCR
	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	;	ALK - SET PLL PARAMETERS
	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	SSBX	XF
	STM		#0x0000, CLKMD
test_clk_status:
	LDM		CLKMD, A			;
	AND		#0x0001, A
	BC		test_clk_status, ANEQ
	STM		#0xB197, CLKMD
	RSBX	XF
	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

****************************************************************************
*  INIT STACK POINTER.  REMEMBER STACK GROWS FROM HIGH TO LOW ADDRESSES.   *
****************************************************************************
	STM		#__stack,SP		; set to beginning of stack memory
	ADDM	#(__STACK_SIZE-1),*(SP) ; add size to get to top
	ANDM	#0fffeh,*(SP)		; make sure it is an even address
	
	SSBX	SXM			; turn on SXM for LD #cinit,A

****************************************************************************
* SET UP REQUIRED VALUES IN STATUS REGISTER                                *
****************************************************************************
	SSBX	CPL			; turn on compiler mode bit

****************************************************************************
* SETTING THESE STATUS BITS TO RESET VALUES.  IF YOU RUN _c_int00 FROM     *
* RESET, YOU CAN REMOVE THIS CODE                                          *
****************************************************************************
	LD		#0,ARP
	RSBX	C16
	RSBX	CMPT
	RSBX	FRCT

	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	;	ALK - CLEAR BSS
	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	LD		#(_ebss - _bss), A
	BC		bss_done, AEQ
	STM		#_bss, AR1			; load .BSS start
	RPT		#(_ebss - _bss - 1)		; clear BSS
	ST		#0, *AR1+			;
	;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bss_done:
****************************************************************************
*  IF cinit IS NOT -1, PROCESS INITIALIZATION TABLES			   *
****************************************************************************
	; near mode
	;LD		#cinit,A                ; Get pointer to init tables
    ; far mode
	LDX	#cinit,16,A		
	OR	#cinit,A,A
    ;
	ADD		#1,A,B
	BC		DONE_INIT,BEQ		; if (cinit == -1) no init tables

;  PROCESS INITIALIZATION TABLES.  TABLES ARE IN PROGRAM MEMORY IN THE
;  FOLLOWING FORMAT:                                                       

;       .word  <length of init data in words>                              
;       .word  <address of variable to initialize>                        
;       .word  <init data>                                                
;       .word  ...                                                        
                                                                          
;  The init table is terminated with a zero length                         
    ; near mode
	;STM		#0, AH
	
	FB 		START				; start processing

LOOP:
	READA	*(AR2)				; AR2 = address
	ADD		#1,A				; A += 1

	RPT		*(AR1)				; repeat length+1 times
	READA	*AR2+				; copy from table to memory

	ADD		*(AR1),A			; A += length (READA doesn't change A)
	ADD		#1,A				; A += 1

START:
	READA	*(AR1)				; AR1 = length
	ADD		#1,A				; A += 1
	BANZ	LOOP,*AR1-			; if (length-- != 0) continue 


;CALL USER'S PROGRAM                                                     *
DONE_INIT:
	FCALL    _main			
	FB	    _c_int00			; to never return

		.end
		
		
		
