	.psec
	.code
_ungetc::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	r14,r6
	cp	r7,#%FFFF
	jr	eq,Lc6d8
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,Lc6b8
	ld	r2,@r14
	cp	r2,%0004(r14)
	jr	ugt,Lc6c8
Lc6b8:
	ld	r2,@r14
	cp	r2,%0004(r14)
	jr	ne,Lc6d8
	test	%0002(r14)
	jr	ne,Lc6d8
	inc	@r14,#1
Lc6c8:
	inc	%0002(r14),#1
	ld	r2,r7
	dec	@r14,#1
	ld	r3,@r14
	ldb	@r3,rl2
	sub	r2,r2
	jr	Lc6dc
Lc6d8:
	ld	r2,#%FFFF
Lc6dc:
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
