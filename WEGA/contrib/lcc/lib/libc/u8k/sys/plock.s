	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_plock::
	{
	ld	r0,SP(#4)
	xor	r4,r4
	sc	#53			  /*syslock*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
