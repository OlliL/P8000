	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_chmod::
	{
	ldm	r0,|_stkseg+4|(sp),#3
	xor	r4,r4
	sc	#15			  /*chmod*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
