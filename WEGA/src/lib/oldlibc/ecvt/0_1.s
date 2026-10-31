	.extern	__pow10
	.psec
	.code
_ecvt::
	.word	%8F38,%8403
	ld	r7,r3
	ld	r6,r2
	ld	r5,r15(#%0002)
	call	L0026
	ret
	.word	%8F38,%8403
	ld	r7,r3
	neg	r7
	ld	r6,r2
	ld	r5,r15(#%0002)
	call	L0026
	ret
L0026:
	dec	r15,#12
	ldm	@r15,r8,#6
	ld	r3,r5
	ld	r5,r6
	clr	@r3
	clr	@r5
	.word	%8F30,%8804
	bit	r8,#15
	jr	eq,L0042
	set	@r3,#0
	.word	%8E34,%8380
L0042:
	res	r8,#15
	cp	r8,#%7FFF
	jr	ne,L00a4
	or	r9,r10
	or	r9,r11
	or	r9,r12
	jr	ne,L0064
	lda	r3,D0000
	lda	r5,D0020
	ldk	r1,#4
	ldirb	@r3,@r5,r1
	jp	L01c0
L0064:
	lda	r3,D0000
	lda	r5,D0024
	ldk	r1,#4
	ldirb	@r3,@r5,r1
	.word	%8F30,%8401
	res	r4,#7
	ldk	r1,#6
L007a:
	subb	rh4,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	cpb	rh4,#%09
	jr	gt,L008e
	orb	rh4,#%30
	jr	L0092
L008e:
	addb	rh4,#%36
L0092:
	ldb	@r3,rh4
	inc	r3,#1
	djnz	r1,L007a
	ldb	@r3,#%29
	inc	r3,#1
	clrb	@r3
	jp	L01c0
L00a4:
	.word	%8E30,%0010
	jr	ne,L00b6
L00aa:
	ld	r2,#%3000
	ld	D0000,r2
	jp	L01c0
L00b6:
	cp	r7,#%0013
	jp	le,L00c2
	ld	r7,#%0013
L00c2:
	pushl	@r15,rr4
	dec	r15,#10
	.word	%0FFC,%8504
	dec	r15,#10
	.word	%0FFC,%8404
	.word	%8E44,%8300
	sub	r6,r6
	cp	r7,#%0000
	jr	le,L00f6
	.word	%8F30,%8004
	sub	r0,#%3FFF
	bit	r1,#15
	jr	ne,L00ea
	sub	r1,r1
L00ea:
	add	r1,r1
	ldl	rr2,#%4D104D42
	multl	rq0,rr0
	ld	r6,r0
L00f6:
	ld	r5,r7
	cp	r5,#%0000
	jr	gt,L0104
	neg	r5
	ld	r4,r5
	jr	L010e
L0104:
	sub	r5,r6
	dec	r5,#1
	ld	r4,r5
	jr	pl,L010e
	neg	r4
L010e:
	pushl	@r15,rr6
	push	@r15,r5
	ld	r7,r4
	call	__pow10
	.word	%8E14,%8400
	pop	r5,@r15
	bit	r5,#15
	jr	ne,L0128
	.word	%8E14,%40B0
	jr	L012c
L0128:
	.word	%8E14,%40A0
L012c:
	.word	%8E14,%81D0
	.word	%8E54,%8100
	popl	rr6,@r15
	cp	r7,#%0000
	jr	le,L015c
	pushl	@r15,rr6
	call	__pow10
	popl	rr6,@r15
	.word	%8E50,%4010
	jr	c,L015c
	jr	eq,L0150
	inc	r6,#1
	jr	L00f6
L0150:
	inc	r6,#1
	ldl	rr0,#%41200000
	.word	%8F58,%40A1
L015c:
	.word	%8E04,%8500
	.word	%0FF4,%8404
	inc	r15,#10
	.word	%0FF4,%8504
	inc	r15,#10
	.word	%8F00,%8014
	andb	rh0,#%0F
	lda	r11,D0000
	popl	rr8,@r15
	inc	r6,#1
	ld	@r9,r6
	cp	r7,#%0000
	jr	gt,L01a6
	.word	%8E00,%0010
	jr	eq,L00aa
	sub	r6,r6
L018c:
	orb	rh0,rh0
	jr	ne,L0196
	calr	L01ce
	inc	r6,#1
	jr	L018c
L0196:
	sub	r6,#%0013
	sub	r7,r6
	ld	@r9,r7
	ld	r7,r6
	neg	r7
	sub	r6,r6
	jr	L01ac
L01a6:
	ld	r6,r7
	sub	r6,#%0013
L01ac:
	orb	rh0,#%30
L01b0:
	inc	r6,#1
	jr	le,L01b8
	ldb	@r11,rh0
	inc	r11,#1
L01b8:
	calr	L01ce
	cp	r6,r7
	jr	lt,L01b0
	clrb	@r11
L01c0:
	lda	r3,D0000
	ld	r2,r3
	ldm	r8,@r15,#6
	inc	r15,#12
	ret
L01ce:
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
	.psec
	.data
D0000:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
D0020:
	.byte	%49,%4e,%46,%00
D0024:
	.byte	%4e,%41,%4e,%28,%30,%30,%30,%30
	.byte	%30,%00
