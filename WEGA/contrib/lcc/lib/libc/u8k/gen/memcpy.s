//
// memcpy - copies n characters from s2 to s1, and returns s1
//
	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_memcpy::
	{
	ldm	r2,|_stkseg+4|(sp),#5	// rr2 = s1
					// rr4 = s2
					// r6  = n
	cp	r6,#0
	ret	le
	ldl	rr0,rr2			// save s1
	ldirb	@rr2,@rr4,r6
	ldl	rr2,rr0			// return s1
	ret
	}
