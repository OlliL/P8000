	.seg
	.extern	_stkseg
	.extern	_ioctl
	.code

_stty::
	dec	r15,#10
	ldm	|_stkseg+0|(r15),r4,#4
	ld	r6,#%7409
	ldl	rr2,rr4
	call	_ioctl
	inc	r15,#10
	ret
_gtty::
	dec	r15,#10
	ldm	|_stkseg+0|(r15),r4,#4
	ld	r6,#%7408
	ldl	rr2,rr4
	call	_ioctl
	inc	r15,#10
	ret
