	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_getpgrp::
	{
	xor	r0,r0
	sc	#39			  /*setpgrp*/
	ld	r2,r4
	ret
	}
