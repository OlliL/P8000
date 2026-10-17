// doscan.o: magic E711 nonsegmented
	.extern	__sctab
	.extern	__doscan
	.extern	__ctype
	.extern	_tolower
	.extern	__filbuf
	.extern	_ungetc
	.extern	__innum
	.extern	__inflt
	.extern	__instr
	.extern	__getccl

// segment table entry 0 (segment 0), text, 1536 bytes
	.psec
	.code
__doscan::
	sub	r15,#%001E
	ldm	%0018(r15),r12,#3
	ldm	@r15,r4,#4
	ld	r13,r5
	ld	r14,r6
	clr	%0008(r15)
	clr	%0010(r15)
	clr	%0014(r15)
	jp	L0t_0180
L0t_0022:
	add	r4,r4
	neg	r4
	ld	r2,L0d_0092(r4)
	jp	@r2
L0t_002c:
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r12,r3
	cp	r3,#%0025
	jp	eq,L0t_019a
	clr	%000E(r15)
	cp	r3,#%002A
	jr	eq,L0t_0060
	ld	%000E(r15),r13
	inc	r13,#2
	inc	%0014(r15),#1
	cp	%0014(r15),#%0005
	jr	ne,L0t_0068
	ld	r13,%0000(r15)
	jr	L0t_0068
L0t_0060:
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L0t_0068:
	clr	%000A(r15)
	ld	%0012(r15),#%0001
	jr	L0t_008e
L0t_0074:
	ld	r3,%000A(r15)
	mult	rr2,#%000A
	add	r3,r12
	sub	r3,#%0030
	ld	%000A(r15),r3
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L0t_008e:
	ldb	rl2,__ctype+1(r12)
	extsb	r2
	bit	r2,#2
	jr	ne,L0t_0074
	test	%000A(r15)
	jr	ne,L0t_00a4
	ld	%000A(r15),#%7530
L0t_00a4:
	cp	r12,#%006C
	jr	ne,L0t_00ba
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	ld	%0012(r15),#%0002
	jr	L0t_00da
L0t_00ba:
	cp	r12,#%0068
	jr	ne,L0t_00ce
	clr	%0012(r15)
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	jr	L0t_00da
L0t_00ce:
	cp	r12,#%005B
	jr	ne,L0t_00da
	ld	r7,r14
	calr	__getccl
	ld	r14,r2
L0t_00da:
	ldb	rl2,__ctype+1(r12)
	extsb	r2
	bit	r2,#0
	jr	eq,L0t_00f2
	ld	r7,r12
	call	_tolower
	ld	r12,r2
	ld	%0012(r15),#%0002
L0t_00f2:
	test	r12
	jr	ne,L0t_00fc
L0t_00f6:
	ld	r2,#%FFFF
	jr	L0t_01e2
L0t_00fc:
	lda	r2,%0010(r15)
	ld	r6,r12
	ld	r5,%000A(r15)
	ld	r4,%0012(r15)
	ld	r3,%0006(r15)
	ld	r7,%000E(r15)
	calr	__innum
	test	r2
	jr	eq,L0t_0122
	test	%000E(r15)
	jr	eq,L0t_0122
	inc	%0008(r15),#1
L0t_0122:
	test	%0010(r15)
	jr	eq,L0t_0180
	test	%0008(r15)
	jr	eq,L0t_00f6
	jr	L0t_01de
L0t_0130:
	ld	r2,%0006(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_014c
	ld	r7,%0006(r15)
	call	__filbuf
	jr	L0t_015a
L0t_014c:
	ld	r2,%0006(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_015a:
	ld	%000C(r15),r2
	cp	r2,#%0020
	jr	eq,L0t_0130
	cp	r2,#%0009
	jr	eq,L0t_0130
	cp	r2,#%000A
	jr	eq,L0t_0130
	cp	r2,#%FFFF
	jr	eq,L0t_0180
	ld	r7,r2
	ld	r6,%0006(r15)
	call	_ungetc
L0t_0180:
	ldb	rl3,@r14
	extsb	r3
	ld	r12,r3
	ld	r2,r3
	inc	r14,#1
	ld	r3,#L0d_0080
	ld	r4,#%0005
	cpir	r2,@r3,r4,eq
	jp	eq,L0t_0022
L0t_019a:
	ld	r2,%0006(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_01b6
	ld	r7,%0006(r15)
	call	__filbuf
	jr	L0t_01c4
L0t_01b6:
	ld	r2,%0006(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_01c4:
	ld	%000C(r15),r2
	cp	r12,%000C(r15)
	jr	eq,L0t_0180
	cp	r2,#%FFFF
	jr	eq,L0t_00f6
	ld	r7,r2
	ld	r6,%0006(r15)
	call	_ungetc
L0t_01de:
	ld	r2,%0008(r15)
L0t_01e2:
	ldm	r12,%0018(r15),#3
	add	r15,#%001E
	ret
__innum::
	sub	r15,#%001C
	ldm	%0018(r15),r13,#2
	ldm	@r15,r2,#6
	clr	%0010(r15)
	cp	r6,#%0063
	jr	eq,L0t_0214
	cp	r6,#%0073
	jr	eq,L0t_0214
	cp	%0008(r15),#%005B
	jr	ne,L0t_023c
L0t_0214:
	test	%000A(r15)
	jr	eq,L0t_0224
	ld	r7,%000A(r15)
	ld	r6,@r7
	ld	r7,r6
	jr	L0t_0226
L0t_0224:
	sub	r7,r7
L0t_0226:
	ld	r6,%0008(r15)
	ld	r5,%0006(r15)
	ld	r4,%0002(r15)
	ld	r3,%0000(r15)
	calr	__instr
	jp	L0t_0474
L0t_023c:
	cp	%0008(r15),#%0065
	jr	eq,L0t_024c
	cp	%0008(r15),#%0066
	jr	ne,L0t_0268
L0t_024c:
	ld	r7,%000A(r15)
	ld	r6,%0006(r15)
	ld	r5,%0004(r15)
	ld	r4,%0002(r15)
	ld	r3,%0000(r15)
	call	__inflt
	jp	L0t_0474
L0t_0268:
	subl	rr2,rr2
	ldl	%0012(r15),rr2
	ldk	r13,#10
	cp	%0008(r15),#%006F
	jr	ne,L0t_027c
	ldk	r13,#8
	jr	L0t_0288
L0t_027c:
	cp	%0008(r15),#%0078
	jr	ne,L0t_0288
	ld	r13,#%0010
L0t_0288:
	clr	%000C(r15)
L0t_028c:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_02a8
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_02b6
L0t_02a8:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_02b6:
	ld	r14,r2
	cp	r2,#%0020
	jr	eq,L0t_028c
	cp	r2,#%0009
	jr	eq,L0t_028c
	cp	r2,#%000A
	jr	eq,L0t_028c
	cp	r2,#%002D
	jr	ne,L0t_0330
	inc	%000C(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_02f0
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_02fe
L0t_02f0:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_02fe:
	ld	r14,r2
	dec	%0006(r15),#1
	jr	L0t_0366
L0t_0306:
	test	%000C(r15)
	jr	eq,L0t_0316
	subl	rr2,rr2
	subl	rr2,%0012(r15)
	ldl	%0012(r15),rr2
L0t_0316:
	cp	r14,#%FFFF
	jp	eq,L0t_0424
	ld	r7,r14
	ld	r6,%0002(r15)
	call	_ungetc
	ld	r2,%0000(r15)
	clr	@r2
	jr	L0t_042c
L0t_0330:
	cp	r14,#%002B
	jr	ne,L0t_0366
	dec	%0006(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0356
L0t_034c:
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_0364
L0t_0356:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_0364:
	ld	r14,r2
L0t_0366:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0t_0306
	ldb	rl2,__ctype+1(r14)
	extsb	r2
	bit	r2,#2
	jr	ne,L0t_039a
	cp	r13,#%0010
	jr	ne,L0t_0306
	cp	r14,#%0061
	jr	lt,L0t_038e
	cp	r14,#%0066
	jr	le,L0t_039a
L0t_038e:
	cp	r14,#%0041
	jr	lt,L0t_0306
	cp	r14,#%0046
	jr	gt,L0t_0306
L0t_039a:
	ld	%0010(r15),#%0001
	cp	r13,#%0008
	jr	ne,L0t_03b0
	ldl	rr2,%0012(r15)
	slal	rr2,#3
	jr	L0t_03d0
L0t_03b0:
	cp	r13,#%000A
	jr	ne,L0t_03c8
	ldl	rr2,%0012(r15)
	slal	rr2,#2
	addl	rr2,%0012(r15)
	slal	rr2,#1
	jr	L0t_03d0
L0t_03c8:
	ldl	rr2,%0012(r15)
	slal	rr2,#4
L0t_03d0:
	ldl	%0012(r15),rr2
	ld	%000E(r15),r14
	cp	r14,#%0030
	jr	lt,L0t_03ea
	cp	r14,#%0039
	jr	gt,L0t_03ea
	sub	r14,#%0030
	jr	L0t_0400
L0t_03ea:
	cp	r14,#%0061
	jr	lt,L0t_03fc
	cp	r14,#%0066
	jr	gt,L0t_03fc
	sub	r14,#%0057
	jr	L0t_0400
L0t_03fc:
	sub	r14,#%0037
L0t_0400:
	ld	r3,r14
	exts	rr2
	addl	rr2,%0012(r15)
	ldl	%0012(r15),rr2
	ld	r14,%000E(r15)
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0356
	jr	L0t_034c
L0t_0424:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L0t_042c:
	test	%000A(r15)
	jr	eq,L0t_0472
	ld	r2,%0004(r15)
	ld	r3,#L0d_0094
	ld	r4,#%0003
	cpir	r2,@r3,r4,eq
	jr	ne,L0t_046c
	add	r4,r4
	neg	r4
	ld	r2,L0d_009e(r4)
	jp	@r2
L0t_044e:
	ld	r3,%000A(r15)
	ld	r4,@r3
	ld	r5,%0014(r15)
	ld	@r4,r5
	jr	L0t_046c
L0t_045c:
	ldk	r2,#1
	jr	L0t_0474
L0t_0460:
	ld	r3,%000A(r15)
	ld	r4,@r3
	ldl	rr6,%0012(r15)
	ldl	@r4,rr6
L0t_046c:
	test	%0010(r15)
	jr	ne,L0t_045c
L0t_0472:
	sub	r2,r2
L0t_0474:
	ldm	r13,%0018(r15),#2
	add	r15,#%001C
	ret
__instr::
	sub	r15,#%0018
	ldm	%0010(r15),r11,#4
	ldm	@r15,r3,#4
	ld	r13,r4
	ld	r14,r7
	ld	r2,r3
	clr	@r2
	ld	r11,r14
	cp	r6,#%0063
	jr	ne,L0t_04aa
	cp	r5,#%7530
	jr	ne,L0t_04aa
	ld	%0004(r15),#%0001
L0t_04aa:
	clr	%000C(r15)
	cp	%0006(r15),#%0073
	jr	ne,L0t_04bc
	ld	%000C(r15),#%0001
L0t_04bc:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	ge,L0t_04d0
	ld	r7,r13
	call	__filbuf
	jr	L0t_04da
L0t_04d0:
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L0t_04da:
	ld	r12,r2
	ldb	rl3,__sctab(r2)
	extsb	r3
	and	r3,%000C(r15)
	jr	eq,L0t_04ee
	cp	r12,#%FFFF
	jr	ne,L0t_04bc
L0t_04ee:
	ld	%000C(r15),#%0001
	cp	%0006(r15),#%0063
	jr	ne,L0t_0502
	clr	%000C(r15)
	jr	L0t_0556
L0t_0502:
	cp	%0006(r15),#%005B
	jr	ne,L0t_0556
	ld	%000C(r15),#%0002
	jr	L0t_0556
L0t_0512:
	ldb	rl2,__sctab(r12)
	extsb	r2
	and	r2,%000C(r15)
	test	r2
	jr	ne,L0t_055c
	test	r14
	jr	eq,L0t_052a
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
L0t_052a:
	dec	%0004(r15),#1
	cp	%0004(r15),#%0000
	jr	le,L0t_055c
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	ge,L0t_054a
	ld	r7,r13
	call	__filbuf
	jr	L0t_0554
L0t_054a:
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L0t_0554:
	ld	r12,r2
L0t_0556:
	cp	r12,#%FFFF
	jr	ne,L0t_0512
L0t_055c:
	cp	r12,#%FFFF
	jr	eq,L0t_057a
	cp	%0004(r15),#%0000
	jr	le,L0t_0572
	ld	r7,r12
	ld	r6,r13
	call	_ungetc
L0t_0572:
	ld	r2,%0000(r15)
	clr	@r2
	jr	L0t_0582
L0t_057a:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L0t_0582:
	test	r14
	jr	eq,L0t_059a
	cp	r14,r11
	jr	eq,L0t_059a
	cp	%0006(r15),#%0063
	jr	eq,L0t_0596
	clrb	@r14
	inc	r14,#1
L0t_0596:
	ldk	r2,#1
	jr	L0t_059c
L0t_059a:
	sub	r2,r2
L0t_059c:
	ldm	r11,%0010(r15),#4
	add	r15,#%0018
	ret
__getccl::
	dec	r15,#14
	ld	r1,r7
	ldk	r5,#0
	cpb	@r1,#%5E
	jr	ne,L0t_05b8
	inc	r5,#1
	inc	r1,#1
L0t_05b8:
	ldk	r4,#0
L0t_05ba:
	test	r5
	jr	eq,L0t_05c4
	resb	__sctab(r4),#1
	jr	L0t_05c8
L0t_05c4:
	setb	__sctab(r4),#1
L0t_05c8:
	inc	r4,#1
	cp	r4,#%0080
	jr	lt,L0t_05ba
L0t_05d0:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r4,r3
	and	r3,#%007F
	cp	r3,#%005D
	jr	eq,L0t_05fa
	test	r5
	jr	eq,L0t_05ee
	setb	__sctab(r4),#1
	jr	L0t_05f2
L0t_05ee:
	resb	__sctab(r4),#1
L0t_05f2:
	inc	r4,#1
	test	r4
	jr	ne,L0t_05d0
	dec	r1,#1
L0t_05fa:
	ld	r2,r1
	inc	r15,#14
	ret

// segment table entry 0 (segment 0), data, 160 bytes
	.psec
	.data
__sctab::
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0001
	.word	%0100
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0100
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
L0d_0080:
	.word	%0000
	.word	%0009
	.word	%000A
	.word	%0020
	.word	%0025
	.word	L0t_01de
	.word	L0t_0130
	.word	L0t_0130
	.word	L0t_0130
L0d_0092:
	.word	L0t_002c
L0d_0094:
	.word	%0000
	.word	%0001
	.word	%0002
	.word	L0t_044e
	.word	L0t_044e
L0d_009e:
	.word	L0t_0460
