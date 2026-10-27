	.seg
	.extern	_stkseg
	.extern	_f970a
	.extern	_f91ea
	.extern	_malloc
	.extern	_f9576
	.extern	_f903a
	.extern	_cnvt
	.code

_flsbuf::
	ldl	rr12,rr4
	ldb	rl6,rr12(#%000A)
	extsb	r6
	bit	r6,#7
	jr	eq,L8836
	ldl	rr2,rr4
	inc	r3,#10
	setb	@rr2,#1
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#4
L8836:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	eq,L886a
	ldb	rl2,|_stkseg+7|(r15)
	ldb	|_stkseg+9|(r15),rl2
	ldk	r8,#1
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,#%7F000009
	add	r5,r15
	ld	r3,r8
	call	_f970a
	ld	r9,r2
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	L8902
L886a:
	ldl	rr2,rr12(#%0006)
	ldl	rr10,rr2
	testl	rr2
	jr	ne,L88ce
	cpl	rr12,#%0100194E
	jr	ne,L88ac
	ldb	rl7,_d1959+0
	extsb	r7
	call	_f91ea
	test	r2
	jr	eq,L8896
L888e:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#2
	jr	L8836
L8896:
	ldl	rr2,rr12
	inc	r3,#6
	ldl	rr4,#%010090E8
	ldl	@rr2,rr4
	ldl	rr2,#%010090E8
	ldl	@rr12,rr2
	jr	L8836
L88ac:
	ld	r7,#%0200
	call	_malloc
	ldl	rr10,rr2
	ldl	rr4,rr12
	inc	r5,#6
	ldl	@rr4,rr2
	testl	rr2
	jr	eq,L888e
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#3
	ldk	r9,#0
	ld	r8,r9
	jr	L88f0
L88ce:
	ldl	rr2,@rr12
	sub	r3,r11
	ld	r9,r3
	ld	r8,r3
	cp	r3,#%0000
	jr	le,L88f0
	ldl	@rr12,rr10
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,rr10
	ld	r3,r9
	call	_f970a
	ld	r9,r2
L88f0:
	ldl	rr2,rr12
	inc	r3,#4
	ld	@rr2,#%01FF
	ldb	rl2,|_stkseg+7|(r15)
	ldb	@rr10,rl2
	inc	r11,#1
	ldl	@rr12,rr10
L8902:
	cp	r8,r9
	jr	eq,L8912
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
	ld	r2,#%FFFF
	jr	L8916
L8912:
	ld	r2,|_stkseg+6|(r15)
L8916:
	ldm	r8,_stkseg+12(r15),#6
	add	r15,#%0018
	ret
L8924:
	dec	r15,#16
	ldm	_stkseg+6(r15),r9,#5
	ldl	rr12,rr6
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0006
	cp	r2,#%0002
	jr	ne,L8980
	ldl	rr2,rr12(#%0006)
	ldl	rr10,rr2
	testl	rr2
	jr	eq,L8980
	ldl	rr2,@rr12
	sub	r3,r11
	ld	r9,r3
	cp	r3,#%0000
	jr	le,L8980
	ldl	@rr12,rr10
	ldl	rr2,rr12
	inc	r3,#4
	ld	@rr2,#%0200
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,rr10
	ld	r3,r9
	call	_f970a
	cp	r9,r2
	jr	eq,L8980
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
	ld	r2,#%FFFF
	jr	L8982
L8980:
	sub	r2,r2
L8982:
	ldm	r9,_stkseg+6(r15),#5
	inc	r15,#16
	ret
_f898e::
	dec	r15,#4
	ldm	_stkseg+0(r15),r12,#2
	ldl	rr12,#%01001942
	jr	L89a6
L89a0:
	ldl	rr6,rr12
	calr	L89ba
	inc	r13,#12
L89a6:
	cpl	rr12,_d1A32+0
	jr	c,L89a0
	ldm	r12,_stkseg+0(r15),#2
	inc	r15,#4
	ret
L89ba:
	dec	r15,#12
	ldm	_stkseg+6(r15),r11,#3
	ldl	rr12,rr6
	ld	r11,#%FFFF
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0083
	jr	eq,L8a28
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0040
	test	r2
	jr	ne,L8a28
	calr	L8924
	ld	r11,r2
	ldb	rl7,rr12(#%000B)
	extsb	r7
	call	_f9576
	cp	r2,#%0000
	jr	ge,L8a00
	ld	r2,#%FFFF
	jr	L8a3c
L8a00:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#3
	jr	eq,L8a14
	ldl	rr6,rr12(#%0006)
	call	_f903a
L8a14:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%000C
	jr	eq,L8a28
	ldl	rr2,rr12
	inc	r3,#6
	subl	rr4,rr4
	ldl	@rr2,rr4
L8a28:
	ldl	rr2,rr12
	inc	r3,#10
	ldb	rl4,@rr2
	andb	rl4,#%00
	ldb	@rr2,rl4
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ld	r2,r11
L8a3c:
	ldm	r11,_stkseg+6(r15),#3
	inc	r15,#12
	ret
	call	_cnvt
	ret

	.data

_d1959::
	.word	%0100

_d1A32::
	.word	%0100
