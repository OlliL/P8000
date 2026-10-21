	.psec
	.code
__length::
	dec	r15,#10
	ld	r1,r7
	ldk	r4,#0
L0006:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	testb	rl3
	jr	eq,L0018
	inc	r4,#1
	cp	r4,#%4000
	jr	le,L0006
L0018:
	ld	r2,r4
	inc	r15,#10
	ret
