	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_execv::
	{
	ldm	r0,|_stkseg+4|(sp),#4	/* path, argv */
	ldl	rr4,_environ
	sc	#59			/*exece*/
	ld	r2,r4
	jp	cerror
	}
