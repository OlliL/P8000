//
// memset - set an array of nchars starting at s1 to the character c.
//	    Return s1.
//
// Input:	s1 - destination
//		 c - initializing character
//		 n - number of bytes to copy
//
	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_memset::
	{
	ldm	r2,|_stkseg+4|(sp),#4	// rr2 = s1
					// r4  = c
					// r5  = n
	cp	r5,#0
	ret	le
	ldb	@rr2,rl4
	dec	r5,#1
	ret	z
	ldl	rr6,rr2			// destination
	inc	r7,#1
	ldl	rr0,rr2
	ldirb	@rr6,@rr2,r5
	ldl	rr2,rr0
	ret
	}
