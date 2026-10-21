	.extern	__filbuf, __flsbuf
	.psec
	.code
_fread::
	sub	r15,#%0016
	ldm	%0010(r15),r12,#3
	ldm	%0002(r15),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	%000A(r15)
	test	r6
	jr	ne,L002e
	jr	L006a
L001e:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
	dec	%000C(r15),#1
	jr	ne,L0040
	inc	%000A(r15),#1
L002e:
	ld	r2,%000A(r15)
	cp	r2,%0002(r15)
	jr	nc,L006a
	ld	r3,%0004(r15)
	ld	%000C(r15),r3
L0040:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L005a
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	jr	L0062
L005a:
	ld	r7,r13
	call	__filbuf
	ld	r3,r2
L0062:
	ld	r12,r3
	cp	r3,#%0000
	jr	ge,L001e
L006a:
	ld	r2,%000A(r15)
	ldm	r12,%0010(r15),#3
	add	r15,#%0016
	ret
_fwrite::
	sub	r15,#%0014
	ldm	%000E(r15),r12,#3
	ldm	%0002(r15),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	%000A(r15)
	test	r6
	jr	ne,L00de
	jr	L00e8
L0098:
	ld	r12,%0004(r15)
L009c:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L00b8
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r13
	inc	@r13,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	extsb	r4
	jr	L00ca
L00b8:
	ld	r7,r14
	inc	r14,#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,r13
	call	__flsbuf
	ld	r4,r2
L00ca:
	djnz	r12,L009c
	ldb	rl2,%0006(r13)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L00e8
	inc	%000A(r15),#1
L00de:
	ld	r2,%000A(r15)
	cp	r2,%0002(r15)
	jr	c,L0098
L00e8:
	ld	r2,%000A(r15)
	ldm	r12,%000E(r15),#3
	add	r15,#%0014
	ret
