	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_mdmctl::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	sc	#62			  /*unused*/
	ret	nc
	jp	cerror
	}
