	.extern	__cnvt, __flsbuf, __fegcvt, __length, _strcpy
	.psec
	.code
__align::
	dec	r15,#8
	ldm	@r15,r5,#3
	ld	r4,r7
	test	@r4
	jr	eq,L0012
	cp	@r7,#%0002
	jr	ne,L001c
L0012:
	ld	r7,r4
	inc	@r7,#1
	ld	r7,r6
	inc	@r7,#2
	jr	L0034
L001c:
	ld	r2,%0004(r15)
	cp	@r2,#%0004
	jr	ne,L0034
	inc	@r2,#2
	ld	r2,%0002(r15)
	ld	r3,%0000(r15)
	ld	@r2,r3
	jr	L0058
L0034:
	ld	r2,%0002(r15)
	ld	r3,@r2
	ld	r4,@r3
	ld	%0006(r15),r4
	ld	r3,@r2
	ld	r5,@r2
	ld	r6,%0002(r3)
	ld	@r5,r6
	ld	r2,%0002(r15)
	ld	r3,@r2
	ld	r5,%0006(r15)
	ld	%0002(r3),r5
L0058:
	inc	r15,#8
	ret
__doprnt::
	sub	r15,#%0086
	ldm	%007E(r15),r11,#4
	ldm	@r15,r2,#5
	ld	r14,r7
	clr	%000C(r15)
	ld	%000E(r15),r6
	call	__length
	ld	r12,r2
	jr	L009a
L007c:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jp	ge,L07ce
L0090:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
L009a:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L08c8
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r13,r3
	cp	r3,#%0025
	jr	ne,L007c
	test	r12
	jp	eq,L08c8
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%0018(r15)
	ld	%001A(r15),#%FFFF
	ldb	%007B(r15),#%20
	clr	%0078(r15)
	clr	%0074(r15)
	clr	%0072(r15)
	clr	%0076(r15)
	clr	%006E(r15)
	jr	L0130
L00ea:
	test	%0074(r15)
	jr	eq,L015e
	ldb	%006B(r15),#%20
	ld	%0076(r15),#%0001
	jr	L0164
L00fe:
	test	%0076(r15)
	jr	ne,L0158
	ld	%0076(r15),#%0001
	jr	L0150
L010c:
	test	%0074(r15)
	jr	ne,L0158
	ld	%0074(r15),#%0001
	jr	L0150
L011a:
	test	%0072(r15)
	jr	ne,L0158
	ld	%0072(r15),#%0001
	jr	L0150
L0128:
	ldk	r3,#0
	ldb	rl3,@r14
	ld	r13,r3
	inc	r14,#1
L0130:
	ld	r2,r13
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L0158
	rl	r2,#1
	ld	r2,D0034(r2)
	jp	@r2
L0144:
	test	%006E(r15)
	jr	ne,L0158
	ld	%006E(r15),#%0001
L0150:
	ld	r3,r12
	dec	r12,#1
	test	r3
	jr	ne,L0128
L0158:
	test	%0076(r15)
	jr	eq,L00ea
L015e:
	ldb	%006B(r15),#%2B
L0164:
	cp	r13,#%0061
	jr	ge,L0216
	cp	r13,#%002A
	jr	ne,L0194
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0018(r15),r3
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L01b4
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L01b4
L0194:
	cp	r13,#%0030
	jr	ne,L01a0
	ldb	%007B(r15),#%30
L01a0:
	lda	r5,%0018(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L01b4:
	test	r12
	jp	eq,L08c8
	cpb	@r14,#%2E
	jr	ne,L0206
	inc	r14,#1
	dec	r12,#1
	test	r12
	jp	eq,L08c8
	cpb	@r14,#%2A
	jr	ne,L01f6
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%001A(r15),r3
	inc	%000E(r15),#2
	ldk	r11,#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0202
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L0202
L01f6:
	lda	r5,%001A(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L0202:
	sub	r12,r11
	add	r14,r11
L0206:
	test	r12
	jp	eq,L08c8
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L0216:
	cp	%001A(r15),#%0000
	jr	ge,L023c
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0006
	cpir	r2,@r3,r4,eq
	jr	ne,L023c
	add	r4,r4
	neg	r4
	ld	r2,D005a(r4)
	jp	@r2
L0238:
	clr	%001A(r15)
L023c:
	ld	%007C(r15),r13
	ld	r2,r13
	ld	r3,#D0014
	ld	r4,#%0010
	cpir	r2,@r3,r4,eq
	jp	ne,L07ba
	add	r4,r4
	neg	r4
	ld	r2,D007a(r4)
	jp	@r2
L025c:
	ld	%001A(r15),#%0006
	jr	L023c
L0264:
	ld	%001C(r15),#%0008
	jr	L027a
L026c:
	ld	%001C(r15),#%0010
	jr	L027a
L0274:
	ld	%001C(r15),#%000A
L027a:
	ld	r3,%000E(r15)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	%001E(r15),rr6
	inc	%000E(r15),#2
	clr	%001E(r15)
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L03dc
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jp	L03dc
L02aa:
	test	r12
	jr	eq,L0376
	dec	r12,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r3
	subb	rh4,rh4
	ld	r13,r4
	cp	r4,#%006F
	jr	ne,L0310
L02c0:
	ld	%001C(r15),#%0008
	jr	L02de
L02c8:
	ld	%001C(r15),#%0010
	jr	L02de
L02d0:
	ld	%001C(r15),#%FFF6
	jr	L02de
L02d8:
	ld	%001C(r15),#%000A
L02de:
	cp	%0002(r15),#%0005
	jr	ge,L02f4
	lda	r7,%0002(r15)
	lda	r6,%000E(r15)
	ld	r5,%0004(r15)
	calr	__align
L02f4:
	ld	r2,%000E(r15)
	ldl	rr4,@r2
	ldl	%001E(r15),rr4
	inc	%000E(r15),#4
	inc	%0002(r15),#2
	cp	%0002(r15),#%0005
	jr	eq,L03d4
	jr	L03dc
L0310:
	cp	r13,#%0078
	jr	eq,L02c8
	cp	r13,#%0064
	jr	eq,L02d0
	cp	r13,#%0075
	jr	eq,L02d8
	ld	r3,%0000(r15)
	dec	%0002(r3),#1
	ld	r4,%0002(r3)
	cp	r4,#%0000
	jr	lt,L0348
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,#%6C
	ldb	rl3,#%6C
	extsb	r3
	jr	L0354
L0348:
	ld	r7,#%006C
	ld	r6,%0000(r15)
	call	__flsbuf
L0354:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L03a4
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	extsb	r2
L0376:
	test	%001A(r15)
	jr	eq,L038e
	cp	%007C(r15),#%0073
	jr	ne,L038e
	cp	r11,%001A(r15)
	jr	le,L038e
	ld	r11,%001A(r15)
L038e:
	cp	r11,%0018(r15)
	jp	ge,L07e2
	ld	r2,%0018(r15)
	sub	r2,r11
	ld	%0070(r15),r2
	jp	L07e6
L03a4:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L0376
L03b0:
	ld	%001C(r15),#%FFF6
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	r5,r3
	exts	rr4
	ldl	%001E(r15),rr4
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L03dc
L03d4:
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
L03dc:
	test	%0076(r15)
	jr	eq,L0410
	cp	%001C(r15),#%FFF6
	jr	ne,L0410
	ldl	rr2,%001E(r15)
	cpl	rr2,#%00000000
	jr	lt,L0410
	ldb	rl4,%006B(r15)
	ldb	%0024(r15),rl4
	lda	r5,%0024(r15)
	ld	%0078(r15),#%0001
	inc	r5,#1
	ld	%0064(r15),r5
	jr	L0476
L0410:
	cp	%001C(r15),#%0008
	jr	ne,L0466
	test	%0072(r15)
	jr	eq,L0466
	ldl	rr2,%001E(r15)
	testl	rr2
	jr	eq,L0466
	ldb	%0024(r15),#%30
	lda	r4,%0024(r15)
	ld	%0078(r15),#%0001
	inc	r4,#1
	jr	L0460
L043a:
	test	%0072(r15)
	jr	eq,L046e
	ldl	rr2,%001E(r15)
	testl	rr2
	jr	eq,L046e
	ldb	%0024(r15),#%30
	ldb	%0025(r15),#%78
	lda	r4,%0024(r15)
	ld	%0078(r15),#%0002
	inc	r4,#2
L0460:
	ld	%0064(r15),r4
	jr	L0476
L0466:
	cp	%001C(r15),#%0010
	jr	eq,L043a
L046e:
	lda	r2,%0024(r15)
	ld	%0064(r15),r2
L0476:
	cp	%001C(r15),#%FFF6
	jr	ne,L04d8
	ldl	rr2,%001E(r15)
	cpl	rr2,#%00000000
	jr	ge,L04d8
	cpl	rr2,#%80000000
	jr	ne,L0498
	ld	%001C(r15),#%000A
L0498:
	ldl	rr6,%001E(r15)
	multl	rq4,#%FFFFFFFF
	ldl	%001E(r15),rr6
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L04cc
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L04d8
L04cc:
	ld	r7,#%002D
	ld	r6,%0000(r15)
	call	__flsbuf
L04d8:
	lda	r2,%0024(r15)
	ld	%0022(r15),r2
	ld	r7,%0064(r15)
	ld	r6,%001C(r15)
	ldl	rr4,%001E(r15)
	call	__cnvt
	add	r2,%0078(r15)
L04f4:
	ld	r11,r2
	jp	L0376
L04fa:
	ldb	%007B(r15),#%20
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0022(r15),r3
	inc	%000E(r15),#2
	test	r3
	jr	ne,L0518
	ld	%0022(r15),#D0000
L0518:
	ld	r7,%0022(r15)
	call	__length
	ld	r11,r2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0376
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	jp	L0376
L053c:
	ldb	%007B(r15),#%20
	ldk	r11,#1
	ld	r2,%000E(r15)
	ld	%0022(r15),r2
	inc	%000E(r15),#2
	inc	%0022(r15),#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0376
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
	jp	L0376
L056e:
	cp	%0002(r15),#%0005
	jr	ge,L05b2
	cp	%000C(r15),#%0004
	jr	ge,L0598
	ld	r2,%000E(r15)
	ld	%0010(r15),r2
	ld	r3,%0006(r15)
	ld	%000E(r15),r3
	inc	%0006(r15),#8
	inc	%000C(r15),#1
	jr	L05b6
L0598:
	cp	%000C(r15),#%0004
	jr	ne,L05b6
	clr	%0010(r15)
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	inc	%0002(r15),#5
	jr	L05b6
L05b2:
	clr	%0010(r15)
L05b6:
	lda	r6,%0024(r15)
	ld	r7,%001A(r15)
	ld	r2,%000E(r15)
	.word	%0F24,%8303
	ld	r5,r13
	call	__fegcvt
	ld	%0022(r15),r2
	test	%0010(r15)
	jr	eq,L05e0
	ld	r2,%0010(r15)
	ld	%000E(r15),r2
	jr	L05e4
L05e0:
	inc	%000E(r15),#8
L05e4:
	test	%0076(r15)
	jr	eq,L0638
	ld	r2,%0022(r15)
	cpb	@r2,#%2D
	jr	eq,L0638
	testb	@r2
	jr	eq,L0638
	ld	r3,r2
	ld	%0064(r15),r2
	ldb	rl4,@r3
	ldb	%0067(r15),rl4
	ldb	rl5,%006B(r15)
	ldb	@r3,rl5
	inc	%0064(r15),#1
	jr	L062c
L0610:
	ld	r2,%0064(r15)
	ldb	rl3,@r2
	ldb	%0069(r15),rl3
	ldb	rl4,%0067(r15)
	ldb	@r2,rl4
	inc	%0064(r15),#1
	ldb	rl4,%0069(r15)
	ldb	%0067(r15),rl4
L062c:
	testb	%0067(r15)
	jr	ne,L0610
	ld	r2,%0064(r15)
	clrb	@r2
L0638:
	test	%0072(r15)
	jr	ne,L06e8
	cp	r13,#%0065
	jr	eq,L064a
	cp	r13,#%0045
	jr	ne,L0696
L064a:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L066a
L0654:
	ld	r2,%0064(r15)
	ld	r3,r2
	ldb	rl4,%0001(r2)
	ldb	@r3,rl4
	inc	%0064(r15),#1
	jr	L068c
L0666:
	inc	%0064(r15),#1
L066a:
	ld	r2,%0064(r15)
	testb	@r2
	jp	eq,L0782
	cpb	@r2,#%2E
	jr	ne,L0666
	inc	%0064(r15),#1
	ld	r2,%0064(r15)
	cpb	@r2,#%65
	jr	ne,L0666
	dec	%0064(r15),#1
L068c:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L0654
	jr	L0782
L0696:
	cp	r13,#%0067
	jr	eq,L06a2
	cp	r13,#%0047
	jr	ne,L0782
L06a2:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L06be
L06ac:
	ld	r7,%006C(r15)
	ld	r6,%0064(r15)
	call	_strcpy
	jr	L0782
L06ba:
	inc	%0064(r15),#1
L06be:
	ld	r2,%0064(r15)
	testb	@r2
	jr	eq,L0782
	cpb	@r2,#%2E
	jr	ne,L06ba
	ld	r3,r2
	ld	%006C(r15),r2
L06d2:
	inc	%0064(r15),#1
	ld	r2,%0064(r15)
	cpb	@r2,#%30
	jr	eq,L06d2
	cpb	@r2,#%65
	jr	eq,L06ac
	jr	L0782
L06e8:
	cp	r13,#%0066
	jr	ne,L0720
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L0706
L06f8:
	ld	r2,%0064(r15)
	cpb	@r2,#%2E
	jr	eq,L070e
	inc	%0064(r15),#1
L0706:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L06f8
L070e:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L0782
	ldb	@r2,#%2E
	inc	%0064(r15),#1
	jr	L077c
L0720:
	cp	r13,#%0067
	jr	eq,L072c
	cp	r13,#%0047
	jr	ne,L0782
L072c:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L0744
L0736:
	ld	r2,%0064(r15)
	cpb	@r2,#%2E
	jr	eq,L074c
	inc	%0064(r15),#1
L0744:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L0736
L074c:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L0782
	ldb	@r2,#%2E
	inc	%0064(r15),#1
	clr	%0078(r15)
	jr	L0772
L0762:
	ld	r3,%0064(r15)
	ldb	@r3,#%30
	inc	%0064(r15),#1
	inc	%0078(r15),#1
L0772:
	ld	r2,%0078(r15)
	cp	r2,%001A(r15)
	jr	lt,L0762
L077c:
	ld	r2,%0064(r15)
	clrb	@r2
L0782:
	cp	r13,#%0045
	jr	eq,L078e
	cp	r13,#%0047
	jr	ne,L07a8
L078e:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
L0796:
	ld	r2,%0064(r15)
	testb	@r2
	jr	eq,L07a8
	cpb	@r2,#%65
	jr	ne,L07b4
	ldb	@r2,#%45
L07a8:
	ld	r7,%0022(r15)
	call	__length
	jp	L04f4
L07b4:
	inc	%0064(r15),#1
	jr	L0796
L07ba:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jp	lt,L0090
L07ce:
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	extsb	r2
	jp	L009a
L07e2:
	clr	%0070(r15)
L07e6:
	test	%006E(r15)
	jr	eq,L0822
	jr	L0874
L07ee:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%007B(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L0822
L0802:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L07ee
	ldb	rl7,%007B(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
L0822:
	ld	r2,%0070(r15)
	dec	%0070(r15),#1
	test	r2
	jr	ne,L0802
	jr	L0874
L0830:
	ld	r2,%0022(r15)
	inc	%0022(r15),#1
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	rl3,@r2
	ldb	@r4,rl3
	extsb	r3
	jr	L0874
L084a:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0830
	ld	r7,%0022(r15)
	inc	%0022(r15),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,%0000(r15)
	call	__flsbuf
	ld	r3,r2
L0874:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L084a
	test	%006E(r15)
	jr	ne,L0898
	jp	L009a
L0886:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%007B(r15)
	ldb	@r3,rl2
	extsb	r2
L0898:
	ld	r2,%0070(r15)
	dec	%0070(r15),#1
	test	r2
	jp	eq,L009a
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0886
	ldb	rl7,%007B(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L0898
L08c8:
	ldm	r11,%007E(r15),#4
	add	r15,#%0086
	ret
__atoip::
	sub	r15,#%0016
	ldm	@r15,r5,#2
	ld	r1,r7
	clr	%000A(r15)
	ld	r4,r6
	test	r6
	jr	eq,L08fa
	cpb	@r1,#%2D
	jr	ne,L08fa
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%000A(r15),#%0001
L08fa:
	ldk	r5,#0
	jr	L092a
L08fe:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%000D(r15),rl3
	cpb	rl3,#%30
	jr	lt,L092e
	cpb	rl3,#%39
	jr	gt,L092e
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%000D(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L092a:
	test	r4
	jr	ne,L08fe
L092e:
	test	%000A(r15)
	jr	eq,L093a
	ld	r2,r5
	neg	r2
	jr	L093c
L093a:
	ld	r2,r5
L093c:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	add	r15,#%0016
	ret
	.psec
	.data
D0000:
	.byte	%28,%6e,%75,%6c,%6c,%29,%00,%00
D0008:
	.byte	%00,%45,%00,%47,%00,%65,%00,%66
	.byte	%00,%67,%00,%73
D0014:
	.byte	%00,%44,%00,%45,%00,%47,%00,%4f
	.byte	%00,%55,%00,%58,%00,%63,%00,%64
	.byte	%00,%65,%00,%66,%00,%67,%00,%6c
	.byte	%00,%6f,%00,%73,%00,%75,%00,%78
D0034:
	.addr	L010c
	.addr	L0158
	.addr	L0158
	.addr	L011a
	.addr	L0158
	.addr	L0158
	.addr	L0158
	.addr	L0158
	.addr	L0158
	.addr	L0158
	.addr	L0158
	.addr	L00fe
	.addr	L0158
	.addr	L0144
	.addr	L025c
	.addr	L025c
	.addr	L025c
	.addr	L025c
	.addr	L025c
D005a:
	.addr	L0238
	.addr	L02d0
	.addr	L056e
	.addr	L056e
	.addr	L02c0
	.addr	L02d8
	.addr	L02c8
	.addr	L053c
	.addr	L03b0
	.addr	L056e
	.addr	L056e
	.addr	L056e
	.addr	L02aa
	.addr	L0264
	.addr	L04fa
	.addr	L0274
D007a:
	.addr	L026c
