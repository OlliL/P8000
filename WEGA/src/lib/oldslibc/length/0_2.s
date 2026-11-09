	.seg
	.extern	_stkseg
	.psec
	.code

__length::
	dec	r15,#14
	ldm	_stkseg+8(r15),r11,#3
	ldl	rr12,rr6
	ldk	r11,#0
Lae6a:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	testb	rl4
	jr	eq,Lae7c
	inc	r11,#1
	cp	r11,#%4000
	jr	le,Lae6a
Lae7c:
	ld	r2,r11
	ldm	r11,_stkseg+8(r15),#3
	inc	r15,#14
	ret
