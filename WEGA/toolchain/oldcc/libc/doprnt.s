// doprnt.o: magic E711 nonsegmented
	.extern	__align
	.comm	2, __pfile
	.extern	__doprnt
	.extern	__length
	.extern	__flsbuf
	.extern	__cnvt
	.extern	__stfl
	.extern	__fegcvt
	.extern	_strcpy
	.extern	__fltuse
	.extern	__atoip

// segment table entry 0 (segment 0), text, 2476 bytes
	.psec
	.code
__align::
	dec	r15,#8
	ldm	@r15,r5,#3
	ld	r4,r7
	test	@r4
	jr	eq,L0t_0012
	cp	@r7,#%0002
	jr	ne,L0t_0020
L0t_0012:
	ld	r2,%0004(r15)
	inc	@r2,#1
	ld	r2,%0002(r15)
	inc	@r2,#2
	jr	L0t_0038
L0t_0020:
	ld	r2,%0004(r15)
	cp	@r2,#%0004
	jr	ne,L0t_0038
	inc	@r2,#2
	ld	r2,%0002(r15)
	ld	r3,%0000(r15)
	ld	@r2,r3
	jr	L0t_005c
L0t_0038:
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
L0t_005c:
	inc	r15,#8
	ret
__doprnt::
	sub	r15,#%00A4
	ldm	%009C(r15),r11,#4
	ldm	@r15,r3,#4
	ld	r14,r7
	clr	%000A(r15)
	clr	%0072(r15)
	lda	r2,%0074(r15)
	ld	%0094(r15),r2
	ld	%000C(r15),r6
	call	__length
	ld	r12,r2
	clr	L0d_0000
	jr	L0t_00c2
L0t_0090:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_00b4
L0t_00a2:
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	extsb	r2
	jr	L0t_00be
L0t_00b4:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_00be:
	inc	%0072(r15),#1
L0t_00c2:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0t_0904
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r13,r3
	cp	r3,#%0025
	jr	ne,L0t_0090
	test	r12
	jp	eq,L0t_0904
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%0010(r15)
	ld	%0012(r15),#%FFFF
	ldb	%0097(r15),#%20
	clr	%0070(r15)
	clr	%006C(r15)
	clr	%006A(r15)
	clr	%006E(r15)
	clr	%0066(r15)
	jr	L0t_0158
L0t_0112:
	test	%006C(r15)
	jr	eq,L0t_0186
	ldb	%0063(r15),#%20
	ld	%006E(r15),#%0001
	jr	L0t_018c
L0t_0126:
	test	%006E(r15)
	jr	ne,L0t_0180
	ld	%006E(r15),#%0001
	jr	L0t_0178
L0t_0134:
	test	%006C(r15)
	jr	ne,L0t_0180
	ld	%006C(r15),#%0001
	jr	L0t_0178
L0t_0142:
	test	%006A(r15)
	jr	ne,L0t_0180
	ld	%006A(r15),#%0001
	jr	L0t_0178
L0t_0150:
	ldk	r3,#0
	ldb	rl3,@r14
	ld	r13,r3
	inc	r14,#1
L0t_0158:
	ld	r2,r13
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L0t_0180
	rl	r2,#1
	ld	r2,L0d_0036(r2)
	jp	@r2
L0t_016c:
	test	%0066(r15)
	jr	ne,L0t_0180
	ld	%0066(r15),#%0001
L0t_0178:
	ld	r3,r12
	dec	r12,#1
	test	r3
	jr	ne,L0t_0150
L0t_0180:
	test	%006E(r15)
	jr	eq,L0t_0112
L0t_0186:
	ldb	%0063(r15),#%2B
L0t_018c:
	cp	r13,#%0061
	jr	lt,L0t_023a
	jr	L0t_0252
L0t_0194:
	cp	r13,#%002A
	jr	ne,L0t_01be
	ld	r2,%000C(r15)
	ld	r3,@r2
	ld	%0010(r15),r3
	inc	%000C(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0t_01de
	ld	r4,%0004(r15)
	ld	%000C(r15),r4
	jr	L0t_01de
L0t_01be:
	cp	r13,#%0030
	jr	ne,L0t_01ca
	ldb	%0097(r15),#%30
L0t_01ca:
	lda	r5,%0010(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L0t_01de:
	test	r12
	jr	eq,L0t_0252
	cpb	@r14,#%2E
	jr	ne,L0t_022c
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,L0t_0252
	cpb	@r14,#%2A
	jr	ne,L0t_021c
	ld	r2,%000C(r15)
	ld	r3,@r2
	ld	%0012(r15),r3
	inc	%000C(r15),#2
	ldk	r11,#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0t_0228
	ld	r4,%0004(r15)
	ld	%000C(r15),r4
	jr	L0t_0228
L0t_021c:
	lda	r5,%0012(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L0t_0228:
	sub	r12,r11
	add	r14,r11
L0t_022c:
	test	r12
	jr	eq,L0t_0252
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L0t_023a:
	cp	r13,#%002A
	jr	eq,L0t_0194
	cp	r13,#%0030
	jr	lt,L0t_024c
	cp	r13,#%0039
	jr	le,L0t_0194
L0t_024c:
	cp	r13,#%002E
	jr	eq,L0t_0194
L0t_0252:
	cp	%0012(r15),#%0000
	jr	ge,L0t_0278
	ld	r2,r13
	ld	r3,#L0d_000a
	ld	r4,#%0006
	cpir	r2,@r3,r4,eq
	jr	ne,L0t_0278
	add	r4,r4
	neg	r4
	ld	r2,L0d_005c(r4)
	jp	@r2
L0t_0274:
	clr	%0012(r15)
L0t_0278:
	ld	%0098(r15),r13
	ld	r2,r13
	ld	r3,#L0d_0016
	ld	r4,#%0010
	cpir	r2,@r3,r4,eq
	jp	ne,L0t_0798
	add	r4,r4
	neg	r4
	ld	r2,L0d_007c(r4)
	jp	@r2
L0t_0298:
	ld	%0012(r15),#%0006
	jr	L0t_0278
L0t_02a0:
	ld	%0014(r15),#%0008
	jr	L0t_02b6
L0t_02a8:
	ld	%0014(r15),#%0010
	jr	L0t_02b6
L0t_02b0:
	ld	%0014(r15),#%000A
L0t_02b6:
	ld	r3,%000C(r15)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	%0016(r15),rr6
	inc	%000C(r15),#2
	clr	%0016(r15)
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0t_041c
	ld	r4,%0004(r15)
	ld	%000C(r15),r4
	jp	L0t_041c
L0t_02e6:
	test	r12
	jr	eq,L0t_03c2
	dec	r12,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r3
	subb	rh4,rh4
	ld	r13,r4
	cp	r4,#%006F
	jr	ne,L0t_034c
L0t_02fc:
	ld	%0014(r15),#%0008
	jr	L0t_031a
L0t_0304:
	ld	%0014(r15),#%0010
	jr	L0t_031a
L0t_030c:
	ld	%0014(r15),#%FFF6
	jr	L0t_031a
L0t_0314:
	ld	%0014(r15),#%000A
L0t_031a:
	cp	%0002(r15),#%0005
	jr	ge,L0t_0330
	lda	r7,%0002(r15)
	lda	r6,%000C(r15)
	ld	r5,%0004(r15)
	calr	__align
L0t_0330:
	ld	r2,%000C(r15)
	ldl	rr4,@r2
	ldl	%0016(r15),rr4
	inc	%000C(r15),#4
	inc	%0002(r15),#2
	cp	%0002(r15),#%0005
	jr	eq,L0t_0414
	jr	L0t_041c
L0t_034c:
	cp	r13,#%0078
	jr	eq,L0t_0304
	cp	r13,#%0064
	jr	eq,L0t_030c
	cp	r13,#%0075
	jr	eq,L0t_0314
	ld	r3,%0000(r15)
	dec	%0002(r3),#1
	ld	r4,%0002(r3)
	cp	r4,#%0000
	jr	lt,L0t_0384
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,#%6C
	ldb	rl3,#%6C
	extsb	r3
	jr	L0t_0390
L0t_0384:
	ld	r7,#%006C
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_0390:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_03b4
	ld	r2,r13
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	extsb	r2
	jr	L0t_03be
L0t_03b4:
	ld	r7,r13
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_03be:
	inc	%0072(r15),#2
L0t_03c2:
	test	%0012(r15)
	jr	eq,L0t_03da
	cp	%0098(r15),#%0073
	jr	ne,L0t_03da
	cp	r11,%0012(r15)
	jr	le,L0t_03da
	ld	r11,%0012(r15)
L0t_03da:
	cp	r11,%0010(r15)
	jp	ge,L0t_07b0
	ld	r2,%0010(r15)
	sub	r2,r11
	ld	%0068(r15),r2
	jp	L0t_07b4
L0t_03f0:
	ld	%0014(r15),#%FFF6
	ld	r2,%000C(r15)
	ld	r3,@r2
	ld	r5,r3
	exts	rr4
	ldl	%0016(r15),rr4
	inc	%000C(r15),#2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jr	ne,L0t_041c
L0t_0414:
	ld	r3,%0004(r15)
	ld	%000C(r15),r3
L0t_041c:
	test	%006E(r15)
	jr	eq,L0t_0450
	cp	%0014(r15),#%FFF6
	jr	ne,L0t_0450
	ldl	rr2,%0016(r15)
	cpl	rr2,#%00000000
	jr	lt,L0t_0450
	ldb	rl2,%0063(r15)
	ldb	%001C(r15),rl2
	lda	r3,%001C(r15)
	ld	%0070(r15),#%0001
	inc	r3,#1
	ld	%005C(r15),r3
	jr	L0t_04b2
L0t_0450:
	cp	%0014(r15),#%0008
	jr	ne,L0t_047a
	test	%006A(r15)
	jr	eq,L0t_047a
	ldl	rr2,%0016(r15)
	testl	rr2
	jr	eq,L0t_047a
	ldb	%001C(r15),#%30
	lda	r2,%001C(r15)
	ld	%0070(r15),#%0001
	inc	r2,#1
	jr	L0t_04ae
L0t_047a:
	cp	%0014(r15),#%0010
	jr	ne,L0t_04aa
	test	%006A(r15)
	jr	eq,L0t_04aa
	ldl	rr2,%0016(r15)
	testl	rr2
	jr	eq,L0t_04aa
	ldb	%001C(r15),#%30
	ldb	%001D(r15),#%78
	lda	r2,%001C(r15)
	ld	%0070(r15),#%0002
	inc	r2,#2
	jr	L0t_04ae
L0t_04aa:
	lda	r2,%001C(r15)
L0t_04ae:
	ld	%005C(r15),r2
L0t_04b2:
	lda	r2,%001C(r15)
	ld	%001A(r15),r2
	ld	r7,%005C(r15)
	ld	r6,%0014(r15)
	ldl	rr4,%0016(r15)
	call	__cnvt
	add	r2,%0070(r15)
L0t_04ce:
	ld	r11,r2
	jp	L0t_03c2
L0t_04d4:
	ldb	%0097(r15),#%20
	ld	r2,%000C(r15)
	ld	r3,@r2
	ld	%001A(r15),r3
	inc	%000C(r15),#2
	test	r3
	jr	ne,L0t_04f2
	ld	%001A(r15),#L0d_0002
L0t_04f2:
	ld	r7,%001A(r15)
	call	__length
	ld	r11,r2
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0t_03c2
	ld	r2,%0004(r15)
	ld	%000C(r15),r2
	jp	L0t_03c2
L0t_0516:
	ldb	%0097(r15),#%20
	ldk	r11,#1
	ld	r2,%000C(r15)
	ld	%001A(r15),r2
	inc	%000C(r15),#2
	inc	%001A(r15),#1
	inc	%0002(r15),#1
	cp	%0002(r15),#%0005
	jp	ne,L0t_03c2
	ld	r3,%0004(r15)
	ld	%000C(r15),r3
	jp	L0t_03c2
L0t_0548:
	ld	r2,L0d_0000
	inc	L0d_0000,#1
	test	r2
	jr	ne,L0t_055c
	lda	r7,%0074(r15)
	call	__stfl
L0t_055c:
	cp	%000A(r15),#%0004
	jr	ge,L0t_057e
	ld	r2,%000C(r15)
	ld	%000E(r15),r2
	ld	r3,%0094(r15)
	ld	%000C(r15),r3
	inc	%0094(r15),#8
	inc	%000A(r15),#1
	jr	L0t_0596
L0t_057e:
	cp	%000A(r15),#%0004
	jr	ne,L0t_0596
	clr	%000E(r15)
	ld	r2,%0004(r15)
	ld	%000C(r15),r2
	inc	%0002(r15),#5
L0t_0596:
	lda	r6,%001C(r15)
	ld	r7,%0012(r15)
	ld	r2,%000C(r15)
	.word	%0F24,%8303	// EPU
	ld	r5,r13
	call	__fegcvt
	ld	%001A(r15),r2
	test	%000E(r15)
	jr	eq,L0t_05c0
	ld	r2,%000E(r15)
	ld	%000C(r15),r2
	jr	L0t_05c4
L0t_05c0:
	inc	%000C(r15),#8
L0t_05c4:
	test	%006E(r15)
	jr	eq,L0t_0616
	ld	r2,%001A(r15)
	cpb	@r2,#%2D
	jr	eq,L0t_0616
	testb	@r2
	jr	eq,L0t_0616
	ld	%005C(r15),r2
	ldb	rl3,@r2
	ldb	%005F(r15),rl3
	ldb	rl4,%0063(r15)
	ldb	@r2,rl4
	inc	%005C(r15),#1
	jr	L0t_060a
L0t_05ee:
	ld	r2,%005C(r15)
	ldb	rl3,@r2
	ldb	%0061(r15),rl3
	ldb	rl4,%005F(r15)
	ldb	@r2,rl4
	inc	%005C(r15),#1
	ldb	rl4,%0061(r15)
	ldb	%005F(r15),rl4
L0t_060a:
	testb	%005F(r15)
	jr	ne,L0t_05ee
	ld	r2,%005C(r15)
	clrb	@r2
L0t_0616:
	test	%006A(r15)
	jr	ne,L0t_06c6
	cp	r13,#%0065
	jr	eq,L0t_0628
	cp	r13,#%0045
	jr	ne,L0t_0674
L0t_0628:
	ld	r2,%001A(r15)
	ld	%005C(r15),r2
	jr	L0t_0648
L0t_0632:
	ld	r2,%005C(r15)
	ld	r3,r2
	ldb	rl4,%0001(r2)
	ldb	@r3,rl4
	inc	%005C(r15),#1
	jr	L0t_066a
L0t_0644:
	inc	%005C(r15),#1
L0t_0648:
	ld	r2,%005C(r15)
	testb	@r2
	jp	eq,L0t_0760
	cpb	@r2,#%2E
	jr	ne,L0t_0644
	inc	%005C(r15),#1
	ld	r2,%005C(r15)
	cpb	@r2,#%65
	jr	ne,L0t_0644
	dec	%005C(r15),#1
L0t_066a:
	ld	r2,%005C(r15)
	testb	@r2
	jr	ne,L0t_0632
	jr	L0t_0760
L0t_0674:
	cp	r13,#%0067
	jr	eq,L0t_0680
	cp	r13,#%0047
	jr	ne,L0t_0760
L0t_0680:
	ld	r2,%001A(r15)
	ld	%005C(r15),r2
	jr	L0t_069c
L0t_068a:
	ld	r7,%0064(r15)
	ld	r6,%005C(r15)
	call	_strcpy
	jr	L0t_0760
L0t_0698:
	inc	%005C(r15),#1
L0t_069c:
	ld	r2,%005C(r15)
	testb	@r2
	jr	eq,L0t_0760
	cpb	@r2,#%2E
	jr	ne,L0t_0698
	ld	r3,r2
	ld	%0064(r15),r2
L0t_06b0:
	inc	%005C(r15),#1
	ld	r2,%005C(r15)
	cpb	@r2,#%30
	jr	eq,L0t_06b0
	cpb	@r2,#%65
	jr	eq,L0t_068a
	jr	L0t_0760
L0t_06c6:
	cp	r13,#%0066
	jr	ne,L0t_06fe
	ld	r2,%001A(r15)
	ld	%005C(r15),r2
	jr	L0t_06e4
L0t_06d6:
	ld	r2,%005C(r15)
	cpb	@r2,#%2E
	jr	eq,L0t_06ec
	inc	%005C(r15),#1
L0t_06e4:
	ld	r2,%005C(r15)
	testb	@r2
	jr	ne,L0t_06d6
L0t_06ec:
	ld	r2,%005C(r15)
	testb	@r2
	jr	ne,L0t_0760
	ldb	@r2,#%2E
	inc	%005C(r15),#1
	jr	L0t_075a
L0t_06fe:
	cp	r13,#%0067
	jr	eq,L0t_070a
	cp	r13,#%0047
	jr	ne,L0t_0760
L0t_070a:
	ld	r2,%001A(r15)
	ld	%005C(r15),r2
	jr	L0t_0722
L0t_0714:
	ld	r2,%005C(r15)
	cpb	@r2,#%2E
	jr	eq,L0t_072a
	inc	%005C(r15),#1
L0t_0722:
	ld	r2,%005C(r15)
	testb	@r2
	jr	ne,L0t_0714
L0t_072a:
	ld	r2,%005C(r15)
	testb	@r2
	jr	ne,L0t_0760
	ldb	@r2,#%2E
	inc	%005C(r15),#1
	clr	%0070(r15)
	jr	L0t_0750
L0t_0740:
	ld	r3,%005C(r15)
	ldb	@r3,#%30
	inc	%005C(r15),#1
	inc	%0070(r15),#1
L0t_0750:
	ld	r2,%0070(r15)
	cp	r2,%0012(r15)
	jr	lt,L0t_0740
L0t_075a:
	ld	r2,%005C(r15)
	clrb	@r2
L0t_0760:
	cp	r13,#%0045
	jr	eq,L0t_076c
	cp	r13,#%0047
	jr	ne,L0t_0786
L0t_076c:
	ld	r2,%001A(r15)
	ld	%005C(r15),r2
L0t_0774:
	ld	r2,%005C(r15)
	testb	@r2
	jr	eq,L0t_0786
	cpb	@r2,#%65
	jr	ne,L0t_0792
	ldb	@r2,#%45
L0t_0786:
	ld	r7,%001A(r15)
	call	__length
	jp	L0t_04ce
L0t_0792:
	inc	%005C(r15),#1
	jr	L0t_0774
L0t_0798:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jp	lt,L0t_00b4
	jp	L0t_00a2
L0t_07b0:
	clr	%0068(r15)
L0t_07b4:
	test	%0066(r15)
	jr	eq,L0t_07f4
	jr	L0t_0800
L0t_07bc:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_07e2
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%0097(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L0t_07f0
L0t_07e2:
	ldb	rl7,%0097(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_07f0:
	inc	%0072(r15),#1
L0t_07f4:
	ld	r2,%0068(r15)
	dec	%0068(r15),#1
	test	r2
	jr	ne,L0t_07bc
L0t_0800:
	cp	%0014(r15),#%FFF6
	jr	ne,L0t_08ac
	ldl	rr2,%0016(r15)
	cpl	rr2,#%00000000
	jr	ge,L0t_08ac
	ld	%0014(r15),#%000A
	ldl	rr6,rr2
	multl	rq4,#%FFFFFFFF
	ldl	%0016(r15),rr6
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_084c
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L0t_0858
L0t_084c:
	ld	r7,#%002D
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_0858:
	inc	%0072(r15),#1
	inc	%001A(r15),#1
	dec	r11,#1
	jr	L0t_08ac
L0t_0864:
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_0890
	ld	r2,%001A(r15)
	inc	%001A(r15),#1
	ld	r3,%0000(r15)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	rl3,@r2
	ldb	@r4,rl3
	extsb	r3
	jr	L0t_08a8
L0t_0890:
	ld	r7,%001A(r15)
	inc	%001A(r15),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,%0000(r15)
	call	__flsbuf
	ld	r3,r2
L0t_08a8:
	inc	%0072(r15),#1
L0t_08ac:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0t_0864
	test	%0066(r15)
	jp	eq,L0t_00c2
L0t_08bc:
	ld	r2,%0068(r15)
	dec	%0068(r15),#1
	test	r2
	jp	eq,L0t_00c2
	ld	r2,%0000(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L0t_08f0
	ld	r2,%0000(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%0097(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L0t_08fe
L0t_08f0:
	ldb	rl7,%0097(r15)
	extsb	r7
	ld	r6,%0000(r15)
	call	__flsbuf
L0t_08fe:
	inc	%0072(r15),#1
	jr	L0t_08bc
L0t_0904:
	test	__pfile
	jr	eq,L0t_0922
	ld	r2,__pfile
	ldb	rl3,%0006(r2)
	extsb	r3
	and	r3,#%0020
	test	r3
	jr	eq,L0t_0922
	ld	r2,#%FFFF
	jr	L0t_0926
L0t_0922:
	ld	r2,%0072(r15)
L0t_0926:
	ldm	r11,%009C(r15),#4
	add	r15,#%00A4
	ret
__atoip::
	sub	r15,#%0012
	ldm	@r15,r5,#2
	ld	r1,r7
	clr	%0006(r15)
	ld	r4,r6
	test	r6
	jr	eq,L0t_0958
	cpb	@r1,#%2D
	jr	ne,L0t_0958
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0006(r15),#%0001
L0t_0958:
	ldk	r5,#0
	jr	L0t_0988
L0t_095c:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0009(r15),rl3
	cpb	rl3,#%30
	jr	lt,L0t_098c
	cpb	rl3,#%39
	jr	gt,L0t_098c
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0009(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L0t_0988:
	test	r4
	jr	ne,L0t_095c
L0t_098c:
	test	%0006(r15)
	jr	eq,L0t_0998
	ld	r2,r5
	neg	r2
	jr	L0t_099a
L0t_0998:
	ld	r2,r5
L0t_099a:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	add	r15,#%0012
	ret

// segment table entry 0 (segment 0), data, 126 bytes
	.psec
	.data
L0d_0000:
	.word	%0000
L0d_0002:
	.word	%286E
	.word	%756C
	.word	%6C29
	.word	%0000
L0d_000a:
	.word	%0045
	.word	%0047
	.word	%0065
	.word	%0066
	.word	%0067
	.word	%0073
L0d_0016:
	.word	%0044
	.word	%0045
	.word	%0047
	.word	%004F
	.word	%0055
	.word	%0058
	.word	%0063
	.word	%0064
	.word	%0065
	.word	%0066
	.word	%0067
	.word	%006C
	.word	%006F
	.word	%0073
	.word	%0075
	.word	%0078
L0d_0036:
	.word	L0t_0134
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0142
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0180
	.word	L0t_0126
	.word	L0t_0180
	.word	L0t_016c
	.word	L0t_0298
	.word	L0t_0298
	.word	L0t_0298
	.word	L0t_0298
	.word	L0t_0298
L0d_005c:
	.word	L0t_0274
	.word	L0t_030c
	.word	L0t_0548
	.word	L0t_0548
	.word	L0t_02fc
	.word	L0t_0314
	.word	L0t_0304
	.word	L0t_0516
	.word	L0t_03f0
	.word	L0t_0548
	.word	L0t_0548
	.word	L0t_0548
	.word	L0t_02e6
	.word	L0t_02a0
	.word	L0t_04d4
	.word	L0t_02b0
L0d_007c:
	.word	L0t_02a8
