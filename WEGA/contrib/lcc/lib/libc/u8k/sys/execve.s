	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_execve::
	{
	ldm	r0,|_stkseg+4|(sp),#6 
	sc	#59			  /*exece*/
	ld	r2,r4
	jp	cerror
	}
