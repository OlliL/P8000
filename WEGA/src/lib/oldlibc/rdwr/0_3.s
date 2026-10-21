	.extern	__filbuf, __flsbuf
	.psec
	.code
_fread::
	dec	r15,#14
	ldm	%0008(r15),r12,#3
	ldm	%0000(r15),r5,#2
	ld	r14,r7
	ld	r13,r4
	clr	%0004(r15)
	test	r6
	jr	ne,L002c
	jr	L0066
L001c:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
	dec	%0006(r15),#1
	jr	ne,L003e
	inc	%0004(r15),#1
L002c:
	ld	r2,%0004(r15)
	cp	r2,%0000(r15)
	jr	nc,L0066
	ld	r2,%0002(r15)
	ld	%0006(r15),r2
L003e:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L0058
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L005e
L0058:
	ld	r7,r13
	call	__filbuf
L005e:
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L001c
L0066:
	ld	r2,%0004(r15)
	ldm	r12,%0008(r15),#3
	inc	r15,#14
	ret
_fwrite::
	dec	r15,#12
	ldm	%0006(r15),r12,#3
	ldm	%0000(r15),r5,#2
	ld	r14,r7
	ld	r13,r4
	clr	%0004(r15)
	test	r6
	jr	ne,L00d4
	jr	L00de
L0090:
	ld	r12,%0002(r15)
L0094:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L00b0
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r13
	inc	@r13,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	L00c0
L00b0:
	ld	r7,r14
	inc	r14,#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,r13
	call	__flsbuf
	ld	r0,r2
L00c0:
	djnz	r12,L0094
	ldb	rl2,%0006(r13)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L00de
	inc	%0004(r15),#1
L00d4:
	ld	r2,%0004(r15)
	cp	r2,%0000(r15)
	jr	c,L0090
L00de:
	ld	r2,%0004(r15)
	ldm	r12,%0006(r15),#3
	inc	r15,#12
	ret
