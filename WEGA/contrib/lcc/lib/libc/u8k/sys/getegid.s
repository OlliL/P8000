	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_getegid::
	{
	sc	#47			  /*getgid*/
	ld	r2,r5
	ret
	}
