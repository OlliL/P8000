	.seg

	sp := r15

	.psec
	.code
_chown::
	{
	ldm	r0,|_stkseg+4|(sp),#4 
	xor	r4,r4
	sc	#16			/* chown */
	ld	r2,r4
	ret	nc
	jp	cerror
	}
