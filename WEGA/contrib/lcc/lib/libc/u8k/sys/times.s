	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_times::
	{
	ld	r2,#%0016
	ldl	rr0,SP(#4)
	subl	rr4,rr4
	sc	#43			  /*times*/
	ldl	rr2,rr4
	ret	nc
	jp	cerror
	}
