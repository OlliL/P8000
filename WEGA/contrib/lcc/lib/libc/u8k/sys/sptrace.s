	.seg

	sp := r15
	SP := rr14

	.psec data
	.data
.comm	2,_errno

	.psec
	.code
_sptrace::
	{
	xor	r0,r0
	ld	_errno,r0
	ldm	r0,|_stkseg+4|(sp),#5
	sc	#58			  /*reserved*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
