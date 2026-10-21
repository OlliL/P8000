	.extern	_malloc, _read
	.psec
	.code
__filbuf::
	dec	r15,#6
	ld	%0004(r15),r14
	ld	r14,r7
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L0016
	setb	%0006(r14),#0
L0016:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,L002e
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#6
	jr	eq,L0048
L002e:
	ld	r2,#%FFFF
	jr	L00da
L0034:
	ld	r7,#%0200
	call	_malloc
	ld	%0004(r14),r2
	test	r2
	jr	ne,L0068
	setb	%0006(r14),#2
L0048:
	test	%0004(r14)
	jr	ne,L006c
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	eq,L0034
	ldb	rl2,%0007(r14)
	extsb	r2
	add	r2,#D0000
	ld	%0004(r14),r2
	jr	L0048
L0068:
	setb	%0006(r14),#3
L006c:
	ld	r2,%0004(r14)
	ld	@r14,r2
	ldb	rl5,%0006(r14)
	extsb	r5
	bit	r5,#2
	jr	eq,L0080
	ldk	r5,#1
	jr	L0084
L0080:
	ld	r5,#%0200
L0084:
	ld	r6,@r14
	ldb	rl7,%0007(r14)
	extsb	r7
	call	_read
	ld	%0002(r14),r2
	dec	%0002(r14),#1
	cp	%0002(r14),#%0000
	jr	ge,L00c6
	cp	%0002(r14),#%FFFF
	jr	ne,L00bc
	setb	%0006(r14),#4
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L00c0
	resb	%0006(r14),#0
	jr	L00c0
L00bc:
	setb	%0006(r14),#5
L00c0:
	clr	%0002(r14)
	jr	L002e
L00c6:
	ld	r2,@r14
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	ld	%0002(r15),r3
	inc	@r14,#1
	ld	r2,%0002(r15)
L00da:
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
	.psec
	.data
D0000:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00
