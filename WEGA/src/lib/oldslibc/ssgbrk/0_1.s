	.seg
	.extern	_cerror
	.code

_ssgbrk::
	ld	r0,r7
	ld	r1,r6
	ldl	rr4,#%00000000
	sc	#%2D
	ldl	rr2,rr4
	jp	c,_cerror
	ret
