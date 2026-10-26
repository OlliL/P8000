	.extern	_chmod, _close, _creat, _lseek, _open, _umask
	.psec
	.code
__endopen::
	dec	r15,#10
	ldm	%0004(r15),r12,#3
	ldl	%0000(r15),rr6
	ld	r14,r5
	test	r5
	jr	ne,Lca06
Lca02:
	sub	r2,r2
	jr	Lcaee
Lca06:
	ld	r2,r6
	cpb	@r2,#%72
	jr	ne,Lca18
	ld	r2,r6
	cpb	%0001(r2),#%77
	jr	eq,Lca2a
Lca18:
	ld	r2,r6
	cpb	@r2,#%77
	jr	ne,Lca38
	ld	r2,r6
	cpb	%0001(r2),#%72
	jr	ne,Lca38
Lca2a:
	ld	r2,r6
	ldb	@r2,#%72
	ld	r2,r6
	ldb	%0001(r2),#%2B
Lca38:
	ld	r2,r6
	cpb	%0001(r2),#%2B
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
	ld	r13,r2
	ld	r2,r6
	ldb	rl2,@r2
	extsb	r2
	cp	r2,#%0061
	jr	eq,Lca6a
	cp	r2,#%0072
	jr	eq,Lcaa2
	cp	r2,#%0077
	jr	ne,Lca02
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	Lcaf8
	jr	Lcab4
Lca6a:
	test	r13
	jr	eq,Lca72
	ldk	r6,#2
	jr	Lca74
Lca72:
	ldk	r6,#1
Lca74:
	ld	r7,%0002(r15)
	call	_open
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,Lca96
	cp	%69C8,#%0002
	jr	ne,Lca96
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	Lcaf8
	ld	r12,r2
Lca96:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	Lcab6
Lcaa2:
	test	r13
	jr	eq,Lcaaa
	ldk	r6,#2
	jr	Lcaac
Lcaaa:
	sub	r6,r6
Lcaac:
	ld	r7,%0002(r15)
	call	_open
Lcab4:
	ld	r12,r2
Lcab6:
	cp	r12,#%0000
	jr	lt,Lca02
	clr	%0002(r14)
	ld	r2,r12
	ldb	%0007(r14),rl2
	test	r13
	jr	eq,Lcad8
	ldb	rl2,%0006(r14)
	orb	rl2,#%80
	ldb	%0006(r14),rl2
	jr	Lcaec
Lcad8:
	ld	r2,%0000(r15)
	cpb	@r2,#%72
	jr	ne,Lcae8
	setb	%0006(r14),#0
	jr	Lcaec
Lcae8:
	setb	%0006(r14),#1
Lcaec:
	ld	r2,r14
Lcaee:
	ldm	r12,%0004(r15),#3
	inc	r15,#10
	ret
Lcaf8:
	dec	r15,#8
	ldm	%0002(r15),r12,#3
	ld	%0000(r15),r6
	ld	r14,r7
	test	r6
	jr	eq,Lcb4a
	sub	r7,r7
	call	_umask
	ld	r13,r2
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
	cp	r2,#%0000
	jr	lt,Lcb42
	ld	r7,r2
	call	_close
	ld	r7,r14
	ldk	r6,#2
	call	_open
	ld	r12,r2
	ld	r6,r13
	com	r6
	and	r6,#%01B6
	ld	r7,r14
	call	_chmod
Lcb42:
	ld	r7,r13
	call	_umask
	jr	Lcb56
Lcb4a:
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
Lcb56:
	ld	r2,r12
	ldm	r12,%0002(r15),#3
	inc	r15,#8
	ret
