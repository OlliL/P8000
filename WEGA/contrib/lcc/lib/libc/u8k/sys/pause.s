	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_pause::
	{
	sc	#29			  /*pause*/
	ret
	}
