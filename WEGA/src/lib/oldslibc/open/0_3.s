	.seg
	.extern	chkseg
	.extern	cerror
	.code

_open::
	ld	r2,#%0016
	ld	r0,r6
	call	chkseg
	cp	r0,#%FFFF
	jp	eq,cerror
	ldl	rr0,rr6
	ld	r2,r5
	ld	r3,r4
	sc	#%05
	ld	r2,r4
	ret	nc
	jp	cerror
