	.psec
	.code
_swab::
	dec	r15,#6
	ld	%0000(r15),r12
	ld	r1,r7
	ld	r4,r6
	ld	r3,r5
	exts	rr2
	div	rr2,#%0002
	ld	r12,r3
	jr	L002e
L0016:
	ld	r2,@r1
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,@r1
	ldb	rl3,rh3
	extsb	r3
	and	r3,#%00FF
	add	r2,r3
	ld	@r4,r2
	inc	r4,#2
	inc	r1,#2
L002e:
	dec	r12,#1
	cp	r12,#%0000
	jr	ge,L0016
	ld	r12,%0000(r15)
	inc	r15,#6
	ret
