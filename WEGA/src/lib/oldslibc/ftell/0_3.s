	.seg
	.extern	_stkseg
	.extern	_lseek
	.code

_ftell::
	dec	r15,#10
	ld	_stkseg+8(r15),r13
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,rr6
	ld	r2,rr2(#%0004)
	cp	r2,#%0000
	jr	ge,Lfdb8
	ldl	rr2,rr6
	inc	r3,#4
	clr	@rr2
Lfdb8:
	ldl	rr2,rr6
	ldb	rl2,rr2(#%000A)
	extsb	r2
	bit	r2,#0
	jr	eq,Lfe20
	ldl	rr2,rr6
	ld	r2,rr2(#%0004)
	neg	r2
	ld	r13,r2
	jr	Lfdfe
Lfdd0:
	ldk	r13,#0
	ldl	rr2,rr6
	ldb	rl2,rr2(#%000A)
	extsb	r2
	bit	r2,#1
	jr	eq,Lfdfe
	ldl	rr2,rr6
	ldl	rr0,rr2(#%0006)
	testl	rr0
	jr	eq,Lfdfe
	ldl	rr2,rr6
	ldb	rl2,rr2(#%000A)
	extsb	r2
	and	r2,#%0004
	test	r2
	jr	ne,Lfdfe
	ldl	rr2,@rr6
	sub	r3,r1
	ld	r13,r3
Lfdfe:
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl7,rr2(#%000B)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ldl	|_stkseg+4|(r15),rr2
	cpl	rr2,#%00000000
	jr	ge,Lfe36
	jr	Lfe42
Lfe20:
	ldl	rr2,rr6
	ldb	rl2,rr2(#%000A)
	extsb	r2
	and	r2,#%0082
	jr	ne,Lfdd0
	ldl	rr2,#%FFFFFFFF
	jr	Lfe46
Lfe36:
	ld	r3,r13
	exts	rr2
	addl	rr2,|_stkseg+4|(r15)
	ldl	|_stkseg+4|(r15),rr2
Lfe42:
	ldl	rr2,|_stkseg+4|(r15)
Lfe46:
	ld	r13,_stkseg+8(r15)
	inc	r15,#10
	ret
