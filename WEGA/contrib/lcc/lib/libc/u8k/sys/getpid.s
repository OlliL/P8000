	.seg

	sp := r15
	SP := rr14

	.psec data
	.data

	.psec
	.code
_getpid::
	{
	sc	#20			  /*getpid*/
	ld	r2,r4
	ret
	}
