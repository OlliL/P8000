	.seg
	.extern	_stkseg
	.code

_strcmp::
	ldl	rr10,rr4
	ldl	rr12,rr6
L93fa:
	dec	r9,#1
	cp	r9,#%0000
	jr	lt,L941a
	ldl	rr2,rr10
	inc	r11,#1
	ldb	rl4,@rr2
	cpb	rl4,@rr12
	jr	ne,L941a
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	testb	rl4
	jr	ne,L93fa
L9416:
	sub	r2,r2
	jr	L942e
L941a:
	cp	r9,#%0000
	jr	lt,L9416
	dec	r11,#1
	ldb	rl2,@rr10
	extsb	r2
	ldb	rl3,@rr12
	extsb	r3
	sub	r3,r2
	ld	r2,r3
L942e:
	ldm	r9,_stkseg+12(r15),#5
	add	r15,#%0016
	ret
