//
// memccpy - copies characters from memory area s2 into s1, stopping after
//	    the first occurrence of character c has been copied, or after
//	    n characters have been copied, whichever comes first. It 
//	    returns a pointer to the character after the copy of c in s1
//	    or a NULL pointer if c was not found in the first n characters
//	    of s2.
//
	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_memccpy::
	{
	ldm	r2,|_stkseg+4|(sp),#6	// rr2 = s1
					// rr4 = s2
					// r6  = c
					// r7  = n
	cp	r7,#0
	jr	le,~ret00
	ldl	rr0,rr4			// save s2
	cpirb	rl6,@rr4,r7,eq
	jr	nz,~ret0
	subl	rr4,rr0			// new byte count
	ld	r7,r5
	ldl	rr4,rr0
	ldirb	@rr2,@rr4,r7
	ret
~ret0:
	ldl	rr4,rr0
	ld	r7,|_stkseg+14|(sp)
	ldirb	@rr2,@rr4,r7
~ret00:					// return ((char *)0)
	subl	rr2,rr2
	ret
	}
