	.seg
	.extern	_stkseg
	.extern	_getpid
	.extern	_access
	.psec
	.code

_mktemp::
	sub	r15,#%0018
	ldm	_stkseg+16(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	call	_getpid
	ld	r11,r2
	ldl	rr12,|_stkseg+0|(r15)
Lb2b8:
	ldl	rr2,rr12
	inc	r13,#1
	testb	@rr2
	jr	ne,Lb2b8
	dec	r13,#1
	jr	Lb2dc
Lb2c4:
	ld	r3,r11
	ldk	r2,#0
	div	rr2,#%000A
	add	r2,#%0030
	ldb	@rr12,rl2
	ld	r3,r11
	ldk	r2,#0
	div	rr2,#%000A
	ld	r11,r3
Lb2dc:
	dec	r13,#1
	ldb	rl2,@rr12
	cpb	rl2,#%58
	jr	eq,Lb2c4
	inc	r13,#1
	ld	r10,#%0061
	jr	Lb302
Lb2ee:
	cp	r10,#%007A
	jr	ne,Lb2fc
	ldl	rr2,#D94d0
	jr	Lb318
Lb2fc:
	ld	r2,r10
	ldb	@rr12,rl2
	inc	r10,#1
Lb302:
	ldl	rr6,|_stkseg+0|(r15)
	sub	r5,r5
	call	_access
	cp	r2,#%FFFF
	jr	ne,Lb2ee
	ldl	rr2,|_stkseg+0|(r15)
Lb318:
	ldm	r10,_stkseg+16(r15),#4
	add	r15,#%0018
	ret
	.psec data
	.data
D94d0:
	.byte	%2f
	.byte	%00
