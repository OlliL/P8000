	.extern	_getenv
	.comm	26, _cbuf

// segment table entry 0 (segment 0), text, 1164 bytes
	.psec
	.code
_ctime::
	dec	r15,#4
	ld	%0000(r15),r7
	calr	_localtime
	ld	%0002(r15),r2
	ld	r7,r2
	calr	_asctime
	inc	r15,#4
	ret
_localtime::
	dec	r15,#14
	ldm	%0006(r15),r11,#4
	ld	%0000(r15),r7
	calr	_tzset
	ld	r2,%0000(r15)
	ldl	rr2,@r2
	subl	rr2,_timezone
	ldl	%0002(r15),rr2
	lda	r7,%0002(r15)
	calr	_gmtime
	ld	r13,r2
	ld	r14,%000E(r13)
	ld	r12,#%0077
	ld	r11,#%012F
	cp	%000A(r13),#%004A
	jr	eq,L0t_0054
	cp	%000A(r13),#%004B
	jr	ne,L0t_0074
L0t_0054:
	ld	r2,%000A(r13)
	sub	r2,#%004A
	sla	r2,#2
	ld	r12,_daytab(r2)
	ld	r2,%000A(r13)
	sub	r2,#%004A
	sla	r2,#2
	ld	r11,L0d_0024(r2)
L0t_0074:
	ld	r7,r13
	ld	r6,r12
	calr	_sunday
	ld	r12,r2
	ld	r7,r13
	ld	r6,r11
	calr	_sunday
	ld	r11,r2
	test	_daylight
	jr	eq,L0t_00c4
	cp	r14,r12
	jr	gt,L0t_009a
	cp	r14,r12
	jr	ne,L0t_00c4
	cp	%0004(r13),#%0002
	jr	lt,L0t_00c4
L0t_009a:
	cp	r14,r11
	jr	lt,L0t_00aa
	cp	r14,r11
	jr	ne,L0t_00c4
	cp	%0004(r13),#%0001
	jr	ge,L0t_00c4
L0t_00aa:
	ldl	rr2,%0002(r15)
	addl	rr2,#%00000E10
	ldl	%0002(r15),rr2
	lda	r7,%0002(r15)
	calr	_gmtime
	ld	r13,r2
	inc	%0010(r13),#1
L0t_00c4:
	ld	r2,r13
	ldm	r11,%0006(r15),#4
	inc	r15,#14
	ret
_sunday::
	dec	r15,#4
	ldm	%0000(r15),r13,#2
	ld	r14,r7
	ld	r13,r6
	cp	r6,#%003A
	jr	lt,L0t_00ee
	ld	r7,%000A(r14)
	calr	_dysize
	sub	r2,#%016D
	add	r13,r2
L0t_00ee:
	ld	r2,r13
	sub	r2,%000E(r14)
	add	r2,%000C(r14)
	add	r2,#%02BC
	ld	r1,r2
	exts	rr0
	div	rr0,#%0007
	ld	r2,r13
	sub	r2,r0
	ldm	r13,%0000(r15),#2
	inc	r15,#4
	ret
_gmtime::
	dec	r15,#16
	ldm	%000A(r15),r12,#3
	ld	%0000(r15),r7
	ld	r6,%0000(r15)
	ldl	rr6,@r6
	extsl	rq4
	divl	rq4,#%00015180
	ldl	%0002(r15),rr4
	ld	r6,%0000(r15)
	ldl	rr6,@r6
	extsl	rq4
	divl	rq4,#%00015180
	ldl	%0006(r15),rr6
	ldl	rr2,%0002(r15)
	cpl	rr2,#%00000000
	jr	ge,L0t_0166
	addl	rr2,#%00015180
	ldl	%0002(r15),rr2
	ldl	rr2,rr6
	subl	rr2,#%00000001
	ldl	%0006(r15),rr2
L0t_0166:
	ld	r12,#L0d_002a
	ldl	rr6,%0002(r15)
	extsl	rq4
	divl	rq4,#%0000003C
	ld	@r12,r5
	inc	r12,#2
	ldl	rr6,%0002(r15)
	extsl	rq4
	divl	rq4,#%0000003C
	ld	r13,r7
	ld	r3,r7
	exts	rr2
	div	rr2,#%003C
	ld	@r12,r2
	inc	r12,#2
	ld	r3,r13
	exts	rr2
	div	rr2,#%003C
	ld	r13,r3
	ld	@r12,r3
	inc	r12,#2
	ldl	rr6,%0006(r15)
	addl	rr6,#%00700004
	extsl	rq4
	divl	rq4,#%00000007
	ld	L0d_0036,r5
	ldl	rr2,%0006(r15)
	cpl	rr2,#%00000000
	jr	lt,L0t_01ec
	ld	r13,#%0046
L0t_01c8:
	ld	r7,r13
	calr	_dysize
	ld	r1,r2
	exts	rr0
	cpl	rr0,%0006(r15)
	jr	gt,L0t_0212
	ld	r7,r13
	calr	_dysize
	ld	r1,r2
	exts	rr0
	ldl	rr2,%0006(r15)
	subl	rr2,rr0
	ldl	%0006(r15),rr2
	inc	r13,#1
	jr	L0t_01c8
L0t_01ec:
	ld	r13,#%0046
	jr	L0t_0206
L0t_01f2:
	ld	r7,r13
	dec	r7,#1
	calr	_dysize
	ld	r1,r2
	exts	rr0
	addl	rr0,%0006(r15)
	ldl	%0006(r15),rr0
	dec	r13,#1
L0t_0206:
	ldl	rr2,%0006(r15)
	cpl	rr2,#%00000000
	jr	lt,L0t_01f2
L0t_0212:
	ld	L0d_0034,r13
	ld	r14,%0008(r15)
	ld	L0d_0038,r14
	ld	r7,r13
	calr	_dysize
	cp	r2,#%016E
	jr	ne,L0t_022e
	ld	L0d_0002,#%001D
L0t_022e:
	ldk	r13,#0
	jr	L0t_023c
L0t_0232:
	ld	r2,r13
	add	r2,r13
	sub	r14,_dmsize(r2)
	inc	r13,#1
L0t_023c:
	ld	r2,r13
	add	r2,r13
	cp	r14,_dmsize(r2)
	jr	ge,L0t_0232
	ld	L0d_0002,#%001C
	lda	r2,%0001(r14)
	ld	@r12,r2
	inc	r12,#2
	ld	@r12,r13
	inc	r12,#2
	clr	L0d_003a
	ld	r2,#L0d_002a
	ldm	r12,%000A(r15),#3
	inc	r15,#16
	ret
_asctime::
	dec	r15,#8
	ldm	%0002(r15),r12,#3
	ld	%0000(r15),r7
	ld	r14,#_cbuf
	ld	r13,#L0d_0044
L0t_027e:
	ld	r2,r13
	inc	r13,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	testb	rl0
	jr	ne,L0t_027e
	ld	r2,%0000(r15)
	ld	r3,%000C(r2)
	mult	rr2,#%0003
	add	r3,#L0d_005e
	ld	r13,r3
	ld	r14,#_cbuf
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	inc	r14,#1
	ld	r2,%0000(r15)
	inc	r2,#8
	ld	r12,r2
	ld	r3,@r12
	mult	rr2,#%0003
	add	r3,#L0d_0074
	ld	r13,r3
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	dec	r12,#2
	ld	r6,@r12
	ld	r7,r14
	calr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#%0064
	ld	r7,r2
	calr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#%0064
	ld	r7,r2
	calr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#%0064
	ld	r7,r2
	calr	_ct_numb
	ld	r14,r2
	ld	r2,%0000(r15)
	cp	%000A(r2),#%0064
	jr	lt,L0t_0330
	ldb	%0001(r14),#%32
	ldb	%0002(r14),#%30
L0t_0330:
	inc	r14,#2
	ld	r6,%0000(r15)
	ld	r6,%000A(r6)
	add	r6,#%0064
	ld	r7,r14
	calr	_ct_numb
	ld	r14,r2
	ld	r2,#_cbuf
	ldm	r12,%0002(r15),#3
	inc	r15,#8
	ret
_dysize::
	dec	r15,#2
	ld	r3,r7
	exts	rr2
	div	rr2,#%0004
	test	r2
	jr	ne,L0t_0366
	ld	r2,#%016E
	jr	L0t_036a
L0t_0366:
	ld	r2,#%016D
L0t_036a:
	inc	r15,#2
	ret
_ct_numb::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	r14,r7
	inc	r14,#1
	cp	r6,#%000A
	jr	lt,L0t_039a
	ld	r3,r6
	exts	rr2
	div	rr2,#%000A
	ld	r1,r3
	ldk	r0,#0
	div	rr0,#%000A
	add	r0,#%0030
	ldb	@r14,rl0
	jr	L0t_039e
L0t_039a:
	ldb	@r14,#%20
L0t_039e:
	inc	r14,#1
	ld	r3,r6
	exts	rr2
	div	rr2,#%000A
	add	r2,#%0030
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,r14
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
_tzset::
	dec	r15,#8
	ldm	%0002(r15),r12,#3
	ld	r7,#L0d_0098+1
	call	_getenv
	ld	r14,r2
	test	r2
	jr	eq,L0t_0482
	testb	@r14
	jr	eq,L0t_0482
	ldk	r12,#3
	ld	r13,_tzname
L0t_03dc:
	testb	@r14
	jr	eq,L0t_03ea
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	extsb	r2
	jr	L0t_03ee
L0t_03ea:
	ld	r2,#%0020
L0t_03ee:
	ldb	@r13,rl2
	inc	r13,#1
	dec	r12,#1
	jr	ne,L0t_03dc
	cpb	@r14,#%2D
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
	ld	%0000(r15),r2
	test	r2
	jr	eq,L0t_040a
	inc	r14,#1
L0t_040a:
	ldk	r12,#0
	jr	L0t_0428
L0t_040e:
	cpb	@r14,#%39
	jr	gt,L0t_042e
	ld	r3,r12
	mult	rr2,#%000A
	ldb	rl2,@r14
	extsb	r2
	add	r3,r2
	sub	r3,#%0030
	ld	r12,r3
	inc	r14,#1
L0t_0428:
	cpb	@r14,#%30
	jr	ge,L0t_040e
L0t_042e:
	test	%0000(r15)
	jr	eq,L0t_043a
	ld	r2,r12
	neg	r2
	ld	r12,r2
L0t_043a:
	ld	r7,r12
	mult	rr6,#%003C
	ld	r3,r7
	exts	rr2
	ldl	rr6,rr2
	multl	rq4,#%0000003C
	ldl	_timezone,rr6
	cpb	@r14,#%00
	ldk	r2,#0
	tcc	ne,r2
	extsb	r2
	ld	_daylight,r2
	test	r2
	jr	eq,L0t_0482
	ld	r13,L0d_001e
	ldk	r12,#3
L0t_0468:
	testb	@r14
	jr	eq,L0t_0476
	ld	r2,r14
	inc	r14,#1
	ldb	rl2,@r2
	extsb	r2
	jr	L0t_047a
L0t_0476:
	ld	r2,#%0020
L0t_047a:
	ldb	@r13,rl2
	inc	r13,#1
	dec	r12,#1
	jr	ne,L0t_0468
L0t_0482:
	ldm	r12,%0002(r15),#3
	inc	r15,#8
	ret

// segment table entry 0 (segment 0), data, 156 bytes
	.psec
	.data
_dmsize::
	.word	%001F
L0d_0002:
	.word	%001C
	.word	%001F
	.word	%001E
	.word	%001F
	.word	%001E
	.word	%001F
	.word	%001F
	.word	%001E
	.word	%001F
	.word	%001E
	.word	%001F
_timezone::
	.word	%0000
	.word	%4650
_tzname::
	.word	L0d_003c
L0d_001e:
	.word	L0d_0040
_daylight::
	.word	%0001
_daytab::
	.word	%0005
L0d_0024:
	.word	%014D
	.word	%003A
	.word	%012F
L0d_002a:
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
L0d_0034:
	.word	%0000
L0d_0036:
	.word	%0000
L0d_0038:
	.word	%0000
L0d_003a:
	.word	%0000
L0d_003c:
	.word	%4553
	.word	%5400
L0d_0040:
	.word	%4544
	.word	%5400
L0d_0044:
	.word	%4461
	.word	%7920
	.word	%4D6F
	.word	%6E20
	.word	%3030
	.word	%2030
	.word	%303A
	.word	%3030
	.word	%3A30
	.word	%3020
	.word	%3139
	.word	%3030
	.word	%0A00
L0d_005e:
	.word	%5375
	.word	%6E4D
	.word	%6F6E
	.word	%5475
	.word	%6557
	.word	%6564
	.word	%5468
	.word	%7546
	.word	%7269
	.word	%5361
	.word	%7400
L0d_0074:
	.word	%4A61
	.word	%6E46
	.word	%6562
	.word	%4D61
	.word	%7241
	.word	%7072
	.word	%4D61
	.word	%794A
	.word	%756E
	.word	%4A75
	.word	%6C41
	.word	%7567
	.word	%5365
	.word	%704F
	.word	%6374
	.word	%4E6F
	.word	%7644
	.word	%6563
L0d_0098:
	.word	%0054
	.word	%5A00
