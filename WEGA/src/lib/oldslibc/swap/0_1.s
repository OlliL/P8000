	.seg
	.extern	_stkseg
	.code

_swap::
L946e:
	dec	r15,#10
	ld	|_stkseg+0|(r15),r7
	ldl	rr2,#%7F000002
	add	r3,r15
	ldl	rr4,rr2
	ldb	rl6,|_stkseg+1|(r15)
	ldb	@rr4,rl6
	ldl	rr2,rr4
	inc	r3,#1
	ldb	rl6,|_stkseg+0|(r15)
	ldb	@rr2,rl6
	ld	r2,@rr4
	inc	r15,#10
	ret
_longswap::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,|_stkseg+0|(r15)
	test	@rr12
	jr	eq,L94b0
	ld	r7,@rr12
	calr	L946e
	ld	@rr12,r2
L94b0:
	inc	r13,#2
	test	@rr12
	jr	eq,L94bc
	ld	r7,@rr12
	calr	L946e
	ld	@rr12,r2
L94bc:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
_swapsegt::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	ld	r2,rr12(#%0004)
	test	r2
	jr	eq,L94ea
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
L94ea:
	ld	r2,rr12(#%0006)
	test	r2
	jr	eq,L94fc
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#6
	ld	@rr4,r2
L94fc:
	ld	r2,rr12(#%0008)
	test	r2
	jr	eq,L950e
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#8
	ld	@rr4,r2
L950e:
	ld	r2,rr12(#%000A)
	test	r2
	jr	eq,L9520
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#10
	ld	@rr4,r2
L9520:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
