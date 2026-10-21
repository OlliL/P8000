	.psec
	.code
_strlen::
	dec	r15,#10
	ld	r1,r7
	ldk	r3,#0
	jr	L000a
L0008:
	inc	r3,#1
L000a:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L0008
	ld	r2,r3
	inc	r15,#10
	ret
