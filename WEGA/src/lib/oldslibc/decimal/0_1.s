	.seg
	.extern	_stkseg
	.code

_decimal::
	dec	r15,#4
	ldl	|_stkseg+0|(r15),rr8
	ldl	rr8,rr6
	ldl	rr0,rr4
	subl	rr2,rr2
	ldl	rr4,rr2
	ldk	r6,#1
	testl	rr0
	jr	eq,L8c30
L8bfc:
	srll	rr0,#1
	jr	nc,L8c1a
	addb	rh4,rl6
	dab	rh4
	adcb	rl3,rh6
	dab	rl3
	adcb	rh3,rl5
	dab	rh3
	adcb	rl2,rh5
	dab	rl2
	adcb	rh2,rl4
	dab	rh2
	testl	rr0
	jr	eq,L8c30
L8c1a:
	addb	rl6,rl6
	dab	rl6
	adcb	rh6,rh6
	dab	rh6
	adcb	rl5,rl5
	dab	rl5
	adcb	rh5,rh5
	dab	rh5
	adcb	rl4,rl4
	dab	rl4
	jr	L8bfc
L8c30:
	ldl	rr6,#%00000001
	ldb	rl0,#%30
	rrdb	rl0,rh4
	ldb	@rr8,rl0
	rrdb	rl0,rh4
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rl3
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rl3
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rh3
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rh3
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rl2
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rl2
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rh2
	subl	rr8,rr6
	ldb	@rr8,rl0
	rrdb	rl0,rh2
	subl	rr8,rr6
	ldb	@rr8,rl0
	ldb	rl0,#%30
	ldk	r1,#10
	cpirb	rl0,@rr8,r1,ne
	subl	rr8,rr6
	ldl	rr2,rr8
	ldl	rr8,|_stkseg+0|(r15)
	inc	r15,#4
	ret
	dec	r15,#12
	ldm	_stkseg+6(r15),r11,#3
	ldl	rr12,rr6
	ldk	r11,#0
L8c94:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	testb	rl4
	jr	eq,L8ca6
	inc	r11,#1
	cp	r11,#%4000
	jr	le,L8c94
L8ca6:
	ld	r2,r11
	ldm	r11,_stkseg+6(r15),#3
	inc	r15,#12
	ret
