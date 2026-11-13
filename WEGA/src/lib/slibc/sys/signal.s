! @[$] signal.s	2.1  09/12/83 11:43:15 - 87wega3.2 !

signal module

$segmented

$SECTION signal_b

  internal
    L1
      ARRAY [20 LONG]

$SDEFAULT

  external
    cerror	procedure

  global
    _signal procedure
      entry
	ld	r0,r7
	cp	r0,#%0014
	jp	ge,cerror
	ld	r1,r4
	ld	r2,r5
	ld	r3,r0
	sla	r3,#1
	sla	r3,#1
	ldl	rr6,L1(r3)
	ldl	L1(r3),rr4
	cpl	rr4,#0
	jr	z,L2
	cpl	rr4,#1
	jr	z,L2
	sra	r3,#1
	lda	rr4,L5(r3)
	sra	r3,#1
	ld	r0,r3
	ld	r1,r4
	ld	r2,r5
L2:
	sc	#48
	bit	r5,#0
	jr	z,L3
	ldl	rr2,rr4
	jr	L4
L3:
	ldl	rr2,rr6
L4:
	ret	nc
	jp	cerror
L5:
	calr	L7
L6:
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
	calr	L7
L7:

	sub	r15,#30
	ldm	@rr14,r0,#15
	ld	r7,rr14(#32)
	ldl	rr4,#L6
	sub	r7,r5
	sla	r7,#1
	ldl	rr2,L1(r7)
	srl	r7,#1
	srl	r7,#1
	call	@rr2
	ldm	r0,@rr14,#15
	addl	rr14,#34
	sc	#0
    end _signal
end signal
