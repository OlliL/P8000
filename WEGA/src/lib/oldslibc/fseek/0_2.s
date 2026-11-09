	.seg
	.extern	_stkseg
	.extern	_lseek
	.extern	_fflush
	.psec
	.code

_fseek::
	sub	r15,#%0018
	ldm	_stkseg+18(r15),r11,#3
	ldm	|_stkseg+0|(r15),r3,#3
	ldl	rr12,rr6
	inc	r7,#10
	resb	@rr6,#4
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#0
	jr	eq,L97c4
	cp	r3,#%0002
	jr	ge,L978e
	ldl	rr6,rr12(#%0006)
	testl	rr6
	jr	eq,L978e
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	ne,L978e
	ld	r2,rr12(#%0004)
	ld	r11,r2
	ldl	|_stkseg+12|(r15),rr4
	test	r3
	jr	ne,L9736
	ldb	rl7,rr12(#%000B)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ld	r5,r11
	exts	rr4
	subl	rr4,rr2
	addl	rr4,|_stkseg+12|(r15)
	ldl	|_stkseg+12|(r15),rr4
	jr	L9744
L9736:
	ld	r3,r11
	exts	rr2
	ldl	rr4,|_stkseg+2|(r15)
	subl	rr4,rr2
	ldl	|_stkseg+2|(r15),rr4
L9744:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	ne,L978e
	cp	r11,#%0000
	jr	le,L978e
	ld	r3,r11
	exts	rr2
	cpl	rr2,|_stkseg+12|(r15)
	jr	lt,L978e
	ldl	rr2,rr12(#%0006)
	ldl	rr4,@rr12
	sub	r3,r5
	ld	r5,r3
	exts	rr4
	cpl	rr4,|_stkseg+12|(r15)
	jr	gt,L978e
	ld	r2,|_stkseg+14|(r15)
	ld	r5,rr12(#%0002)
	add	r5,r2
	ld	rr12(#%0002),r5
	ldl	rr4,rr12
	inc	r5,#4
	ld	r2,|_stkseg+14|(r15)
	ld	r3,@rr4
	sub	r3,r2
	ld	@rr4,r3
	jr	L981e
L978e:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,L97a4
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#0
L97a4:
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,|_stkseg+2|(r15)
	ld	r3,|_stkseg+0|(r15)
	call	_lseek
	ldl	|_stkseg+12|(r15),rr2
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	L980c
L97c4:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0082
	jr	eq,L980c
	ldl	rr6,rr12
	call	_fflush
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,L97f4
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#1
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
L97f4:
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,|_stkseg+2|(r15)
	ld	r3,|_stkseg+0|(r15)
	call	_lseek
	ldl	|_stkseg+12|(r15),rr2
L980c:
	ldl	rr2,|_stkseg+12|(r15)
	cpl	rr2,#%FFFFFFFF
	jr	ne,L981e
	ld	r2,#%FFFF
	jr	L9820
L981e:
	sub	r2,r2
L9820:
	ldm	r11,_stkseg+18(r15),#3
	add	r15,#%0018
	ret
