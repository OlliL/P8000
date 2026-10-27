	.seg
	.extern	_stkseg
	.extern	_f952c
	.code

_mktemp::
	ld	r11,r2
	ldl	rr12,|_stkseg+0|(r15)
L922a:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L922a
	dec	r13,#1
	jr	L924e
L9236:
	ld	r3,r11
	ldk	r2,#0
	div	rr2,#%000A
	add	r2,#%0030
	ldb	@rr12,rl2
	ld	r3,r11
	ldk	r2,#0
	div	rr2,#%000A
	ld	r11,r3
L924e:
	dec	r13,#1
	ldb	rl2,@rr12
	cpb	rl2,#%58
	jr	eq,L9236
	inc	r13,#1
	ld	r10,#%0061
	jr	L9274
L9260:
	cp	r10,#%007A
	jr	ne,L926e
	ldl	rr2,#%01001AAA
	jr	L928a
L926e:
	ld	r2,r10
	ldb	@rr12,rl2
	inc	r10,#1
L9274:
	ldl	rr6,|_stkseg+0|(r15)
	sub	r5,r5
	call	_f952c
	cp	r2,#%FFFF
	jr	ne,L9260
	ldl	rr2,|_stkseg+0|(r15)
L928a:
	ldm	r10,_stkseg+8(r15),#4
	inc	r15,#16
	ret
