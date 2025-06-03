	.mmregs
	
	.global _slow_clock_mode
	.global _fast_clock_mode
	
	.ref	_uart_a_rate
	.ref	_uart_b_rate
	

_fast_clock_mode:
		PSHM	AL
		PSHM	AH
		PSHM	AG
		PSHM	ST0
		PSHM	ST1
		
		RSBX	SXM
		
		LDM	CLKMD,	A
		AND	#0xF000, A
		SUB #0xB000, A
		BC	L2, AEQ
		
		STM	#0x7276, 0x28	;SWWSR
		STM	#1, 0x2B		;SWCR
		STM	#0, 0x26		;TCR0
		
		stm	#0x0000,CLKMD
L1:
		ldm	CLKMD,A
		and	#0x0001,A
		bc	L1,ANEQ
		stm	#0xB197,CLKMD
		
		stm	#49151, 0x25
		ANDM #0xFFF0, 26
		
		FCALLD	_uart_a_rate
		NOP
		LD		#107, A
		FCALL	_uart_b_rate

L2:
		POPM	ST1
		POPM	ST0
		POPM	AG
		POPM	AH
		POPM	AL
		FRET
		

_slow_clock_mode:
		PSHM	AL
		PSHM	AH
		PSHM	AG
		PSHM	ST0
		PSHM	ST1
		
		RSBX	SXM
		
		LDM	CLKMD,	A
		AND	#0xF000, A
		SUB #0xF000, A
		BC	L4, AEQ
		
		stm	#0x0000,CLKMD
L3:
		ldm	CLKMD,A
		and	#0x0001,A
		bc	L3,ANEQ
		;stm	#0x03EF,CLKMD
		STM	#0xF3EF, CLKMD
		;STM	#0, CLKMD
		
		STM	#40960, 0x25	;PRD0
		STM	#0xF, 0x26		;TCR0
		
		STM	#0x7249, 0x28	;SWWSR
		STM	#0, 0x2B		;SWCR

		FCALLD	_uart_a_rate
		NOP
		LD		#9, A
		FCALL	_uart_b_rate
		

L4:		
		POPM	ST1
		POPM	ST0
		POPM	AG
		POPM	AH
		POPM	AL
		FRET

