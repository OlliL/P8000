	.seg
	.extern	_cerror
	.code

_ioctl::
	ld	r0,r7
	ld	r1,r6
	ldl	rr2,rr4
	ldl	rr4,#%00000000
	sc	#%36
	ld	r2,r4
	ret	nc
	jp	_cerror
