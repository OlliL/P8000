	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_write::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	sc	#4			  /*write*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
