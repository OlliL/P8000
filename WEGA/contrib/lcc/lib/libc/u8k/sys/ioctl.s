	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_ioctl::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	subl	rr4,rr4
	sc	#54			  /*ioctl*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
