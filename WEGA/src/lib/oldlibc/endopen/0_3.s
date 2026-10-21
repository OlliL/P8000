	.extern	_close, _creat, _errno, _lseek, _open
	.psec
	.code
__endopen::
	dec	r15,#10
	ldm	%0004(r15),r12,#3
	ldl	%0000(r15),rr6
	ld	r14,r5
	test	r5
	jr	ne,L0016
L0012:
	sub	r2,r2
	jr	L00fe
L0016:
	ld	r2,r6
	cpb	@r2,#%72
	jr	ne,L0028
	ld	r2,r6
	cpb	%0001(r2),#%77
	jr	eq,L003a
L0028:
	ld	r2,r6
	cpb	@r2,#%77
	jr	ne,L0048
	ld	r2,r6
	cpb	%0001(r2),#%72
	jr	ne,L0048
L003a:
	ld	r2,r6
	ldb	@r2,#%61
	ld	r2,r6
	ldb	%0001(r2),#%2B
L0048:
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
	jr	eq,L007a
	cp	r2,#%0072
	jr	eq,L00b2
	cp	r2,#%0077
	jr	ne,L0012
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	L0108
	jr	L00c4
L007a:
	test	r13
	jr	eq,L0082
	ldk	r6,#2
	jr	L0084
L0082:
	ldk	r6,#1
L0084:
	ld	r7,%0002(r15)
	call	_open
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L00a6
	cp	_errno,#%0002
	jr	ne,L00a6
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	L0108
	ld	r12,r2
L00a6:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	L00c6
L00b2:
	test	r13
	jr	eq,L00ba
	ldk	r6,#2
	jr	L00bc
L00ba:
	sub	r6,r6
L00bc:
	ld	r7,%0002(r15)
	call	_open
L00c4:
	ld	r12,r2
L00c6:
	cp	r12,#%0000
	jr	lt,L0012
	clr	%0002(r14)
	ld	r2,r12
	ldb	%0007(r14),rl2
	test	r13
	jr	eq,L00e8
	ldb	rl2,%0006(r14)
	orb	rl2,#%80
	ldb	%0006(r14),rl2
	jr	L00fc
L00e8:
	ld	r2,%0000(r15)
	cpb	@r2,#%72
	jr	ne,L00f8
	setb	%0006(r14),#0
	jr	L00fc
L00f8:
	setb	%0006(r14),#1
L00fc:
	ld	r2,r14
L00fe:
	ldm	r12,%0004(r15),#3
	inc	r15,#10
	ret
L0108:
	dec	r15,#6
	ldm	%0002(r15),r13,#2
	ld	%0000(r15),r6
	ld	r14,r7
	ld	r6,#%01B6
	call	_creat
	ld	r13,r2
	test	%0000(r15)
	jr	eq,L013c
	cp	r2,#%0000
	jr	lt,L013c
	ld	r7,r2
	call	_close
	ld	r7,r14
	ldk	r6,#2
	call	_open
	ld	r13,r2
L013c:
	ld	r2,r13
	ldm	r13,%0002(r15),#2
	inc	r15,#6
	ret
