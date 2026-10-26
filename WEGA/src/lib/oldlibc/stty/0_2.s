	.extern	_ioctl
	.psec
	.code
_gtty::
	dec	r15,#4
	ldl	%0000(r15),rr6
	ld	r7,%0002(r15)
	ld	r6,#%7408
	ld	r5,%0000(r15)
	call	_ioctl
	inc	r15,#4
	ret
