	.extern	_fflush, _lseek
	.psec
	.code
_fseek::
	dec	r15,#14
	ldm	%000A(r15),r13,#2
	ldm	%0000(r15),r3,#3
	ld	r14,r7
	resb	%0006(r14),#4
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#0
	jr	eq,L00de
	cp	r3,#%0002
	jr	ge,L00ae
	test	%0004(r14)
	jr	eq,L00ae
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	ne,L00ae
	ld	r13,%0002(r14)
	ldl	%0006(r15),rr4
	test	r3
	jr	ne,L005e
	ldb	rl7,%0007(r14)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ld	r1,r13
	exts	rr0
	subl	rr0,rr2
	addl	rr0,%0006(r15)
	ldl	%0006(r15),rr0
	jr	L006c
L005e:
	ld	r1,r13
	exts	rr0
	ldl	rr6,%0002(r15)
	subl	rr6,rr0
	ldl	%0002(r15),rr6
L006c:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	ne,L00ae
	cp	r13,#%0000
	jr	le,L00ae
	ld	r3,r13
	exts	rr2
	cpl	rr2,%0006(r15)
	jr	lt,L00ae
	ld	r2,%0004(r14)
	sub	r2,@r14
	ld	r1,r2
	exts	rr0
	cpl	rr0,%0006(r15)
	jr	gt,L00ae
	ld	r2,%0008(r15)
	add	r2,@r14
	ld	@r14,r2
	ld	r2,%0008(r15)
	ld	r3,%0002(r14)
	sub	r3,r2
	ld	%0002(r14),r3
	jr	L0130
L00ae:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L00c2
	ld	r2,%0004(r14)
	ld	@r14,r2
	resb	%0006(r14),#0
L00c2:
	ldb	rl7,%0007(r14)
	extsb	r7
	ldl	rr4,%0002(r15)
	ld	r3,%0000(r15)
	call	_lseek
	ldl	%0006(r15),rr2
	clr	%0002(r14)
	jr	L011e
L00de:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0082
	jr	eq,L011e
	ld	r7,r14
	call	_fflush
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L0108
	clr	%0002(r14)
	resb	%0006(r14),#1
	ld	r2,%0004(r14)
	ld	@r14,r2
L0108:
	ldb	rl7,%0007(r14)
	extsb	r7
	ldl	rr4,%0002(r15)
	ld	r3,%0000(r15)
	call	_lseek
	ldl	%0006(r15),rr2
L011e:
	ldl	rr2,%0006(r15)
	cpl	rr2,#%FFFFFFFF
	jr	ne,L0130
	ld	r2,#%FFFF
	jr	L0132
L0130:
	sub	r2,r2
L0132:
	ldm	r13,%000A(r15),#2
	inc	r15,#14
	ret
