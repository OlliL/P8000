	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec
	.code
_strlen::
	{
	ldl	rr4,|_stkseg+4|(sp)		// s
	xor	r2,r2
	ld	r1,r2
	cpirb	rl1,@rr4,r2,eq
	neg	r2
	dec	r2,#1
	ret
	}
