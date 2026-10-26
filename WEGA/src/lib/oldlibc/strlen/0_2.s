	.psec
	.code
_strlen::
	dec	r15,#4
	ld	r1,r7
	ldk	r3,#0
	jr	Ld0a6
Ld0a4:
	inc	r3,#1
Ld0a6:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,Ld0a4
	ld	r2,r3
	inc	r15,#4
	ret
