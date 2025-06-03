		.title "v.xx vtext section relocation"
		.mmregs

		.global vtext_load
		.global vcoefs_load
		.global vtext_v32_load
		.global vcoefs_v32_load
		.global vtext_v22_load
		.global vcoefs_v22_load
;		.global vtext_v21_load
;		.global vtext_v27_load
;		.global vcoefs_v27_load
;		.global vtext_v29_load
;		.global vcoefs_v29_load
		.global vtext_fax_load
		.global vcoefs_fax_load
		
		
		.sect	"vtext"
		.label	vtext_load
		.sect	"vcoefs"
		.label	vcoefs_load
		.sect	"vtext_v32"
		.label	vtext_v32_load
		.sect	"vcoefs_v32"
		.label	vcoefs_v32_load
		.sect	"vtext_v22"
		.label	vtext_v22_load
		.sect	"vcoefs_v22"
		.label	vcoefs_v22_load
;		.sect	"vtext_v21"
;		.label	vtext_v21_load
;		.sect	"vtext_v27"
;		.label	vtext_v27_load
;		.sect	"vcoefs_v27"
;		.label	vcoefs_v27_load
;		.sect	"vtext_v29"
;		.label	vtext_v29_load
;		.sect	"vcoefs_v29"
;		.label	vcoefs_v29_load
		.sect	"vtext_fax"
		.label	vtext_fax_load
		.sect	"vcoefs_fax"
		.label	vcoefs_fax_load
		.sect   "vtext_pump"
		.label  vtext_pump_load



		.text
		.global _v32reloc
		.global _v22reloc
		.global _v21reloc
		.global _v27reloc
		.global _v29reloc
		.global _pumpreloc

_v32reloc:
		LDX		#copyV32, 16, A
		OR		#copyV32, A, A
        FCALL	copy_section_init
        FCALL	copy_common_sect_init
        FRET

_v22reloc:
		LDX		#copyV22, 16, A
		OR		#copyV22, A, A
        FCALL	copy_section_init
        FCALL	copy_common_sect_init
        FRET

_v21reloc:
;		LDX		#copyV21, 16, A
;		OR		#copyV21, A, A
;        FCALL	copy_section_init
;        FCALL	copy_common_sect_init
;		FRET
		
_v27reloc:
;		LDX		#copyV27, 16, A
;		OR		#copyV27, A, A
;        FCALL	copy_section_init
;        FCALL	copy_common_sect_init
;        FRET
        
_v29reloc:
;		LDX		#copyV29, 16, A
;		OR		#copyV29, A, A
;		FCALL	copy_section_init
;        FCALL	copy_common_sect_init
;        FRET
		LDX		#copyVfax, 16, A
		OR		#copyVfax, A, A
		FCALL	copy_section_init
        FCALL	copy_common_sect_init
        FRET
        
_pumpreloc:
;		LDX		#copyV29, 16, A
;		OR		#copyV29, A, A
;		FCALL	copy_section_init
;        FCALL	copy_common_sect_init
;        FRET
		LDX		#copypump, 16, A
		OR		#copypump, A, A
		FCALL	copy_section_init
        FRET

copy_common_sect_init:
		LDX		#copyV, 16, A
		OR		#copyV, A, A
		
copy_section_init:
		RSBX	SXM	
        STM		#12h, AR4

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
		
		fret     ; Return to caller function.

		nop
		nop
		nop

		.ref	vtext_start, vtext_len
		.ref	vcoefs_start, vcoefs_len
		.ref	vtext_v32_start, vtext_v32_len
		.ref	vcoefs_v32_start, vcoefs_v32_len
		.ref	vtext_v22_start, vtext_v22_len
		.ref	vcoefs_v22_start, vcoefs_v22_len
;		.ref	vtext_v21_start, vtext_v21_len
;		.ref	vtext_v27_start, vtext_v27_len
;		.ref	vcoefs_v27_start, vcoefs_v27_len
;		.ref	vtext_v29_start, vtext_v29_len
;		.ref	vcoefs_v29_start, vcoefs_v29_len
		.ref	vtext_fax_start, vtext_fax_len
		.ref	vcoefs_fax_start, vcoefs_fax_len
		.ref    vtext_pump_start, vtext_pump_len


		.align 4
copyV:		.long	vtext_load
			.word	vtext_start
			.word	vtext_len
			.long	vcoefs_load
			.word	vcoefs_start
			.word	vcoefs_len
			.long	0
			.word	0
			.word	0		
copyV32:	.long	vtext_v32_load
			.word	vtext_v32_start
			.word	vtext_v32_len
			.long	vcoefs_v32_load
			.word	vcoefs_v32_start
			.word	vcoefs_v32_len
			.long	0
			.word	0
			.word	0		
copyV22:	.long	vtext_v22_load
			.word	vtext_v22_start
			.word	vtext_v22_len
			.long	vcoefs_v22_load
			.word	vcoefs_v22_start
			.word	vcoefs_v22_len
			.long	0
			.word	0
			.word	0		
;copyV21:	.long	vtext_v21_load
;			.word	vtext_v21_start
;			.word	vtext_v21_len
;			.long	0
;			.word	0
;			.word	0		
;copyV27:	.long	vtext_v27_load
;			.word	vtext_v27_start
;			.word	vtext_v27_len
;			.long	vcoefs_v27_load
;			.word	vcoefs_v27_start
;			.word	vcoefs_v27_len
;			.long	0
;			.word	0
;			.word	0		
;copyV29:	.long	vtext_v29_load
;			.word	vtext_v29_start
;			.word	vtext_v29_len
;			.long	vcoefs_v29_load
;			.word	vcoefs_v29_start
;			.word	vcoefs_v29_len
;			.long	0
;			.word	0
;			.word	0		
copyVfax:	.long	vtext_fax_load
			.word	vtext_fax_start
			.word	vtext_fax_len
			.long	vcoefs_fax_load
			.word	vcoefs_fax_start
			.word	vcoefs_fax_len
			.long	0
			.word	0
			.word	0
copypump:	.long	vtext_pump_load
			.word	vtext_pump_start
			.word	vtext_pump_len
			.long	0
			.word	0
			.word	0
			.long	0
			.word	0
			.word	0			
		.end

