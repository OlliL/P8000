	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_ustat::
	{
	ldm	r0,|_stkseg+4|(sp),#3
	ldk	r3,#2
	sc	#57			  /*reserved*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
