	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_nice::
	{
	ld	r0,SP(#4)
	xor	r4,r4
	sc	#34			  /*nice*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
