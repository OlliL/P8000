	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_close::
	{
	ld	r0,SP(#4)
	xor	r4,r4
	sc	#6			  /*close*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
