	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_mkseg::
	{
	ldl	rr0,SP(#4)
	sc	#55			  /*unused*/
	ldl	rr2,rr4
	jp	c,cerror
	ret
	}
