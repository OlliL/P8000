	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_time::
	{
	sc	#13			  /*time*/
	ldl	rr2,rr4
	ldl	rr6,SP(#4)
	testl	rr6
	ret	z
	ldl	@rr6,rr2
	ret
	}
