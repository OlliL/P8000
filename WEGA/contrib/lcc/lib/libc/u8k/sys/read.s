	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_read::
	{
	ldm	r0,|_stkseg+4|(sp),#4		// fildes, buf, nbyte
	sc	#3			  /*read*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
