	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec
	.code
_strcat::
	{
	ldm	r4,|_stkseg+4|(sp),#4	// rr4 - s1
					// rr6 - s2
	ld	r1,#%fff6		// cnt = 65526
	xor	r0,r0
	cpirb	rl0,@rr4,r1,eq		// search for zero byte
	dec	r5,#1			// rr4 - addr of s1_zero byte
	ldl	rr2,rr6
	ld	r1,#%fff6
	cpirb	rl0,@rr2,r1,eq
	sub	r3,r7			// length of s2
	ldirb	@rr4,@rr6,r3
	ldl	rr2,|_stkseg+4|(sp)	// rr2 - s1
	ret
	}
