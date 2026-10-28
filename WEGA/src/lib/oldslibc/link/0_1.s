	.seg
	.extern	_cerror
	.code

_link::
	ldl	rr0,rr6
	ldl	rr2,rr4
	xor	r4,r4
	sc	#%09
	ld	r2,r4
	ret	nc
	jp	_cerror
