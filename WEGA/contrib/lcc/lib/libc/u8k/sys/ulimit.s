	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_ulimit::
	{
	ldm	r0,|_stkseg+4|(sp),#3
	subl	rr4,rr4
	sc	#65			  /*lt*/
	ldl	rr2,rr4
	ret	nc
	jp	cerror
	}
