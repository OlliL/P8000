	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_getppid::
	{
	sc	#20			  /*getpid*/
	ld	r2,r5
	ret
	}
