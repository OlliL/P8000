	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_sgbrk::
	{
	ldl	rr0,SP(#4)
	xor	r4,r4
	sc	#17			  /*break*/
	ldl	rr2,rr4
	jp	c,cerror
	ret
	}
