fpe module

$segmented

constant
sp := r15

stareg	:= %0000
staevnt	:= %0020
stafcw	:= %0022
stapcsg	:= %0024

internal
!
freecpu procedure
  entry
	ret
end freecpu

needcpu procedure
  entry
	ret
end needcpu
!
! siehe mch.s
gettext procedure
  entry
	ld	r1, rr8(#stafcw)
	bit	r1, #15
	jr	nz, L_002e

	push	@rr14, #%0000
	ldl	rr4, rr14
	call	_fuiword
	pop	r3, @rr14
	ret
L_002e:
	push	@rr14, #%0000
	ldl	rr4, rr14
	call	_fuword
	pop	r3, @rr14
	ret
end gettext

getmem procedure
  entry
	ld	r1, rr8(#stafcw)
	bit	r1, #15
	jp	nz, _copyin
	call	_nsseg
	ld	r6, r2
	jp	_copyin
end getmem

putmem procedure
  entry
	ld	r1, rr8(#stafcw)
	bit	r1, #15
	jp	nz, _copyout
	ex	r4, r6
	ex	r5, r7
	call	_nsseg
	ex	r4, r6
	ex	r5, r7
	ld	r4, r2
	jp	_copyout
end putmem
!

actadd array [* long] := [ addno, addsrc0, ressrc, resnsrc,
			   adddst0, addno, ressrc, resnsrc,
			   resdst, resdst, addinf, resnsrc,
			   resndst, resndst, resndst, resprec,
			   dotrap ]

Fcvt procedure
  entry
Fsub:
bumpup:
	or	r4, #%3010
	callr	setup
	xor	r6, #%8000
	jr	L_00b8
Fadd:
	or	r4, #%3010
	callr	setup
L_00b8:
	ldl	rr2, actadd(r1)
	call	@rr2
	jpr	delivery
end Fcvt

addinf procedure
  entry
	resflg	p
	ld	r1, rr12(#%0060)
	xor	r1, r6
	lda	rr2, rr12(#%005d)
	bitb	@rr2, #0
	jpr	le, resinv
	ret
end addinf

addsrc0 procedure
  entry
	lda	rr6, rr12(#%0060)
	ldm	r6, @rr6, #6
	jr	adddst0
 end addsrc0

addno procedure
  entry
	pushl	@rr14, rr4
	lda	rr2, rr12(#%0060)
	ldm	r0, @rr2, #6
	xorb	rh0, rh6
	sub	r1, r7
	jr	z, L_0128
	sbcb	rl0, rl6
	jr	mi, L_010a
	ldl	rr6, rr12(#%0060)
	ex	r11, r5
	ex	r10, r4
	ex	r9, r3
	ex	r8, r2
	jr	L_010c
L_010a:
	neg	r1
L_010c:
	cp	r1, #%0042
	jr	ule, L_0116
	ld	r1, #%0042
L_0116:
	subb	rl0, rl0
	rrc	r2
	rrc	r3
	rrc	r4
	rrc	r5
	rrcb	rh1
	tccb	c, rh1
	decb	rl1
	jr	nz, L_0116
L_0128:
	bitb	rh0, #7
	jr	nz, L_0148
	addl	rr10, rr4
	adc	r9, r3
	adc	r8, r2
	popl	rr4, @rr14
	ldb	rl5, rh1
	jr	nc, adddst0
	ldk	r0, #1
	callr	shiftdigs
	add	r7, #%0001
	adcb	rl6, rh0
	jr	adddst0
L_0148:
	cpl	rr8, rr2
	jr	ugt, L_0160
	jr	c, L_0154
	cpl	rr10, rr4
	jr	ugt, L_0160
	jr	z, L_0176
L_0154:
	ex	r8, r2
	ex	r9, r3
	ex	r10, r4
	ex	r11, r5
	xorb	rh6, #%80
L_0160:
	subb	rl1, rh1
	sbc	r11, r5
	sbc	r10, r4
	popl	rr4, @rr14
	ldb	rl5, rl1
	sbc	r9, r3
	sbc	r8, r2
	callr	normdigs
	jr	adddst0
L_0176:
	popl	rr4, @rr14
	subl	rr10, rr10
	ldl	rr8, rr10
	ldl	rr6, rr8
	ld	r1, #%0606
	lda	rr2, rr12(#%005d)
	andb	rh1, @rr2
	cpb	rh1, rl1
	ret	nz
	setb	rh6, #7
	ret
end addno

adddst0 procedure
  entry
	callr	under
	callr	round
	jpr	over
end adddst0

actmul array [* long] := [ mulno, donone, donone, resnsrc,
			   resdst, donone, resinv, resnsrc,
			   resdst, resinv, donone, resnsrc,
			   resndst, resndst, resndst, resprec,
			   dotrap ]

Fmul procedure
  entry
	or	r4, #%3010
	callr	setup
	ld	r0, rr12(#%0060)
	xor	r0, r6
	push	@rr14, r0
	ldl	rr2, actmul(r1)
	call	@rr2
	pop	r0, @rr14
	ldb	rh6, rh0
	jpr	delivery
end Fmul

mulno procedure
  entry
	pushl	@rr14, rr4
	ldl	rr0, rr12(#%0060)
	xorb	rh6, rh0
	addl	rr0, rr6
	subl	rr0, #%00003ffe
	ldb	rh0, rh6
	pushl	@rr14, rr0
	ldl	rr6, rr12(#%0068)
	testl	rr10
	jr	nz, L_01f4
	push	@rr14, #%0000
	ldl	rr4, rr8
	subl	rr8, rr8
	testl	rr6
	jr	z, L_0230
	jr	L_0216
L_01f4:
	ldl	rr4, rr10
	calr	L_026e
	or	r3, r3
	tcc	nz, r2
	push	@rr14, r2
	ldl	rr10, rr0
	ldl	rr6, rr12(#%0064)
	calr	L_026e
	ldl	rr4, rr8
	ldl	rr8, rr0
	sub	r6, r6
	addl	rr10, rr2
	adc	r9, r6
	adc	r8, r6
	ldl	rr6, rr12(#%0068)
L_0216:
	calr	L_026e
	addl	rr2, rr10
	or	r3, @rr14
	tcc	nz, r2
	orb	rl2, rl2
	tccb	nz, rh2
	ld	@rr14, r2
	adc	r9, r1
	adc	r8, r0
	ldl	rr10, rr8
	ldk	r9, #0
	ld	r8, r9
	adc	r9, r9
L_0230:
	ldl	rr6, rr12(#%0064)
	calr	L_026e
	addl	rr10, rr2
	adc	r9, r1
	adc	r8, r0
	pop	r0, @rr14
	popl	rr6, @rr14
	bit	r8, #15
	jr	nz, L_0256
	addb	rl0, rl0
	adc	r11, r11
	adc	r10, r10
	adc	r9, r9
	adc	r8, r8
	subb	rl0, rl0
	sub	r7, #%0001
	sbcb	rl6, rl0
L_0256:
	popl	rr4, @rr14
	orb	rl5, rh0
	callr	under
	callr	round
	jpr	over
L_026e:
	ldl	rr2, rr6
	multl	rq0, rr4
	ret	z
L_0274:
	jr	mi, L_027e
	bit	r4, #15
	ret	z
L_027a:
	addl	rr0, rr6
	jr	L_0286
L_027e:
	bit	r4, #15
	jr	z, L_0286
	addl	rr0, rr6
	ret
L_0286:
	addl	rr0, rr4
	ret
end mulno

actdiv array [* long] := [ divno, divdz, reszero, resnsrc,
			   reszero, resinv, reszero, resnsrc,
			   resdst, resdst, resinv, resnsrc,
			   resndst, resndst, resndst, resprec,
			   dotrap ]

Fdiv procedure
  entry
	or	r4, #%3010
	callr	setup
	ld	r0, rr12(#%0060)
	xor	r0, r6
	push	@rr14, r0
	ldl	rr2, actdiv(r1)
	call	@rr2
	pop	r0, @rr14
	ldb	rh6, rh0
	jpr	delivery
end Fdiv

divno procedure
  entry
	bit	r8, #15
	jpr	z, resinv
	pushl	@rr14, rr4
	lda	rr2, rr12(#%0060)
	ldm	r0, @rr2, #6
	xorb	rh6, rh0
	subl	rr0, rr6
	addl	rr0, #%00003fff
	ld	r7, #%0040
	cpl	rr2, rr8
	jr	nz, L_02d6
	cpl	rr4, rr10
L_02d6:
	jr	nc, L_02e0
	inc	r7
	subl	rr0, #%00000001
L_02e0:
	ldb	rh0, rh6
	pushl	@rr14, rr0
	pushl	@rr14, rr12
	ld	r0, r7
	ldl	rr6, rr4
	ldl	rr4, rr2
	ldl	rr2, rr8
	ldl	rr12, rr10
	callr	nonrestore
	bit	r11, #0
	jr	nz, L_0300
	addl	rr6, rr12
	adc	r5, r3
	adc	r4, r2
L_0300:
	callr	remvsdiv
	popl	rr12, @rr14
	popl	rr6, @rr14
	popl	rr4, @rr14
	orb	rl5, rl0
	callr	under
	callr	round
	jpr	over
end divno

divdz procedure
  entry
	setb	rh5, #3
	jpr	resinf
end divdz

nonrestore procedure
  entry
	subl	rr10, rr10
	ldl	rr8, rr10
	jr	L_0344
L_0330:
	bit	r11, #0
	jr	nz, L_0344
	addl	rr6, rr12
	adc	r5, r3
	adc	r4, r2
	jr	nc, L_0350
	bit	r8, #15
	jr	nz, L_0350
	comflg	c
	jr	L_0350
L_0344:
	subl	rr6, rr12
	sbc	r5, r3
	sbc	r4, r2
	bit	r8, #15
	jr	nz, L_0350
	comflg	c
L_0350:
	adc	r11, r11
	adc	r10, r10
	adc	r9, r9
	adc	r8, r8
	dec	r0
	ret	z
L_035c:
	addl	rr6, rr6
	adc	r5, r5
	adc	r4, r4
	adc	r8, r8
	rr	r8
	jr	L_0330
end nonrestore

remvsdiv procedure
  entry
	ldb	rl0, #%c0
	bit	r4, #15
	ret	nz
L_036e:
	addl	rr6, rr6
	adc	r5, r5
	adc	r4, r4
	cpl	rr4, rr2
	jr	nz, L_037a
	cpl	rr6, rr12
L_037a:
	jr	ugt, L_038a
	jr	z, L_0388
	resb	rl0, #7
	testl	rr4
	jr	nz, L_038a
	testl	rr6
	jr	nz, L_038a
L_0388:
	resb	rl0, #6
L_038a:
	resflg	c
	rrc	r4
	rrc	r5
	rrc	r6
	rrc	r7
	ret
end remvsdiv

cmpcase	array [* long] := [ cmpno, cmps0, cmpsinf, cmpunor,
			    cmpd0, cmpeq, cmpsinf, cmpunor,
			    cmpdinf, cmpdinf, cmp2inf, cmpunor,
			    cmpunor, cmpunor, cmpunor, cmpunor,
			    cmpunor ]

cpzcase array [* long] := [ cmps0, cmpeq, cmpdinf, cmpunor,
			    donone, donone, donone, donone,
			    donone, donone, donone, donone,
			    donone, donone, donone, donone,
			    cmpunor ]

Fcp procedure
  entry
Fcpx:
	or	r4, #%1010
	callr	setup
	ld	r0, rr12(#%0060)
	ldl	rr2, cmpcase(r1)
	jr	L_03be
Fcpz:
Fcpzx:
	or	r4, #%1000
	callr	setup
	ld	r0, r6
	ldl	rr2, cpzcase(r1)
L_03be:
	call	@rr2
	cpb	rl5, #%10
	jr	nz, L_03d0
	lda	rr2, rr12(#%007a)
	bit	@rr2, #5
	jr	z, L_03d0
	setb	rh5, #0
L_03d0:
	ldb	rr12(#%0059), rl5
	ret
end Fcp

cmpunor procedure
  entry
	ldb	rl5, #%10
	ret
end cmpunor

cmp2inf procedure
  entry
	xor	r0, r6
	jr	mi, L_03e2
	ldb	rl5, #%40
	ret
L_03e2:
	lda	rr2, rr12(#%005d)
	bitb	@rr2, #0
	jr	nz, cmpd0
	ldb	rl5, #%40
	ret
end cmp2inf

cmpdinf procedure
  entry
	lda	rr2, rr12(#%005d)
	bitb	@rr2, #0
	jr	nz, cmps0
	ldb	rl5, #%10
	ret
end cmpdinf

cmpsinf procedure
  entry
	lda	rr2, rr12(#%005d)
	bitb	@rr2, #0
	jr	nz, cmpd0
	ldb	rl5, #%10
	ret
end cmpsinf

cmpeq procedure
  entry
	ldb	rl5, #%40
	ret
end cmpeq

cmpd0 procedure
  entry
	ldb	rl5, #%08
	bit	r6, #15
	ret	nz
L_0410:
	ldb	rl5, #%a8
	ret
end cmpd0

cmps0 procedure
  entry
	ldb	rl5, #%a8
	bit	r0, #15
	ret	nz
L_041a:
	ldb	rl5, #%08
	ret
end cmps0

cmpno procedure
  entry
	xorb	rh0, rh6
	jr	mi, cmpd0
	pushl	@rr14, rr4
	lda	rr2, rr12(#%0062)
	ldm	r1, @rr2, #5
	cp	r1, r7
	ldl	rr0, rr4
	popl	rr4, @rr14
	jr	gt, L_0446
	jr	lt, L_0450
	cpl	rr2, rr8
	jr	ugt, L_0446
	jr	c, L_0450
	cpl	rr0, rr10
	jr	ugt, L_0446
	jr	c, L_0450
	ldb	rl5, #%40
	ret
L_0446:
	ldb	rl5, #%08
	bitb	rh6, #7
	ret	z
L_044c:
	ldb	rl5, #%a8
	ret
L_0450:
	ldb	rl5, #%a8
	bitb	rh6, #7
	ret	z
L_0456:
	ldb	rl5, #%08
	ret
end cmpno

sqrcase	array [* long] := [ sqrno, donone, sqrin, resnsrc,
			    donone, donone, donone, donone,
			    donone, donone, donone, donone,
			    donone, donone, donone, donone,
			    dotrap ]

Fsqr procedure
  entry
	or	r4, #%2010
	callr	setup
	ldl	rr2, sqrcase(r1)
	call	@rr2
	jpr	delivery
end Fsqr

sqrin procedure
  entry
	bitb	rh6, #7
	jpr	nz, resinv
	lda	rr2, rr12(#%005d)
	bitb	@rr2, #0
	jpr	z, resinv
	ret
end sqrin

sqrno procedure
  entry
	bitb	rh6, #7
	jpr	nz, resinv
	bit	r8, #15
	jpr	z, resinv
	pushl	@rr14, rr4
	pushl	@rr14, rr12
	sub	r0, r0
	rrcb	rl6
	rrc	r7
	ldctlb	rh0, flags
	jr	nc, L_04ae
	add	r7, #%0001
	adcb	rl6, rl0
L_04ae:
	add	r7, #%1fff
	adcb	rl6, rl0
	pushl	@rr14, rr6
	subl	rr6, rr6
	ldctlb	flags, rh0
	jr	c, L_04c4
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	adc	r7, r7
L_04c4:
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	adc	r7, r7
	ldl	rr4, rr6
	ldl	rr6, rr8
	ldl	rr8, rr10
	ld	r10, r4
	ldk	r11, #5
	dec	r5
	ldb	rh0, #%10
L_04da:
	addl	rr8, rr8
	adc	r7, r7
	adc	r6, r6
	adc	r5, r5
	adc	r4, r4
	addl	rr8, rr8
	adc	r7, r7
	adc	r6, r6
	adc	r5, r5
	adc	r4, r4
	ldl	rr2, rr4
	subl	rr4, rr10
	jr	pl, L_04f8
	ldl	rr4, rr2
	jr	L_04fa
L_04f8:
	set	r11, #1
L_04fa:
	addl	rr10, rr10
	dec	r11
	decb	rh0
	jr	nz, L_04da
	sub	r13, r13
	ldb	rh0, #%10
L_0506:
	addl	rr6, rr6
	adc	r5, r5
	adc	r4, r4
	adc	r13, r13
	addl	rr6, rr6
	adc	r5, r5
	adc	r4, r4
	adc	r13, r13
	ld	r12, r13
	ldl	rr2, rr4
	subl	rr4, rr10
	sbc	r13, r9
	ldctlb	rl0, flags
	addl	rr10, rr10
	adc	r9, r9
	inc	r11, #3
	ldctlb	flags, rl0
	jr	pl, L_0530
	ld	r13, r12
	ldl	rr4, rr2
	res	r11, #2
L_0530:
	decb	rh0
	jr	nz, L_0506
	com	r8
	com	r9
	com	r10
	com	r11
	setflg	c
	rrc	r9
	rrc	r10
	rrc	r11
	setflg	c
	rrc	r9
	rrc	r10
	rrc	r11
	ldl	rr6, rr4
	ld	r5, r13
	sub	r4, r4
	ldb	rh0, #%1f
L_0554:
	ldl	rr12, rr4
	ldl	rr2, rr6
	addl	rr6, rr10
	adc	r5, r9
	adc	r4, r8
	jr	c, L_0564
	ldl	rr4, rr12
	ldl	rr6, rr2
L_0564:
	ldctlb	rl0, flags
	comflg	c
	adc	r11, r11
	adc	r10, r10
	adc	r9, r9
	adc	r8, r8
	ldctlb	flags, rl0
	adc	r7, r7
	adc	r6, r6
	adc	r5, r5
	adc	r4, r4
	ldctlb	flags, rl0
	adc	r7, r7
	adc	r6, r6
	adc	r5, r5
	adc	r4, r4
	decb	rh0
	jr	nz, L_0554
	ldctlb	rl0, flags
	tccb	c, rl0
	jr	c, L_05a0
	ldl	rr12, rr4
	ldl	rr2, rr6
	addl	rr6, rr10
	adc	r5, r9
	adc	r4, r8
	jr	c, L_059e
	ldl	rr4, rr12
	ldl	rr6, rr2
L_059e:
	ldctlb	rl0, flags
L_05a0:
	com	r8
	com	r9
	com	r10
	com	r11
	and	r0, #%0081
	or	r4, r5
	or	r4, r6
	or	r4, r7
	tccb	nz, rl0
	popl	rr6, @rr14
	popl	rr12, @rr14
	popl	rr4, @rr14
	ldb	rl5, rl0
	jpr	round
end sqrno

actrem	array [* long] := [ remno, reminv, remnod, resnsrc,
			    remnz, reminv, remnz, resnsrc,
			    reminv, reminv, reminv, resnsrc,
			    resndst, resndst, resndst, resprec,
			    dotrap ]

Fremstep procedure
  entry
	or	r4, #%3010
	callr	setup
	ld	r0, rr12(#%0060)
	xorb	rh0, rh6
	andb	rh0, #%80
	rrb	rh0
	setb	rh0, #7
	push	@rr14, r0
	ldl	rr2, actrem(r1)
	call	@rr2
	pop	r0, @rr14
	ldb	rl5, rh0
	ldb	rr12(#%0059), rl5
	jpr	delivery
end Fremstep

reminv procedure
  entry
	jpr	resinv
end reminv

remnz procedure
  entry
	callr	reszero
	ld	r0, rr12(#%0060)
	andb	rh0, #%80
	orb	rh6, rh0
	ret
end remnz

remnod procedure
  entry
	callr	resdst
	jr	L_06bc
end remnod

remno procedure
  entry
	bit	r8, #15
	jr	z, reminv
	pushl	@rr14, rr12
	pushl	@rr14, rr4
	add	r13, #%0060
	ldm	r0, @rr12, #6
	push	@rr14, r0
	bit	r2, #15
	jr	nz, L_0638
L_062e:
	dec	r1
	addl	rr4, rr4
	adc	r3, r3
	adc	r2, r2
	jr	pl, L_062e
L_0638:
	exts	rr0
	pushl	@rr14, rr0
	dec	r7, #2
	exts	rr6
	subl	rr0, rr6
	jr	gt, L_064e
	ldl	rr6, rr4
	ldl	rr4, rr2
	sub	r0, r0
	ld	r11, r0
	jr	L_069c
L_064e:
	cp	r1, #%007f
	jr	ule, L_0676
	lda	rr6, rr14(#%0012)
	resb	@rr6, #7
	subl	rr0, rr0
	set	r1, #6
	ldl	rr6, @rr14
	subl	rr6, rr0
	ldl	@rr14, rr6
	calr	L_06d6
	ldl	rr10, rr6
	ldl	rr8, rr4
	popl	rr6, @rr14
	pop	r2, @rr14
	ldb	rh6, rh2
	popl	rr4, @rr14
	popl	rr12, @rr14
	ret
L_0676:
	inc	r7
	exts	rr6
	ldl	@rr14, rr6
	calr	L_06d6
	bit	r11, #0
	jr	z, L_069c
	testl	rr4
	jr	nz, L_068e
	testl	rr6
	jr	nz, L_068e
	bit	r11, #1
	jr	z, L_0692
L_068e:
	inc	r11, #2
	set	r0, #15
L_0692:
	subl	rr12, rr6
	sbc	r3, r5
	sbc	r2, r4
	ldl	rr6, rr12
	ldl	rr4, rr2
L_069c:
	ldk	r1, #14
	and	r1, r11
	ldl	rr10, rr6
	ldl	rr8, rr4
	popl	rr6, @rr14
	pop	r2, @rr14
	popl	rr4, @rr14
	popl	rr12, @rr14
	ldb	rh1, rr14(#%0004)
	rlb	rl1, #2
	orb	rh1, rl1
	ldb	rr14(#%0004), rh1
	xorb	rh0, rh2
	ldb	rh6, rh0
L_06bc:
	callr	normordigs
	callr	under
	callr	round
	jpr	over
L_06d6:
	ld	r0, r1
	ldl	rr6, rr4
	ldl	rr4, rr2
	ldl	rr2, rr8
	ldl	rr12, rr10
	callr	nonrestore
	bit	r11, #0
	ret	nz
L_06ea:
	addl	rr6, rr12
	adc	r5, r3
	adc	r4, r2
	ret
end remno

setup procedure
  entry
	sub	r1, r1
	bitb	rh4, #4
	jr	z, L_0706
	ldl	rr2, rr12(#%007c)
	calr	L_072c
	lda	rr2, rr12(#%0060)
	ldm	@rr2, r6, #6
L_0706:
	bitb	rl4, #4
	jr	z, L_071e
	rl	r1, #2
	exb	rh4, rl4
	ldl	rr2, rr12(#%0080)
	calr	L_072c
	lda	rr2, rr12(#%006c)
	ldm	@rr2, r6, #6
	exb	rh4, rl4
L_071e:
	andb	rh1, rh1
	jr	z, L_0728
	setb	rh5, #6
	ld	r1, #%0010
L_0728:
	rl	r1, #2
	ret
L_072c:
	bitb	rh4, #2
	jr	z, L_074e
	ldm	r7, @rr2, #5
	ld	r6, #%8000
	andb	rh6, rh7
	res	r7, #15
	cp	r7, #%7fff
	jr	z, L_07c0
	bit	r8, #15
	ret	nz
L_0746:
	test	r7
	jr	z, L_07dc
	setb	rh5, #5
	jr	L_07dc
L_074e:
	bitb	rh4, #1
	jr	nz, L_0784
	ldl	rr6, @rr2
	subl	rr2, rr2
	exb	rl3, rh6
	exb	rh6, rl6
	exb	rl6, rh7
	exb	rh7, rl7
	ldl	rr8, rr6
	subl	rr10, rr10
	rl	r8
	rlcb	rl3
	rrcb	rh2
	rrc	r8
	ldl	rr6, rr2
	or	r7, r7
	jr	nz, L_0776
	ld	r7, #%3f81
	jr	L_07dc
L_0776:
	cp	r7, #%00ff
	jr	z, L_07c0
	add	r7, #%3f80
	set	r8, #15
	ret
L_0784:
	ldm	r7, @rr2, #4
	sub	r6, r6
	ld	r11, r6
	setb	rh6, #7
	andb	rh6, rh7
	res	r7, #15
	ldk	r3, #5
L_0794:
	rrc	r7
	rrc	r8
	rrc	r9
	rrc	r10
	rrc	r11
	dec	r3
	jr	nz, L_0794
	rlc	r8
	rlc	r7
	rrc	r8
	or	r7, r7
	jr	nz, L_07b2
	ld	r7, #%3c01
	jr	L_07dc
L_07b2:
	cp	r7, #%07ff
	jr	z, L_07c0
	add	r7, #%3c00
	set	r8, #15
	ret
L_07c0:
	ld	r7, #%7fff
	testl	rr8
	jr	nz, L_07d0
	testl	rr10
	jr	nz, L_07d0
	set	r1, #1
	ret
L_07d0:
	or	r1, #%0003
	bit	r8, #14
	ret	nz
L_07d8:
	set	r1, #8
	ret
L_07dc:
	testl	rr8
	jr	nz, L_07ea
	testl	rr10
	jr	nz, L_07ea
	sub	r7, r7
	set	r1, #0
	ret
L_07ea:
	setb	rh5, #5
L_07ec:
	dec	r7
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	jr	pl, L_07ec
	bit	r7, #15
	ret	z
L_07fa:
	ldb	rl6, #%ff
	ret
end setup

Fnoop procedure
  entry
	ret
end Fnoop

Fstp procedure
  entry
	ret
end Fstp

donone procedure
  entry
ressrc:
	ret
end donone

dotrap procedure
  entry
	ldb	rh0, rr12(#%005e)
	bitb	rh0, #6
	ret	nz
L_080c:
	jr	L_082e
resdst:
	lda	rr6, rr12(#%0060)
	ldm	r6, @rr6, #6
	ret
reszero:
	subl	rr10, rr10
	ldl	rr8, rr10
	ldl	rr6, rr10
	ret
resinf:
	subl	rr10, rr10
	ldl	rr8, rr10
	ldl	rr6, #%00007fff
	ret
resinv:
	setb	rh5, #0
L_082e:
	subl	rr10, rr10
	ldl	rr8, #%40000000
	ldl	rr6, #%00007fff
	ret
resprec:
	jpr	resnsrc
resndst:
	lda	rr6, rr12(#%0060)
	ldm	r6, @rr6, #6
resnsrc:
	bitb	rh4, #0
	ret	z
L_0850:
	bitb	rh4, #1
	jr	nz, L_0860
	testl	rr10
	jr	nz, L_0868
	ld	r0, #%00ff
	and	r0, r9
	ret	z
L_0860:
	ld	r0, #%07ff
	and	r0, r11
	ret	z
L_0868:
	setb	rh5, #0
	ret
end dotrap

shiftdigs procedure
  entry
	and	r0, r0
	jr	mi, L_0886
	ret	z
L_0872:
	rrc	r8
	rrc	r9
	rrc	r10
	rrc	r11
	rrcb	rl5
	tccb	c, rl5
	resflg	c
	dec	r0
	jr	nz, L_0872
	ret
L_0886:
	resflg	c
	addb	rl5, rl5
	adc	r11, r11
	adc	r10, r10
	adc	r9, r9
	adc	r8, r8
	inc	r0
	jr	nz, L_0886
	ret
end shiftdigs

normordigs procedure
  entry
	ld	r0, r11
	orb	rl0, rl5
	or	r0, r10
	or	r0, r9
	or	r0, r8
normdigs:
	and	r0, r0
	jr	nz, L_08ac
	ld	r7, r0
	ldb	rl6, rl0
	ret
L_08ac:
	ldk	r0, #1
	subb	rh0, rh0
	and	r8, r8
L_08b2:
	ret	mi
L_08b4:
	sub	r7, r0
	sbcb	rl6, rh0
	subb	rh0, rh0
	addb	rl5, rl5
	adc	r11, r11
	adc	r10, r10
	adc	r9, r9
	adc	r8, r8
	jr	L_08b2
end normordigs

under procedure
  entry
	bitb	rh4, #2
	jr	nz, L_08de
	bitb	rh4, #1
	jr	z, L_08d6
	ldl	rr0, #%3c010600
	jr	L_08e8
L_08d6:
	ldl	rr0, #%3f8100c0
	jr	L_08e8
L_08de:
	ldl	rr0, #%00006000
	bitb	rl6, #0
	jr	nz, itsunder
L_08e8:
	cp	r0, r7
	ret	ule
itsunder:
	setb	rh5, #2
	subb	rl6, rl6
	lda	rr2, rr12(#%005e)
	bitb	@rr2, #2
	jr	nz, L_0908
	ld	r2, r0
	sub	r0, r7
	resflg	c
	callr	shiftdigs
	ld	r7, r2
	jr	L_090a
L_0908:
	add	r7, r1
L_090a:
	ret
end under

round procedure
  entry
	ldb	rh0, rr12(#%005d)
	ldb	rl0, #%0f
	andb	rl0, rh4
foolsround:
	subl	rr2, rr2
	bitb	rl0, #1
	jr	nz, L_0934
	bitb	rl0, #0
	jr	nz, L_0956
	ld	r1, #%7fc0
	andb	rh1, rl5
	jr	z, L_0928
	setb	rl5, #6
L_0928:
	andb	rl5, rl1
	rr	r11
	rl	r11
	rrcb	rl5
	set	r3, #0
	jr	L_097a
L_0934:
	ld	r1, #%03ff
	and	r1, r11
	orb	rl1, rl5
	subb	rl5, rl5
	or	r1, r1
	jr	z, L_0944
	setb	rl5, #5
L_0944:
	ld	r1, #%0c00
	and	r1, r11
	rldb	rl1, rh1
	orb	rl5, rh1
	and	r11, #%f800
	set	r3, #11
	jr	L_097a
L_0956:
	ld	r1, #%007f
	and	r1, r9
	or	r1, r10
	orb	rl1, rl5
	subb	rl5, rl5
	or	r1, r11
	jr	z, L_0968
	setb	rl5, #5
L_0968:
	ld	r1, #%0180
	and	r1, r9
	rr	r1
	orb	rl5, rl1
	subl	rr10, rr10
	and	r9, #%ff00
	set	r2, #8
L_097a:
	ldb	rl1, #%60
	andb	rl1, rl5
	jr	z, L_09ac
	setb	rh5, #4
	bitb	rh0, #2
	jr	nz, L_098c
	bitb	rh0, #1
	jr	z, L_09a0
	jr	L_09ac
L_098c:
	bitb	rh0, #1
	jr	z, L_0998
	bitb	rh6, #7
	jr	z, L_09ac
	setb	rl5, #4
	jr	L_09ac
L_0998:
	bitb	rh6, #7
	jr	nz, L_09ac
	setb	rl5, #4
	jr	L_09ac
L_09a0:
	bitb	rl5, #6
	jr	z, L_09ac
	cpb	rl5, #%40
	jr	z, L_09ac
	setb	rl5, #4
L_09ac:
	bitb	rl5, #4
	ret	z
L_09b0:
	sub	r1, r1
	add	r11, r3
	adc	r10, r1
	adc	r9, r2
	adc	r8, r1
	ret	nc
L_09bc:
	set	r8, #15
	add	r7, #%0001
	adcb	rl6, rl1
	ret
over:
	bitb	rh4, #2
	jr	nz, L_09de
	bitb	rh4, #1
	jr	z, L_09d6
	ldl	rr0, #%43fe0600
	jr	L_09e4
L_09d6:
	ldl	rr0, #%407e00c0
	jr	L_09e4
L_09de:
	ldl	rr0, #%7ffe6000
L_09e4:
	cp	r0, r7
	ret	nc
L_09e8:
	setb	rh5, #1
	lda	rr2, rr12(#%005e)
	bitb	@rr2, #1
	jr	z, L_09f6
	sub	r7, r1
	ret
L_09f6:
	setb	rh5, #4
	andb	rh6, #%80
	ldb	rh1, #%06
	lda	rr2, rr12(#%005d)
	andb	rh1, @rr2
	cpb	rh1, #%00
	jr	z, L_0a3c
	orb	rh1, rh6
	cpb	rh1, #%04
	jr	z, L_0a3c
	cpb	rh1, #%86
	jr	z, L_0a3c
	bit	r8, #15
	jr	z, L_0a38
	ldl	rr10, #%ffffffff
	ldl	rr8, rr10
	bitb	rh4, #0
	jr	z, L_0a38
	bitb	rh4, #1
	jr	nz, L_0a34
	subl	rr10, rr10
	ld	r9, #%ff00
	jr	L_0a38
L_0a34:
	ld	r11, #%f800
L_0a38:
	ld	r7, r0
	ret
L_0a3c:
	subl	rr10, rr10
	ldl	rr8, rr10
	ld	r7, #%7fff
	ret
end round

delivery procedure
  entry
	bitb	rh4, #2
	jr	nz, L_0a50
	bitb	rh4, #1
	jr	nz, L_0aaa
	jr	L_0a70
L_0a50:
	or	r7, r7
	jr	z, L_0a60
	cp	r7, #%7fff
	jr	z, L_0a60
	callr	normordigs
L_0a60:
	rlc	r7
	rlc	r6
	rrc	r7
	ldl	rr2, rr12(#%007c)
	ldm	@rr2, r7, #5
	ret
L_0a70:
	or	r7, r7
	jr	nz, L_0a7a
	ld	r7, #%3f81
	jr	L_0a86
L_0a7a:
	cp	r7, #%7fff
	jr	nz, L_0a86
	ld	r7, #%407f
	set	r8, #15
L_0a86:
	sub	r7, #%3f80
	bit	r8, #15
	jr	nz, L_0a90
	dec	r7
L_0a90:
	rlc	r8
	rlc	r6
	rrcb	rl7
	rrc	r8
	ld	r6, r8
	or	r7, r9
	exb	rh7, rl7
	exb	rl6, rh7
	exb	rh6, rl6
	ldl	rr2, rr12(#%007c)
	ldl	@rr2, rr6
	ret
L_0aaa:
	or	r7, r7
	jr	nz, L_0ab4
	ld	r7, #%3c01
	jr	L_0ac0
L_0ab4:
	cp	r7, #%7fff
	jr	nz, L_0ac0
	ld	r7, #%43ff
	set	r8, #15
L_0ac0:
	sub	r7, #%3c00
	bit	r8, #15
	jr	nz, L_0aca
	dec	r7
L_0aca:
	rlc	r8
	rrc	r7
	rrc	r8
	ldk	r0, #5
L_0ad2:
	rlc	r11
	rlc	r10
	rlc	r9
	rlc	r8
	rlc	r7
	dec	r0
	jr	nz, L_0ad2
	rlc	r7
	rlc	r6
	rrc	r7
	ldl	rr2, rr12(#%007c)
	ldm	@rr2, r7, #4
	ret
end delivery

toptab array [* long] := [ Fclr, Fcpz, Fcpzx, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fnoop,
			   Fnoop, Fcp, Fcpx, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fnoop,
			   Fadd, Fsub, Fdiv, Fmul,
			   Fremstep, Fnoop, Fnoop, Fnoop,
			   Fld, Fldbcd, Fldil, Fldm,
			   Ftil, Fnoop, Fnoop, Fnoop,
			   Fabs, Fneg, Fnoop, Fnoop,
			   Fsqr, Fint, Fnoop, Fnoop,
			   Fldctl, Fldctlb, Fsetflg, Fresflg,
			   Fsetmode, Fnoop, Fnoop, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fnoop,
			   Fnoop, Fnoop, Fnoop, Fstp ]
global

_fpepu procedure			!Aufruf von trap.c!
  entry
fpepu:
!	dec	sp, #12!
!	ldm	@rr14, r8, #6!
	ldl	rr8, rr6		!state (Stackpointer)!
	ldl	rr12, rr4		!&u.u_fpstate!
	ldl	rr12(#%0084), rr8
	ld	r10, rr8(#staevnt)
	calr	L_0c1c
	ld	r11, r3
	ld	r2, #%3000
	and	r2, r11
	rrdb	rl2, rh2
	bit	r10, #8
	jr	nz, L_0b1e
	calr	L_0bcc
	calr	L_0bc2
	jr	L_0b46
L_0b1e:
	bit	r10, #2
	jr	nz, L_0b36
	bit	r10, #3
	jr	nz, L_0b2e
	calr	L_0bd0
	calr	L_0bf6
	calr	L_0bc4
	jr	L_0b46
L_0b2e:
	calr	L_0bc2
	calr	L_0bf6
	calr	L_0bd2
	jr	L_0b46
L_0b36:
	bit	r10, #3
	jr	nz, L_0b40
	calr	L_0bbe
	calr	L_0c34
	jr	L_0b46
L_0b40:
	calr	L_0bcc
	calr	L_0ce0
	jr	L_0b46
L_0b46:
	ldl	rr12(#%0078), rr10
	ld	r8, r2
	ld	r1, #%c0f0
	and	r1, r11
	rlb	rh1, #2
	rr	r1, #2
	ldl	rr10, toptab(r1)
!	callr	freecpu!
	ld	r4, r8
	sub	r5, r5
	call	@rr10
	ldl	rr8, rr4
!	callr	needcpu!
!	ldl	rr4, rr8!
	ld	r0, rr12(#%0078)
	and	r0, #%010c
	jr	nz, L_0b8a
	ldl	rr2, rr12(#%0084)
	add	r3, #stafcw+1
	ldb	@rr2, rl5
L_0b8a:
	ldb	rl0, rr12(#%005b)
	orb	rl0, rh5
	ldb	rr12(#%005b), rl0
	ldb	rr12(#%005a), rh5
	ldb	rh0, rr12(#%005e)
	andb	rh0, rh5
	ldb	rr12(#%0088), rh0
	bitb	rh4, #6
	jr	z, L_0baa
	calr	L_0cfa
	jr	L_0bae
L_0baa:
	bitb	rl4, #6
	jr	z, L_0bb2
L_0bae:
	add	sp, #%0014
L_0bb2:
	ld	r2, rr12(#%0088)
!	ldm	r8, @rr14, #6!
!	inc	sp, #12!
	ret
L_0bbe:
	calr	L_0bda
	jr	L_0bc4
L_0bc2:
	calr	L_0be0
L_0bc4:
	orb	rh2, rl3
	ldl	rr12(#%007c), rr6
	ret
L_0bcc:
	calr	L_0bda
	jr	L_0bd2
L_0bd0:
	calr	L_0be0
L_0bd2:
	orb	rl2, rl3
	ldl	rr12(#%0080), rr6
	ret
L_0bda:
	ld	r7, r11
	rrdb	rh7, rl7
	jr	L_0be2
L_0be0:
	ld	r7, r10
L_0be2:
	and	r7, #%00f0
	rr	r7
	ld	r6, r7
	rr	r7, #2
	add	r7, r6
	add	r7, r13
	ld	r6, r12
	ldb	rl3, #%84
	ret
L_0bf6:
	lda	rr6, rr8(#stareg)
	ld	r3, #%0f0f
	and	r3, r11
	ldb	rl0, rh3
	subb	rh0, rh0
	add	r0, r0
	add	r7, r0
	ret
L_0c1c:
	lda	rr4, rr8(#stapcsg)
	ldl	rr6, @rr4
	inc	r5, #2
	inc	@rr4, #2
	callr	gettext
L_0c10:
	orb	rl2, rl2
	ret	z
	ldb	rr12(#%0089), rl2
	ret
L_0c34:
	calr	L_0c5e
	ldl	rr12(#%008c), rr6
	orb	rl2, rl3
	setb	rl2, #6
	inc	r3
!	add	r3, r3		getmem arbeitet jetzt wordweise!
	popl	rr0, @rr14
	sub	sp, #%0014
	ldl	rr12(#%0080), rr14
	ldl	rr4, rr14
	pushl	@rr14, rr0
	push	@rr14, r2
	callr	getmem
	calr	L_0c10
	pop	r2, @rr14
	ret
L_0c5e:
	bit	r10, #14
	jr	z, L_0caa
	pushl	@rr14, rr2
	calr	L_0c1c
	ld	r7, r3
	popl	rr2, @rr14
	ld	r0, rr8(#stafcw)
	bit	r0, #15
	jr	z, L_0c90
	bit	r7, #15
	jr	nz, L_0c84
	ldb	rh6, rh7
	subb	rl6, rl6
	ldb	rh7, rl6
	jr	L_0c90
L_0c84:
	push	@rr14, r2
	push	@rr14, r7
	calr	L_0c1c
	ld	r7, r3
	pop	r6, @rr14
	pop	r2, @rr14
L_0c90:
	ld	r1, #%00f0
	and	r1, r10
	jr	z, L_0cda
	rr	r1, #2
	rr	r1
	add	r1, #stareg
	ld	r0, rr8(r1)
	add	r7, r0
	jr	L_0cda
L_0caa:
	ld	r0, rr8(#stafcw)
	ld	r1, #%00f0
	and	r1, r10
	rr	r1, #2
	rr	r1
	bit	r0, #15
	jr	z, L_0cd0
	res	r1, #1
	add	r1, #stareg
	ldl	rr6, rr8(r1)
	jr	L_0cda
L_0cd0:
	add	r1, #stareg
	ld	r7, rr8(r1)
L_0cda:
	ldk	r3, #15
	and	r3, r11
	ret
L_0ce0:
	calr	L_0c5e
	ldl	rr12(#%008c), rr6
	popl	rr4, @rr14
	sub	sp, #%0014
	ldl	rr12(#%007c), rr14
	ldb	rl1, #%0f
	and	r1, r11
	setb	rl1, #6
	orb	rh2, rl1
	jp	@rr4
L_0cfa:
	ldl	rr8, rr12(#%0084)
	ldl	rr10, rr12(#%0078)
	ldl	rr4, rr12(#%008c)
	ldl	rr6, rr12(#%007c)
	ld	r3, #%000f
	and	r3, r11
	inc	r3
!	add	r3, r3		putmem arbeitet jetzt wordweise!
	callr	putmem
	jpr	L_0c10
end _fpepu

actld array [* long] := [ ldno, donone, donone, ldnan,
			  donone, donone, donone, donone,
			  donone, donone, donone, donone,
			  donone, donone, donone, donone,
			  dotrap ]
internal

Fld procedure
  entry
	or	r4, #%2010
	callr	setup
L_0d28:
	ldl	rr2, actld(r1)
	call	@rr2
L_0d30:
	jpr	delivery
Fneg:
	or	r4, #%2010
	callr	setup
	xorb	rh6, #%80
	jr	L_0d28
Fabs:
	or	r4, #%2010
	callr	setup
	resb	rh6, #7
	jr	L_0d28
Fclr:
	or	r4, #%2000
	callr	setup
	callr	reszero
	jr	L_0d30
end Fld

ldnan procedure
  entry
	jpr	resnsrc
end ldnan

ldno procedure
  entry
	callr	under
	callr	round
	jpr	over
end ldno

Fldm procedure
  entry
	ld	r1, rr12(#%007a)
	and	r1, #%000f
	inc	r1
	ldl	rr2, rr12(#%007c)
	ldl	rr6, rr12(#%0080)
	ldir	@rr2, @rr6, r1
	sub	r5, r5
	ret
end Fldm

actint array [* long] := [ intno, donone, donone, resnsrc,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   dotrap ]

Fint procedure
  entry
	or	r4, #%2010
	callr	setup
	ldl	rr2, actint(r1)
	call	@rr2
	jpr	delivery
end Fint

intu0 procedure
  entry
	and	r6, #%8000
	sub	r7, r7
	ret
end intu0

intno procedure
  entry
	callr	normordigs
	ld	r1, #%403e
	bitb	rh4, #0
	jr	z, L_0ddc
	bitb	rh4, #1
	jr	z, L_0dd8
	ld	r1, #%4033
	jr	L_0ddc
L_0dd8:
	ld	r1, #%4016
L_0ddc:
	ld	r0, r1
	sub	r0, r7
	jr	le, L_0dec
	callr	shiftdigs
	ld	r7, r1
	subb	rl6, rl6
L_0dec:
	callr	round
	ldb	rl1, rl5
	subb	rl5, rl5
	callr	normordigs
	ldb	rl5, rl1
	jpr	over
end intno

actldd array [* long] := [ lddno, ldd0, lddjnk, lddjnk,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone ]

Fldbcd procedure
  entry
	bitb	rh4, #7
	jr	z, L_0e9a
	or	r4, #%2000
	calr	setup
	subl	rr8, rr8
	ldl	rr10, rr8
	ldl	rr2, rr12(#%0080)
	pop	r1, @rr2
	push	@rr14, r1
	and	r1, #%0fff
	pushl	@rr14, rr12
	pushl	@rr14, rr4
	sub	r0, r0
	calr	L_0e58
	pop	r1, @rr2
	calr	L_0e58
	pop	r1, @rr2
	calr	L_0e58
	pop	r1, @rr2
	calr	L_0e58
	pop	r1, @rr2
	calr	L_0e58
	popl	rr4, @rr14
	popl	rr12, @rr14
	pop	r6, @rr14
	and	r6, #%8000
	ld	r7, #%403e
	bitb	rh5, #7
	jr	z, L_0e50
	calr	resinv
	resb	rh5, #0
	jpr	delivery
L_0e50:
	calr	normordigs
	calr	round
	jpr	delivery
L_0e58:
	ldk	r12, #4
L_0e5a:
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	ldl	rr4, rr8
	ldl	rr6, rr10
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	addl	rr10, rr10
	adc	r9, r9
	adc	r8, r8
	addl	rr10, rr6
	adc	r9, r5
	adc	r8, r4
	rldb	rl0, rh1
	rldb	rh1, rl1
	cp	r0, #%0009
	jr	ugt, L_0e8e
	sub	r4, r4
	add	r11, r0
	adc	r10, r4
	adc	r9, r4
	adc	r8, r4
	djnz	r12, L_0e5a
	ret
L_0e8e:
	ld	r5, rr14(#%0006)
	setb	rh5, #7
	ld	rr14(#%0006), r5
	ret
L_0e9a:
	or	r4, #%0010
	calr	setup
	ldl	rr2, actldd(r1)
	jp	@rr2
end Fldbcd

ldd0 procedure
  entry
	sub	r7, r7
	ldb	rl6, rl7
	jr	L_0f54
end ldd0

lddno procedure
  entry
	calr	intadjust
	bitb	rh5, #7
	jr	nz, lddjnk
	ldl	rr0, #%8ac72304
	cpl	rr8, rr0
	jr	ugt, lddjnk
	jr	c, L_0ecc
	ldl	rr0, #%89e80000
	cpl	rr10, rr0
	jr	nc, lddjnk
L_0ecc:
	pushl	@rr14, rr12
	pushl	@rr14, rr4
	push	@rr14, r6
	ldl	rr4, rr8
	ldl	rr6, rr10
	ldl	rr2, #%de0b6b3a
	ldl	rr12, #%76400000
	ld	r0, #%0041
	calr	nonrestore
	subl	rr0, rr0
	add	r11, #%0001
	adc	r10, r0
	adc	r9, r0
	adc	r8, r0
	ldl	rr4, rr8
	ldl	rr2, rr0
	pop	r7, @rr14
	and	r7, #%8000
	ld	r6, #%0013
L_0f02:
	rldb	rh7, rl7
	rldb	rl7, rh0
	rldb	rh0, rl0
	rldb	rl0, rh1
	rldb	rh1, rl1
	rldb	rl1, rh2
	rldb	rh2, rl2
	rldb	rl2, rh3
	rldb	rh3, rl3
	rldb	rl3, rh4
	rrdb	rh6, rh4
	decb	rl6
	jr	z, L_0f3a
	addl	rr10, rr10
	adc	r5, r5
	adc	r4, r4
	ldl	rr12, rr10
	ldl	rr8, rr4
	addl	rr10, rr10
	adc	r5, r5
	adc	r4, r4
	addl	rr10, rr10
	adc	r5, r5
	adc	r4, r4
	addl	rr10, rr12
	adc	r5, r9
	adc	r4, r8
	jr	L_0f02
L_0f3a:
	ldl	rr10, rr2
	ldl	rr8, rr0
	popl	rr4, @rr14
	popl	rr12, @rr14
	jr	L_0f54
end lddno

lddjnk procedure
  entry
	setb	rh5, #7
	ldl	rr10, #%ffffffff
	ldl	rr8, rr10
	ld	r7, r8
	or	r6, #%0fff
L_0f54:
	ldl	rr2, rr12(#%007c)
	ldm	@rr2, r7, #5
	ret
end lddjnk

actldi array [* long] := [ ldino, ldi0, ldijnk, ldijnk,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone, donone, donone, donone,
			   donone ]

Fldil procedure
  entry
	bitb	rh4, #2
	jr	z, L_0fb2
	or	r4, #%2000
	callr	setup
	ldl	rr6, rr12(#%0080)
	bitb	rl4, #1
	jr	nz, L_0f7a
	ldl	rr10, @rr6
	extsl	rq8
	jr	L_0f7e
L_0f7a:
	ldm	r8, @rr6, #4
L_0f7e:
	lda	rr2, rr12(#%006c)
	ldm	@rr2, r8, #4
	ldl	rr6, #%8000403e
	and	r6, r8
	jr	z, L_0f9e
	subl	rr0, rr0
	ldl	rr2, rr0
	subl	rr2, rr10
	sbc	r1, r9
	sbc	r0, r8
	ldl	rr10, rr2
	ldl	rr8, rr0
L_0f9e:
	callr	normordigs
	callr	round
	jpr	delivery
L_0fb2:
	or	r4, #%0010
	callr	setup
	ldl	rr2, actldi(r1)
	call	@rr2
	ldl	rr2, rr12(#%007c)
	bitb	rh4, #1
	jr	nz, L_0fd0
	ldl	@rr2, rr10
	ret
L_0fd0:
	ldm	@rr2, r8, #4
	ret
end Fldil

ldijnk procedure
  entry
	setb	rh5, #7
ldi0:
	subl	rr8, rr8
	ldl	rr10, rr8
	ret
end ldijnk

ldino procedure
  entry
	callr	intadjust
	subl	rr0, rr0
	set	r0, #15
	bitb	rh4, #1
	jr	nz, L_0ffa
	testl	rr8
	jr	nz, L_1006
	bit	r10, #15
	jr	z, L_1008
	cpl	rr10, rr0
	jr	nz, L_1006
	jr	L_1008
L_0ffa:
	bit	r8, #15
	jr	z, L_1008
	cpl	rr8, rr0
	jr	nz, L_1006
	testl	rr10
	jr	z, L_1008
L_1006:
	setb	rh5, #7
L_1008:
	bitb	rh6, #7
	ret	z
L_100c:
	res	r0, #15
	ldl	rr2, rr0
	subl	rr2, rr10
	sbc	r1, r9
	sbc	r0, r8
	ldl	rr10, rr2
	ldl	rr8, rr0
	ret
end ldino

intadjust procedure
  entry
	callr	normordigs
	sub	r7, #%403e
	jr	le, L_102a
	setb	rh5, #7
L_102a:
	ld	r0, r7
	neg	r0
	resflg	c
	callr	shiftdigs
	ldb	rh0, #%06
	ldb	rl0, rr12(#%005d)
	andb	rh0, rl0
	ldb	rl0, #%04
	jpr	foolsround
end intadjust

Ftil procedure
  entry
	ldb	rh0, rr12(#%005d)
	push	@rr14, r0
	andb	rh0, #%f9
	orb	rh0, #%02
	ldb	rr12(#%005d), rh0
	callr	Fldil
	pop	r0, @rr14
	ldb	rr12(#%005d), rh0
	ret
end Ftil

Fldctlb procedure
  entry
	sub	r5, r5
	ldb	rh1, rr12(#%0059)
	ldb	rl1, rr12(#%005b)
	ld	r0, rr12(#%0078)
	and	r0, #%00f0
	jr	nz, L_1094
	bitb	rl1, #3
	jr	z, L_1084
	setb	rl5, #7
	setb	rl5, #5
L_1084:
	bitb	rl1, #1
	jr	z, L_108c
	setb	rl5, #7
	setb	rl5, #4
L_108c:
	bitb	rl1, #2
	jr	z, L_1092
	setb	rl5, #6
L_1092:
	jr	L_10e2
L_1094:
	bit	r0, #4
	jr	nz, L_10be
	bit	r0, #6
	jr	nz, L_10de
	bitb	rl1, #3
	jr	z, L_10a4
	setb	rl5, #5
	setb	rl5, #6
L_10a4:
	bitb	rl1, #7
	jr	z, L_10ac
	setb	rl5, #4
	setb	rl5, #6
L_10ac:
	bitb	rl1, #4
	jr	z, L_10b2
	setb	rl5, #7
L_10b2:
	bitb	rl1, #0
	jr	nz, L_10ba
	bitb	rl1, #6
	jr	z, L_10bc
L_10ba:
	setb	rl5, #6
L_10bc:
	jr	L_10e2
L_10be:
	bit	r0, #5
	jr	nz, L_10dc
	bitb	rl1, #0
	jr	z, L_10c8
	setb	rl5, #7
L_10c8:
	bitb	rl1, #6
	jr	z, L_10ce
	setb	rl5, #5
L_10ce:
	bitb	rl1, #3
	jr	z, L_10d4
	setb	rl5, #4
L_10d4:
	bitb	rl1, #4
	jr	z, L_10da
	setb	rl5, #6
L_10da:
	jr	L_10e2
L_10dc:
	jr	L_10e2
L_10de:
	ldb	rl5, rh1
	jr	L_10e2
L_10e2:
	subb	rh5, rh5
	ret
end Fldctlb

Fldctl procedure
  entry
	ldl	rr0, rr12(#%0078)
	bit	r0, #2
	jr	nz, L_10f2
	rldb	rh0, rl0
	ld	r1, r0
L_10f2:
	and	r1, #%0f00
	exb	rl1, rh1
	bitb	rh4, #2
	jr	nz, L_114e
	ldl	rr2, rr12(#%007c)
	or	r1, r1
	jr	nz, L_110a
	ldl	rr6, rr12(#%0050)
	jr	L_114a
L_110a:
	dec	r1
	jr	nz, L_1110
	jr	L_114a
L_1110:
	dec	r1
	jr	nz, L_1116
	jr	L_114a
L_1116:
	dec	r1
	jr	nz, L_1120
	ldl	rr6, rr12(#%0054)
	jr	L_114a
L_1120:
	dec	r1
	jr	nz, L_112a
	ldl	rr6, rr12(#%0058)
	jr	L_114a
L_112a:
	dec	r1
	jr	nz, L_1134
	ldl	rr6, rr12(#%005c)
	jr	L_114a
L_1134:
	dec	r1
	jr	nz, L_113e
	lda	rr6, rr12(#%006c)
	jr	L_1142
L_113e:
	lda	rr6, rr12(#%0060)
L_1142:
	ldk	r0, #6
	ldir	@rr2, @rr6, r0
	jr	L_11a0
L_114a:
	ldl	@rr2, rr6
	jr	L_11a0
L_114e:
	ldl	rr2, rr12(#%0080)
	ldl	rr6, @rr2
	or	r1, r1
	jr	nz, L_115e
	ldl	rr12(#%0050), rr6
	jr	L_11a0
L_115e:
	dec	r1
	jr	nz, L_1164
	jr	L_11a0
L_1164:
	dec	r1
	jr	nz, L_116a
	jr	L_11a0
L_116a:
	dec	r1
	jr	nz, L_1174
	ldl	rr12(#%0054), rr6
	jr	L_11a0
L_1174:
	dec	r1
	jr	nz, L_117e
	ldl	rr12(#%0058), rr6
	jr	L_11a0
L_117e:
	dec	r1
	jr	nz, L_1188
	ldl	rr12(#%005c), rr6
	jr	L_11a0
L_1188:
	inc	r3, #4
	ldm	r8, @rr2, #4
	dec	r1
	jr	nz, L_1198
	lda	rr2, rr12(#%006c)
	jr	L_119c
L_1198:
	lda	rr2, rr12(#%0060)
L_119c:
	ldm	@rr2, r6, #6
L_11a0:
	sub	r5, r5
	ret
end Fldctl

Fsetmode procedure
  entry
	ld	r0, rr12(#%007a)
	and	r0, #%0707
	ldb	rh1, rr12(#%005d)
	comb	rh0
	andb	rh1, rh0
	orb	rh1, rl0
	ldb	rr12(#%005d), rh1
	sub	r5, r5
	ret
end Fsetmode

Fsetflg procedure
  entry
Fresflg:
	ldl	rr0, rr12(#%0078)
	and	r0, #%00f0
	ldb	rh0, rl1
	ldb	rl1, rh1
	srlb	rh1, #4
	and	r1, #%0f0f
	rrb	rh1, #2
	rrb	rh1, #2
	orb	rl1, rh1
	orb	rl0, rl0
	jr	nz, L_11e2
	lda	rr2, rr12(#%005b)
	jr	L_11f0
L_11e2:
	bitb	rl0, #4
	jr	z, L_11ec
	lda	rr2, rr12(#%0059)
	jr	L_11f0
L_11ec:
	lda	rr2, rr12(#%005e)
L_11f0:
	ldb	rl0, @rr2
	bitb	rh0, #4
	jr	z, L_11fa
	orb	rl0, rl1
	jr	L_11fe
L_11fa:
	comb	rl1
	andb	rl0, rl1
L_11fe:
	ldb	@rr2, rl0
	sub	r5, r5
	ret
end Fsetflg

end fpe
