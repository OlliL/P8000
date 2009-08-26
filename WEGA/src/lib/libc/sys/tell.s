	.psec
	.code
_tell::
	{
	dec	r15,#6
	ld	@r15,r7
	ld	r6,r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	inc	r15,#6
	ret
	}
