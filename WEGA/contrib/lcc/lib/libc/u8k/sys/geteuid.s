	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_geteuid::
	{
	sc	#24			  /*getuid*/
	ld	r2,r5
	ret
	}
