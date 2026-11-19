	.seg
	.extern	__pow10
	.code

_ecvt::
	.word	%8F38,%8403	// EPU
	ld	r7,r3
	ldl	rr4,rr14(#%0004)
	ldl	rr2,rr14(#%0008)
	call	Lb4cc
	ret
_fcvt::
	.word	%8F38,%8403	// EPU
	ld	r7,r3
	neg	r7
	ldl	rr4,rr14(#%0004)
	ldl	rr2,rr14(#%0008)
	call	Lb4cc
	ret
Lb4cc:
__cvt::
	dec	r15,#12
	ldm	@rr14,r8,#6
	clr	@rr2
	clr	@rr4
	.word	%8F30,%8804	// EPU
	bit	r8,#15
	jr	eq,Lb4e4
	set	@rr2,#0
	.word	%8E34,%8380	// EPU
Lb4e4:
	res	r8,#15
	cp	r8,#%7FFF
	jr	ne,Lb552
	or	r9,r10
	or	r9,r11
	or	r9,r12
	jr	ne,Lb50c
	lda	rr2,D76fc
	lda	rr4,D771c
	ldk	r1,#4
	ldirb	@rr2,@rr4,r1
	jp	Lb67a
Lb50c:
	lda	rr2,D76fc
	lda	rr4,D7720
	ldk	r1,#4
	ldirb	@rr2,@rr4,r1
	.word	%8F30,%8401	// EPU
	res	r4,#7
	ldk	r1,#6
Lb526:
	subb	rh4,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	cpb	rh4,#%09
	jr	gt,Lb53a
	orb	rh4,#%30
	jr	Lb53e
Lb53a:
	addb	rh4,#%36
Lb53e:
	ldb	@rr2,rh4
	inc	r3,#1
	djnz	r1,Lb526
	ldb	@rr2,#%29
	inc	r3,#1
	clrb	@rr2
	jp	Lb67a
Lb552:
	.word	%8E30,%0010	// EPU
	jr	ne,Lb568
Lb558:
	ld	r2,#%3000
	ld	D76fc,r2
	jp	Lb67a
Lb568:
	cp	r7,#%0013
	jp	le,Lb576
	ld	r7,#%0013
Lb576:
	pushl	@rr14,rr4
	dec	r15,#10
	.word	%0FEC,%8504	// EPU
	dec	r15,#10
	.word	%0FEC,%8404	// EPU
	.word	%8E44,%8300	// EPU
	sub	r6,r6
	cp	r7,#%0000
	jr	le,Lb5aa
	.word	%8F30,%8004	// EPU
	sub	r0,#%3FFF
	bit	r1,#15
	jr	ne,Lb59e
	sub	r1,r1
Lb59e:
	add	r1,r1
	ldl	rr2,#%4D104D42
	multl	rq0,rr0
	ld	r6,r0
Lb5aa:
	ld	r5,r7
	cp	r5,#%0000
	jr	gt,Lb5b8
	neg	r5
	ld	r4,r5
	jr	Lb5c2
Lb5b8:
	sub	r5,r6
	dec	r5,#1
	ld	r4,r5
	jr	pl,Lb5c2
	neg	r4
Lb5c2:
	pushl	@rr14,rr6
	push	@rr14,r5
	ld	r7,r4
	call	__pow10
	.word	%8E14,%8400	// EPU
	pop	r5,@rr14
	bit	r5,#15
	jr	ne,Lb5de
	.word	%8E14,%40B0	// EPU
	jr	Lb5e2
Lb5de:
	.word	%8E14,%40A0	// EPU
Lb5e2:
	.word	%8E14,%81D0	// EPU
	.word	%8E54,%8100	// EPU
	popl	rr6,@rr14
	cp	r7,#%0000
	jr	le,Lb614
	pushl	@rr14,rr6
	call	__pow10
	popl	rr6,@rr14
	.word	%8E50,%4010	// EPU
	jr	c,Lb614
	jr	eq,Lb608
	inc	r6,#1
	jr	Lb5aa
Lb608:
	inc	r6,#1
	ldl	rr0,#%41200000
	.word	%8F58,%40A1	// EPU
Lb614:
	.word	%8E04,%8500	// EPU
	.word	%0FE4,%8404	// EPU
	inc	r15,#10
	.word	%0FE4,%8504	// EPU
	inc	r15,#10
	.word	%8F00,%8014	// EPU
	andb	rh0,#%0F
	lda	rr10,D76fc
	popl	rr8,@rr14
	inc	r6,#1
	ld	@rr8,r6
	cp	r7,#%0000
	jr	gt,Lb660
	.word	%8E00,%0010	// EPU
	jr	eq,Lb558
	sub	r6,r6
Lb646:
	orb	rh0,rh0
	jr	ne,Lb650
	calr	Lb688
	inc	r6,#1
	jr	Lb646
Lb650:
	sub	r6,#%0013
	sub	r7,r6
	ld	@rr8,r7
	ld	r7,r6
	neg	r7
	sub	r6,r6
	jr	Lb666
Lb660:
	ld	r6,r7
	sub	r6,#%0013
Lb666:
	orb	rh0,#%30
Lb66a:
	inc	r6,#1
	jr	le,Lb672
	ldb	@rr10,rh0
	inc	r11,#1
Lb672:
	calr	Lb688
	cp	r6,r7
	jr	lt,Lb66a
	clrb	@rr10
Lb67a:
	lda	rr2,D76fc
	ldm	r8,@rr14,#6
	inc	r15,#12
	ret
Lb688:
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

	.psec data
	.data
D76fc:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D771c:
	.byte	%49
	.byte	%4e
	.byte	%46
	.byte	%00
D7720:
	.byte	%4e
	.byte	%41
	.byte	%4e
	.byte	%28
	.byte	%30
	.byte	%30
	.byte	%30
	.byte	%30
	.byte	%30
	.byte	%00
