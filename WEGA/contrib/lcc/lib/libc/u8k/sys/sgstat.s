	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_sgstat::
	{
	ldl	rr0,SP(#4)
	xor	r4,r4
	sc	#56			  /*mpxchan*/
	ld	r2,r4
	jp	c,cerror
	ret
	}
