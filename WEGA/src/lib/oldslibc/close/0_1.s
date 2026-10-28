	.seg
	.extern	_cerror
	.code

_close::
	ld	r0,r7
	xor	r4,r4
	sc	#%06
	ld	r2,r4
	ret	nc
	jp	_cerror
