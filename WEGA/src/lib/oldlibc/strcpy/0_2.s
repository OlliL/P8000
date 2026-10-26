	.psec
	.code
_strcpy::
	dec	r15,#6
	ld	r1,r7
	ld	r4,r6
	ld	r5,r7
Ld086:
	ld	r2,r4
	inc	r4,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	testb	rl0
	jr	ne,Ld086
	ld	r2,r5
	inc	r15,#6
	ret
