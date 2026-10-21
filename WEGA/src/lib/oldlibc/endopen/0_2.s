	.extern	_chmod, _close, _creat, _errno, _lseek, _open, _umask
	.psec
	.code
__endopen::
	sub	r15,#%0012
	ldm	%000C(r15),r12,#3
	ldl	%0002(r15),rr6
	ld	r14,r5
	test	r5
	jr	ne,L0018
L0014:
	sub	r2,r2
	jr	L0110
L0018:
	ld	r2,%0002(r15)
	cpb	@r2,#%72
	jr	ne,L002a
	cpb	%0001(r2),#%77
	jr	eq,L003c
L002a:
	ld	r2,%0002(r15)
	cpb	@r2,#%77
	jr	ne,L004e
	cpb	%0001(r2),#%72
	jr	ne,L004e
L003c:
	ld	r2,%0002(r15)
	ldb	@r2,#%72
	ld	r2,%0002(r15)
	ldb	%0001(r2),#%2B
L004e:
	ld	r2,%0002(r15)
	cpb	%0001(r2),#%2B
	ldk	r3,#0
	tcc	eq,r3
	extsb	r3
	ld	r13,r3
	ld	r2,%0002(r15)
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
L0082:
	ld	r7,%0004(r15)
	ld	r6,r13
	calr	L011c
	jr	L00d6
L008c:
	test	r13
	jr	eq,L0094
	ldk	r6,#2
	jr	L0096
L0094:
	ldk	r6,#1
L0096:
	ld	r7,%0004(r15)
	call	_open
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L00b8
	cp	_errno,#%0002
	jr	ne,L00b8
	ld	r7,%0004(r15)
	ld	r6,r13
	calr	L011c
	ld	r12,r2
L00b8:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	L00d8
L00c4:
	test	r13
	jr	eq,L00cc
	ldk	r6,#2
	jr	L00ce
L00cc:
	sub	r6,r6
L00ce:
	ld	r7,%0004(r15)
	call	_open
L00d6:
	ld	r12,r2
L00d8:
	cp	r12,#%0000
	jr	lt,L0014
	clr	%0002(r14)
	ld	r2,r12
	ldb	%0007(r14),rl2
	test	r13
	jr	eq,L00fa
	ldb	rl2,%0006(r14)
	orb	rl2,#%80
	ldb	%0006(r14),rl2
	jr	L010e
L00fa:
	ld	r2,%0002(r15)
	cpb	@r2,#%72
	jr	ne,L010a
	setb	%0006(r14),#0
	jr	L010e
L010a:
	setb	%0006(r14),#1
L010e:
	ld	r2,r14
L0110:
	ldm	r12,%000C(r15),#3
	add	r15,#%0012
	ret
L011c:
	dec	r15,#16
	ldm	%000A(r15),r12,#3
	ld	@r15,r6
	ld	r14,r7
	test	r6
	jr	eq,L016c
	sub	r7,r7
	call	_umask
	ld	r13,r2
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
	cp	r2,#%0000
	jr	lt,L0164
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
L0164:
	ld	r7,r13
	call	_umask
	jr	L0178
L016c:
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
L0178:
	ld	r2,r12
	ldm	r12,%000A(r15),#3
	inc	r15,#16
	ret
	.psec
	.data
D0000:
	.byte	%00,%61,%00,%72,%00,%77
	.addr	L008c
	.addr	L00c4
D000a:
	.addr	L0082
