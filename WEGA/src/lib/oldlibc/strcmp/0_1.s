	.psec
	.code
_strcmp::
	dec	r15,#4
	ld	r1,r7
	ld	r4,r6
L0006:
	ld	r2,r4
	inc	r4,#1
	ldb	rl2,@r2
	cpb	rl2,@r1
	jr	ne,L001e
	ld	r2,r1
	inc	r1,#1
	ldb	rl2,@r2
	testb	rl2
	jr	ne,L0006
	sub	r2,r2
	jr	L002a
L001e:
	ldb	rl2,@r1
	extsb	r2
	dec	r4,#1
	ldb	rl3,@r4
	extsb	r3
	sub	r2,r3
L002a:
	inc	r15,#4
	ret
