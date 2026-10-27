	.seg
	.extern	_f898e
	.extern	_stkseg
	.extern	_decimal
	.code

_cnvt::
	ld	r8,r7
	call	_f898e
	ld	r0,r8
	sc	#%01
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
	sub	r15,#%0028
	ldm	_stkseg+32(r15),r10,#4
	ldm	|_stkseg+0|(r15),r2,#6
	ldl	rr10,rr6
	cp	r5,#%FFF6
	jr	ne,L8b06
	ld	|_stkseg+6|(r15),#%000A
	cpl	rr2,#%00000000
	jr	ge,L8b06
	ldb	@rr10,#%2D
	inc	r11,#1
	subl	rr4,rr4
	subl	rr4,rr2
	ldl	|_stkseg+0|(r15),rr4
L8b06:
	ldl	rr2,#%7F000010
	add	r3,r15
	ldl	rr12,rr2
	ldl	rr2,|_stkseg+0|(r15)
	ldl	|_stkseg+12|(r15),rr2
	ld	r4,|_stkseg+6|(r15)
	ldl	rr2,#%01001A4E
	ld	r5,#%0003
	cpir	r4,@rr2,r5,eq
	jr	ne,L8bda
	sll	r5,#2
	neg	r5
	ldl	rr2,_d1A4A+18(r5)
	jp	@rr2
L8b3a:
	ldl	rr2,|_stkseg+12|(r15)
	ldk	r2,#0
	and	r3,#%0007
	addl	rr2,#%00000030
	ldb	@rr12,rl3
	inc	r13,#1
	ldl	rr2,|_stkseg+12|(r15)
	srll	rr2,#3
	ldl	|_stkseg+12|(r15),rr2
	testl	rr2
	jr	ne,L8b3a
	jr	L8b92
L8b60:
	ldl	rr2,|_stkseg+12|(r15)
	ldk	r2,#0
	and	r3,#%000F
	addl	rr2,#%00000030
	ldb	@rr12,rl3
	cpb	@rr12,#%39
	jr	le,L8b80
	ldb	rl5,@rr12
	addb	rl5,#%27
	ldb	@rr12,rl5
L8b80:
	inc	r13,#1
	ldl	rr2,|_stkseg+12|(r15)
	srll	rr2,#4
	ldl	|_stkseg+12|(r15),rr2
	testl	rr2
	jr	ne,L8b60
L8b92:
	dec	r13,#1
	ldb	rl5,@rr12
	ldb	@rr10,rl5
	inc	r11,#1
	ldl	rr2,#%7F000010
	add	r3,r15
	cpl	rr12,rr2
	jr	ne,L8b92
	jr	L8bd4
	ldl	rr6,#%7F000010
	add	r7,r15
	inc	r7,#9
	ldl	rr4,|_stkseg+0|(r15)
	call	_decimal
	ldl	rr12,rr2
L8bbe:
	ldb	rl2,@rr12
	ldb	@rr10,rl2
	inc	r13,#1
	inc	r11,#1
	ldl	rr2,#%7F000010
	add	r3,r15
	inc	r3,#10
	cpl	rr12,rr2
	jr	ne,L8bbe
L8bd4:
	ld	r2,r11
	sub	r2,|_stkseg+10|(r15)
L8bda:
	ldm	r10,_stkseg+32(r15),#4
	add	r15,#%0028
	ret

	.data

_d1A4A::
	.word	%0000
	.word	%8AAE
	.word	%0008
	.word	%000A
	.word	%0010
	.word	%0000
	.word	%8B3A
	.word	%0000
	.word	%8BA8
	.word	%0000
