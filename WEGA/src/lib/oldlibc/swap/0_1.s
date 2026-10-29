	.psec
	.code
_swap::
	dec	r15,#10
	ld	%0008(r15),r14
	ld	%0000(r15),r7
	lda	r14,%0002(r15)
	ldb	rl2,%0001(r15)
	ldb	@r14,rl2
	ldb	rl2,%0000(r15)
	ldb	%0001(r14),rl2
	ld	r2,@r14
	ld	r14,%0008(r15)
	inc	r15,#10
	ret
_longswap::
	dec	r15,#6
	ld	%0004(r15),r14
	ld	@r15,r7
	ld	r14,%0000(r15)
	test	@r14
	jr	eq,L003c
	ld	r7,@r14
	calr	_swap
	ld	@r14,r2
L003c:
	inc	r14,#2
	test	@r14
	jr	eq,L0048
	ld	r7,@r14
	calr	_swap
	ld	@r14,r2
L0048:
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
_swap_quad::
	dec	r15,#6
	ld	%0004(r15),r14
	ld	@r15,r7
	ld	r14,r7
	test	%0004(r14)
	jr	eq,L006a
	ld	r7,%0004(r14)
	calr	_swap
	ld	%0004(r14),r2
L006a:
	test	%0006(r14)
	jr	eq,L007a
	ld	r7,%0006(r14)
	calr	_swap
	ld	%0006(r14),r2
L007a:
	test	%0008(r14)
	jr	eq,L008a
	ld	r7,%0008(r14)
	calr	_swap
	ld	%0008(r14),r2
L008a:
	test	%000A(r14)
	jr	eq,L009a
	ld	r7,%000A(r14)
	calr	_swap
	ld	%000A(r14),r2
L009a:
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
