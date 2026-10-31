	.extern	__cnvt, __ctype, __filbuf, _atof, _ecvt, _strcpy, _ungetc
	.psec
	.code
__fltuse::
	sub	r15,#%0096
	ldm	%008C(r15),r10,#5
	ldm	%0000(r15),r3,#5
	lda	r2,%000C(r15)
	ld	r14,r2
	ldk	r10,#0
	ld	r11,r10
	ld	r12,r10
	cp	r6,#%007E
	jr	le,L0028
	ld	%0006(r15),#%007E
L0028:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L004e
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L0056
L004e:
	ld	r7,%0002(r15)
	call	__filbuf
L0056:
	ldb	%000B(r15),rl2
	cpb	rl2,#%20
	jr	eq,L0028
	cpb	rl2,#%09
	jr	eq,L0028
	cpb	rl2,#%0A
	jr	eq,L0028
	cpb	rl2,#%2D
	jr	ne,L00ae
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L009c
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L00a4
L009c:
	ld	r7,%0002(r15)
	call	__filbuf
L00a4:
	ldb	%000B(r15),rl2
	dec	%0006(r15),#1
	jr	L00ec
L00ae:
	cpb	%000B(r15),#%2B
	jr	ne,L00ec
	dec	%0006(r15),#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L00e0
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L00e8
L00e0:
	ld	r7,%0002(r15)
	call	__filbuf
L00e8:
	ldb	%000B(r15),rl2
L00ec:
	cpb	%000B(r15),#%49
	jr	eq,L00fe
	cpb	%000B(r15),#%4E
	jp	ne,L0250
L00fe:
	cpb	%000B(r15),#%49
	jr	ne,L010c
	ld	r2,#D0000
	jr	L0110
L010c:
	ld	r2,#D0009
L0110:
	ld	r13,r2
	jr	L0162
L0114:
	testb	@r13
	jr	eq,L0176
	ldb	rl2,@r13
	cpb	rl2,%000B(r15)
	jr	eq,L0126
	cpb	@r13,#%7E
	jr	ne,L0176
L0126:
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	inc	r13,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0156
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L015e
L0156:
	ld	r7,%0002(r15)
	call	__filbuf
L015e:
	ldb	%000B(r15),rl2
L0162:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0176
	cpb	%FFFF(r14),#%29
	jr	ne,L0114
L0176:
	cpb	%000B(r15),#%FF
	jr	eq,L0270
	ldb	rl7,%000B(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	_ungetc
	ld	r2,%0000(r15)
	clr	@r2
	jr	L0278
L0194:
	test	r10
	jr	ne,L01d4
	test	r11
	jr	ne,L01d4
	inc	r10,#1
L019e:
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0244
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L024c
L01cc:
	cpb	%000B(r15),#%2E
	jr	eq,L0194
L01d4:
	cpb	%000B(r15),#%65
	jr	eq,L01e4
	cpb	%000B(r15),#%45
	jr	ne,L0176
L01e4:
	test	r11
	jr	ne,L0176
	test	r12
	jr	eq,L0176
	inc	r11,#1
	ldb	rl2,%000B(r15)
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L021c
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L0224
L021c:
	ld	r7,%0002(r15)
	call	__filbuf
L0224:
	ldb	%000B(r15),rl2
	cpb	rl2,#%2B
	jr	eq,L019e
	cpb	rl2,#%2D
	jr	eq,L019e
	cpb	rl2,#%30
	jr	lt,L0176
	extsb	r2
	cp	r2,#%0039
	jr	le,L019e
	jr	L0176
L0244:
	ld	r7,%0002(r15)
	call	__filbuf
L024c:
	ldb	%000B(r15),rl2
L0250:
	dec	%0006(r15),#1
	cp	%0006(r15),#%0000
	jr	lt,L0176
	ldb	rl2,%000B(r15)
	extsb	r2
	ldb	rl2,__ctype+1(r2)
	extsb	r2
	bit	r2,#2
	jr	eq,L01cc
	inc	r12,#1
	jr	L019e
L0270:
	ld	r2,%0000(r15)
	ld	@r2,#%0001
L0278:
	test	%0008(r15)
	jr	eq,L0286
	lda	r2,%000C(r15)
	cp	r14,r2
	jr	ne,L028a
L0286:
	sub	r2,r2
	jr	L02d6
L028a:
	clrb	@r14
	inc	r14,#1
	ld	r2,%0004(r15)
	test	r2
	jr	eq,L02a6
	cp	r2,#%0001
	jr	eq,L02a6
	cp	r2,#%0002
	jr	eq,L02be
L02a2:
	ldk	r2,#1
	jr	L02d6
L02a6:
	lda	r7,%000C(r15)
	call	_atof
	.word	%8F08,%8403
	ld	r2,%0008(r15)
	ld	r2,@r2
	.word	%0F2C,%8001
	jr	L02a2
L02be:
	lda	r7,%000C(r15)
	call	_atof
	.word	%8F08,%8403
	ld	r2,%0008(r15)
	ld	r2,@r2
	.word	%0F2C,%8003
	jr	L02a2
L02d6:
	ldm	r10,%008C(r15),#5
	add	r15,#%0096
	ret
	dec	r15,#14
	ldl	%0002(r15),rr2
	ldm	%0006(r15),r4,#4
	ld	r0,#%0067
	ld	@r15,r0
	ldm	r4,%0006(r15),#4
	ld	r3,%0004(r15)
	ld	r2,%0002(r15)
	calr	__fegcvt
	inc	r15,#14
	ret
	dec	r15,#14
	ldl	%0002(r15),rr2
	ldm	%0006(r15),r4,#4
	ldb	rl0,%0011(r15)
	extsb	r0
	ld	@r15,r0
	ldm	r4,%0006(r15),#4
	ld	r3,%0004(r15)
	ld	r2,%0002(r15)
	calr	__fegcvt
	inc	r15,#14
	ret
__fegcvt::
	sub	r15,#%001A
	ldm	%0012(r15),r11,#4
	ldl	%0002(r15),rr2
	ldm	%0006(r15),r4,#4
	ld	r14,r2
	cp	r3,#%0000
	jr	ge,L0352
	ld	%0004(r15),#%0006
L0352:
	cpb	%001D(r15),#%66
	jr	ne,L0362
	ld	r3,%0004(r15)
	neg	r3
	jr	L0368
L0362:
	ld	r3,%0004(r15)
	inc	r3,#1
L0368:
	ld	r11,r3
	lda	r3,%0010(r15)
	ld	@r15,r3
	lda	r2,%000E(r15)
	ld	r3,r11
	ldm	r4,%0006(r15),#4
	call	_ecvt
	ld	r13,r2
	test	%0010(r15)
	jr	eq,L038e
	ldb	@r14,#%2D
	inc	r14,#1
L038e:
	cpb	@r13,#%39
	jr	le,L03a0
	ld	r7,r14
	ld	r6,r13
	call	_strcpy
	jp	L04f6
L03a0:
	cpb	%001D(r15),#%67
	jr	ne,L0400
	ld	r2,%0004(r15)
	neg	r2
	ld	r1,r2
	exts	rr0
	div	rr0,#%0002
	cp	r1,%000E(r15)
	jr	gt,L0400
	cp	%0004(r15),#%0006
	jr	ge,L03c8
	ldk	r2,#6
	jr	L03cc
L03c8:
	ld	r2,%0004(r15)
L03cc:
	cp	r2,%000E(r15)
	jr	lt,L0400
	ld	r2,%000E(r15)
	add	r2,%0004(r15)
	cp	r2,#%0013
	jr	gt,L0400
	ld	r2,%0004(r15)
	neg	r2
	ld	r11,r2
	lda	r2,%0010(r15)
	ld	@r15,r2
	lda	r2,%000E(r15)
	ld	r3,r11
	ldm	r4,%0006(r15),#4
	call	_ecvt
	ld	r13,r2
L0400:
	cpb	@r13,#%30
	jr	ne,L0450
	testb	%0001(r13)
	jr	ne,L0450
	cpb	%001D(r15),#%67
	jr	ne,L042e
	ldk	r11,#0
L0416:
	ldk	r12,#1
L0418:
	cp	r12,#%0000
	jr	le,L047c
	testb	@r13
	jr	eq,L047c
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	dec	r12,#1
	jr	L0418
L042e:
	cpb	%001D(r15),#%66
	jr	ne,L0440
	ldk	r2,#1
	sub	r2,%0004(r15)
	ld	r12,r2
	jr	L045a
L0440:
	ldk	r2,#1
	sub	r2,%0004(r15)
	ld	r12,r2
	ld	%000E(r15),#%0001
	jr	L045a
L0450:
	cp	r11,#%0000
	jr	gt,L0416
	ld	r12,%000E(r15)
L045a:
	cp	r12,#%0000
	jr	gt,L0418
	ldb	@r14,#%30
	inc	r14,#1
	ldb	@r14,#%2E
	inc	r14,#1
L046c:
	cp	r12,#%0000
	jr	ge,L049e
	ldb	@r14,#%30
	inc	r14,#1
	inc	r12,#1
	jr	L046c
L047c:
	cp	r12,#%0000
	jr	le,L048c
	ldb	@r14,#%30
	inc	r14,#1
	dec	r12,#1
	jr	L047c
L048c:
	ldb	@r14,#%2E
	inc	r14,#1
	jr	L049e
L0494:
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	inc	r12,#1
L049e:
	testb	@r13
	jr	ne,L0494
	cp	r11,#%0000
	jr	le,L04cc
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
	jr	L04f6
L04cc:
	cp	r11,#%0000
	jr	ge,L04f2
	clrb	@r14
	cpb	%001D(r15),#%67
	jr	eq,L04e0
	jr	L04f6
L04de:
	clrb	@r14
L04e0:
	dec	r14,#1
	ldb	rl2,@r14
	cpb	rl2,#%30
	jr	eq,L04de
	cpb	@r14,#%2E
	jr	eq,L04f4
	jr	L04f6
L04f2:
	dec	r14,#1
L04f4:
	clrb	@r14
L04f6:
	ld	r2,%0002(r15)
	ldm	r11,%0012(r15),#4
	add	r15,#%001A
	ret
	.psec
	.data
D0000:
	.byte	%49,%4e,%46,%49,%4e,%49,%54,%59
	.byte	%00
D0009:
	.byte	%4e,%41,%4e,%28,%7e,%7e,%7e,%7e
	.byte	%7e,%7e,%29,%00,%00
