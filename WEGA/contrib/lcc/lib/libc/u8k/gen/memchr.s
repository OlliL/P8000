//
// memchr - Returns a pointer to the first occurrence of character c in
//	   the first n characters of memory area s1, or NULL if c does
//	   not occur
//
// Input:	s1 - memory area in which to conduct search
//		 c - character being sought
//		 n - maximum number of characters to search
//
// Output:	pointer to character 'c' or NULL
//
	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_memchr::
	{
	ldm	r2,|_stkseg+4|(sp),#4	// rr2 = s1
					// r4  = c
					// r5  = n
	cp	r5,#0
	jr	le,~ret0
	cpirb	rl4,@rr2,r5,eq
	jr	nz,~ret0
	dec	r3,#1
	ret
~ret0:					// return ((char *)0)
	subl	rr2,rr2
	ret
	}
