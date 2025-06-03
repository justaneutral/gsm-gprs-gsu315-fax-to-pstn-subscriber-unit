	.if ($isdefed("CONTEXT_") = 0)
CONTEXT_    .set    1
	.mmregs
context_save	.macro	mask
		;ssbx	intm
        pshm    st0
        pshm    st1
        pshm    al
        pshm    ah
        pshm    ag
        pshm    bl
        pshm    bh
        pshm    bg
        pshm    t
        pshm    trn
        pshm    ar0
        pshm    ar1
        pshm    ar2
        pshm    ar3
        pshm    ar4
        pshm    ar5
        pshm    ar6
        pshm    ar7
        pshm    bk
        pshm    brc
        pshm    rsa
        pshm    rea
        pshm    pmst
        ssbx cpl
        ;rsbx	intm
	.endm

context_restore	.macro	mask
    	rsbx cpl
    	ld #0, dp
        popm    pmst
        popm    rea
        popm    rsa
        popm    brc
        popm    bk
        popm    ar7
        popm    ar6
        popm    ar5
        popm    ar4
        popm    ar3
        popm    ar2
        popm    ar1
        popm    ar0
        popm    trn
        popm    t
        popm    bg
        popm    bh
        popm    bl
        popm    ag
        popm    ah
        popm    al
        popm    st1
        popm    st0
        ;rsbx	intm
		frete
	.endm

	.endif          ; if CONTEXT_ is not defined


