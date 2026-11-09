	.seg
	.extern	_stkseg
	.psec
	.code

_strcpy::
	sub	r15,#%001C
	ldm	_stkseg+16(r15),r8,#6
	ldl	rr10,rr4
	ldl	rr12,rr6
	ldl	rr8,rr6
Lb3ac:
	ldl	rr2,rr10
	inc	r11,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	testb	rl6
	jr	ne,Lb3ac
	ldl	rr2,rr8
	ldm	r8,_stkseg+16(r15),#6
	add	r15,#%001C
	ret
