	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_sync::
	{
	sc	#36			  /*sync*/
	ret
	}
