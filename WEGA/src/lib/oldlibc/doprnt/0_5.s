	.extern	__cnvt, __flsbuf, __inflt, __length
	.psec
	.code
__align::
	dec	r15,#8
	ld	%0000(r15),r5
	ld	r2,r7
	test	@r2
	jr	eq,Lc00a
	ld	r2,r7
	cp	@r2,#%0002
	jr	ne,Lc014
Lc00a:
	ld	r2,r7
	inc	@r2,#1
	ld	r2,r6
	inc	@r2,#2
	jr	Lc026
Lc014:
	ld	r2,r7
	cp	@r2,#%0004
	jr	ne,Lc026
	ld	r2,r7
	inc	@r2,#2
	ld	r2,r6
	ld	@r2,r5
	jr	Lc04a
Lc026:
	ld	r2,r6
	ld	r2,@r2
	ld	r3,@r2
	ld	%0006(r15),r3
	ld	r2,r6
	ld	r2,@r2
	ld	r3,r6
	ld	r3,@r3
	ld	r0,%0002(r2)
	ld	@r3,r0
	ld	r2,r6
	ld	r2,@r2
	ld	r3,%0006(r15)
	ld	%0002(r2),r3
Lc04a:
	inc	r15,#8
	ret
__doprnt::
	sub	r15,#%0068
	ldm	%0060(r15),r11,#4
	ldm	%0002(r15),r3,#4
	ld	r14,r7
	ld	r2,%0008(r15)
	ld	%000A(r15),r2
	call	__length
	ld	r12,r2
	jr	Lc08e
Lc070:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	ge,Lc49e
Lc084:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
Lc08e:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,Lc59a
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%0025
	jr	ne,Lc070
	test	r12
	jp	eq,Lc59a
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%000C(r15)
	ld	%000E(r15),#%FFFF
	ldb	%005D(r15),#%20
	clr	%0058(r15)
	cp	r2,#%002D
	jr	ne,Lc0e8
	inc	%0058(r15),#1
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,Lc59a
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
Lc0e8:
	cp	r13,#%0061
	jr	lt,Lc196
	jr	Lc1ae
Lc0f0:
	cp	r13,#%002A
	jr	ne,Lc11a
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000C(r15),r3
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,Lc13a
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	Lc13a
Lc11a:
	cp	r13,#%0030
	jr	ne,Lc126
	ldb	%005D(r15),#%30
Lc126:
	lda	r5,%000C(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
Lc13a:
	test	r12
	jr	eq,Lc1ae
	cpb	@r14,#%2E
	jr	ne,Lc188
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,Lc1ae
	cpb	@r14,#%2A
	jr	ne,Lc178
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000E(r15),r3
	inc	%000A(r15),#2
	ldk	r11,#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,Lc184
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	Lc184
Lc178:
	lda	r5,%000E(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
Lc184:
	sub	r12,r11
	add	r14,r11
Lc188:
	test	r12
	jr	eq,Lc1ae
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
Lc196:
	cp	r13,#%002A
	jr	eq,Lc0f0
	cp	r13,#%0030
	jr	lt,Lc1a8
	cp	r13,#%0039
	jr	le,Lc0f0
Lc1a8:
	cp	r13,#%002E
	jr	eq,Lc0f0
Lc1ae:
	cp	%000E(r15),#%0000
	jr	ge,Lc1d0
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,Lc1d0
	ld	r2,%0006(r3)
	jp	@r2
Lc1cc:
	clr	%000E(r15)
Lc1d0:
	ld	%005E(r15),r13
	ld	r2,r13
	ld	r3,#D0018
	ld	r4,#%000E
	cpir	r2,@r3,r4,eq
	jp	ne,Lc48a
	ld	r2,%001A(r3)
	jp	@r2
Lc1ec:
	ld	%000E(r15),#%0006
	jr	Lc1d0
Lc1f4:
	ld	%0010(r15),#%0008
	jr	Lc20a
Lc1fc:
	ld	%0010(r15),#%0010
	jr	Lc20a
Lc204:
	ld	%0010(r15),#%000A
Lc20a:
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	clr	%0012(r15)
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	eq,Lc35c
	jp	Lc364
Lc232:
	test	r12
	jr	eq,Lc2fe
	dec	r12,#1
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%006F
	jr	ne,Lc298
Lc248:
	ld	%0010(r15),#%0008
	jr	Lc266
Lc250:
	ld	%0010(r15),#%0010
	jr	Lc266
Lc258:
	ld	%0010(r15),#%FFF6
	jr	Lc266
Lc260:
	ld	%0010(r15),#%000A
Lc266:
	cp	%0004(r15),#%0005
	jr	ge,Lc27c
	lda	r7,%0004(r15)
	lda	r6,%000A(r15)
	ld	r5,%0006(r15)
	calr	__align
Lc27c:
	ld	r2,%000A(r15)
	ldl	rr0,@r2
	ldl	%0012(r15),rr0
	inc	%000A(r15),#4
	inc	%0004(r15),#2
	cp	%0004(r15),#%0005
	jr	eq,Lc35c
	jr	Lc364
Lc298:
	cp	r13,#%0078
	jr	eq,Lc250
	cp	r13,#%0064
	jr	eq,Lc258
	cp	r13,#%0075
	jr	eq,Lc260
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,Lc2d0
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%6C
	ldb	rl2,#%6C
	extsb	r2
	jr	Lc2dc
Lc2d0:
	ld	r7,#%006C
	ld	r6,%0002(r15)
	call	__flsbuf
Lc2dc:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,Lc32c
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
Lc2fe:
	test	%000E(r15)
	jr	eq,Lc316
	cp	%005E(r15),#%0073
	jr	ne,Lc316
	cp	r11,%000E(r15)
	jr	le,Lc316
	ld	r11,%000E(r15)
Lc316:
	cp	r11,%000C(r15)
	jp	ge,Lc4b2
	ld	r2,%000C(r15)
	sub	r2,r11
	ld	%005A(r15),r2
	jp	Lc4b6
Lc32c:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
	jr	Lc2fe
Lc338:
	ld	%0010(r15),#%FFF6
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,Lc364
Lc35c:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
Lc364:
	lda	r2,%0018(r15)
	ld	%0016(r15),r2
	cp	%0010(r15),#%FFF6
	jr	ne,Lc3ce
	ldl	rr2,%0012(r15)
	cpl	rr2,#%00000000
	jr	ge,Lc3ce
	cpl	rr2,#%80000000
	jr	ne,Lc38e
	ld	%0010(r15),#%000A
Lc38e:
	ldl	rr6,%0012(r15)
	multl	rq4,#%FFFFFFFF
	ldl	%0012(r15),rr6
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,Lc3c2
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	Lc3ce
Lc3c2:
	ld	r7,#%002D
	ld	r6,%0002(r15)
	call	__flsbuf
Lc3ce:
	ld	r7,%0016(r15)
	ld	r6,%0010(r15)
	ldl	rr4,%0012(r15)
	call	__cnvt
Lc3de:
	ld	r11,r2
	jr	Lc2fe
Lc3e2:
	ldb	%005D(r15),#%20
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%0016(r15),r3
	inc	%000A(r15),#2
	test	r3
	jr	ne,Lc400
	ld	%0016(r15),#D0000
Lc400:
	ld	r7,%0016(r15)
	call	__length
	ld	r11,r2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	ne,Lc2fe
Lc418:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jp	Lc2fe
Lc424:
	ldb	%005D(r15),#%20
	ldk	r11,#1
	ld	r2,%000A(r15)
	ld	%0016(r15),r2
	inc	%000A(r15),#2
	inc	%0016(r15),#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	eq,Lc418
	jp	Lc2fe
Lc44c:
	cp	%0004(r15),#%0005
	jr	ge,Lc45c
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
Lc45c:
	ld	@r15,r13
	lda	r2,%0018(r15)
	ld	r3,%000A(r15)
	.word	%0F34,%8003	// EPU
	.word	%8F00,%8403	// EPU
	ld	r3,%000E(r15)
	call	__inflt
	ld	%0016(r15),r2
	inc	%000A(r15),#8
	inc	%0004(r15),#5
	ld	r7,r2
	call	__length
	jr	Lc3de
Lc48a:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	lt,Lc084
Lc49e:
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
	jp	Lc08e
Lc4b2:
	clr	%005A(r15)
Lc4b6:
	test	%0058(r15)
	jr	eq,Lc4f4
	jr	Lc544
Lc4be:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
	jr	Lc4f4
Lc4d2:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,Lc4be
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
Lc4f4:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jr	ne,Lc4d2
	jr	Lc544
Lc502:
	ld	r2,%0016(r15)
	inc	%0016(r15),#1
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	Lc544
Lc51c:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,Lc502
	ld	r7,%0016(r15)
	inc	%0016(r15),#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r0,r2
Lc544:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,Lc51c
	test	%0058(r15)
	jr	ne,Lc568
	jp	Lc08e
Lc556:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
Lc568:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jp	eq,Lc08e
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,Lc556
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
	jr	Lc568
Lc59a:
	ldm	r11,%0060(r15),#4
	add	r15,#%0068
	ret
__atoip::
	dec	r15,#14
	ldm	%0000(r15),r5,#2
	ld	r1,r7
	clr	%0004(r15)
	ld	r4,r6
	test	r6
	jr	eq,Lc5cc
	cpb	@r1,#%2D
	jr	ne,Lc5cc
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0004(r15),#%0001
Lc5cc:
	ldk	r5,#0
	jr	Lc5fc
Lc5d0:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0007(r15),rl3
	cpb	rl3,#%30
	jr	lt,Lc600
	cpb	rl3,#%39
	jr	gt,Lc600
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0007(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
Lc5fc:
	test	r4
	jr	ne,Lc5d0
Lc600:
	test	%0004(r15)
	jr	eq,Lc60c
	ld	r2,r5
	neg	r2
	jr	Lc60e
Lc60c:
	ld	r2,r5
Lc60e:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	inc	r15,#14
	.data
D0000:
	.byte	%28,%6E
	.byte	%75,%6C
	.byte	%6C,%29
	.byte	%00,%00
D0008:
	.byte	%00,%65
	.byte	%00,%66
	.byte	%00,%67
	.byte	%00,%73
	.addr	Lc1ec
	.addr	Lc1ec
	.addr	Lc1ec
	.addr	Lc1cc
D0018:
	.byte	%00,%44
	.byte	%00,%4F
	.byte	%00,%55
	.byte	%00,%58
	.byte	%00,%63
	.byte	%00,%64
	.byte	%00,%65
	.byte	%00,%66
	.byte	%00,%67
	.byte	%00,%6C
	.byte	%00,%6F
	.byte	%00,%73
	.byte	%00,%75
	.byte	%00,%78
	.addr	Lc258
	.addr	Lc248
	.addr	Lc260
	.addr	Lc250
	.addr	Lc424
	.addr	Lc338
	.addr	Lc44c
	.addr	Lc44c
	.addr	Lc44c
	.addr	Lc232
	.addr	Lc1f4
	.addr	Lc3e2
	.addr	Lc204
	.addr	Lc1fc
