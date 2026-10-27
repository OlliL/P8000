	.seg
	.code

_stubs::
	ldl	rr0,rr6
	xor	r4,r4
	sc	#%0A
	ld	r2,r4
	ret	nc
	jp	L971c
_f970a::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#%04
	ld	r2,r4
	ret	nc
	jp	L971c
L971c:
_f971c::
	ld	_d1BAE+0,r2
	ld	_d1BAE+2,r5
	ldl	rr2,#%FFFFFFFF
	ret
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00

	.data

_d1BAE::
	.word	%0000
	.word	%0000
