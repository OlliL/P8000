	.seg

	sp := r15
	SP := rr14

	.psec bss
	.bss
L1:
	.=.+80

	.psec
	.code
_signal::
	{
	ldm	r0,|_stkseg+4|(sp),#3
	cp	r0,#%0014
	jp	uge,cerror
	ld	r3,r0		/* signo */
	ld	r4,r1
	ld	r5,r2		/* new function */
	sla	r3,#2
	ldl	rr6,L1(r3)	/* old function */
	ldl	L1(r3),rr4
	cpl	rr4,#%00000000
	jr	eq,L2
	cpl	rr4,#%00000001
	jr	eq,L2
	sra	r3
	lda	rr4,L5(r3)	/* intermediate function */
	ld	r1,r4
	ld	r2,r5
L2:
	sc	#48		/* sig */
	bit	r5,#0		/* SIG_IGN if on */
	jr	z,L3
	ldl	rr2,rr4
	jr	L4
L3:
	ldl	rr2,rr6
L4:
	ret	nc
	jp	cerror
L5:
	calr	L7
L6:
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
L7:
	sub	sp,#28
	ldm	@SP,r0,#14
	ld	r7,SP(#30)	/* return pc offset */
	ldl	rr4,#L6
	sub	r7,r5
	sla	r7		/* 4*signo */
	ldl	rr2,L1(r7)
	srl	r7,#2
	push	@SP,r7
	call	@rr2
	inc	sp,#2
	ldm	r0,@SP,#14
	add	sp,#32
	sc	#0		/* sysiret */
	}
