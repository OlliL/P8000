	.seg
	.extern	_bss1
	.extern	_f971c
	.code

_signal::
	ld	r0,r7
	ld	r1,r6
	ldl	rr2,rr4
	ldl	rr4,#%00000000
	sc	#%36
	ld	r2,r4
	ret	nc
	jp	_f971c
_f95c8::
	ldl	rr0,rr6
	ldl	rr2,rr4
	xor	r4,r4
	sc	#%09
	ld	r2,r4
	ret	nc
	jp	_f971c
_f95da::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	subl	rr4,rr4
	sc	#%13
	ldl	rr2,rr4
	ret	nc
	jp	_f971c
_f95ee::
	ld	r0,r7
	ld	r1,r6
	sc	#%37
	ldl	rr2,rr4
	jp	c,_f971c
	ret
_f95fe::
	ldl	rr0,rr6
	ld	r2,r5
	ld	r3,r4
	sc	#%05
	ld	r2,r4
	ret	nc
	jp	_f971c
_f9610::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	sc	#%03
	ld	r2,r4
	ret	nc
	jp	_f971c
_f9622::
	ld	r0,r7
	ld	r1,r6
	ldl	rr4,#%00000000
	sc	#%2D
	ldl	rr2,rr4
	jp	c,_f971c
	ret
_f9638::
	ld	r0,r7
	cp	r0,#%0014
	jp	ge,_f971c
	ld	r1,r4
	ld	r2,r5
	ld	r3,r0
	sla	r3,#1
	sla	r3,#1
	ldl	rr6,_bss1+0(r3)
	ldl	_bss1+0(r3),rr4
	cpl	rr4,#%00000000
	jr	eq,L9682
	cpl	rr4,#%00000001
	jr	eq,L9682
	sra	r3,#1
	lda	rr4,L9696(r3)
	sra	r3,#1
	ld	r0,r3
	ld	r1,r4
	ld	r2,r5
L9682:
	sc	#%30
	bit	r5,#0
	jr	eq,L968c
	ldl	rr2,rr4
	jr	L968e
L968c:
	ldl	rr2,rr6
L968e:
	ret	nc
	jp	_f971c
L9696:
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
	calr	L96be
L96be:
	sub	r15,#%001E
	ldm	@rr14,r0,#15
	ld	r7,rr14(#%0020)
	ldl	rr4,#%80009698
	sub	r7,r5
	sla	r7,#1
	ldl	rr2,_bss1+0(r7)
	srl	r7,#1
	srl	r7,#1
	call	@rr2
	ldm	r0,@rr14,#15
	addl	rr14,#%00000022
	sc	#%00
