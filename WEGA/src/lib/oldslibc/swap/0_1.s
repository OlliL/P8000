	.seg
	.extern	_stkseg
	.extern	_f971c
	.code

_swap::
L946e:
	dec	r15,#10
	ld	|_stkseg+0|(r15),r7
	ldl	rr2,#%7F000002
	add	r3,r15
	ldl	rr4,rr2
	ldb	rl6,|_stkseg+1|(r15)
	ldb	@rr4,rl6
	ldl	rr2,rr4
	inc	r3,#1
	ldb	rl6,|_stkseg+0|(r15)
	ldb	@rr2,rl6
	ld	r2,@rr4
	inc	r15,#10
	ret
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,|_stkseg+0|(r15)
	test	@rr12
	jr	eq,L94b0
	ld	r7,@rr12
	calr	L946e
	ld	@rr12,r2
L94b0:
	inc	r13,#2
	test	@rr12
	jr	eq,L94bc
	ld	r7,@rr12
	calr	L946e
	ld	@rr12,r2
L94bc:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	ld	r2,rr12(#%0004)
	test	r2
	jr	eq,L94ea
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
L94ea:
	ld	r2,rr12(#%0006)
	test	r2
	jr	eq,L94fc
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#6
	ld	@rr4,r2
L94fc:
	ld	r2,rr12(#%0008)
	test	r2
	jr	eq,L950e
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#8
	ld	@rr4,r2
L950e:
	ld	r2,rr12(#%000A)
	test	r2
	jr	eq,L9520
	ld	r7,r2
	calr	L946e
	ldl	rr4,rr12
	inc	r5,#10
	ld	@rr4,r2
L9520:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
_f952c::
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#%21
	ld	r2,r4
	ret	nc
	jp	_f971c
_f953e::
	dec	r15,#6
	ld	_stkseg+4(r15),r13
	ld	r13,r7
	cp	r7,#%0041
	jr	lt,L9558
	cp	r7,#%005A
	jr	gt,L9558
	sub	r13,#%FFE0
L9558:
	ld	r2,r13
	ld	r13,_stkseg+4(r15)
	inc	r15,#6
	ret
_f9564::
	ldl	rr0,rr6
	ld	r2,r5
	xor	r4,r4
	sc	#%0F
	ld	r2,r4
	ret	nc
	jp	_f971c
_f9576::
	ld	r0,r7
	xor	r4,r4
	sc	#%06
	ld	r2,r4
	ret	nc
	jp	_f971c
_f9586::
	ldl	rr0,rr6
	ld	r2,r5
	sc	#%08
	ld	r2,r4
	ret	nc
	jp	_f971c
_f9596::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	xor	r4,r4
	sc	#%1C
	ld	r2,r4
	ret	nc
	jp	_f971c
