	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_mount::
	{
	ldm	r0,|_stkseg+4|(sp),#5
	sc	#21			  /*mount*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
