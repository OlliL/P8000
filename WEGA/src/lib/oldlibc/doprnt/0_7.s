	.extern	__cnvt, __flsbuf, __inflt, __length
	.psec
	.code
__align::
	dec	r15,#8
	ld	%0000(r15),r5
	ld	r2,r7
	test	@r2
	jr	eq,L0014
	ld	r2,r7
	cp	@r2,#%0002
	jr	ne,L001e
L0014:
	ld	r2,r7
	inc	@r2,#1
	ld	r2,r6
	inc	@r2,#2
	jr	L0030
L001e:
	ld	r2,r7
	cp	@r2,#%0004
	jr	ne,L0030
	ld	r2,r7
	inc	@r2,#2
	ld	r2,r6
	ld	@r2,r5
	jr	L0054
L0030:
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
L0054:
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
	jr	L0098
L007a:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	ge,L0498
L008e:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
L0098:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0594
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%0025
	jr	ne,L007a
	test	r12
	jp	eq,L0594
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
	jr	ne,L00f2
	inc	%0058(r15),#1
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0594
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L00f2:
	cp	r13,#%0061
	jr	lt,L01a0
	jr	L01b8
L00fa:
	cp	r13,#%002A
	jr	ne,L0124
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000C(r15),r3
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0144
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L0144
L0124:
	cp	r13,#%0030
	jr	ne,L0130
	ldb	%005D(r15),#%30
L0130:
	lda	r5,%000C(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	L05a0
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L0144:
	test	r12
	jr	eq,L01b8
	cpb	@r14,#%2E
	jr	ne,L0192
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,L01b8
	cpb	@r14,#%2A
	jr	ne,L0182
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000E(r15),r3
	inc	%000A(r15),#2
	ldk	r11,#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L018e
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L018e
L0182:
	lda	r5,%000E(r15)
	ld	r6,r12
	ld	r7,r14
	calr	L05a0
	ld	r11,r2
L018e:
	sub	r12,r11
	add	r14,r11
L0192:
	test	r12
	jr	eq,L01b8
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L01a0:
	cp	r13,#%002A
	jr	eq,L00fa
	cp	r13,#%0030
	jr	lt,L01b2
	cp	r13,#%0039
	jr	le,L00fa
L01b2:
	cp	r13,#%002E
	jr	eq,L00fa
L01b8:
	cp	%000E(r15),#%0000
	jr	ge,L01da
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,L01da
	ld	r2,%0006(r3)
	jp	@r2
L01d6:
	clr	%000E(r15)
L01da:
	ld	%005E(r15),r13
	ld	r2,r13
	ld	r3,#D0018
	ld	r4,#%000E
	cpir	r2,@r3,r4,eq
	jp	ne,L0484
	ld	r2,%001A(r3)
	jp	@r2
L01f6:
	ld	%000E(r15),#%0006
	jr	L01da
L01fe:
	ld	%0010(r15),#%0008
	jr	L0214
L0206:
	ld	%0010(r15),#%0010
	jr	L0214
L020e:
	ld	%0010(r15),#%000A
L0214:
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	clr	%0012(r15)
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	eq,L0366
	jp	L036e
L023c:
	test	r12
	jr	eq,L0308
	dec	r12,#1
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%006F
	jr	ne,L02a2
L0252:
	ld	%0010(r15),#%0008
	jr	L0270
L025a:
	ld	%0010(r15),#%0010
	jr	L0270
L0262:
	ld	%0010(r15),#%FFF6
	jr	L0270
L026a:
	ld	%0010(r15),#%000A
L0270:
	cp	%0004(r15),#%0005
	jr	ge,L0286
	lda	r7,%0004(r15)
	lda	r6,%000A(r15)
	ld	r5,%0006(r15)
	calr	__align
L0286:
	ld	r2,%000A(r15)
	ldl	rr0,@r2
	ldl	%0012(r15),rr0
	inc	%000A(r15),#4
	inc	%0004(r15),#2
	cp	%0004(r15),#%0005
	jr	eq,L0366
	jr	L036e
L02a2:
	cp	r13,#%0078
	jr	eq,L025a
	cp	r13,#%0064
	jr	eq,L0262
	cp	r13,#%0075
	jr	eq,L026a
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L02da
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%6C
	ldb	rl2,#%6C
	extsb	r2
	jr	L02e6
L02da:
	ld	r7,#%006C
	ld	r6,%0002(r15)
	call	__flsbuf
L02e6:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0336
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
L0308:
	test	%000E(r15)
	jr	eq,L0320
	cp	%005E(r15),#%0073
	jr	ne,L0320
	cp	r11,%000E(r15)
	jr	le,L0320
	ld	r11,%000E(r15)
L0320:
	cp	r11,%000C(r15)
	jp	ge,L04ac
	ld	r2,%000C(r15)
	sub	r2,r11
	ld	%005A(r15),r2
	jp	L04b0
L0336:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
	jr	L0308
L0342:
	ld	%0010(r15),#%FFF6
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L036e
L0366:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L036e:
	lda	r2,%0018(r15)
	ld	%0016(r15),r2
	cp	%0010(r15),#%FFF6
	jr	ne,L03c8
	ldl	rr2,%0012(r15)
	cpl	rr2,#%00000000
	jr	ge,L03c8
	ldl	rr6,rr2
	multl	rq4,#%FFFFFFFF
	ldl	%0012(r15),rr6
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L03bc
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L03c8
L03bc:
	ld	r7,#%002D
	ld	r6,%0002(r15)
	call	__flsbuf
L03c8:
	ld	r7,%0016(r15)
	ld	r6,%0010(r15)
	ldl	rr4,%0012(r15)
	call	__cnvt
L03d8:
	ld	r11,r2
	jr	L0308
L03dc:
	ldb	%005D(r15),#%20
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%0016(r15),r3
	inc	%000A(r15),#2
	test	r3
	jr	ne,L03fa
	ld	%0016(r15),#D0000
L03fa:
	ld	r7,%0016(r15)
	call	__length
	ld	r11,r2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	ne,L0308
L0412:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jp	L0308
L041e:
	ldb	%005D(r15),#%20
	ldk	r11,#1
	ld	r2,%000A(r15)
	ld	%0016(r15),r2
	inc	%000A(r15),#2
	inc	%0016(r15),#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	eq,L0412
	jp	L0308
L0446:
	cp	%0004(r15),#%0005
	jr	ge,L0456
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L0456:
	ld	@r15,r13
	lda	r2,%0018(r15)
	ld	r3,%000A(r15)
	.word	%0F34,%8003
	.word	%8F00,%8403
	ld	r3,%000E(r15)
	call	__inflt
	ld	%0016(r15),r2
	inc	%000A(r15),#8
	inc	%0004(r15),#5
	ld	r7,r2
	call	__length
	jr	L03d8
L0484:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	lt,L008e
L0498:
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
	jp	L0098
L04ac:
	clr	%005A(r15)
L04b0:
	test	%0058(r15)
	jr	eq,L04ee
	jr	L053e
L04b8:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
	jr	L04ee
L04cc:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L04b8
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
L04ee:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jr	ne,L04cc
	jr	L053e
L04fc:
	ld	r2,%0016(r15)
	inc	%0016(r15),#1
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	L053e
L0516:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L04fc
	ld	r7,%0016(r15)
	inc	%0016(r15),#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r0,r2
L053e:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0516
	test	%0058(r15)
	jr	ne,L0562
	jp	L0098
L0550:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
L0562:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jp	eq,L0098
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L0550
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
	jr	L0562
L0594:
	ldm	r11,%0060(r15),#4
	add	r15,#%0068
	ret
L05a0:
	dec	r15,#14
	ldm	%0000(r15),r5,#2
	ld	r1,r7
	clr	%0004(r15)
	ld	r4,r6
	test	r6
	jr	eq,L05c6
	cpb	@r1,#%2D
	jr	ne,L05c6
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0004(r15),#%0001
L05c6:
	ldk	r5,#0
	jr	L05f6
L05ca:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0007(r15),rl3
	cpb	rl3,#%30
	jr	lt,L05fa
	cpb	rl3,#%39
	jr	gt,L05fa
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0007(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L05f6:
	test	r4
	jr	ne,L05ca
L05fa:
	test	%0004(r15)
	jr	eq,L0606
	ld	r2,r5
	neg	r2
	jr	L0608
L0606:
	ld	r2,r5
L0608:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	inc	r15,#14
	.psec
	.data
D0000:
	.byte	%28,%6e,%75,%6c,%6c,%29,%00,%00
D0008:
	.byte	%00,%65,%00,%66,%00,%67,%00,%73
	.addr	L01f6
	.addr	L01f6
	.addr	L01f6
	.addr	L01d6
D0018:
	.byte	%00,%44,%00,%4f,%00,%55,%00,%58
	.byte	%00,%63,%00,%64,%00,%65,%00,%66
	.byte	%00,%67,%00,%6c,%00,%6f,%00,%73
	.byte	%00,%75,%00,%78
	.addr	L0262
	.addr	L0252
	.addr	L026a
	.addr	L025a
	.addr	L041e
	.addr	L0342
	.addr	L0446
	.addr	L0446
	.addr	L0446
	.addr	L023c
	.addr	L01fe
	.addr	L03dc
	.addr	L020e
	.addr	L0206
