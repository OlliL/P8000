	.seg

	fp := r13
	FP := rr12
	sp := r15
	SP := rr14

	.psec data
	.data
	.=.+4			/* for NULL pointer */
	.comm	4,	_environ;

	.psec
	.code
start::
	{
	addb	rh0,#0		// for NULL pointer
	ld	r7,@SP		/* argc */
	dec	sp,#8
	ld	@SP,r7
	ldl	rr4,SP
	inc	r5,#10		/* argv */
/*	ldl	SP(#2),rr4	*/
	ldl	|_stkseg+2|(sp),rr4
	ldl	rr2,@rr4	/* addr of fst arg string */
	dec	r3,#4
	xor	r0,r0
	ld	r1,r0
	ldl	@rr2,rr0
	inc	r7
	sla	r7,#2		/* (argc+1)*2 */
	add	r5,r7
	ldl	_environ,rr4
	ldl	SP(#6),rr4
	ldl	FP,SP
	call	_main
	push	@SP,r2
	call	_exit
	pop	r0,@SP
	sc	#1		/* exit */
	}
	.extern	_exit
