	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_wait::
	{
	sc	#7			  /*wait*/
	ldl	rr2,rr4
	jp	c,cerror
	ldl	rr6,SP(#4)
	testl	rr6
	ret	z
	ld	@rr6,r3
	ret
	}
