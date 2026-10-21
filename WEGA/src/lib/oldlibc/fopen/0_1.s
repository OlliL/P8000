	.extern	__endopen, __findiop
	.psec
	.code
_fopen::
	dec	r15,#6
	ldl	%0000(r15),rr6
	call	__findiop
	ld	%0004(r15),r2
	ld	r7,%0002(r15)
	ld	r6,%0000(r15)
	ld	r5,r2
	call	__endopen
	inc	r15,#6
	ret
