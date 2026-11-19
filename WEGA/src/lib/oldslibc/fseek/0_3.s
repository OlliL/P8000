	.seg
	.extern	_stkseg
	.extern	_lseek
	.extern	_fflush
	.code

_fseek::
	dec	r15,#16
	ldm	_stkseg+10(r15),r11,#3
	ldm	|_stkseg+0|(r15),r3,#3
	ldl	rr12,rr6
	ldl	rr0,rr6
	inc	r1,#10
	ldl	rr6,rr0
	resb	@rr6,#4
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#0
	jr	eq,Lfd32
	cp	r3,#%0002
	jr	ge,Lfcfc
	ldl	rr0,rr12(#%0006)
	testl	rr0
	jr	eq,Lfcfc
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	ne,Lfcfc
	ld	r2,rr12(#%0004)
	ld	r11,r2
	ldl	|_stkseg+6|(r15),rr4
	test	r3
	jr	ne,Lfca4
	ldb	rl7,rr12(#%000B)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ld	r1,r11
	exts	rr0
	subl	rr0,rr2
	addl	rr0,|_stkseg+6|(r15)
	ldl	|_stkseg+6|(r15),rr0
	jr	Lfcb2
Lfca4:
	ld	r1,r11
	exts	rr0
	ldl	rr6,|_stkseg+2|(r15)
	subl	rr6,rr0
	ldl	|_stkseg+2|(r15),rr6
Lfcb2:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	ne,Lfcfc
	cp	r11,#%0000
	jr	le,Lfcfc
	ld	r3,r11
	exts	rr2
	cpl	rr2,|_stkseg+6|(r15)
	jr	lt,Lfcfc
	ldl	rr2,rr12(#%0006)
	ldl	rr0,@rr12
	sub	r3,r1
	ld	r1,r3
	exts	rr0
	cpl	rr0,|_stkseg+6|(r15)
	jr	gt,Lfcfc
	ld	r2,|_stkseg+8|(r15)
	ld	r1,rr12(#%0002)
	add	r1,r2
	ld	rr12(#%0002),r1
	ldl	rr2,rr12
	inc	r3,#4
	ld	r0,|_stkseg+8|(r15)
	ld	r1,@rr2
	sub	r1,r0
	ld	@rr2,r1
	jr	Lfd8c
Lfcfc:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,Lfd12
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#0
Lfd12:
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,|_stkseg+2|(r15)
	ld	r3,|_stkseg+0|(r15)
	call	_lseek
	ldl	|_stkseg+6|(r15),rr2
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	Lfd7a
Lfd32:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0082
	jr	eq,Lfd7a
	ldl	rr6,rr12
	call	_fflush
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,Lfd62
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#1
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
Lfd62:
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldl	rr4,|_stkseg+2|(r15)
	ld	r3,|_stkseg+0|(r15)
	call	_lseek
	ldl	|_stkseg+6|(r15),rr2
Lfd7a:
	ldl	rr2,|_stkseg+6|(r15)
	cpl	rr2,#%FFFFFFFF
	jr	ne,Lfd8c
	ld	r2,#%FFFF
	jr	Lfd8e
Lfd8c:
	sub	r2,r2
Lfd8e:
	ldm	r11,_stkseg+10(r15),#3
	inc	r15,#16
	ret
