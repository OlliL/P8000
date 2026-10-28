	.seg
	.extern	_stkseg
	.extern	_gtty
	.code

_isatty::
	dec	r15,#10
	ld	|_stkseg+0|(r15),r7
	ldl	rr4,#%7F000002
	add	r5,r15
	ld	r6,r7
	call	_gtty
	cp	r2,#%0000
	jr	ge,L920a
	sub	r2,r2
	jr	L920c
L920a:
	ldk	r2,#1
L920c:
	inc	r15,#10
	ret
