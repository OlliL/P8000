	.seg
	.extern	_stkseg
	.psec
	.code

_strlen::
	dec	r15,#14
	ld	_stkseg+8(r15),r11
	ldl	rr4,rr6
	ldk	r11,#0
	jr	Lb432
Lb430:
	inc	r11,#1
Lb432:
	ldl	rr2,rr4
	inc	r5,#1
	testb	@rr2
	jr	ne,Lb430
	ld	r2,r11
	ld	r11,_stkseg+8(r15)
	inc	r15,#14
	ret
