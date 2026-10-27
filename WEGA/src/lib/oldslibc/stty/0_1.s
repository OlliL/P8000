	.seg
	.extern	_stkseg
	.extern	_signal
	.code

_stty::
	dec	r15,#10
	ldm	|_stkseg+0|(r15),r4,#4
	ld	r6,#%7409
	ldl	rr2,rr4
	call	_signal
	inc	r15,#10
	ret
_f92ae::
	dec	r15,#10
	ldm	|_stkseg+0|(r15),r4,#4
	ld	r6,#%7408
	ldl	rr2,rr4
	call	_signal
	inc	r15,#10
	ret
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
