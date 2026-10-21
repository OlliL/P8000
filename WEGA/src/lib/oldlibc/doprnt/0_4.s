	.comm	2, __pfile
	.extern	__cnvt, __flsbuf, __inflt, __length, _main, _strcpy
	.psec
	.code
__align::
	dec	r15,#8
	ldm	@r15,r5,#3
	ld	r4,r7
	test	@r4
	jr	eq,L0012
	cp	@r7,#%0002
	jr	ne,L0020
L0012:
	ld	r2,%0004(r15)
	inc	@r2,#1
	ld	r2,%0002(r15)
	inc	@r2,#2
	jr	L0038
L0020:
	ld	r2,%0004(r15)
	cp	@r2,#%0004
	jr	ne,L0038
	inc	@r2,#2
	ld	r2,%0002(r15)
	ld	r3,%0000(r15)
	ld	@r2,r3
	jr	L005c
L0038:
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
L005c:
	inc	r15,#8
	ret
__doprnt::
	sub	r15,#%0084
	ldm	%007C(r15),r11,#4
	ldm	@r15,r2,#5
	ld	r14,r7
	clr	%000C(r15)
	clr	%0074(r15)
	ld	%000E(r15),r6
	call	__length
	ld	r12,r2
	jr	L00b6
L0084:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L00a2
L0096:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L00b2
L00a2:
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	subb	rh2,rh2
L00b2:
	inc	%0074(r15),#1
L00b6:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L08e6
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r13,r3
	cp	r3,#%0025
	jr	ne,L0084
	test	r12
	jp	eq,L08e6
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%0012(r15)
	ld	%0014(r15),#%FFFF
	ldb	%0077(r15),#%20
	clr	%0072(r15)
	clr	%006E(r15)
	clr	%006C(r15)
	clr	%0070(r15)
	clr	%0068(r15)
	jr	L014c
L0106:
	test	%006E(r15)
	jr	eq,L017a
	ldb	%0065(r15),#%20
	ld	%0070(r15),#%0001
	jr	L0180
L011a:
	test	%0070(r15)
	jr	ne,L0174
	ld	%0070(r15),#%0001
	jr	L016c
L0128:
	test	%006E(r15)
	jr	ne,L0174
	ld	%006E(r15),#%0001
	jr	L016c
L0136:
	test	%006C(r15)
	jr	ne,L0174
	ld	%006C(r15),#%0001
	jr	L016c
L0144:
	ldk	r3,#0
	ldb	rl3,@r14
	ld	r13,r3
	inc	r14,#1
L014c:
	ld	r2,r13
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L0174
	rl	r2,#1
	ld	r2,D0034(r2)
	jp	@r2
L0160:
	test	%0068(r15)
	jr	ne,L0174
	ld	%0068(r15),#%0001
L016c:
	ld	r3,r12
	dec	r12,#1
	test	r3
	jr	ne,L0144
L0174:
	test	%0070(r15)
	jr	eq,L0106
L017a:
	ldb	%0065(r15),#%2B
L0180:
	cp	r13,#%0061
	jr	lt,L022e
	jr	L0246
L0188:
	cp	r13,#_main
	jr	ne,L01b2
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0012(r15),r3
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L01d2
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L01d2
L01b2:
	cp	r13,#%0030
	jr	ne,L01be
	ldb	%0077(r15),#%30
L01be:
	lda	r5,%0012(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L01d2:
	test	r12
	jr	eq,L0246
	cpb	@r14,#%2E
	jr	ne,L0220
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,L0246
	cpb	@r14,#%2A
	jr	ne,L0210
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%0014(r15),r3
	inc	%000E(r15),#2
	ldk	r11,#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L021c
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jr	L021c
L0210:
	lda	r5,%0014(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L021c:
	sub	r12,r11
	add	r14,r11
L0220:
	test	r12
	jr	eq,L0246
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L022e:
	cp	r13,#_main
	jr	eq,L0188
	cp	r13,#%0030
	jr	lt,L0240
	cp	r13,#%0039
	jr	le,L0188
L0240:
	cp	r13,#%002E
	jr	eq,L0188
L0246:
	cp	%0014(r15),#%0000
	jr	ge,L026c
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%0006
	cpir	r2,@r3,r4,eq
	jr	ne,L026c
	add	r4,r4
	neg	r4
	ld	r2,D005a(r4)
	jp	@r2
L0268:
	clr	%0014(r15)
L026c:
	ld	%0078(r15),r13
	ld	r2,r13
	ld	r3,#D0014
	ld	r4,#%0010
	cpir	r2,@r3,r4,eq
	jp	ne,L07de
	add	r4,r4
	neg	r4
	ld	r2,D007a(r4)
	jp	@r2
L028c:
	ld	%0014(r15),#%0006
	jr	L026c
L0294:
	ld	%0016(r15),#%0008
	jr	L02aa
L029c:
	ld	%0016(r15),#%0010
	jr	L02aa
L02a4:
	ld	%0016(r15),#%000A
L02aa:
	ld	r3,%000E(r15)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	%0018(r15),rr6
	inc	%000E(r15),#2
	clr	%0018(r15)
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0410
	ld	r4,%0004(r15)
	ld	%000E(r15),r4
	jp	L0410
L02da:
	test	r12
	jr	eq,L03b6
	dec	r12,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r3
	subb	rh4,rh4
	ld	r13,r4
	cp	r4,#%006F
	jr	ne,L0340
L02f0:
	ld	%0016(r15),#%0008
	jr	L030e
L02f8:
	ld	%0016(r15),#%0010
	jr	L030e
L0300:
	ld	%0016(r15),#%FFF6
	jr	L030e
L0308:
	ld	%0016(r15),#%000A
L030e:
	cp	%0002(r15),#%0005
	jr	ge,L0324
	lda	r7,%0002(r15)
	lda	r6,%000E(r15)
	ld	r5,%0004(r15)
	calr	__align
L0324:
	ld	r2,%000E(r15)
	ldl	rr4,@r2
	ldl	%0018(r15),rr4
	inc	%000E(r15),#4
	inc	%0002(r15),#2
	cp	%0002(r15),#%0005
	jr	eq,L0408
	jr	L0410
L0340:
	cp	r13,#%0078
	jr	eq,L02f8
	cp	r13,#%0064
	jr	eq,L0300
	cp	r13,#%0075
	jr	eq,L0308
	ld	r3,%0000(r15)
	dec	%0002(r3),#1
	ld	r4,%0002(r3)
	cp	r4,#%0000
	jr	ge,L0372
	ld	r7,#%006C
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L0384
L0372:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#%6C
	ldb	rl2,#%6C
	subb	rh2,rh2
L0384:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L03a2
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L03b2
L03a2:
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	subb	rh2,rh2
L03b2:
	inc	%0074(r15),#2
L03b6:
	test	%0014(r15)
	jr	eq,L03ce
	cp	%0078(r15),#%0073
	jr	ne,L03ce
	cp	r11,%0014(r15)
	jr	le,L03ce
	ld	r11,%0014(r15)
L03ce:
	cp	r11,%0012(r15)
	jp	ge,L07f6
	ld	r2,%0012(r15)
	sub	r2,r11
	ld	%006A(r15),r2
	jp	L07fa
L03e4:
	ld	%0016(r15),#%FFF6
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	r5,r3
	exts	rr4
	ldl	%0018(r15),rr4
	inc	%000E(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0410
L0408:
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
L0410:
	test	%0070(r15)
	jr	eq,L0444
	cp	%0016(r15),#%FFF6
	jr	ne,L0444
	ldl	rr2,%0018(r15)
	cpl	rr2,#%00000000
	jr	lt,L0444
	ldb	rl2,%0065(r15)
	ldb	%001E(r15),rl2
	lda	r3,%001E(r15)
	ld	%0072(r15),#%0001
	inc	r3,#1
	ld	%005E(r15),r3
	jr	L04a6
L0444:
	cp	%0016(r15),#%0008
	jr	ne,L046e
	test	%006C(r15)
	jr	eq,L046e
	ldl	rr2,%0018(r15)
	testl	rr2
	jr	eq,L046e
	ldb	%001E(r15),#%30
	lda	r2,%001E(r15)
	ld	%0072(r15),#%0001
	inc	r2,#1
	jr	L04a2
L046e:
	cp	%0016(r15),#%0010
	jr	ne,L049e
	test	%006C(r15)
	jr	eq,L049e
	ldl	rr2,%0018(r15)
	testl	rr2
	jr	eq,L049e
	ldb	%001E(r15),#%30
	ldb	%001F(r15),#%78
	lda	r2,%001E(r15)
	ld	%0072(r15),#%0002
	inc	r2,#2
	jr	L04a2
L049e:
	lda	r2,%001E(r15)
L04a2:
	ld	%005E(r15),r2
L04a6:
	cp	%0016(r15),#%FFF6
	jr	ne,L050c
	ldl	rr2,%0018(r15)
	cpl	rr2,#%00000000
	jr	ge,L050c
	cpl	rr2,#%80000000
	jr	ne,L04c8
	ld	%0016(r15),#%000A
L04c8:
	ldl	rr6,%0018(r15)
	multl	rq4,#%FFFFFFFF
	ldl	%0018(r15),rr6
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L04f6
	ld	r7,#%002D
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L0508
L04f6:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#%2D
	ldb	rl2,#%2D
	subb	rh2,rh2
L0508:
	inc	%0074(r15),#1
L050c:
	lda	r2,%001E(r15)
	ld	%001C(r15),r2
	ld	r7,%005E(r15)
	ld	r6,%0016(r15)
	ldl	rr4,%0018(r15)
	call	__cnvt
	add	r2,%0072(r15)
L0528:
	ld	r11,r2
	jp	L03b6
L052e:
	ldb	%0077(r15),#%20
	ld	r2,%000E(r15)
	ld	r3,@r2
	ld	%001C(r15),r3
	inc	%000E(r15),#2
	test	r3
	jr	ne,L054c
	ld	%001C(r15),#D0000
L054c:
	ld	r7,%001C(r15)
	call	__length
	ld	r11,r2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L03b6
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	jp	L03b6
L0570:
	ldb	%0077(r15),#%20
	ldk	r11,#1
	ld	r2,%000E(r15)
	ld	%001C(r15),r2
	inc	%000E(r15),#2
	inc	%001C(r15),#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L03b6
	ld	r3,%0004(r15)
	ld	%000E(r15),r3
	jp	L03b6
L05a2:
	cp	%000C(r15),#%0004
	jr	ge,L05c4
	ld	r2,%000E(r15)
	ld	%0010(r15),r2
	ld	r3,%0006(r15)
	ld	%000E(r15),r3
	inc	%0006(r15),#8
	inc	%000C(r15),#1
	jr	L05dc
L05c4:
	cp	%000C(r15),#%0004
	jr	ne,L05dc
	clr	%0010(r15)
	ld	r2,%0004(r15)
	ld	%000E(r15),r2
	inc	%0002(r15),#5
L05dc:
	lda	r6,%001E(r15)
	ld	r7,%0014(r15)
	ld	r2,%000E(r15)
	.word	%0F24,%8303
	ld	r5,r13
	call	__inflt
	ld	%001C(r15),r2
	test	%0010(r15)
	jr	eq,L0606
	ld	r2,%0010(r15)
	ld	%000E(r15),r2
	jr	L060a
L0606:
	inc	%000E(r15),#8
L060a:
	test	%0070(r15)
	jr	eq,L065c
	ld	r2,%001C(r15)
	cpb	@r2,#%2D
	jr	eq,L065c
	testb	@r2
	jr	eq,L065c
	ld	%005E(r15),r2
	ldb	rl3,@r2
	ldb	%0061(r15),rl3
	ldb	rl4,%0065(r15)
	ldb	@r2,rl4
	inc	%005E(r15),#1
	jr	L0650
L0634:
	ld	r2,%005E(r15)
	ldb	rl3,@r2
	ldb	%0063(r15),rl3
	ldb	rl4,%0061(r15)
	ldb	@r2,rl4
	inc	%005E(r15),#1
	ldb	rl4,%0063(r15)
	ldb	%0061(r15),rl4
L0650:
	testb	%0061(r15)
	jr	ne,L0634
	ld	r2,%005E(r15)
	clrb	@r2
L065c:
	test	%006C(r15)
	jr	ne,L070c
	cp	r13,#%0065
	jr	eq,L066e
	cp	r13,#%0045
	jr	ne,L06ba
L066e:
	ld	r2,%001C(r15)
	ld	%005E(r15),r2
	jr	L068e
L0678:
	ld	r2,%005E(r15)
	ld	r3,r2
	ldb	rl4,%0001(r2)
	ldb	@r3,rl4
	inc	%005E(r15),#1
	jr	L06b0
L068a:
	inc	%005E(r15),#1
L068e:
	ld	r2,%005E(r15)
	testb	@r2
	jp	eq,L07a6
	cpb	@r2,#%2E
	jr	ne,L068a
	inc	%005E(r15),#1
	ld	r2,%005E(r15)
	cpb	@r2,#%65
	jr	ne,L068a
	dec	%005E(r15),#1
L06b0:
	ld	r2,%005E(r15)
	testb	@r2
	jr	ne,L0678
	jr	L07a6
L06ba:
	cp	r13,#%0067
	jr	eq,L06c6
	cp	r13,#%0047
	jr	ne,L07a6
L06c6:
	ld	r2,%001C(r15)
	ld	%005E(r15),r2
	jr	L06e2
L06d0:
	ld	r7,%0066(r15)
	ld	r6,%005E(r15)
	call	_strcpy
	jr	L07a6
L06de:
	inc	%005E(r15),#1
L06e2:
	ld	r2,%005E(r15)
	testb	@r2
	jr	eq,L07a6
	cpb	@r2,#%2E
	jr	ne,L06de
	ld	r3,r2
	ld	%0066(r15),r2
L06f6:
	inc	%005E(r15),#1
	ld	r2,%005E(r15)
	cpb	@r2,#%30
	jr	eq,L06f6
	cpb	@r2,#%65
	jr	eq,L06d0
	jr	L07a6
L070c:
	cp	r13,#%0066
	jr	ne,L0744
	ld	r2,%001C(r15)
	ld	%005E(r15),r2
	jr	L072a
L071c:
	ld	r2,%005E(r15)
	cpb	@r2,#%2E
	jr	eq,L0732
	inc	%005E(r15),#1
L072a:
	ld	r2,%005E(r15)
	testb	@r2
	jr	ne,L071c
L0732:
	ld	r2,%005E(r15)
	testb	@r2
	jr	ne,L07a6
	ldb	@r2,#%2E
	inc	%005E(r15),#1
	jr	L07a0
L0744:
	cp	r13,#%0067
	jr	eq,L0750
	cp	r13,#%0047
	jr	ne,L07a6
L0750:
	ld	r2,%001C(r15)
	ld	%005E(r15),r2
	jr	L0768
L075a:
	ld	r2,%005E(r15)
	cpb	@r2,#%2E
	jr	eq,L0770
	inc	%005E(r15),#1
L0768:
	ld	r2,%005E(r15)
	testb	@r2
	jr	ne,L075a
L0770:
	ld	r2,%005E(r15)
	testb	@r2
	jr	ne,L07a6
	ldb	@r2,#%2E
	inc	%005E(r15),#1
	clr	%0072(r15)
	jr	L0796
L0786:
	ld	r3,%005E(r15)
	ldb	@r3,#%30
	inc	%005E(r15),#1
	inc	%0072(r15),#1
L0796:
	ld	r2,%0072(r15)
	cp	r2,%0014(r15)
	jr	lt,L0786
L07a0:
	ld	r2,%005E(r15)
	clrb	@r2
L07a6:
	cp	r13,#%0045
	jr	eq,L07b2
	cp	r13,#%0047
	jr	ne,L07cc
L07b2:
	ld	r2,%001C(r15)
	ld	%005E(r15),r2
L07ba:
	ld	r2,%005E(r15)
	testb	@r2
	jr	eq,L07cc
	cpb	@r2,#%65
	jr	ne,L07d8
	ldb	@r2,#%45
L07cc:
	ld	r7,%001C(r15)
	call	__length
	jp	L0528
L07d8:
	inc	%005E(r15),#1
	jr	L07ba
L07de:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jp	ge,L00a2
	jp	L0096
L07f6:
	clr	%006A(r15)
L07fa:
	test	%0068(r15)
	jr	eq,L083a
	jr	L088e
L0802:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0824
	ldb	rl7,%0077(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L0836
L0824:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%0077(r15)
	ldb	@r3,rl2
	subb	rh2,rh2
L0836:
	inc	%0074(r15),#1
L083a:
	ld	r2,%006A(r15)
	dec	%006A(r15),#1
	test	r2
	jr	ne,L0802
	jr	L088e
L0848:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0872
	ld	r7,%001C(r15)
	inc	%001C(r15),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L088a
L0872:
	ld	r2,%001C(r15)
	inc	%001C(r15),#1
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	rl3,@r2
	ldb	@r4,rl3
	subb	rh3,rh3
L088a:
	inc	%0074(r15),#1
L088e:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0848
	test	%0068(r15)
	jp	eq,L00b6
L089e:
	ld	r2,%006A(r15)
	dec	%006A(r15),#1
	test	r2
	jp	eq,L00b6
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L08ce
	ldb	rl7,%0077(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
	jr	L08e0
L08ce:
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%0077(r15)
	ldb	@r3,rl2
	subb	rh2,rh2
L08e0:
	inc	%0074(r15),#1
	jr	L089e
L08e6:
	test	__pfile
	jr	eq,L0904
	ld	r2,__pfile
	ldb	rl3,%0006(r2)
	extsb	r3
	and	r3,#%0020
	test	r3
	jr	eq,L0904
	ld	r2,#%FFFF
	jr	L0908
L0904:
	ld	r2,%0074(r15)
L0908:
	ldm	r11,%007C(r15),#4
	add	r15,#%0084
	ret
__atoip::
	sub	r15,#%0012
	ldm	@r15,r5,#2
	ld	r1,r7
	clr	%0006(r15)
	ld	r4,r6
	test	r6
	jr	eq,L093a
	cpb	@r1,#%2D
	jr	ne,L093a
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0006(r15),#%0001
L093a:
	ldk	r5,#0
	jr	L096a
L093e:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0009(r15),rl3
	cpb	rl3,#%30
	jr	lt,L096e
	cpb	rl3,#%39
	jr	gt,L096e
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0009(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L096a:
	test	r4
	jr	ne,L093e
L096e:
	test	%0006(r15)
	jr	eq,L097a
	ld	r2,r5
	neg	r2
	jr	L097c
L097a:
	ld	r2,r5
L097c:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	add	r15,#%0012
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
	.addr	L0128
	.addr	L0174
	.addr	L0174
	.addr	L0136
	.addr	L0174
	.addr	L0174
	.addr	L0174
	.addr	L0174
	.addr	L0174
	.addr	L0174
	.addr	L0174
	.addr	L011a
	.addr	L0174
	.addr	L0160
	.addr	L028c
	.addr	L028c
	.addr	L028c
	.addr	L028c
	.addr	L028c
D005a:
	.addr	L0268
	.addr	L0300
	.addr	L05a2
	.addr	L05a2
	.addr	L02f0
	.addr	L0308
	.addr	L02f8
	.addr	L0570
	.addr	L03e4
	.addr	L05a2
	.addr	L05a2
	.addr	L05a2
	.addr	L02da
	.addr	L0294
	.addr	L052e
	.addr	L02a4
D007a:
	.addr	L029c
