	.seg
	.extern	_stkseg
	.extern	__deciml
	.psec
	.code

__cnvt::
	sub	r15,#%0030
	ldm	_stkseg+40(r15),r10,#4
	ldm	|_stkseg+0|(r15),r2,#6
	ldl	rr10,rr6
	cp	r5,#%FFF6
	jr	ne,Lacdc
	ld	|_stkseg+6|(r15),#%000A
	cpl	rr2,#%00000000
	jr	ge,Lacdc
	ldb	@rr10,#%2D
	inc	r11,#1
	subl	rr4,rr4
	subl	rr4,rr2
	ldl	|_stkseg+0|(r15),rr4
Lacdc:
	ldl	rr2,#%7F000018
	add	r3,r15
	ldl	rr12,rr2
	ldl	rr2,|_stkseg+0|(r15)
	ldl	|_stkseg+20|(r15),rr2
	ld	r4,|_stkseg+6|(r15)
	ldl	rr2,#D9124
	ld	r5,#%0003
	cpir	r4,@rr2,r5,eq
	jr	ne,Ladb0
	sll	r5,#2
	neg	r5
	ldl	rr2,D9132(r5)
	jp	@rr2
Lad10:
	ldl	rr2,|_stkseg+20|(r15)
	ldk	r2,#0
	and	r3,#%0007
	addl	rr2,#%00000030
	ldb	@rr12,rl3
	inc	r13,#1
	ldl	rr2,|_stkseg+20|(r15)
	srll	rr2,#3
	ldl	|_stkseg+20|(r15),rr2
	testl	rr2
	jr	ne,Lad10
	jr	Lad68
Lad36:
	ldl	rr2,|_stkseg+20|(r15)
	ldk	r2,#0
	and	r3,#%000F
	addl	rr2,#%00000030
	ldb	@rr12,rl3
	cpb	@rr12,#%39
	jr	le,Lad56
	ldb	rl5,@rr12
	addb	rl5,#%27
	ldb	@rr12,rl5
Lad56:
	inc	r13,#1
	ldl	rr2,|_stkseg+20|(r15)
	srll	rr2,#4
	ldl	|_stkseg+20|(r15),rr2
	testl	rr2
	jr	ne,Lad36
Lad68:
	dec	r13,#1
	ldb	rl5,@rr12
	ldb	@rr10,rl5
	inc	r11,#1
	ldl	rr2,#%7F000018
	add	r3,r15
	cpl	rr12,rr2
	jr	ne,Lad68
	jr	Ladaa
Lad7e:
	ldl	rr6,#%7F000018
	add	r7,r15
	inc	r7,#9
	ldl	rr4,|_stkseg+0|(r15)
	call	__deciml
	ldl	rr12,rr2
Lad94:
	ldb	rl2,@rr12
	ldb	@rr10,rl2
	inc	r13,#1
	inc	r11,#1
	ldl	rr2,#%7F000018
	add	r3,r15
	inc	r3,#10
	cpl	rr12,rr2
	jr	ne,Lad94
Ladaa:
	ld	r2,r11
	sub	r2,|_stkseg+10|(r15)
Ladb0:
	ldm	r10,_stkseg+40(r15),#4
	add	r15,#%0030
	ret
	.psec data
	.data
D9124:
	.byte	%00
	.byte	%08
	.byte	%00
	.byte	%0a
	.byte	%00
	.byte	%10
	.long	Lad10
	.long	Lad7e
D9132:
	.long	Lad36
