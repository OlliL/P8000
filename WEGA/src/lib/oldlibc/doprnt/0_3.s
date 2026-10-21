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
	jp	ge,L042a
L008e:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
L0098:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0526
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%0025
	jr	ne,L007a
	test	r12
	jp	eq,L0526
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
	jp	eq,L0526
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L00f2:
	cp	r13,#%0061
	jr	ge,L01a4
	cp	r13,#%002A
	jr	ne,L0122
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000C(r15),r3
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0142
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L0142
L0122:
	cp	r13,#%0030
	jr	ne,L012e
	ldb	%005D(r15),#%30
L012e:
	lda	r5,%000C(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L0142:
	test	r12
	jp	eq,L0526
	cpb	@r14,#%2E
	jr	ne,L0194
	inc	r14,#1
	dec	r12,#1
	test	r12
	jp	eq,L0526
	cpb	@r14,#%2A
	jr	ne,L0184
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000E(r15),r3
	inc	%000A(r15),#2
	ldk	r11,#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0190
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L0190
L0184:
	lda	r5,%000E(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L0190:
	sub	r12,r11
	add	r14,r11
L0194:
	test	r12
	jp	eq,L0526
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L01a4:
	cp	%000E(r15),#%0000
	jr	ge,L01c6
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,L01c6
	ld	r2,%0006(r3)
	jp	@r2
L01c2:
	clr	%000E(r15)
L01c6:
	ld	%005E(r15),r13
	ld	r2,r13
	ld	r3,#D0018
	ld	r4,#%000E
	cpir	r2,@r3,r4,eq
	jp	ne,L0416
	ld	r2,%001A(r3)
	jp	@r2
L01e2:
	ld	%000E(r15),#%0006
	jr	L01c6
L01ea:
	ld	%0010(r15),#%0008
	jr	L0200
L01f2:
	ld	%0010(r15),#%0010
	jr	L0200
L01fa:
	ld	%0010(r15),#%000A
L0200:
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	clr	%0012(r15)
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	eq,L0352
	jp	L035a
L0228:
	test	r12
	jr	eq,L02f4
	dec	r12,#1
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%006F
	jr	ne,L028e
L023e:
	ld	%0010(r15),#%0008
	jr	L025c
L0246:
	ld	%0010(r15),#%0010
	jr	L025c
L024e:
	ld	%0010(r15),#%FFF6
	jr	L025c
L0256:
	ld	%0010(r15),#%000A
L025c:
	cp	%0004(r15),#%0005
	jr	ge,L0272
	lda	r7,%0004(r15)
	lda	r6,%000A(r15)
	ld	r5,%0006(r15)
	calr	__align
L0272:
	ld	r2,%000A(r15)
	ldl	rr0,@r2
	ldl	%0012(r15),rr0
	inc	%000A(r15),#4
	inc	%0004(r15),#2
	cp	%0004(r15),#%0005
	jr	eq,L0352
	jr	L035a
L028e:
	cp	r13,#%0078
	jr	eq,L0246
	cp	r13,#%0064
	jr	eq,L024e
	cp	r13,#%0075
	jr	eq,L0256
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L02c6
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%6C
	ldb	rl2,#%6C
	extsb	r2
	jr	L02d2
L02c6:
	ld	r7,#%006C
	ld	r6,%0002(r15)
	call	__flsbuf
L02d2:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0322
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
L02f4:
	test	%000E(r15)
	jr	eq,L030c
	cp	%005E(r15),#%0073
	jr	ne,L030c
	cp	r11,%000E(r15)
	jr	le,L030c
	ld	r11,%000E(r15)
L030c:
	cp	r11,%000C(r15)
	jp	ge,L043e
	ld	r2,%000C(r15)
	sub	r2,r11
	ld	%005A(r15),r2
	jp	L0442
L0322:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
	jr	L02f4
L032e:
	ld	%0010(r15),#%FFF6
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L035a
L0352:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L035a:
	lda	r2,%0018(r15)
	ld	%0016(r15),r2
	ld	r7,r2
	ld	r6,%0010(r15)
	ldl	rr4,%0012(r15)
	call	__cnvt
L0370:
	ld	r11,r2
	jr	L02f4
L0374:
	ldb	%005D(r15),#%20
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%0016(r15),r3
	inc	%000A(r15),#2
	test	r3
	jr	ne,L0392
	ld	%0016(r15),#D0000
L0392:
	ld	r7,%0016(r15)
	call	__length
	ld	r11,r2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L02f4
L03a8:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L02f4
L03b2:
	ldb	%005D(r15),#%20
	ldk	r11,#1
	ld	r2,%000A(r15)
	ld	%0016(r15),r2
	inc	%000A(r15),#2
	inc	%0016(r15),#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	eq,L03a8
	jr	L02f4
L03d8:
	cp	%0004(r15),#%0005
	jr	ge,L03e8
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L03e8:
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
	jr	L0370
L0416:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	lt,L008e
L042a:
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
	jp	L0098
L043e:
	clr	%005A(r15)
L0442:
	test	%0058(r15)
	jr	eq,L0480
	jr	L04d0
L044a:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
	jr	L0480
L045e:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L044a
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
L0480:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jr	ne,L045e
	jr	L04d0
L048e:
	ld	r2,%0016(r15)
	inc	%0016(r15),#1
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	L04d0
L04a8:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L048e
	ld	r7,%0016(r15)
	inc	%0016(r15),#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r0,r2
L04d0:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L04a8
	test	%0058(r15)
	jr	ne,L04f4
	jp	L0098
L04e2:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
L04f4:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jp	eq,L0098
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L04e2
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
	jr	L04f4
L0526:
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
	jr	eq,L0558
	cpb	@r1,#%2D
	jr	ne,L0558
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0004(r15),#%0001
L0558:
	ldk	r5,#0
	jr	L0588
L055c:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0007(r15),rl3
	cpb	rl3,#%30
	jr	lt,L058c
	cpb	rl3,#%39
	jr	gt,L058c
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0007(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L0588:
	test	r4
	jr	ne,L055c
L058c:
	test	%0004(r15)
	jr	eq,L0598
	ld	r2,r5
	neg	r2
	jr	L059a
L0598:
	ld	r2,r5
L059a:
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
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01c2
D0018:
	.byte	%00,%44,%00,%4f,%00,%55,%00,%58
	.byte	%00,%63,%00,%64,%00,%65,%00,%66
	.byte	%00,%67,%00,%6c,%00,%6f,%00,%73
	.byte	%00,%75,%00,%78
	.addr	L024e
	.addr	L023e
	.addr	L0256
	.addr	L0246
	.addr	L03b2
	.addr	L032e
	.addr	L03d8
	.addr	L03d8
	.addr	L03d8
	.addr	L0228
	.addr	L01ea
	.addr	L0374
	.addr	L01fa
	.addr	L01f2
