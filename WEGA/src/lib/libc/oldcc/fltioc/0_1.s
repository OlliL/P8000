// fltioc.o: magic E711 nonsegmented
	.extern	__fltuse
	.extern	__inflt
	.extern	__filbuf
	.extern	_ungetc
	.extern	__ctype
	.extern	_atof
	.extern	_gcvt
	.extern	_fegcvt
	.extern	__fegcvt
	.extern	_ecvt
	.extern	_strcpy
	.extern	__cnvt

// segment table entry 0 (segment 0), text, 1244 bytes
	.psec
	.code
__fltuse::
	ret
__inflt::
	sub	r15,#%0098
	ldm	%008E(r15),r10,#5
	ldm	@r15,r3,#5
	lda	r2,%000C(r15)
	ld	r14,r2
	ldk	r10,#0
	ld	r11,r10
	ld	r12,r10
	cp	r6,#%007E
	jr	le,L0t_0028
	ld	%0006(r15),#%007E
L0t_0028:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0044
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_0052
L0t_0044:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_0052:
	ldb	%000B(r15),rl2
	cpb	rl2,#%20
	jr	eq,L0t_0028
	cpb	rl2,#%09
	jr	eq,L0t_0028
	cpb	rl2,#%0A
	jr	eq,L0t_0028
	cpb	rl2,#%2D
	jr	ne,L0t_00a6
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_008e
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_009c
L0t_008e:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_009c:
	ldb	%000B(r15),rl2
	dec	%0006(r15),#1
	jr	L0t_00e0
L0t_00a6:
	cpb	%000B(r15),#%2B
	jr	ne,L0t_00e0
	dec	%0006(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_00ce
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_00dc
L0t_00ce:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_00dc:
	ldb	%000B(r15),rl2
L0t_00e0:
	cpb	%000B(r15),#%49
	jr	eq,L0t_00f2
	cpb	%000B(r15),#%4E
	jp	ne,L0t_0238
L0t_00f2:
	cpb	%000B(r15),#%49
	jr	ne,L0t_0100
	ld	r2,#L0d_0000
	jr	L0t_0104
L0t_0100:
	ld	r2,#L0d_000a
L0t_0104:
	ld	r13,r2
	jr	L0t_0152
L0t_0108:
	testb	@r13
	jr	eq,L0t_0166
	ldb	rl2,@r13
	cpb	rl2,%000B(r15)
	jr	eq,L0t_011a
	cpb	@r13,#%7E
	jr	ne,L0t_0166
L0t_011a:
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	inc	r13,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0140
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_014e
L0t_0140:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_014e:
	ldb	%000B(r15),rl2
L0t_0152:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0t_0166
	cpb	%FFFF(r14),#%29
	jr	ne,L0t_0108
L0t_0166:
	cpb	%000B(r15),#%FF
	jr	eq,L0t_0258
	ldb	rl7,%000B(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	_ungetc
	ld	r2,%0000(r15)
	clr	@r2
	jr	L0t_0260
L0t_0184:
	test	r10
	jr	ne,L0t_01ba
	test	r11
	jr	ne,L0t_01ba
	inc	r10,#1
L0t_018e:
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0226
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_0234
L0t_01b2:
	cpb	%000B(r15),#%2E
	jr	eq,L0t_0184
L0t_01ba:
	cpb	%000B(r15),#%65
	jr	eq,L0t_01ca
	cpb	%000B(r15),#%45
	jr	ne,L0t_0166
L0t_01ca:
	test	r11
	jr	ne,L0t_0166
	test	r12
	jr	eq,L0t_0166
	inc	r11,#1
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_01f8
	ld	r7,%0002(r15)
	call	__filbuf
	jr	L0t_0206
L0t_01f8:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_0206:
	ldb	%000B(r15),rl2
	cpb	rl2,#%2B
	jr	eq,L0t_018e
	cpb	rl2,#%2D
	jr	eq,L0t_018e
	cpb	rl2,#%30
	jr	lt,L0t_0166
	extsb	r2
	cp	r2,#%0039
	jr	le,L0t_018e
	jr	L0t_0166
L0t_0226:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L0t_0234:
	ldb	%000B(r15),rl2
L0t_0238:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0t_0166
	ldb	rl2,%000B(r15)
	extsb	r2
	ldb	rl3,__ctype+1(r2)
	extsb	r3
	bit	r3,#2
	jr	eq,L0t_01b2
	inc	r12,#1
	jr	L0t_018e
L0t_0258:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L0t_0260:
	test	%0008(r15)
	jr	eq,L0t_026e
	lda	r2,%000C(r15)
	cp	r14,r2
	jr	ne,L0t_0272
L0t_026e:
	sub	r2,r2
	jr	L0t_02bc
L0t_0272:
	clrb	@r14
	inc	r14,#1
	ld	r2,%0004(r15)
	ld	r3,#L0d_0016
	ld	r4,#%0003
	cpir	r2,@r3,r4,eq
	jr	ne,L0t_02a4
	add	r4,r4
	neg	r4
	ld	r2,L0d_0020(r4)
	jp	@r2
L0t_0292:
	lda	r7,%000C(r15)
	call	_atof
	ld	r2,%0008(r15)
	ld	r3,@r2
	.word	%0F3C,%8001	// EPU
L0t_02a4:
	ldk	r2,#1
	jr	L0t_02bc
L0t_02a8:
	lda	r7,%000C(r15)
	call	_atof
	ld	r2,%0008(r15)
	ld	r3,@r2
	.word	%0F3C,%8003	// EPU
	jr	L0t_02a4
L0t_02bc:
	ldm	r10,%008E(r15),#5
	add	r15,#%0098
	ret
_gcvt::
	dec	r15,#14
	.word	%4FFC,%8303,%0004	// EPU
	ldl	@r15,rr6
	.word	%8E04,%8300	// EPU
	.word	%8E34,%8000	// EPU
	ld	r2,r7
	ld	r3,r6
	ld	r5,#%0067
	calr	__fegcvt
	inc	r15,#14
	ret
_fegcvt::
	dec	r15,#16
	.word	%4FFC,%8303,%0006	// EPU
	ldm	@r15,r5,#3
	.word	%8E04,%8300	// EPU
	.word	%8E34,%8000	// EPU
	ld	r4,r7
	ld	r2,r6
	ldb	rl3,rl5
	extsb	r5
	calr	__fegcvt
	inc	r15,#16
	ret
__fegcvt::
	sub	r15,#%001C
	ldm	%0014(r15),r11,#4
	.word	%4FFC,%8303,%0006	// EPU
	ldm	@r15,r5,#3
	ld	r14,r6
	cp	r7,#%0000
	jr	ge,L0t_032c
	ld	%0004(r15),#%0006
L0t_032c:
	cpb	%0001(r15),#%66
	jr	ne,L0t_033c
	ld	r2,%0004(r15)
	neg	r2
	jr	L0t_0342
L0t_033c:
	ld	r2,%0004(r15)
	inc	r2,#1
L0t_0342:
	ld	r11,r2
	lda	r6,%000E(r15)
	lda	r5,%0010(r15)
	.word	%4FF4,%8303,%0006	// EPU
	ld	r7,r11
	call	_ecvt
	ld	r13,r2
	test	%0010(r15)
	jr	eq,L0t_0366
	ldb	@r14,#%2D
	inc	r14,#1
L0t_0366:
	cpb	@r13,#%39
	jr	le,L0t_0378
	ld	r7,r14
	ld	r6,r13
	call	_strcpy
	jp	L0t_04cc
L0t_0378:
	cpb	%0001(r15),#%67
	jr	ne,L0t_03d6
	ld	r2,%0004(r15)
	neg	r2
	ld	r5,r2
	exts	rr4
	div	rr4,#%0002
	cp	r5,%000E(r15)
	jr	gt,L0t_03d6
	cp	%0004(r15),#%0006
	jr	ge,L0t_03a0
	ldk	r2,#6
	jr	L0t_03a4
L0t_03a0:
	ld	r2,%0004(r15)
L0t_03a4:
	cp	r2,%000E(r15)
	jr	lt,L0t_03d6
	ld	r2,%000E(r15)
	add	r2,%0004(r15)
	cp	r2,#%0013
	jr	gt,L0t_03d6
	ld	r2,%0004(r15)
	neg	r2
	ld	r11,r2
	lda	r6,%000E(r15)
	lda	r5,%0010(r15)
	.word	%4FF4,%8303,%0006	// EPU
	ld	r7,r11
	call	_ecvt
	ld	r13,r2
L0t_03d6:
	cpb	@r13,#%30
	jr	ne,L0t_0426
	testb	%0001(r13)
	jr	ne,L0t_0426
	cpb	%0001(r15),#%67
	jr	ne,L0t_0404
	ldk	r11,#0
L0t_03ec:
	ldk	r12,#1
L0t_03ee:
	cp	r12,#%0000
	jr	le,L0t_0452
	testb	@r13
	jr	eq,L0t_0452
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	dec	r12,#1
	jr	L0t_03ee
L0t_0404:
	cpb	%0001(r15),#%66
	jr	ne,L0t_0416
	ldk	r2,#1
	sub	r2,%0004(r15)
	ld	r12,r2
	jr	L0t_0430
L0t_0416:
	ldk	r2,#1
	sub	r2,%0004(r15)
	ld	r12,r2
	ld	%000E(r15),#%0001
	jr	L0t_0430
L0t_0426:
	cp	r11,#%0000
	jr	gt,L0t_03ec
	ld	r12,%000E(r15)
L0t_0430:
	cp	r12,#%0000
	jr	gt,L0t_03ee
	ldb	@r14,#%30
	inc	r14,#1
	ldb	@r14,#%2E
	inc	r14,#1
L0t_0442:
	cp	r12,#%0000
	jr	ge,L0t_0474
	ldb	@r14,#%30
	inc	r14,#1
	inc	r12,#1
	jr	L0t_0442
L0t_0452:
	cp	r12,#%0000
	jr	le,L0t_0462
	ldb	@r14,#%30
	inc	r14,#1
	dec	r12,#1
	jr	L0t_0452
L0t_0462:
	ldb	@r14,#%2E
	inc	r14,#1
	jr	L0t_0474
L0t_046a:
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	inc	r12,#1
L0t_0474:
	testb	@r13
	jr	ne,L0t_046a
	cp	r11,#%0000
	jr	le,L0t_04a2
	ldb	@r14,#%65
	inc	r14,#1
	dec	%000E(r15),#1
	ld	r5,%000E(r15)
	exts	rr4
	ld	r6,#%FFF6
	ld	r7,r14
	call	__cnvt
	ld	r12,r2
	ld	r2,r14
	add	r2,r12
	clrb	@r2
	jr	L0t_04cc
L0t_04a2:
	cp	r11,#%0000
	jr	ge,L0t_04c8
	clrb	@r14
	cpb	%0001(r15),#%67
	jr	eq,L0t_04b6
	jr	L0t_04cc
L0t_04b4:
	clrb	@r14
L0t_04b6:
	dec	r14,#1
	ldb	rl2,@r14
	cpb	rl2,#%30
	jr	eq,L0t_04b4
	cpb	@r14,#%2E
	jr	eq,L0t_04ca
	jr	L0t_04cc
L0t_04c8:
	dec	r14,#1
L0t_04ca:
	clrb	@r14
L0t_04cc:
	ld	r2,%0002(r15)
	ldm	r11,%0014(r15),#4
	add	r15,#%001C
	ret

// segment table entry 0 (segment 0), data, 34 bytes
	.psec
	.data
L0d_0000:
	.word	%494E
	.word	%4649
	.word	%4E49
	.word	%5459
	.word	%0000
L0d_000a:
	.word	%4E41
	.word	%4E28
	.word	%7E7E
	.word	%7E7E
	.word	%7E7E
	.word	%2900
L0d_0016:
	.word	%0000
	.word	%0001
	.word	%0002
	.word	L0t_0292
	.word	L0t_0292
L0d_0020:
	.word	L0t_02a8
