	.seg
	.extern	_cerror
	.code

_mkseg::
	ld	r0,r7
	ld	r1,r6
	sc	#%37
	ldl	rr2,rr4
	jp	c,_cerror
	ret
