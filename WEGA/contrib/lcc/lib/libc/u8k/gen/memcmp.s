//
// memcmp - compares its arguments, looking at the first n characters only,
//	   and returns an integer less than, equal to, or greater than 0,
//	   according as s1 is lexicographically less than, equal to, or 
//	   greater than s2.
//
// Input:	s1 and s2 - the strings to compare
//		n - maximum number of bytes to compare
//
// Output:	0 if s1 == s2
//		<0 if s1 < s2
//		>0 if s1 > s2
//
	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_memcmp::
	{
	ldm	r2,|_stkseg+4|(sp),#5	// rr2 = s1
					// rr4 = s2
					// r6  = n
	cp	r7,#0
	jr	le,~reteq
	cpsirb	@rr2,@rr4,r6,ne
	jr	z,~noteq
~reteq:					// return(0) (s1 == s2)
	xor	r2,r2
	ret
~noteq:
	jr	ult,~retlt
~retgt:					// return(1) (s1 > s2)
	ldk	r2,#1
	ret
~retlt:					// return(-1) (s1 < s2)
	ld	r2,#-1
	ret
	}
