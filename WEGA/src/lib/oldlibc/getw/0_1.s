	.extern	__filbuf
	.psec
	.code
_getw::
	dec	r15,#4
	ldm	%0000(r15),r13,#2
	ld	r14,r7
	dec	%0002(r14),#1
	cp	%0002(r14),#%0000
	jr	lt,Lc648
	ld	r2,@r14
	inc	@r14,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	Lc64e
Lc648:
	ld	r7,r14
	call	__filbuf
Lc64e:
	ld	r13,r2
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#4
	jr	eq,Lc660
	ld	r2,#%FFFF
	jr	Lc68a
Lc660:
	dec	%0002(r14),#1
	cp	%0002(r14),#%0000
	jr	lt,Lc67a
	ld	r2,@r14
	inc	@r14,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	Lc680
Lc67a:
	ld	r7,r14
	call	__filbuf
Lc680:
	ld	r3,r13
	ldb	rh3,rl3
	clrb	rl3
	or	r3,r2
	ld	r2,r3
Lc68a:
	ldm	r13,%0000(r15),#2
	inc	r15,#4
	ret
