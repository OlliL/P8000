	.seg

	sp := r15
	SP := rr14

	.psec data
	.code
_stime::
	{
	ldl	rr0,SP(#4)
	xor	r4,r4
	sc	#25			  /*stime*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
