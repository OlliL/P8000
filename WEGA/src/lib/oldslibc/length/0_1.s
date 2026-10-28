	.seg
	.extern	_stkseg
	.code

__length::
	dec	r15,#12
	ldm	_stkseg+6(r15),r11,#3
	ldl	rr12,rr6
	ldk	r11,#0
L8c94:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	testb	rl4
	jr	eq,L8ca6
	inc	r11,#1
	cp	r11,#%4000
	jr	le,L8c94
L8ca6:
	ld	r2,r11
	ldm	r11,_stkseg+6(r15),#3
	inc	r15,#12
	ret
