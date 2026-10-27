	.seg
	.extern	_stkseg
	.extern	_malloc
	.extern	_f9610
	.code

_filbuf::
	ret
_f8688::
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
_f86fa::
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ldl	rr12,rr6
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,L8716
	ldl	rr2,rr6
	inc	r3,#10
	setb	@rr2,#0
L8716:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,L872e
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#6
	jr	eq,L874e
L872e:
	ld	r2,#%FFFF
	jr	L8802
L8734:
	ld	r7,#%0200
	call	_malloc
	ldl	rr4,rr12
	inc	r5,#6
	ldl	@rr4,rr2
	testl	rr2
	jr	ne,L8776
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#2
L874e:
	ldl	rr2,rr12(#%0006)
	testl	rr2
	jr	ne,L877c
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	eq,L8734
	ldb	rl2,rr12(#%000B)
	extsb	r2
	ldl	rr4,#%0100192E
	add	r5,r2
	ldl	rr2,rr12
	inc	r3,#6
	ldl	@rr2,rr4
	jr	L874e
L8776:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#3
L877c:
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldb	rl3,rr12(#%000A)
	extsb	r3
	bit	r3,#2
	jr	eq,L8796
	ldk	r3,#1
	jr	L879a
L8796:
	ld	r3,#%0200
L879a:
	ldl	rr4,@rr12
	call	_f9610
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
	ldl	rr2,rr12
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L87e6
	ld	r2,rr12(#%0004)
	cp	r2,#%FFFF
	jr	ne,L87d8
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#4
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,L87de
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#0
	jr	L87de
L87d8:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
L87de:
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	L872e
L87e6:
	ldl	rr2,@rr12
	ldb	rl4,@rr2
	subb	rh4,rh4
	and	r4,#%00FF
	ld	_stkseg+4(r15),r4
	inc	r13,#2
	inc	@rr12,#1
	dec	r13,#2
	ld	r2,_stkseg+4(r15)
L8802:
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
_f880e::
	sub	r15,#%0018
	ldm	_stkseg+12(r15),r8,#6
	ld	|_stkseg+6|(r15),r7
