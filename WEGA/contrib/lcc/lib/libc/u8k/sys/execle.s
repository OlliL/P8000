	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_execle::
	{
	ldl	rr0,SP(#4)		/* path */
	ldl	rr2,SP
	inc	r3,#8			/* argv */
	ldl	rr4,rr2
L1:
	ldl	rr6,@rr4
	inc	r5,#4
	cpl	rr6,#%00000000		/* if zero than rr4 == envp */
	jr	ne,L1
	sc	#59			/* exece */
	ld	r2,r4
	jp	cerror
	}
