	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_access::
	{
	ldm	r0,|_stkseg+4|(sp),#3
	xor	r4,r4
	sc	#33			  /*access*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
