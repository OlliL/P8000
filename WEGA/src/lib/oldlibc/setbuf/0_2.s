	.extern	_free
	.psec
	.code
_setbuf::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	%0000(r15),r6
	ld	r14,r7
	test	%0004(r14)
	jr	eq,L0024
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#3
	jr	eq,L0024
	ld	r7,%0004(r14)
	call	_free
L0024:
	ldb	rl2,%0006(r14)
	andb	rl2,#%F3
	ldb	%0006(r14),rl2
	ld	r2,%0000(r15)
	ld	%0004(r14),r2
	test	r2
	jr	ne,L0042
	setb	%0006(r14),#2
	jr	L0048
L0042:
	ld	r2,%0004(r14)
	ld	@r14,r2
L0048:
	clr	%0002(r14)
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
