	.extern	__filbuf, __flsbuf
	.psec
	.code
_fread::
	sub	r15,#%0018
	ldm	%0012(r15),r12,#3
	ldm	%0004(r15),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	%000C(r15)
	test	r6
	jr	ne,L0062
	jr	L006c
L001e:
	ld	r3,%0006(r15)
	ld	%000E(r15),r3
L0026:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L0042
	ld	r2,@r13
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	ld	@r15,r3
	inc	@r13,#1
	jr	L004a
L0042:
	ld	r7,r13
	call	__filbuf
	ld	@r15,r2
L004a:
	ld	r12,@r15
	cp	r12,#%0000
	jr	lt,L006c
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
	dec	%000E(r15),#1
	jr	ne,L0026
	inc	%000C(r15),#1
L0062:
	ld	r2,%000C(r15)
	cp	r2,%0004(r15)
	jr	c,L001e
L006c:
	ld	r2,%000C(r15)
	ldm	r12,%0012(r15),#3
	add	r15,#%0018
	ret
_fwrite::
	sub	r15,#%0016
	ldm	%0010(r15),r12,#3
	ldm	%0004(r15),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	%000C(r15)
	test	r6
	jr	ne,L00e0
	jr	L00ea
L009a:
	ld	r12,%0006(r15)
L009e:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L00ba
	ld	r2,@r13
	ldb	rl3,@r14
	ldb	@r2,rl3
	extsb	r3
	ld	@r15,r3
	inc	r14,#1
	inc	@r13,#1
	jr	L00cc
L00ba:
	ld	r7,r14
	inc	r14,#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,r13
	call	__flsbuf
	ld	@r15,r2
L00cc:
	djnz	r12,L009e
	ldb	rl2,%0006(r13)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L00ea
	inc	%000C(r15),#1
L00e0:
	ld	r2,%000C(r15)
	cp	r2,%0004(r15)
	jr	c,L009a
L00ea:
	ld	r2,%000C(r15)
	ldm	r12,%0010(r15),#3
	add	r15,#%0016
	ret
