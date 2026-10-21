	.extern	_fread
	.psec
	.code
_readsym::
	dec	r15,#8
	ld	%0006(r15),r14
	ldl	%0000(r15),rr6
	ldk	r6,#14
	ldk	r5,#1
	ld	r4,%0000(r15)
	call	_fread
	cp	r2,#%0001
	jr	eq,L0022
	ld	r2,#%FFFF
	jr	L0070
L0022:
	ld	r2,%0002(r15)
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#7
	jr	ne,L0034
	sub	r2,r2
	jr	L0070
L0034:
	ld	r2,%0002(r15)
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%007F
	dec	r2,#8
	ldb	%0005(r15),rl2
	extsb	r2
	inc	r2,#13
	ld	r1,r2
	exts	rr0
	div	rr0,#%000E
	ld	r14,r1
	ld	r7,%0002(r15)
	ldk	r6,#14
	ld	r5,r1
	ld	r4,%0000(r15)
	call	_fread
	cp	r14,r2
	jr	eq,L006e
	ld	r14,#%FFFF
L006e:
	ld	r2,r14
L0070:
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
