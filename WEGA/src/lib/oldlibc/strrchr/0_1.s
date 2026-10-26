	.psec
	.code
_strrchr::
	dec	r15,#6
	ld	%0000(r15),r6
	ld	r1,r7
	ldk	r3,#0
L5496:
	ldb	rl2,@r1
	cpb	rl2,%0001(r15)
	jr	ne,L54a0
	ld	r3,r1
L54a0:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L5496
	ld	r2,r3
	inc	r15,#6
	ret
