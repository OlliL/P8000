	.extern	__endopen, _fclose
	.psec
	.code
_freopen::
	dec	r15,#6
	ld	%0004(r15),r14
	ldl	%0000(r15),rr6
	ld	r14,r5
	ld	r7,r5
	call	_fclose
	ld	r7,%0002(r15)
	ld	r6,%0000(r15)
	ld	r5,r14
	call	__endopen
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
