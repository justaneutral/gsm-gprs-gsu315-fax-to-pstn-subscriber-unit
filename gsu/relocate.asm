		.title "Relocation"
		.mmregs
		.ref 	_c_int00
		

		.global	vectors_load
		.global	const_load
		.global flash_text_load
		
		.sect	".vectors"
		.label	vectors_load
		
		.sect	".const"
		.label	const_load
		
		.sect	".flash_text"
		.label	flash_text_load
		
		.sect	"pump"
		.label	pump_load
		
		.sect reloc_txt
		.global relocate

relocate:
	SSBX	INTM
	LD	#0, DP
	STM	#000e0h, PMST
	STM	#00c00h, ST0
	STM	#02900h, ST1 
	STM	#00000h, IMR
	STM	#0FFFFh, IFR
    STM #07276h, SWWSR
    STM #00002h, BSCR
    STM #00001h, 02bh ;SWCR

;processor preparation.
		SSBX	SXM			; turn on SXM for LD #cinit,A
		SSBX	CPL			; turn on compiler mode bit
		RSBX	OVM			; clear overflow mode bit
       	LD	#0,ARP
		RSBX	C16
		RSBX	CMPT
		RSBX	FRCT


		stm #0x7276,0x28
		stm #0x1, 0x2B
		stm #(0xff80 | 0x0040 | 0x0020), 0x1D
		stm	#0x0000,CLKMD
test_clk_status:
		ldm	CLKMD,A
		and	#0x0001,A
		bc	test_clk_status,ANEQ
		stm	#0xB197,CLKMD
		NOP
		NOP
		
		; clear RAM
		STM		#60h, AR1
		LD		#0, A
		RPT		#(10000h - 60h - 1)
		STL		A, *AR1+
		

		RSBX	SXM	
        STM		#12h, AR4

		LDX		#copyTable, 16, A
		OR		#copyTable, A, A

copy_section:
		STM		#60h, AR2
		STM		#60h, AR3
		RPT		#3			; 4 words
		READA	*AR2+
		
		LD		A, B		; save ACCA

		LD		*AR3+, 16, A
		OR		*AR3+, A	; ACCA has the source address
		
		BC		copy_end, AEQ
		
		MVDD	*AR3+, *AR4	; AR2 has the dest address
		
		RPT		*AR3
		READA	*AR2+
		
		LD		B, A		; restore ACCA
		ADD		#4, A		; move to the next entry
		
		FB		copy_section
								
copy_end:
		
		fbd		_c_int00     ; Br to entry point.

		nop
		nop
		nop

		.ref	vectors_start, vectors_len
		.ref	vtext_start, vtext_len
		.ref	const_start, const_len
		.ref	flsh_txt_start, flsh_txt_len
		.ref 	pump_start, pump_len
		.align 4
		
copyTable:
		; ".vectors"
		.long	vectors_load
		.word	vectors_start
		.word	vectors_len

		
		;	".const"
		.long	const_load
		.word	const_start
		.word	const_len
		
		;	.wrtflash_text
		.long	flash_text_load
		.word 	flsh_txt_start
		.word 	flsh_txt_len
		
		;   pump
		.long 	pump_load
		.word 	pump_start
		.word 	pump_len

		;   end
		.long	0
		.word	0
		.word	0
		
		.end

