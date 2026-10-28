	.seg
	.extern	_cerror
	.code

_creat::
	ldl	rr0,rr6
	ld	r2,r5
	sc	#%08
	ld	r2,r4
	ret	nc
	jp	_cerror
