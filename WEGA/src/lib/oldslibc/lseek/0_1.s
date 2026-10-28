	.seg
	.extern	_cerror
	.code

_lseek::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	subl	rr4,rr4
	sc	#%13
	ldl	rr2,rr4
	ret	nc
	jp	_cerror
