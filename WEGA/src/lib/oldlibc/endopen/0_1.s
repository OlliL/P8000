	.extern	_chmod, _close, _creat, _errno, _lseek, _open, _umask
	.psec
	.code
__endopen::
	sub	r15,#%0018
	ldm	%0012(r15),r12,#3
	ldl	%0008(r15),rr6
	ld	r14,r5
	test	r5
	jr	ne,L001a
L0014:
	sub	r2,r2
	jp	L0140
L001a:
	ld	r2,%0008(r15)
	cpb	@r2,#%72
	ldk	r3,#0
	tcc	eq,r3
	extsb	r3
	ld	@r15,r3
	test	r3
	jr	eq,L0040
	ld	r2,%0008(r15)
	cpb	%0001(r2),#%77
	ldk	r3,#0
	tcc	eq,r3
	extsb	r3
	ld	@r15,r3
L0040:
	test	@r15
	jr	ne,L0056
	ld	r2,%0008(r15)
	cpb	@r2,#%77
	jr	ne,L0068
	cpb	%0001(r2),#%72
	jr	ne,L0068
L0056:
	ld	r2,%0008(r15)
	ldb	@r2,#%72
	ld	r2,%0008(r15)
	ldb	%0001(r2),#%2B
L0068:
	ld	r2,%0008(r15)
	cpb	%0001(r2),#%2B
	ldk	r3,#0
	tcc	eq,r3
	extsb	r3
	ld	r13,r3
	ld	r2,%0008(r15)
	ldb	rl4,@r2
	extsb	r4
	ld	r3,r4
	ld	r2,#D0000
	ld	r4,#%0003
	cpir	r3,@r2,r4,eq
	jr	ne,L0014
	add	r4,r4
	neg	r4
	ld	r3,D000a(r4)
	jp	@r3
L009c:
	ld	r7,%000A(r15)
	ld	r6,r13
	calr	L014c
	jr	L0106
L00a6:
	test	r13
	jr	eq,L00b2
	ld	%0002(r15),#%0002
	jr	L00b8
L00b2:
	ld	%0002(r15),#%0001
L00b8:
	ld	r7,%000A(r15)
	ld	r6,%0002(r15)
	call	_open
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L00de
	cp	_errno,#%0002
	jr	ne,L00de
	ld	r7,%000A(r15)
	ld	r6,r13
	calr	L014c
	ld	r12,r2
L00de:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	L0108
L00ea:
	test	r13
	jr	eq,L00f6
	ld	%0004(r15),#%0002
	jr	L00fa
L00f6:
	clr	%0004(r15)
L00fa:
	ld	r7,%000A(r15)
	ld	r6,%0004(r15)
	call	_open
L0106:
	ld	r12,r2
L0108:
	cp	r12,#%0000
	jr	lt,L0014
	clr	%0002(r14)
	ld	r2,r12
	ldb	%0007(r14),rl2
	test	r13
	jr	eq,L012a
	ldb	rl2,%0006(r14)
	orb	rl2,#%80
	ldb	%0006(r14),rl2
	jr	L013e
L012a:
	ld	r2,%0008(r15)
	cpb	@r2,#%72
	jr	ne,L013a
	setb	%0006(r14),#0
	jr	L013e
L013a:
	setb	%0006(r14),#1
L013e:
	ld	r2,r14
L0140:
	ldm	r12,%0012(r15),#3
	add	r15,#%0018
	ret
L014c:
	dec	r15,#16
	ldm	%000A(r15),r12,#3
	ld	@r15,r6
	ld	r14,r7
	test	r6
	jr	eq,L019c
	sub	r7,r7
	call	_umask
	ld	r13,r2
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
	cp	r2,#%0000
	jr	lt,L0194
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
L0194:
	ld	r7,r13
	call	_umask
	jr	L01a8
L019c:
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
L01a8:
	ld	r2,r12
	ldm	r12,%000A(r15),#3
	inc	r15,#16
	ret
	.psec
	.data
D0000:
	.byte	%00,%61,%00,%72,%00,%77
	.addr	L00a6
	.addr	L00ea
D000a:
	.addr	L009c
