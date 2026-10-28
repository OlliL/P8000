	.seg
	.extern	_cerror
	.code

_access::
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#%21
	ld	r2,r4
	ret	nc
	jp	_cerror
