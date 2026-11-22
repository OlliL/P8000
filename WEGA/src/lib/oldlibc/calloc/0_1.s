	.extern	_malloc, _free
	.psec
	.code
_calloc::
	dec	r15,#10
	ldm	%0004(r15),r12,#3
	ldl	%0000(r15),rr6
	ld	r3,%0002(r15)
	mult	rr2,%0000(r15)
	ld	%0002(r15),r3
	ld	r7,r3
	call	_malloc
	ld	r14,r2
	test	r2
	jr	ne,L0026
	sub	r2,r2
	jr	L0044
L0026:
	ld	r13,r14
	ld	r2,%0002(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r12,r2
	jr	L003a
L0036:
	clr	@r13
	inc	r13,#2
L003a:
	dec	r12,#1
	cp	r12,#%0000
	jr	ge,L0036
	ld	r2,r14
L0044:
	ldm	r12,%0004(r15),#3
	inc	r15,#10
	ret
_cfree::
	dec	r15,#6
	ldm	%0000(r15),r5,#3
	call	_free
	inc	r15,#6
	ret
