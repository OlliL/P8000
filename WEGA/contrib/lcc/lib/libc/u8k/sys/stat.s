	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_stat::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	xor	r4,r4
	sc	#18			  /*stat*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
