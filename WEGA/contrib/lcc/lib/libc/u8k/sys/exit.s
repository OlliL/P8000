	.seg

	sp := r15
	SP := rr14

	.psec
	.code
__exit::
	{
	ld	r0,SP(#4)
	sc	#1			  /*exit*/
	ld	r2,r4
	}
