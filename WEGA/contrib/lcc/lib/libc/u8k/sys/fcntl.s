	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_fcntl::
	{
	ldm	r0,|_stkseg+3|(sp),#4
	xor	r4,r4
	sc	#38			  // switch
	ld	r2,r4
	ret	nc
	jp	cerror
	}
