	.seg

	sp := r15
	SP := rr14

	.psec
	.code
_stty::
	{
	ld	r4,SP(#4)
	ld	r5,#%7409
	ldl	rr6,SP(#6)
	dec	sp,#8
	ldm	@SP,r4,#4
	call	_ioctl
	inc	sp,#8
	ret
	}
_gtty::
	{
	ld	r4,SP(#4)
	ld	r5,#%7408
	ldl	rr6,SP(#6)
	dec	sp,#8
	ldm	@SP,r4,#4
	call	_ioctl
	inc	sp,#8
	ret
	}
