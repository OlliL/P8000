// malloc.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_malloc
	.extern	_stkseg
	.extern	_mkseg
	.extern	_ssgbrk
	.extern	_free
	.extern	_realloc
	.extern	_calloc
	.extern	_cfree
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 1334 bytes
	.psec
	.code
_malloc::
	sub	r15,#%001A
	ldm	_stkseg+14(r15),r8,#6
	ld	r13,r7
	clr	|_stkseg+8|(r15)
	cp	r7,L1d_0030
	jr	ule,L0t_0022
L0t_001a:
	subl	rr2,rr2
	jp	L0t_02ba
L0t_0022:
	ldl	rr2,L1d_0040
	testl	rr2
	jr	eq,L0t_0034
	test	L1d_0048
	jr	ne,L0t_003a
L0t_0034:
	ld	|_stkseg+8|(r15),#%0001
L0t_003a:
	ldl	rr2,L1d_0040
	testl	rr2
	jr	ne,L0t_00b4
	ldl	rr4,#L1d_0038
	ldl	L1d_0040,rr4
	ldl	rr2,#L1d_003c
	ldl	L1d_0044,rr2
	ldl	rr2,#L1d_003c
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr2,L1d_0040
	ldl	@rr2,rr4
	ldl	rr4,#L1d_0038
	or	r5,#%0001
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	rr4,L1d_0044
	ldl	@rr4,rr6
	ldl	rr2,L1d_0040
	ldl	rr6,rr2
	ldl	rr0,@rr2
	ldl	@rr6,rr0
	ldl	rr4,L1d_0044
	ldl	rr6,rr4
	ldl	rr0,@rr4
	ldl	@rr6,rr0
	sub	r7,r7
	sub	r6,r6
	call	_mkseg
	ldl	L1d_0034,rr2
L0t_00b4:
	ld	r2,r13
	inc	r2,#7
	srl	r2,#2
	ld	r12,r2
	ldl	rr10,L1d_0040
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	rr2,L1d_0044
	ldl	|_stkseg+2|(r15),rr2
L0t_00dc:
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L0t_011a
	jr	L0t_0152
L0t_00ea:
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
	ldl	L1d_0040,rr4
	jr	L0t_011a
L0t_0102:
	cpl	rr8,L1d_0040
	jr	eq,L0t_00ea
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
L0t_011a:
	ldl	rr2,@rr8
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L0t_0102
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr8,rr4
	jr	c,L0t_0146
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr10,rr4
	jp	ule,L0t_0288
L0t_0146:
	cpl	rr8,L1d_0044
	jr	ne,L0t_0152
	ldl	|_stkseg+2|(r15),rr10
L0t_0152:
	ldl	rr10,rr8
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	cpl	rr10,L1d_0040
	jr	ne,L0t_00dc
	ldl	rr2,L1d_0044
	sub	r3,|_stkseg+4|(r15)
	sra	r3,#2
	ld	r6,r12
	sub	r6,r3
	add	r6,|_stkseg+8|(r15)
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#2
	addl	rr2,#%000007FF
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#%00000800
	ld	|_stkseg+6|(r15),r7
	ld	r2,r7
	cp	r7,L1d_0032
	jp	ugt,L0t_001a
	sla	r2,#11
	dec	r2,#1
	ld	r13,r2
	ldb	rl7,L1d_0034
	extsb	r7
	ld	r6,r2
	call	_ssgbrk
	ldl	rr10,rr2
	cpl	rr2,#%FFFFFFFF
	jr	ne,L0t_0218
	ld	r6,r12
	inc	r6,#1
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#2
	addl	rr2,#%000007FF
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#%00000800
	ld	|_stkseg+6|(r15),r7
	ld	r2,r7
	sla	r2,#11
	dec	r2,#1
	ld	r13,r2
	sub	r7,r7
	ld	r6,r2
	call	_mkseg
	ldl	rr10,rr2
	cp	r3,#%FFFF
	jp	eq,L0t_001a
	ldl	L1d_0034,rr2
	ld	|_stkseg+8|(r15),#%0001
L0t_0218:
	test	|_stkseg+8|(r15)
	jr	eq,L0t_0250
	ldl	rr2,rr10
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr2,L1d_0044
	ldl	@rr2,rr4
	ld	r4,|_stkseg+6|(r15)
	sla	r4,#9
	sla	r4,#2
	ldl	rr6,rr10
	add	r7,r4
	dec	r7,#4
	ldl	rr8,rr6
	ld	L1d_0048,#%0001
	jr	L0t_026c
L0t_0250:
	ldl	rr10,|_stkseg+2|(r15)
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#9
	sla	r2,#2
	ldl	rr4,L1d_0044
	add	r5,r2
	dec	r5,#4
	ldl	rr8,rr4
L0t_026c:
	ldl	rr2,#L1d_0038
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr8,rr4
	ldl	rr2,@rr8
	ldl	@rr8,rr2
	ldl	L1d_0044,rr8
L0t_0288:
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	ldl	L1d_0040,rr4
	cpl	rr8,L1d_0040
	jr	ule,L0t_02a4
	ldl	rr2,rr4
	ldl	@rr2,rr8
L0t_02a4:
	ldl	rr2,L1d_0040
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr10,rr4
	ldl	rr2,rr10
	inc	r3,#4
L0t_02ba:
	ldm	r8,_stkseg+14(r15),#6
	add	r15,#%001A
	ret
L0t_02c8:
	sub	r15,#%0012
	ldm	_stkseg+6(r15),r8,#4
	ld	r1,r7
	ld	r2,r7
	inc	r2,#7
	srl	r2,#2
	ld	r6,r2
	ldl	rr10,L1d_0040
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
L0t_02f4:
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L0t_0332
	jr	L0t_035e
L0t_0302:
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
	ldl	L1d_0040,rr4
	jr	L0t_0332
L0t_031a:
	cpl	rr8,L1d_0040
	jr	eq,L0t_0302
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
L0t_0332:
	ldl	rr2,@rr8
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L0t_031a
	ld	r2,r6
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr8,rr4
	jr	c,L0t_035e
	ld	r2,r6
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr10,rr4
	jr	ugt,L0t_035e
	ldl	rr2,rr10
	jr	L0t_0378
L0t_035e:
	ldl	rr10,rr8
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	cpl	rr10,L1d_0040
	jr	ne,L0t_02f4
	subl	rr2,rr2
L0t_0378:
	ldm	r8,_stkseg+6(r15),#4
	add	r15,#%0012
	ret
_free::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg|(r15),rr6
	ldl	rr12,|_stkseg|(r15)
	ldl	rr2,rr12
	dec	r3,#4
	ldl	rr12,rr2
	ldl	L1d_0040,rr2
	ldl	rr2,@rr12
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr12,rr4
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
_realloc::
	sub	r15,#%0026
	ldm	_stkseg+26(r15),r8,#6
	ld	|_stkseg|(r15),r5
	ldl	rr12,rr6
	ldl	rr2,rr6
	dec	r3,#4
	ldl	|_stkseg+8|(r15),rr2
	ldl	rr6,@rr2
	and	r7,#%FFFE
	sub	r7,r13
	sra	r7,#2
	ld	|_stkseg+20|(r15),r7
	inc	r5,#3
	srl	r5,#2
	ld	|_stkseg+6|(r15),r5
	ldl	rr2,|_stkseg+8|(r15)
	ldl	rr4,@rr2
	ldk	r4,#0
	and	r5,#%0001
	testl	rr4
	jr	eq,L0t_0406
	ldl	rr6,rr12
	calr	_free
L0t_0406:
	ld	r7,|_stkseg|(r15)
	calr	L0t_02c8
	ldl	|_stkseg+12|(r15),rr2
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#2
	ldl	rr4,|_stkseg+12|(r15)
	add	r5,r2
	cpl	rr4,|_stkseg+8|(r15)
	jr	ule,L0t_0474
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#2
	ldl	rr4,|_stkseg+12|(r15)
	add	r5,r2
	ld	r2,|_stkseg+20|(r15)
	sla	r2,#2
	ldl	rr6,|_stkseg+8|(r15)
	add	r7,r2
	cpl	rr4,rr6
	jr	nc,L0t_0474
	ldl	rr2,|_stkseg+12|(r15)
	ldl	rr4,@rr2
	or	r5,#%0001
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	@rr2,rr6
	ld	r7,|_stkseg|(r15)
	calr	_malloc
	ldl	|_stkseg+16|(r15),rr2
	ldl	rr2,|_stkseg+12|(r15)
	ldl	rr4,@rr2
	and	r5,#%FFFE
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	@rr2,rr6
	jr	L0t_047e
L0t_0474:
	ld	r7,|_stkseg|(r15)
	calr	_malloc
	ldl	|_stkseg+16|(r15),rr2
L0t_047e:
	ldl	rr2,|_stkseg+16|(r15)
	testl	rr2
	jr	eq,L0t_04be
	cpl	rr12,|_stkseg+16|(r15)
	jr	eq,L0t_04be
	ldl	rr10,rr12
	ldl	rr8,rr2
	ld	r2,|_stkseg+6|(r15)
	cp	r2,|_stkseg+20|(r15)
	jr	ule,L0t_04a0
	ld	r3,|_stkseg+20|(r15)
	jr	L0t_04a4
L0t_04a0:
	ld	r3,|_stkseg+6|(r15)
L0t_04a4:
	ld	|_stkseg+6|(r15),r3
	jr	L0t_04b2
L0t_04aa:
	ldl	rr2,@rr10
	ldl	@rr8,rr2
	inc	r11,#4
	inc	r9,#4
L0t_04b2:
	ld	r2,|_stkseg+6|(r15)
	dec	|_stkseg+6|(r15),#1
	test	r2
	jr	ne,L0t_04aa
L0t_04be:
	ldl	rr2,|_stkseg+16|(r15)
	ldm	r8,_stkseg+26(r15),#6
	add	r15,#%0026
	ret
_calloc::
	sub	r15,#%0016
	ldm	_stkseg+10(r15),r8,#6
	ldl	|_stkseg|(r15),rr6
	ld	r3,|_stkseg+2|(r15)
	mult	rr2,r6
	ld	|_stkseg+2|(r15),r3
	ld	r7,r3
	calr	_malloc
	ldl	rr8,rr2
	testl	rr2
	jr	ne,L0t_04f8
	subl	rr2,rr2
	jr	L0t_0518
L0t_04f8:
	ldl	rr12,rr8
	ld	r2,|_stkseg+2|(r15)
	inc	r2,#3
	srl	r2,#2
	ld	r11,r2
	jr	L0t_050e
L0t_0508:
	subl	rr2,rr2
	ldl	@rr12,rr2
	inc	r13,#4
L0t_050e:
	dec	r11,#1
	cp	r11,#%0000
	jr	ge,L0t_0508
	ldl	rr2,rr8
L0t_0518:
	ldm	r8,_stkseg+10(r15),#6
	add	r15,#%0016
	ret
_cfree::
	dec	r15,#8
	ldm	|_stkseg|(r15),r4,#4
	ldl	rr2,rr6
	calr	_free
	inc	r15,#8
	ret

// segment table entry 1 (segment 1), data, 74 bytes
	.psec	data
	.data
	.word	%405B
	.word	%245D
	.word	%6D61
	.word	%6C6C
	.word	%6F63
	.word	%2E63
	.word	%0933
	.word	%2E33
	.word	%2020
	.word	%3039
	.word	%2F31
	.word	%322F
	.word	%3833
	.word	%2031
	.word	%303A
	.word	%3336
	.word	%3A35
	.word	%3220
	.word	%2D20
	.word	%3837
	.word	%7765
	.word	%6761
	.word	%332E
	.word	%3200
L1d_0030:
	.word	%FFF8
L1d_0032:
	.word	%0020
L1d_0034:
	.word	%0000
	.word	%0000
L1d_0038:
	.word	%0000
	.word	%0000
L1d_003c:
	.word	%0000
	.word	%0000
L1d_0040:
	.word	%0000
	.word	%0000
L1d_0044:
	.word	%0000
	.word	%0000
L1d_0048:
	.word	%0000
