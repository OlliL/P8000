	.seg
	.extern	_stkseg
	.code

_strrchr::
	sub	r15,#%0012
	ldm	_stkseg+10(r15),r10,#4
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	subl	rr10,rr10
L930c:
	ldb	rl2,@rr12
	cpb	rl2,|_stkseg+1|(r15)
	jr	ne,L9316
	ldl	rr10,rr12
L9316:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L930c
	ldl	rr2,rr10
	ldm	r10,_stkseg+10(r15),#4
	add	r15,#%0012
	ret
