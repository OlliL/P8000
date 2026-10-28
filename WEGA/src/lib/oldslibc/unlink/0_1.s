	.seg
	.extern	_cerror
	.code

_unlink::
	ldl	rr0,rr6
	xor	r4,r4
	sc	#%0A
	ld	r2,r4
	ret	nc
	jp	_cerror
