! ecvt.s  reconstructed from the shipped ecvt.o with host/tools/s2as.py !

ecvt module

$segmented

  external
    __pow10	procedure

  internal
    D0000:
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
	WORD := %0000
    D0020:
	WORD := %494e
	WORD := %4600
    D0024:
	WORD := %4e41
	WORD := %4e28
	WORD := %3030
	WORD := %3030
	WORD := %3000

  global
    _ecvt procedure
      entry
	call	__cvt
	ret
    end _ecvt

    _fcvt procedure
      entry
	neg	r7
	call	__cvt
	ret
    end _fcvt

    __cvt procedure
      entry
	dec	r15,#12
	ldm	@rr14,r8,#6
	clr	@rr2
	clr	@rr4
	fld	r8,f3
	bit	r8,#15
	jr	z,L1
	set	@rr2,#0
	fabs	f3,f3
L1:
	res	r8,#15
	cp	r8,#%7FFF
	jr	nz,L6
	or	r9,r10
	or	r9,r11
	or	r9,r12
	jr	nz,L2
	lda	rr2,D0000
	lda	rr4,D0020
	ldk	r1,#4
	ldirb	@rr2,@rr4,r1
	jp	L24
L2:
	lda	rr2,D0000
	lda	rr4,D0024
	ldk	r1,#4
	ldirb	@rr2,@rr4,r1
	flds	r4,f3
	res	r4,#7
	ldk	r1,#6
L3:
	subb	rh4,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	cpb	rh4,#%09
	jr	gt,L4
	orb	rh4,#%30
	jr	L5
L4:
	addb	rh4,#%36
L5:
	ldb	@rr2,rh4
	inc	r3,#1
	djnz	r1,L3
	ldb	@rr2,#%29
	inc	r3,#1
	clrb	@rr2
	jp	L24
L6:
	fcpz	f3
	jr	nz,L8
L7:
	ld	r2,#%3000
	ld	D0000,r2
	jp	L24
L8:
	cp	r7,#%0013
	jp	le,LS1
	ld	r7,#%0013
LS1:
	pushl	@rr14,rr4
	dec	r15,#10
	fld	@rr14,f5
	dec	r15,#10
	fld	@rr14,f4
	fld	f4,f3
	sub	r6,r6
	cp	r7,#%0000
	jr	le,L10
	fld	r0,f3
	sub	r0,#%3FFF
	bit	r1,#15
	jr	nz,L9
	sub	r1,r1
L9:
	add	r1,r1
	ldl	rr2,#%4D104D42
	multl	rq0,rr0
	ld	r6,r0
L10:
	ld	r5,r7
	cp	r5,#%0000
	jr	gt,L11
	neg	r5
	ld	r4,r5
	jr	L13
L11:
	sub	r5,r6
	dec	r5,#1
	ld	r4,r5
	jr	pl,L13
	neg	r4
L13:
	pushl	@rr14,rr6
	push	@rr14,r5
	ld	r7,r4
	call	__pow10
	fld	f1,f4
	pop	r5,@rr14
	bit	r5,#15
	jr	nz,L14
	fmul	f1,f0
	jr	L15
L14:
	fdiv	f1,f0
L15:
	fint	f1,f1
	fld	f5,f1
	popl	rr6,@rr14
	cp	r7,#%0000
	jr	le,L17
	pushl	@rr14,rr6
	call	__pow10
	popl	rr6,@rr14
	fcpf	f5,f0
	jr	c,L17
	jr	z,L16
	inc	r6,#1
	jr	L10
L16:
	inc	r6,#1
	ldl	rr0,#%41200000
	fdivs	f5,r0
L17:
	fld	f0,f5
	fld	f4,@rr14
	inc	r15,#10
	fld	f5,@rr14
	inc	r15,#10
	fldbcd	r0,f0
	andb	rh0,#%0F
	lda	rr10,D0000
	popl	rr8,@rr14
	inc	r6,#1
	ld	@rr8,r6
	cp	r7,#%0000
	jr	gt,L20
	fcpz	f0
	jr	z,L7
	sub	r6,r6
L18:
	orb	rh0,rh0
	jr	nz,L19
	calr	L25
	inc	r6,#1
	jr	L18
L19:
	sub	r6,#%0013
	sub	r7,r6
	ld	@rr8,r7
	ld	r7,r6
	neg	r7
	sub	r6,r6
	jr	L21
L20:
	ld	r6,r7
	sub	r6,#%0013
L21:
	orb	rh0,#%30
L22:
	inc	r6,#1
	jr	le,L23
	ldb	@rr10,rh0
	inc	r11,#1
L23:
	calr	L25
	cp	r6,r7
	jr	lt,L22
	clrb	@rr10
L24:
	lda	rr2,D0000
	ldm	r8,@rr14,#6
	inc	r15,#12
	ret
L25:
	rldb	rh0,rl0
	rldb	rl0,rh1
	rldb	rh1,rl1
	rldb	rl1,rh2
	rldb	rh2,rl2
	rldb	rl2,rh3
	rldb	rh3,rl3
	rldb	rl3,rh4
	rldb	rh4,rl4
	ret
    end __cvt

end ecvt
