	.seg
	.extern	_cerror
	.code

_read::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#%03
	ld	r2,r4
	ret	nc
	jp	_cerror
