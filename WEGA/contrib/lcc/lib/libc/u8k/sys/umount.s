	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_umount::
	{
	ld	r2,#%0016
	ldl	rr0,SP(#4)
	xor	r4,r4
	sc	#22			  /*umount*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
