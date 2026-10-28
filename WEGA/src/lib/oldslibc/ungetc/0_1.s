	.seg
	.extern	_stkseg
	.code

_ungetc::
	dec	r15,#14
	ldm	_stkseg+10(r15),r12,#2
	ld	|_stkseg+6|(r15),r7
	ldl	rr12,rr4
	cp	r7,#%FFFF
	jr	eq,L86ea
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,L86b6
	ldl	rr2,rr4
	inc	r3,#6
	ldl	rr4,@rr12
	cpl	rr4,@rr2
	jr	ugt,L86ce
L86b6:
	ldl	rr2,rr12
	inc	r3,#6
	ldl	rr4,@rr12
	cpl	rr4,@rr2
	jr	ne,L86ea
	ld	r2,rr12(#%0004)
	test	r2
	jr	ne,L86ea
	inc	r13,#2
	inc	@rr12,#1
	dec	r13,#2
L86ce:
	ldl	rr2,rr12
	inc	r3,#4
	ldl	rr2,rr12
	inc	r3,#4
	inc	@rr2,#1
	inc	r13,#2
	dec	@rr12,#1
	dec	r13,#2
	ldl	rr2,@rr12
	ldb	rl4,|_stkseg+7|(r15)
	ldb	@rr2,rl4
	sub	r2,r2
	jr	L86ee
L86ea:
	ld	r2,#%FFFF
L86ee:
	ldm	r12,_stkseg+10(r15),#2
	inc	r15,#14
	ret
