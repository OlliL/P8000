! @[$] signal.s	2.1  09/12/83 11:43:15 - 87wega3.2 !

signal module

$SECTION signal_b

  internal
    L1
      ARRAY [20 WORD]

$SDEFAULT

  external
    cerror	procedure

  global
    _signal procedure
      entry
	ld	r2,#22
	ld	r0,r7
	ld	r1,r6
	cp	r0,#%0014
	jp	ge,cerror
	ld	r3,r0
	sla	r3,#1
	ld	r2,L1(r3)
	ld	L1(r3),r1
	rr	r1,#1
	rl	r1,#1
	jr	ule,L2
	lda	r1,L5(r3)
L2:
	sc	#48
	bit	r4,#0
	jr	z,L4
	ld	r2,r4
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
	ldm	@r15,r0,#15
	ld	r7,30(r15)
	sub	r7,#L6
	ld	r2,L1(r7)
	srl	r7,#1
	call	@r2
	ldm	r0,@r15,#15
	add	r15,#32
	sc	#0
    end _signal
end signal
