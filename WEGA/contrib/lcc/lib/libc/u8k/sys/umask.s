	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_umask::
	{
	ld	r0,SP(#4)
	sc	#60			  /*umask*/
	ld	r2,r4
	ret
	}
