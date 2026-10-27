	.seg
	.extern	_stkseg
	.code

_swab::
	ret
	sub	r15,#%0014
	ldm	_stkseg+10(r15),r9,#5
	ldl	rr10,rr4
	ldl	rr12,rr6
	exts	rr2
	div	rr2,#%0002
	ld	r9,r3
	jr	L9360
L9348:
	ld	r2,@rr12
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,@rr12
	ldb	rl3,rh3
	extsb	r3
	and	r3,#%00FF
	add	r2,r3
	ld	@rr10,r2
	inc	r11,#2
	inc	r13,#2
L9360:
	dec	r9,#1
	cp	r9,#%0000
	jr	ge,L9348
	ldm	r9,_stkseg+10(r15),#5
	add	r15,#%0014
	ret
	sub	r15,#%0012
	ldm	_stkseg+10(r15),r10,#4
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	subl	rr10,rr10
L938a:
	ldb	rl2,@rr12
	cpb	rl2,|_stkseg+1|(r15)
	jr	ne,L9394
	ldl	rr10,rr12
L9394:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L938a
	ldl	rr2,rr10
	ldm	r10,_stkseg+10(r15),#4
	add	r15,#%0012
	ret
	sub	r15,#%0018
	ldm	_stkseg+12(r15),r8,#6
	ldl	rr10,rr4
	ldl	rr12,rr6
	ldl	rr8,rr6
L93be:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,L93be
	dec	r13,#1
L93c8:
	ldl	rr2,rr10
	inc	r11,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	testb	rl6
	jr	ne,L93c8
	ldl	rr2,rr8
	ldm	r8,_stkseg+12(r15),#6
	add	r15,#%0018
	ret
	sub	r15,#%0016
	ldm	_stkseg+12(r15),r9,#5
	ld	r9,r3
