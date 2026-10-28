	.seg
	.extern	_stkseg
	.code

_swab::
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
