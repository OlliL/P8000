	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_kill::
	{
	ldl	rr0,SP(#4)
	xor	r4,r4
	sc	#37			  /*kill*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
