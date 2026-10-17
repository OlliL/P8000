// length.o: magic E711 nonsegmented
	.extern	__length

// segment table entry 0 (segment 0), text, 30 bytes
	.psec
	.code
__length::
	dec	r15,#8
	ld	r1,r7
	ldk	r4,#0
L0t_0006:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	testb	rl3
	jr	eq,L0t_0018
	inc	r4,#1
	cp	r4,#%4000
	jr	le,L0t_0006
L0t_0018:
	ld	r2,r4
	inc	r15,#8
	ret
