	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_getgid::
	{
	sc	#47			  /*getgid*/
	ld	r2,r4
	ret
	}
