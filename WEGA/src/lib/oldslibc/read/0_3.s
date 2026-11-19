	.seg
	.extern	chkseg
	.extern	cerror
	.code

_read::
	ld	r2,#%0016
	ld	r0,r4
	call	chkseg
	cp	r0,#%FFFF
	jp	eq,cerror
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#%03
	ld	r2,r4
	ret	nc
	jp	cerror
