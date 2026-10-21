	.extern	_access, _getpid
	.psec
	.code
_mktemp::
	dec	r15,#16
	ldm	%000A(r15),r12,#3
	ld	@r15,r7
	call	_getpid
	ld	r13,r2
	ld	r14,%0000(r15)
L0014:
	ld	r2,r14
	inc	r14,#1
	testb	@r2
	jr	ne,L0014
	dec	r14,#1
	jr	L0038
L0020:
	ld	r3,r13
	ldk	r2,#0
	div	rr2,#%000A
	add	r2,#%0030
	ldb	@r14,rl2
	ld	r3,r13
	ldk	r2,#0
	div	rr2,#%000A
	ld	r13,r3
L0038:
	dec	r14,#1
	ldb	rl2,@r14
	cpb	rl2,#%58
	jr	eq,L0020
	inc	r14,#1
	ld	r12,#%0061
	jr	L005c
L004a:
	cp	r12,#%007A
	jr	ne,L0056
	ld	r2,#D0000
	jr	L0070
L0056:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r12,#1
L005c:
	ld	r7,%0000(r15)
	sub	r6,r6
	call	_access
	cp	r2,#%FFFF
	jr	ne,L004a
	ld	r2,%0000(r15)
L0070:
	ldm	r12,%000A(r15),#3
	inc	r15,#16
	ret
	.psec
	.data
D0000:
	.byte	%2f,%00
