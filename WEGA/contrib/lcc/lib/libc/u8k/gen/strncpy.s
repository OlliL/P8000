	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec
	.code
_strncpy::
	{
	ldm	r2,|_stkseg+4|(sp),#5	// rr2 <-> s1 , rr4 <-> s2 , r6 <-> n
~while:
	dec	r6,#1
	jpr	lt,~out
	ldb	rl7,@rr4
	ldb	@rr2,rl7
	inc	r5,#1
	inc	r3,#1
	testb	rl7
	jr	nz,~while
~while1:
	dec	r6,#1
	jpr	lt,~out
	clrb	@rr2
	inc	r3,#1
	jpr	~while1
~out:
	ldl	rr2,SP(#4)
	ret
	}
