	.extern	__iob, __lastbuf, __sobuf, _close, _free, _isatty, _malloc, _write
	.psec
	.code
__flsbuf::
	sub	r15,#%0016
	ldm	%000E(r15),r11,#4
	ld	%0002(r15),r7
	ld	r14,r6
	ldb	rl6,%0006(r14)
	extsb	r6
	bit	r6,#7
	jr	eq,L0042
	setb	%0006(r14),#1
	resb	%0006(r14),#4
	jr	L0042
L0024:
	ldb	rl7,__iob+15
	extsb	r7
	call	_isatty
	test	r2
	jr	eq,L0038
L0032:
	setb	%0006(r14),#2
	jr	L0042
L0038:
	ld	%0004(r14),#__sobuf
	ld	@r14,#__sobuf
L0042:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	eq,L006e
	ldb	rl2,%0003(r15)
	ldb	%000B(r15),rl2
	ldk	r11,#1
	lda	r6,%000B(r15)
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r5,r11
	call	_write
	ld	r12,r2
	clr	%0002(r14)
	jr	L00c8
L006e:
	ld	r13,%0004(r14)
	test	r13
	jr	ne,L0098
	cp	r14,#__iob+8
	jr	eq,L0024
	ld	r7,#%0200
	call	_malloc
	ld	r13,r2
	ld	%0004(r14),r2
	test	r2
	jr	eq,L0032
	setb	%0006(r14),#3
	ldk	r12,#0
	ld	r11,r12
	jr	L00b8
L0098:
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	ld	r11,r2
	cp	r2,#%0000
	jr	le,L00b8
	ld	@r14,r13
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	call	_write
	ld	r12,r2
L00b8:
	ld	%0002(r14),#%01FF
	ldb	rl2,%0003(r15)
	ldb	@r13,rl2
	inc	r13,#1
	ld	@r14,r13
L00c8:
	cp	r11,r12
	jr	eq,L00d6
	setb	%0006(r14),#5
	ld	r2,#%FFFF
	jr	L00da
L00d6:
	ld	r2,%0002(r15)
L00da:
	ldm	r11,%000E(r15),#4
	add	r15,#%0016
	ret
_fflush::
	dec	r15,#14
	ldm	%0008(r15),r12,#3
	ld	r14,r7
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0006
	cp	r2,#%0002
	jr	ne,L0138
	ld	r13,%0004(r14)
	test	r13
	jr	eq,L0138
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	cp	r2,#%0000
	jr	le,L0138
	ld	@r14,r13
	ld	%0002(r14),#%0200
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	call	_write
	cp	r12,r2
	jr	eq,L0138
	setb	%0006(r14),#5
	ld	r2,#%FFFF
	jr	L013a
L0138:
	sub	r2,r2
L013a:
	ldm	r12,%0008(r15),#3
	inc	r15,#14
	ret
__cleanup::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	r14,#__iob
	jr	L0156
L0150:
	ld	r7,r14
	calr	_fclose
	inc	r14,#8
L0156:
	cp	r14,__lastbuf
	jr	c,L0150
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
_fclose::
	dec	r15,#10
	ldm	%0006(r15),r13,#2
	ld	r14,r7
	ld	r13,#%FFFF
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0083
	jr	eq,L01c6
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0040
	test	r2
	jr	ne,L01c6
	calr	_fflush
	ld	r13,r2
	ldb	rl7,%0007(r14)
	extsb	r7
	call	_close
	cp	r2,#%0000
	jr	ge,L01a4
	ld	r13,#%FFFF
L01a4:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#3
	jr	eq,L01b6
	ld	r7,%0004(r14)
	call	_free
L01b6:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%000C
	jr	eq,L01c6
	clr	%0004(r14)
L01c6:
	ldb	rl2,%0006(r14)
	andb	rl2,#%00
	ldb	%0006(r14),rl2
	clr	%0002(r14)
	ld	r2,r13
	ldm	r13,%0006(r15),#2
	inc	r15,#10
	ret
