	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_execl::
	{
	ldl	rr0,SP(#4)		/* path */
	ldl	rr2,SP
	inc	r3,#8			/* argv */
	ldl	rr4,_environ
	sc	#59			/* exece */
	ld 	r2,r4
	jp	cerror
	}
