	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec	data
	.data

	.psec
	.code
_strcmp::
	{
	ldm	r4,|_stkseg+4|(sp),#4	// rr4 <-> s1 , rr6 <-> s2
	cpl	rr4,rr6
	jr	eq,~ret0
~while:	ldl	rr2,rr6
	inc	r7
	ldb	rl0,@rr2
	cpb	rl0,@rr4
	jr	ne,~L2
	ldl	rr2,rr4
	inc	r5
	ldb	rl0,@rr2
	testb	rl0
	jr	nz,~while
~ret0:
	xor	r2,r2
	ret
~L2:	dec	r7
	ldb	rl3,@rr6
	ldb	rl2,@rr4
	subb	rl2,rl3
	extsb	r2
	ret
	}
