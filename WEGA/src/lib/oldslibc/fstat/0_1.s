	.seg
	.extern	_cerror
	.code

_fstat::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	xor	r4,r4
	sc	#%1C
	ld	r2,r4
	ret	nc
	jp	_cerror
