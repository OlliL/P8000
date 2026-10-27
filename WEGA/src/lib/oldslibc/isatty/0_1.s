	.seg
	.extern	_stkseg
	.extern	_f92ae
	.extern	_getpid
	.code

_isatty::
	ret
_f91ea::
	dec	r15,#10
	ld	|_stkseg+0|(r15),r7
	ldl	rr4,#%7F000002
	add	r5,r15
	ld	r6,r7
	call	_f92ae
	cp	r2,#%0000
	jr	ge,L920a
	sub	r2,r2
	jr	L920c
L920a:
	ldk	r2,#1
L920c:
	inc	r15,#10
	ret
	dec	r15,#16
	ldm	_stkseg+8(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	call	_getpid
