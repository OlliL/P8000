	.extern	__filbuf, __inflt, _tolower, _ungetc
	.psec
	.code
__doscan::
	sub	r15,#%0020
	ldm	%001A(r15),r12,#3
	ldm	@r15,r4,#4
	ld	r13,r5
	ld	r14,r6
	clr	%000A(r15)
	clr	%0012(r15)
	clr	%0016(r15)
	jp	L0184
L0022:
	add	r4,r4
	neg	r4
	ld	r2,D0092(r4)
	jp	@r2
L002c:
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r12,r3
	cp	r3,#%0025
	jp	eq,L019e
	clr	%0010(r15)
	cp	r3,#%002A
	jr	eq,L0060
	ld	%0010(r15),r13
	inc	r13,#2
	inc	%0016(r15),#1
	cp	%0016(r15),#%0005
	jr	ne,L0068
	ld	r13,%0000(r15)
	jr	L0068
L0060:
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L0068:
	clr	%000C(r15)
	ld	%0014(r15),#%0001
	jr	L008e
L0074:
	ld	r3,%000C(r15)
	mult	rr2,#%000A
	add	r3,r12
	sub	r3,#%0030
	ld	%000C(r15),r3
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L008e:
	ldb	rl2,%09B5(r12)
	extsb	r2
	bit	r2,#2
	jr	ne,L0074
	test	%000C(r15)
	jr	ne,L00a4
	ld	%000C(r15),#%7530
L00a4:
	cp	r12,#%006C
	jr	ne,L00ba
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	ld	%0014(r15),#%0002
	jr	L00da
L00ba:
	cp	r12,#%0068
	jr	ne,L00ce
	clr	%0014(r15)
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	jr	L00da
L00ce:
	cp	r12,#%005B
	jr	ne,L00da
	ld	r7,r14
	calr	_getccl
	ld	r14,r2
L00da:
	ldb	rl2,%09B5(r12)
	extsb	r2
	bit	r2,#0
	jr	eq,L00f2
	ld	r7,r12
	call	_tolower
	ld	r12,r2
	ld	%0014(r15),#%0002
L00f2:
	test	r12
	jr	ne,L00fc
L00f6:
	ld	r2,#%FFFF
	jr	L01ea
L00fc:
	lda	r2,%0012(r15)
	ld	r6,r12
	ld	r5,%000C(r15)
	ld	r4,%0014(r15)
	ld	r3,%0006(r15)
	ld	r7,%0010(r15)
	calr	_innum
	test	r2
	jr	eq,L0122
	test	%0010(r15)
	jr	eq,L0122
	inc	%000A(r15),#1
L0122:
	test	%0012(r15)
	jr	eq,L0184
	test	%000A(r15)
	jr	eq,L00f6
	jr	L01e6
L0130:
	ld	r2,%0006(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0156
	ld	r2,%0006(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	extsb	r2
	and	r2,#%00FF
	jr	L015e
L0156:
	ld	r7,%0006(r15)
	call	__filbuf
L015e:
	ld	%000E(r15),r2
	cp	r2,#%0020
	jr	eq,L0130
	cp	r2,#%0009
	jr	eq,L0130
	cp	r2,#%000A
	jr	eq,L0130
	cp	r2,#%FFFF
	jr	eq,L0184
	ld	r7,r2
	ld	r6,%0006(r15)
	call	_ungetc
L0184:
	ldb	rl3,@r14
	extsb	r3
	ld	r12,r3
	ld	r2,r3
	inc	r14,#1
	ld	r3,#D0080
	ld	r4,#%0005
	cpir	r2,@r3,r4,eq
	jp	eq,L0022
L019e:
	ld	r2,%0006(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L01c4
	ld	r2,%0006(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	extsb	r2
	and	r2,#%00FF
	jr	L01cc
L01c4:
	ld	r7,%0006(r15)
	call	__filbuf
L01cc:
	ld	%000E(r15),r2
	cp	r12,%000E(r15)
	jr	eq,L0184
	cp	r2,#%FFFF
	jr	eq,L00f6
	ld	r7,r2
	ld	r6,%0006(r15)
	call	_ungetc
L01e6:
	ld	r2,%000A(r15)
L01ea:
	ldm	r12,%001A(r15),#3
	add	r15,#%0020
	ret
_innum::
	sub	r15,#%0020
	ldm	%001C(r15),r13,#2
	ldm	@r15,r2,#6
	clr	%0014(r15)
	cp	r6,#%0063
	jr	eq,L021a
	cp	r6,#%0073
	jr	eq,L021a
	cp	r6,#%005B
	jr	ne,L0240
L021a:
	test	r7
	jr	eq,L0228
	ld	r7,%000A(r15)
	ld	r0,@r7
	ld	r7,r0
	jr	L022a
L0228:
	sub	r7,r7
L022a:
	ld	r6,%0008(r15)
	ld	r5,%0006(r15)
	ld	r4,%0002(r15)
	ld	r3,%0000(r15)
	calr	_instr
	jp	L0486
L0240:
	cp	%0008(r15),#%0065
	jr	eq,L0250
	cp	%0008(r15),#%0066
	jr	ne,L026c
L0250:
	ld	r7,%000A(r15)
	ld	r6,%0006(r15)
	ld	r5,%0004(r15)
	ld	r4,%0002(r15)
	ld	r3,%0000(r15)
	call	__inflt
	jp	L0486
L026c:
	subl	rr2,rr2
	ldl	%0016(r15),rr2
	ldk	r13,#10
	cp	%0008(r15),#%006F
	jr	ne,L0280
	ldk	r13,#8
	jr	L028c
L0280:
	cp	%0008(r15),#%0078
	jr	ne,L028c
	ld	r13,#%0010
L028c:
	clr	%0010(r15)
L0290:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L02b6
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	extsb	r2
	and	r2,#%00FF
	jr	L02be
L02b6:
	ld	r7,%0002(r15)
	call	__filbuf
L02be:
	ld	r14,r2
	cp	r2,#%0020
	jr	eq,L0290
	cp	r2,#%0009
	jr	eq,L0290
	cp	r2,#%000A
	jr	eq,L0290
	cp	r2,#%002D
	jr	ne,L033e
	inc	%0010(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0302
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	extsb	r2
	and	r2,#%00FF
	jr	L030a
L0302:
	ld	r7,%0002(r15)
	call	__filbuf
L030a:
	ld	r14,r2
	dec	%0006(r15),#1
	jr	L0378
L0312:
	test	%0010(r15)
	jr	eq,L0322
	subl	rr2,rr2
	subl	rr2,%0016(r15)
	ldl	%0016(r15),rr2
L0322:
	cp	r14,#%FFFF
	jp	eq,L0436
	ld	r7,r14
	ld	r6,%0002(r15)
	call	_ungetc
	ld	r2,%0000(r15)
	clr	@r2
	jp	L043e
L033e:
	cp	r14,#%002B
	jr	ne,L0378
	dec	%0006(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L036e
L035a:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	extsb	r2
	and	r2,#%00FF
	jr	L0376
L036e:
	ld	r7,%0002(r15)
	call	__filbuf
L0376:
	ld	r14,r2
L0378:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0312
	ldb	rl2,%09B5(r14)
	extsb	r2
	bit	r2,#2
	jr	ne,L03ac
	cp	r13,#%0010
	jr	ne,L0312
	cp	r14,#%0061
	jr	lt,L03a0
	cp	r14,#%0066
	jr	le,L03ac
L03a0:
	cp	r14,#%0041
	jr	lt,L0312
	cp	r14,#%0046
	jr	gt,L0312
L03ac:
	ld	%0014(r15),#%0001
	cp	r13,#%0008
	jr	ne,L03c2
	ldl	rr2,%0016(r15)
	slal	rr2,#3
	jr	L03e2
L03c2:
	cp	r13,#%000A
	jr	ne,L03da
	ldl	rr2,%0016(r15)
	slal	rr2,#2
	addl	rr2,%0016(r15)
	slal	rr2,#1
	jr	L03e2
L03da:
	ldl	rr2,%0016(r15)
	slal	rr2,#4
L03e2:
	ldl	%0016(r15),rr2
	ld	%0012(r15),r14
	cp	r14,#%0030
	jr	lt,L03fc
	cp	r14,#%0039
	jr	gt,L03fc
	sub	r14,#%0030
	jr	L0412
L03fc:
	cp	r14,#%0061
	jr	lt,L040e
	cp	r14,#%0066
	jr	gt,L040e
	sub	r14,#%0057
	jr	L0412
L040e:
	sub	r14,#%0037
L0412:
	ld	r3,r14
	exts	rr2
	addl	rr2,%0016(r15)
	ldl	%0016(r15),rr2
	ld	r14,%0012(r15)
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L036e
	jr	L035a
L0436:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L043e:
	test	%000A(r15)
	jr	eq,L0484
	ld	r2,%0004(r15)
	ld	r3,#D0094
	ld	r4,#%0003
	cpir	r2,@r3,r4,eq
	jr	ne,L047e
	add	r4,r4
	neg	r4
	ld	r2,D009e(r4)
	jp	@r2
L0460:
	ld	r3,%000A(r15)
	ld	r4,@r3
	ld	r5,%0018(r15)
	ld	@r4,r5
	jr	L047e
L046e:
	ldk	r2,#1
	jr	L0486
L0472:
	ld	r3,%000A(r15)
	ld	r4,@r3
	ldl	rr6,%0016(r15)
	ldl	@r4,rr6
L047e:
	test	%0014(r15)
	jr	ne,L046e
L0484:
	sub	r2,r2
L0486:
	ldm	r13,%001C(r15),#2
	add	r15,#%0020
	ret
_instr::
	sub	r15,#%001A
	ldm	%0012(r15),r11,#4
	ldm	@r15,r3,#4
	ld	r13,r4
	ld	r14,r7
	ld	r2,r3
	clr	@r2
	ld	r11,r14
	cp	r6,#%0063
	jr	ne,L04bc
	cp	r5,#%7530
	jr	ne,L04bc
	ld	%0004(r15),#%0001
L04bc:
	clr	%000E(r15)
	cp	%0006(r15),#%0073
	jr	ne,L04ce
	ld	%000E(r15),#%0001
L04ce:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L04e8
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	jr	L04f0
L04e8:
	ld	r7,r13
	call	__filbuf
	ld	r3,r2
L04f0:
	ld	r12,r3
	ldb	rl2,D0000(r3)
	extsb	r2
	and	r2,%000E(r15)
	jr	eq,L0504
	cp	r12,#%FFFF
	jr	ne,L04ce
L0504:
	ld	%000E(r15),#%0001
	cp	%0006(r15),#%0063
	jr	ne,L0518
	clr	%000E(r15)
	jr	L0570
L0518:
	cp	%0006(r15),#%005B
	jr	ne,L0570
	ld	%000E(r15),#%0002
	jr	L0570
L0528:
	ldb	rl2,D0000(r12)
	extsb	r2
	and	r2,%000E(r15)
	test	r2
	jr	ne,L0576
	test	r14
	jr	eq,L0540
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
L0540:
	dec	%0004(r15),#1
	cp	%0004(r15),#%0000
	jr	le,L0576
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L0566
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	extsb	r3
	and	r3,#%00FF
	jr	L056e
L0566:
	ld	r7,r13
	call	__filbuf
	ld	r3,r2
L056e:
	ld	r12,r3
L0570:
	cp	r12,#%FFFF
	jr	ne,L0528
L0576:
	cp	r12,#%FFFF
	jr	eq,L0594
	cp	%0004(r15),#%0000
	jr	le,L058c
	ld	r7,r12
	ld	r6,r13
	call	_ungetc
L058c:
	ld	r2,%0000(r15)
	clr	@r2
	jr	L059c
L0594:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L059c:
	test	r14
	jr	eq,L05b4
	cp	r14,r11
	jr	eq,L05b4
	cp	%0006(r15),#%0063
	jr	eq,L05b0
	clrb	@r14
	inc	r14,#1
L05b0:
	ldk	r2,#1
	jr	L05b6
L05b4:
	sub	r2,r2
L05b6:
	ldm	r11,%0012(r15),#4
	add	r15,#%001A
	ret
_getccl::
	dec	r15,#14
	ld	r1,r7
	ldk	r5,#0
	cpb	@r1,#%5E
	jr	ne,L05d2
	inc	r5,#1
	inc	r1,#1
L05d2:
	ldk	r4,#0
L05d4:
	test	r5
	jr	eq,L05de
	resb	D0000(r4),#1
	jr	L05e2
L05de:
	setb	D0000(r4),#1
L05e2:
	inc	r4,#1
	cp	r4,#%0080
	jr	lt,L05d4
L05ea:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r4,r3
	and	r3,#%007F
	cp	r3,#%005D
	jr	eq,L0614
	test	r5
	jr	eq,L0608
	setb	D0000(r4),#1
	jr	L060c
L0608:
	resb	D0000(r4),#1
L060c:
	inc	r4,#1
	test	r4
	jr	ne,L05ea
	dec	r1,#1
L0614:
	ld	r2,r1
	inc	r15,#14
	ret
	.psec
	.data
D0000:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%01,%01,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%01,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
D0080:
	.byte	%00,%00,%00,%09,%00,%0a,%00,%20
	.byte	%00,%25
	.addr	L01e6
	.addr	L0130
	.addr	L0130
	.addr	L0130
D0092:
	.addr	L002c
D0094:
	.byte	%00,%00,%00,%01,%00,%02
	.addr	L0460
	.addr	L0460
D009e:
	.addr	L0472
