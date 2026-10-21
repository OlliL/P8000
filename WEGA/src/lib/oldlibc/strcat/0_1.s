	.psec
	.code
_strcat::
	dec	r15,#14
	ld	%0008(r15),r12
	ld	r5,r6
	ld	r1,r7
	ld	r12,r7
L000c:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L000c
	dec	r1,#1
L0016:
	ld	r2,r5
	inc	r5,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jr	ne,L0016
	ld	r2,r12
	ld	r12,%0008(r15)
	inc	r15,#14
	ret
