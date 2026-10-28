	.seg
	.extern	_cerror
	.code

_open::
	ldl	rr0,rr6
	ld	r2,r5
	ld	r3,r4
	sc	#%05
	ld	r2,r4
	ret	nc
	jp	_cerror
