	.seg
	.extern	_cerror
	.code

_signal::
	ld	r0,r7
	cp	r0,#%0014
	jp	ge,_cerror
	ld	r1,r4
	ld	r2,r5
	ld	r3,r0
	sla	r3,#1
	sla	r3,#1
	ldl	rr6,<<1>>%1BB2(r3)
	ldl	<<1>>%1BB2(r3),rr4
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
	jp	_cerror
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
	ldl	rr2,<<1>>%1BB2(r7)
	srl	r7,#1
	srl	r7,#1
	call	@rr2
	ldm	r0,@rr14,#15
	addl	rr14,#%00000022
	sc	#%00
