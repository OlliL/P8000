	.extern	_lseek
	.psec
	.code
_ftell::
	dec	r15,#8
	ld	%0006(r15),r14
	ld	%0000(r15),r7
	ld	r2,r7
	cp	%0002(r2),#%0000
	jr	ge,Lbd42
	ld	r2,r7
	clr	%0002(r2)
Lbd42:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#0
	jr	eq,Lbdac
	ld	r2,r7
	ld	r2,%0002(r2)
	neg	r2
	ld	r14,r2
	jr	Lbd8c
Lbd5a:
	ldk	r14,#0
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#1
	jr	eq,Lbd8c
	ld	r2,r7
	test	%0004(r2)
	jr	eq,Lbd8c
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0004
	test	r2
	jr	ne,Lbd8c
	ld	r2,r7
	ld	r3,r7
	ld	r3,@r3
	sub	r3,%0004(r2)
	ld	r14,r3
Lbd8c:
	ld	r7,%0000(r15)
	ldb	rl7,%0007(r7)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ldl	%0002(r15),rr2
	cpl	rr2,#%00000000
	jr	ge,Lbdc2
	jr	Lbdce
Lbdac:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0082
	jr	ne,Lbd5a
	ldl	rr2,#%FFFFFFFF
	jr	Lbdd2
Lbdc2:
	ld	r3,r14
	exts	rr2
	addl	rr2,%0002(r15)
	ldl	%0002(r15),rr2
Lbdce:
	ldl	rr2,%0002(r15)
Lbdd2:
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
