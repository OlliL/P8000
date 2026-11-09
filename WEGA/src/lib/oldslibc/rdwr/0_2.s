	.seg
	.extern	_stkseg
	.extern	__filbuf
	.extern	__flsbuf
	.psec
	.code

_fread::
	sub	r15,#%001E
	ldm	_stkseg+20(r15),r9,#5
	ldl	|_stkseg+4|(r15),rr4
	ldl	rr10,rr2
	ldl	rr12,rr6
	clr	|_stkseg+14|(r15)
	test	r5
	jr	ne,L9896
	jr	L98a0
L984c:
	ld	r3,|_stkseg+6|(r15)
	ld	|_stkseg+16|(r15),r3
L9854:
	ldl	rr2,rr10
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L9874
	ldl	rr2,@rr10
	inc	r11,#2
	inc	@rr10,#1
	dec	r11,#2
	ldb	rl4,@rr2
	extsb	r4
	and	r4,#%00FF
	jr	L987e
L9874:
	ldl	rr6,rr10
	call	__filbuf
	ld	r4,r2
L987e:
	ld	r9,r4
	cp	r4,#%0000
	jr	lt,L98a0
	ld	r2,r4
	ldb	@rr12,rl2
	inc	r13,#1
	dec	|_stkseg+16|(r15),#1
	jr	ne,L9854
	inc	|_stkseg+14|(r15),#1
L9896:
	ld	r2,|_stkseg+14|(r15)
	cp	r2,|_stkseg+4|(r15)
	jr	c,L984c
L98a0:
	ld	r2,|_stkseg+14|(r15)
	ldm	r9,_stkseg+20(r15),#5
	add	r15,#%001E
	ret
_fwrite::
	sub	r15,#%001C
	ldm	_stkseg+18(r15),r9,#5
	ldl	|_stkseg+4|(r15),rr4
	ldl	rr10,rr2
	ldl	rr12,rr6
	clr	|_stkseg+14|(r15)
	test	r5
	jr	ne,L991c
	jr	L9926
L98d0:
	ld	r9,|_stkseg+6|(r15)
L98d4:
	ldl	rr2,rr10
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L98f6
	ldl	rr2,rr12
	inc	r13,#1
	ldl	rr4,@rr10
	inc	r11,#2
	inc	@rr10,#1
	dec	r11,#2
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	extsb	r6
	jr	L9908
L98f6:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl7,@rr2
	extsb	r7
	ldl	rr4,rr10
	call	__flsbuf
	ld	r6,r2
L9908:
	djnz	r9,L98d4
	ldb	rl2,rr10(#%000A)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L9926
	inc	|_stkseg+14|(r15),#1
L991c:
	ld	r2,|_stkseg+14|(r15)
	cp	r2,|_stkseg+4|(r15)
	jr	c,L98d0
L9926:
	ld	r2,|_stkseg+14|(r15)
	ldm	r9,_stkseg+18(r15),#5
	add	r15,#%001C
	ret
