	.seg

	sp := r15
	SP := rr14

	.psec data
	.data
.comm	2,	_errno;

	.psec
	.code
_ptrace::
	{
	xor	r4,r4
	ld	_errno,r4
	ldm	r0,|_stkseg+4|(sp),#5
	sc	#26			  /*ptrace*/
	ld	r2,r4
	ret	nc
	jp	cerror
	}
