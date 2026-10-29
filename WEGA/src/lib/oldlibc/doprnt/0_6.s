	.extern	__cnvt, __fegcvt, __flsbuf, __length, __pfile, _strcpy
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
	sub	r15,#%008A
	ldm	%0082(r15),r11,#4
	ldm	@r15,r2,#5
	ld	r14,r7
	clr	%000C(r15)
	clr	%007A(r15)
	ld	%000E(r15),r6
	call	__length
	ld	r12,r2
	jr	L00b2
L0080:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L00a4
L0092:
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	extsb	r2
	jr	L00ae
L00a4:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
L00ae:
	inc	%007A(r15),#1
L00b2:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L08f8
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r13,r3
	cp	r3,#%0025
	jr	ne,L0080
	test	r12
	jp	eq,L08f8
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%0018(r15)
	ld	%001A(r15),#%FFFF
	ldb	%007D(r15),#%20
	clr	%0078(r15)
	clr	%0074(r15)
	clr	%0072(r15)
	clr	%0076(r15)
	clr	%006E(r15)
	jr	L0148
L0102:
	test	%0074(r15)
	jr	eq,L0176
	ldb	%006B(r15),#%20
	ld	%0076(r15),#%0001
	jr	L017c
L0116:
	test	%0076(r15)
	jr	ne,L0170
	ld	%0076(r15),#%0001
	jr	L0168
L0124:
	test	%0074(r15)
	jr	ne,L0170
	ld	%0074(r15),#%0001
	jr	L0168
L0132:
	test	%0072(r15)
	jr	ne,L0170
	ld	%0072(r15),#%0001
	jr	L0168
L0140:
	ldk	r3,#0
	ldb	rl3,@r14
	ld	r13,r3
	inc	r14,#1
L0148:
	ld	r2,r13
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L0170
	rl	r2,#1
	ld	r2,D0034(r2)
	jp	@r2
L015c:
	test	%006E(r15)
	jr	ne,L0170
	ld	%006E(r15),#%0001
L0168:
	ld	r3,r12
	dec	r12,#1
	test	r3
	jr	ne,L0140
L0170:
	test	%0076(r15)
	jr	eq,L0102
L0176:
	ldb	%006B(r15),#%2B
L017c:
	cp	r13,#%0061
	jr	lt,L022a
	jr	L0242
L0184:
	cp	r13,#%002A
	jr	ne,L01ae
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0018(r15),r3
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L01ce
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L01ce
L01ae:
	cp	r13,#%0030
	jr	ne,L01ba
	ldb	%007D(r15),#%30
L01ba:
	lda	r5,%0018(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L01ce:
	test	r12
	jr	eq,L0242
	cpb	@r14,#%2E
	jr	ne,L021c
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,L0242
	cpb	@r14,#%2A
	jr	ne,L020c
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%001A(r15),r3
	inc	%000E(r15),#2
	ldk	r11,#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0218
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L0218
L020c:
	lda	r5,%001A(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L0218:
	sub	r12,r11
	add	r14,r11
L021c:
	test	r12
	jr	eq,L0242
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L022a:
	cp	r13,#%002A
	jr	eq,L0184
	cp	r13,#%0030
	jr	lt,L023c
	cp	r13,#%0039
	jr	le,L0184
L023c:
	cp	r13,#%002E
	jr	eq,L0184
L0242:
	cp	%001A(r15),#%0000
	jr	ge,L0268
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0006
	cpir	r2,@r3,r4,eq
	jr	ne,L0268
	add	r4,r4
	neg	r4
	ld	r2,D005a(r4)
	jp	@r2
L0264:
	clr	%001A(r15)
L0268:
	ld	%007E(r15),r13
	ld	r2,r13
	ld	r3,#D0014
	ld	r4,#%0010
	cpir	r2,@r3,r4,eq
	jp	ne,L07ee
	add	r4,r4
	neg	r4
	ld	r2,D007a(r4)
	jp	@r2
L0288:
	ld	%001A(r15),#%0006
	jr	L0268
L0290:
	ld	%001C(r15),#%0008
	jr	L02a6
L0298:
	ld	%001C(r15),#%0010
	jr	L02a6
L02a0:
	ld	%001C(r15),#%000A
L02a6:
	ld	r3,%000E(r15)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	%001E(r15),rr6
	inc	%000E(r15),#2
	clr	%001E(r15)
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L040c
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jp	L040c
L02d6:
	test	r12
	jr	eq,L03b2
	dec	r12,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r3
	subb	rh4,rh4
	ld	r13,r4
	cp	r4,#%006F
	jr	ne,L033c
L02ec:
	ld	%001C(r15),#%0008
	jr	L030a
L02f4:
	ld	%001C(r15),#%0010
	jr	L030a
L02fc:
	ld	%001C(r15),#%FFF6
	jr	L030a
L0304:
	ld	%001C(r15),#%000A
L030a:
	cp	%0002(r15),#%0005
	jr	ge,L0320
	lda	r7,%0002(r15)
	lda	r6,%000E(r15)
	ld	r5,%0004(r15)
	calr	__align
L0320:
	ld	r2,%000E(r15)
	ldl	rr4,@r2
	ldl	%001E(r15),rr4
	inc	%000E(r15),#4
	inc	%0002(r15),#2
	cp	%0002(r15),#%0005
	jr	eq,L0404
	jr	L040c
L033c:
	cp	r13,#%0078
	jr	eq,L02f4
	cp	r13,#%0064
	jr	eq,L02fc
	cp	r13,#%0075
	jr	eq,L0304
	ld	r3,%0000(r15)
	dec	%0002(r3),#1
	ld	r4,%0002(r3)
	cp	r4,#%0000
	jr	lt,L0374
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,#%6C
	ldb	rl3,#%6C
	extsb	r3
	jr	L0380
L0374:
	ld	r7,#%006C
	ld	r6,%0000(r15)
	call	__flsbuf
L0380:
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
	jr	L03ae
L03a4:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
L03ae:
	inc	%007A(r15),#2
L03b2:
	test	%001A(r15)
	jr	eq,L03ca
	cp	%007E(r15),#%0073
	jr	ne,L03ca
	cp	r11,%001A(r15)
	jr	le,L03ca
	ld	r11,%001A(r15)
L03ca:
	cp	r11,%0018(r15)
	jp	ge,L0806
	ld	r2,%0018(r15)
	sub	r2,r11
	ld	%0070(r15),r2
	jp	L080a
L03e0:
	ld	%001C(r15),#%FFF6
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	r5,r3
	exts	rr4
	ldl	%001E(r15),rr4
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L040c
L0404:
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
L040c:
	test	%0076(r15)
	jr	eq,L0440
	cp	%001C(r15),#%FFF6
	jr	ne,L0440
	ldl	rr2,%001E(r15)
	cpl	rr2,#%00000000
	jr	lt,L0440
	ldb	rl4,%006B(r15)
	ldb	%0024(r15),rl4
	lda	r5,%0024(r15)
	ld	%0078(r15),#%0001
	inc	r5,#1
	ld	%0064(r15),r5
	jr	L04a6
L0440:
	cp	%001C(r15),#%0008
	jr	ne,L0496
	test	%0072(r15)
	jr	eq,L0496
	ldl	rr2,%001E(r15)
	testl	rr2
	jr	eq,L0496
	ldb	%0024(r15),#%30
	lda	r4,%0024(r15)
	ld	%0078(r15),#%0001
	inc	r4,#1
	jr	L0490
L046a:
	test	%0072(r15)
	jr	eq,L049e
	ldl	rr2,%001E(r15)
	testl	rr2
	jr	eq,L049e
	ldb	%0024(r15),#%30
	ldb	%0025(r15),#%78
	lda	r4,%0024(r15)
	ld	%0078(r15),#%0002
	inc	r4,#2
L0490:
	ld	%0064(r15),r4
	jr	L04a6
L0496:
	cp	%001C(r15),#%0010
	jr	eq,L046a
L049e:
	lda	r2,%0024(r15)
	ld	%0064(r15),r2
L04a6:
	cp	%001C(r15),#%FFF6
	jr	ne,L050c
	ldl	rr2,%001E(r15)
	cpl	rr2,#%00000000
	jr	ge,L050c
	cpl	rr2,#%80000000
	jr	ne,L04c8
	ld	%001C(r15),#%000A
L04c8:
	ldl	rr6,%001E(r15)
	multl	rq4,#%FFFFFFFF
	ldl	%001E(r15),rr6
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L04fc
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L0508
L04fc:
	ld	r7,#%002D
	ld	r6,%0000(r15)
	call	__flsbuf
L0508:
	inc	%007A(r15),#1
L050c:
	lda	r2,%0024(r15)
	ld	%0022(r15),r2
	ld	r7,%0064(r15)
	ld	r6,%001C(r15)
	ldl	rr4,%001E(r15)
	call	__cnvt
	add	r2,%0078(r15)
L0528:
	ld	r11,r2
	jp	L03b2
L052e:
	ldb	%007D(r15),#%20
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0022(r15),r3
	inc	%000E(r15),#2
	test	r3
	jr	ne,L054c
	ld	%0022(r15),#D0000
L054c:
	ld	r7,%0022(r15)
	call	__length
	ld	r11,r2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L03b2
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	jp	L03b2
L0570:
	ldb	%007D(r15),#%20
	ldk	r11,#1
	ld	r2,%000E(r15)
	ld	%0022(r15),r2
	inc	%000E(r15),#2
	inc	%0022(r15),#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L03b2
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
	jp	L03b2
L05a2:
	cp	%0002(r15),#%0005
	jr	ge,L05e6
	cp	%000C(r15),#%0004
	jr	ge,L05cc
	ld	r2,%000E(r15)
	ld	%0010(r15),r2
	ld	r3,%0006(r15)
	ld	%000E(r15),r3
	inc	%0006(r15),#8
	inc	%000C(r15),#1
	jr	L05ea
L05cc:
	cp	%000C(r15),#%0004
	jr	ne,L05ea
	clr	%0010(r15)
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	inc	%0002(r15),#5
	jr	L05ea
L05e6:
	clr	%0010(r15)
L05ea:
	lda	r6,%0024(r15)
	ld	r7,%001A(r15)
	ld	r2,%000E(r15)
	.word	%0F24,%8303
	ld	r5,r13
	call	__fegcvt
	ld	%0022(r15),r2
	test	%0010(r15)
	jr	eq,L0614
	ld	r2,%0010(r15)
	ld	%000E(r15),r2
	jr	L0618
L0614:
	inc	%000E(r15),#8
L0618:
	test	%0076(r15)
	jr	eq,L066c
	ld	r2,%0022(r15)
	cpb	@r2,#%2D
	jr	eq,L066c
	testb	@r2
	jr	eq,L066c
	ld	r3,r2
	ld	%0064(r15),r2
	ldb	rl4,@r3
	ldb	%0067(r15),rl4
	ldb	rl5,%006B(r15)
	ldb	@r3,rl5
	inc	%0064(r15),#1
	jr	L0660
L0644:
	ld	r2,%0064(r15)
	ldb	rl3,@r2
	ldb	%0069(r15),rl3
	ldb	rl4,%0067(r15)
	ldb	@r2,rl4
	inc	%0064(r15),#1
	ldb	rl4,%0069(r15)
	ldb	%0067(r15),rl4
L0660:
	testb	%0067(r15)
	jr	ne,L0644
	ld	r2,%0064(r15)
	clrb	@r2
L066c:
	test	%0072(r15)
	jr	ne,L071c
	cp	r13,#%0065
	jr	eq,L067e
	cp	r13,#%0045
	jr	ne,L06ca
L067e:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L069e
L0688:
	ld	r2,%0064(r15)
	ld	r3,r2
	ldb	rl4,%0001(r2)
	ldb	@r3,rl4
	inc	%0064(r15),#1
	jr	L06c0
L069a:
	inc	%0064(r15),#1
L069e:
	ld	r2,%0064(r15)
	testb	@r2
	jp	eq,L07b6
	cpb	@r2,#%2E
	jr	ne,L069a
	inc	%0064(r15),#1
	ld	r2,%0064(r15)
	cpb	@r2,#%65
	jr	ne,L069a
	dec	%0064(r15),#1
L06c0:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L0688
	jr	L07b6
L06ca:
	cp	r13,#%0067
	jr	eq,L06d6
	cp	r13,#%0047
	jr	ne,L07b6
L06d6:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L06f2
L06e0:
	ld	r7,%006C(r15)
	ld	r6,%0064(r15)
	call	_strcpy
	jr	L07b6
L06ee:
	inc	%0064(r15),#1
L06f2:
	ld	r2,%0064(r15)
	testb	@r2
	jr	eq,L07b6
	cpb	@r2,#%2E
	jr	ne,L06ee
	ld	r3,r2
	ld	%006C(r15),r2
L0706:
	inc	%0064(r15),#1
	ld	r2,%0064(r15)
	cpb	@r2,#%30
	jr	eq,L0706
	cpb	@r2,#%65
	jr	eq,L06e0
	jr	L07b6
L071c:
	cp	r13,#%0066
	jr	ne,L0754
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L073a
L072c:
	ld	r2,%0064(r15)
	cpb	@r2,#%2E
	jr	eq,L0742
	inc	%0064(r15),#1
L073a:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L072c
L0742:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L07b6
	ldb	@r2,#%2E
	inc	%0064(r15),#1
	jr	L07b0
L0754:
	cp	r13,#%0067
	jr	eq,L0760
	cp	r13,#%0047
	jr	ne,L07b6
L0760:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
	jr	L0778
L076a:
	ld	r2,%0064(r15)
	cpb	@r2,#%2E
	jr	eq,L0780
	inc	%0064(r15),#1
L0778:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L076a
L0780:
	ld	r2,%0064(r15)
	testb	@r2
	jr	ne,L07b6
	ldb	@r2,#%2E
	inc	%0064(r15),#1
	clr	%0078(r15)
	jr	L07a6
L0796:
	ld	r3,%0064(r15)
	ldb	@r3,#%30
	inc	%0064(r15),#1
	inc	%0078(r15),#1
L07a6:
	ld	r2,%0078(r15)
	cp	r2,%001A(r15)
	jr	lt,L0796
L07b0:
	ld	r2,%0064(r15)
	clrb	@r2
L07b6:
	cp	r13,#%0045
	jr	eq,L07c2
	cp	r13,#%0047
	jr	ne,L07dc
L07c2:
	ld	r2,%0022(r15)
	ld	%0064(r15),r2
L07ca:
	ld	r2,%0064(r15)
	testb	@r2
	jr	eq,L07dc
	cpb	@r2,#%65
	jr	ne,L07e8
	ldb	@r2,#%45
L07dc:
	ld	r7,%0022(r15)
	call	__length
	jp	L0528
L07e8:
	inc	%0064(r15),#1
	jr	L07ca
L07ee:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jp	lt,L00a4
	jp	L0092
L0806:
	clr	%0070(r15)
L080a:
	test	%006E(r15)
	jr	eq,L084a
	jr	L08a0
L0812:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0838
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%007D(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L0846
L0838:
	ldb	rl7,%007D(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
L0846:
	inc	%007A(r15),#1
L084a:
	ld	r2,%0070(r15)
	dec	%0070(r15),#1
	test	r2
	jr	ne,L0812
	jr	L08a0
L0858:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0884
	ld	r2,%0022(r15)
	inc	%0022(r15),#1
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	rl3,@r2
	ldb	@r4,rl3
	extsb	r3
	jr	L089c
L0884:
	ld	r7,%0022(r15)
	inc	%0022(r15),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,%0000(r15)
	call	__flsbuf
	ld	r3,r2
L089c:
	inc	%007A(r15),#1
L08a0:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0858
	test	%006E(r15)
	jp	eq,L00b2
L08b0:
	ld	r2,%0070(r15)
	dec	%0070(r15),#1
	test	r2
	jp	eq,L00b2
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L08e4
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%007D(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L08f2
L08e4:
	ldb	rl7,%007D(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
L08f2:
	inc	%007A(r15),#1
	jr	L08b0
L08f8:
	test	__pfile
	jr	eq,L0916
	ld	r2,__pfile
	ldb	rl3,%0006(r2)
	extsb	r3
	and	r3,#%0020
	test	r3
	jr	eq,L0916
	ld	r2,#%FFFF
	jr	L091a
L0916:
	ld	r2,%007A(r15)
L091a:
	ldm	r11,%0082(r15),#4
	add	r15,#%008A
	ret
__atoip::
	sub	r15,#%0016
	ldm	@r15,r5,#2
	ld	r1,r7
	clr	%000A(r15)
	ld	r4,r6
	test	r6
	jr	eq,L094c
	cpb	@r1,#%2D
	jr	ne,L094c
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%000A(r15),#%0001
L094c:
	ldk	r5,#0
	jr	L097c
L0950:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%000D(r15),rl3
	cpb	rl3,#%30
	jr	lt,L0980
	cpb	rl3,#%39
	jr	gt,L0980
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%000D(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L097c:
	test	r4
	jr	ne,L0950
L0980:
	test	%000A(r15)
	jr	eq,L098c
	ld	r2,r5
	neg	r2
	jr	L098e
L098c:
	ld	r2,r5
L098e:
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
	.addr	L0124
	.addr	L0170
	.addr	L0170
	.addr	L0132
	.addr	L0170
	.addr	L0170
	.addr	L0170
	.addr	L0170
	.addr	L0170
	.addr	L0170
	.addr	L0170
	.addr	L0116
	.addr	L0170
	.addr	L015c
	.addr	L0288
	.addr	L0288
	.addr	L0288
	.addr	L0288
	.addr	L0288
D005a:
	.addr	L0264
	.addr	L02fc
	.addr	L05a2
	.addr	L05a2
	.addr	L02ec
	.addr	L0304
	.addr	L02f4
	.addr	L0570
	.addr	L03e0
	.addr	L05a2
	.addr	L05a2
	.addr	L05a2
	.addr	L02d6
	.addr	L0290
	.addr	L052e
	.addr	L02a0
D007a:
	.addr	L0298
