	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_ssgbrk::
	{
	ldl	rr0,SP(#4)
	subl	rr4,rr4
	sc	#45			  /*unused*/
	ldl	rr2,rr4
	jp	c,cerror
	ret
	}
