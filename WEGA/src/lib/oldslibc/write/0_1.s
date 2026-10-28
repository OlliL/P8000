	.seg
	.extern	_cerror
	.code

_write::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#%04
	ld	r2,r4
	ret	nc
	jp	_cerror
