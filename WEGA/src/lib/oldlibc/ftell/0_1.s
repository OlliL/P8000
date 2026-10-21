	.extern	_lseek
	.psec
	.code
_ftell::
	dec	r15,#8
	ld	%0006(r15),r14
	ld	%0000(r15),r7
	ld	r2,r7
	cp	%0002(r2),#%0000
	jr	ge,L001a
	ld	r2,r7
	clr	%0002(r2)
L001a:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#0
	jr	eq,L0080
	ld	r2,r7
	ld	r2,%0002(r2)
	neg	r2
	ld	r14,r2
	jr	L0064
L0032:
	ldk	r14,#0
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#1
	jr	eq,L0064
	ld	r2,r7
	test	%0004(r2)
	jr	eq,L0064
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0004
	test	r2
	jr	ne,L0064
	ld	r2,r7
	ld	r3,r7
	ld	r3,@r3
	sub	r3,%0004(r2)
	ld	r14,r3
L0064:
	ldb	rl7,%0007(r7)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ldl	%0002(r15),rr2
	cpl	rr2,#%00000000
	jr	ge,L0096
	jr	L00a2
L0080:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0082
	jr	ne,L0032
	ldl	rr2,#%FFFFFFFF
	jr	L00a6
L0096:
	ld	r3,r14
	exts	rr2
	addl	rr2,%0002(r15)
	ldl	%0002(r15),rr2
L00a2:
	ldl	rr2,%0002(r15)
L00a6:
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
