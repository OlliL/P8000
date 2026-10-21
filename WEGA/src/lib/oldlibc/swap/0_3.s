	.psec
	.code
_swap::
	dec	r15,#6
	ld	%0004(r15),r14
	ld	%0000(r15),r7
	lda	r2,%0002(r15)
	ld	r14,r2
	ldb	rl2,%0001(r15)
	ldb	@r14,rl2
	ldb	rl2,%0000(r15)
	ldb	%0001(r14),rl2
	ld	r2,@r14
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
_longswap::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	%0000(r15),r7
	ld	r14,r7
	test	@r14
	jr	eq,L003e
	ld	r7,@r14
	calr	_swap
	ld	@r14,r2
L003e:
	inc	r14,#2
	test	@r14
	jr	eq,L004a
	ld	r7,@r14
	calr	_swap
	ld	@r14,r2
L004a:
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
	dec	r15,#4
	ld	%0002(r15),r14
	ld	%0000(r15),r7
	ld	r14,r7
	test	%0004(r14)
	jr	eq,L006e
	ld	r7,%0004(r14)
	calr	_swap
	ld	%0004(r14),r2
L006e:
	test	%0006(r14)
	jr	eq,L007e
	ld	r7,%0006(r14)
	calr	_swap
	ld	%0006(r14),r2
L007e:
	test	%0008(r14)
	jr	eq,L008e
	ld	r7,%0008(r14)
	calr	_swap
	ld	%0008(r14),r2
L008e:
	test	%000A(r14)
	jr	eq,L009e
	ld	r7,%000A(r14)
	calr	_swap
	ld	%000A(r14),r2
L009e:
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
