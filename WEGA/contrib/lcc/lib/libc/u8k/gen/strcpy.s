	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec
	.code
_strcpy::
	{
	ldm	r4,|_stkseg+4|(sp),#4	// rr4 - s1 , rr6 - s2
	ldl	rr2,rr6
	subl	rr0,rr0
	cpirb	rl0,@rr2,r1,eq
	neg	r1			// strlen(s2) + 1
	ldl	rr2,rr4			// return value s1
	ldirb	@rr4,@rr6,r1
	ret
	}
