	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_dup2::
	{
	ldl	rr0,SP(#4)
	set	r0,#6
	sc	#41			  /*dup*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
