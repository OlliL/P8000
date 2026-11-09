	.seg
	.extern	_stkseg
	.extern	_write
	.extern	_isatty
	.extern	_malloc
	.extern	_close
	.extern	_free
	.extern	__iob
	.extern	__lastbuf
	.extern	__sobuf
	.psec
	.code

__flsbuf::
	sub	r15,#%0020
	ldm	_stkseg+20(r15),r8,#6
	ld	|_stkseg+6|(r15),r7
	ldl	rr12,rr4
	ldb	rl6,rr12(#%000A)
	extsb	r6
	bit	r6,#7
	jr	eq,La7c8
	ldl	rr2,rr4
	inc	r3,#10
	setb	@rr2,#1
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#4
La7c8:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	eq,La7fc
	ldb	rl2,|_stkseg+7|(r15)
	ldb	|_stkseg+17|(r15),rl2
	ldk	r8,#1
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,#%7F000011
	add	r5,r15
	ld	r3,r8
	call	_write
	ld	r9,r2
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	La894
La7fc:
	ldl	rr2,rr12(#%0006)
	ldl	rr10,rr2
	testl	rr2
	jr	ne,La860
	cpl	rr12,#__iob+12
	jr	ne,La83e
	ldb	rl7,__iob+23
	extsb	r7
	call	_isatty
	test	r2
	jr	eq,La828
La820:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#2
	jr	La7c8
La828:
	ldl	rr2,rr12
	inc	r3,#6
	ldl	rr4,#__sobuf
	ldl	@rr2,rr4
	ldl	rr2,#__sobuf
	ldl	@rr12,rr2
	jr	La7c8
La83e:
	ld	r7,#%0200
	call	_malloc
	ldl	rr10,rr2
	ldl	rr4,rr12
	inc	r5,#6
	ldl	@rr4,rr2
	testl	rr2
	jr	eq,La820
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#3
	ldk	r9,#0
	ld	r8,r9
	jr	La882
La860:
	ldl	rr2,@rr12
	sub	r3,r11
	ld	r9,r3
	ld	r8,r3
	cp	r3,#%0000
	jr	le,La882
	ldl	@rr12,rr10
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,rr10
	ld	r3,r9
	call	_write
	ld	r9,r2
La882:
	ldl	rr2,rr12
	inc	r3,#4
	ld	@rr2,#%01FF
	ldb	rl2,|_stkseg+7|(r15)
	ldb	@rr10,rl2
	inc	r11,#1
	ldl	@rr12,rr10
La894:
	cp	r8,r9
	jr	eq,La8a4
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
	ld	r2,#%FFFF
	jr	La8a8
La8a4:
	ld	r2,|_stkseg+6|(r15)
La8a8:
	ldm	r8,_stkseg+20(r15),#6
	add	r15,#%0020
	ret
La8b6:
_fflush::
	sub	r15,#%0016
	ldm	_stkseg+12(r15),r9,#5
	ldl	rr12,rr6
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0006
	cp	r2,#%0002
	jr	ne,La914
	ldl	rr2,rr12(#%0006)
	ldl	rr10,rr2
	testl	rr2
	jr	eq,La914
	ldl	rr2,@rr12
	sub	r3,r11
	ld	r9,r3
	cp	r3,#%0000
	jr	le,La914
	ldl	@rr12,rr10
	ldl	rr2,rr12
	inc	r3,#4
	ld	@rr2,#%0200
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,rr10
	ld	r3,r9
	call	_write
	cp	r9,r2
	jr	eq,La914
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
	ld	r2,#%FFFF
	jr	La916
La914:
	sub	r2,r2
La916:
	ldm	r9,_stkseg+12(r15),#5
	add	r15,#%0016
	ret
__cleanup::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	rr12,#__iob
	jr	La93c
La936:
	ldl	rr6,rr12
	calr	La950
	inc	r13,#12
La93c:
	cpl	rr12,__lastbuf
	jr	c,La936
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
La950:
_fclose::
	dec	r15,#14
	ldm	_stkseg+8(r15),r11,#3
	ldl	rr12,rr6
	ld	r11,#%FFFF
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0083
	jr	eq,La9bc
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0040
	test	r2
	jr	ne,La9bc
	calr	La8b6
	ld	r11,r2
	ldb	rl7,rr12(#%000B)
	extsb	r7
	call	_close
	cp	r2,#%0000
	jr	ge,La994
	ld	r11,#%FFFF
La994:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#3
	jr	eq,La9a8
	ldl	rr6,rr12(#%0006)
	call	_free
La9a8:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%000C
	jr	eq,La9bc
	ldl	rr2,rr12
	inc	r3,#6
	subl	rr4,rr4
	ldl	@rr2,rr4
La9bc:
	ldl	rr2,rr12
	inc	r3,#10
	ldb	rl4,@rr2
	andb	rl4,#%00
	ldb	@rr2,rl4
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ld	r2,r11
	ldm	r11,_stkseg+8(r15),#3
	inc	r15,#14
	ret
