	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_mknod::
	{
	ldm	r0,|_stkseg+4|(sp),#4
	xor	r4,r4
	sc	#14			  /*mknod*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
