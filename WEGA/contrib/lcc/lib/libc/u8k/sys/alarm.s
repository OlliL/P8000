	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_alarm::
	{
	ld	r0,SP(#4)
	sc	#27			  /*alarm*/
	ld	r2,r4
	ret
	}
