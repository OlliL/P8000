	.extern	__filbuf, __iob
	.psec
	.code
_gets::
	dec	r15,#12
	ldm	%0008(r15),r13,#2
	ld	@r15,r7
	ld	r13,r7
	jr	L001a
L000e:
	cp	r14,#%0000
	jr	lt,L004a
	ld	r2,r14
	ldb	@r13,rl2
	inc	r13,#1
L001a:
	dec	__iob+2,#1
	cp	__iob+2,#%0000
	jr	lt,L0038
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	jr	L0042
L0038:
	ld	r7,#__iob
	call	__filbuf
	ld	r3,r2
L0042:
	ld	r14,r3
	cp	r3,#%000A
	jr	ne,L000e
L004a:
	cp	r14,#%0000
	jr	ge,L005a
	cp	r13,%0000(r15)
	jr	ne,L005a
	sub	r2,r2
	jr	L0062
L005a:
	clrb	@r13
	inc	r13,#1
	ld	r2,%0000(r15)
L0062:
	ldm	r13,%0008(r15),#2
	inc	r15,#12
	ret
