	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_uname::
	{
	ld	r2,#%0016
	ldl	rr0,SP(#4)
	xor	r3,r3
	sc	#57			  /*reserved*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
