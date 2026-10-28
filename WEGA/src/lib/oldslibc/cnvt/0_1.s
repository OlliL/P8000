	.seg
	.extern	_stkseg
	.extern	__deciml
	.code

__cnvt::
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
	ldl	rr2,_d1A5C+0(r5)
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
	call	__deciml
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

_d1A5C::
	.word	%0000
