	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_lkdata::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	sc	#49			  /*reserved*/
	ldl	rr2,rr4
	ret	nc
	jp	cerror
	}
