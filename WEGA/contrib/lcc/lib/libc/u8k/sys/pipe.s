	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_pipe::
	{
	sc	#42			  /*pipe*/
	ld	r2,r4
	jp	c,cerror
	ldl	rr6,SP(#4)
	ldl	@rr6,rr4
	xor	r2,r2
	ret
	}
