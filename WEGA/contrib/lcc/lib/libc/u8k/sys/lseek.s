	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_lseek::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	subl	rr4,rr4
	sc	#19			  /*seek*/
	ldl	rr2,rr4
	ret	nc
	jp	cerror
	}
