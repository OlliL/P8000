	.seg
	.extern	_stkseg
	.code

_atoi::
	dec	r15,#14
	ldm	_stkseg+6(r15),r10,#4
	ldl	rr12,rr6
	ldk	r11,#0
	ldk	r10,#0
	jr	L8a8a
L8a6e:
	cpb	@rr12,#%39
	jr	gt,L8ab8
	ld	r3,r11
	mult	rr2,#%000A
	ldb	rl2,@rr12
	extsb	r2
	add	r3,r2
	sub	r3,#%0030
	ld	r11,r3
	jr	L8ab0
	inc	r13,#1
L8a8a:
	ldb	rl3,@rr12
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001A36
	ld	r3,#%0004
	cpir	r2,@rr4,r3,eq
	jr	ne,L8ab2
	sll	r3,#2
	neg	r3
	ldl	rr4,_d1A4A+0(r3)
	jp	@rr4
	inc	r10,#1
L8ab0:
	inc	r13,#1
L8ab2:
	cpb	@rr12,#%30
	jr	ge,L8a6e
L8ab8:
	test	r10
	jr	eq,L8ac2
	ld	r2,r11
	neg	r2
	jr	L8ac4
L8ac2:
	ld	r2,r11
L8ac4:
	ldm	r10,_stkseg+6(r15),#4
	inc	r15,#14
	ret

	.data

_d1A4A::
	.word	%0000
