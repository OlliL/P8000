/* libc part of /lib/cparse as one module, cut by host/tools/unlink.py from tools/cparse_libc.spec; the pointer words
   of the data are made symbolic by host/tools/datafix.py (tools/HANDOFF_cparse.md, step 2). Provisional: the split into
   the shipped members and the oldlibc versions is open. */
	.extern	_end
	.comm	512, __sobuf
	.comm	512, __sibuf
	.comm	40, _sigtab
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
	ldb	rl2,D05ed(r2)
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
	calr	L0330
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
	calr	L0330
	inc	r15,#14
	ret
L0330:
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
	call	L1182
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
	call	L1182
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
	call	L192c
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
_fseek::
	dec	r15,#14
	ldm	%000A(r15),r13,#2
	ldm	%0000(r15),r3,#3
	ld	r14,r7
	resb	%0006(r14),#4
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#0
	jr	eq,L05e4
	cp	r3,#%0002
	jr	ge,L05b4
	test	%0004(r14)
	jr	eq,L05b4
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	ne,L05b4
	ld	r13,%0002(r14)
	ldl	%0006(r15),rr4
	test	r3
	jr	ne,L0564
	ldb	rl7,%0007(r14)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ld	r1,r13
	exts	rr0
	subl	rr0,rr2
	addl	rr0,%0006(r15)
	ldl	%0006(r15),rr0
	jr	L0572
L0564:
	ld	r1,r13
	exts	rr0
	ldl	rr6,%0002(r15)
	subl	rr6,rr0
	ldl	%0002(r15),rr6
L0572:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	ne,L05b4
	cp	r13,#%0000
	jr	le,L05b4
	ld	r3,r13
	exts	rr2
	cpl	rr2,%0006(r15)
	jr	lt,L05b4
	ld	r2,%0004(r14)
	sub	r2,@r14
	ld	r1,r2
	exts	rr0
	cpl	rr0,%0006(r15)
	jr	gt,L05b4
	ld	r2,%0008(r15)
	add	r2,@r14
	ld	@r14,r2
	ld	r2,%0008(r15)
	ld	r3,%0002(r14)
	sub	r3,r2
	ld	%0002(r14),r3
	jr	L0636
L05b4:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L05c8
	ld	r2,%0004(r14)
	ld	@r14,r2
	resb	%0006(r14),#0
L05c8:
	ldb	rl7,%0007(r14)
	extsb	r7
	ldl	rr4,%0002(r15)
	ld	r3,%0000(r15)
	call	_lseek
	ldl	%0006(r15),rr2
	clr	%0002(r14)
	jr	L0624
L05e4:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0082
	jr	eq,L0624
	ld	r7,r14
	call	_fflush
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L060e
	clr	%0002(r14)
	resb	%0006(r14),#1
	ld	r2,%0004(r14)
	ld	@r14,r2
L060e:
	ldb	rl7,%0007(r14)
	extsb	r7
	ldl	rr4,%0002(r15)
	ld	r3,%0000(r15)
	call	_lseek
	ldl	%0006(r15),rr2
L0624:
	ldl	rr2,%0006(r15)
	cpl	rr2,#%FFFFFFFF
	jr	ne,L0636
	ld	r2,#%FFFF
	jr	L0638
L0636:
	sub	r2,r2
L0638:
	ldm	r13,%000A(r15),#2
	inc	r15,#14
	ret
_ftell::
	dec	r15,#8
	ld	%0006(r15),r14
	ld	%0000(r15),r7
	ld	r2,r7
	cp	%0002(r2),#%0000
	jr	ge,L065c
	ld	r2,r7
	clr	%0002(r2)
L065c:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#0
	jr	eq,L06c6
	ld	r2,r7
	ld	r2,%0002(r2)
	neg	r2
	ld	r14,r2
	jr	L06a6
L0674:
	ldk	r14,#0
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	bit	r2,#1
	jr	eq,L06a6
	ld	r2,r7
	test	%0004(r2)
	jr	eq,L06a6
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0004
	test	r2
	jr	ne,L06a6
	ld	r2,r7
	ld	r3,r7
	ld	r3,@r3
	sub	r3,%0004(r2)
	ld	r14,r3
L06a6:
	ld	r7,%0000(r15)
	ldb	rl7,%0007(r7)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	call	_lseek
	ldl	%0002(r15),rr2
	cpl	rr2,#%00000000
	jr	ge,L06dc
	jr	L06e8
L06c6:
	ld	r2,r7
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0082
	jr	ne,L0674
	ldl	rr2,#%FFFFFFFF
	jr	L06ec
L06dc:
	ld	r3,r14
	exts	rr2
	addl	rr2,%0002(r15)
	ldl	%0002(r15),rr2
L06e8:
	ldl	rr2,%0002(r15)
L06ec:
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
_fread::
	dec	r15,#14
	ldm	%0008(r15),r12,#3
	ldm	%0000(r15),r5,#2
	ld	r14,r7
	ld	r13,r4
	clr	%0004(r15)
	test	r6
	jr	ne,L0720
	jr	L075a
L0710:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
	dec	%0006(r15),#1
	jr	ne,L0732
	inc	%0004(r15),#1
L0720:
	ld	r2,%0004(r15)
	cp	r2,%0000(r15)
	jr	nc,L075a
	ld	r2,%0002(r15)
	ld	%0006(r15),r2
L0732:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L074c
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	jr	L0752
L074c:
	ld	r7,r13
	call	__filbuf
L0752:
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L0710
L075a:
	ld	r2,%0004(r15)
	ldm	r12,%0008(r15),#3
	inc	r15,#14
	ret
_fwrite::
	dec	r15,#12
	ldm	%0006(r15),r12,#3
	ldm	%0000(r15),r5,#2
	ld	r14,r7
	ld	r13,r4
	clr	%0004(r15)
	test	r6
	jr	ne,L07c8
	jr	L07d2
L0784:
	ld	r12,%0002(r15)
L0788:
	dec	%0002(r13),#1
	cp	%0002(r13),#%0000
	jr	lt,L07a4
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r13
	inc	@r13,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	L07b4
L07a4:
	ld	r7,r14
	inc	r14,#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,r13
	call	__flsbuf
	ld	r0,r2
L07b4:
	djnz	r12,L0788
	ldb	rl2,%0006(r13)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L07d2
	inc	%0004(r15),#1
L07c8:
	ld	r2,%0004(r15)
	cp	r2,%0000(r15)
	jr	c,L0784
L07d2:
	ld	r2,%0004(r15)
	ldm	r12,%0006(r15),#3
	inc	r15,#12
	ret
_fopen::
	dec	r15,#6
	ldl	%0000(r15),rr6
	call	L1856
	ld	%0004(r15),r2
	ld	r7,%0002(r15)
	ld	r6,%0000(r15)
	ld	r5,r2
	call	L16e4
	inc	r15,#6
	ret
_fdopen::
	dec	r15,#6
	ldm	%0002(r15),r13,#2
	ld	%0000(r15),r7
	ld	r14,r6
	call	L1856
	ld	r13,r2
	test	r2
	jr	eq,L086a
	clr	%0002(r13)
	ldb	rl2,%0001(r15)
	ldb	%0007(r13),rl2
	ldb	rl2,@r14
	extsb	r2
	cp	r2,#%0061
	jr	eq,L086e
	cp	r2,#%0072
	jr	ne,L0864
	setb	%0006(r13),#0
L0838:
	cpb	%0001(r14),#%2B
	jr	eq,L0848
	cpb	%0001(r14),#%77
	jr	ne,L0860
L0848:
	ldb	rl2,%0006(r13)
	andb	rl2,#%FC
	ldb	%0006(r13),rl2
	ldb	rl2,%0006(r13)
	orb	rl2,#%80
	ldb	%0006(r13),rl2
L0860:
	ld	r2,r13
	jr	L0880
L0864:
	cp	r2,#%0077
	jr	eq,L087a
L086a:
	sub	r2,r2
	jr	L0880
L086e:
	ld	r7,%0000(r15)
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
L087a:
	setb	%0006(r13),#1
	jr	L0838
L0880:
	ldm	r13,%0002(r15),#2
	inc	r15,#6
	ret
_printf::
	sub	r15,#%0016
	ldm	%0000(r15),r2,#6
	ld	%000C(r15),r6
	ld	%000E(r15),r5
	ld	%0010(r15),r4
	ld	%0012(r15),r3
	ld	%0014(r15),r2
	lda	r6,%000C(r15)
	lda	r5,%0018(r15)
	ld	r7,%000A(r15)
	sub	r4,r4
	ld	r3,#D01c4
	call	L09f2
	ldb	rl2,D01ca
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L08d2
	ld	r2,#%FFFF
	jr	L08d4
L08d2:
	sub	r2,r2
L08d4:
	add	r15,#%0016
	ret
_fprintf::
	sub	r15,#%0014
	ldm	%0000(r15),r2,#6
	ld	%000C(r15),r5
	ld	%000E(r15),r4
	ld	%0010(r15),r3
	ld	%0012(r15),r2
	lda	r6,%000C(r15)
	lda	r5,%0016(r15)
	ld	r7,%0008(r15)
	ldk	r4,#1
	ld	r3,%000A(r15)
	call	L09f2
	ld	r2,%000A(r15)
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L0922
	ld	r2,#%FFFF
	jr	L0924
L0922:
	sub	r2,r2
L0924:
	add	r15,#%0014
	ret
_sprintf::
	sub	r15,#%001C
	ldm	%0000(r15),r2,#6
	ldb	%0012(r15),#%42
	ld	%000C(r15),r7
	ld	%000E(r15),#%7FFF
	ld	%0014(r15),r5
	ld	%0016(r15),r4
	ld	%0018(r15),r3
	ld	%001A(r15),r2
	lda	r6,%0014(r15)
	lda	r5,%001E(r15)
	lda	r3,%000C(r15)
	ldk	r4,#1
	ld	r7,%0008(r15)
	call	L09f2
	dec	%000E(r15),#1
	cp	%000E(r15),#%0000
	jr	lt,L0986
	ld	r2,%000C(r15)
	inc	%000C(r15),#1
	clrb	@r2
	subb	rl2,rl2
	extsb	r2
	jr	L0990
L0986:
	lda	r6,%000C(r15)
	sub	r7,r7
	call	__flsbuf
L0990:
	ld	r2,%000A(r15)
	add	r15,#%001C
	ret
L099a:
	dec	r15,#8
	ld	%0000(r15),r5
	ld	r2,r7
	test	@r2
	jr	eq,L09ae
	ld	r2,r7
	cp	@r2,#%0002
	jr	ne,L09b8
L09ae:
	ld	r2,r7
	inc	@r2,#1
	ld	r2,r6
	inc	@r2,#2
	jr	L09ca
L09b8:
	ld	r2,r7
	cp	@r2,#%0004
	jr	ne,L09ca
	ld	r2,r7
	inc	@r2,#2
	ld	r2,r6
	ld	@r2,r5
	jr	L09ee
L09ca:
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
L09ee:
	inc	r15,#8
	ret
L09f2:
	sub	r15,#%0068
	ldm	%0060(r15),r11,#4
	ldm	%0002(r15),r3,#4
	ld	r14,r7
	ld	r2,%0008(r15)
	ld	%000A(r15),r2
	call	L1abc
	ld	r12,r2
	jr	L0a32
L0a14:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	ge,L0e42
L0a28:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
L0a32:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0f3e
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%0025
	jr	ne,L0a14
	test	r12
	jp	eq,L0f3e
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
	jr	ne,L0a8c
	inc	%0058(r15),#1
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L0f3e
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L0a8c:
	cp	r13,#%0061
	jr	lt,L0b3a
	jr	L0b52
L0a94:
	cp	r13,#%002A
	jr	ne,L0abe
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000C(r15),r3
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0ade
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L0ade
L0abe:
	cp	r13,#%0030
	jr	ne,L0aca
	ldb	%005D(r15),#%30
L0aca:
	lda	r5,%000C(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	L0f4a
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L0ade:
	test	r12
	jr	eq,L0b52
	cpb	@r14,#%2E
	jr	ne,L0b2c
	inc	r14,#1
	dec	r12,#1
	test	r12
	jr	eq,L0b52
	cpb	@r14,#%2A
	jr	ne,L0b1c
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%000E(r15),r3
	inc	%000A(r15),#2
	ldk	r11,#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0b28
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jr	L0b28
L0b1c:
	lda	r5,%000E(r15)
	ld	r6,r12
	ld	r7,r14
	calr	L0f4a
	ld	r11,r2
L0b28:
	sub	r12,r11
	add	r14,r11
L0b2c:
	test	r12
	jr	eq,L0b52
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L0b3a:
	cp	r13,#%002A
	jr	eq,L0a94
	cp	r13,#%0030
	jr	lt,L0b4c
	cp	r13,#%0039
	jr	le,L0a94
L0b4c:
	cp	r13,#%002E
	jr	eq,L0a94
L0b52:
	cp	%000E(r15),#%0000
	jr	ge,L0b74
	ld	r2,r13
	ld	r3,#D001e
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,L0b74
	ld	r2,%0006(r3)
	jp	@r2
L0b70:
	clr	%000E(r15)
L0b74:
	ld	%005E(r15),r13
	ld	r2,r13
	ld	r3,#D002e
	ld	r4,#%000E
	cpir	r2,@r3,r4,eq
	jp	ne,L0e2e
	ld	r2,%001A(r3)
	jp	@r2
L0b90:
	ld	%000E(r15),#%0006
	jr	L0b74
L0b98:
	ld	%0010(r15),#%0008
	jr	L0bae
L0ba0:
	ld	%0010(r15),#%0010
	jr	L0bae
L0ba8:
	ld	%0010(r15),#%000A
L0bae:
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	clr	%0012(r15)
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	eq,L0d00
	jp	L0d08
L0bd6:
	test	r12
	jr	eq,L0ca2
	dec	r12,#1
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	subb	rh2,rh2
	ld	r13,r2
	cp	r2,#%006F
	jr	ne,L0c3c
L0bec:
	ld	%0010(r15),#%0008
	jr	L0c0a
L0bf4:
	ld	%0010(r15),#%0010
	jr	L0c0a
L0bfc:
	ld	%0010(r15),#%FFF6
	jr	L0c0a
L0c04:
	ld	%0010(r15),#%000A
L0c0a:
	cp	%0004(r15),#%0005
	jr	ge,L0c20
	lda	r7,%0004(r15)
	lda	r6,%000A(r15)
	ld	r5,%0006(r15)
	calr	L099a
L0c20:
	ld	r2,%000A(r15)
	ldl	rr0,@r2
	ldl	%0012(r15),rr0
	inc	%000A(r15),#4
	inc	%0004(r15),#2
	cp	%0004(r15),#%0005
	jr	eq,L0d00
	jr	L0d08
L0c3c:
	cp	r13,#%0078
	jr	eq,L0bf4
	cp	r13,#%0064
	jr	eq,L0bfc
	cp	r13,#%0075
	jr	eq,L0c04
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0c74
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%6C
	ldb	rl2,#%6C
	extsb	r2
	jr	L0c80
L0c74:
	ld	r7,#%006C
	ld	r6,%0002(r15)
	call	__flsbuf
L0c80:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0cd0
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
L0ca2:
	test	%000E(r15)
	jr	eq,L0cba
	cp	%005E(r15),#%0073
	jr	ne,L0cba
	cp	r11,%000E(r15)
	jr	le,L0cba
	ld	r11,%000E(r15)
L0cba:
	cp	r11,%000C(r15)
	jp	ge,L0e56
	ld	r2,%000C(r15)
	sub	r2,r11
	ld	%005A(r15),r2
	jp	L0e5a
L0cd0:
	ld	r7,r13
	ld	r6,%0002(r15)
	call	__flsbuf
	jr	L0ca2
L0cdc:
	ld	%0010(r15),#%FFF6
	ld	r2,%000A(r15)
	ld	r2,@r2
	ld	r1,r2
	exts	rr0
	ldl	%0012(r15),rr0
	inc	%000A(r15),#2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	ne,L0d08
L0d00:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L0d08:
	lda	r2,%0018(r15)
	ld	%0016(r15),r2
	cp	%0010(r15),#%FFF6
	jr	ne,L0d72
	ldl	rr2,%0012(r15)
	cpl	rr2,#%00000000
	jr	ge,L0d72
	cpl	rr2,#%80000000
	jr	ne,L0d32
	ld	%0010(r15),#%000A
L0d32:
	ldl	rr6,%0012(r15)
	multl	rq4,#%FFFFFFFF
	ldl	%0012(r15),rr6
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	lt,L0d66
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	@r2,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L0d72
L0d66:
	ld	r7,#%002D
	ld	r6,%0002(r15)
	call	__flsbuf
L0d72:
	ld	r7,%0016(r15)
	ld	r6,%0010(r15)
	ldl	rr4,%0012(r15)
	call	L192c
L0d82:
	ld	r11,r2
	jr	L0ca2
L0d86:
	ldb	%005D(r15),#%20
	ld	r2,%000A(r15)
	ld	r3,@r2
	ld	%0016(r15),r3
	inc	%000A(r15),#2
	test	r3
	jr	ne,L0da4
	ld	%0016(r15),#D0016
L0da4:
	ld	r7,%0016(r15)
	call	L1abc
	ld	r11,r2
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jp	ne,L0ca2
L0dbc:
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
	jp	L0ca2
L0dc8:
	ldb	%005D(r15),#%20
	ldk	r11,#1
	ld	r2,%000A(r15)
	ld	%0016(r15),r2
	inc	%000A(r15),#2
	inc	%0016(r15),#1
	inc	%0004(r15),#1
	cp	%0004(r15),#%0005
	jr	eq,L0dbc
	jp	L0ca2
L0df0:
	cp	%0004(r15),#%0005
	jr	ge,L0e00
	ld	r2,%0006(r15)
	ld	%000A(r15),r2
L0e00:
	ld	@r15,r13
	lda	r2,%0018(r15)
	ld	r3,%000A(r15)
	.word	%0F34,%8003
	.word	%8F00,%8403
	ld	r3,%000E(r15)
	call	L0330
	ld	%0016(r15),r2
	inc	%000A(r15),#8
	inc	%0004(r15),#5
	ld	r7,r2
	call	L1abc
	jr	L0d82
L0e2e:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jp	lt,L0a28
L0e42:
	ld	r2,r13
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	@r3,rl2
	extsb	r2
	jp	L0a32
L0e56:
	clr	%005A(r15)
L0e5a:
	test	%0058(r15)
	jr	eq,L0e98
	jr	L0ee8
L0e62:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
	jr	L0e98
L0e76:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L0e62
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
L0e98:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jr	ne,L0e76
	jr	L0ee8
L0ea6:
	ld	r2,%0016(r15)
	inc	%0016(r15),#1
	ld	r3,%0002(r15)
	inc	@r3,#1
	ld	r3,@r3
	dec	r3,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	extsb	r0
	jr	L0ee8
L0ec0:
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L0ea6
	ld	r7,%0016(r15)
	inc	%0016(r15),#1
	ldb	rl7,@r7
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r0,r2
L0ee8:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0ec0
	test	%0058(r15)
	jr	ne,L0f0c
	jp	L0a32
L0efa:
	ld	r2,%0002(r15)
	inc	@r2,#1
	ld	r2,@r2
	dec	r2,#1
	ldb	rl3,%005D(r15)
	ldb	@r2,rl3
	extsb	r3
L0f0c:
	ld	r2,%005A(r15)
	dec	%005A(r15),#1
	test	r2
	jp	eq,L0a32
	ld	r2,%0002(r15)
	dec	%0002(r2),#1
	ld	r2,%0002(r2)
	cp	r2,#%0000
	jr	ge,L0efa
	ldb	rl7,%005D(r15)
	extsb	r7
	ld	r6,%0002(r15)
	call	__flsbuf
	ld	r3,r2
	jr	L0f0c
L0f3e:
	ldm	r11,%0060(r15),#4
	add	r15,#%0068
	ret
L0f4a:
	dec	r15,#14
	ldm	%0000(r15),r5,#2
	ld	r1,r7
	clr	%0004(r15)
	ld	r4,r6
	test	r6
	jr	eq,L0f70
	cpb	@r1,#%2D
	jr	ne,L0f70
	dec	%0002(r15),#1
	inc	r1,#1
	ld	%0004(r15),#%0001
L0f70:
	ldk	r5,#0
	jr	L0fa0
L0f74:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%0007(r15),rl3
	cpb	rl3,#%30
	jr	lt,L0fa4
	cpb	rl3,#%39
	jr	gt,L0fa4
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%0007(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L0fa0:
	test	r4
	jr	ne,L0f74
L0fa4:
	test	%0004(r15)
	jr	eq,L0fb0
	ld	r2,r5
	neg	r2
	jr	L0fb2
L0fb0:
	ld	r2,r5
L0fb2:
	ld	r3,%0000(r15)
	ld	@r3,r2
	ld	r2,%0002(r15)
	sub	r2,r4
	inc	r15,#14
	ret
_ungetc::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	r14,r6
	cp	r7,#%FFFF
	jr	eq,L1006
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,L0fe6
	ld	r2,@r14
	cp	r2,%0004(r14)
	jr	ugt,L0ff6
L0fe6:
	ld	r2,@r14
	cp	r2,%0004(r14)
	jr	ne,L1006
	test	%0002(r14)
	jr	ne,L1006
	inc	@r14,#1
L0ff6:
	inc	%0002(r14),#1
	ld	r2,r7
	dec	@r14,#1
	ld	r3,@r14
	ldb	@r3,rl2
	sub	r2,r2
	jr	L100a
L1006:
	ld	r2,#%FFFF
L100a:
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
_atofs::
	calr	L1022
	.word	%8F00,%8601
	ret
_atof::
	calr	L1022
	.word	%8F00,%8403
	ret
L1022:
	dec	r15,#12
	ldm	@r15,r8,#6
	ldl	rr12,rr6
	subl	rr0,rr0
	ldl	rr2,rr0
	ldl	rr4,rr2
	ldl	rr6,rr4
	ldl	rr8,rr6
	ldb	rh2,#%13
	dec	r13,#1
L1038:
	inc	r13,#1
	ldb	rl0,@r13
	cpb	rl0,#%20
	jr	eq,L1038
	cpb	rl0,#%09
	jr	eq,L1038
	ld	r1,#%0013
L104c:
	ldb	rl0,@r13
	inc	r13,#1
	lda	r11,D0097
	ld	r8,#%0012
	cpdrb	rl0,@r11,r8,eq
	ldb	rl1,r11(r1)
	ld	r11,D0066(r1)
	jp	@r11
L1066:
	ldk	r8,#3
	lda	r11,D0116
	cpsirb	@r13,@r11,r8,ne
	jr	eq,L1094
L1072:
	ldb	rl0,@r13
	inc	r13,#1
	orb	rl0,rl0
	jr	eq,L1094
	cpb	rl0,#%29
	jr	eq,L1094
	rldb	rh6,rl6
	rldb	rl6,rh7
	rldb	rh7,rl7
	bitb	rl0,#6
	jr	eq,L108c
	incb	rl0,#9
L108c:
	andb	rl0,#%0F
	orb	rl7,rl0
	jr	L1072
L1094:
	testl	rr6
	jr	ne,L109a
	set	r6,#6
L109a:
	or	r6,#%7F80
L109e:
	or	r6,r3
	.word	%8F08,%8601
	jr	L117a
L10a6:
	ldb	rl1,#%49
	jr	L104c
L10aa:
	setb	rh3,#7
L10ac:
	ldb	rl1,#%25
	jr	L104c
L10b0:
	ldb	rl1,#%25
L10b2:
	ldb	rl0,@r13
	cpb	rl0,#%30
	jr	ne,L104c
	inc	r13,#1
	jr	L10b2
L10be:
	ldb	rl1,#%37
	decb	rh2,#1
	jr	ge,L10f4
	incb	rl2,#1
	jr	L104c
L10c8:
	ldb	rl1,#%5B
	jr	L104c
L10cc:
	ldb	rl1,#%6D
	jr	L104c
L10d0:
	decb	rl2,#1
	ldb	rl1,#%49
	jr	L104c
L10d6:
	ldb	rl1,#%5B
	decb	rh2,#1
	jr	lt,L104c
	decb	rl2,#1
	jr	L10f4
L10e0:
	setb	rh3,#6
L10e2:
	ldb	rl1,#%7F
	jr	L104c
L10e6:
	mult	rr8,#%000A
	and	r0,#%000F
	add	r9,r0
	ldb	rl1,#%7F
	jr	L104c
L10f4:
	calr	L10fe
	andb	rl0,#%0F
	orb	rl7,rl0
	jr	L104c
L10fe:
	rldb	rh3,rl3
	rldb	rl3,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	rldb	rl5,rh6
	rldb	rh6,rl6
	rldb	rl6,rh7
	rldb	rh7,rl7
	ret
L1112:
	rrdb	rh7,rl7
	rrdb	rl6,rh7
	rrdb	rh6,rl6
	rrdb	rl5,rh6
	rrdb	rh5,rl5
	rrdb	rl4,rh5
	rrdb	rh4,rl4
	rrdb	rl3,rh4
	rrdb	rh3,rl3
	ret
L1126:
	bitb	rh3,#6
	resb	rh3,#6
	jr	eq,L112e
	neg	r9
L112e:
	extsb	r2
	add	r9,r2
	jr	eq,L1150
	jr	mi,L1144
L1136:
	ldb	rl0,#%0F
	andb	rl0,rh3
	jr	ne,L1150
	calr	L10fe
	dec	r9,#1
	jr	ne,L1136
	jr	L1150
L1144:
	ldb	rl0,#%0F
	andb	rl0,rl7
	jr	ne,L1150
	calr	L1112
	inc	r9,#1
	jr	ne,L1144
L1150:
	dec	r15,#10
	ldm	@r15,r3,#5
	ld	r7,r9
	bit	r7,#15
	jr	eq,L115e
	neg	r7
L115e:
	call	L1364
	.word	%0FF4,%8114
	inc	r15,#10
	bit	r9,#15
	jr	eq,L1176
	.word	%8E14,%40A0
	.word	%8E04,%8100
	jr	L117a
L1176:
	.word	%8E04,%41B0
L117a:
	ldm	r8,@r15,#6
	inc	r15,#12
	ret
L1182:
	.word	%8F38,%8403
	ld	r7,r3
	ld	r6,r2
	ld	r5,r15(#%0002)
	call	L11a8
	ret
	.word	%8F38,%8403
	ld	r7,r3
	neg	r7
	ld	r6,r2
	ld	r5,r15(#%0002)
	call	L11a8
	ret
L11a8:
	dec	r15,#12
	ldm	@r15,r8,#6
	ld	r3,r5
	ld	r5,r6
	clr	@r3
	clr	@r5
	.word	%8F30,%8804
	bit	r8,#15
	jr	eq,L11c4
	set	@r3,#0
	.word	%8E34,%8380
L11c4:
	res	r8,#15
	cp	r8,#%7FFF
	jr	ne,L1226
	or	r9,r10
	or	r9,r11
	or	r9,r12
	jr	ne,L11e6
	lda	r3,D011a
	lda	r5,D013a
	ldk	r1,#4
	ldirb	@r3,@r5,r1
	jp	L1342
L11e6:
	lda	r3,D011a
	lda	r5,D013e
	ldk	r1,#4
	ldirb	@r3,@r5,r1
	.word	%8F30,%8401
	res	r4,#7
	ldk	r1,#6
L11fc:
	subb	rh4,rh4
	rldb	rh4,rl4
	rldb	rl4,rh5
	rldb	rh5,rl5
	cpb	rh4,#%09
	jr	gt,L1210
	orb	rh4,#%30
	jr	L1214
L1210:
	addb	rh4,#%36
L1214:
	ldb	@r3,rh4
	inc	r3,#1
	djnz	r1,L11fc
	ldb	@r3,#%29
	inc	r3,#1
	clrb	@r3
	jp	L1342
L1226:
	.word	%8E30,%0010
	jr	ne,L1238
L122c:
	ld	r2,#%3000
	ld	D011a,r2
	jp	L1342
L1238:
	cp	r7,#%0013
	jp	le,L1244
	ld	r7,#%0013
L1244:
	pushl	@r15,rr4
	dec	r15,#10
	.word	%0FFC,%8504
	dec	r15,#10
	.word	%0FFC,%8404
	.word	%8E44,%8300
	sub	r6,r6
	cp	r7,#%0000
	jr	le,L1278
	.word	%8F30,%8004
	sub	r0,#%3FFF
	bit	r1,#15
	jr	ne,L126c
	sub	r1,r1
L126c:
	add	r1,r1
	ldl	rr2,#%4D104D42
	multl	rq0,rr0
	ld	r6,r0
L1278:
	ld	r5,r7
	cp	r5,#%0000
	jr	gt,L1286
	neg	r5
	ld	r4,r5
	jr	L1290
L1286:
	sub	r5,r6
	dec	r5,#1
	ld	r4,r5
	jr	pl,L1290
	neg	r4
L1290:
	pushl	@r15,rr6
	push	@r15,r5
	ld	r7,r4
	call	L1364
	.word	%8E14,%8400
	pop	r5,@r15
	bit	r5,#15
	jr	ne,L12aa
	.word	%8E14,%40B0
	jr	L12ae
L12aa:
	.word	%8E14,%40A0
L12ae:
	.word	%8E14,%81D0
	.word	%8E54,%8100
	popl	rr6,@r15
	cp	r7,#%0000
	jr	le,L12de
	pushl	@r15,rr6
	call	L1364
	popl	rr6,@r15
	.word	%8E50,%4010
	jr	c,L12de
	jr	eq,L12d2
	inc	r6,#1
	jr	L1278
L12d2:
	inc	r6,#1
	ldl	rr0,#%41200000
	.word	%8F58,%40A1
L12de:
	.word	%8E04,%8500
	.word	%0FF4,%8404
	inc	r15,#10
	.word	%0FF4,%8504
	inc	r15,#10
	.word	%8F00,%8014
	andb	rh0,#%0F
	lda	r11,D011a
	popl	rr8,@r15
	inc	r6,#1
	ld	@r9,r6
	cp	r7,#%0000
	jr	gt,L1328
	.word	%8E00,%0010
	jr	eq,L122c
	sub	r6,r6
L130e:
	orb	rh0,rh0
	jr	ne,L1318
	calr	L1350
	inc	r6,#1
	jr	L130e
L1318:
	sub	r6,#%0013
	sub	r7,r6
	ld	@r9,r7
	ld	r7,r6
	neg	r7
	sub	r6,r6
	jr	L132e
L1328:
	ld	r6,r7
	sub	r6,#%0013
L132e:
	orb	rh0,#%30
L1332:
	inc	r6,#1
	jr	le,L133a
	ldb	@r11,rh0
	inc	r11,#1
L133a:
	calr	L1350
	cp	r6,r7
	jr	lt,L1332
	clrb	@r11
L1342:
	lda	r3,D011a
	ld	r2,r3
	ldm	r8,@r15,#6
	inc	r15,#12
	ret
L1350:
	rldb	rh0,rl0
	rldb	rl0,rh1
	rldb	rh1,rl1
	rldb	rl1,rh2
	rldb	rh2,rl2
	rldb	rl2,rh3
	rldb	rh3,rl3
	rldb	rl3,rh4
	rldb	rh4,rl4
	ret
L1364:
	dec	r15,#4
	.word	%0FFC,%C401
	.word	%8E04,%C640
	ldl	rr2,#%3F800000
	cp	r7,#%1388
	jr	c,L1384
	set	r2,#14
	.word	%8F08,%8201
	inc	r15,#4
	ret
L1384:
	.word	%8F08,%8201
	cp	r7,#%001B
	jr	le,L13a4
	lda	r3,D0148
	ldk	r1,#8
L1394:
	pop	r0,@r3
	cp	r7,r0
	jr	c,L13a0
	.word	%0F34,%40B4
	sub	r7,r0
L13a0:
	inc	r3,#10
	djnz	r1,L1394
L13a4:
	ldl	rr2,#%3F800000
	.word	%8F18,%8201
	ld	r2,#%4120
	.word	%8F28,%8201
	or	r7,r7
	jr	eq,L13cc
L13ba:
	bit	r7,#0
	jr	eq,L13c2
	.word	%8E14,%42B0
L13c2:
	.word	%8E24,%42B0
	res	r7,#0
	rr	r7,#1
	jr	ne,L13ba
L13cc:
	.word	%0FF4,%C401
	inc	r15,#4
	.word	%8E04,%41B0
	ret
__filbuf::
	dec	r15,#2
	ld	%0000(r15),r14
	ld	r14,r7
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L13ee
	setb	%0006(r14),#0
L13ee:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,L1406
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#6
	jr	eq,L1420
L1406:
	ld	r2,#%FFFF
	jr	L14aa
L140c:
	ld	r7,#%0200
	call	_malloc
	ld	%0004(r14),r2
	test	r2
	jr	ne,L1440
	setb	%0006(r14),#2
L1420:
	test	%0004(r14)
	jr	ne,L1444
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	eq,L140c
	ldb	rl2,%0007(r14)
	extsb	r2
	add	r2,#D01a8
	ld	%0004(r14),r2
	jr	L1420
L1440:
	setb	%0006(r14),#3
L1444:
	ld	r2,%0004(r14)
	ld	@r14,r2
	ldb	rl5,%0006(r14)
	extsb	r5
	bit	r5,#2
	jr	eq,L1458
	ldk	r5,#1
	jr	L145c
L1458:
	ld	r5,#%0200
L145c:
	ld	r6,@r14
	ldb	rl7,%0007(r14)
	extsb	r7
	call	_read
	ld	%0002(r14),r2
	dec	%0002(r14),#1
	cp	%0002(r14),#%0000
	jr	ge,L149e
	cp	%0002(r14),#%FFFF
	jr	ne,L1494
	setb	%0006(r14),#4
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L1498
	resb	%0006(r14),#0
	jr	L1498
L1494:
	setb	%0006(r14),#5
L1498:
	clr	%0002(r14)
	jr	L1406
L149e:
	ld	r2,@r14
	ldb	rl2,@r2
	extsb	r2
	and	r2,#%00FF
	inc	@r14,#1
L14aa:
	ld	r14,%0000(r15)
	inc	r15,#2
	ret
_setbuf::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	%0000(r15),r6
	ld	r14,r7
	test	%0004(r14)
	jr	eq,L14d6
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#3
	jr	eq,L14d6
	ld	r7,%0004(r14)
	call	_free
L14d6:
	ldb	rl2,%0006(r14)
	andb	rl2,#%F3
	ldb	%0006(r14),rl2
	ld	r2,%0000(r15)
	ld	%0004(r14),r2
	test	r2
	jr	ne,L14f4
	setb	%0006(r14),#2
	jr	L14fa
L14f4:
	ld	r2,%0004(r14)
	ld	@r14,r2
L14fa:
	clr	%0002(r14)
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
__flsbuf::
	dec	r15,#12
	ldm	%0004(r15),r11,#4
	ld	%0000(r15),r7
	ld	r14,r6
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#7
	jr	eq,L1546
	setb	%0006(r14),#1
	resb	%0006(r14),#4
	jr	L1546
L1528:
	ldb	rl7,D01cb
	extsb	r7
	call	_isatty
	test	r2
	jr	eq,L153c
L1536:
	setb	%0006(r14),#2
	jr	L1546
L153c:
	ld	%0004(r14),#__sobuf
	ld	@r14,#__sobuf
L1546:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#2
	jr	eq,L1572
	ldb	rl2,%0001(r15)
	ldb	%0003(r15),rl2
	ldk	r11,#1
	lda	r6,%0003(r15)
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r5,r11
	call	_write
	ld	r12,r2
	clr	%0002(r14)
	jr	L15cc
L1572:
	ld	r13,%0004(r14)
	test	r13
	jr	ne,L159c
	cp	r14,#D01c4
	jr	eq,L1528
	ld	r7,#%0200
	call	_malloc
	ld	r13,r2
	ld	%0004(r14),r2
	test	r2
	jr	eq,L1536
	setb	%0006(r14),#3
	ldk	r12,#0
	ld	r11,r12
	jr	L15bc
L159c:
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	ld	r11,r2
	cp	r2,#%0000
	jr	le,L15bc
	ld	@r14,r13
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	call	_write
	ld	r12,r2
L15bc:
	ld	%0002(r14),#%01FF
	ldb	rl2,%0001(r15)
	ldb	@r13,rl2
	inc	r13,#1
	ld	@r14,r13
L15cc:
	cp	r11,r12
	jr	eq,L15da
	setb	%0006(r14),#5
	ld	r2,#%FFFF
	jr	L15de
L15da:
	ld	r2,%0000(r15)
L15de:
	ldm	r11,%0004(r15),#4
	inc	r15,#12
	ret
_fflush::
	dec	r15,#6
	ldm	%0000(r15),r12,#3
	ld	r14,r7
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0006
	cp	r2,#%0002
	jr	ne,L163a
	ld	r13,%0004(r14)
	test	r13
	jr	eq,L163a
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	cp	r2,#%0000
	jr	le,L163a
	ld	@r14,r13
	ld	%0002(r14),#%0200
	ldb	rl7,%0007(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	call	_write
	cp	r12,r2
	jr	eq,L163a
	setb	%0006(r14),#5
	ld	r2,#%FFFF
	jr	L163c
L163a:
	sub	r2,r2
L163c:
	ldm	r12,%0000(r15),#3
	inc	r15,#6
	ret
__cleanup::
	dec	r15,#2
	ld	%0000(r15),r14
	ld	r14,#__iob
	jr	L1658
L1652:
	ld	r7,r14
	calr	_fclose
	inc	r14,#8
L1658:
	cp	r14,D025c
	jr	c,L1652
	ld	r14,%0000(r15)
	inc	r15,#2
	ret
_fclose::
	dec	r15,#4
	ldm	%0000(r15),r13,#2
	ld	r14,r7
	ld	r13,#%FFFF
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0083
	jr	eq,L16c8
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0040
	test	r2
	jr	ne,L16c8
	calr	_fflush
	ld	r13,r2
	ldb	rl7,%0007(r14)
	extsb	r7
	call	_close
	cp	r2,#%0000
	jr	ge,L16a6
	ld	r13,#%FFFF
L16a6:
	ldb	rl2,%0006(r14)
	extsb	r2
	bit	r2,#3
	jr	eq,L16b8
	ld	r7,%0004(r14)
	call	_free
L16b8:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%000C
	jr	eq,L16c8
	clr	%0004(r14)
L16c8:
	ldb	rl2,%0006(r14)
	andb	rl2,#%00
	ldb	%0006(r14),rl2
	clr	%0002(r14)
	ld	r2,r13
	ldm	r13,%0000(r15),#2
	inc	r15,#4
	ret
L16e4:
	dec	r15,#10
	ldm	%0004(r15),r12,#3
	ldl	%0000(r15),rr6
	ld	r14,r5
	test	r5
	jr	ne,L16fa
L16f6:
	sub	r2,r2
	jr	L17e2
L16fa:
	ld	r2,r6
	cpb	@r2,#%72
	jr	ne,L170c
	ld	r2,r6
	cpb	%0001(r2),#%77
	jr	eq,L171e
L170c:
	ld	r2,r6
	cpb	@r2,#%77
	jr	ne,L172c
	ld	r2,r6
	cpb	%0001(r2),#%72
	jr	ne,L172c
L171e:
	ld	r2,r6
	ldb	@r2,#%72
	ld	r2,r6
	ldb	%0001(r2),#%2B
L172c:
	ld	r2,r6
	cpb	%0001(r2),#%2B
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
	ld	r13,r2
	ld	r2,r6
	ldb	rl2,@r2
	extsb	r2
	cp	r2,#%0061
	jr	eq,L175e
	cp	r2,#%0072
	jr	eq,L1796
	cp	r2,#%0077
	jr	ne,L16f6
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	L17ec
	jr	L17a8
L175e:
	test	r13
	jr	eq,L1766
	ldk	r6,#2
	jr	L1768
L1766:
	ldk	r6,#1
L1768:
	ld	r7,%0002(r15)
	call	_open
	ld	r12,r2
	cp	r2,#%0000
	jr	ge,L178a
	cp	D06f0,#%0002
	jr	ne,L178a
	ld	r7,%0002(r15)
	ld	r6,r13
	calr	L17ec
	ld	r12,r2
L178a:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
	jr	L17aa
L1796:
	test	r13
	jr	eq,L179e
	ldk	r6,#2
	jr	L17a0
L179e:
	sub	r6,r6
L17a0:
	ld	r7,%0002(r15)
	call	_open
L17a8:
	ld	r12,r2
L17aa:
	cp	r12,#%0000
	jr	lt,L16f6
	clr	%0002(r14)
	ld	r2,r12
	ldb	%0007(r14),rl2
	test	r13
	jr	eq,L17cc
	ldb	rl2,%0006(r14)
	orb	rl2,#%80
	ldb	%0006(r14),rl2
	jr	L17e0
L17cc:
	ld	r2,%0000(r15)
	cpb	@r2,#%72
	jr	ne,L17dc
	setb	%0006(r14),#0
	jr	L17e0
L17dc:
	setb	%0006(r14),#1
L17e0:
	ld	r2,r14
L17e2:
	ldm	r12,%0004(r15),#3
	inc	r15,#10
	ret
L17ec:
	dec	r15,#8
	ldm	%0002(r15),r12,#3
	ld	%0000(r15),r6
	ld	r14,r7
	test	r6
	jr	eq,L183e
	sub	r7,r7
	call	_umask
	ld	r13,r2
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
	cp	r2,#%0000
	jr	lt,L1836
	ld	r7,r2
	call	_close
	ld	r7,r14
	ldk	r6,#2
	call	_open
	ld	r12,r2
	ld	r6,r13
	com	r6
	and	r6,#%01B6
	ld	r7,r14
	call	_chmod
L1836:
	ld	r7,r13
	call	_umask
	jr	L184a
L183e:
	ld	r7,r14
	ld	r6,#%01B6
	call	_creat
	ld	r12,r2
L184a:
	ld	r2,r12
	ldm	r12,%0002(r15),#3
	inc	r15,#8
	ret
L1856:
	dec	r15,#2
	ld	%0000(r15),r14
	ld	r14,#__iob
	jr	L186e
L1862:
	cp	r14,D025c
	jr	c,L186c
	sub	r2,r2
	jr	L187c
L186c:
	inc	r14,#8
L186e:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0083
	jr	ne,L1862
	ld	r2,r14
L187c:
	ld	r14,%0000(r15)
	inc	r15,#2
	ret
__callex::
	call	_exit
	ret
_exit::
	ld	r8,r7
	call	__cleanup
	ld	r0,r8
	sc	#%01
_perror::
	dec	r15,#8
	ldm	%0004(r15),r13,#2
	ld	%0000(r15),r7
	ld	r14,#D025e
	cp	D06f0,#%0005
	jr	eq,L18b4
	cp	D06f0,#%0006
	jr	ne,L18cc
L18b4:
	test	D06f2
	jr	eq,L18cc
	ld	r2,D06f2
	cp	r2,D02dc
	jr	ge,L18dc
	add	r2,r2
	ld	r14,D02cc(r2)
	jr	L18dc
L18cc:
	ld	r2,D06f0
	cp	r2,D02ca
	jr	ge,L18dc
	add	r2,r2
	ld	r14,D027e(r2)
L18dc:
	ld	r7,%0000(r15)
	call	_strlen
	ld	r13,r2
	test	r2
	jr	eq,L1902
	ldk	r7,#2
	ld	r6,%0000(r15)
	ld	r5,r2
	call	_write
	ldk	r7,#2
	ld	r6,#D026c
	ld	r5,r7
	call	_write
L1902:
	ld	r7,r14
	call	_strlen
	ld	%0002(r15),r2
	ldk	r7,#2
	ld	r6,r14
	ld	r5,r2
	call	_write
	ldk	r7,#2
	ld	r6,#D026f
	ldk	r5,#1
	call	_write
	ldm	r13,%0004(r15),#2
	inc	r15,#8
	ret
L192c:
	sub	r15,#%001E
	ldm	%001A(r15),r13,#2
	ldm	%0000(r15),r4,#4
	ld	r13,r7
	cp	r6,#%FFF6
	jr	ne,L1962
	ld	%0004(r15),#%000A
	cpl	rr4,#%00000000
	jr	ge,L1962
	ldb	@r13,#%2D
	inc	r13,#1
	subl	rr2,rr2
	subl	rr2,%0000(r15)
	ldl	%0000(r15),rr2
L1962:
	lda	r2,%000C(r15)
	ld	r14,r2
	ldl	rr2,%0000(r15)
	ldl	%0008(r15),rr2
	ld	r2,%0004(r15)
	jr	L1a12
L1976:
	ldl	rr2,%0008(r15)
	ldk	r2,#0
	and	r3,#%0007
	addl	rr2,#%00000030
	ldb	@r14,rl3
	inc	r14,#1
	ldl	rr2,%0008(r15)
	ld	r0,#%0003
	neg	r0
	sdll	rr2,r0
	ldl	%0008(r15),rr2
	testl	rr2
	jr	ne,L1976
	jr	L19da
L19a2:
	ldl	rr2,%0008(r15)
	ldk	r2,#0
	and	r3,#%000F
	addl	rr2,#%00000030
	ldb	@r14,rl3
	cpb	@r14,#%39
	jr	le,L19c2
	ldb	rl2,@r14
	addb	rl2,#%27
	ldb	@r14,rl2
L19c2:
	inc	r14,#1
	ldl	rr2,%0008(r15)
	ld	r0,#%0004
	neg	r0
	sdll	rr2,r0
	ldl	%0008(r15),rr2
	testl	rr2
	jr	ne,L19a2
L19da:
	dec	r14,#1
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r13,#1
	lda	r2,%000C(r15)
	cp	r14,r2
	jr	ne,L19da
L19ea:
	ld	r2,r13
	sub	r2,%0006(r15)
	jr	L1a24
L19f2:
	lda	r7,%0015(r15)
	ldl	rr4,%0000(r15)
	call	L1a30
	ld	r14,r2
L1a00:
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r14,#1
	inc	r13,#1
	lda	r2,%0016(r15)
	cp	r14,r2
	jr	ne,L1a00
	jr	L19ea
L1a12:
	cp	r2,#%0008
	jr	eq,L1976
	cp	r2,#%000A
	jr	eq,L19f2
	cp	r2,#%0010
	jr	eq,L19a2
L1a24:
	ldm	r13,%001A(r15),#2
	add	r15,#%001E
	ret
L1a30:
	ldl	rr0,rr4
	subl	rr2,rr2
	ldl	rr4,rr2
	ldk	r6,#1
	testl	rr0
	jr	eq,L1a70
L1a3c:
	srll	rr0,#1
	jr	nc,L1a5a
	addb	rh4,rl6
	dab	rh4
	adcb	rl3,rh6
	dab	rl3
	adcb	rh3,rl5
	dab	rh3
	adcb	rl2,rh5
	dab	rl2
	adcb	rh2,rl4
	dab	rh2
	testl	rr0
	jr	eq,L1a70
L1a5a:
	addb	rl6,rl6
	dab	rl6
	adcb	rh6,rh6
	dab	rh6
	adcb	rl5,rl5
	dab	rl5
	adcb	rh5,rh5
	dab	rh5
	adcb	rl4,rl4
	dab	rl4
	jr	L1a3c
L1a70:
	ld	r6,r7
	ldb	rl0,#%30
	rrdb	rl0,rh4
	ldb	@r6,rl0
	rrdb	rl0,rh4
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh3
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rl2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh2
	dec	r6,#1
	ldb	@r6,rl0
	rrdb	rl0,rh2
	dec	r6,#1
	ldb	@r6,rl0
	ldb	rl0,#%30
	ldk	r1,#10
	cpirb	rl0,@r6,r1,ne
	dec	r6,#1
	ld	r2,r6
	ret
L1abc:
	dec	r15,#4
	ld	r1,r7
	ldk	r3,#0
L1ac2:
	ld	r2,r1
	inc	r1,#1
	ldb	rl2,@r2
	testb	rl2
	jr	eq,L1ad4
	inc	r3,#1
	cp	r3,#%4000
	jr	le,L1ac2
L1ad4:
	ld	r2,r3
	inc	r15,#4
	ret
_malloc::
	dec	r15,#12
	ldm	%0002(r15),r10,#5
	ld	%0000(r15),r7
	test	D0272
	jr	ne,L1b10
	ld	r2,#D0274
	or	r2,#%0001
	ld	D0272,r2
	ld	r2,#D0272
	or	r2,#%0001
	ld	D0274,r2
	ld	D0278,#D0274
	ld	D0276,#D0272
L1b10:
	ld	r2,%0000(r15)
	inc	r2,#3
	res	r2,#0
	rr	r2,#1
	ld	r11,r2
	ld	r14,D0276
	jr	L1bdc
L1b22:
	ld	r13,r14
	ld	r2,@r14
	and	r2,#%FFFE
	ld	r14,r2
	cp	r2,r13
	jr	ugt,L1be0
	cp	r13,D0278
	jr	ne,L1b3c
	cp	r2,#D0272
	jr	eq,L1b40
L1b3c:
	sub	r2,r2
	jr	L1c2e
L1b40:
	inc	D027c,#1
	cp	D027c,#%0001
	jr	ule,L1be0
	ld	r2,@r12
	cp	r2,D0278
	jr	ne,L1b64
	ld	r2,D0278
	sub	r2,r12
	sra	r2,#1
	ld	r3,r11
	sub	r3,r2
	jr	L1b66
L1b64:
	ld	r3,r11
L1b66:
	ld	r10,r3
	lda	r2,%0200(r10)
	srl	r2,#9
	sla	r2,#9
	ld	D027c,r2
	sub	r7,r7
	call	_sbrk
	ld	r13,r2
	ld	r2,D027c
	add	r2,r2
	add	r2,r13
	cp	r13,r2
	jr	ugt,L1b3c
	ld	r7,D027c
	add	r7,r7
	call	_sbrk
	ld	r13,r2
	cp	r2,#%FFFF
	jr	eq,L1b3c
	ld	r2,D0278
	ld	@r2,r13
	ld	r2,D0278
	inc	r2,#2
	cp	r13,r2
	jr	eq,L1bbe
	ld	r2,D0278
	ld	r2,@r2
	or	r2,#%0001
	ld	r3,D0278
	ld	@r3,r2
L1bbe:
	ld	r2,D027c
	add	r2,r2
	dec	r2,#2
	add	r2,r13
	ld	@r13,r2
	ld	D0278,r2
	ld	r2,#D0272
	or	r2,#%0001
	ld	r3,D0278
	ld	@r3,r2
L1bdc:
	clr	D027c
L1be0:
	bit	@r14,#0
	jr	ne,L1b22
	ld	r12,r14
	jr	L1bec
L1be8:
	ld	r2,@r13
	ld	@r14,r2
L1bec:
	ld	r13,@r14
	bit	@r13,#0
	jr	eq,L1be8
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r13,r2
	jr	c,L1b22
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r14,r2
	jr	ugt,L1b22
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	ld	D0276,r2
	cp	r13,D0276
	jr	ule,L1c20
	ld	r3,@r2
	ld	D027a,r3
	ld	r3,@r14
	ld	@r2,r3
L1c20:
	ld	r2,D0276
	or	r2,#%0001
	ld	@r14,r2
	lda	r2,%0002(r14)
L1c2e:
	ldm	r10,%0002(r15),#5
	inc	r15,#12
	ret
_free::
	dec	r15,#4
	ld	r1,r7
	ld	r3,r7
	dec	r3,#2
	ld	D0276,r3
	ld	r2,@r3
	and	r2,#%FFFE
	ld	@r3,r2
	inc	r15,#4
	ret
_realloc::
	dec	r15,#14
	ldm	%0008(r15),r12,#3
	ld	%0000(r15),r6
	ld	r14,r7
	bit	%FFFE(r14),#0
	jr	eq,L1c66
	calr	_free
L1c66:
	ld	r2,%FFFE(r14)
	sub	r2,r14
	sra	r2,#1
	ld	%0006(r15),r2
	ld	r7,%0000(r15)
	calr	_malloc
	ld	r13,r2
	test	r2
	jr	eq,L1ce8
	cp	r2,r14
	jr	eq,L1ce8
	ld	%0002(r15),r14
	ld	%0004(r15),r2
	ld	r2,%0000(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r12,r2
	cp	r2,%0006(r15)
	jr	nc,L1cb8
	ld	%0006(r15),r2
	jr	L1cb8
L1ca4:
	ld	r2,%0002(r15)
	ld	r3,%0004(r15)
	ld	r0,@r2
	ld	@r3,r0
	inc	%0002(r15),#2
	inc	%0004(r15),#2
L1cb8:
	ld	r2,%0006(r15)
	dec	%0006(r15),#1
	test	r2
	jr	ne,L1ca4
	cp	r13,r14
	jr	nc,L1ce8
	ld	r2,r12
	add	r2,r12
	add	r2,r13
	cp	r14,r2
	jr	ugt,L1ce8
	ld	r2,r12
	add	r2,r12
	add	r2,r13
	sub	r2,r14
	sra	r2,#1
	add	r2,r2
	add	r2,r13
	ld	r3,D027a
	ld	@r2,r3
L1ce8:
	ld	r2,r13
	ldm	r12,%0008(r15),#3
	inc	r15,#14
	ret
_isatty::
	dec	r15,#8
	ld	%0000(r15),r7
	lda	r6,%0002(r15)
	ld	r7,%0000(r15)
	call	L1d30
	cp	r2,#%0000
	jr	ge,L1d10
	sub	r2,r2
	jr	L1d12
L1d10:
	ldk	r2,#1
L1d12:
	inc	r15,#8
	ret
_gtty::
	dec	r15,#4
	ldl	%0000(r15),rr6
	ld	r7,%0002(r15)
	ld	r6,#%7409
	ld	r5,%0000(r15)
	call	_ioctl
	inc	r15,#4
	ret
L1d30:
	dec	r15,#4
	ldl	%0000(r15),rr6
	ld	r7,%0002(r15)
	ld	r6,#%7408
	ld	r5,%0000(r15)
	call	_ioctl
	inc	r15,#4
	ret
_strcat::
	dec	r15,#6
	ld	r1,r7
	ld	r4,r6
	ld	r5,r7
L1d52:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L1d52
	dec	r1,#1
L1d5c:
	ld	r2,r4
	inc	r4,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	testb	rl0
	jr	ne,L1d5c
	ld	r2,r5
	inc	r15,#6
	ret
_strcmp::
	dec	r15,#4
	ld	r1,r7
	ld	r4,r6
L1d78:
	ld	r2,r4
	inc	r4,#1
	ldb	rl2,@r2
	cpb	rl2,@r1
	jr	ne,L1d90
	ld	r2,r1
	inc	r1,#1
	ldb	rl2,@r2
	testb	rl2
	jr	ne,L1d78
	sub	r2,r2
	jr	L1d9c
L1d90:
	ldb	rl2,@r1
	extsb	r2
	dec	r4,#1
	ldb	rl3,@r4
	extsb	r3
	sub	r2,r3
L1d9c:
	inc	r15,#4
	ret
_strcpy::
	dec	r15,#6
	ld	r1,r7
	ld	r4,r6
	ld	r5,r7
L1da8:
	ld	r2,r4
	inc	r4,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	testb	rl0
	jr	ne,L1da8
	ld	r2,r5
	inc	r15,#6
	ret
_strlen::
	dec	r15,#4
	ld	r1,r7
	ldk	r3,#0
	jr	L1dc8
L1dc6:
	inc	r3,#1
L1dc8:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jr	ne,L1dc6
	ld	r2,r3
	inc	r15,#4
	ret
_brk::
	ld	r0,r7
	xor	r4,r4
	sc	#%11
	ld	r2,r4
	jp	c,cerror
	ld	D06ee,r0
	ret
_sbrk::
	ld	r0,r7
	ld	r2,D06ee
	add	r0,r2
	ld	r1,r0
	dec	r1,#1
	dec	r2,#1
	cpb	rh1,rh2
	jr	eq,L1e02
	sc	#%11
	ld	r2,r4
	jp	c,cerror
L1e02:
	ld	r2,D06ee
	ld	D06ee,r0
	ret
_chmod::
	ld	r0,r7
	ld	r1,r6
	xor	r4,r4
	sc	#%0F
	ld	r2,r4
	ret	nc
	jp	cerror
_close::
	ld	r0,r7
	xor	r4,r4
	sc	#%06
	ld	r2,r4
	ret	nc
	jp	cerror
_creat::
	ld	r0,r7
	ld	r1,r6
	sc	#%08
	ld	r2,r4
	ret	nc
	jp	cerror
_ioctl::
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	xor	r4,r4
	sc	#%36
	ld	r2,r4
	ret	nc
	jp	cerror
_lseek::
	ld	r0,r7
	ld	r1,r4
	ld	r2,r5
	subl	rr4,rr4
	sc	#%13
	ldl	rr2,rr4
	ret	nc
	jp	cerror
_open::
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#%05
	ld	r2,r4
	ret	nc
	jp	cerror
_read::
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#%03
	ld	r2,r4
	ret	nc
	jp	cerror
_signal::
	ld	r2,#%0016
	ld	r0,r7
	ld	r1,r6
	cp	r0,#%0014
	jp	ge,cerror
	ld	r3,r0
	sla	r3,#1
	ld	r2,_sigtab(r3)
	ld	_sigtab(r3),r1
	rr	r1,#1
	rl	r1,#1
	jr	ule,L1ea4
	lda	r1,L1eb2(r3)
L1ea4:
	sc	#%30
	bit	r4,#0
	jr	eq,L1eac
	ld	r2,r4
L1eac:
	ret	nc
	jp	cerror
L1eb2:
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
	calr	L1eda
L1eda:
	sub	r15,#%001E
	ldm	@r15,r0,#15
	ld	r7,%001E(r15)
	sub	r7,#%BDBA
	ld	r2,_sigtab(r7)
	srl	r7,#1
	call	@r2
	ldm	r0,@r15,#15
	add	r15,#%0020
	sc	#%00
_umask::
	ld	r0,r7
	sc	#%3C
	ld	r2,r4
	ret
_write::
	ld	r0,r7
	ld	r1,r6
	ld	r2,r5
	sc	#%04
	ld	r2,r4
	ret	nc
	jp	cerror
cerror::
	ld	D06f0,r2
	ld	D06f2,r5
	ldl	rr2,#%FFFFFFFF
	ret
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	addb	rh0,#%00
	.psec
	.data
D0000:
	.byte	%49,%4e,%46,%49,%4e,%49,%54,%59
	.byte	%00
D0009:
	.byte	%4e,%41,%4e,%28,%7e,%7e,%7e,%7e
	.byte	%7e,%7e,%29,%00,%00
D0016:
	.byte	%28,%6e,%75,%6c,%6c,%29,%00,%00
D001e:
	.byte	%00,%65,%00,%66,%00,%67,%00,%73
	.addr	L0b90
	.addr	L0b90
	.addr	L0b90
	.addr	L0b70
D002e:
	.byte	%00,%44,%00,%4f,%00,%55,%00,%58
	.byte	%00,%63,%00,%64,%00,%65,%00,%66
	.byte	%00,%67,%00,%6c,%00,%6f,%00,%73
	.byte	%00,%75,%00,%78
	.addr	L0bfc
	.addr	L0bec
	.addr	L0c04
	.addr	L0bf4
	.addr	L0dc8
	.addr	L0cdc
	.addr	L0df0
	.addr	L0df0
	.addr	L0df0
	.addr	L0bd6
	.addr	L0b98
	.addr	L0d86
	.addr	L0ba8
	.addr	L0ba0
D0066:
	.addr	L109e
	.addr	L1066
	.addr	L109a
	.addr	L10a6
	.addr	L10aa
	.addr	L10ac
	.addr	L10b0
	.addr	L10be
	.addr	L10c8
	.addr	L10cc
	.addr	L10d0
	.addr	L10d6
	.addr	L10e0
	.addr	L10e2
	.addr	L10e6
	.addr	L1126
	.byte	%00,%4e,%49,%65,%45,%2e,%2b,%2d
	.byte	%39,%38,%37,%36,%35,%34,%33,%32
	.byte	%31
D0097:
	.byte	%30,%00,%02,%04,%00,%00,%06,%0a
	.byte	%08,%0e,%0e,%0e,%0e,%0e,%0e,%0e
	.byte	%0e,%0e,%0c,%00,%02,%04,%00,%00
	.byte	%06,%00,%00,%0e,%0e,%0e,%0e,%0e
	.byte	%0e,%0e,%0e,%0e,%0c,%1e,%1e,%1e
	.byte	%12,%12,%10,%1e,%1e,%0e,%0e,%0e
	.byte	%0e,%0e,%0e,%0e,%0e,%0e,%0e,%1e
	.byte	%1e,%1e,%1e,%1e,%1e,%1e,%1e,%16
	.byte	%16,%16,%16,%16,%16,%16,%16,%16
	.byte	%14,%1e,%1e,%1e,%12,%12,%1e,%1e
	.byte	%1e,%16,%16,%16,%16,%16,%16,%16
	.byte	%16,%16,%16,%1e,%1e,%1e,%1e,%1e
	.byte	%1e,%1a,%18,%1c,%1c,%1c,%1c,%1c
	.byte	%1c,%1c,%1c,%1c,%1c,%1e,%1e,%1e
	.byte	%1e,%1e,%1e,%1e,%1e,%1c,%1c,%1c
	.byte	%1c,%1c,%1c,%1c,%1c,%1c,%1c
D0116:
	.byte	%41,%4e,%28,%00
D011a:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
D013a:
	.byte	%49,%4e,%46,%00
D013e:
	.byte	%4e,%41,%4e,%28,%30,%30,%30,%30
	.byte	%30,%00
D0148:
	.byte	%0d,%80,%6c,%d7,%bf,%ce,%0f,%5a
	.byte	%b8,%a6,%76,%1e,%06,%c0,%55,%5d
	.byte	%9c,%b0,%0b,%fd,%6f,%02,%53,%39
	.byte	%03,%60,%4b,%25,%8d,%9e,%89,%d1
	.byte	%13,%46,%bd,%a5,%01,%b0,%45,%9a
	.byte	%86,%a3,%36,%4e,%a6,%2c,%67,%2d
	.byte	%00,%d8,%42,%cc,%b9,%a7,%4a,%06
	.byte	%37,%ce,%2e,%e1,%00,%6c,%41,%65
	.byte	%da,%01,%ee,%64,%1a,%70,%8d,%ea
	.byte	%00,%36,%40,%b2,%a7,%0c,%3c,%40
	.byte	%a6,%4e,%6c,%52,%00,%1b,%40,%58
	.byte	%ce,%cb,%8f,%27,%f4,%20,%0f,%3a
D01a8:
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00
__iob::
	.addr	__sibuf
	.byte	%00,%00
	.addr	__sibuf
	.byte	%01,%00
D01c4:
	.byte	%00,%00,%00,%00,%00,%00
D01ca:
	.byte	%02
D01cb:
	.byte	%01,%00,%00,%00,%00,%00,%00,%06
	.byte	%02,%00,%00,%00,%00,%00,%00,%00
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
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00
D025c:
	.addr	__iob+160
D025e:
	.byte	%55,%6e,%6b,%6e,%6f,%77,%6e,%20
	.byte	%65,%72,%72,%6f,%72,%00
D026c:
	.byte	%3a,%20,%00
D026f:
	.byte	%0a,%00,%00
D0272:
	.byte	%00,%00
D0274:
	.byte	%00,%00
D0276:
	.byte	%00,%00
D0278:
	.byte	%00,%00
D027a:
	.byte	%00,%00
D027c:
	.byte	%00,%00
D027e:
	.addr	D02de
	.addr	D02e6
	.addr	D02f0
	.addr	D030a
	.addr	D031a
	.addr	D0332
	.addr	D033c
	.addr	D0356
	.addr	D0368
	.addr	D037a
	.addr	D038a
	.addr	D0396
	.addr	D03a8
	.addr	D03b8
	.addr	D03ca
	.addr	D03d6
	.addr	D03ec
	.addr	D03fe
	.addr	D040a
	.addr	D041c
	.addr	D042b
	.addr	D043b
	.addr	D044a
	.addr	D045b
	.addr	D046f
	.addr	D0483
	.addr	D0494
	.addr	D04a3
	.addr	D04b2
	.addr	D04ca
	.addr	D04d7
	.addr	D04ed
	.addr	D04fc
	.addr	D0508
	.addr	D051b
	.addr	D052c
	.addr	D0535
	.addr	D054b
D02ca:
	.byte	%00,%26
D02cc:
	.addr	D056e
	.addr	D0578
	.addr	D0592
	.addr	D05a6
	.addr	D05b2
	.addr	D05c9
	.addr	D05d2
	.addr	D05df
D02dc:
	.byte	%00,%08
D02de:
	.byte	%45,%72,%72,%6f,%72,%20,%30,%00
D02e6:
	.byte	%4e,%6f,%74,%20,%6f,%77,%6e,%65
	.byte	%72,%00
D02f0:
	.byte	%4e,%6f,%20,%73,%75,%63,%68,%20
	.byte	%66,%69,%6c,%65,%20,%6f,%72,%20
	.byte	%64,%69,%72,%65,%63,%74,%6f,%72
	.byte	%79,%00
D030a:
	.byte	%4e,%6f,%20,%73,%75,%63,%68,%20
	.byte	%70,%72,%6f,%63,%65,%73,%73,%00
D031a:
	.byte	%49,%6e,%74,%65,%72,%72,%75,%70
	.byte	%74,%65,%64,%20,%73,%79,%73,%74
	.byte	%65,%6d,%20,%63,%61,%6c,%6c,%00
D0332:
	.byte	%49,%2f,%4f,%20,%65,%72,%72,%6f
	.byte	%72,%00
D033c:
	.byte	%4e,%6f,%20,%73,%75,%63,%68,%20
	.byte	%64,%65,%76,%69,%63,%65,%20,%6f
	.byte	%72,%20,%61,%64,%64,%72,%65,%73
	.byte	%73,%00
D0356:
	.byte	%41,%72,%67,%20,%6c,%69,%73,%74
	.byte	%20,%74,%6f,%6f,%20,%6c,%6f,%6e
	.byte	%67,%00
D0368:
	.byte	%45,%78,%65,%63,%20,%66,%6f,%72
	.byte	%6d,%61,%74,%20,%65,%72,%72,%6f
	.byte	%72,%00
D037a:
	.byte	%42,%61,%64,%20,%66,%69,%6c,%65
	.byte	%20,%6e,%75,%6d,%62,%65,%72,%00
D038a:
	.byte	%4e,%6f,%20,%63,%68,%69,%6c,%64
	.byte	%72,%65,%6e,%00
D0396:
	.byte	%4e,%6f,%20,%6d,%6f,%72,%65,%20
	.byte	%70,%72,%6f,%63,%65,%73,%73,%65
	.byte	%73,%00
D03a8:
	.byte	%4e,%6f,%74,%20,%65,%6e,%6f,%75
	.byte	%67,%68,%20,%63,%6f,%72,%65,%00
D03b8:
	.byte	%50,%65,%72,%6d,%69,%73,%73,%69
	.byte	%6f,%6e,%20,%64,%65,%6e,%69,%65
	.byte	%64,%00
D03ca:
	.byte	%42,%61,%64,%20,%61,%64,%64,%72
	.byte	%65,%73,%73,%00
D03d6:
	.byte	%42,%6c,%6f,%63,%6b,%20,%64,%65
	.byte	%76,%69,%63,%65,%20,%72,%65,%71
	.byte	%75,%69,%72,%65,%64,%00
D03ec:
	.byte	%4d,%6f,%75,%6e,%74,%20,%64,%65
	.byte	%76,%69,%63,%65,%20,%62,%75,%73
	.byte	%79,%00
D03fe:
	.byte	%46,%69,%6c,%65,%20,%65,%78,%69
	.byte	%73,%74,%73,%00
D040a:
	.byte	%43,%72,%6f,%73,%73,%2d,%64,%65
	.byte	%76,%69,%63,%65,%20,%6c,%69,%6e
	.byte	%6b,%00
D041c:
	.byte	%4e,%6f,%20,%73,%75,%63,%68,%20
	.byte	%64,%65,%76,%69,%63,%65,%00
D042b:
	.byte	%4e,%6f,%74,%20,%61,%20,%64,%69
	.byte	%72,%65,%63,%74,%6f,%72,%79,%00
D043b:
	.byte	%49,%73,%20,%61,%20,%64,%69,%72
	.byte	%65,%63,%74,%6f,%72,%79,%00
D044a:
	.byte	%49,%6e,%76,%61,%6c,%69,%64,%20
	.byte	%61,%72,%67,%75,%6d,%65,%6e,%74
	.byte	%00
D045b:
	.byte	%46,%69,%6c,%65,%20,%74,%61,%62
	.byte	%6c,%65,%20,%6f,%76,%65,%72,%66
	.byte	%6c,%6f,%77,%00
D046f:
	.byte	%54,%6f,%6f,%20,%6d,%61,%6e,%79
	.byte	%20,%6f,%70,%65,%6e,%20,%66,%69
	.byte	%6c,%65,%73,%00
D0483:
	.byte	%4e,%6f,%74,%20,%61,%20,%74,%79
	.byte	%70,%65,%77,%72,%69,%74,%65,%72
	.byte	%00
D0494:
	.byte	%54,%65,%78,%74,%20,%66,%69,%6c
	.byte	%65,%20,%62,%75,%73,%79,%00
D04a3:
	.byte	%46,%69,%6c,%65,%20,%74,%6f,%6f
	.byte	%20,%6c,%61,%72,%67,%65,%00
D04b2:
	.byte	%4e,%6f,%20,%73,%70,%61,%63,%65
	.byte	%20,%6c,%65,%66,%74,%20,%6f,%6e
	.byte	%20,%64,%65,%76,%69,%63,%65,%00
D04ca:
	.byte	%49,%6c,%6c,%65,%67,%61,%6c,%20
	.byte	%73,%65,%65,%6b,%00
D04d7:
	.byte	%52,%65,%61,%64,%2d,%6f,%6e,%6c
	.byte	%79,%20,%66,%69,%6c,%65,%20,%73
	.byte	%79,%73,%74,%65,%6d,%00
D04ed:
	.byte	%54,%6f,%6f,%20,%6d,%61,%6e,%79
	.byte	%20,%6c,%69,%6e,%6b,%73,%00
D04fc:
	.byte	%42,%72,%6f,%6b,%65,%6e,%20,%70
	.byte	%69,%70,%65,%00
D0508:
	.byte	%41,%72,%67,%75,%6d,%65,%6e,%74
	.byte	%20,%74,%6f,%6f,%20,%6c,%61,%72
	.byte	%67,%65,%00
D051b:
	.byte	%52,%65,%73,%75,%6c,%74,%20,%74
	.byte	%6f,%6f,%20,%6c,%61,%72,%67,%65
	.byte	%00
D052c:
	.byte	%44,%65,%61,%64,%6c,%6f,%63,%6b
	.byte	%00
D0535:
	.byte	%53,%65,%67,%6d,%65,%6e,%74,%20
	.byte	%6e,%6f,%74,%20,%61,%76,%61,%69
	.byte	%6c,%61,%62,%6c,%65,%00
D054b:
	.byte	%54,%72,%69,%65,%64,%20,%74,%6f
	.byte	%20,%70,%72,%6f,%66,%69,%6c,%65
	.byte	%20,%74,%6f,%6f,%20,%6d,%61,%6e
	.byte	%79,%20,%73,%65,%67,%6d,%65,%6e
	.byte	%74,%73,%00
D056e:
	.byte	%49,%2f,%4f,%20,%65,%72,%72,%6f
	.byte	%72,%00
D0578:
	.byte	%4e,%6f,%20,%73,%75,%63,%68,%20
	.byte	%64,%65,%76,%69,%63,%65,%20,%6f
	.byte	%72,%20,%61,%64,%64,%72,%65,%73
	.byte	%73,%00
D0592:
	.byte	%44,%61,%74,%61,%20,%74,%72,%61
	.byte	%6e,%73,%66,%65,%72,%20,%65,%72
	.byte	%72,%6f,%72,%00
D05a6:
	.byte	%44,%65,%76,%69,%63,%65,%20,%62
	.byte	%75,%73,%79,%00
D05b2:
	.byte	%44,%65,%76,%69,%63,%65,%20,%77
	.byte	%72,%69,%74,%65,%20,%70,%72,%6f
	.byte	%74,%65,%63,%74,%65,%64,%00
D05c9:
	.byte	%4e,%6f,%20,%6d,%65,%64,%69,%61
	.byte	%00
D05d2:
	.byte	%45,%6e,%64,%20,%6f,%66,%20,%6d
	.byte	%65,%64,%69,%61,%00
D05df:
	.byte	%45,%6e,%64,%20,%6f,%66,%20,%64
	.byte	%61,%74,%61,%00,%00
__ctype::
	.byte	%00
D05ed:
	.byte	%20,%20,%20,%20,%20,%20,%20,%20
	.byte	%20,%28,%28,%28,%28,%28,%20,%20
	.byte	%20,%20,%20,%20,%20,%20,%20,%20
	.byte	%20,%20,%20,%20,%20,%20,%20,%20
	.byte	%48,%10,%10,%10,%10,%10,%10,%10
	.byte	%10,%10,%10,%10,%10,%10,%10,%10
	.byte	%84,%84,%84,%84,%84,%84,%84,%84
	.byte	%84,%84,%10,%10,%10,%10,%10,%10
	.byte	%10,%81,%81,%81,%81,%81,%81,%01
	.byte	%01,%01,%01,%01,%01,%01,%01,%01
	.byte	%01,%01,%01,%01,%01,%01,%01,%01
	.byte	%01,%01,%01,%10,%10,%10,%10,%10
	.byte	%10,%82,%82,%82,%82,%82,%82,%02
	.byte	%02,%02,%02,%02,%02,%02,%02,%02
	.byte	%02,%02,%02,%02,%02,%02,%02,%02
	.byte	%02,%02,%02,%10,%10,%10,%10,%20
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
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00,%00,%00,%00,%00,%00,%00,%00
	.byte	%00
D06ee:
	.addr	_end
D06f0:
	.byte	%00,%00
D06f2:
	.byte	%00,%00

