	.seg
	.extern	_stkseg
	.code

_strchr::
	dec	r15,#14
	ldm	_stkseg+10(r15),r12,#2
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
L92d6:
	ldb	rl2,@rr12
	cpb	rl2,|_stkseg+1|(r15)
	jr	ne,L92e2
	ldl	rr2,rr12
	jr	L92ec
L92e2:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L92d6
	subl	rr2,rr2
L92ec:
	ldm	r12,_stkseg+10(r15),#2
	inc	r15,#14
	ret
