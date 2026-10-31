	.psec
	.code
__length::
	dec	r15,#4
	ld	r1,r7
	ldk	r3,#0
L0006:
	ld	r2,r1
	inc	r1,#1
	ldb	rl2,@r2
	testb	rl2
	jr	eq,L0018
	inc	r3,#1
	cp	r3,#%4000
	jr	le,L0006
L0018:
	ld	r2,r3
	inc	r15,#4
	ret
