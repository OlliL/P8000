	.seg

	sp := r15
	SP := rr14

	.psec data
	.data
_errno::
	.word	%0
_deverr::
	.word	%0

	.psec
	.code
cerror::
	{
	ld	_errno,r2
	ld	_deverr,r5
	ldl	rr2,#%ffffffff
	ret
	}
