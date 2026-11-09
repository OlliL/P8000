	.seg
	.extern	_stkseg
	.psec
	.code

_strncpy::
	sub	r15,#%001E
	ldm	_stkseg+20(r15),r9,#5
	ld	|_stkseg+0|(r15),r3
	ldl	rr10,rr4
	ldl	rr12,rr6
	ldl	|_stkseg+12|(r15),rr6
	ldk	r9,#0
	jr	Lb40a
Lb3e8:
	ldl	rr2,rr10
	inc	r11,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	testb	rl6
	jr	eq,Lb3fe
	jr	Lb408
Lb3fa:
	clrb	@rr12
	inc	r13,#1
Lb3fe:
	inc	r9,#1
	cp	r9,|_stkseg+0|(r15)
	jr	lt,Lb3fa
	jr	Lb410
Lb408:
	inc	r9,#1
Lb40a:
	cp	r9,|_stkseg+0|(r15)
	jr	lt,Lb3e8
Lb410:
	ldl	rr2,|_stkseg+12|(r15)
	ldm	r9,_stkseg+20(r15),#5
	add	r15,#%001E
	ret
