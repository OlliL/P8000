	.extern	cerror
	.psec
	.code
_times::
	ld	r0,r7
	xor	r4,r4
	sc	#%2B
	ld	r2,r4
	ret	nc
	jp	cerror
