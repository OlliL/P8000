	.seg
	.extern	_stkseg
	.code

_strcpy::
	sub	r15,#%0018
	ldm	_stkseg+12(r15),r8,#6
	ldl	rr10,rr4
	ldl	rr12,rr6
	ldl	rr8,rr6
L944e:
	ldl	rr2,rr10
	inc	r11,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	testb	rl6
	jr	ne,L944e
	ldl	rr2,rr8
	ldm	r8,_stkseg+12(r15),#6
	add	r15,#%0018
	ret
