	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_creat::
	{
	ldm	r0,|_stkseg+4|(sp),#3 
	.word	%5cf1
	.word	3-1
	.word	%7f04
	sc	#8			  /*creat*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
