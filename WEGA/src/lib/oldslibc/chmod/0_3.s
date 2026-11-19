	.seg
	.extern	chkseg
	.extern	cerror
	.code

_chmod::
	ld	r2,#%0016
	ld	r0,r6
	call	chkseg
	cp	r0,#%FFFF
	jp	eq,cerror
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#%0F
	ld	r2,r4
	ret	nc
	jp	cerror
