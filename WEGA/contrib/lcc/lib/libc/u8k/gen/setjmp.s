// C library -- setjmp, longjmp

//	longjmp(a,v)
// will	generate a "return(v)" from
// the last call to
//	setjmp(a)
// by restoring return_pc and r8-r15 from 'a'
// and doing a return.
//

	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_setjmp::
	{
	popl	rr6,@SP			// return addr
	ldl	rr4,@SP			// a
	ldm	@rr4,r6,#10
	xor	r2,r2			// return 0
	jp	@rr6
	}
_longjmp::
	{
	inc	sp,#4
	ldl	rr4,@SP			// a
	ld	r2,|_stkseg+4|(sp)	// v
	test	r2
	jr	nz,~L1
	ld	r2,#1			// force v non-zero
~L1:
	ldm	r6,@rr4,#10
	jp	@rr6		// return v but as if from setjmp
	}
