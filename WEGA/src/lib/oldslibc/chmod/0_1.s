	.seg
	.extern	_cerror
	.code

_chmod::
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#%0F
	ld	r2,r4
	ret	nc
	jp	_cerror
