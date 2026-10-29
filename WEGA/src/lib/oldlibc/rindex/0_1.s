	.psec
	.code
_rindex::
	dec	r15,#12
	ld	@r15,r6
	ld	r1,r7
	ldk	r3,#0
L0008:
	ldb	rl2,@r1
	cpb	rl2,%0001(r15)
	jr	ne,L0012
	ld	r3,r1
L0012:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L0008
	ld	r2,r3
	inc	r15,#12
	ret
