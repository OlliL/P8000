	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_dup::
	{
	ldl	rr0,SP(#4)
	sc	#41			  /*dup*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
