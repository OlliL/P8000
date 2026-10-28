	.seg
	.extern	_stkseg
	.extern	_printf
	.extern	_calloc
	.extern	_exit
	.extern	_rindex
	.extern	_unlink
	.extern	_umask
	.extern	_chmod
	.extern	_signal
	.extern	_sscanf
	.extern	_access
	.extern	_open
	.extern	_read
	.extern	_close
	.extern	_strchr
	.extern	_lseek
	.extern	_swap
	.extern	_mktemp
	.extern	_swab
	.extern	_creat
	.extern	_putchar
	.extern	_link
	.extern	_write
	.extern	_strcpy
	.extern	_strcat
	.extern	_strncmp
	.extern	_fstat
	.extern	_atoi
	.extern	_strrchr
	.extern	_realloc
	.extern	_free
	.code

L56:
	sub	r15,#%0016
	ldm	_stkseg+14(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	clr	|_stkseg+8|(r15)
	ldl	rr12,rr6
	ldl	rr10,rr4
	ldb	rl2,@rr12
	ldb	|_stkseg+11|(r15),rl2
	extsb	r2
	bit	r2,#7
	jr	eq,La8
	ldb	rl2,|_stkseg+11|(r15)
	andb	rl2,#%7F
	ldb	|_stkseg+11|(r15),rl2
	ldb	rl3,rl2
	extsb	r3
	inc	r3,#7
	ld	r1,r3
	exts	rr0
	div	rr0,#%0010
	ld	|_stkseg+8|(r15),r1
	ld	r3,r1
	sla	r3,#4
	inc	r3,#8
	ldb	|_stkseg+11|(r15),rl3
	jr	Ld0
La8:
	cp	<<1>>%0476,#%0000
	jr	le,Lba
	ldb	|_stkseg+11|(r15),#%0E
	jr	Ld0
Lba:
	ldb	|_stkseg+11|(r15),#%08
	jr	Ld0
Lc2:
	ldl	rr2,rr10
	inc	r11,#1
	ldb	rl4,@rr12
	ldb	@rr2,rl4
	testb	rl4
	jr	eq,Ld0
	inc	r13,#1
Ld0:
	ldb	rl2,|_stkseg+11|(r15)
	decb	|_stkseg+11|(r15),#1
	testb	rl2
	jr	ne,Lc2
	ld	r2,|_stkseg+8|(r15)
	ldm	r10,_stkseg+14(r15),#4
	add	r15,#%0016
	ret
Lee:
	sub	r15,#%0016
	ldm	_stkseg+10(r15),r8,#6
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr8,rr6
	ldl	rr12,rr6
	ldl	rr10,rr4
	clrb	|_stkseg+9|(r15)
	jr	L110
L10c:
	incb	|_stkseg+9|(r15),#1
L110:
	ldl	rr2,rr8
	inc	r9,#1
	testb	@rr2
	jr	ne,L10c
	cpb	|_stkseg+9|(r15),#%08
	jr	gt,L128
	ldl	rr6,rr12
	ldl	rr4,rr10
	calr	L56
	jr	L16a
L128:
	ldb	rl2,|_stkseg+9|(r15)
	extsb	r2
	or	r2,#%0080
	ldb	@rr10,rl2
	inc	r11,#1
	dec	r9,#1
	jr	L142
L13a:
	ldb	rl2,@rr12
	ldb	@rr10,rl2
	inc	r13,#1
	inc	r11,#1
L142:
	cpl	rr12,rr8
	jr	c,L13a
	decb	|_stkseg+9|(r15),#8
	jr	L150
L14c:
	decb	|_stkseg+9|(r15),#16
L150:
	cpb	|_stkseg+9|(r15),#%00
	jr	gt,L14c
	jr	L15e
L15a:
	clrb	@rr10
	inc	r11,#1
L15e:
	ldb	rl2,|_stkseg+9|(r15)
	decb	|_stkseg+9|(r15),#1
	testb	rl2
	jr	ne,L15a
L16a:
	ldm	r8,_stkseg+10(r15),#6
	add	r15,#%0016
	ret
L178:
	dec	r15,#12
	ldm	_stkseg+8(r15),r12,#2
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr12,rr6
	ldb	rl2,@rr6
	extsb	r2
	bit	r2,#7
	jr	eq,L1be
	ldl	rr2,rr6
	inc	r13,#1
	ldb	rl0,@rr2
	extsb	r0
	and	r0,#%007F
	cp	r0,#%0008
	jr	le,L1be
	dec	r13,#1
	ldl	rr6,rr12
	call	L6522
	ldk	r7,#1
	ldl	rr4,#%0100005E
	call	L3408
	inc	r13,#1
L1be:
	ldl	rr6,rr12
	ldl	rr4,|_stkseg+0|(r15)
	calr	L56
	ldm	r12,_stkseg+8(r15),#2
	inc	r15,#12
	ret
	dec	r15,#8
	ldm	_stkseg+0(r15),r10,#4
	ldl	rr6,#%010000A6
	call	_printf
	ld	r7,<<2>>%04FA
	call	L6fd0
	ldl	rr10,rr2
	sub	r7,r7
	call	L6fd0
L1fe:
	ldl	rr12,rr2
	jr	L210
L202:
	ldl	rr6,rr12
	calr	L220
	ldl	rr6,rr12
	call	L705e
	jr	L1fe
L210:
	cpl	rr12,rr10
	jr	c,L202
	ldm	r10,_stkseg+0(r15),#4
	inc	r15,#8
	ret
L220:
	sub	r15,#%0012
	ldm	_stkseg+10(r15),r10,#4
	ldl	rr12,rr6
	testl	rr6
	jr	ne,L236
	sub	r2,r2
	jr	L326
L236:
	ldl	rr6,rr12
	inc	r7,#8
	call	L6522
	ld	r3,rr12(#%0006)
	and	r3,#%00FF
	ldl	rr4,@rr12
	ldl	rr6,#%010000B8
	call	_printf
	ldb	rl2,rr12(#%0004)
	extsb	r2
	bit	r2,#5
	jr	eq,L26c
	ldl	rr6,#%010000D2
	call	_printf
L26c:
	ldb	rl3,rr12(#%0004)
	extsb	r3
	and	r3,#%001F
	ld	r2,r3
	ldl	rr4,#%01000102
	ld	r3,#%0008
	cpir	r2,@rr4,r3,eq
	jr	ne,L2e0
	sll	r3,#2
	neg	r3
	ldl	rr4,<<1>>%0132(r3)
	jp	@rr4
	ldl	rr10,#%010000D8
	jr	L2e6
L29e:
	inc	|_stkseg+4|(r15),#1
	dec	|_stkseg+6|(r15),#16
	jr	L316
	ldl	rr10,#%010000DE
	jr	L2e6
	ldl	rr10,#%010000E2
	jr	L2e6
	ldl	rr10,#%010000E6
	jr	L2e6
	ldl	rr10,#%010000EC
	jr	L2e6
	ldl	rr10,#%010000F2
	jr	L2e6
	ldl	rr10,#%010000F6
	jr	L2e6
	ldl	rr10,#%010000FA
	jr	L2e6
L2e0:
	ldl	rr10,#%01000100
L2e6:
	ldl	rr6,#%01000112
	ldl	rr4,rr10
	call	_printf
	ldb	rl2,rr12(#%0008)
	extsb	r2
	bit	r2,#7
	jr	eq,L324
	ldb	rl2,rr12(#%0008)
	extsb	r2
	and	r2,#%007F
	dec	r2,#8
	ld	|_stkseg+6|(r15),r2
	inc	r13,#16
	ld	|_stkseg+4|(r15),#%0001
L316:
	cp	|_stkseg+6|(r15),#%0000
	jr	gt,L29e
	ld	r2,|_stkseg+4|(r15)
	jr	L326
L324:
	ldk	r2,#1
L326:
	ldm	r10,_stkseg+10(r15),#4
	add	r15,#%0012
	ret
	dec	r15,#12
	ldm	_stkseg+6(r15),r11,#3
	ldl	rr6,#%01000136
	call	_printf
	ldk	r11,#0
	jr	L3e8
L34e:
	ld	r7,r11
	call	L6c6
	ldl	rr12,rr2
	ldl	rr6,#%0100014C
	ld	r5,r11
	ldb	rl4,@rr12
	extsb	r4
	call	_printf
	ld	r5,rr12(#%0004)
	ld	r4,rr12(#%0006)
	ld	r3,rr12(#%0008)
	ldl	rr6,#%01000158
	call	_printf
	ld	r5,rr12(#%0010)
	ld	r4,rr12(#%0012)
	ld	r3,rr12(#%0014)
	ldl	rr6,#%01000180
	call	_printf
	ld	r5,rr12(#%0022)
	ld	r4,rr12(#%0024)
	ld	r3,rr12(#%0026)
	ldl	rr6,#%010001A6
	call	_printf
	ldl	rr2,rr12(#%001E)
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr4,rr12(#%0016)
	ldl	rr2,rr12(#%001A)
	ldl	rr6,#%010001CC
	call	_printf
	ld	r5,rr12(#%0030)
	ld	r4,rr12(#%0032)
	ld	r3,rr12(#%0034)
	ldl	rr6,#%010001F2
	call	_printf
	inc	r11,#1
L3e8:
	cp	r11,<<2>>%F32C
	jr	lt,L34e
	ldm	r11,_stkseg+6(r15),#3
	inc	r15,#12
	ret
	dec	r15,#4
	ldm	_stkseg+0(r15),r12,#2
	ldl	rr6,#%01000216
	call	_printf
	ldl	rr12,<<2>>%F32E
	jp	L552
L41e:
	ld	r5,rr12(#%0010)
	and	r5,#%00FF
	ldb	rl4,@rr12
	extsb	r4
	and	r4,#%00FF
	ldl	rr6,#%0100022C
	call	_printf
	ldb	rl5,rr12(#%0001)
	extsb	r5
	and	r5,#%00FF
	ldb	rl4,rr12(#%0002)
	extsb	r4
	and	r4,#%00FF
	ldb	rl3,rr12(#%0003)
	extsb	r3
	and	r3,#%00FF
	ldl	rr6,#%0100023E
	call	_printf
	ld	r5,rr12(#%0004)
	ld	r4,rr12(#%0006)
	ld	r3,rr12(#%0008)
	ldl	rr6,#%01000268
	call	_printf
	ld	r5,rr12(#%001A)
	ld	r4,rr12(#%001C)
	ld	r3,rr12(#%001E)
	ldl	rr6,#%01000290
	call	_printf
	ldl	rr6,#%010002B6
	call	_printf
	ld	r2,rr12(#%000A)
	bit	r2,#0
	jr	eq,L4b4
	ldl	rr6,#%010002BE
	call	_printf
L4b4:
	ld	r2,rr12(#%000A)
	bit	r2,#1
	jr	eq,L4c8
	ldl	rr6,#%010002C8
	call	_printf
L4c8:
	ld	r2,rr12(#%000A)
	bit	r2,#2
	jr	eq,L4dc
	ldl	rr6,#%010002D2
	call	_printf
L4dc:
	ld	r2,rr12(#%000A)
	bit	r2,#4
	jr	eq,L4f0
	ldl	rr6,#%010002DA
	call	_printf
L4f0:
	ld	r2,rr12(#%000A)
	bit	r2,#5
	jr	eq,L504
	ldl	rr6,#%010002E4
	call	_printf
L504:
	ld	r2,rr12(#%000A)
	bit	r2,#6
	jr	eq,L518
	ldl	rr6,#%010002EE
	call	_printf
L518:
	ld	r2,rr12(#%000A)
	bit	r2,#3
	jr	eq,L52c
	ldl	rr6,#%010002F8
	call	_printf
L52c:
	ld	r2,rr12(#%000A)
	bit	r2,#8
	jr	eq,L540
	ldl	rr6,#%01000302
	call	_printf
L540:
	ldl	rr6,#%0100030E
	call	_printf
	ldl	rr2,rr12(#%0012)
	ldl	rr12,rr2
L552:
	testl	rr12
	jp	ne,L41e
	ldm	r12,_stkseg+0(r15),#2
	inc	r15,#4
	ret
	dec	r15,#8
	ldl	rr6,#%01000310
	call	_printf
	ld	r2,<<2>>%F336
	ld	|_stkseg+0|(r15),r2
	ld	r2,<<2>>%F33E
	ld	|_stkseg+2|(r15),r2
	ldl	rr6,#%0100031E
	ld	r5,<<2>>%F332
	ld	r4,<<2>>%F33A
	ld	r3,<<2>>%F334
	ld	r2,<<2>>%F33C
	call	_printf
	ldl	rr6,#%0100033E
	ld	r5,<<2>>%F338
	ld	r4,<<2>>%F340
	call	_printf
	ld	|_stkseg+4|(r15),#%0002
L5ca:
	ldl	rr6,#%0100034E
	call	_printf
	clr	|_stkseg+6|(r15)
L5da:
	ld	r2,|_stkseg+6|(r15)
	add	r2,|_stkseg+4|(r15)
	sla	r2,#3
	ld	r5,<<2>>%F332(r2)
	ldl	rr6,#%01000358
	call	_printf
	inc	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0009
	jr	lt,L5da
	ldl	rr6,#%0100035E
	call	_printf
	clr	|_stkseg+6|(r15)
L614:
	ld	r2,|_stkseg+6|(r15)
	add	r2,|_stkseg+4|(r15)
	sla	r2,#3
	ld	r5,<<2>>%F334(r2)
	ldl	rr6,#%01000368
	call	_printf
	inc	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0009
	jr	lt,L614
	ldl	rr6,#%0100036E
	call	_printf
	clr	|_stkseg+6|(r15)
L64e:
	ld	r2,|_stkseg+6|(r15)
	add	r2,|_stkseg+4|(r15)
	sla	r2,#3
	ld	r5,<<2>>%F336(r2)
	ldl	rr6,#%01000378
	call	_printf
	inc	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0009
	jr	lt,L64e
	ldl	rr6,#%0100037E
	call	_printf
	clr	|_stkseg+6|(r15)
L688:
	ld	r2,|_stkseg+6|(r15)
	add	r2,|_stkseg+4|(r15)
	sla	r2,#3
	ld	r5,<<2>>%F338(r2)
	ldl	rr6,#%01000384
	call	_printf
	inc	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0009
	jr	lt,L688
	ld	r2,|_stkseg+4|(r15)
	inc	r2,#9
	ld	|_stkseg+4|(r15),r2
	cp	r2,#%0080
	jr	lt,L5ca
	inc	r15,#8
	ret
L6c6:
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ld	|_stkseg+0|(r15),r7
	cp	r7,<<1>>%04EC
	jr	le,L6ea
	ldk	r7,#2
	ldl	rr4,#%010003C0
	call	L3408
L6ea:
	ld	r2,|_stkseg+0|(r15)
	cp	r2,<<1>>%03B8
	jr	gt,L732
	jr	L742
L6f8:
	ldk	r7,#1
	ld	r6,#%0036
	call	_calloc
	inc	<<1>>%03B8,#1
	ld	r4,<<1>>%03B8
	sla	r4,#2
	ldl	rr6,<<1>>%03BA
	add	r7,r4
	ldl	@rr6,rr2
	ldl	rr12,rr2
	testl	rr2
	jr	ne,L732
	ldk	r7,#2
	ldl	rr4,#%010003F8
	call	L3408
L732:
	ld	r2,|_stkseg+0|(r15)
	cp	r2,<<1>>%03B8
	jr	gt,L6f8
	ldl	rr2,rr12
	jr	L756
L742:
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#2
	ldl	rr4,<<1>>%03BA
	ldl	rr6,rr4(r2)
	ldl	rr2,rr6
L756:
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
L762:
	dec	r15,#2
	ld	|_stkseg+0|(r15),r7
	ldk	r6,#4
	call	_calloc
	ldl	<<1>>%03BA,rr2
	testl	rr2
	jr	ne,L788
	ldk	r7,#2
	ldl	rr4,#%01000420
	call	L3408
L788:
	inc	r15,#2
	ret
_main::
	sub	r15,#%001E
	ldm	_stkseg+22(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	calr	Laba
	cp	|_stkseg+6|(r15),#%0001
	jr	ne,L7b0
	ldk	r7,#4
	call	_exit
L7b0:
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr2,@rr6
	ldl	rr6,rr2
	ld	r5,#%002F
	call	_rindex
	ldl	<<2>>%E4D6,rr2
	testl	rr2
	jr	ne,L7da
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	ldl	<<2>>%E4D6,rr4
	jr	L7e0
L7da:
	inc	<<2>>%E4D8,#1
L7e0:
	clr	|_stkseg+8|(r15)
L7e4:
	ld	r2,|_stkseg+8|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+8|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+8|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F332
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+8|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%FFFF
	inc	|_stkseg+8|(r15),#1
	cp	|_stkseg+8|(r15),#%0080
	jr	lt,L7e4
	ld	r7,<<1>>%04EE
	call	L6ffe
	ld	r7,|_stkseg+6|(r15)
	ldl	rr4,|_stkseg+0|(r15)
	calr	Lb04
	ld	r7,<<1>>%04EC
	call	L762
	clr	<<1>>%4A46
	ld	|_stkseg+8|(r15),#%0001
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	ldl	<<1>>%4A36,rr2
	jp	La0c
L880:
	subl	rr4,rr4
	ldl	<<2>>%E4C0,rr4
	ldl	rr4,<<1>>%4A36
	ldl	rr10,@rr4
	inc	<<1>>%4A38,#4
	cpb	@rr10,#%2D
	jp	ne,L9d4
	ldk	r13,#1
L8a2:
	ldb	rl2,rr10(r13)
	testb	rl2
	jp	eq,L9d8
	ldb	rl3,rr10(r13)
	extsb	r3
	ld	r2,r3
	sub	r2,#%0041
	cp	r2,#%0039
	jr	ugt,L9be
	rl	r2,#2
	ldl	rr4,<<1>>%0584(r2)
	jp	@rr4
	ld	r3,|_stkseg+8|(r15)
	ld	|_stkseg+10|(r15),r3
	ld	|_stkseg+8|(r15),#%FFFF
	ldl	rr4,<<1>>%4A36
	ldl	|_stkseg+12|(r15),rr4
	ldl	rr6,|_stkseg+0|(r15)
	ldl	|_stkseg+18|(r15),rr6
	ld	r0,|_stkseg+6|(r15)
	ld	|_stkseg+16|(r15),r0
	ldl	rr6,<<1>>%4A40
	ldl	|_stkseg+0|(r15),rr6
	ld	r1,<<1>>%4A44
	ld	|_stkseg+6|(r15),r1
	ldl	rr6,|_stkseg+0|(r15)
	ldl	<<1>>%4A36,rr6
	inc	<<1>>%4A46,#1
	jr	L9d8
	inc	r13,#1
	inc	|_stkseg+8|(r15),#1
L91e:
	inc	<<1>>%4A38,#4
	inc	r13,#1
	jr	L8a2
	ld	r3,r13
	dec	r3,#1
	ldl	rr6,rr10
	add	r7,r3
	jr	L9d6
	test	<<2>>%E49C
	jr	eq,L948
	ldk	r7,#2
	ldl	rr4,#%010004F2
	call	L3408
L948:
	ld	<<2>>%E4A8,#%0001
	inc	|_stkseg+8|(r15),#1
	ld	r2,|_stkseg+8|(r15)
	cp	r2,|_stkseg+6|(r15)
	jr	lt,L972
	ldb	rl3,rr10(r13)
	extsb	r3
	ldl	rr4,#%01000516
	ldk	r7,#2
	call	L3408
L972:
	ldl	rr6,rr10
	add	r7,r13
	ldl	rr4,<<1>>%4A36
	ldl	rr2,@rr4
	ldl	rr4,rr2
	calr	L1070
	test	r2
	jr	eq,L91e
	inc	r13,#1
	jr	L91e
	test	<<2>>%E4A8
	jr	eq,L9a0
	ldk	r7,#2
	ldl	rr4,#%01000528
	call	L3408
L9a0:
	inc	<<2>>%E49C,#1
	ldl	rr6,rr10
	add	r7,r13
	ldl	rr4,#%7F000008
	add	r5,r15
	ld	r3,|_stkseg+6|(r15)
	call	L3c16
	jr	L9d8
L9be:
	ldb	rl3,rr10(r13)
	extsb	r3
	ldl	rr4,#%0100054C
	ldk	r7,#2
	call	L3408
	jr	L9d8
L9d4:
	ldl	rr6,rr10
L9d6:
	calr	L1364
L9d8:
	inc	|_stkseg+8|(r15),#1
	jr	La0c
L9de:
	ld	r2,|_stkseg+10|(r15)
	inc	r2,#2
	ld	|_stkseg+8|(r15),r2
	inc	|_stkseg+14|(r15),#4
	ldl	rr2,|_stkseg+12|(r15)
	ldl	<<1>>%4A36,rr2
	ldl	rr4,|_stkseg+18|(r15)
	ldl	|_stkseg+0|(r15),rr4
	ld	r6,|_stkseg+16|(r15)
	ld	|_stkseg+6|(r15),r6
	clr	<<1>>%4A46
La0c:
	ld	r2,|_stkseg+8|(r15)
	cp	r2,|_stkseg+6|(r15)
	jp	lt,L880
	test	<<1>>%4A46
	jr	ne,L9de
	test	<<2>>%E4A2
	jr	eq,La44
	testb	<<1>>%42B2
	jr	eq,La44
	ldl	rr6,#%0100055A
	ldl	rr4,<<2>>%E4D6
	call	_printf
La44:
	ld	r7,|_stkseg+6|(r15)
	ldl	rr4,|_stkseg+0|(r15)
	calr	L11ea
	ldm	r10,_stkseg+22(r15),#4
	add	r15,#%001E
	ret
La5c:
	dec	r15,#2
	ldl	rr6,#%0100066C
	call	_unlink
	test	<<1>>%04E8
	jr	ne,Laaa
	test	<<2>>%E496
	jr	eq,La82
	test	<<1>>%04E6
	jr	ne,Laaa
La82:
	test	<<2>>%E4A2
	jr	ne,Laaa
	sub	r7,r7
	call	_umask
	com	r2
	and	r2,#%01FF
	ld	|_stkseg+0|(r15),r2
	ldl	rr6,<<1>>%0494
	ld	r5,r2
	call	_chmod
Laaa:
	ld	r7,<<1>>%04E8
	call	_exit
	inc	r15,#2
	ret
Laba:
	ldk	r7,#2
	ldl	rr4,#%00000001
	call	_signal
	cpl	rr2,#%00000001
	jr	eq,Lade
	ldk	r7,#2
	ldl	rr4,#%00000A5C
	call	_signal
Lade:
	ldk	r7,#15
	ldl	rr4,#%00000001
	call	_signal
	cpl	rr2,#%00000001
	jr	eq,Lb02
	ldk	r7,#15
	ldl	rr4,#%00000A5C
	call	_signal
Lb02:
	ret
Lb04:
	sub	r15,#%0018
	ldm	|_stkseg+0|(r15),r4,#4
	clr	|_stkseg+18|(r15)
	jp	Lee2
Lb18:
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	cpb	@rr4,#%2D
	jp	ne,Lee2
	ldl	rr6,@rr2
	ldb	rl5,rr6(#%0001)
	extsb	r5
	ld	r4,r5
	ldl	rr2,#%0100081A
	ld	r5,#%0011
	cpir	r4,@rr2,r5,eq
	jp	ne,Lee2
	sll	r5,#2
	neg	r5
	ldl	rr2,<<1>>%087C(r5)
	jp	@rr2
	test	|_stkseg+18|(r15)
	jr	eq,Lb68
	ldk	r7,#2
	ldl	rr4,#%01000672
	call	L3408
Lb68:
	cp	|_stkseg+6|(r15),#%0001
	jr	gt,Lb7e
	ldk	r7,#2
	ldl	rr4,#%01000686
	call	L3408
Lb7e:
	inc	|_stkseg+2|(r15),#4
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	ldl	<<1>>%4A3C,rr4
	dec	|_stkseg+6|(r15),#1
	ldl	rr6,|_stkseg+0|(r15)
	ldl	|_stkseg+14|(r15),rr6
	ld	r0,|_stkseg+6|(r15)
	ld	|_stkseg+12|(r15),r0
	ldl	rr6,<<1>>%4A3C
	calr	Lf26
	ldl	rr2,<<1>>%4A40
	dec	r3,#4
	ldl	|_stkseg+0|(r15),rr2
	ld	r2,<<1>>%4A44
	inc	r2,#1
	ld	|_stkseg+6|(r15),r2
	inc	|_stkseg+18|(r15),#1
	inc	<<1>>%4A46,#1
	jp	Lee2
	inc	<<2>>%E4AC,#1
	jp	Lee2
	inc	<<2>>%E4A6,#1
	jp	Lee2
	ldl	rr2,#%7F00000A
	add	r3,r15
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr4,@rr6
	ldl	rr6,rr4
	ldl	rr4,#%01000698
	call	_sscanf
	cp	r2,#%0001
	jr	ge,Lc1a
	ldk	r7,#2
	ldl	rr4,#%0100069E
	call	L3408
Lc1a:
	inc	<<2>>%E49E,#1
	ld	r7,|_stkseg+10|(r15)
	ld	r6,#%0072
	call	L42f8
	jp	Lee2
	inc	<<2>>%E4A2,#1
	inc	<<2>>%E496,#1
	jp	Lee2
	inc	<<2>>%E4A4,#1
	inc	<<2>>%E4AA,#1
	jp	Lee2
	cp	|_stkseg+6|(r15),#%0001
	jr	gt,Lc6e
	ldk	r7,#2
	ldl	rr4,#%010006BA
	call	L3408
Lc6e:
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr2,rr6(#%0004)
	ldl	rr6,rr2
	call	L6b5a
	ldl	|_stkseg+20|(r15),rr2
	ldl	rr6,rr2
	call	L6bae
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	ldb	rl6,rr4(#%0001)
	cpb	rl6,#%65
	jr	ne,Lca6
	ldl	rr4,<<2>>%04FC
	ldl	<<2>>%E492,rr4
Lca6:
	dec	|_stkseg+6|(r15),#1
	inc	|_stkseg+2|(r15),#4
	jp	Lee2
	inc	<<2>>%E498,#1
	jp	Lee2
	inc	<<2>>%E49A,#1
	ld	<<1>>%3E80,#%0001
	ld	<<1>>%3E7C,#%0001
	test	<<1>>%3E82
	jp	ne,Lee2
	inc	<<1>>%3E82,#1
	jp	Lee2
	inc	<<2>>%E4A0,#1
	jp	Lee2
	cp	|_stkseg+6|(r15),#%0001
	jr	gt,Ld10
	ldk	r7,#2
	ldl	rr4,#%010006D0
	call	L3408
Ld10:
	inc	|_stkseg+2|(r15),#4
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	ldl	<<1>>%0494,rr4
	ldl	rr6,rr4
	sub	r5,r5
	call	_access
	test	r2
	jr	ne,Ld54
	ldl	rr6,<<1>>%0494
	ldk	r5,#2
	call	_access
	test	r2
	jr	eq,Ld54
	ldk	r7,#2
	ldl	rr4,#%010006E0
	ldl	rr2,<<1>>%0494
	call	L3408
Ld54:
	dec	|_stkseg+6|(r15),#1
	inc	<<2>>%E4BC,#1
	jp	Lee2
	cp	|_stkseg+6|(r15),#%0001
	jr	gt,Ld7a
	ldk	r7,#2
	ldl	rr4,#%010006F6
	call	L3408
Ld7a:
	ldl	rr4,|_stkseg+0|(r15)
	ldl	rr6,@rr4
	ldb	rl3,rr6(#%0002)
	extsb	r3
	ld	r2,r3
	cp	r3,#%0070
	jr	eq,Le04
	cp	r3,#%0073
	jr	ne,Le6c
	ldl	rr6,rr4
	ldl	rr4,rr6(#%0004)
	ldl	rr6,rr4
	ldl	rr2,#%7F000008
	add	r3,r15
	ldl	rr4,#%0100070E
	call	_sscanf
	cp	r2,#%0001
	jr	eq,Ldc4
	ldk	r7,#2
	ldl	rr4,#%01000712
	call	L3408
Ldc4:
	ldk	r2,#0
	ld	r3,|_stkseg+8|(r15)
	addl	rr2,#%00000002
	slal	rr2,#4
	cpl	rr2,#%0000FFF8
	jr	le,Ldea
	ldk	r7,#2
	ldl	rr4,#%01000744
	call	L3408
Ldea:
	ld	r2,|_stkseg+8|(r15)
	ld	<<1>>%04EE,r2
	ld	r7,|_stkseg+8|(r15)
	call	L702a
	jp	Lca6
Le04:
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr2,rr6(#%0004)
	ldl	rr6,rr2
	ldl	rr2,#%7F000008
	add	r3,r15
	ldl	rr4,#%01000770
	call	_sscanf
	cp	r2,#%0001
	jr	eq,Le36
	ldk	r7,#2
	ldl	rr4,#%01000774
	call	L3408
Le36:
	ldk	r2,#0
	ld	r3,|_stkseg+8|(r15)
	addl	rr2,#%00000002
	slal	rr2,#2
	cpl	rr2,#%0000FFF8
	jr	le,Le5c
	ldk	r7,#2
	ldl	rr4,#%010007AE
	call	L3408
Le5c:
	ld	r2,|_stkseg+8|(r15)
	ld	<<1>>%04EC,r2
	jp	Lca6
Le6c:
	ldk	r7,#2
	ldl	rr4,#%010007E2
	call	L3408
	jp	Lca6
	inc	<<2>>%E4BA,#1
	jr	Lee2
	ld	r7,#%007F
	ld	r6,#%0073
	call	L42f8
	ld	<<2>>%E4AE,#%0001
	jr	Lee2
	ldl	rr4,|_stkseg+0|(r15)
	ldl	rr6,@rr4
	ldb	rl3,rr6(#%0002)
	extsb	r3
	ld	r2,r3
	cp	r3,#%006C
	jr	eq,Led2
	cp	r3,#%0073
	jr	eq,Ledc
	ldl	rr6,@rr4
	ldb	rl3,rr6(#%0002)
	extsb	r3
	ldl	rr4,#%010007FE
	ldk	r7,#2
	call	L3408
	jr	Lee2
Led2:
	ld	<<1>>%04EA,#%0001
	jr	Lee2
Ledc:
	clr	<<1>>%04EA
Lee2:
	dec	|_stkseg+6|(r15),#1
	inc	|_stkseg+2|(r15),#4
	jr	Lf04
Leec:
	inc	|_stkseg+16|(r15),#4
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+0|(r15),rr2
	dec	|_stkseg+12|(r15),#1
	ld	r4,|_stkseg+12|(r15)
	ld	|_stkseg+6|(r15),r4
Lf04:
	cp	|_stkseg+6|(r15),#%0000
	jp	gt,Lb18
	ld	r2,<<1>>%4A46
	dec	<<1>>%4A46,#1
	test	r2
	jr	ne,Leec
	add	r15,#%0018
	ret
Lf26:
	sub	r15,#%0014
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,#%01006988
	ldl	|_stkseg+14|(r15),rr2
	ldl	rr2,#%01006988
	ldl	<<1>>%4A48,rr2
	ldl	rr2,rr6
	sub	r5,r5
	call	_open
	ld	|_stkseg+4|(r15),r2
	cp	r2,#%0000
	jr	ge,Lf66
	ldk	r7,#2
	ldl	rr4,#%01000880
	call	L3408
Lf66:
	clr	|_stkseg+12|(r15)
	clr	|_stkseg+8|(r15)
	jr	L1024
Lf70:
	test	|_stkseg+12|(r15)
	jr	ne,L1024
	inc	|_stkseg+12|(r15),#1
	ldl	rr2,|_stkseg+14|(r15)
	clrb	@rr2
	inc	|_stkseg+16|(r15),#1
	ldl	rr2,|_stkseg+14|(r15)
	cpl	rr2,#%01009098
	jr	ule,Lf9e
	ldk	r7,#2
	ldl	rr4,#%010008B2
	call	L3408
Lf9e:
	inc	|_stkseg+8|(r15),#1
	cp	|_stkseg+8|(r15),#%07D0
	jr	le,Lfb8
	ldk	r7,#2
	ldl	rr4,#%010008D2
	call	L3408
Lfb8:
	ld	r2,|_stkseg+8|(r15)
	sla	r2,#2
	ldl	rr4,#%01004A48
	add	r5,r2
	ldl	rr2,|_stkseg+14|(r15)
	ldl	@rr4,rr2
	jr	L1024
Lfd0:
	cp	|_stkseg+6|(r15),#%FFFF
	jr	ne,Lfe6
	ldk	r7,#2
	ldl	rr4,#%0100089C
	call	L3408
Lfe6:
	ldb	rl2,|_stkseg+11|(r15)
	extsb	r2
	ldb	rl3,<<1>>%1AAD(r2)
	extsb	r3
	bit	r3,#3
	jr	ne,Lf70
	clr	|_stkseg+12|(r15)
	ldl	rr2,|_stkseg+14|(r15)
	ldb	rl4,|_stkseg+11|(r15)
	ldb	@rr2,rl4
	inc	|_stkseg+16|(r15),#1
	ldl	rr2,|_stkseg+14|(r15)
	cpl	rr2,#%01009098
	jr	ule,L1024
	ldk	r7,#2
	ldl	rr4,#%010008F2
	call	L3408
L1024:
	ldl	rr4,#%7F00000B
	add	r5,r15
	ld	r7,|_stkseg+4|(r15)
	ldk	r3,#1
	call	_read
	cp	r2,#%0000
	ldk	r2,#0
	tcc	ne,r2
	ld	|_stkseg+6|(r15),r2
	test	r2
	jr	ne,Lfd0
	ldl	rr2,#%01004A48
	ldl	<<1>>%4A40,rr2
	ld	r2,|_stkseg+8|(r15)
	ld	<<1>>%4A44,r2
	ld	r7,|_stkseg+4|(r15)
	call	_close
	sub	r2,r2
	add	r15,#%0014
	ret
L1070:
	sub	r15,#%001A
	ldm	_stkseg+18(r15),r10,#4
	ldm	|_stkseg+4|(r15),r4,#4
	incb	<<1>>%42B2,#1
	ldl	rr0,rr6
	inc	r1,#1
	ldl	rr6,rr0
	ldb	rl3,@rr6
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%0100093E
	ld	r6,#%0004
	cpir	r2,@rr4,r6,eq
	jr	ne,L10d2
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%099A(r6)
	jp	@rr4
	ldk	r13,#1
	jr	L10e4
L10b6:
	cp	r13,#%0002
	jr	eq,L10c2
	cp	r13,#%0003
	jr	ne,L10f0
L10c2:
	ldk	r12,#1
	jr	L10f2
	ldk	r13,#2
	jr	L10e4
	ldk	r13,#3
	jr	L10e4
	ldk	r13,#0
	jr	L10e4
L10d2:
	ldk	r7,#2
	ldl	rr4,#%01000912
	ldl	rr2,|_stkseg+8|(r15)
	call	L3408
L10e4:
	ldl	rr10,|_stkseg+4|(r15)
	test	<<2>>%E49A
	jr	ne,L10b6
L10f0:
	ldk	r12,#0
L10f2:
	ld	r3,r12
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(r13)
	testb	rl2
	jr	eq,L1140
	ld	r3,r12
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(r13)
	extsb	r2
	ld	|_stkseg+0|(r15),r2
	ldl	rr2,|_stkseg+8|(r15)
	ldb	rl4,rr2(#%0001)
	extsb	r4
	ld	|_stkseg+2|(r15),r4
	ldk	r7,#2
	ldl	rr4,#%01000946
	ldl	rr2,|_stkseg+8|(r15)
	call	L3408
L1140:
	ld	r3,r12
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	add	r5,r13
	ldl	rr2,|_stkseg+8|(r15)
	ldb	rl6,@rr2
	ldb	@rr4,rl6
	ldl	rr4,#%01000966
	ldl	|_stkseg+12|(r15),rr4
	cpb	@rr10,#%30
	jr	eq,L1174
	ldl	rr4,#%0100096C
	ldl	|_stkseg+12|(r15),rr4
	jr	L1190
L1174:
	ldb	rl2,rr10(#%0001)
	extsb	r2
	or	r2,#%0020
	cp	r2,#%0078
	jr	ne,L1190
	inc	r11,#2
	ldl	rr2,#%01000972
	ldl	|_stkseg+12|(r15),rr2
L1190:
	ld	r3,r12
	mult	rr2,#%000A
	ldl	rr4,#%01004534
	add	r5,r3
	ld	r2,r13
	add	r2,r13
	add	r5,r2
	ldl	rr2,rr4
	ldl	rr4,|_stkseg+12|(r15)
	ldl	rr6,rr10
	call	_sscanf
	cp	r2,#%0001
	jr	eq,L11d2
	ldl	rr2,|_stkseg+4|(r15)
	ldl	|_stkseg+0|(r15),rr2
	ldk	r7,#2
	ldl	rr4,#%01000978
	ldl	rr2,|_stkseg+8|(r15)
	call	L3408
L11d2:
	test	r13
	jr	ne,L11da
	sub	r2,r2
	jr	L11dc
L11da:
	ldk	r2,#1
L11dc:
	ldm	r10,_stkseg+18(r15),#4
	add	r15,#%001A
	ret
L11ea:
	sub	r15,#%0026
	ldm	_stkseg+26(r15),r8,#6
	ldm	|_stkseg+0|(r15),r4,#4
	clr	|_stkseg+8|(r15)
	subl	rr2,rr2
	ldl	<<2>>%E4C0,rr2
	call	L44e6
	calr	L2056
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	ldl	rr8,rr2
	clr	<<2>>%F32C
	clr	<<1>>%3C02
	ldl	rr2,#%01003E8A
	ldl	<<1>>%0486,rr2
	ldk	r13,#1
	jp	L1340
L1238:
	popl	rr10,@rr8
	cpb	@rr10,#%2D
	jr	ne,L1310
	ldk	r12,#1
L1242:
	ldb	rl2,rr10(r12)
	testb	rl2
	jr	eq,L1318
	ldb	rl3,rr10(r12)
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%0100099E
	ld	r6,#%000C
	cpir	r2,@rr4,r6,eq
	jr	ne,L12be
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%09E2(r6)
	jp	@rr4
	ld	|_stkseg+12|(r15),r13
	ld	r13,#%FFFF
	ldl	|_stkseg+14|(r15),rr8
	ldl	rr4,|_stkseg+0|(r15)
	ldl	|_stkseg+18|(r15),rr4
	ld	r3,|_stkseg+6|(r15)
	ld	|_stkseg+10|(r15),r3
	ldl	rr6,<<1>>%4A40
	ldl	|_stkseg+0|(r15),rr6
	ld	r0,<<1>>%4A44
	ld	|_stkseg+6|(r15),r0
	ldl	rr8,|_stkseg+0|(r15)
	ld	|_stkseg+8|(r15),#%0001
	jr	L1318
	ld	r2,r12
	inc	r2,#1
	ldb	rl3,rr10(r2)
	testb	rl3
	jr	eq,L12ba
	inc	r12,#1
L12ba:
	inc	r13,#1
	inc	r9,#4
L12be:
	inc	r12,#1
	jr	L1242
	inc	r12,#1
	jr	L12be
	dec	r12,#1
	ldl	rr2,rr10
	add	r3,r12
	ldb	@rr2,#%2D
	ldl	rr6,rr10
	add	r7,r12
	jr	L1312
	ldl	rr6,rr10
	ld	r5,#%003A
	call	_strchr
	ldl	|_stkseg+22|(r15),rr2
	testl	rr2
	jr	eq,L12f6
	ldb	rl4,rr2(#%0002)
	testb	rl4
	jr	ne,L12f6
	inc	r13,#1
	inc	r9,#4
L12f6:
	ldl	rr2,@rr8
	cpb	@rr2,#%2B
	jr	ne,L1318
L12fe:
	inc	r13,#1
	inc	r9,#4
	ldl	rr2,@rr8
	cpb	@rr2,#%2B
	jr	ne,L12fe
	inc	r13,#1
	inc	r9,#4
	jr	L1318
L1310:
	ldl	rr6,rr10
L1312:
	call	L23fe
L1318:
	inc	r13,#1
	jr	L1340
L131c:
	ld	r2,|_stkseg+12|(r15)
	inc	r2,#2
	ld	r13,r2
	inc	|_stkseg+16|(r15),#4
	ldl	rr8,|_stkseg+14|(r15)
	ldl	rr2,|_stkseg+18|(r15)
	ldl	|_stkseg+0|(r15),rr2
	ld	r4,|_stkseg+10|(r15)
	ld	|_stkseg+6|(r15),r4
	clr	|_stkseg+8|(r15)
L1340:
	cp	r13,|_stkseg+6|(r15)
	jp	lt,L1238
	test	|_stkseg+8|(r15)
	jr	ne,L131c
	call	L2cbc
	ldm	r8,_stkseg+26(r15),#6
	add	r15,#%0026
	ret
L1364:
	dec	r15,#12
	ldm	_stkseg+8(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	call	L3144
	cp	r2,#%0003
	jp	ugt,L148c
	rl	r2,#2
	ldl	rr4,<<1>>%09FC(r2)
	jp	@rr4
	sub	r7,r7
	subl	rr4,rr4
	calr	L1696
	jr	L148c
	ldl	rr2,#%00000001
L139c:
	ldl	|_stkseg+4|(r15),rr2
	ldl	rr6,rr2
	calr	L14a4
	test	r2
	jr	eq,L148c
	ldl	rr2,<<2>>%044E
	addl	rr2,#%0000001B
	sral	rr2,#1
	addl	rr2,|_stkseg+4|(r15)
	jr	L139c
	ldl	rr6,<<2>>%044E
	extsl	rq4
	divl	rq4,#%0000000C
	ld	<<1>>%3E88,r7
	ldl	rr6,<<2>>%044E
	call	L7112
	ldl	<<1>>%3E84,rr2
	ld	r7,<<2>>%E4BE
	ldl	rr4,#%0000001C
	sub	r3,r3
	call	_lseek
	ld	r3,<<1>>%3E88
	mult	rr2,#%000C
	ldl	rr4,<<1>>%3E84
	ld	r7,<<2>>%E4BE
	call	_read
	ldl	rr6,<<1>>%3E84
	ld	r5,<<1>>%3E88
	call	L6f46
L1426:
	calr	L1560
	test	r2
	jr	ne,L1426
	ldl	rr2,<<1>>%0486
	ldl	rr4,#%FFFFFFFF
	ldl	@rr2,rr4
	inc	<<1>>%0488,#4
	jr	L148c
	sub	r7,r7
	ldl	rr4,#%010009E6
	call	L3408
	ldl	rr2,<<2>>%044E
	addl	rr2,#%0000001B
	sral	rr2,#1
	addl	rr2,#%00000001
L1466:
	ldl	|_stkseg+4|(r15),rr2
	jr	L1482
L146c:
	ldl	rr2,<<2>>%044E
	addl	rr2,#%0000001B
	sral	rr2,#1
	addl	rr2,|_stkseg+4|(r15)
	jr	L1466
L1482:
	ldl	rr6,|_stkseg+4|(r15)
	calr	L14a4
	test	r2
	jr	ne,L146c
L148c:
	ld	r7,<<2>>%E4BE
	call	_close
	ldm	r12,_stkseg+8(r15),#2
	inc	r15,#12
	ret
L14a4:
	dec	r15,#8
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+0|(r15)
	ld	r3,#%001C
	call	L2f3a
	cp	<<2>>%041E,#%0000
	jr	gt,L14e0
	ldl	rr2,<<1>>%0486
	ldl	rr4,#%FFFFFFFF
	ldl	@rr2,rr4
	inc	<<1>>%0488,#4
	sub	r2,r2
	jr	L155c
L14e0:
	ldl	rr6,#%02000438
	ld	r5,#%001A
	call	L2e76
	ldl	rr6,#%7F000004
	add	r7,r15
	ldk	r5,#2
	call	L2e76
	test	<<2>>%E4B6
	jr	eq,L1516
	ld	r7,|_stkseg+4|(r15)
	call	_swap
	ld	|_stkseg+4|(r15),r2
L1516:
	cp	|_stkseg+4|(r15),#%E707
	jr	eq,L1536
	cp	|_stkseg+4|(r15),#%E711
	jr	eq,L1536
	cp	|_stkseg+4|(r15),#%E607
	jr	eq,L1536
	cp	|_stkseg+4|(r15),#%E611
	jr	ne,L155a
L1536:
	ldl	rr4,|_stkseg+0|(r15)
	addl	rr4,#%0000000D
	ldk	r7,#1
	calr	L1696
	test	r2
	jr	eq,L155a
	ldl	rr2,<<1>>%0486
	ldl	rr4,|_stkseg+0|(r15)
	ldl	@rr2,rr4
	inc	<<1>>%0488,#4
L155a:
	ldk	r2,#1
L155c:
	inc	r15,#8
	ret
L1560:
	dec	r15,#16
	ldl	rr2,<<1>>%0486
	ldl	|_stkseg+10|(r15),rr2
	ldl	rr4,<<1>>%3E84
	ldl	|_stkseg+2|(r15),rr4
	clr	|_stkseg+0|(r15)
	jr	L1646
L157c:
	ld	r5,|_stkseg+0|(r15)
	mult	rr4,#%000C
	ldl	rr6,<<1>>%3E84
	add	r7,r5
	cpl	rr6,|_stkseg+2|(r15)
	jr	c,L1642
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	inc	r3,#4
	ldl	rr4,<<1>>%3E84
	ldb	rl2,rr4(r3)
	extsb	r2
	and	r2,#%0080
	test	r2
	jr	eq,L15dc
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	inc	r3,#4
	add	r5,r3
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	inc	r3,#4
	ldl	rr6,<<1>>%3E84
	ldb	rl2,rr6(r3)
	extsb	r2
	and	r2,#%007F
	add	r5,r2
	ldl	|_stkseg+2|(r15),rr4
L15dc:
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	inc	r3,#4
	ldl	rr6,<<1>>%3E84
	add	r7,r3
	call	L6b84
	ldl	rr4,@rr2
	ldl	|_stkseg+6|(r15),rr4
	testl	rr4
	jr	eq,L1642
	ldl	rr2,rr4
	ldb	rl4,rr2(#%0004)
	cpb	rl4,#%20
	jr	ne,L1642
	test	<<2>>%E4B6
	jr	eq,L1628
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	ldl	rr6,<<1>>%3E84
	add	r7,r3
	call	L65ec
L1628:
	ld	r3,|_stkseg+0|(r15)
	mult	rr2,#%000C
	ldl	rr6,<<1>>%3E84
	ldl	rr4,rr6(r3)
	sral	rr4,#1
	ldl	rr6,rr4
	calr	L14a4
L1642:
	inc	|_stkseg+0|(r15),#1
L1646:
	ld	r2,|_stkseg+0|(r15)
	cp	r2,<<1>>%3E88
	jr	c,L157c
	ldl	rr2,|_stkseg+10|(r15)
	cpl	rr2,<<1>>%0486
	ldk	r2,#0
	tcc	ne,r2
	inc	r15,#16
	ret
L1664:
	dec	r15,#14
	ldm	|_stkseg+0|(r15),r4,#4
	ldk	r2,#0
	ld	r3,|_stkseg+6|(r15)
	ld	r0,r2
	ld	r1,|_stkseg+4|(r15)
	addl	rr2,rr0
	ldl	|_stkseg+8|(r15),rr2
	cpl	rr2,#%00010000
	jr	lt,L168e
	ldk	r7,#1
	call	L3408
L168e:
	ld	r2,|_stkseg+10|(r15)
	inc	r15,#14
	ret
L1696:
	sub	r15,#%002C
	ldm	_stkseg+36(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr6,|_stkseg+0|(r15)
	ldk	r5,#1
	call	L3516
	clr	|_stkseg+12|(r15)
	ld	|_stkseg+8|(r15),#%0001
	ld	r2,<<2>>%04FA
	ld	|_stkseg+14|(r15),r2
	ld	r3,<<2>>%0468
	and	r3,#%0001
	cp	r3,#%0001
	jr	ne,L16ec
	ldk	r7,#1
	ldl	rr4,#%01000A0C
	call	L3408
	jp	L18e2
L16ec:
	ld	r2,<<2>>%0436
	add	r2,<<2>>%0434
	res	r2,#0
	rr	r2,#1
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<2>>%0454
	addl	rr4,<<2>>%0458
	addl	rr4,|_stkseg+0|(r15)
	ldl	|_stkseg+0|(r15),rr4
	ldl	rr6,#%02000414
	ld	r3,<<2>>%0460
	call	L2f3a
	jp	L186a
L172c:
	test	<<2>>%E4AC
	jr	eq,L1742
	cpb	<<2>>%0472,#%4C
	jp	eq,L186a
L1742:
	ld	r2,|_stkseg+10|(r15)
	inc	r2,#1
	add	r2,|_stkseg+8|(r15)
	ld	|_stkseg+8|(r15),r2
	jp	L186a
L1756:
	call	L6684
	ld	|_stkseg+10|(r15),r2
	ldb	rl2,<<2>>%046E
	ldb	|_stkseg+17|(r15),rl2
	test	<<2>>%E4A6
	jr	eq,L178a
	ldb	rl3,rl2
	extsb	r3
	and	r3,#%001F
	ldb	|_stkseg+19|(r15),rl3
	cpb	rl3,#%01
	jr	eq,L186a
	cpb	rl3,#%04
	jr	gt,L186a
L178a:
	ldb	rl2,|_stkseg+17|(r15)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L172c
	call	L6a58
	ldl	|_stkseg+32|(r15),rr2
	ldl	rr6,rr2
	call	L6bae
	test	r2
	jr	eq,L17c0
	ldl	rr12,<<2>>%04FC
	ldb	rl2,rr12(#%0004)
	cpb	rl2,#%20
	jr	ne,L1812
	jr	L186a
L17c0:
	ldl	rr12,<<2>>%04FC
	ldb	rl2,rr12(#%0004)
	cpb	rl2,#%20
	jr	eq,L17e6
	test	|_stkseg+6|(r15)
	jr	eq,L186a
	ldl	rr6,rr12
	ldl	rr4,#%0200046A
	call	L3b58
	jr	L186a
L17e6:
	cpb	<<2>>%046E,#%20
	jr	ne,L17fe
	ldl	rr2,<<2>>%046A
	cpl	rr2,@rr12
	jr	le,L186a
	ldl	rr4,rr2
	jr	L1868
L17fe:
	ldl	rr2,@rr12
	testl	rr2
	jr	eq,L180e
	cpb	<<2>>%046E,#%22
	jr	eq,L186a
L180e:
	inc	|_stkseg+12|(r15),#1
L1812:
	ldl	rr2,rr12
	inc	r3,#6
	ld	r4,<<1>>%3C02
	add	r4,<<2>>%0470
	ld	@rr2,r4
	ld	r7,rr12(#%0006)
	call	L6c6
	ldl	rr10,rr2
	ld	r4,rr2(#%0010)
	ld	<<2>>%E4D0,r4
	ld	r2,rr10(#%0012)
	ld	<<2>>%E4D2,r2
	ld	r2,rr10(#%0014)
	ld	<<2>>%E4D4,r2
	clr	@rr12
	call	L6da8
	ldl	rr2,rr12
	inc	r3,#4
	ldb	rl4,<<2>>%046E
	ldb	@rr2,rl4
	ldl	rr2,<<2>>%046A
L1868:
	ldl	@rr12,rr2
L186a:
	cp	<<2>>%041E,#%0000
	jp	gt,L1756
	test	|_stkseg+6|(r15)
	jr	eq,L1884
	test	|_stkseg+12|(r15)
	jr	eq,L18a4
L1884:
	ld	r6,|_stkseg+8|(r15)
	sla	r6,#4
	ld	r7,<<2>>%E4CE
	ldl	rr4,#%01000A20
	calr	L1664
	ld	<<2>>%E4CE,r2
	ldk	r2,#1
	jr	L18e4
L18a4:
	ld	r2,<<2>>%04FA
	cp	r2,|_stkseg+14|(r15)
	jr	le,L18d6
	dec	<<2>>%04FA,#1
	ld	r2,<<2>>%04FA
	sla	r2,#2
	ldl	rr4,<<2>>%0500(r2)
	ldl	|_stkseg+24|(r15),rr4
	testl	rr4
	jr	eq,L18a4
	ldl	rr2,rr4
	subl	rr4,rr4
	ldl	@rr2,rr4
	jr	L18a4
L18d6:
	ld	r2,<<1>>%3C02
	ld	<<2>>%F32C,r2
L18e2:
	sub	r2,r2
L18e4:
	ldm	r10,_stkseg+36(r15),#4
	add	r15,#%002C
	ret
L18f2:
	sub	r15,#%0024
	ldm	_stkseg+24(r15),r8,#6
	ld	|_stkseg+4|(r15),r5
	ldl	rr12,rr6
	subl	rr2,rr2
	ldl	<<2>>%F328,rr2
	subl	rr2,rr2
	ldl	<<2>>%F324,rr2
	ld	r4,rr12(#%0004)
	ld	|_stkseg+18|(r15),r4
	ld	r4,rr12(#%0006)
	ld	|_stkseg+20|(r15),r4
	ld	r4,rr12(#%0008)
	ld	|_stkseg+22|(r15),r4
	test	<<2>>%E49A
	jr	eq,L1938
	ldk	r4,#4
	jr	L193a
L1938:
	sub	r4,r4
L193a:
	ldb	<<1>>%4292,rl4
	test	<<2>>%E49A
	jr	eq,L194c
	sub	r2,r2
	jr	L194e
L194c:
	ldk	r2,#1
L194e:
	ldb	<<1>>%429A,rl2
	test	|_stkseg+20|(r15)
	jr	eq,L195e
	ldk	r2,#2
	jr	L1960
L195e:
	ldk	r2,#1
L1960:
	ldb	<<1>>%42A2,rl2
	ld	r2,|_stkseg+18|(r15)
	ld	<<1>>%4296,r2
	ld	r3,|_stkseg+20|(r15)
	ld	<<1>>%429E,r3
	ld	r2,|_stkseg+22|(r15)
	ld	<<1>>%42A6,r2
	ldk	r10,#0
L1986:
	ld	r2,r10
	sla	r2,#3
	ldl	rr4,#%01004290
	add	r5,r2
	clr	@rr4
	ld	r2,r10
	sla	r2,#3
	ldl	rr4,#%0100428C
	add	r5,r2
	clr	@rr4
	inc	r10,#1
	cp	r10,#%0004
	jr	lt,L1986
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldb	rl2,<<1>>%42B4(r3)
	testb	rl2
	jr	eq,L1aa4
	test	<<2>>%E49A
	jr	eq,L19cc
	ldk	r3,#2
	jr	L19ce
L19cc:
	ldk	r3,#1
L19ce:
	ld	r10,r3
	jr	L1a50
L19d2:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(r10)
	testb	rl2
	jr	eq,L1a4e
	ld	r2,r10
	sla	r2,#2
	ldl	rr4,<<1>>%04AC(r2)
	ldb	rl2,@rr4
	extsb	r2
	ld	|_stkseg+0|(r15),r2
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldb	rl2,<<1>>%42B4(r3)
	ldb	rl3,rl2
	extsb	r3
	ld	r7,|_stkseg+4|(r15)
	mult	rr6,#%0005
	ldl	rr0,#%010042B4
	add	r1,r7
	add	r1,r10
	ldl	rr6,rr0
	ldb	rl2,@rr6
	extsb	r2
	ldl	rr6,#%01000A36
	ldl	rr4,<<2>>%E4D6
	call	_printf
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	add	r5,r10
	clrb	@rr4
L1a4e:
	inc	r10,#1
L1a50:
	cp	r10,#%0004
	jr	lt,L19d2
	ldb	<<1>>%428A,#%04
	clrb	<<1>>%4292
	ldb	<<1>>%429A,#%01
	ld	r2,|_stkseg+18|(r15)
	add	r2,|_stkseg+20|(r15)
	add	r2,|_stkseg+22|(r15)
	ld	<<1>>%428E,r2
	ld	<<1>>%4A34,#%0001
	test	|_stkseg+22|(r15)
	jr	eq,L1a96
	ld	<<1>>%4A34,#%0003
	jr	L1aa4
L1a96:
	test	|_stkseg+20|(r15)
	jr	eq,L1aa4
	ld	<<1>>%4A34,#%0002
L1aa4:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldb	rl2,<<1>>%42B4(r3)
	testb	rl2
	jr	eq,L1aba
	sub	r3,r3
	jr	L1abc
L1aba:
	ldk	r3,#1
L1abc:
	ld	r11,r3
	jp	L1d4c
L1ac4:
	ld	r2,r11
	sla	r2,#3
	ldl	rr4,#%0100428A
	add	r5,r2
	ldl	|_stkseg+12|(r15),rr4
	ldl	rr2,rr4
	ldb	rl4,@rr2
	extsb	r4
	sla	r4,#3
	ld	r5,<<1>>%4290(r4)
	ld	r8,r5
	ld	r7,|_stkseg+4|(r15)
	mult	rr6,#%0005
	ldl	rr0,#%010042B4
	add	r1,r7
	add	r1,r11
	ldl	rr6,rr0
	ldb	rl0,@rr6
	extsb	r0
	ld	r4,r0
	cp	r0,#%0062
	jp	eq,L1cb8
	cp	r0,#%0074
	jr	eq,L1be4
L1b12:
	clr	<<1>>%428E
	ldl	rr2,|_stkseg+12|(r15)
	inc	r3,#4
	ld	r4,r8
	add	r4,@rr2
	ldl	rr2,|_stkseg+12|(r15)
	inc	r3,#6
	ld	@rr2,r4
	ldl	rr2,|_stkseg+12|(r15)
	inc	r3,#6
	cp	r8,@rr2
	jr	ule,L1b50
	ld	r2,r11
	sla	r2,#2
	ldl	rr4,<<1>>%04AC(r2)
	ldl	rr2,rr4
	ldl	rr4,#%01000B04
	ldk	r7,#1
	call	L3408
L1b50:
	ldk	r10,#1
	jp	L1d3a
L1b58:
	ld	r2,r10
	sla	r2,#3
	ldl	rr4,#%01004290
	add	r5,r2
	cp	r8,@rr4
	jp	nc,L1d38
	ld	r2,r10
	sla	r2,#3
	ldl	rr4,#%0100428C
	add	r5,r2
	ldl	rr2,|_stkseg+12|(r15)
	ld	r6,rr2(#%0006)
	cp	r6,@rr4
	jp	ule,L1d38
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+0|(r15),r2
	ld	r3,r10
	sla	r3,#2
	ldl	rr4,<<1>>%04AC(r3)
	ld	r3,r11
	sla	r3,#2
	ldl	rr6,<<1>>%04AC(r3)
	ldl	rr2,rr6
	ldl	rr6,#%01000B18
	call	_printf
	ldl	rr6,rr12
	ld	r5,r11
	calr	L1f96
	cp	r11,#%0002
	jp	ne,L1d26
	clr	<<1>>%42A0
	ldb	<<1>>%42A2,#%01
	inc	<<2>>%F320,#1
	jp	L1d32
L1be4:
	ld	r5,r11
	add	r5,r11
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%000A
	ldl	rr6,#%01004534
	add	r7,r3
	add	r7,r5
	ldl	rr2,|_stkseg+12|(r15)
	ld	r5,rr2(#%0004)
	cp	r5,@rr6
	jr	ule,L1c28
	ld	r5,|_stkseg+4|(r15)
	ld	|_stkseg+0|(r15),r5
	ld	r6,r11
	sla	r6,#2
	ldl	rr2,<<1>>%04AC(r6)
	ldl	rr4,#%01000A58
	ldk	r7,#1
	call	L3408
L1c28:
	ld	r2,r11
	add	r2,r11
	ld	r5,|_stkseg+4|(r15)
	mult	rr4,#%000A
	ldl	rr6,#%01004534
	add	r7,r5
	add	r7,r2
	ld	r2,r11
	add	r2,r11
	ld	r5,|_stkseg+4|(r15)
	mult	rr4,#%000A
	ldl	rr0,#%01004534
	add	r1,r5
	add	r1,r2
	ldl	rr2,rr0
	ld	r4,@rr2
	and	r4,#%FF00
	ld	r8,r4
	cp	r4,@rr6
	jr	eq,L1c8e
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+0|(r15),r2
	ld	|_stkseg+2|(r15),r4
	ld	r3,r11
	sla	r3,#2
	ldl	rr4,<<1>>%04AC(r3)
	ldl	rr2,rr4
	ldl	rr4,<<2>>%E4D6
	ldl	rr6,#%01000A80
	call	_printf
L1c8e:
	ldl	rr2,|_stkseg+12|(r15)
	inc	r3,#4
	ld	r4,r8
	sub	r4,@rr2
	and	r4,#%FF00
	ld	r8,r4
	ldl	rr2,|_stkseg+12|(r15)
	ld	r4,rr2(#%0004)
	add	r4,#%00FF
	and	r4,#%FF00
	inc	r3,#4
	ld	@rr2,r4
	jp	L1b12
L1cb8:
	ld	r2,r11
	add	r2,r11
	ld	r5,|_stkseg+4|(r15)
	mult	rr4,#%000A
	ldl	rr6,#%01004534
	add	r7,r5
	ld	r3,rr6(r2)
	and	r3,#%FF00
	ld	r8,r3
	ld	r2,r11
	add	r2,r11
	ld	r5,|_stkseg+4|(r15)
	mult	rr4,#%000A
	ldl	rr6,#%01004534
	add	r7,r5
	add	r7,r2
	cp	r3,@rr6
	jp	eq,L1b12
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+0|(r15),r2
	ld	|_stkseg+2|(r15),r3
	ld	r3,r11
	sla	r3,#2
	ldl	rr4,<<1>>%04AC(r3)
	ldl	rr2,rr4
	ldl	rr4,<<2>>%E4D6
	ldl	rr6,#%01000AC0
	call	_printf
	jp	L1b12
L1d26:
	clr	<<1>>%42A8
	inc	<<2>>%F322,#1
L1d32:
	inc	<<2>>%E4A0,#1
L1d38:
	inc	r10,#1
L1d3a:
	cp	r10,r11
	jp	lt,L1b58
	ldl	rr2,|_stkseg+12|(r15)
	inc	r3,#2
	ld	@rr2,r8
	inc	r11,#1
L1d4c:
	cp	r11,#%0004
	jp	lt,L1ac4
	ldl	rr2,rr12
	add	r3,#%001A
	ld	r4,<<1>>%4294
	ld	@rr2,r4
	test	<<2>>%F320
	jr	eq,L1d74
	ldl	rr2,<<2>>%F324
	jr	L1d76
L1d74:
	ldl	rr2,rr12
L1d76:
	add	r3,#%001C
	ld	r4,<<1>>%429C
	ld	@rr2,r4
	test	<<2>>%F322
	jr	eq,L1d92
	ldl	rr2,<<2>>%F328
	jr	L1d94
L1d92:
	ldl	rr2,rr12
L1d94:
	add	r3,#%001E
	ld	r4,<<1>>%42A4
	ld	@rr2,r4
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldb	rl2,<<1>>%42B4(r3)
	testb	rl2
	jr	eq,L1dda
	ldk	r10,#1
	test	|_stkseg+18|(r15)
	jr	ne,L1dc4
	ldk	r10,#2
	test	|_stkseg+20|(r15)
	jr	ne,L1dc4
	ldk	r10,#3
L1dc4:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	add	r5,r10
	ldb	@rr4,#%01
L1dda:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(#%0001)
	testb	rl2
	jr	eq,L1e08
	ldl	rr2,rr12
	inc	r3,#10
	set	@rr2,#4
	ld	r7,rr12(#%001A)
	call	L2e4c
	ldl	rr4,rr12
	inc	r5,#1
	ldb	@rr4,rl2
L1e08:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(#%0002)
	testb	rl2
	jr	eq,L1e5c
	test	<<2>>%F320
	jr	eq,L1e4a
	ldl	rr2,<<2>>%F324
	inc	r3,#10
	set	@rr2,#5
	ldl	rr2,<<2>>%F324
	ld	r7,rr2(#%001C)
	call	L2e4c
	ldl	rr4,<<2>>%F324
	jr	L1e58
L1e4a:
	ldl	rr2,rr12
	inc	r3,#10
	set	@rr2,#5
	ld	r7,rr12(#%001C)
	calr	L2e4c
	ldl	rr4,rr12
L1e58:
	inc	r5,#2
	ldb	@rr4,rl2
L1e5c:
	test	<<2>>%F322
	jr	eq,L1eba
	ldl	rr2,<<2>>%F328
	inc	r3,#10
	set	@rr2,#6
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(#%0003)
	testb	rl2
	jr	ne,L1ea6
	ld	r2,rr12(#%0006)
	ldl	rr4,rr12
	add	r5,#%001C
	add	r2,@rr4
	add	r2,#%00FF
	and	r2,#%FF00
	ldl	rr4,<<2>>%F328
	add	r5,#%001E
	ld	@rr4,r2
L1ea6:
	ldl	rr2,<<2>>%F328
	ld	r7,rr2(#%001E)
	calr	L2e4c
	ldl	rr4,<<2>>%F328
	jr	L1ee0
L1eba:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldl	rr4,#%010042B4
	add	r5,r3
	ldb	rl2,rr4(#%0003)
	testb	rl2
	jr	eq,L1ee4
	ldl	rr2,rr12
	inc	r3,#10
	set	@rr2,#6
	ld	r7,rr12(#%001E)
	calr	L2e4c
	ldl	rr4,rr12
L1ee0:
	inc	r5,#3
	ldb	@rr4,rl2
L1ee4:
	ld	r3,|_stkseg+4|(r15)
	mult	rr2,#%0005
	ldb	rl2,<<1>>%42B4(r3)
	testb	rl2
	jr	ne,L1f88
	test	<<2>>%F320
	jr	ne,L1f06
	test	<<2>>%F322
	jr	eq,L1f0e
L1f06:
	clr	<<1>>%4A34
	jr	L1f88
L1f0e:
	ld	<<1>>%4A34,#%0001
	ld	r2,<<1>>%4298
	cp	r2,<<1>>%42A0
	jr	nc,L1f2e
	ld	<<1>>%4A34,#%0002
	jr	L1f44
L1f2e:
	ld	r2,<<1>>%4298
	cp	r2,<<1>>%42A8
	jr	nc,L1f44
	ld	<<1>>%4A34,#%0003
L1f44:
	ld	r2,<<1>>%4A34
	jr	L1f7c
L1f4c:
	ld	r3,<<1>>%42A0
	cp	r3,<<1>>%42A8
	jr	nc,L1f88
	ld	<<1>>%4A34,#%0003
	jr	L1f88
L1f64:
	ld	r3,<<1>>%42A0
	cp	r3,<<1>>%42A8
	jr	ule,L1f88
	ld	<<1>>%4A34,#%0002
	jr	L1f88
L1f7c:
	cp	r2,#%0002
	jr	eq,L1f4c
	cp	r2,#%0003
	jr	eq,L1f64
L1f88:
	ldm	r8,_stkseg+24(r15),#6
	add	r15,#%0024
	ret
L1f96:
	dec	r15,#6
	ldm	|_stkseg+0|(r15),r5,#3
	cp	<<1>>%3C04,#%0100
	jr	le,L1fb6
	ldk	r7,#2
	ldl	rr4,#%01000B62
	call	L3408
L1fb6:
	cp	|_stkseg+0|(r15),#%0002
	jr	ne,L2008
	ld	r7,<<1>>%3C04
	inc	<<1>>%3C04,#1
	call	L719e
	ldl	<<2>>%F324,rr2
	ldb	@rr2,#%FF
	ldl	rr4,|_stkseg+2|(r15)
	ld	r6,rr4(#%0006)
	ldl	rr0,rr2
	inc	r1,#6
	ldl	rr4,rr0
	ld	@rr4,r6
	ldl	rr4,rr2
	inc	r5,#10
	set	@rr4,#1
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#6
	clr	@rr4
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#10
	res	@rr4,#1
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#2
	jr	L2050
L2008:
	ld	r7,<<1>>%3C04
	inc	<<1>>%3C04,#1
	call	L719e
	ldl	<<2>>%F328,rr2
	ldb	@rr2,#%FF
	ldl	rr4,|_stkseg+2|(r15)
	ld	r6,rr4(#%0008)
	ldl	rr0,rr2
	inc	r1,#8
	ldl	rr4,rr0
	ld	@rr4,r6
	ldl	rr4,rr2
	inc	r5,#10
	set	@rr4,#2
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#8
	clr	@rr4
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#10
	res	@rr4,#2
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#3
L2050:
	clrb	@rr4
	inc	r15,#6
	ret
L2056:
	sub	r15,#%2026
	ldm	_stkseg+8222(r15),r10,#4
	ldl	rr6,#%0200E4DA
	sub	r5,r5
	calr	L22a4
	ldl	rr6,#%0100049E
	call	_mktemp
	ldl	rr6,#%0200E6E2
	ldk	r5,#1
	calr	L22a4
	ldl	rr6,#%0200E8EA
	ldk	r5,#1
	calr	L22a4
	test	<<2>>%E4A4
	jr	eq,L209c
	test	<<2>>%E4AA
	jr	ne,L20a6
L209c:
	ldl	rr6,#%0200F10A
	ldk	r5,#1
	calr	L22a4
L20a6:
	test	<<2>>%E4A2
	jr	eq,L20cc
	ldl	rr6,#%0200EF02
	ldk	r5,#1
	calr	L22a4
	ldl	rr6,#%0200EAF2
	ldk	r5,#1
	calr	L22a4
	ldl	rr6,#%0200ECFA
	ldk	r5,#1
	calr	L22a4
L20cc:
	cp	<<2>>%E4B4,#%0001
	jr	ne,L20f8
	test	<<2>>%E4A0
	jr	eq,L20e4
	ld	r2,#%E605
	jr	L2118
L20e4:
	test	<<2>>%E49A
	jr	eq,L20f2
	ld	r2,#%E611
	jr	L2118
L20f2:
	ld	r2,#%E607
	jr	L2118
L20f8:
	test	<<2>>%E4A0
	jr	eq,L2106
	ld	r2,#%E705
	jr	L2118
L2106:
	test	<<2>>%E49A
	jr	eq,L2114
	ld	r2,#%E711
	jr	L2118
L2114:
	ld	r2,#%E707
L2118:
	ld	<<2>>%0452,r2
	ldl	rr2,<<1>>%3C16
	ldl	<<2>>%0454,rr2
	ldl	rr4,<<1>>%3C1A
	ldl	<<2>>%0458,rr4
	ldl	rr2,<<1>>%3C1E
	ldl	<<2>>%045C,rr2
	test	<<2>>%E4A4
	jr	eq,L214e
	sub	r4,r4
	jr	L2166
L214e:
	ld	r4,<<2>>%E4CE
	srl	r4,#4
	add	r4,<<2>>%04FA
	ld	r3,r4
	mult	rr2,#%000C
	ld	r4,r3
L2166:
	ld	<<2>>%0460,r4
	ldl	rr2,<<2>>%E492
	testl	rr2
	jr	eq,L21cc
	ldb	rl4,rr2(#%0004)
	cpb	rl4,#%22
	jr	eq,L21c2
	ldk	r7,#1
	ldl	rr4,#%01000B80
	call	L3408
	jr	L21aa
L2190:
	ld	r2,rr10(#%001A)
	ld	r5,r2
	ldk	r4,#0
	ldb	rl3,@rr10
	extsb	r3
	exts	rr2
	slal	rr2,#24
	addl	rr4,rr2
L21a4:
	ldl	<<2>>%0462,rr4
L21aa:
	clrb	<<2>>%0466
	clrb	<<2>>%0467
	test	<<2>>%E4A2
	jr	eq,L21e6
	sub	r2,r2
	jr	L21e8
L21c2:
	ldl	rr2,<<2>>%E492
	ldl	rr4,@rr2
	jr	L21a4
L21cc:
	ldl	rr10,<<2>>%F32E
L21d2:
	testl	rr10
	jr	eq,L21aa
	ld	r2,rr10(#%0004)
	test	r2
	jr	ne,L2190
	ldl	rr2,rr10(#%0012)
	ldl	rr10,rr2
	jr	L21d2
L21e6:
	ldk	r2,#1
L21e8:
	ld	<<2>>%0468,r2
	ldk	r13,#0
	ldl	rr10,<<2>>%F32E
	jr	L223a
L21f8:
	ldl	rr2,#%7F000006
	add	r3,r15
	ld	r4,r13
	sla	r4,#4
	add	r3,r4
	ldl	rr6,rr10
	ldk	r4,#8
	ldir	@rr2,@rr6,r4
	sub	r7,#%0010
	test	<<2>>%E4B6
	jr	eq,L2232
	ldl	rr6,#%7F000006
	add	r7,r15
	ld	r2,r13
	sla	r2,#4
	add	r7,r2
	call	L6620
L2232:
	inc	r13,#1
	ldl	rr2,rr10(#%0012)
	ldl	rr10,rr2
L223a:
	testl	rr10
	jr	ne,L21f8
	ldl	rr6,#%7F002006
	add	r7,r15
	ld	r5,r13
	calr	L2346
	test	<<2>>%E4B6
	jr	eq,L226c
	ldl	rr6,#%7F002006
	add	r7,r15
	ldl	rr4,#%7F002006
	add	r5,r15
	ld	r3,#%0018
	call	_swab
L226c:
	ldl	rr4,#%7F002006
	add	r5,r15
	ldl	rr6,#%0200E4DA
	ld	r3,#%0018
	calr	L2f04
	ldl	rr4,#%7F000006
	add	r5,r15
	ld	r3,r13
	sla	r3,#4
	ldl	rr6,#%0200E4DA
	calr	L2f04
	ldm	r10,_stkseg+8222(r15),#4
	add	r15,#%2026
	ret
L22a4:
	dec	r15,#12
	ld	_stkseg+10(r15),r13
	ldm	|_stkseg+0|(r15),r5,#3
	test	r5
	jr	eq,L22be
	ldl	rr2,#%0100049E
	jr	L22c4
L22be:
	ldl	rr2,<<1>>%0494
L22c4:
	ldl	|_stkseg+6|(r15),rr2
	ldl	rr6,rr2
	ld	r5,#%01B6
	call	_creat
	ld	r13,r2
	cp	r2,#%0000
	jr	ge,L22f8
	test	|_stkseg+0|(r15)
	jr	eq,L22ea
	ldl	rr4,#%01000B98
	jr	L22f0
L22ea:
	ldl	rr4,#%01000BAC
L22f0:
	ldk	r7,#2
	call	L3408
L22f8:
	ld	r7,r13
	call	_close
	ldl	rr6,|_stkseg+6|(r15)
	ldk	r5,#2
	call	_open
	ldl	rr4,|_stkseg+2|(r15)
	ld	@rr4,r2
	test	|_stkseg+0|(r15)
	jr	eq,L2324
	ldl	rr6,#%0100049E
	call	_unlink
L2324:
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#2
	ld	@rr2,#%0100
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#8
	ldl	rr4,|_stkseg+2|(r15)
	inc	r5,#4
	ldl	@rr4,rr2
	ld	r13,_stkseg+10(r15)
	inc	r15,#12
	ret
L2346:
	dec	r15,#6
	ldm	|_stkseg+0|(r15),r5,#3
	ldl	rr2,|_stkseg+2|(r15)
	ld	r4,<<2>>%0452
	ld	@rr2,r4
	ldl	rr0,<<1>>%3C16
	addl	rr0,<<1>>%3C1A
	ldl	rr2,rr6
	inc	r3,#2
	ldl	@rr2,rr0
	ldl	rr2,rr6
	ldl	rr0,rr2(#%0002)
	cpl	rr0,#%00010000
	jr	le,L2390
	test	<<2>>%E4A8
	jr	eq,L2390
	ldk	r7,#2
	ldl	rr4,#%01000BC2
	call	L3408
L2390:
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#6
	ldl	rr4,<<1>>%3C1E
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#10
	ld	r4,|_stkseg+0|(r15)
	sla	r4,#4
	ld	@rr2,r4
	ldl	rr2,|_stkseg+2|(r15)
	add	r3,#%0012
	ld	r4,<<2>>%0468
	ld	@rr2,r4
	test	<<2>>%E4AE
	jr	eq,L23d0
	ldl	rr2,|_stkseg+2|(r15)
	add	r3,#%0012
	set	@rr2,#5
L23d0:
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#12
	clr	@rr2
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#14
	ldl	rr4,<<2>>%0462
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+2|(r15)
	add	r3,#%0016
	clr	@rr2
	ldl	rr2,|_stkseg+2|(r15)
	add	r3,#%0014
	clr	@rr2
	inc	r15,#6
	ret
L23fe:
	dec	r15,#16
	ldm	_stkseg+8(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	calr	L3144
	test	r2
	jr	eq,L241e
	ldl	rr10,<<1>>%0486
	jr	L2468
L241c:
	inc	r13,#1
L241e:
	testb	@rr12
	jr	ne,L241c
L2422:
	cpl	rr12,|_stkseg+0|(r15)
	jr	c,L2432
	dec	r13,#1
	ldb	rl2,@rr12
	cpb	rl2,#%2F
	jr	ne,L2422
L2432:
	inc	r13,#1
	ldl	rr4,rr12
	subl	rr6,rr6
	calr	L2492
	jr	L247a
L243c:
	ldl	rr6,#%02000414
	ldl	rr4,@rr10
	ld	r3,#%001A
	calr	L2f3a
	ldl	rr6,#%02000438
	ld	r5,#%001A
	calr	L2e76
	ldl	rr6,@rr10
	addl	rr6,#%0000000D
	ldl	rr4,#%02000438
	calr	L2492
	inc	r11,#4
L2468:
	ldl	rr2,@rr10
	cpl	rr2,#%FFFFFFFF
	jr	ne,L243c
	inc	r11,#4
	ldl	<<1>>%0486,rr10
L247a:
	ld	r7,<<2>>%E4BE
	call	_close
	ldm	r10,_stkseg+8(r15),#4
	inc	r15,#16
	ret
L2492:
	sub	r15,#%0024
	ldm	_stkseg+24(r15),r8,#6
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,rr6
	ldk	r5,#2
	call	L3516
	ldl	rr6,|_stkseg+0|(r15)
	call	L6a08
	ldl	rr8,#%020084D8
	ld	r2,<<2>>%0436
	add	r2,<<2>>%0434
	res	r2,#0
	rr	r2,#1
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,|_stkseg+4|(r15)
	ldl	|_stkseg+4|(r15),rr4
	addl	rr4,<<2>>%0454
	addl	rr4,<<2>>%0458
	ldl	rr6,#%02000414
	ld	r3,<<2>>%0460
	calr	L2f3a
	ldk	r13,#0
	jp	L2600
L24fc:
	call	L6684
	inc	r2,#1
	ld	|_stkseg+8|(r15),r2
	call	L6c6a
	ldb	rl2,<<2>>%046E
	ldb	|_stkseg+11|(r15),rl2
	test	<<2>>%E4A6
	jr	eq,L2538
	ldb	rl3,rl2
	extsb	r3
	and	r3,#%001F
	ldb	|_stkseg+13|(r15),rl3
	cpb	rl3,#%01
	jr	eq,L25fc
	cpb	rl3,#%04
	jr	gt,L25fc
L2538:
	ldb	rl2,|_stkseg+11|(r15)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	ne,L257c
	test	<<2>>%E4A4
	jr	ne,L25fc
	test	<<2>>%E4AA
	jr	ne,L25fc
	test	<<2>>%E4AC
	jr	eq,L2568
	cpb	<<2>>%0472,#%4C
	jr	eq,L25fc
L2568:
	ldl	rr6,#%0200F10A
	ldl	rr4,#%0200046A
	call	L6e1a
	jr	L25fc
L257c:
	call	L6a58
	ldl	rr10,@rr2
	testl	rr10
	jr	ne,L2592
	ldk	r7,#2
	ldl	rr4,#%01000BF2
	calr	L3408
L2592:
	cpb	<<2>>%046E,#%20
	jr	ne,L25ba
	cpl	rr8,#%0200E47E
	jr	c,L25ae
	ldk	r7,#2
	ldl	rr4,#%01000C14
	calr	L3408
L25ae:
	ld	@rr8,r13
	ldl	rr2,rr8
	inc	r3,#2
	ldl	@rr2,rr10
	inc	r9,#6
	jr	L25fc
L25ba:
	ldb	rl2,rr10(#%0004)
	cpb	rl2,<<2>>%046E
	jr	ne,L25d0
	ldl	rr2,<<2>>%046A
	cpl	rr2,@rr10
	jr	eq,L25fc
L25d0:
	ldl	rr6,#%02000472
	call	L6522
	cp	r2,#%0008
	jr	ge,L25ea
	ldk	r7,#9
	call	_putchar
L25ea:
	ldk	r7,#9
	call	_putchar
	ldk	r7,#1
	ldl	rr4,#%01000C2A
	calr	L3408
L25fc:
	add	r13,|_stkseg+8|(r15)
L2600:
	cp	<<2>>%041E,#%0000
	jp	gt,L24fc
	ldl	rr6,|_stkseg+4|(r15)
	ldl	rr4,rr8
	calr	L2624
	ldm	r8,_stkseg+24(r15),#6
	add	r15,#%0024
	ret
L2624:
	sub	r15,#%002A
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	|_stkseg+34|(r15),rr6
	ld	r3,<<2>>%0434
	exts	rr2
	div	rr2,#%0002
	ldk	r0,#0
	ld	r1,r3
	subl	rr6,rr0
	ldl	|_stkseg+30|(r15),rr6
	ld	|_stkseg+28|(r15),#%0008
	ld	r2,|_stkseg+28|(r15)
	add	r2,r2
	ld	r7,<<2>>%0434
	exts	rr6
	div	rr6,r2
	ld	|_stkseg+26|(r15),r7
	ldl	rr2,<<2>>%0454
	addl	rr2,<<2>>%0458
	sral	rr2,#1
	ldl	|_stkseg+38|(r15),rr2
	clr	|_stkseg+24|(r15)
L267a:
	ld	r2,|_stkseg+24|(r15)
	cp	r2,|_stkseg+26|(r15)
	jp	nc,L27d2
	ld	r5,r2
	mult	rr4,|_stkseg+28|(r15)
	ldk	r6,#0
	ld	r7,r5
	addl	rr6,|_stkseg+30|(r15)
	ldl	rr4,rr6
	ld	r3,|_stkseg+28|(r15)
	add	r3,r3
	ldl	rr6,#%02000414
	calr	L2f3a
	ldl	rr6,#%7F000008
	add	r7,r15
	ld	r5,|_stkseg+28|(r15)
	add	r5,r5
	calr	L2e76
	test	<<2>>%E4B6
	jr	eq,L26cc
	ldl	rr6,#%7F000008
	add	r7,r15
	call	L6620
L26cc:
	ld	r3,|_stkseg+18|(r15)
	and	r3,#%0003
	ld	r2,r3
	ldl	rr4,#%01000C3E
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jr	ne,L273c
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%0C4C(r6)
	jp	@rr4
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+34|(r15)
	ld	r3,|_stkseg+12|(r15)
	calr	L2f3a
	ldl	rr4,|_stkseg+34|(r15)
	addl	rr4,|_stkseg+38|(r15)
	ldl	rr6,#%02000424
	ld	r3,|_stkseg+12|(r15)
	calr	L2f3a
	ld	r7,|_stkseg+12|(r15)
	calr	L3090
	ld	r5,r2
	exts	rr4
	addl	rr4,|_stkseg+34|(r15)
	ldl	|_stkseg+34|(r15),rr4
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr4,#%0200E6E2
	ldl	rr2,#%0200EF02
L273a:
	calr	L27d8
L273c:
	inc	|_stkseg+24|(r15),#1
	jr	L267a
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+34|(r15)
	ld	r3,|_stkseg+12|(r15)
	calr	L2f3a
	ldl	rr4,|_stkseg+34|(r15)
	addl	rr4,|_stkseg+38|(r15)
	ldl	rr6,#%02000424
	ld	r3,|_stkseg+12|(r15)
	calr	L2f3a
	ld	r7,|_stkseg+12|(r15)
	calr	L3090
	ld	r5,r2
	exts	rr4
	addl	rr4,|_stkseg+34|(r15)
	ldl	|_stkseg+34|(r15),rr4
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr4,#%0200E6E2
	ldl	rr2,#%0200EF02
	calr	L27d8
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+34|(r15)
	ld	r3,|_stkseg+14|(r15)
	calr	L2f3a
	ldl	rr4,|_stkseg+34|(r15)
	addl	rr4,|_stkseg+38|(r15)
	ldl	rr6,#%02000424
	ld	r3,|_stkseg+14|(r15)
	calr	L2f3a
	ld	r7,|_stkseg+14|(r15)
	calr	L3090
	ld	r5,r2
	exts	rr4
	addl	rr4,|_stkseg+34|(r15)
	ldl	|_stkseg+34|(r15),rr4
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr4,#%0200E8EA
	ldl	rr2,#%0200EAF2
	jr	L273a
L27d2:
	add	r15,#%002A
	ret
L27d8:
	sub	r15,#%0024
	ldm	_stkseg+24(r15),r8,#6
	ldm	|_stkseg+0|(r15),r2,#6
	clr	|_stkseg+22|(r15)
L27ee:
	dec	<<2>>%041E,#1
	cp	<<2>>%041E,#%0000
	jr	gt,L2826
	cp	<<2>>%041E,#%0000
	jr	ge,L2814
	jp	L2cae
L280e:
	inc	<<2>>%0418,#1
L2814:
	inc	<<2>>%041E,#1
	ldl	rr6,#%02000414
	calr	L30a4
	ld	r12,r2
	jr	L2844
L2826:
	dec	<<2>>%0418,#1
	cp	<<2>>%0418,#%0000
	jr	lt,L280e
	ldl	rr2,<<2>>%041A
	ld	r12,@rr2
	inc	<<2>>%041C,#2
L2844:
	dec	<<2>>%042E,#1
	cp	<<2>>%042E,#%0000
	jr	gt,L2882
	cp	<<2>>%042E,#%0000
	jr	ge,L2870
	ldk	r7,#2
	ldl	rr4,#%01000C50
	calr	L3408
	jr	L2870
L286a:
	inc	<<2>>%0428,#1
L2870:
	inc	<<2>>%042E,#1
	ldl	rr6,#%02000424
	calr	L30a4
	ld	r13,r2
	jr	L28a0
L2882:
	dec	<<2>>%0428,#1
	cp	<<2>>%0428,#%0000
	jr	lt,L286a
	ldl	rr2,<<2>>%042A
	ld	r13,@rr2
	inc	<<2>>%042C,#2
L28a0:
	test	<<2>>%E4B6
	jr	eq,L28bc
	ld	r7,r12
	call	L65c6
	ld	r12,r2
	ld	r7,r13
	call	L65c6
	ld	r13,r2
L28bc:
	bit	r13,#3
	jp	ne,L2b02
	bit	r13,#3
	jr	eq,L28cc
	ld	r3,r13
	jr	L28d2
L28cc:
	ld	r3,r13
	sra	r3,#4
L28d2:
	and	r3,#%0007
	ld	r2,r3
	ldl	rr4,#%01000CF6
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jp	ne,L2af2
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%0DA4(r6)
	jp	@rr4
	bit	r13,#0
	jr	ne,L2908
	ldk	r7,#2
	ldl	rr4,#%01000C62
	calr	L3408
L2908:
	ld	r2,r13
	and	r2,#%0007
	sra	r2,#1
	inc	r2,#1
	ldb	<<2>>%046E,rl2
	ld	r2,r12
	and	r2,#%00FF
	ld	r5,r2
	exts	rr4
	ldl	<<2>>%046A,rr4
	ld	r2,r13
	ldb	rl2,rh2
	clrb	rh2
	ld	<<2>>%0470,r2
	call	L6c6a
	ldl	rr2,<<2>>%046A
	ld	r12,r3
	and	r3,#%FF00
	jr	eq,L2954
	ldk	r7,#2
	ldl	rr4,#%01000C88
	calr	L3408
L2954:
	ldl	rr2,<<2>>%046A
	sral	rr2,#16
	and	r3,#%FF00
	add	r12,r3
L2964:
	ld	r2,<<2>>%0470
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,r13
	and	r3,#%00FF
	add	r2,r3
	ld	r13,r2
L2978:
	test	<<2>>%E4B6
	jr	eq,L2994
	ld	r7,r12
	call	L65c6
	ld	r12,r2
	ld	r7,r13
	call	L65c6
	ld	r13,r2
L2994:
	ld	r7,r12
	ldl	rr4,|_stkseg+4|(r15)
	call	L3a88
	test	<<2>>%E4A2
	jp	eq,L27ee
	ld	r7,r13
	ldl	rr4,|_stkseg+0|(r15)
	call	L3a88
	jp	L27ee
	bit	r13,#0
	jr	ne,L29cc
	ldk	r7,#2
	ldl	rr4,#%01000CAA
	calr	L3408
L29cc:
	ld	r10,r12
	ld	r2,r13
	ldb	rl2,rh2
	clrb	rh2
	ld	<<2>>%0470,r2
	ld	r2,r13
	and	r2,#%0007
	sra	r2,#1
	inc	r2,#1
	ldb	<<2>>%046E,rl2
	ld	r3,r12
	exts	rr2
	ldl	<<2>>%046A,rr2
	call	L6c6a
	ldl	rr2,<<2>>%046A
	sral	rr2,#16
	ld	r2,r10
	and	r2,#%8000
	or	r2,r3
	ld	r10,r2
	test	<<2>>%E4B6
	jr	eq,L2a22
	ld	r7,r2
	call	L65c6
	ld	r10,r2
L2a22:
	ld	r7,r10
	ldl	rr4,|_stkseg+4|(r15)
	call	L3a88
	ld	r11,r13
L2a30:
	ld	|_stkseg+22|(r15),#%0001
	jp	L27ee
	ld	r2,r13
	and	r2,#%000E
	test	r2
	jr	eq,L2a96
	test	|_stkseg+22|(r15)
	jr	eq,L2a68
	cp	r13,#%FF0F
	ldk	r2,#0
	tcc	ne,r2
	and	r2,r11
	and	r2,#%FF0F
	jr	eq,L2a68
	and	r13,#%00F0
	ld	r2,r11
	and	r2,#%FF0F
	or	r13,r2
L2a68:
	ld	r2,r13
	ldb	rl2,rh2
	clrb	rh2
	ld	<<2>>%0470,r2
	ld	r2,r13
	and	r2,#%0007
	sra	r2,#1
	inc	r2,#1
	ldb	<<2>>%046E,rl2
	ld	r3,r12
	exts	rr2
	ldl	<<2>>%046A,rr2
	call	L6c6a
L2a96:
	test	|_stkseg+22|(r15)
	jr	eq,L2ad6
	clr	|_stkseg+22|(r15)
	ld	r2,<<2>>%0470
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,r11
	and	r3,#%00FF
	add	r2,r3
	ld	r11,r2
	test	<<2>>%E4B6
	jr	eq,L2ac6
	ld	r7,r2
	call	L65c6
	ld	r11,r2
L2ac6:
	test	<<2>>%E4A2
	jr	eq,L2ad6
	ld	r7,r11
	ldl	rr4,|_stkseg+0|(r15)
	calr	L3a88
L2ad6:
	ld	r2,r13
	and	r2,#%000E
	test	r2
	jp	eq,L2978
	ldl	rr2,<<2>>%046A
	ld	r12,r3
	jp	L2964
L2af2:
	ldk	r7,#2
	ldl	rr4,#%01000CD0
L2afa:
	calr	L3408
	jp	L2978
L2b02:
	ldl	rr6,|_stkseg+8|(r15)
	ld	r5,r13
	calr	L34b8
	ldl	rr8,rr2
	ldb	rl2,rr8(#%0004)
	cpb	rl2,#%20
	jr	ne,L2b46
	ldl	rr6,rr8
	call	L70ce
	add	r2,<<2>>%F312
	sla	r2,#4
	bit	r13,#3
	jr	eq,L2b30
	ld	r3,r13
	jr	L2b36
L2b30:
	ld	r3,r13
	sra	r3,#4
L2b36:
	and	r3,#%0007
	add	r3,r2
	inc	r3,#8
	ld	r13,r3
	jp	L2978
L2b46:
	bit	r13,#3
	jr	eq,L2b4e
	ld	r3,r13
	jr	L2b54
L2b4e:
	ld	r3,r13
	sra	r3,#4
L2b54:
	and	r3,#%0007
	ld	r2,r3
	ldl	rr4,#%01000D96
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jp	ne,L2ca0
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%0DB0(r6)
	jp	@rr4
	ldb	rl3,rr8(#%0004)
	extsb	r3
	sub	r3,#%0021
	add	r3,r3
	add	r3,#%0021
	ld	r13,r3
	ld	r3,rr8(#%0006)
	ldb	rh3,rl3
	clrb	rl3
	ld	r4,r13
	and	r4,#%00FF
	add	r3,r4
	ld	r13,r3
	ldl	rr4,@rr8
	add	r5,#%00FF
	and	r5,r12
	ld	r12,r5
	and	r5,#%FF00
	jr	eq,L2bba
	ldk	r7,#2
	ldl	rr4,#%01000CFC
	calr	L3408
L2bba:
	ldl	rr2,@rr8
	sral	rr2,#16
	and	r3,#%FF00
	add	r12,r3
	jp	L2978
	bit	r13,#0
	jr	ne,L2bda
	ldk	r7,#2
	ldl	rr4,#%01000D1C
	calr	L3408
L2bda:
	ldl	rr2,@rr8
	sral	rr2,#16
	and	r3,#%FF00
	ld	r2,r12
	and	r2,#%8000
	or	r2,r3
	ld	r10,r2
	test	<<2>>%E4B6
	jr	eq,L2c00
	ld	r7,r2
	call	L65c6
	ld	r10,r2
L2c00:
	ld	r7,r10
	ldl	rr4,|_stkseg+4|(r15)
	calr	L3a88
	ldl	|_stkseg+12|(r15),rr8
	jp	L2a30
	test	|_stkseg+22|(r15)
	jr	eq,L2c72
	clr	|_stkseg+22|(r15)
	cpl	rr8,|_stkseg+12|(r15)
	jr	eq,L2c2c
	ldk	r7,#2
	ldl	rr4,#%01000D42
	calr	L3408
L2c2c:
	ldb	rl2,rr8(#%0004)
	extsb	r2
	sub	r2,#%0021
	add	r2,r2
	add	r2,#%0011
	ld	r11,r2
	ld	r2,rr8(#%0006)
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,r11
	and	r3,#%00FF
	add	r2,r3
	ld	r11,r2
	test	<<2>>%E4B6
	jr	eq,L2c62
	ld	r7,r2
	call	L65c6
	ld	r11,r2
L2c62:
	test	<<2>>%E4A2
	jr	eq,L2c72
	ld	r7,r11
	ldl	rr4,|_stkseg+0|(r15)
	calr	L3a88
L2c72:
	ldb	rl2,rr8(#%0004)
	extsb	r2
	sub	r2,#%0021
	add	r2,r2
	inc	r2,#1
	ld	r13,r2
	ld	r2,rr8(#%0006)
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,r13
	and	r3,#%00FF
	add	r2,r3
	ld	r13,r2
	ldl	rr2,@rr8
	add	r3,r12
	ld	r12,r3
	jp	L2978
L2ca0:
	ldk	r7,#2
	ldl	rr4,#%01000D70
	jp	L2afa
L2cae:
	ldm	r8,_stkseg+24(r15),#6
	add	r15,#%0024
	ret
L2cbc:
	call	L6134
	test	<<2>>%E4A2
	jr	eq,L2cd2
	ldl	rr6,#%0200ECFA
	calr	L2dde
L2cd2:
	test	<<2>>%E4A4
	jr	ne,L2cf0
	test	<<2>>%E4AA
	jr	ne,L2cea
	ldl	rr6,#%0200F10A
	calr	L2dde
L2cea:
	call	L6efe
L2cf0:
	ldl	rr6,#%0200E4DA
	calr	L3ac6
	ld	r7,<<2>>%E4DA
	call	_close
	test	<<2>>%E4BC
	jr	ne,L2d36
	ldl	rr6,<<1>>%048A
	call	_unlink
	ldl	rr6,#%01000DB4
	ldl	rr4,<<1>>%048A
	call	_link
	ldl	rr2,<<1>>%048A
	ldl	<<1>>%0494,rr2
L2d36:
	ld	r2,<<1>>%4A3A
	ld	<<1>>%04E8,r2
	calr	L2d4c
	call	La5c
	ret
L2d4c:
	sub	r15,#%001A
	ldl	rr6,<<1>>%0494
	ldk	r5,#1
	call	_open
	ld	|_stkseg+0|(r15),r2
	cp	r2,#%FFFF
	jr	ne,L2d72
	ldk	r7,#2
	ldl	rr4,#%01000DBA
	calr	L3408
L2d72:
	ldl	rr4,#%7F00000E
	add	r5,r15
	ldl	rr2,#%7F000002
	add	r3,r15
	sub	r5,r3
	ld	r3,r5
	exts	rr2
	ldl	rr4,rr2
	ld	r7,|_stkseg+0|(r15)
	sub	r3,r3
	call	_lseek
	cp	r2,#%FFFF
	jr	ne,L2da6
	ldk	r7,#2
	ldl	rr4,#%01000DD8
	calr	L3408
L2da6:
	ld	r3,<<1>>%04F0
	mult	rr2,#%000E
	ld	<<1>>%04F0,r3
	ld	r7,|_stkseg+0|(r15)
	ldl	rr4,#%010004F0
	ldk	r3,#2
	call	_write
	cp	r2,#%0002
	jr	eq,L2dd8
	ldk	r7,#2
	ldl	rr4,#%01000DF6
	calr	L3408
L2dd8:
	add	r15,#%001A
	ret
L2dde:
	dec	r15,#12
	ldm	_stkseg+4(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,rr6
	calr	L3ac6
	ldl	rr2,|_stkseg+0|(r15)
	ld	r13,@rr2
	ld	r7,r13
	subl	rr4,rr4
	sub	r3,r3
	call	_lseek
	jr	L2e1e
L2e04:
	sra	r12,#1
	ldl	rr10,#%0200E6EA
L2e0e:
	ldl	rr2,rr10
	inc	r11,#2
	ld	r7,@rr2
	ldl	rr4,#%0200E4DA
	calr	L3a88
	djnz	r12,L2e0e
L2e1e:
	ld	r7,r13
	ldl	rr4,#%0200E6EA
	ld	r3,#%0200
	call	_read
	ld	r12,r2
	cp	r2,#%0001
	jr	gt,L2e04
	ld	r7,r13
	call	_close
	ldm	r10,_stkseg+4(r15),#4
	inc	r15,#12
	ret
L2e4c:
	dec	r15,#4
	ld	|_stkseg+0|(r15),r7
	and	r7,#%FF00
	cp	r7,|_stkseg+0|(r15)
	jr	eq,L2e66
	ldk	r7,#2
	ldl	rr4,#%01000E16
	calr	L3408
L2e66:
	ld	r2,|_stkseg+0|(r15)
	ldb	rl2,rh2
	clrb	rh2
	and	r2,#%00FF
	inc	r15,#4
	ret
L2e76:
	dec	r15,#16
	ldm	_stkseg+6(r15),r9,#5
	ldm	|_stkseg+0|(r15),r5,#3
	ld	r9,r5
	sra	r9,#1
	ldl	rr10,rr6
	ld	r4,<<2>>%0418
	sub	r4,r9
	ld	<<2>>%0418,r4
	cp	r4,#%0000
	jr	lt,L2edc
	ld	r2,<<2>>%041E
	sub	r2,r9
	ld	<<2>>%041E,r2
	cp	r2,#%0000
	jr	le,L2ece
	ldl	rr12,<<2>>%041A
L2ebc:
	ld	r2,@rr12
	ld	@rr10,r2
	inc	r13,#2
	inc	r11,#2
	djnz	r9,L2ebc
	ldl	<<2>>%041A,rr12
	jr	L2ef8
L2ece:
	ld	r2,<<2>>%041E
	add	r2,r9
	ld	<<2>>%041E,r2
L2edc:
	ld	r2,<<2>>%0418
	add	r2,r9
	ld	<<2>>%0418,r2
L2eea:
	ldl	rr6,#%02000414
	calr	L30a4
	ld	@rr10,r2
	inc	r11,#2
	djnz	r9,L2eea
L2ef8:
	ldm	r9,_stkseg+6(r15),#5
	inc	r15,#16
	ret
L2f04:
	sub	r15,#%0012
	ldm	_stkseg+10(r15),r10,#4
	ldm	|_stkseg+0|(r15),r3,#5
	ldl	rr10,rr4
	sra	r3,#1
	ld	r13,r3
L2f1e:
	ldl	rr2,rr10
	inc	r11,#2
	ld	r7,@rr2
	ldl	rr4,|_stkseg+6|(r15)
	calr	L3a88
	djnz	r13,L2f1e
	ldm	r10,_stkseg+10(r15),#4
	add	r15,#%0012
	ret
L2f3a:
	sub	r15,#%001C
	ldm	_stkseg+20(r15),r10,#4
	ldm	|_stkseg+0|(r15),r3,#5
	sral	rr4,#8
	and	r4,#%00FF
	ldl	|_stkseg+12|(r15),rr4
	ldl	rr4,|_stkseg+2|(r15)
	ldk	r4,#0
	and	r5,#%00FF
	ldl	|_stkseg+16|(r15),rr4
	ldl	rr10,rr6
	ldl	rr4,rr10(#%000C)
	dec	@rr4,#1
	ldl	rr12,#%02000004
	ldl	rr4,rr12
	ld	r2,rr4(#%0002)
	ld	r1,r2
	exts	rr0
	cpl	rr0,|_stkseg+12|(r15)
	jr	eq,L302e
	ldl	rr12,#%0200020A
	ldl	rr4,rr12
	ld	r2,rr4(#%0002)
	ld	r1,r2
	exts	rr0
	cpl	rr0,|_stkseg+12|(r15)
	jr	eq,L302e
	test	@rr12
	jr	eq,L2faa
	ldl	rr12,#%02000004
	ldl	rr2,rr12
	test	@rr2
	jr	ne,L3024
L2faa:
	test	<<2>>%0004
	jr	ne,L2fce
	test	<<2>>%020A
	jr	ne,L2fce
	ld	r2,<<2>>%0006
	cp	r2,<<2>>%020C
	jr	ge,L2fce
	ldl	rr12,#%02000004
L2fce:
	ldl	rr2,rr12
	inc	r3,#2
	ld	r4,|_stkseg+14|(r15)
	ld	@rr2,r4
	ldl	rr4,|_stkseg+2|(r15)
	and	r5,#%FF00
	slal	rr4,#1
	ld	r7,<<2>>%E4BE
	sub	r3,r3
	call	_lseek
	ldl	rr4,rr12
	inc	r5,#6
	ld	r7,<<2>>%E4BE
	ld	r3,#%0200
	call	_read
	sra	r2,#1
	ld	|_stkseg+10|(r15),r2
	cp	r2,#%0000
	jr	ge,L3018
	clr	|_stkseg+10|(r15)
L3018:
	ldl	rr2,rr12
	inc	r3,#4
	ld	r4,|_stkseg+10|(r15)
	ld	@rr2,r4
	jr	L302e
L3024:
	ldk	r7,#2
	ldl	rr4,#%01000E2E
	calr	L3408
L302e:
	inc	@rr12,#1
	ldl	rr2,rr10
	inc	r3,#2
	ld	r4,|_stkseg+14|(r15)
	ld	@rr2,r4
	ldl	rr2,rr10
	inc	r3,#12
	ldl	@rr2,rr12
	ldl	rr2,rr12
	inc	r3,#6
	ldl	rr4,|_stkseg+16|(r15)
	slal	rr4,#1
	add	r3,r5
	ldl	rr4,rr10
	inc	r5,#6
	ldl	@rr4,rr2
	cp	|_stkseg+0|(r15),#%FFFF
	jr	eq,L3068
	ld	r7,|_stkseg+0|(r15)
	calr	L3090
	ldl	rr4,rr10
	inc	r5,#10
	ld	@rr4,r2
L3068:
	ld	r2,rr12(#%0004)
	sub	r2,|_stkseg+18|(r15)
	ldl	rr4,rr10
	inc	r5,#4
	ld	@rr4,r2
	cp	r2,#%0000
	jr	gt,L3082
	ldl	rr2,rr10
	inc	r3,#10
	clr	@rr2
L3082:
	ldm	r10,_stkseg+20(r15),#4
	add	r15,#%001C
	ret
L3090:
	dec	r15,#4
	ld	|_stkseg+0|(r15),r7
	sra	r7,#1
	and	r7,#%7FFF
	ld	r2,r7
	inc	r15,#4
	ret
L30a4:
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	ldl	rr2,rr6
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L30e0
	ld	r4,rr12(#%0002)
	inc	r4,#1
	ld	r3,r4
	exts	rr2
	slal	rr2,#8
	ldl	rr4,rr2
	ldl	rr6,rr12
	ld	r3,#%FFFF
	calr	L2f3a
	ldl	rr2,rr12
	inc	r3,#4
	dec	@rr2,#1
L30e0:
	ldl	rr2,rr12
	inc	r3,#10
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	gt,L311a
	ld	r2,rr12(#%000A)
	cp	r2,#%0000
	jr	ge,L3102
	ldk	r7,#2
	ldl	rr4,#%010004D8
	calr	L3408
L3102:
	inc	<<2>>%0410,#1
	ldl	rr2,rr12(#%000C)
	dec	@rr2,#1
	ldl	rr2,rr12
	inc	r3,#12
	ldl	rr4,#%02000410
	ldl	@rr2,rr4
L311a:
	ldl	rr2,rr12(#%0006)
	ld	r4,@rr2
	ld	r2,r4
	ld	_stkseg+4(r15),r4
	ldl	rr4,rr12
	inc	r5,#6
	inc	r5,#2
	inc	@rr4,#2
	dec	r5,#2
	ld	r2,_stkseg+4(r15)
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
L3144:
	sub	r15,#%002C
	ldm	_stkseg+36(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	ld	<<2>>%E4BE,#%FFFF
	clrb	<<2>>%0438
	ldl	<<2>>%E4C0,rr6
	cpb	@rr12,#%2D
	jp	ne,L32f4
	ldb	rl2,rr12(#%0001)
	cpb	rl2,#%6C
	jp	ne,L32f4
	cp	<<2>>%E4B4,#%0001
	jr	ne,L31b2
	ldl	rr6,#%01009098
	ldl	rr4,#%01000E48
	call	_strcpy
	ldl	rr6,#%01009098
	ld	r5,#%002F
	call	_rindex
	inc	r3,#5
	jr	L31d6
L31b2:
	ldl	rr6,#%01009098
	ldl	rr4,#%01000E52
	call	_strcpy
	ldl	rr6,#%01009098
	ld	r5,#%002F
	call	_rindex
	inc	r3,#4
L31d6:
	ldl	rr10,rr2
	ldl	rr2,#%010090B6
	ldl	<<2>>%E4C0,rr2
	ldb	rl2,rr12(#%0002)
	testb	rl2
	jr	ne,L31f2
	ldl	rr12,#%01000E5C
L31f2:
	ldl	rr4,rr12
	inc	r5,#2
	ldl	rr6,rr10
	call	_strcpy
	ldl	rr6,rr10
	ldl	rr4,#%01000E60
	call	_strcat
	ldl	rr6,#%010090B6
	ldl	rr4,#%01000E38
	call	_strcpy
	ldl	rr6,#%010090B6
	ldl	rr4,#%01009098
	call	_strcat
	ldl	rr6,#%010090B6
	sub	r5,r5
	call	_open
	ld	<<2>>%E4BE,r2
	cp	r2,#%FFFF
	jr	ne,L32f4
	ldl	rr6,#%010090B6
	ldl	rr4,#%01000E38
	call	_strcpy
	ldl	rr6,#%010090B6
	ldl	rr4,#%01000E3A
	call	_strcat
	ldl	rr6,#%010090B6
	ldl	rr4,#%01009098
	call	_strcat
	ldl	rr6,#%010090B6
	sub	r5,r5
	call	_open
	ld	<<2>>%E4BE,r2
	cp	r2,#%FFFF
	jr	ne,L32f4
	ldl	rr6,#%010090B6
	ldl	rr4,#%01000E38
	call	_strcpy
	ldl	rr6,#%010090B6
	ldl	rr4,#%01000E40
	call	_strcat
	ldl	rr6,#%010090B6
	ldl	rr4,#%01009098
	call	_strcat
	ldl	rr6,#%010090B6
	sub	r5,r5
	call	_open
	ld	<<2>>%E4BE,r2
	cp	r2,#%FFFF
	jr	ne,L32f4
	ldk	r7,#2
	ldl	rr4,#%01000E64
	calr	L3408
L32f4:
	cp	<<2>>%E4BE,#%FFFF
	jr	ne,L3322
	ldl	rr6,<<2>>%E4C0
	sub	r5,r5
	call	_open
	ld	<<2>>%E4BE,r2
	cp	r2,#%0000
	jr	ge,L3322
	ldk	r7,#2
	ldl	rr4,#%01000E78
	calr	L3408
L3322:
	ld	<<2>>%020C,#%FFFF
	ld	<<2>>%0006,#%FFFF
	clr	<<2>>%020A
	clr	<<2>>%0004
	ldl	rr2,#%02000410
	ldl	<<2>>%0430,rr2
	ldl	rr2,#%02000410
	ldl	<<2>>%0420,rr2
	ld	<<2>>%0410,#%0002
	ldl	rr6,#%02000414
	subl	rr4,rr4
	ldk	r3,#2
	calr	L2f3a
	cp	<<2>>%041E,#%0000
	jr	gt,L337e
	ldk	r7,#2
	ldl	rr4,#%010004D8
	calr	L3408
L337e:
	ldl	rr6,#%02000414
	calr	L30a4
	cp	r2,#%FF65
	jr	eq,L3390
	sub	r2,r2
	jr	L33fa
L3390:
	ldl	rr6,#%02000414
	ldl	rr4,#%00000001
	ld	r3,#%001A
	calr	L2f3a
	cp	<<2>>%041E,#%0000
	jr	gt,L33b0
L33ac:
	ldk	r2,#1
	jr	L33fa
L33b0:
	ldl	rr6,#%02000438
	ld	r5,#%001A
	calr	L2e76
	ldl	rr6,#%02000438
	ldl	rr4,#%010004CE
	ldk	r3,#14
	call	_strncmp
	test	r2
	jr	ne,L33ac
	ldl	rr4,#%7F000004
	add	r5,r15
	ld	r7,<<2>>%E4BE
	call	_fstat
	ldl	rr2,|_stkseg+26|(r15)
	cpl	rr2,<<2>>%0446
	jr	le,L33f8
	ldk	r2,#3
	jr	L33fa
L33f8:
	ldk	r2,#2
L33fa:
	ldm	r10,_stkseg+36(r15),#4
	add	r15,#%002C
	ret
L3408:
	dec	r15,#14
	ldm	|_stkseg+2|(r15),r2,#6
	test	<<1>>%4A3A
	jr	ne,L342a
	ldl	rr6,#%01000E84
	ldl	rr4,<<2>>%E4D6
	call	_printf
L342a:
	ldl	rr2,<<2>>%E4C0
	testl	rr2
	jr	eq,L3468
	ldl	rr6,#%01000E8A
	ldl	rr4,rr2
	call	_printf
	testb	<<2>>%0438
	jr	eq,L345c
	ldl	rr6,#%01000E8E
	ldl	rr4,#%02000438
	call	_printf
L345c:
	ldl	rr6,#%01000E96
	call	_printf
L3468:
	ld	r2,_stkseg+22(r15)
	ld	|_stkseg+0|(r15),r2
	ldl	rr6,|_stkseg+6|(r15)
	ld	r5,|_stkseg+4|(r15)
	ld	r4,|_stkseg+2|(r15)
	ld	r3,_stkseg+18(r15)
	ld	r2,_stkseg+20(r15)
	call	_printf
	ldl	rr6,#%01000E9A
	call	_printf
	cp	|_stkseg+12|(r15),#%0001
	jr	le,L34aa
	call	La5c
L34aa:
	ld	r2,|_stkseg+12|(r15)
	ld	<<1>>%4A3A,r2
	inc	r15,#14
	ret
L34b8:
	sub	r15,#%0020
	ldm	_stkseg+20(r15),r8,#6
	ldm	|_stkseg+0|(r15),r5,#3
	ldl	rr8,rr6
	sra	r5,#4
	and	r5,#%0FFF
	ld	r13,r5
	ldl	rr10,#%020084D8
	jr	L34ea
L34de:
	cp	r13,@rr10
	jr	ne,L34e8
	ldl	rr2,rr10(#%0002)
	jr	L3508
L34e8:
	inc	r11,#6
L34ea:
	cpl	rr10,rr8
	jr	c,L34de
	ldl	rr6,#%01000E9C
	ld	r5,|_stkseg+0|(r15)
	call	_printf
	ldk	r7,#2
	ldl	rr4,#%01000EAA
	calr	L3408
L3508:
	ldm	r8,_stkseg+20(r15),#6
	add	r15,#%0020
	ret
L3516:
	sub	r15,#%2030
	ldm	_stkseg+8232(r15),r10,#4
	ldm	|_stkseg+0|(r15),r5,#3
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+2|(r15)
	ldk	r3,#2
	calr	L2f3a
	ldl	rr6,#%7F000006
	add	r7,r15
	ldk	r5,#2
	calr	L2e76
	test	<<2>>%E4B6
	jr	eq,L3558
	ld	r7,|_stkseg+6|(r15)
	call	L65c6
	ld	|_stkseg+6|(r15),r2
L3558:
	cp	|_stkseg+6|(r15),#%E607
	jr	eq,L3568
	cp	|_stkseg+6|(r15),#%E611
	jr	ne,L3570
L3568:
	ld	<<2>>%E4B4,#%0001
L3570:
	cp	|_stkseg+6|(r15),#%E707
	jr	eq,L358c
	cp	|_stkseg+6|(r15),#%E711
	jr	eq,L358c
	test	<<2>>%E4B4
	jp	eq,L3752
L358c:
	inc	<<2>>%E4B2,#1
	ld	<<2>>%0436,#%0018
	ldl	rr6,#%02000414
	ldl	rr4,|_stkseg+2|(r15)
	ld	r3,<<2>>%0436
	calr	L2f3a
	ldl	rr6,#%7F000008
	add	r7,r15
	ld	r5,<<2>>%0436
	calr	L2e76
	test	<<2>>%E4B6
	jr	eq,L35e0
	ldl	rr6,#%7F000008
	add	r7,r15
	ldl	rr4,#%7F000008
	add	r5,r15
	ld	r3,<<2>>%0436
	call	_swab
L35e0:
	ld	r2,|_stkseg+18|(r15)
	ld	<<2>>%0434,r2
	ld	r5,r2
	exts	rr4
	div	rr4,#%0010
	ld	r11,r5
	ld	r3,<<2>>%0436
	res	r3,#0
	rr	r3,#1
	ldk	r4,#0
	ld	r5,r3
	addl	rr4,|_stkseg+2|(r15)
	ldl	rr6,#%02000414
	ld	r3,<<2>>%0434
	calr	L2f3a
	ldl	rr6,#%7F000020
	add	r7,r15
	ld	r5,<<2>>%0434
	calr	L2e76
	test	<<2>>%E4B6
	jr	eq,L3656
	ldl	rr2,#%7F000020
	add	r3,r15
	ldl	rr12,rr2
	jr	L3642
L3638:
	ldl	rr6,rr12
	call	L6620
	inc	r13,#16
L3642:
	ldl	rr2,#%7F000020
	add	r3,r15
	ld	r4,r11
	sla	r4,#4
	add	r3,r4
	cpl	rr12,rr2
	jr	c,L3638
L3656:
	ld	r2,|_stkseg+6|(r15)
	and	r2,#%F7FF
	ld	<<2>>%0452,r2
	subl	rr2,rr2
	ldl	<<2>>%0454,rr2
	subl	rr2,rr2
	ldl	<<2>>%0458,rr2
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<2>>%045C,rr2
	clrb	<<2>>%0466
	clrb	<<2>>%0467
	ldk	r10,#0
	ldl	rr2,#%7F000020
	add	r3,r15
	ldl	rr12,rr2
	jr	L36f8
L3698:
	ld	r2,rr12(#%0004)
	inc	r2,#1
	and	r2,#%FFFE
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
	ld	r2,rr12(#%0006)
	inc	r2,#1
	and	r2,#%FFFE
	ldl	rr4,rr12
	inc	r5,#6
	ld	@rr4,r2
	ld	r2,rr12(#%0008)
	inc	r2,#1
	and	r2,#%FFFE
	ldl	rr4,rr12
	inc	r5,#8
	ld	@rr4,r2
	ld	r2,rr12(#%0004)
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<2>>%0454
	ldl	<<2>>%0454,rr4
	ld	r2,rr12(#%0006)
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<2>>%0458
	ldl	<<2>>%0458,rr4
	ld	r2,rr12(#%000A)
	or	r10,r2
	inc	r13,#16
L36f8:
	ldl	rr2,#%7F000020
	add	r3,r15
	ld	r4,r11
	sla	r4,#4
	add	r3,r4
	cpl	rr12,rr2
	jr	c,L3698
	ld	r2,r10
	and	r2,#%00F8
	jr	eq,L371e
	ldk	r7,#2
	ldl	rr4,#%01000EBE
	calr	L3408
L371e:
	ldl	rr2,|_stkseg+22|(r15)
	ldl	<<2>>%0462,rr2
	ld	r4,|_stkseg+26|(r15)
	and	r4,#%0001
	ld	<<2>>%0468,r4
	ld	r2,|_stkseg+20|(r15)
	ld	<<2>>%0460,r2
	ldl	rr6,#%7F000020
	add	r7,r15
	ld	r5,r11
	ld	r4,|_stkseg+0|(r15)
	calr	L3760
	ldk	r2,#1
L3752:
	ldm	r10,_stkseg+8232(r15),#4
	add	r15,#%2030
	ret
L3760:
	sub	r15,#%002E
	ldm	_stkseg+36(r15),r9,#5
	ld	r10,r4
	ld	r11,r5
	ldl	rr12,rr6
	ld	r2,<<2>>%F32C
	ld	<<1>>%3C02,r2
	ld	r7,<<2>>%F32C
	call	L6c6
	ldl	|_stkseg+32|(r15),rr2
	cp	r10,#%0001
	jp	ne,L3a16
	ldl	rr4,#%7F000010
	add	r5,r15
	ldl	rr6,<<2>>%E4C0
	call	L4444
	ld	r9,<<1>>%0478
	clr	|_stkseg+8|(r15)
	jr	L37e4
L37b8:
	ld	r5,r9
	mult	rr4,#%000C
	ldl	rr6,<<1>>%3C2A(r5)
	ldl	rr4,#%7F000010
	add	r5,r15
	calr	L3a30
	test	r2
	jr	ne,L37f0
	ld	r3,r9
	mult	rr2,#%000C
	ld	r2,<<1>>%3C28(r3)
	ld	r9,r2
	inc	|_stkseg+8|(r15),#1
L37e4:
	ld	r2,|_stkseg+8|(r15)
	cp	r2,<<1>>%047A
	jr	lt,L37b8
L37f0:
	ld	r2,<<1>>%3E7E
	ld	|_stkseg+10|(r15),r2
	ldl	rr4,#%7F00000A
	add	r5,r15
	inc	r5,#2
	ld	r3,<<1>>%3E7C
	ld	@rr4,r3
	ldl	rr4,#%7F00000A
	add	r5,r15
	inc	r5,#4
	ld	r3,<<1>>%3E80
	ld	@rr4,r3
	ld	r3,|_stkseg+8|(r15)
	cp	r3,<<1>>%047A
	jp	eq,L3a16
	ld	r5,r9
	mult	rr4,#%000C
	ld	r4,<<1>>%3C22(r5)
	cp	r4,#%FFFF
	jr	le,L3850
	ld	r7,r9
	mult	rr6,#%000C
	ld	r4,<<1>>%3C22(r7)
	ld	|_stkseg+10|(r15),r4
L3850:
	ld	r3,r9
	mult	rr2,#%000C
	ld	r2,<<1>>%3C24(r3)
	cp	r2,#%FFFF
	jr	le,L387a
	ld	r5,r9
	mult	rr4,#%000C
	ld	r2,<<1>>%3C24(r5)
	ldl	rr4,#%7F00000A
	add	r5,r15
	inc	r5,#2
	ld	@rr4,r2
L387a:
	ld	r3,r9
	mult	rr2,#%000C
	ld	r2,<<1>>%3C26(r3)
	cp	r2,#%FFFF
	jp	le,L3a16
	ld	r5,r9
	mult	rr4,#%000C
	ld	r2,<<1>>%3C26(r5)
	ldl	rr4,#%7F00000A
	add	r5,r15
	inc	r5,#4
	ld	@rr4,r2
	jp	L3a16
L38ae:
	ld	r2,<<2>>%F32C
	cp	r2,<<1>>%04EC
	jr	lt,L38c6
	ldk	r7,#2
	ldl	rr4,#%01000EDA
	calr	L3408
L38c6:
	cp	r10,#%0001
	jr	ne,L39b4
	ldl	rr2,|_stkseg+32|(r15)
	ldl	rr6,rr12
	ldk	r4,#8
	ldir	@rr2,@rr6,r4
	sub	r7,#%0010
	inc	r13,#16
	cp	<<2>>%0452,#%E707
	jr	eq,L38fc
	cp	<<2>>%0452,#%E607
	jr	eq,L38fc
	sub	r7,r7
	ldl	rr4,#%01000F0C
	calr	L3408
L38fc:
	cp	<<2>>%0452,#%E707
	jr	eq,L3910
	cp	<<2>>%0452,#%E607
	jr	ne,L3938
L3910:
	ldl	rr2,|_stkseg+32|(r15)
	ld	r4,rr2(#%0004)
	neg	r4
	add	r3,#%0012
	ld	@rr2,r4
	ldl	rr2,|_stkseg+32|(r15)
	ld	r4,rr2(#%0012)
	inc	r3,#6
	sub	r4,@rr2
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0014
	ld	@rr2,r4
	jr	L394c
L3938:
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0012
	clr	@rr2
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0014
	clr	@rr2
L394c:
	ldl	rr2,|_stkseg+32|(r15)
	inc	r3,#16
	clr	@rr2
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0022
	ld	r4,|_stkseg+10|(r15)
	ld	@rr2,r4
	ldl	rr2,#%7F00000A
	add	r3,r15
	ld	r4,rr2(#%0002)
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0024
	ld	@rr2,r4
	ldl	rr2,#%7F00000A
	add	r3,r15
	ld	r4,rr2(#%0004)
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0026
	ld	@rr2,r4
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0030
	ld	@rr2,#%FFFF
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0032
	ld	@rr2,#%FFFF
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0034
	ld	@rr2,#%FFFF
	jr	L3a00
L39b4:
	ldl	rr2,|_stkseg+32|(r15)
	add	r3,#%0028
	ldl	rr4,<<1>>%047C
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+32|(r15)
	ld	r4,rr2(#%0004)
	ldk	r6,#0
	ld	r7,r4
	addl	rr6,<<1>>%047C
	ldl	<<1>>%047C,rr6
	add	r3,#%002C
	ldl	rr4,<<1>>%0480
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+32|(r15)
	ld	r4,rr2(#%0006)
	ldk	r6,#0
	ld	r7,r4
	addl	rr6,<<1>>%0480
	ldl	<<1>>%0480,rr6
L3a00:
	inc	<<2>>%F32C,#1
	ld	r7,<<2>>%F32C
	call	L6c6
	ldl	|_stkseg+32|(r15),rr2
L3a16:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jp	ne,L38ae
	ldm	r9,_stkseg+36(r15),#5
	add	r15,#%002E
	ret
L3a30:
	dec	r15,#10
	ldm	|_stkseg+0|(r15),r4,#4
L3a38:
	ldl	rr2,|_stkseg+4|(r15)
	ldb	rl4,@rr2
	ldl	rr6,|_stkseg+0|(r15)
	ldb	rl5,@rr6
	cpb	rl4,rl5
	jr	ne,L3a64
	inc	|_stkseg+2|(r15),#1
	ldl	rr4,|_stkseg+4|(r15)
	inc	|_stkseg+6|(r15),#1
	testb	@rr4
	jr	ne,L3a38
	test	<<1>>%0476
	jr	ne,L3a82
	ldk	r2,#1
	jr	L3a84
L3a64:
	test	<<1>>%0476
	jr	eq,L3a82
	ldl	rr2,|_stkseg+4|(r15)
	ldb	rl4,@rr2
	extsb	r4
	ldl	rr6,|_stkseg+0|(r15)
	ldb	rl5,@rr6
	extsb	r5
	sub	r4,r5
	ld	r2,r4
	jr	L3a84
L3a82:
	sub	r2,r2
L3a84:
	inc	r15,#10
	ret
L3a88:
	dec	r15,#12
	ldm	_stkseg+8(r15),r12,#2
	ld	|_stkseg+6|(r15),r7
	ldl	rr12,rr4
	ldl	rr2,rr12(#%0004)
	ld	@rr2,r7
	ldl	rr2,rr12
	inc	r3,#4
	inc	r3,#2
	inc	@rr2,#2
	dec	r3,#2
	ldl	rr2,rr12
	inc	r3,#2
	dec	@rr2,#1
	ld	r6,@rr2
	cp	r6,#%0000
	jr	gt,L3aba
	ldl	rr6,rr12
	calr	L3ac6
L3aba:
	ldm	r12,_stkseg+8(r15),#2
	inc	r15,#12
	ret
L3ac6:
	dec	r15,#12
	ldm	_stkseg+6(r15),r11,#3
	ldl	rr12,rr6
	ldl	rr2,rr12(#%0004)
	ldl	rr4,rr6
	inc	r5,#8
	sub	r3,r5
	ld	r11,r3
	cp	r3,#%0000
	jr	le,L3b00
	ldl	rr4,rr12
	inc	r5,#8
	ld	r7,@rr12
	ld	r3,r11
	call	_write
	cp	r11,r2
	jr	eq,L3b00
	ldk	r7,#2
	ldl	rr4,#%01000F2A
	calr	L3408
L3b00:
	ldl	rr2,rr12
	inc	r3,#8
	ldl	rr4,rr12
	inc	r5,#4
	ldl	@rr4,rr2
	ldl	rr2,rr12
	inc	r3,#2
	ld	@rr2,#%0100
	ldm	r11,_stkseg+6(r15),#3
	inc	r15,#12
	ret
	sub	r15,#%0014
	ldm	_stkseg+12(r15),r10,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr10,rr6
	ldk	r1,#12
L3b32:
	ldb	rl2,@rr10
	extsb	r2
	ld	r5,r2
	ldb	rl2,rr10(#%0001)
	ldb	@rr10,rl2
	ldl	rr2,rr10
	inc	r3,#1
	ld	r4,r5
	ldb	@rr2,rl4
	inc	r11,#2
	djnz	r1,L3b32
	ldm	r10,_stkseg+12(r15),#2
	add	r15,#%0014
	ret
L3b58:
	sub	r15,#%0012
	ldm	_stkseg+14(r15),r12,#2
	ldm	|_stkseg+0|(r15),r4,#4
	test	<<2>>%E4BA
	jr	ne,L3c08
	ldl	rr2,|_stkseg+4|(r15)
	testl	rr2
	jr	eq,L3b98
	ldb	rl4,rr2(#%0004)
	ldb	|_stkseg+9|(r15),rl4
	testb	rl4
	jr	ne,L3b8a
	ldk	r4,#1
	jr	L3b8c
L3b8a:
	ldk	r4,#0
L3b8c:
	bit	r4,#5
	jr	ne,L3c08
	cpb	|_stkseg+9|(r15),#%20
	jr	eq,L3c08
L3b98:
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl4,rr2(#%0004)
	ldb	|_stkseg+9|(r15),rl4
	testb	rl4
	jr	ne,L3bac
	ldk	r4,#1
	jr	L3bae
L3bac:
	ldk	r4,#0
L3bae:
	bit	r4,#5
	jr	ne,L3c08
	cpb	|_stkseg+9|(r15),#%20
	jr	eq,L3c08
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#8
	ldl	rr12,rr2
	ldb	rl2,@rr12
	extsb	r2
	bit	r2,#7
	jr	eq,L3bcc
	inc	r13,#1
L3bcc:
	cpb	@rr12,#%5F
	jr	ne,L3bda
	inc	r13,#1
	cpb	@rr12,#%5F
	jr	eq,L3c08
L3bda:
	ldl	rr6,|_stkseg+0|(r15)
	inc	r7,#8
	call	L6522
	cp	r2,#%0008
	jr	ge,L3bf4
	ldk	r7,#9
	call	_putchar
L3bf4:
	ld	r7,#%0020
	call	_putchar
	sub	r7,r7
	ldl	rr4,#%01000F38
	calr	L3408
L3c08:
	ldm	r12,_stkseg+14(r15),#2
	add	r15,#%0012
	ret
L3c16:
	sub	r15,#%0026
	ldm	|_stkseg+0|(r15),r3,#5
	ldl	rr0,rr6
	inc	r1,#1
	ldl	rr6,rr0
	cpb	@rr6,#%39
	jr	gt,L3c48
	ldl	rr6,|_stkseg+6|(r15)
	ldb	rl2,rr6(#%0001)
	cpb	rl2,#%30
	jr	lt,L3c48
	ldb	|_stkseg+15|(r15),#%61
	ld	|_stkseg+26|(r15),#%0001
	jr	L3c60
L3c48:
	ld	|_stkseg+26|(r15),#%0002
	ldl	rr2,|_stkseg+6|(r15)
	ldb	rl4,rr2(#%0001)
	ldb	|_stkseg+15|(r15),rl4
	inc	<<2>>%E49C,#1
L3c60:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	ne,L3c82
	ldk	r7,#2
	ldl	rr4,#%01000F90
	ldl	rr2,|_stkseg+6|(r15)
	call	L3408
L3c82:
	ld	r2,|_stkseg+26|(r15)
	ld	|_stkseg+24|(r15),r2
	jr	L3ca2
L3c8c:
	ld	r2,|_stkseg+24|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%30
	jr	lt,L3cb4
	inc	|_stkseg+24|(r15),#1
L3ca2:
	ld	r2,|_stkseg+24|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%39
	jr	le,L3c8c
L3cb4:
	ld	r2,|_stkseg+24|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	eq,L3ce0
	ldb	rl3,rr4(r2)
	cpb	rl3,#%3A
	jr	eq,L3ce0
	ldk	r7,#2
	ldl	rr4,#%01000FA2
	ldl	rr2,|_stkseg+6|(r15)
	call	L3408
L3ce0:
	clr	|_stkseg+28|(r15)
	jr	L3d1e
L3ce6:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%30
	jr	lt,L3d30
	cp	|_stkseg+28|(r15),#%0003
	jr	ge,L3d30
	ldb	rl3,rr4(r2)
	ld	r6,|_stkseg+28|(r15)
	ldl	rr0,#%7F000010
	add	r1,r15
	add	r1,r6
	ldl	rr4,rr0
	ldb	@rr4,rl3
	inc	|_stkseg+26|(r15),#1
	inc	|_stkseg+28|(r15),#1
L3d1e:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%39
	jr	le,L3ce6
L3d30:
	ld	r2,|_stkseg+28|(r15)
	ldl	rr4,#%7F000010
	add	r5,r15
	add	r5,r2
	clrb	@rr4
	ldl	rr6,#%7F000010
	add	r7,r15
	call	_atoi
	ld	|_stkseg+30|(r15),r2
	test	<<2>>%E4AE
	jr	eq,L3d70
	cp	r2,#%007F
	jr	ne,L3d70
	ldk	r7,#2
	ldl	rr4,#%01000FB4
	ld	r3,r2
	call	L3408
L3d70:
	cp	|_stkseg+30|(r15),#%0000
	jr	lt,L3d80
	cp	|_stkseg+30|(r15),#%007F
	jr	le,L3d92
L3d80:
	ldk	r7,#2
	ldl	rr4,#%01000FDC
	ld	r3,|_stkseg+30|(r15)
	call	L3408
L3d92:
	ld	r2,|_stkseg+30|(r15)
	cp	r2,<<1>>%3E82
	jr	le,L3da6
	ld	r3,r2
	ld	<<1>>%3E82,r2
L3da6:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%3A
	jp	ne,L3f5a
	incb	<<1>>%42B2,#1
	inc	|_stkseg+26|(r15),#1
	ldb	rl6,|_stkseg+15|(r15)
	extsb	r6
	ld	r3,r6
	ldl	rr4,#%01001032
	ld	r6,#%0004
	cpir	r3,@rr4,r6,eq
	jr	ne,L3e14
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%114E(r6)
	jp	@rr4
	clr	|_stkseg+32|(r15)
	jr	L3e26
L3df2:
	ldk	r7,#2
	ldl	rr4,#%0100103A
	jr	L3e90
	ld	|_stkseg+32|(r15),#%0001
	jr	L3e26
	ld	|_stkseg+32|(r15),#%0002
	jr	L3e26
	ld	|_stkseg+32|(r15),#%0003
	jr	L3e26
L3e14:
	ldk	r7,#2
	ldl	rr4,#%01001000
	ldl	rr2,|_stkseg+6|(r15)
	call	L3408
L3e26:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%74
	jr	eq,L3e42
	ldb	rl3,rr4(r2)
	cpb	rl3,#%62
	jr	ne,L3e88
L3e42:
	ld	r2,|_stkseg+32|(r15)
	ld	r5,|_stkseg+30|(r15)
	mult	rr4,#%0005
	ldl	rr6,#%010042B4
	add	r7,r5
	ldb	rl3,rr6(r2)
	testb	rl3
	jr	ne,L3df2
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	ld	r6,|_stkseg+32|(r15)
	ld	r1,|_stkseg+30|(r15)
	mult	rr0,#%0005
	ldl	rr4,#%010042B4
	add	r5,r1
	add	r5,r6
	ldb	@rr4,rl3
	inc	|_stkseg+26|(r15),#1
	jr	L3e9a
L3e88:
	ldk	r7,#2
	ldl	rr4,#%0100105A
L3e90:
	ldl	rr2,|_stkseg+6|(r15)
	call	L3408
L3e9a:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	ne,L3ec6
	ldl	rr6,<<1>>%4A36
	ldl	rr0,@rr6
	ldl	|_stkseg+6|(r15),rr0
	inc	<<1>>%4A38,#4
	ldl	rr6,|_stkseg+2|(r15)
	inc	@rr6,#1
	clr	|_stkseg+26|(r15)
L3ec6:
	ldl	rr2,#%0100108A
	ldl	|_stkseg+20|(r15),rr2
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%30
	jr	eq,L3eea
	ldl	rr6,#%01001090
	jr	L3f10
L3eea:
	inc	|_stkseg+26|(r15),#1
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	extsb	r3
	or	r3,#%0020
	cp	r3,#%0078
	jr	ne,L3f14
	inc	|_stkseg+26|(r15),#1
	ldl	rr6,#%01001096
L3f10:
	ldl	|_stkseg+20|(r15),rr6
L3f14:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	eq,L3f7c
	ldl	rr6,rr4
	add	r7,r2
	ld	r1,|_stkseg+30|(r15)
	mult	rr0,#%000A
	ldl	rr2,#%01004534
	add	r3,r1
	ld	r0,|_stkseg+32|(r15)
	add	r0,r0
	add	r3,r0
	ldl	rr4,|_stkseg+20|(r15)
	call	_sscanf
	cp	r2,#%0001
	jr	eq,L3f7c
	ldk	r7,#2
	ldl	rr4,#%0100109C
	jr	L3f72
L3f5a:
	ld	r2,|_stkseg+26|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	eq,L3f7c
	ldk	r7,#2
	ldl	rr4,#%010010AC
L3f72:
	ldl	rr2,|_stkseg+6|(r15)
	call	L3408
L3f7c:
	ldl	rr2,<<1>>%4A36
	ldl	rr4,@rr2
	ldl	|_stkseg+10|(r15),rr4
	cpb	@rr4,#%2B
	jr	ne,L4046
	cpb	|_stkseg+15|(r15),#%73
	jr	ne,L3fa4
	ldk	r7,#2
	ldl	rr4,#%010010BE
	call	L3408
L3fa4:
	inc	<<1>>%4A38,#4
	ldl	rr2,<<1>>%4A36
	ldl	rr4,@rr2
	ldl	|_stkseg+10|(r15),rr4
	ldl	rr6,|_stkseg+2|(r15)
	inc	@rr6,#1
	jr	L4014
L3fbe:
	ldl	rr4,|_stkseg+2|(r15)
	ld	r6,@rr4
	cp	r6,|_stkseg+0|(r15)
	jr	ge,L401e
	ldl	rr6,|_stkseg+10|(r15)
	ld	r5,#%002F
	call	_strrchr
	ldl	|_stkseg+34|(r15),rr2
	testl	rr2
	jr	eq,L3fec
	inc	|_stkseg+36|(r15),#1
	ldl	rr2,|_stkseg+34|(r15)
	ldl	|_stkseg+10|(r15),rr2
L3fec:
	ldb	rl7,|_stkseg+15|(r15)
	extsb	r7
	ldl	rr4,|_stkseg+10|(r15)
	ld	r3,|_stkseg+30|(r15)
	calr	L410c
	ldl	rr2,|_stkseg+2|(r15)
	inc	@rr2,#1
	inc	<<1>>%4A38,#4
	ldl	rr4,<<1>>%4A36
	ldl	rr6,@rr4
	ldl	|_stkseg+10|(r15),rr6
L4014:
	ldl	rr2,|_stkseg+10|(r15)
	cpb	@rr2,#%2B
	jr	ne,L3fbe
L401e:
	ldl	rr2,|_stkseg+2|(r15)
	ld	r4,@rr2
	cp	r4,|_stkseg+0|(r15)
	jr	ne,L4038
	ldk	r7,#2
	ldl	rr4,#%010010D2
	call	L3408
L4038:
	ldl	rr2,|_stkseg+2|(r15)
	inc	@rr2,#1
	inc	<<1>>%4A38,#4
	jr	L4106
L4046:
	ldb	rl3,|_stkseg+15|(r15)
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001138
	ld	r6,#%0005
	cpir	r2,@rr4,r6,eq
	jr	ne,L40f2
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%1162(r6)
	jp	@rr4
	ld	r3,|_stkseg+30|(r15)
	ld	<<1>>%3E7E,r3
	jr	L4106
	ld	r3,|_stkseg+30|(r15)
	ld	<<1>>%3E7C,r3
	jr	L4106
	ld	r3,|_stkseg+30|(r15)
	ld	<<1>>%3E80,r3
	jr	L4106
	test	<<2>>%E49A
	jr	eq,L40a0
	ld	r3,|_stkseg+30|(r15)
	inc	r3,#1
	jr	L40a4
L40a0:
	ld	r3,|_stkseg+30|(r15)
L40a4:
	ld	<<1>>%3E80,r3
	ld	<<1>>%3E7C,r3
	ld	r3,|_stkseg+30|(r15)
	ld	<<1>>%3E7E,r3
	ld	r4,<<1>>%3E7C
	cp	r4,<<1>>%3E82
	jr	le,L4106
	ld	<<1>>%3E82,r4
	jr	L4106
	test	<<2>>%E4AE
	jr	eq,L40e6
	ldk	r7,#2
	ldl	rr4,#%010010F0
	call	L3408
L40e6:
	ld	r7,|_stkseg+30|(r15)
	ld	r6,#%0073
	calr	L42f8
	jr	L4106
L40f2:
	ldk	r7,#2
	ldl	rr4,#%01001110
	ldb	rl3,|_stkseg+15|(r15)
	extsb	r3
	call	L3408
L4106:
	add	r15,#%0026
	ret
L410c:
	sub	r15,#%0012
	ldm	|_stkseg+0|(r15),r3,#5
	cp	<<1>>%047A,#%0032
	jr	ne,L4132
	ldk	r7,#2
	ldl	rr4,#%01001166
	ld	r3,#%0032
	call	L3408
L4132:
	ld	r2,<<1>>%0478
	ld	|_stkseg+10|(r15),r2
	clr	|_stkseg+12|(r15)
	ld	<<1>>%0476,#%0001
	clr	|_stkseg+14|(r15)
	jr	L4198
L414e:
	ld	r5,|_stkseg+10|(r15)
	mult	rr4,#%000C
	ldl	rr6,<<1>>%3C2A(r5)
	ldl	rr4,rr6
	ldl	rr6,|_stkseg+2|(r15)
	call	L3a30
	ld	|_stkseg+16|(r15),r2
	cp	r2,#%0000
	jr	lt,L4194
	test	r2
	jp	eq,L427a
	ld	r2,|_stkseg+10|(r15)
	ld	|_stkseg+12|(r15),r2
	ld	r5,|_stkseg+10|(r15)
	mult	rr4,#%000C
	ld	r3,<<1>>%3C28(r5)
	ld	|_stkseg+10|(r15),r3
L4194:
	inc	|_stkseg+14|(r15),#1
L4198:
	ld	r2,|_stkseg+14|(r15)
	cp	r2,<<1>>%047A
	jr	lt,L414e
	ld	r2,|_stkseg+10|(r15)
	cp	r2,<<1>>%0478
	jr	ne,L41de
	test	|_stkseg+14|(r15)
	jr	ne,L41de
	ld	r3,<<1>>%047A
	mult	rr2,#%000C
	ldl	rr4,#%01003C28
	add	r5,r3
	ld	r2,<<1>>%0478
	ld	@rr4,r2
	ld	r2,<<1>>%047A
	ld	<<1>>%0478,r2
	jr	L4218
L41de:
	ld	r3,|_stkseg+12|(r15)
	mult	rr2,#%000C
	ld	r2,<<1>>%3C28(r3)
	ld	r5,<<1>>%047A
	mult	rr4,#%000C
	ldl	rr6,#%01003C28
	add	r7,r5
	ld	@rr6,r2
	ld	r5,|_stkseg+12|(r15)
	mult	rr4,#%000C
	ldl	rr6,#%01003C28
	add	r7,r5
	ld	r3,<<1>>%047A
	ld	@rr6,r3
L4218:
	ld	r2,<<1>>%047A
	ld	|_stkseg+14|(r15),r2
	inc	<<1>>%047A,#1
	ld	r5,|_stkseg+14|(r15)
	mult	rr4,#%000C
	ldl	rr6,#%01003C2A
	add	r7,r5
	ldl	rr4,|_stkseg+2|(r15)
	ldl	@rr6,rr4
	ld	r5,|_stkseg+14|(r15)
	mult	rr4,#%000C
	ldl	rr6,#%01003C26
	add	r7,r5
	ld	@rr6,#%FFFF
	ld	r5,|_stkseg+14|(r15)
	mult	rr4,#%000C
	ldl	rr6,#%01003C24
	add	r7,r5
	ld	@rr6,#%FFFF
	ld	r5,|_stkseg+14|(r15)
	mult	rr4,#%000C
	ldl	rr6,#%01003C22
	add	r7,r5
	ld	@rr6,#%FFFF
L427a:
	cpb	|_stkseg+9|(r15),#%61
	jr	eq,L428a
	cpb	|_stkseg+9|(r15),#%74
	jr	ne,L42a0
L428a:
	ld	r3,|_stkseg+14|(r15)
	mult	rr2,#%000C
	ldl	rr4,#%01003C22
	add	r5,r3
	ld	r2,|_stkseg+0|(r15)
	ld	@rr4,r2
L42a0:
	cpb	|_stkseg+9|(r15),#%61
	jr	eq,L42b0
	cpb	|_stkseg+9|(r15),#%64
	jr	ne,L42c6
L42b0:
	ld	r3,|_stkseg+14|(r15)
	mult	rr2,#%000C
	ldl	rr4,#%01003C24
	add	r5,r3
	ld	r2,|_stkseg+0|(r15)
	ld	@rr4,r2
L42c6:
	cpb	|_stkseg+9|(r15),#%61
	jr	eq,L42d6
	cpb	|_stkseg+9|(r15),#%62
	jr	ne,L42ec
L42d6:
	ld	r3,|_stkseg+14|(r15)
	mult	rr2,#%000C
	ldl	rr4,#%01003C26
	add	r5,r3
	ld	r2,|_stkseg+0|(r15)
	ld	@rr4,r2
L42ec:
	clr	<<1>>%0476
	add	r15,#%0012
	ret
L42f8:
	dec	r15,#4
	ldl	|_stkseg+0|(r15),rr6
	cp	r7,#%0000
	jr	lt,L430a
	cp	r7,#%0080
	jr	lt,L431c
L430a:
	ldk	r7,#2
	ldl	rr4,#%0100118E
	ld	r3,|_stkseg+2|(r15)
	call	L3408
L431c:
	ld	r2,|_stkseg+2|(r15)
	cp	r2,<<1>>%3E82
	jr	le,L4330
	ld	r3,r2
	ld	<<1>>%3E82,r2
L4330:
	ldb	rl3,|_stkseg+1|(r15)
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001266
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jr	ne,L4440
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%1274(r6)
	jp	@rr4
	ld	r3,|_stkseg+2|(r15)
	sla	r3,#3
	ld	r4,<<2>>%F338(r3)
	cp	r4,#%07D0
	jr	ne,L4380
	sub	r7,r7
	ldl	rr4,#%010011B8
	call	L3408
	ld	<<1>>%0474,#%FFFF
L4380:
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%03E8
	jr	L4440
	cp	<<1>>%0474,#%FFFF
	jr	le,L43b6
	ld	r2,<<1>>%0474
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%FFFF
L43b6:
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%03E8
	jr	ne,L43e2
	sub	r7,r7
	ldl	rr4,#%010011F2
	call	L3408
	ld	<<1>>%0474,#%FFFF
	jr	L4440
L43e2:
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%07D0
	ld	r2,|_stkseg+2|(r15)
	ld	<<1>>%0474,r2
	jr	L4440
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%07D0
	jr	ne,L442c
	sub	r7,r7
	ldl	rr4,#%0100122C
	call	L3408
	ld	<<1>>%0474,#%FFFF
L442c:
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%01F4
L4440:
	inc	r15,#4
	ret
L4444:
	dec	r15,#14
	ldm	|_stkseg+0|(r15),r4,#4
	clr	|_stkseg+8|(r15)
	ld	|_stkseg+10|(r15),#%FFFF
	jr	L4474
L4458:
	ld	r2,|_stkseg+8|(r15)
	ldl	rr4,|_stkseg+4|(r15)
	ldb	rl3,rr4(r2)
	cpb	rl3,#%2F
	jr	ne,L4470
	ld	r3,r2
	ld	|_stkseg+10|(r15),r2
L4470:
	inc	|_stkseg+8|(r15),#1
L4474:
	ld	r2,|_stkseg+8|(r15)
	ldl	rr4,|_stkseg+4|(r15)
	ldb	rl3,rr4(r2)
	testb	rl3
	jr	ne,L4458
	sub	r2,|_stkseg+10|(r15)
	dec	r2,#15
	ld	|_stkseg+12|(r15),r2
	cp	r2,#%0000
	jr	le,L44a0
	ld	r2,|_stkseg+8|(r15)
	sub	r2,|_stkseg+12|(r15)
	ld	|_stkseg+8|(r15),r2
L44a0:
	inc	|_stkseg+10|(r15),#1
	clr	|_stkseg+12|(r15)
	jr	L44cc
L44aa:
	ld	r2,|_stkseg+10|(r15)
	ldl	rr4,|_stkseg+4|(r15)
	ldb	rl3,rr4(r2)
	ld	r6,|_stkseg+12|(r15)
	ldl	rr0,|_stkseg+0|(r15)
	add	r1,r6
	ldl	rr4,rr0
	ldb	@rr4,rl3
	inc	|_stkseg+12|(r15),#1
	inc	|_stkseg+10|(r15),#1
L44cc:
	ld	r2,|_stkseg+10|(r15)
	cp	r2,|_stkseg+8|(r15)
	jr	lt,L44aa
	ld	r2,|_stkseg+12|(r15)
	ldl	rr4,|_stkseg+0|(r15)
	add	r5,r2
	clrb	@rr4
	inc	r15,#14
	ret
L44e6:
	sub	r15,#%0024
	ldm	_stkseg+24(r15),r8,#6
	clr	|_stkseg+16|(r15)
	ldl	rr6,#%01001278
	call	L6b5a
	ldl	rr4,@rr2
	ldl	<<2>>%E482,rr4
	ldl	rr6,#%01001280
	call	L6b5a
	ldl	rr4,@rr2
	ldl	<<2>>%E486,rr4
	ldl	rr6,#%01001288
	call	L6b5a
	ldl	rr4,@rr2
	ldl	<<2>>%E48A,rr4
	ldl	rr6,#%0100128E
	call	L6b5a
	ldl	rr4,@rr2
	ldl	<<2>>%E48E,rr4
	testl	rr4
	jr	ne,L456e
	cp	<<1>>%0474,#%FFFF
	jr	le,L4564
	ldk	r7,#1
	ldl	rr4,#%01001296
	call	L3408
	jr	L45aa
L4564:
	ld	<<1>>%0474,#%FFFE
	jr	L45aa
L456e:
	test	<<2>>%E49E
	jr	ne,L459a
	test	<<2>>%E49A
	jr	ne,L459a
	cp	<<1>>%0474,#%FFFF
	jr	ne,L459a
	cp	<<2>>%E49C,#%0002
	jr	ge,L459a
	test	<<2>>%E4AE
	jr	eq,L45a2
L459a:
	test	<<2>>%E4A2
	jr	eq,L45aa
L45a2:
	ld	<<1>>%0474,#%FFFD
L45aa:
	ld	r7,<<2>>%04FA
	call	L6fd0
	ldl	|_stkseg+12|(r15),rr2
	test	<<2>>%E4A2
	jr	ne,L460e
	sub	r7,r7
	call	L6fd0
L45ca:
	ldl	rr8,rr2
	cpl	rr2,|_stkseg+12|(r15)
	jr	nc,L460e
	ldb	rl2,rr8(#%0004)
	cpb	rl2,#%20
	jr	ne,L4660
	ldl	rr2,@rr8
	testl	rr2
	jr	ne,L4660
	cpl	rr8,<<2>>%E48A
	jr	eq,L4660
	cpl	rr8,<<2>>%E486
	jr	eq,L4660
	cpl	rr8,<<2>>%E482
	jr	eq,L4660
	cpl	rr8,<<2>>%E48E
	jr	eq,L4660
	inc	<<2>>%E4A2,#1
	clr	<<2>>%E498
L460e:
	test	<<2>>%E4A2
	jr	eq,L4676
	clr	<<2>>%E4A0
	clr	<<2>>%E4A4
	clrb	<<1>>%42B2
	clr	|_stkseg+22|(r15)
L462c:
	subl	rr2,rr2
L462e:
	ldl	|_stkseg+8|(r15),rr2
	cpl	rr2,#%00000004
	jr	nc,L466a
	ld	r4,|_stkseg+10|(r15)
	ld	r7,|_stkseg+22|(r15)
	mult	rr6,#%0005
	ldl	rr0,#%010042B4
	add	r1,r7
	add	r1,r4
	ldl	rr6,rr0
	clrb	@rr6
	ldl	rr2,|_stkseg+8|(r15)
	addl	rr2,#%00000001
	jr	L462e
L4660:
	ldl	rr6,rr8
	call	L705e
	jr	L45ca
L466a:
	inc	|_stkseg+22|(r15),#1
	cp	|_stkseg+22|(r15),#%0080
	jr	lt,L462c
L4676:
	subl	rr2,rr2
	ldl	<<2>>%E4C8,rr2
	test	<<2>>%E498
	jr	ne,L468e
	test	<<2>>%E4A2
	jr	ne,L469e
L468e:
	sub	r7,r7
	call	L6fd0
L4696:
	ldl	rr8,rr2
	cpl	rr2,|_stkseg+12|(r15)
	jr	c,L4762
L469e:
	calr	L4a16
	test	<<2>>%E498
	jr	ne,L46b0
	test	<<2>>%E4A2
	jr	ne,L46f8
L46b0:
	ldl	rr6,<<2>>%E482
	ld	r5,#%0022
	call	L68a2
	ldl	rr6,<<2>>%E486
	ld	r5,#%0023
	call	L68a2
	ldl	rr6,<<2>>%E48A
	ld	r5,#%0024
	call	L68a2
	test	<<2>>%E4A2
	jr	ne,L46f8
	ldl	rr6,<<2>>%E48E
	ld	r5,#%0021
	call	L68a2
L46f8:
	clr	<<1>>%04E6
	sub	r7,r7
	call	L6fd0
L4706:
	ldl	rr8,rr2
	cpl	rr2,|_stkseg+12|(r15)
	jp	nc,L4934
	cpl	rr2,<<2>>%E482
	jp	eq,L489a
	cpl	rr2,<<2>>%E486
	jp	eq,L489a
	cpl	rr2,<<2>>%E48A
	jp	eq,L489a
	cpl	rr2,<<2>>%E48E
	jp	eq,L489a
	ldb	rl3,rr8(#%0004)
	extsb	r3
	ld	r2,r3
	sub	r2,#%0020
	cp	r2,#%0005
	jp	ugt,L489a
	rl	r2,#2
	ldl	rr4,<<1>>%12F0(r2)
	jp	@rr4
L4762:
	ldb	rl2,rr8(#%0004)
	cpb	rl2,#%20
	jr	ne,L484c
	ldl	rr2,@rr8
	ldk	r2,#0
	ldl	|_stkseg+8|(r15),rr2
	testl	rr2
	jr	eq,L484c
	addl	rr2,#%00000001
	ldk	r2,#0
	and	r3,#%FFFE
	ldl	|_stkseg+8|(r15),rr2
	ldl	rr2,<<2>>%E4C8
	addl	rr2,|_stkseg+8|(r15)
	ldl	|_stkseg+4|(r15),rr2
	cpl	rr2,#%00010000
	jr	ugt,L47a4
	test	|_stkseg+16|(r15)
	jr	ne,L4812
L47a4:
	ld	r2,<<2>>%F32C
	cp	r2,<<1>>%04EC
	jr	lt,L47c0
	ldk	r7,#2
	ldl	rr4,#%010012B2
	call	L3408
L47c0:
	subl	rr2,rr2
	ldl	<<2>>%E4C8,rr2
	ldl	rr2,|_stkseg+8|(r15)
	ldl	|_stkseg+4|(r15),rr2
	ld	r7,<<2>>%F32C
	inc	<<2>>%F32C,#1
	call	L6c6
	ldl	|_stkseg+18|(r15),rr2
	add	r3,#%0034
	ld	@rr2,#%FFFF
	ldl	rr2,|_stkseg+18|(r15)
	inc	r3,#4
	clr	@rr2
	ldl	rr2,|_stkseg+18|(r15)
	inc	r3,#6
	clr	@rr2
	ldl	rr2,|_stkseg+18|(r15)
	add	r3,#%0026
	ld	r4,<<1>>%3E80
	ld	@rr2,r4
	inc	|_stkseg+16|(r15),#1
L4812:
	ldl	rr2,rr8
	inc	r3,#6
	ld	r4,<<2>>%F32C
	dec	r4,#1
	ld	@rr2,r4
	ldl	rr2,<<2>>%E4C8
	ldl	@rr8,rr2
	ldl	rr2,rr8
	inc	r3,#4
	ldb	@rr2,#%25
	ld	r7,<<2>>%F32C
	dec	r7,#1
	call	L6c6
	inc	r3,#8
	ldl	rr4,|_stkseg+4|(r15)
	ldl	<<2>>%E4C8,rr4
	ld	@rr2,r5
L484c:
	ldl	rr6,rr8
	call	L705e
	jp	L4696
	test	<<2>>%E496
	jr	ne,L489a
	set	<<1>>%4A3A,#0
	ldl	rr4,@rr8
	testl	rr4
	jr	ne,L489a
	test	<<1>>%04E6
	jr	ne,L4882
	ldl	rr6,#%010012E4
	call	_printf
L4882:
	inc	<<1>>%04E6,#1
	ldl	rr6,rr8
	inc	r7,#8
	call	L6522
	ldk	r7,#10
	call	_putchar
L489a:
	ldl	rr6,rr8
	call	L705e
	jp	L4706
	ld	r7,rr8(#%0006)
	call	L6c6
	ldl	rr12,rr2
	ldl	rr4,rr2(#%0016)
	ldl	rr10,rr4
	add	r3,#%0022
	ldl	rr4,@rr8
	add	r5,@rr2
	ldl	rr2,rr10
	add	r3,#%001A
L48c8:
	add	r5,@rr2
	ldk	r2,#0
	ld	r3,r5
	ldb	rl5,@rr10
	extsb	r5
	exts	rr4
	slal	rr4,#24
	addl	rr2,rr4
	ldl	@rr8,rr2
	ld	r2,rr10(#%0010)
	ldl	rr4,rr8
	inc	r5,#6
	ld	@rr4,r2
	jr	L489a
	ld	r7,rr8(#%0006)
	call	L6c6
	ldl	rr12,rr2
	ldl	rr4,rr2(#%001A)
	ldl	rr10,rr4
	add	r3,#%0024
	ldl	rr4,@rr8
	add	r5,@rr2
	ldl	rr2,rr10
	add	r3,#%001C
	jr	L48c8
	ldl	rr2,rr8
	inc	r3,#4
	ldb	@rr2,#%24
	ld	r7,rr8(#%0006)
	call	L6c6
	ldl	rr12,rr2
	ldl	rr4,rr2(#%001E)
	ldl	rr10,rr4
	add	r3,#%0026
	ldl	rr4,@rr8
	add	r5,@rr2
	ldl	rr2,rr10
	add	r3,#%001E
	jr	L48c8
L4934:
	test	<<2>>%E4A4
	jr	ne,L4944
	test	<<2>>%E4AA
	jr	eq,L494a
L4944:
	clr	<<2>>%E4CE
L494a:
	ld	r2,<<2>>%E4CE
	srl	r2,#4
	ld	<<2>>%F312,r2
	ldm	r8,_stkseg+24(r15),#6
	add	r15,#%0024
	ret
L4968:
	sub	r15,#%0012
	ldm	|_stkseg+4|(r15),r3,#5
	test	<<2>>%E4AE
	jr	eq,L498a
	cp	r3,#%007F
	jr	ne,L498a
	ldl	rr2,|_stkseg+6|(r15)
	inc	@rr2,#1
	sub	r2,r2
	jr	L4a10
L498a:
	ld	r7,<<1>>%3C04
	call	L719e
	ldl	|_stkseg+10|(r15),rr2
	ld	r2,<<1>>%3C04
	inc	<<1>>%3C04,#1
	cp	r2,#%0100
	jr	le,L49ba
	ldk	r7,#2
	ldl	rr4,#%01001308
	call	L3408
L49ba:
	ld	r2,|_stkseg+4|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0BB8
	jr	ne,L49d8
	ldl	rr4,|_stkseg+10|(r15)
	inc	r5,#10
	set	@rr4,#3
	jr	L49e2
L49d8:
	ldl	rr2,|_stkseg+10|(r15)
	inc	r3,#10
	ld	@rr2,#%0004
L49e2:
	ld	|_stkseg+14|(r15),#%0001
	ldl	rr2,#%7F00000E
	add	r3,r15
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr6,|_stkseg+10|(r15)
	ldl	rr4,|_stkseg+6|(r15)
	sub	r3,r3
	ld	r2,#%0072
	call	L5eec
	ldl	rr4,_stkseg+22(r15)
	ld	@rr4,r2
L4a10:
	add	r15,#%0012
	ret
L4a16:
	sub	r15,#%0036
	ldm	_stkseg+42(r15),r8,#6
	subl	rr2,rr2
	ldl	<<1>>%3C0E,rr2
	subl	rr2,rr2
	ldl	<<1>>%3C0A,rr2
	subl	rr2,rr2
	ldl	<<1>>%3C06,rr2
	subl	rr2,rr2
	ldl	<<1>>%3C12,rr2
	subl	rr2,rr2
	ldl	<<2>>%F32E,rr2
	clr	<<1>>%3C04
	ld	|_stkseg+24|(r15),#%0001
L4a56:
	clr	|_stkseg+26|(r15)
	jp	L4c16
L4a60:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr12,rr2
	ld	r2,rr12(#%0004)
	test	r2
	jr	eq,L4ada
	cp	|_stkseg+24|(r15),#%0001
	jr	ne,L4ada
	ld	r6,r2
	ld	r7,rr12(#%0022)
	sla	r7,#3
	ld	r5,<<2>>%F332(r7)
	ld	r3,rr12(#%0022)
	ld	r4,|_stkseg+26|(r15)
	ldk	r7,#2
	calr	L5a2c
	ldl	rr4,rr12
	add	r5,#%0016
	ldl	@rr4,rr2
	ldl	rr2,rr12(#%0016)
	ld	r4,rr2(#%0004)
	ldl	rr2,rr12
	add	r3,#%0022
	ld	@rr2,r4
	ld	r2,rr12(#%0004)
	ldl	rr4,rr12(#%0016)
	inc	r5,#4
	add	r2,@rr4
	ld	@rr4,r2
	ld	r2,rr12(#%0004)
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<1>>%3C16
	ldl	<<1>>%3C16,rr4
	ldl	rr2,rr12(#%0016)
	inc	r3,#10
	set	@rr2,#0
L4ada:
	ld	r2,rr12(#%0006)
	test	r2
	jr	eq,L4b76
	cp	|_stkseg+24|(r15),#%0002
	jr	ne,L4b76
	ld	r2,rr12(#%0024)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jr	le,L4b16
	ld	r6,rr12(#%0006)
	ld	r2,rr12(#%0024)
	sla	r2,#3
	ld	r5,<<2>>%F336(r2)
	sub	r5,#%04B0
	jr	L4b28
L4b16:
	ld	r6,rr12(#%0006)
	ld	r7,rr12(#%0024)
	sla	r7,#3
	ld	r5,<<2>>%F334(r7)
L4b28:
	ld	r3,rr12(#%0024)
	ld	r4,|_stkseg+26|(r15)
	ldk	r7,#3
	calr	L5a2c
	ldl	rr4,rr12
	add	r5,#%001A
	ldl	@rr4,rr2
	ldl	rr2,rr12(#%001A)
	ld	r4,rr2(#%0006)
	ldl	rr2,rr12
	add	r3,#%0024
	ld	@rr2,r4
	ld	r2,rr12(#%0006)
	ldl	rr4,rr12(#%001A)
	inc	r5,#6
	add	r2,@rr4
	ld	@rr4,r2
	ld	r2,rr12(#%0006)
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<1>>%3C1A
	ldl	<<1>>%3C1A,rr4
	ldl	rr2,rr12(#%001A)
	inc	r3,#10
	set	@rr2,#1
L4b76:
	ld	r2,rr12(#%0008)
	test	r2
	jr	eq,L4c12
	cp	|_stkseg+24|(r15),#%0003
	jr	ne,L4c12
	ld	r2,rr12(#%0026)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	le,L4bb2
	ld	r6,rr12(#%0008)
	ld	r2,rr12(#%0026)
	sla	r2,#3
	ld	r5,<<2>>%F336(r2)
	sub	r5,#%04B0
	jr	L4bc4
L4bb2:
	ld	r6,rr12(#%0008)
	ld	r7,rr12(#%0026)
	sla	r7,#3
	ld	r5,<<2>>%F336(r7)
L4bc4:
	ld	r3,rr12(#%0026)
	ld	r4,|_stkseg+26|(r15)
	ldk	r7,#4
	calr	L5a2c
	ldl	rr4,rr12
	add	r5,#%001E
	ldl	@rr4,rr2
	ldl	rr2,rr12(#%001E)
	ld	r4,rr2(#%0008)
	ldl	rr2,rr12
	add	r3,#%0026
	ld	@rr2,r4
	ld	r2,rr12(#%0008)
	ldl	rr4,rr12(#%001E)
	inc	r5,#8
	add	r2,@rr4
	ld	@rr4,r2
	ld	r2,rr12(#%0008)
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,<<1>>%3C1E
	ldl	<<1>>%3C1E,rr4
	ldl	rr2,rr12(#%001E)
	inc	r3,#10
	set	@rr2,#2
L4c12:
	inc	|_stkseg+26|(r15),#1
L4c16:
	ld	r2,|_stkseg+26|(r15)
	cp	r2,<<2>>%F32C
	jp	lt,L4a60
	inc	|_stkseg+24|(r15),#1
	cp	|_stkseg+24|(r15),#%0004
	jp	lt,L4a56
	cp	<<1>>%0474,#%0000
	jr	lt,L4cf8
	cp	<<1>>%0474,#%0080
	jr	ge,L4cf8
	ldl	rr2,#%7F00FFFF
	ldl	|_stkseg+14|(r15),rr2
	ldl	rr2,#%7F00FFFF
	ldl	|_stkseg+18|(r15),rr2
	ld	r2,<<1>>%0474
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jr	eq,L4c86
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%0016)
	jr	L4cd4
L4c86:
	ld	r2,<<1>>%0474
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jr	eq,L4cae
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%001A)
	jr	L4cd4
L4cae:
	ld	r2,<<1>>%0474
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jr	eq,L4cd8
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%001E)
L4cd4:
	ldl	|_stkseg+14|(r15),rr4
L4cd8:
	ldl	rr2,|_stkseg+18|(r15)
	cpl	rr2,|_stkseg+14|(r15)
	jr	ne,L4cf8
	ld	r4,<<1>>%0474
	sla	r4,#3
	ldl	rr6,#%0200F338
	add	r7,r4
	ld	@rr6,#%0BB8
L4cf8:
	subl	rr2,rr2
	ldl	<<2>>%F32E,rr2
	ld	r2,<<1>>%3E82
	ld	|_stkseg+4|(r15),r2
	clr	|_stkseg+22|(r15)
	clr	|_stkseg+24|(r15)
L4d12:
	ld	r2,|_stkseg+24|(r15)
	cp	r2,|_stkseg+4|(r15)
	jp	gt,L4f08
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%01F4
	jp	le,L4ef0
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%07D0
	jp	eq,L4ef0
	ldl	rr4,#%7F00001C
	add	r5,r15
	ldl	|_stkseg+0|(r15),rr4
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	ld	r3,|_stkseg+24|(r15)
	calr	L4968
	dec	|_stkseg+22|(r15),#1
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%FFFF
	jr	ne,L4dae
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jr	ne,L4dae
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jp	eq,L4ef0
L4dae:
	clr	|_stkseg+30|(r15)
L4db2:
	cp	|_stkseg+30|(r15),#%0080
	jp	ge,L4ef0
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%FFFF
	jp	ne,L4efe
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jp	ne,L4efe
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jr	ne,L4efe
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%FFFF
	jr	ne,L4efe
	sub	r7,r7
	ldl	rr4,#%01001326
	ld	r3,|_stkseg+24|(r15)
	ld	r2,|_stkseg+30|(r15)
	call	L3408
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F332
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+30|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%01F4
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F332
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+30|(r15)
	cp	r2,<<1>>%3E82
	jr	le,L4ef0
	ld	r4,r2
	ld	<<1>>%3E82,r2
L4ef0:
	inc	|_stkseg+24|(r15),#1
	inc	|_stkseg+22|(r15),#1
	jp	L4d12
L4efe:
	inc	|_stkseg+30|(r15),#1
	jp	L4db2
L4f08:
	ld	r2,<<1>>%3E82
	ld	|_stkseg+4|(r15),r2
	clr	|_stkseg+24|(r15)
	clr	|_stkseg+22|(r15)
	jp	L593c
L4f20:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0064
	jr	eq,L4fac
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0032
	jr	eq,L4fac
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%FFFF
	jr	ne,L4fb6
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%0000
	jr	ge,L4fb6
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%0000
	jr	ge,L4fb6
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%0000
	jr	lt,L4fac
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	le,L4fb6
L4fac:
	inc	|_stkseg+22|(r15),#1
	jp	L5938
L4fb6:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%03E8
	jr	eq,L4fac
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0BB8
	jr	eq,L4fac
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jr	eq,L50cc
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%0016)
L5002:
	ldl	|_stkseg+14|(r15),rr4
	clr	<<2>>%F320
	clr	<<2>>%F322
	ldl	rr6,rr4
	ld	r5,|_stkseg+22|(r15)
	call	L18f2
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%FFFF
	jp	le,L5368
	ldl	rr4,|_stkseg+14|(r15)
	ldl	<<1>>%3C0A,rr4
	ldl	rr6,|_stkseg+14|(r15)
	cpb	@rr6,#%FF
	jr	ne,L5074
	ld	|_stkseg+32|(r15),#%0001
	ldl	rr0,#%7F000020
	add	r1,r15
	ldl	|_stkseg+0|(r15),rr0
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	sub	r3,r3
	ld	r2,#%0074
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
L5074:
	test	<<2>>%E4A2
	jr	ne,L5168
	cp	<<1>>%4A34,#%0001
	jr	ne,L5094
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001A)
	and	r4,#%00FF
	jr	ne,L50b2
L5094:
	ld	r2,|_stkseg+22|(r15)
	dec	r2,#1
	ld	r5,r2
	mult	rr4,#%0005
	ldl	rr2,#%010042B4
	add	r3,r5
	ldb	rl4,rr2(#%0001)
	cpb	rl4,#%74
	jr	ne,L5120
L50b2:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0004)
L50ba:
	and	r4,#%00FF
	ld	r5,r4
	neg	r5
	add	r5,#%0100
	ld	|_stkseg+28|(r15),r5
	jr	L5144
L50cc:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jr	eq,L50f2
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%001A)
	jr	L5002
L50f2:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	|_stkseg+26|(r15),r3
	cp	r3,#%FFFF
	jp	eq,L5938
	ld	r7,r3
	call	L6c6
	ldl	rr4,rr2(#%001E)
	jp	L5002
L5120:
	cp	<<1>>%4A34,#%0001
	jr	ne,L5140
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001A)
	ld	r5,r4
	and	r5,#%FF00
	sub	r4,r5
	inc	r3,#4
	add	r4,@rr2
	jr	L50ba
L5140:
	clr	|_stkseg+28|(r15)
L5144:
	cp	<<1>>%4A34,#%0001
	jr	ne,L516c
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0004)
	cp	r4,#%FF00
	jr	ule,L516c
	ld	r2,|_stkseg+28|(r15)
	dec	r2,#2
	ld	|_stkseg+28|(r15),r2
	jr	L516c
L5168:
	clr	|_stkseg+28|(r15)
L516c:
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C16
	ldl	<<1>>%3C16,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#4
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
	ldl	rr2,|_stkseg+14|(r15)
	ldb	rl4,@rr2
	extsb	r4
	cp	r4,<<1>>%0474
	jp	ne,L534a
	inc	r3,#10
	set	@rr2,#3
	jp	L534a
L51aa:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	add	r3,#%0016
	ldl	rr4,|_stkseg+14|(r15)
	cpl	rr4,@rr2
	jp	eq,L534a
	ld	r3,|_stkseg+24|(r15)
	mult	rr2,#%0005
	ldl	rr6,#%010042B4
	add	r7,r3
	ldb	rl2,rr6(#%0001)
	testb	rl2
	jr	eq,L51fa
	ldl	rr2,<<1>>%04B0
	ldl	|_stkseg+0|(r15),rr2
	ld	r3,|_stkseg+22|(r15)
	dec	r3,#1
	ldl	rr4,#%0100137A
	ldk	r7,#2
	call	L3408
L51fa:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr4,rr2(#%0016)
	ldl	|_stkseg+14|(r15),rr4
	ldl	rr2,rr4
	add	r3,#%001A
	clr	@rr2
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<1>>%3C0A,rr2
	ldl	rr6,|_stkseg+14|(r15)
	ld	r5,|_stkseg+22|(r15)
	call	L18f2
	ld	r2,|_stkseg+26|(r15)
	ld	|_stkseg+32|(r15),r2
	ldl	rr4,#%7F000020
	add	r5,r15
	ldl	|_stkseg+0|(r15),rr4
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	ldk	r3,#1
	ld	r2,#%0074
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0008)
	inc	r3,#6
	add	r4,@rr2
	jr	eq,L52ce
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	r2,|_stkseg+32|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	r2,|_stkseg+32|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	add	r5,r2
	ld	@rr4,#%FFFF
L52ce:
	ldl	rr2,rr10
	add	r3,#%0030
	ld	@rr2,#%FFFF
	cp	<<1>>%4A34,#%0001
	jr	ne,L52fc
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C16
	ldl	<<1>>%3C16,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#4
	jr	L5342
L52fc:
	cp	<<1>>%4A34,#%0002
	jr	ne,L5320
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1A
	ldl	<<1>>%3C1A,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#6
	jr	L5342
L5320:
	cp	<<1>>%4A34,#%0003
	jr	ne,L534a
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1E
	ldl	<<1>>%3C1E,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#8
L5342:
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
L534a:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr10,rr2
	ld	r4,rr2(#%0030)
	ld	|_stkseg+26|(r15),r4
	cp	r4,#%FFFF
	jp	gt,L51aa
L5368:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jp	le,L565e
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	|_stkseg+26|(r15),r3
	ld	r7,r3
	call	L6c6
	ldl	rr12,rr2
	ldl	rr2,rr12(#%001A)
	ldl	|_stkseg+18|(r15),rr2
	test	<<2>>%F320
	jr	eq,L53c6
	ldl	rr2,<<2>>%F324
	ldl	|_stkseg+14|(r15),rr2
	ldl	rr4,rr12
	add	r5,#%001A
	ldl	rr6,rr2
	ldl	@rr4,rr2
	dec	|_stkseg+22|(r15),#1
	jr	L53ce
L53c6:
	ldl	rr2,|_stkseg+18|(r15)
	ldl	|_stkseg+14|(r15),rr2
L53ce:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<1>>%3C06,rr2
	ldl	rr4,|_stkseg+14|(r15)
	cpb	@rr4,#%FF
	jr	ne,L540c
	ld	|_stkseg+32|(r15),#%0001
	ldl	rr6,#%7F000020
	add	r7,r15
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	sub	r3,r3
	ld	r2,#%0064
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
L540c:
	test	<<2>>%E4A2
	jr	ne,L54b2
	cp	<<1>>%4A34,#%0002
	jr	ne,L542e
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001C)
	and	r4,#%00FF
	test	r4
	jr	eq,L544c
L542e:
	ld	r2,|_stkseg+22|(r15)
	dec	r2,#1
	ld	r5,r2
	mult	rr4,#%0005
	ldl	rr2,#%010042B4
	add	r3,r5
	ldb	rl4,rr2(#%0002)
	cpb	rl4,#%74
	jr	ne,L547c
L544c:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0006)
	jr	L546a
L5456:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001C)
	ld	r5,r4
	and	r5,#%FF00
	sub	r4,r5
	inc	r3,#6
	add	r4,@rr2
L546a:
	and	r4,#%00FF
	ld	r5,r4
	neg	r5
	add	r5,#%0100
	ld	|_stkseg+28|(r15),r5
	jr	L548a
L547c:
	cp	<<1>>%4A34,#%0002
	jr	eq,L5456
	clr	|_stkseg+28|(r15)
L548a:
	cp	<<1>>%4A34,#%0002
	jr	ne,L54b6
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0004)
	inc	r3,#6
	add	r4,@rr2
	cp	r4,#%FF00
	jr	ule,L54b6
	ld	r2,|_stkseg+28|(r15)
	dec	r2,#2
	ld	|_stkseg+28|(r15),r2
	jr	L54b6
L54b2:
	clr	|_stkseg+28|(r15)
L54b6:
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1A
	ldl	<<1>>%3C1A,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#6
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
	ldl	rr2,|_stkseg+14|(r15)
	ldb	rl4,@rr2
	extsb	r4
	cp	r4,<<1>>%0474
	jp	ne,L5644
	inc	r3,#10
	set	@rr2,#3
	jp	L5644
L54f4:
	ld	r3,|_stkseg+24|(r15)
	mult	rr2,#%0005
	ldl	rr6,#%010042B4
	add	r7,r3
	ldb	rl2,rr6(#%0002)
	testb	rl2
	jr	eq,L552a
	ldl	rr2,<<1>>%04B4
	ldl	|_stkseg+0|(r15),rr2
	ld	r3,|_stkseg+22|(r15)
	dec	r3,#1
	ldl	rr4,#%010013A6
	ldk	r7,#2
	call	L3408
L552a:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr4,rr2(#%001A)
	ldl	|_stkseg+14|(r15),rr4
	ldl	rr2,rr4
	add	r3,#%001C
	clr	@rr2
	ldl	rr6,|_stkseg+14|(r15)
	ld	r5,|_stkseg+22|(r15)
	call	L18f2
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<1>>%3C06,rr2
	ld	r4,|_stkseg+26|(r15)
	ld	|_stkseg+32|(r15),r4
	ldl	rr6,#%7F000020
	add	r7,r15
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	ldk	r3,#1
	ld	r2,#%0064
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
	ldl	rr2,rr10
	add	r3,#%0032
	ld	@rr2,#%FFFF
	cp	<<1>>%4A34,#%0002
	jr	ne,L55be
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1A
	ldl	<<1>>%3C1A,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#6
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
	jr	L5612
L55be:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	r2,|_stkseg+32|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,r3
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
	ld	r5,|_stkseg+28|(r15)
	exts	rr4
	addl	rr4,<<1>>%3C1E
	ldl	<<1>>%3C1E,rr4
	ldl	rr4,|_stkseg+14|(r15)
	inc	r5,#8
	ld	r2,@rr4
	add	r2,|_stkseg+28|(r15)
	ld	@rr4,r2
L5612:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+18|(r15),rr2
	jr	L5644
L561c:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr12,rr2
	add	r3,#%001A
	ldl	rr4,|_stkseg+18|(r15)
	cpl	rr4,@rr2
	jp	ne,L54f4
	ldl	rr2,rr12
	add	r3,#%001A
	ldl	rr4,|_stkseg+14|(r15)
	ldl	@rr2,rr4
L5644:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr10,rr2
	ld	r4,rr2(#%0032)
	ld	|_stkseg+26|(r15),r4
	cp	r4,#%FFFF
	jr	gt,L561c
L565e:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jp	le,L5910
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04B0
	jp	ge,L5910
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	ld	|_stkseg+26|(r15),r3
	ld	r7,r3
	call	L6c6
	ldl	rr12,rr2
	ldl	rr2,rr12(#%001E)
	ldl	|_stkseg+18|(r15),rr2
	test	<<2>>%F322
	jr	eq,L56d4
	ldl	rr2,<<2>>%F328
	ldl	|_stkseg+14|(r15),rr2
	ldl	rr4,rr12
	add	r5,#%001E
	ldl	rr6,rr2
	ldl	@rr4,rr2
	dec	|_stkseg+22|(r15),#1
	jr	L56dc
L56d4:
	ldl	rr2,|_stkseg+18|(r15)
	ldl	|_stkseg+14|(r15),rr2
L56dc:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<1>>%3C0E,rr2
	ldl	rr4,|_stkseg+14|(r15)
	cpb	@rr4,#%FF
	jr	ne,L571a
	ld	|_stkseg+32|(r15),#%0001
	ldl	rr6,#%7F000020
	add	r7,r15
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	sub	r3,r3
	ld	r2,#%0062
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
L571a:
	test	<<2>>%E4A2
	jr	ne,L57c8
	cp	<<1>>%4A34,#%0003
	jr	ne,L573c
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001E)
	and	r4,#%00FF
	test	r4
	jr	eq,L575a
L573c:
	ld	r2,|_stkseg+22|(r15)
	dec	r2,#1
	ld	r5,r2
	mult	rr4,#%0005
	ldl	rr2,#%010042B4
	add	r3,r5
	ldb	rl4,rr2(#%0003)
	cpb	rl4,#%74
	jr	ne,L578a
L575a:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0008)
	jr	L5778
L5764:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%001E)
	ld	r5,r4
	and	r5,#%FF00
	sub	r4,r5
	inc	r3,#8
	add	r4,@rr2
L5778:
	and	r4,#%00FF
	ld	r5,r4
	neg	r5
	add	r5,#%0100
	ld	|_stkseg+28|(r15),r5
	jr	L5798
L578a:
	cp	<<1>>%4A34,#%0003
	jr	eq,L5764
	clr	|_stkseg+28|(r15)
L5798:
	cp	<<1>>%4A34,#%0003
	jr	ne,L57cc
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,rr2(#%0004)
	inc	r3,#6
	add	r4,@rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#8
	add	r4,@rr2
	cp	r4,#%FF00
	jr	ule,L57cc
	ld	r2,|_stkseg+28|(r15)
	dec	r2,#2
	ld	|_stkseg+28|(r15),r2
	jr	L57cc
L57c8:
	clr	|_stkseg+28|(r15)
L57cc:
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1E
	ldl	<<1>>%3C1E,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#8
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
	ldl	rr2,|_stkseg+14|(r15)
	ldb	rl4,@rr2
	extsb	r4
	cp	r4,<<1>>%0474
	jr	ne,L58f6
	inc	r3,#10
	set	@rr2,#3
	jr	L58f6
L5802:
	ld	r3,|_stkseg+24|(r15)
	mult	rr2,#%0005
	ldl	rr6,#%010042B4
	add	r7,r3
	ldb	rl2,rr6(#%0003)
	testb	rl2
	jr	eq,L5838
	ldl	rr2,<<1>>%04B8
	ldl	|_stkseg+0|(r15),rr2
	ld	r3,|_stkseg+22|(r15)
	dec	r3,#1
	ldl	rr4,#%010013D2
	ldk	r7,#2
	call	L3408
L5838:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr4,rr2(#%001E)
	ldl	|_stkseg+14|(r15),rr4
	ldl	rr2,rr4
	ld	r4,rr2(#%001C)
	inc	r3,#6
	add	r4,@rr2
	ldl	rr2,|_stkseg+14|(r15)
	add	r3,#%001E
	ld	@rr2,r4
	ldl	rr2,|_stkseg+14|(r15)
	ldl	<<1>>%3C0E,rr2
	ldl	rr4,|_stkseg+14|(r15)
	cpb	@rr4,#%FF
	jr	ne,L58c8
	ld	r6,|_stkseg+26|(r15)
	ld	|_stkseg+32|(r15),r6
	ldl	rr0,#%7F000020
	add	r1,r15
	ldl	|_stkseg+0|(r15),rr0
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	ldk	r3,#1
	ld	r2,#%0062
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
	ldl	rr2,rr10
	add	r3,#%0034
	ld	@rr2,#%FFFF
	ld	r3,|_stkseg+28|(r15)
	exts	rr2
	addl	rr2,<<1>>%3C1E
	ldl	<<1>>%3C1E,rr2
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#8
	ld	r4,@rr2
	add	r4,|_stkseg+28|(r15)
	ld	@rr2,r4
L58c8:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+18|(r15),rr2
	jr	L58f6
L58d2:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr12,rr2
	add	r3,#%001E
	ldl	rr4,|_stkseg+18|(r15)
	cpl	rr4,@rr2
	jr	ne,L5802
	ldl	rr2,rr12
	add	r3,#%001E
	ldl	rr4,|_stkseg+14|(r15)
	ldl	@rr2,rr4
L58f6:
	ld	r7,|_stkseg+26|(r15)
	call	L6c6
	ldl	rr10,rr2
	ld	r4,rr2(#%0034)
	ld	|_stkseg+26|(r15),r4
	cp	r4,#%FFFF
	jr	gt,L58d2
L5910:
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	le,L5938
	ld	r2,|_stkseg+24|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
L5938:
	inc	|_stkseg+24|(r15),#1
L593c:
	ld	r2,|_stkseg+24|(r15)
	cp	r2,|_stkseg+4|(r15)
	jp	le,L4f20
	cp	<<1>>%0474,#%FFFF
	jr	ne,L59ca
	ld	r7,<<1>>%3C04
	call	L719e
	ldl	|_stkseg+14|(r15),rr2
	ld	r2,<<1>>%3C04
	inc	<<1>>%3C04,#1
	cp	r2,#%0100
	jr	le,L5984
	ldk	r7,#2
	ldl	rr4,#%010013FE
	call	L3408
L5984:
	inc	<<1>>%3E82,#1
	ldl	rr2,|_stkseg+14|(r15)
	inc	r3,#10
	ld	@rr2,#%0008
	ld	|_stkseg+32|(r15),#%0001
	ldl	rr2,#%7F000020
	add	r3,r15
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr4,#%7F000016
	add	r5,r15
	ldl	rr6,|_stkseg+14|(r15)
	sub	r3,r3
	ld	r2,#%0073
	calr	L5eec
	ld	|_stkseg+28|(r15),r2
	ld	r2,|_stkseg+22|(r15)
	dec	r2,#1
	ld	<<1>>%0474,r2
L59ca:
	ldl	rr2,<<1>>%3C12
	cpl	rr2,<<2>>%F32E
	jr	eq,L5a1e
L59d8:
	clr	|_stkseg+40|(r15)
	ldl	rr8,<<2>>%F32E
	ldl	rr2,rr8(#%0012)
	ldl	|_stkseg+36|(r15),rr2
L59ea:
	ldl	rr2,|_stkseg+36|(r15)
	ldb	rl4,@rr2
	cpb	rl4,@rr8
	jr	ge,L5a02
	ld	|_stkseg+40|(r15),#%0001
	ldl	rr6,rr8
	ldl	rr4,|_stkseg+36|(r15)
	calr	L642e
L5a02:
	ldl	rr8,|_stkseg+36|(r15)
	ldl	rr2,rr8
	ldl	rr4,rr2(#%0012)
	ldl	|_stkseg+36|(r15),rr4
	cpl	rr8,<<1>>%3C12
	jr	ne,L59ea
	test	|_stkseg+40|(r15)
	jr	ne,L59d8
L5a1e:
	ldm	r8,_stkseg+42(r15),#6
	add	r15,#%0036
	ret
L5a2c:
	sub	r15,#%001E
	ldm	_stkseg+18(r15),r8,#6
	ldm	|_stkseg+0|(r15),r3,#5
	ld	r13,r6
	cp	r5,#%FFFF
	jp	le,L5caa
L5a4a:
	cp	|_stkseg+4|(r15),#%FFFF
	jp	eq,L5caa
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+10|(r15),r2
	ld	r3,|_stkseg+8|(r15)
	ldl	rr4,#%0100141C
	ld	r6,#%0003
	cpir	r3,@rr4,r6,eq
	jr	ne,L5a9a
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%1456(r6)
	jp	@rr4
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	ldl	rr4,rr2(#%0016)
	ldl	rr10,rr4
	ld	r2,rr8(#%0030)
L5a96:
	ld	|_stkseg+4|(r15),r2
L5a9a:
	ld	r2,rr10(#%0004)
	ldl	rr4,rr10
	inc	r5,#6
	add	r2,@rr4
	ldl	rr4,rr10
	inc	r5,#8
	add	r2,@rr4
	ld	r3,#%FFFE
	sub	r3,r2
	cp	r13,r3
	jr	ugt,L5a4a
	ld	r2,|_stkseg+8|(r15)
	ldl	rr4,#%01001422
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jp	ne,L5edc
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%1462(r6)
	jp	@rr4
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	le,L5b1a
L5aee:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr4,rr2(#%0016)
	ldl	rr10,rr4
	ld	|_stkseg+4|(r15),#%FFFF
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	add	r5,r2
	ld	@rr4,#%FFFF
	jr	L5a9a
L5b1a:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	ldl	rr4,rr2(#%001A)
	ldl	rr10,rr4
	ld	r2,rr8(#%0032)
	jr	L5a96
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	gt,L5aee
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	ldl	rr4,rr2(#%001E)
	ldl	rr10,rr4
	ld	r2,rr8(#%0034)
	jr	L5a96
	ld	r3,|_stkseg+10|(r15)
	ld	|_stkseg+4|(r15),r3
	jr	L5b8c
L5b68:
	ld	r2,rr8(#%0030)
	cp	r2,#%FFFF
	jr	eq,L5ba0
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+10|(r15),r2
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ld	r4,rr2(#%0030)
	ld	|_stkseg+4|(r15),r4
L5b8c:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	add	r3,#%0016
	cpl	rr10,@rr2
	jr	eq,L5b68
L5ba0:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	add	r3,#%0016
	cpl	rr10,@rr2
	jr	ne,L5bba
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+10|(r15),r2
L5bba:
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	ldl	rr8,rr2
	ld	r4,rr2(#%0030)
	ld	|_stkseg+12|(r15),r4
	add	r3,#%0030
	ld	r4,|_stkseg+2|(r15)
	ld	@rr2,r4
	ld	r7,|_stkseg+2|(r15)
	call	L6c6
	add	r3,#%0030
	jp	L5ed6
L5bec:
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
L5bfa:
	add	r5,r2
	ld	r2,|_stkseg+2|(r15)
	ld	@rr4,r2
	jp	L5edc
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jr	eq,L5bec
	ld	r2,|_stkseg+10|(r15)
	ld	|_stkseg+4|(r15),r2
	jr	L5c4a
L5c26:
	ld	r2,rr8(#%0032)
	cp	r2,#%FFFF
	jr	eq,L5c5e
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+10|(r15),r2
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ld	r4,rr2(#%0032)
	ld	|_stkseg+4|(r15),r4
L5c4a:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	add	r3,#%001A
	cpl	rr10,@rr2
	jr	eq,L5c26
L5c5e:
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	add	r3,#%001A
	cpl	rr10,@rr2
	jr	ne,L5c78
	ld	r2,|_stkseg+4|(r15)
	ld	|_stkseg+10|(r15),r2
L5c78:
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	ldl	rr8,rr2
	ld	r4,rr2(#%0032)
	ld	|_stkseg+12|(r15),r4
	add	r3,#%0032
	ld	r4,|_stkseg+2|(r15)
	ld	@rr2,r4
	ld	r7,|_stkseg+2|(r15)
	call	L6c6
	add	r3,#%0032
	jp	L5ed6
L5caa:
	cp	|_stkseg+8|(r15),#%0004
	jr	ne,L5d04
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	ld	|_stkseg+4|(r15),r3
L5cc4:
	cp	|_stkseg+4|(r15),#%FFFF
	jr	le,L5d04
	ld	r7,|_stkseg+4|(r15)
	call	L6c6
	ldl	rr8,rr2
	ldl	rr4,rr2(#%001A)
	ldl	rr10,rr4
	ld	r2,rr8(#%0032)
	ld	|_stkseg+4|(r15),r2
	ld	r2,rr10(#%0004)
	inc	r5,#6
	add	r2,@rr4
	ldl	rr4,rr10
	inc	r5,#8
	add	r2,@rr4
	ld	r3,#%FFFE
	sub	r3,r2
	cp	r13,r3
	jr	ugt,L5cc4
	jp	L5e82
L5d04:
	cp	<<1>>%3C04,#%0100
	jp	ge,L5e74
	inc	<<1>>%0F8E,#1
	ld	r2,|_stkseg+8|(r15)
	ldl	rr4,#%01001428
	ld	r6,#%0003
	cpir	r2,@rr4,r6,eq
	jp	ne,L5e50
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%146E(r6)
	jp	@rr4
	ld	r3,|_stkseg+0|(r15)
	sla	r3,#3
	ldl	rr4,#%0200F336
	add	r5,r3
	ld	r3,|_stkseg+2|(r15)
	add	r3,#%04B0
	ld	@rr4,r3
	ld	r3,|_stkseg+0|(r15)
	sla	r3,#3
	ld	r4,<<2>>%F332(r3)
	cp	r4,#%0000
	jr	ge,L5db8
	ld	r3,|_stkseg+0|(r15)
	sla	r3,#3
	ldl	rr6,#%0200F332
	add	r7,r3
	ld	r3,|_stkseg+2|(r15)
	ld	@rr6,r3
	jr	L5e50
L5d84:
	ldl	rr4,#%01001C02
	ldl	<<1>>%3C12,rr4
	ldl	rr4,#%01001C02
	ldl	<<2>>%F32E,rr4
	inc	<<1>>%3C04,#1
	ldl	rr2,<<2>>%F32E
	ldb	@rr2,#%FF
	ldl	rr2,<<2>>%F32E
	jp	L5ede
L5db8:
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	add	r3,#%0030
	jr	L5e4a
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%0000
	jr	ge,L5e04
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F334
	jr	L5dfa
L5dec:
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
L5dfa:
	add	r5,r2
	ld	r2,|_stkseg+2|(r15)
	ld	@rr4,r2
	jr	L5e50
L5e04:
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	add	r3,#%0032
	jr	L5e4a
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%0000
	jr	lt,L5dec
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%04AF
	jr	gt,L5dec
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	add	r3,#%0034
L5e4a:
	ld	r4,|_stkseg+2|(r15)
	ld	@rr2,r4
L5e50:
	ldl	rr2,<<2>>%F32E
	testl	rr2
	jr	eq,L5d84
	ld	r7,<<1>>%3C04
	inc	<<1>>%3C04,#1
	call	L719e
	ldl	rr10,rr2
	ldb	@rr10,#%FF
	jr	L5edc
L5e74:
	ldk	r7,#2
	ldl	rr4,#%0100142E
	call	L3408
L5e82:
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jr	ne,L5eaa
	ld	r2,|_stkseg+0|(r15)
	sla	r2,#3
	ldl	rr4,#%0200F336
	jp	L5bfa
L5eaa:
	ld	r7,|_stkseg+10|(r15)
	call	L6c6
	ldl	rr8,rr2
	ld	r4,rr2(#%0034)
	ld	|_stkseg+12|(r15),r4
	add	r3,#%0034
	ld	r4,|_stkseg+2|(r15)
	ld	@rr2,r4
	ld	r7,|_stkseg+2|(r15)
	call	L6c6
	add	r3,#%0034
L5ed6:
	ld	r4,|_stkseg+12|(r15)
	ld	@rr2,r4
L5edc:
	ldl	rr2,rr10
L5ede:
	ldm	r8,_stkseg+18(r15),#6
	add	r15,#%001E
	ret
L5eec:
	sub	r15,#%001A
	ldm	_stkseg+14(r15),r8,#6
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr12,rr4
	ldl	rr10,rr6
	test	r3
	jr	eq,L5ffa
	ldk	r8,#0
L5f06:
	cp	r8,#%0080
	jr	ge,L5fc4
	ld	r2,r8
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%FFFF
	jr	ne,L5ff6
	ld	r2,r8
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jr	ne,L5ff6
	ld	r2,r8
	sla	r2,#3
	ld	r3,<<2>>%F336(r2)
	cp	r3,#%FFFF
	jr	ne,L5ff6
	ld	r2,r8
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%FFFF
	jr	ne,L5ff6
	ldl	rr2,rr10
	inc	r3,#16
	ld	@rr2,r8
	ld	r2,r8
	ldb	@rr10,rl2
	ldb	rl3,|_stkseg+1|(r15)
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001472
	ld	r3,#%0003
	cpir	r2,@rr4,r3,eq
	jr	ne,L5f9c
	sll	r3,#2
	neg	r3
	ldl	rr4,<<1>>%1492(r3)
	jp	@rr4
	ld	r3,r8
	sla	r3,#3
	ldl	rr4,#%0200F332
L5f90:
	add	r5,r3
	ldl	rr6,_stkseg+30(r15)
	ld	r3,@rr6
	ld	@rr4,r3
L5f9c:
	cp	r8,<<1>>%3E82
	jr	le,L5faa
	ld	<<1>>%3E82,r8
L5faa:
	ld	r2,r8
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%0064
	ldl	rr2,_stkseg+30(r15)
	ld	@rr2,r8
L5fc4:
	cp	r8,#%0080
	jr	ne,L6072
	ldk	r7,#2
	ldl	rr4,#%01001478
	call	L3408
	jr	L6072
	ld	r3,r8
	sla	r3,#3
	ldl	rr4,#%0200F334
	jr	L5f90
	ld	r3,r8
	sla	r3,#3
	ldl	rr4,#%0200F336
	jr	L5f90
L5ff6:
	inc	r8,#1
	jr	L5f06
L5ffa:
	cpb	|_stkseg+1|(r15),#%72
	jr	ne,L6006
	jr	L6060
L6004:
	inc	@rr12,#1
L6006:
	ld	r2,@rr12
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%03E8
	jr	eq,L6004
	ld	r2,@rr12
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0064
	jr	eq,L6004
	ld	r2,@rr12
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%0BB8
	jr	eq,L6004
	ld	r2,@rr12
	sla	r2,#3
	ld	r3,<<2>>%F338(r2)
	cp	r3,#%FFFF
	jr	ne,L6060
	ld	r2,@rr12
	sla	r2,#3
	ldl	rr4,#%0200F338
	add	r5,r2
	ld	@rr4,#%0032
L6060:
	ldl	rr2,rr10
	inc	r3,#16
	ld	r4,@rr12
	ld	@rr2,r4
	ld	r2,@rr12
	ldb	@rr10,rl2
	ld	r2,@rr12
	inc	r2,#1
	ld	@rr12,r2
L6072:
	ldl	rr2,<<2>>%F32E
	testl	rr2
	jr	ne,L608a
	ldl	<<1>>%3C12,rr10
	ldl	<<2>>%F32E,rr10
	jr	L60aa
L608a:
	ldl	rr2,<<1>>%3C12
	add	r3,#%0012
	ldl	@rr2,rr10
	ldl	rr2,rr10
	add	r3,#%0016
	ldl	rr4,<<1>>%3C12
	ldl	@rr2,rr4
	ldl	<<1>>%3C12,rr10
L60aa:
	ld	r2,rr10(#%0004)
	ldl	rr4,rr10
	inc	r5,#6
	add	r2,@rr4
	ldl	rr4,rr10
	inc	r5,#8
	add	r2,@rr4
	ld	r9,r2
	cp	r2,#%FF00
	jr	ule,L60c4
	inc	r9,#2
L60c4:
	ld	r2,r9
	and	r2,#%00FF
	ld	r9,r2
	test	<<2>>%E4A2
	jr	ne,L60e0
	test	r2
	jr	eq,L60e0
	neg	r2
	add	r2,#%0100
	jr	L60e2
L60e0:
	sub	r2,r2
L60e2:
	ldm	r8,_stkseg+14(r15),#6
	add	r15,#%001A
	ret
L60f0:
	dec	r15,#8
	ldm	|_stkseg+0|(r15),r5,#3
	cpb	rl5,#%74
	jr	ne,L6114
	ldl	rr2,rr6
	ld	r4,rr2(#%0004)
	sub	r4,<<1>>%3E7A
	ld	|_stkseg+6|(r15),r4
	test	r4
	jr	ne,L612a
	jr	L6130
L6114:
	ldl	rr2,|_stkseg+2|(r15)
	ld	r4,rr2(#%0006)
	sub	r4,<<1>>%3E7A
	ld	|_stkseg+6|(r15),r4
	test	r4
	jr	eq,L6130
L612a:
	ld	r7,|_stkseg+6|(r15)
	calr	L64d0
L6130:
	inc	r15,#8
	ret
L6134:
	dec	r15,#6
	ld	_stkseg+4(r15),r13
	ldl	rr6,#%0200E6E2
	call	L3ac6
	ldl	rr6,#%0200E8EA
	call	L3ac6
	test	<<2>>%E4A2
	jr	eq,L6174
	ldl	rr6,#%0200EF02
	call	L3ac6
	ldl	rr6,#%0200EAF2
	call	L3ac6
L6174:
	ldk	r13,#0
	jr	L61fc
L6178:
	clr	<<1>>%3E7A
	ld	r2,r13
	sla	r2,#3
	ld	r3,<<2>>%F332(r2)
	cp	r3,#%FFFF
	jr	le,L61bc
	ld	r2,r13
	sla	r2,#3
	ld	r7,<<2>>%F332(r2)
	call	L6c6
	ldl	rr4,rr2(#%0016)
	ldl	|_stkseg+0|(r15),rr4
	ld	r2,r13
	sla	r2,#3
	ld	r7,<<2>>%F332(r2)
	ld	r6,#%0074
	calr	L620e
L61bc:
	ld	r2,r13
	sla	r2,#3
	ld	r3,<<2>>%F334(r2)
	cp	r3,#%FFFF
	jr	le,L61fa
	ld	r2,r13
	sla	r2,#3
	ld	r7,<<2>>%F334(r2)
	call	L6c6
	ldl	rr4,rr2(#%001A)
	ldl	|_stkseg+0|(r15),rr4
	ld	r2,r13
	sla	r2,#3
	ld	r7,<<2>>%F334(r2)
	ld	r6,#%0064
	calr	L620e
L61fa:
	inc	r13,#1
L61fc:
	cp	r13,<<1>>%3E82
	jr	le,L6178
	ld	r13,_stkseg+4(r15)
	inc	r15,#6
	ret
L620e:
	sub	r15,#%0226
	ldm	_stkseg+538(r15),r8,#6
	ldm	|_stkseg+0|(r15),r4,#3
	ld	r13,r7
	ldl	|_stkseg+8|(r15),rr4
L6226:
	cp	r13,#%FFFF
	jp	eq,L63f2
	ldl	rr2,|_stkseg+0|(r15)
	cpl	rr2,|_stkseg+8|(r15)
	jp	ne,L63f2
	cpb	|_stkseg+5|(r15),#%74
	jr	ne,L627e
	ld	r7,r13
	call	L6c6
	ldl	_stkseg+532(r15),rr2
	ld	r4,rr2(#%0004)
	ld	r11,r4
	ldl	rr4,rr2(#%0028)
	ldl	rr8,rr4
	ld	r4,rr2(#%0030)
	ld	r13,r4
	ld	r7,r4
	call	L6c6
	ldl	rr4,rr2(#%0016)
	ldl	|_stkseg+8|(r15),rr4
	ld	r7,<<2>>%E6E2
	jr	L62b4
L627e:
	ld	r7,r13
	call	L6c6
	ldl	_stkseg+532(r15),rr2
	ld	r4,rr2(#%0006)
	ld	r11,r4
	ldl	rr4,rr2(#%002C)
	ldl	rr8,rr4
	ld	r4,rr2(#%0032)
	ld	r13,r4
	ld	r7,r4
	call	L6c6
	ldl	rr4,rr2(#%001A)
	ldl	|_stkseg+8|(r15),rr4
	ld	r7,<<2>>%E8EA
L62b4:
	ldl	rr4,rr8
	sub	r3,r3
	call	_lseek
	test	<<2>>%E4A2
	jr	eq,L62e6
	ld	r7,<<2>>%EF02
	ldl	rr4,rr8
	sub	r3,r3
	call	_lseek
	ld	r7,<<2>>%EAF2
	ldl	rr4,rr8
	sub	r3,r3
	call	_lseek
L62e6:
	ld	r2,<<1>>%3E7A
	add	r2,r11
	ld	<<1>>%3E7A,r2
L62f4:
	test	r11
	jr	eq,L6226
	cp	r11,#%0200
	jr	ule,L630c
	ld	r2,r11
	sub	r2,#%0200
	ld	r11,r2
	ld	r12,#%0200
	jr	L6310
L630c:
	ld	r12,r11
	ldk	r11,#0
L6310:
	cpb	|_stkseg+5|(r15),#%74
	jr	ne,L633e
	ldl	rr4,#%7F000012
	add	r5,r15
	ld	r7,<<2>>%E6E2
	ld	r3,r12
	call	_read
	cp	r2,#%0001
	jr	ge,L6368
	ldk	r7,#2
	ldl	rr4,#%01001496
	jr	L6362
L633e:
	ldl	rr4,#%7F000012
	add	r5,r15
	ld	r7,<<2>>%E8EA
	ld	r3,r12
	call	_read
	cp	r2,#%0001
	jr	ge,L6368
	ldk	r7,#2
	ldl	rr4,#%010014B6
L6362:
	call	L3408
L6368:
	ldl	rr4,#%7F000012
	add	r5,r15
	ldl	rr6,#%0200E4DA
	ld	r3,r12
	call	L2f04
	test	<<2>>%E4A2
	jr	eq,L62f4
	cpb	|_stkseg+5|(r15),#%74
	jr	ne,L63b2
	ldl	rr4,#%7F000012
	add	r5,r15
	ld	r7,<<2>>%EF02
	ld	r3,r12
	call	_read
	cp	r12,r2
	jr	eq,L63da
	ldk	r7,#2
	ldl	rr4,#%010014D4
	jr	L63d4
L63b2:
	ldl	rr4,#%7F000012
	add	r5,r15
	ld	r7,<<2>>%EAF2
	ld	r3,r12
	call	_read
	cp	r12,r2
	jr	eq,L63da
	ldk	r7,#2
	ldl	rr4,#%010014EA
L63d4:
	call	L3408
L63da:
	ldl	rr4,#%7F000012
	add	r5,r15
	ldl	rr6,#%0200ECFA
	ld	r3,r12
	call	L2f04
	jr	L62f4
L63f2:
	ldl	rr6,|_stkseg+0|(r15)
	ldb	rl5,|_stkseg+5|(r15)
	extsb	r5
	calr	L60f0
	cp	r13,#%FFFF
	jr	eq,L6418
	ldl	rr2,|_stkseg+8|(r15)
	ldl	|_stkseg+0|(r15),rr2
	clr	<<1>>%3E7A
	jp	L6226
L6418:
	clr	<<1>>%3E7A
	sub	r2,r2
	ldm	r8,_stkseg+538(r15),#6
	add	r15,#%0226
	ret
L642e:
	sub	r15,#%0014
	ldm	_stkseg+12(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,rr6
	ldl	rr0,rr2(#%0016)
	ldl	rr12,rr0
	ldl	rr0,rr4
	add	r1,#%0012
	ldl	rr2,rr0
	ldl	rr10,@rr2
	cpl	rr6,<<2>>%F32E
	jr	ne,L6462
	ldl	<<2>>%F32E,rr4
	jr	L646e
L6462:
	ldl	rr2,rr12
	add	r3,#%0012
	ldl	rr4,|_stkseg+0|(r15)
	ldl	@rr2,rr4
L646e:
	ldl	rr2,|_stkseg+0|(r15)
	cpl	rr2,<<1>>%3C12
	jr	ne,L6486
	ldl	rr4,|_stkseg+4|(r15)
	ldl	<<1>>%3C12,rr4
	jr	L6492
L6486:
	ldl	rr2,rr10
	add	r3,#%0016
	ldl	rr4,|_stkseg+4|(r15)
	ldl	@rr2,rr4
L6492:
	ldl	rr2,|_stkseg+4|(r15)
	add	r3,#%0012
	ldl	@rr2,rr10
	ldl	rr2,|_stkseg+4|(r15)
	add	r3,#%0016
	ldl	rr4,|_stkseg+0|(r15)
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+0|(r15)
	add	r3,#%0012
	ldl	rr4,|_stkseg+4|(r15)
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+0|(r15)
	add	r3,#%0016
	ldl	@rr2,rr12
	ldm	r10,_stkseg+12(r15),#4
	add	r15,#%0014
	ret
L64d0:
	dec	r15,#6
	ld	_stkseg+4(r15),r13
	ld	r13,r7
	test	<<2>>%E4A2
	jr	eq,L64e6
	sub	r2,r2
	jr	L6518
L64e6:
	cp	r13,#%0000
	jr	le,L6518
	sub	r7,r7
	ldl	rr4,#%0200E4DA
	call	L3a88
	test	<<2>>%E4A2
	jr	eq,L6510
	sub	r7,r7
	ldl	rr4,#%0200ECFA
	call	L3a88
L6510:
	ld	r2,r13
	dec	r2,#2
	ld	r13,r2
	jr	L64e6
L6518:
	ld	r13,_stkseg+4(r15)
	inc	r15,#6
	ret
L6522:
	dec	r15,#16
	ldm	_stkseg+8(r15),r10,#4
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	clrb	|_stkseg+5|(r15)
	ldb	rl2,@rr12
	extsb	r2
	and	r2,#%007F
	ld	r11,r2
	ldb	rl2,@rr12
	extsb	r2
	cp	r11,r2
	jr	eq,L6584
	inc	r13,#1
	dec	r11,#1
	test	<<1>>%04EA
	jr	ne,L6574
	ldk	r11,#8
	test	<<1>>%1532
	jr	ne,L6586
	inc	<<1>>%1532,#1
	sub	r7,r7
	ldl	rr4,#%01001534
	call	L3408
	jr	L6586
L6574:
	cp	r11,#%002C
	jr	le,L6586
	incb	|_stkseg+5|(r15),#1
	ld	r11,#%0028
	jr	L6586
L6584:
	ldk	r11,#8
L6586:
	ldk	r10,#0
	jr	L659c
L658a:
	testb	@rr12
	jr	eq,L65a4
	ldb	rl7,@rr12
	extsb	r7
	call	_putchar
	inc	r10,#1
	inc	r13,#1
L659c:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L658a
L65a4:
	testb	|_stkseg+5|(r15)
	jr	eq,L65b8
	ldl	rr6,#%01001568
	call	_printf
	inc	r10,#3
L65b8:
	ld	r2,r10
	ldm	r10,_stkseg+8(r15),#4
	inc	r15,#16
	ret
L65c6:
	dec	r15,#10
	ld	|_stkseg+0|(r15),r7
	ldl	rr2,#%7F000002
	add	r3,r15
	ldl	rr4,rr2
	ldb	rl6,|_stkseg+1|(r15)
	ldb	@rr4,rl6
	ldl	rr2,rr4
	inc	r3,#1
	ldb	rl6,|_stkseg+0|(r15)
	ldb	@rr2,rl6
	ld	r2,@rr4
	inc	r15,#10
	ret
L65ec:
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,|_stkseg+0|(r15)
	test	@rr12
	jr	eq,L6608
	ld	r7,@rr12
	calr	L65c6
	ld	@rr12,r2
L6608:
	inc	r13,#2
	test	@rr12
	jr	eq,L6614
	ld	r7,@rr12
	calr	L65c6
	ld	@rr12,r2
L6614:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
L6620:
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,rr6
	ld	r2,rr12(#%0004)
	test	r2
	jr	eq,L6642
	ld	r7,r2
	calr	L65c6
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
L6642:
	ld	r2,rr12(#%0006)
	test	r2
	jr	eq,L6654
	ld	r7,r2
	calr	L65c6
	ldl	rr4,rr12
	inc	r5,#6
	ld	@rr4,r2
L6654:
	ld	r2,rr12(#%0008)
	test	r2
	jr	eq,L6666
	ld	r7,r2
	calr	L65c6
	ldl	rr4,rr12
	inc	r5,#8
	ld	@rr4,r2
L6666:
	ld	r2,rr12(#%000A)
	test	r2
	jr	eq,L6678
	ld	r7,r2
	calr	L65c6
	ldl	rr4,rr12
	inc	r5,#10
	ld	@rr4,r2
L6678:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
L6684:
	sub	r15,#%0026
	ldm	_stkseg+30(r15),r10,#4
	ldk	r12,#0
	test	<<2>>%E4B2
	jr	eq,L678c
	ldl	rr6,#%7F000006
	add	r7,r15
	ldk	r5,#14
	call	L2e76
	ldl	rr2,|_stkseg+6|(r15)
	ldl	<<2>>%046A,rr2
	ldb	rl4,|_stkseg+10|(r15)
	ldb	<<2>>%046E,rl4
	ldb	rl5,|_stkseg+11|(r15)
	extsb	r5
	ld	<<2>>%0470,r5
	ldk	r13,#0
L66cc:
	ldl	rr2,#%7F00000C
	add	r3,r15
	ldb	rl4,rr2(r13)
	ldb	<<2>>%0472(r13),rl4
	inc	r13,#1
	cp	r13,#%0008
	jr	lt,L66cc
	test	<<2>>%E4B6
	jr	eq,L66fa
	ldl	rr6,#%0200046A
	call	L65ec
L66fa:
	ldb	rl2,<<2>>%0472
	extsb	r2
	bit	r2,#7
	jr	eq,L67da
	ldb	rl2,<<2>>%0472
	extsb	r2
	and	r2,#%007F
	ldb	|_stkseg+1|(r15),rl2
	ldl	rr2,#%7F000006
	add	r3,r15
	ldl	rr10,rr2
	ldl	rr2,#%7F00000C
	add	r3,r15
	ldb	rl4,|_stkseg+1|(r15)
	extsb	r4
	add	r3,r4
	ldl	|_stkseg+2|(r15),rr2
	ldl	rr2,#%0200047A
	ldl	|_stkseg+24|(r15),rr2
	ldl	rr2,#%0200047A
	ldl	|_stkseg+20|(r15),rr2
	jr	L675a
L674a:
	ldl	rr6,|_stkseg+20|(r15)
	ldk	r5,#14
	call	L2e76
	inc	|_stkseg+22|(r15),#14
L675a:
	inc	r11,#14
	cpl	rr10,|_stkseg+2|(r15)
	jr	c,L674a
	ld	r2,|_stkseg+22|(r15)
	sub	r2,|_stkseg+26|(r15)
	ld	r5,r2
	exts	rr4
	div	rr4,#%0010
	ld	r12,r5
	ld	r2,|_stkseg+22|(r15)
	sub	r2,|_stkseg+26|(r15)
	ld	r5,r2
	exts	rr4
	div	rr4,#%0010
	test	r4
	jr	eq,L67da
	inc	r12,#1
	jr	L67da
L678c:
	ldl	rr6,#%010015CC
	ldk	r5,#12
	call	L2e76
	test	<<2>>%E4B6
	jr	eq,L67c6
	ld	r7,<<1>>%15D4
	call	L65c6
	ld	<<1>>%15D4,r2
	ld	r7,<<1>>%15D6
	call	L65c6
	ld	<<1>>%15D6,r2
L67c6:
	ldl	rr6,#%010015CC
	ldl	rr4,#%0200046A
	ld	r3,<<2>>%E4B0
	calr	L6800
L67da:
	ldl	rr2,<<2>>%046A
	and	r2,#%FF00
	ldl	<<2>>%046A,rr2
	resb	<<2>>%046E,#6
	ld	r2,r12
	ldm	r10,_stkseg+30(r15),#4
	add	r15,#%0026
	ret
L6800:
	dec	r15,#12
	ldm	|_stkseg+0|(r15),r3,#5
	ldl	rr0,rr6
	inc	r1,#10
	ldl	rr6,rr0
	ld	r2,@rr6
	ldk	r6,#0
	ld	r7,r2
	ldl	rr0,|_stkseg+2|(r15)
	ldl	rr4,rr0
	ldl	@rr4,rr6
	ldl	rr4,|_stkseg+6|(r15)
	ld	r2,rr4(#%0008)
	ldl	rr0,|_stkseg+2|(r15)
	inc	r1,#4
	ldl	rr6,rr0
	ldb	@rr6,rl2
	ldl	rr6,|_stkseg+2|(r15)
	inc	r7,#6
	clr	@rr6
	test	r3
	jr	eq,L6862
	ldl	rr6,|_stkseg+2|(r15)
	ldb	rl2,rr6(#%0004)
	extsb	r2
	and	r2,#%001F
	ldb	|_stkseg+11|(r15),rl2
	cpb	rl2,#%03
	jr	eq,L6858
	cpb	rl2,#%04
	jr	ne,L6862
L6858:
	ldl	rr2,|_stkseg+2|(r15)
	inc	r3,#6
	ld	@rr2,#%0001
L6862:
	inc	r15,#12
	ret
	dec	r15,#8
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,rr4
	inc	r3,#10
	ldl	rr0,|_stkseg+4|(r15)
	ldl	rr6,rr0
	ldl	rr0,@rr6
	ld	@rr2,r1
	ldl	rr2,|_stkseg+4|(r15)
	ldb	rl6,rr2(#%0004)
	extsb	r6
	ldl	rr0,rr4
	inc	r1,#8
	ldl	rr2,rr0
	ld	@rr2,r6
	ldl	rr6,|_stkseg+4|(r15)
	inc	r7,#8
	ldl	rr4,|_stkseg+0|(r15)
	call	L178
	inc	r15,#8
	ret
L68a2:
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ldm	|_stkseg+0|(r15),r5,#3
	ldl	rr12,rr6
	testl	rr6
	jp	eq,L69fc
	ldb	rl2,rr12(#%0004)
	cpb	rl2,#%20
	jr	ne,L68cc
	ldl	rr2,@rr12
	testl	rr2
	jr	eq,L68fe
L68cc:
	ldl	rr6,rr12
	inc	r7,#8
	call	L6522
	cp	r2,#%0008
	jr	ge,L68e4
	ldk	r7,#9
	call	_putchar
L68e4:
	ld	r7,#%0020
	call	_putchar
	ldk	r7,#1
	ldl	rr4,#%010015DC
	call	L3408
	jr	L69fc
L68fe:
	ldl	rr2,rr12
	inc	r3,#4
	ldb	rl4,|_stkseg+1|(r15)
	ldb	@rr2,rl4
	subl	rr2,rr2
	ldl	@rr12,rr2
	ldl	rr2,rr12
	inc	r3,#6
	clr	@rr2
	cpb	|_stkseg+1|(r15),#%22
	jr	ne,L69a4
	ldl	rr2,<<1>>%3C0A
	testl	rr2
	jr	eq,L69fc
	ld	r4,rr2(#%0004)
	ldk	r6,#0
	ld	r7,r4
	ldb	rl4,@rr2
	ldb	rl1,rl4
	extsb	r1
	exts	rr0
	slal	rr0,#24
	addl	rr6,rr0
	jr	L6992
L693c:
	ldl	rr2,<<1>>%3C06
	testl	rr2
	jr	eq,L69fc
	ld	r4,rr2(#%0004)
	ldl	rr6,rr2
	inc	r7,#6
	add	r4,@rr6
	ldk	r6,#0
	ld	r7,r4
	ldl	@rr12,rr6
	ldl	rr2,<<1>>%3C06
	jr	L6984
L695e:
	ldl	rr2,<<1>>%3C0E
	testl	rr2
	jr	eq,L69fc
	ld	r4,rr2(#%0004)
	ldl	rr6,rr2
	inc	r7,#6
	add	r4,@rr6
	ldl	rr6,rr2
	inc	r7,#8
	add	r4,@rr6
	ldk	r6,#0
	ld	r7,r4
	ldl	@rr12,rr6
	ldl	rr2,<<1>>%3C0E
L6984:
	ldb	rl4,@rr2
	ldb	rl7,rl4
	extsb	r7
	exts	rr6
	slal	rr6,#24
	addl	rr6,@rr12
L6992:
	ldl	@rr12,rr6
	ldl	rr4,rr2
	ld	r6,rr4(#%0010)
	ldl	rr0,rr12
	inc	r1,#6
	ldl	rr4,rr0
	ld	@rr4,r6
	jr	L69fc
L69a4:
	cpb	|_stkseg+1|(r15),#%23
	jr	eq,L693c
	cpb	|_stkseg+1|(r15),#%24
	jr	eq,L695e
	cpb	|_stkseg+1|(r15),#%21
	jr	ne,L69fc
	subl	rr2,rr2
	ldl	@rr12,rr2
	cp	<<1>>%0474,#%FFFF
	jr	eq,L69d4
	cp	<<1>>%0474,#%FFFD
	jr	ne,L69e0
L69d4:
	ldl	rr2,rr12
	inc	r3,#6
	ld	r4,<<1>>%3E82
	jr	L69ea
L69e0:
	ldl	rr2,rr12
	inc	r3,#6
	ld	r4,<<1>>%0474
L69ea:
	ld	@rr2,r4
	ld	r2,rr12(#%0006)
	ld	r5,r2
	exts	rr4
	slal	rr4,#24
	addl	rr4,@rr12
	ldl	@rr12,rr4
L69fc:
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
L6a08:
	dec	r15,#4
	ldl	|_stkseg+0|(r15),rr6
	test	<<2>>%E4A4
	jr	ne,L6a54
	test	<<2>>%E4AA
	jr	ne,L6a54
	ldl	rr6,|_stkseg+0|(r15)
	ldl	rr4,#%02000472
	call	L56
	ldb	<<2>>%046E,#%1F
	subl	rr2,rr2
	ldl	<<2>>%046A,rr2
	clr	<<2>>%0470
	calr	L6c6a
	ldl	rr6,#%0200F10A
	ldl	rr4,#%0200046A
	calr	L6e1a
L6a54:
	inc	r15,#4
	ret
L6a58:
	sub	r15,#%001C
	ldm	_stkseg+16(r15),r8,#4
	clr	|_stkseg+0|(r15)
	ldl	rr10,#%02000472
	ldl	|_stkseg+4|(r15),rr10
	ldl	rr2,#%0200047A
	ldl	|_stkseg+8|(r15),rr2
	ldb	rl2,@rr10
	extsb	r2
	bit	r2,#7
	jr	eq,L6aaa
	inc	|_stkseg+6|(r15),#1
	test	<<1>>%04EA
	jr	ne,L6a96
	inc	|_stkseg+10|(r15),#1
	jr	L6aaa
L6a96:
	ldb	rl2,@rr10
	extsb	r2
	and	r2,#%007F
	ldl	rr4,#%02000472
	add	r5,r2
	ldl	|_stkseg+8|(r15),rr4
L6aaa:
	ldl	rr10,|_stkseg+4|(r15)
	jr	L6ac2
L6ab0:
	ld	r2,|_stkseg+0|(r15)
	add	r2,r2
	ldb	rl3,@rr10
	extsb	r3
	add	r2,r3
	ld	|_stkseg+0|(r15),r2
	inc	r11,#1
L6ac2:
	cpl	rr10,|_stkseg+8|(r15)
	jr	c,L6ab0
	ld	r2,|_stkseg+0|(r15)
	and	r2,#%7FFF
	ld	r5,r2
	exts	rr4
	div	rr4,#%0FFA
	inc	r4,#2
	sla	r4,#2
	ldl	rr2,#%020044E8
	add	r3,r4
	ldl	rr6,rr2
	jr	L6b44
L6aea:
	ldl	rr2,@rr6
	inc	r3,#8
	ldl	rr8,rr2
	clr	|_stkseg+2|(r15)
	test	<<1>>%04EA
	jr	eq,L6b04
	ldl	rr10,#%02000472
	jr	L6b12
L6b04:
	ldl	rr10,|_stkseg+4|(r15)
	ldb	rl2,@rr8
	extsb	r2
	bit	r2,#7
	jr	eq,L6b12
	inc	r9,#1
L6b12:
	cpl	rr10,|_stkseg+8|(r15)
	jr	nc,L6b2e
	ldl	rr2,rr10
	inc	r11,#1
	ldb	rl4,@rr2
	ldl	rr2,rr8
	inc	r9,#1
	ldb	rl5,@rr2
	cpb	rl4,rl5
	jr	eq,L6b12
	ld	|_stkseg+2|(r15),#%0001
L6b2e:
	test	|_stkseg+2|(r15)
	jr	eq,L6b4a
	inc	r7,#4
	cpl	rr6,#%020084D8
	jr	c,L6b44
	ldl	rr6,#%020044E8
L6b44:
	ldl	rr2,@rr6
	testl	rr2
	jr	ne,L6aea
L6b4a:
	ldl	rr2,rr6
	ldm	r8,_stkseg+16(r15),#4
	add	r15,#%001C
	ret
L6b5a:
	dec	r15,#8
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,rr6
	ldl	rr4,#%02000472
	call	Lee
	ldb	<<2>>%046E,#%20
	subl	rr2,rr2
	ldl	<<2>>%046A,rr2
	calr	L6a58
	inc	r15,#8
	ret
L6b84:
	dec	r15,#8
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,rr6
	ldl	rr4,#%02000472
	call	L56
	ldb	<<2>>%046E,#%20
	subl	rr2,rr2
	ldl	<<2>>%046A,rr2
	calr	L6a58
	inc	r15,#8
	ret
L6bae:
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,rr6
	ldl	rr4,@rr2
	testl	rr4
	jr	ne,L6c50
	cp	<<2>>%04FA,#%0FFA
	jr	lt,L6bdc
	ldk	r7,#2
	ldl	rr4,#%010015F0
	call	L3408
L6bdc:
	ld	r2,<<2>>%04FA
	sla	r2,#2
	ldl	rr4,#%02000500
	add	r5,r2
	ldl	rr2,|_stkseg+0|(r15)
	ldl	@rr4,rr2
	ld	r7,<<2>>%04FA
	inc	<<2>>%04FA,#1
	calr	L6fd0
	ldl	rr12,rr2
	ldl	<<2>>%04FC,rr2
	ldl	rr4,|_stkseg+0|(r15)
	ldl	@rr4,rr2
	ldl	rr4,rr12
	inc	r5,#8
	ldl	rr6,#%02000472
	call	L56
	add	r2,<<2>>%04FA
	ld	<<2>>%04FA,r2
	ldl	rr2,rr12
	inc	r3,#4
	ldb	rl4,<<2>>%046E
	ldb	@rr2,rl4
	ldl	rr2,<<2>>%046A
	ldl	@rr12,rr2
	ldl	rr2,rr12
	inc	r3,#6
	ld	r4,<<2>>%0470
	ld	@rr2,r4
	ldk	r2,#1
	jr	L6c5e
L6c50:
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr4,@rr2
	ldl	<<2>>%04FC,rr4
	sub	r2,r2
L6c5e:
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
L6c6a:
	dec	r15,#10
	ldm	_stkseg+2(r15),r10,#4
	ld	r7,<<1>>%3C02
	add	r7,<<2>>%0470
	ld	|_stkseg+0|(r15),r7
	call	L6c6
	ldl	rr10,rr2
	ldl	rr12,#%0200046A
	ldb	rl3,<<2>>%046E
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001606
	ld	r6,#%0008
	cpir	r2,@rr4,r6,eq
	jr	ne,L6d88
	sll	r6,#2
	neg	r6
	ldl	rr4,<<1>>%1632(r6)
	jp	@rr4
L6cba:
	ldl	rr4,rr10(#%0016)
	ldb	rl3,@rr4
	extsb	r3
	ldb	rh3,rl3
	clrb	rl3
	ld	@rr12,r3
	inc	r13,#2
	ld	r3,rr10(#%0010)
	ldl	rr4,rr10
	add	r5,#%0022
	add	r3,@rr4
	ldl	rr4,rr10(#%0016)
	add	r5,#%001A
	add	r3,@rr4
	add	r3,@rr12
	ld	@rr12,r3
	ldl	rr4,rr10(#%0016)
L6ce8:
	ld	r3,rr4(#%0010)
	ld	<<2>>%0470,r3
	jr	L6d9c
	ldl	rr4,rr10(#%001A)
	ldb	rl3,@rr4
	extsb	r3
	ldb	rh3,rl3
	clrb	rl3
	ld	@rr12,r3
	inc	r13,#2
	ld	r3,rr10(#%0012)
	ldl	rr4,rr10
	add	r5,#%0024
	add	r3,@rr4
	ldl	rr4,rr10(#%001A)
	add	r5,#%001C
	add	r3,@rr4
	add	r3,@rr12
	ld	@rr12,r3
	ldl	rr4,rr10(#%001A)
	jr	L6ce8
	ldl	rr4,rr10(#%001E)
	ldb	rl3,@rr4
	extsb	r3
	ldb	rh3,rl3
	clrb	rl3
	ld	@rr12,r3
	inc	r13,#2
	ld	r3,rr10(#%0014)
	ldl	rr4,rr10
	add	r5,#%0026
	add	r3,@rr4
	ldl	rr4,rr10(#%001E)
	add	r5,#%001E
	add	r3,@rr4
	add	r3,@rr12
	ld	@rr12,r3
	ldl	rr4,rr10(#%001E)
	jr	L6ce8
L6d54:
	ld	r3,|_stkseg+0|(r15)
	cp	r3,<<2>>%F32C
	jr	ge,L6d78
	ld	r7,r3
	call	L6c6
	ldl	rr10,rr2
	ld	r2,rr10(#%0004)
	test	r2
	jr	ne,L6cba
	inc	|_stkseg+0|(r15),#1
	jr	L6d54
L6d78:
	clr	<<2>>%0470
	subl	rr2,rr2
	ldl	<<2>>%046A,rr2
	jr	L6d9c
L6d88:
	ldb	rl2,<<2>>%046E
	extsb	r2
	bit	r2,#5
	jr	eq,L6d9c
	ldb	<<2>>%046E,#%21
L6d9c:
	ldm	r10,_stkseg+2(r15),#4
	inc	r15,#10
	ret
L6da8:
	ldb	rl3,<<2>>%046E
	extsb	r3
	ld	r2,r3
	ldl	rr4,#%01001636
	ld	r3,#%0007
	cpir	r2,@rr4,r3,eq
	jr	ne,L6e04
	sll	r3,#2
	neg	r3
	ldl	rr4,<<1>>%165C(r3)
	jp	@rr4
	ldl	rr4,<<2>>%046A
	add	r5,<<2>>%E4D0
L6ddc:
	ldk	r6,#0
	ld	r7,r5
	ldl	<<2>>%046A,rr6
	jr	L6e18
	ldl	rr4,<<2>>%046A
	add	r5,<<2>>%E4D2
	jr	L6ddc
	ldl	rr4,<<2>>%046A
	add	r5,<<2>>%E4D4
	jr	L6ddc
L6e04:
	ldb	rl2,<<2>>%046E
	extsb	r2
	bit	r2,#5
	jr	eq,L6e18
	ldb	<<2>>%046E,#%21
L6e18:
	ret
L6e1a:
	sub	r15,#%0022
	ldm	_stkseg+26(r15),r10,#4
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,|_stkseg+0|(r15)
	ldl	rr0,@rr2
	ldl	|_stkseg+10|(r15),rr0
	ldl	rr0,rr4
	inc	r1,#4
	ldl	rr2,rr0
	ldb	rl0,@rr2
	ldb	|_stkseg+14|(r15),rl0
	ldl	rr2,rr4
	ld	r1,rr2(#%0006)
	ldb	|_stkseg+15|(r15),rl1
	ldk	r10,#0
L6e4e:
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#8
	ldb	rl4,rr2(r10)
	ldl	rr2,#%7F000010
	add	r3,r15
	add	r3,r10
	ldb	@rr2,rl4
	inc	r10,#1
	cp	r10,#%0008
	jr	lt,L6e4e
	test	<<2>>%E4B6
	jr	eq,L6e82
	ldl	rr6,#%7F00000A
	add	r7,r15
	call	L65ec
L6e82:
	ldl	rr4,#%7F00000A
	add	r5,r15
	ldl	rr6,|_stkseg+4|(r15)
	ldk	r3,#14
	call	L2f04
	inc	<<1>>%04F0,#1
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl4,rr2(#%0008)
	extsb	r4
	bit	r4,#7
	jr	eq,L6eee
	inc	r3,#16
	ldl	rr12,rr2
	ldl	rr2,|_stkseg+0|(r15)
	ldb	rl4,rr2(#%0008)
	extsb	r4
	and	r4,#%007F
	dec	r4,#8
	ldb	|_stkseg+9|(r15),rl4
	ldk	r11,#1
	jr	L6ee2
L6ec6:
	ldl	rr6,|_stkseg+4|(r15)
	ldl	rr4,rr12
	ldk	r3,#14
	call	L2f04
	inc	<<1>>%04F0,#1
	inc	r13,#14
	inc	r11,#1
	decb	|_stkseg+9|(r15),#14
L6ee2:
	cpb	|_stkseg+9|(r15),#%00
	jr	gt,L6ec6
	ld	r2,r11
	jr	L6ef0
L6eee:
	ldk	r2,#1
L6ef0:
	ldm	r10,_stkseg+26(r15),#4
	add	r15,#%0022
	ret
L6efe:
	dec	r15,#8
	ldm	_stkseg+0(r15),r10,#4
	sub	r7,r7
	calr	L6fd0
	ldl	rr12,rr2
	ld	r7,<<2>>%04FA
	calr	L6fd0
	ldl	rr10,rr2
	jr	L6f36
L6f1a:
	ldl	rr2,rr12
	inc	r3,#4
	setb	@rr2,#6
	ldl	rr6,#%0200E4DA
	ldl	rr4,rr12
	calr	L6e1a
	ldl	rr2,rr12
	inc	r3,#4
	resb	@rr2,#6
	ldl	rr6,rr12
	calr	L705e
	ldl	rr12,rr2
L6f36:
	cpl	rr12,rr10
	jr	c,L6f1a
	ldm	r10,_stkseg+0(r15),#4
	inc	r15,#8
	ret
L6f46:
	sub	r15,#%0012
	ldm	|_stkseg+0|(r15),r5,#3
	ldl	rr2,|_stkseg+2|(r15)
	ldl	|_stkseg+10|(r15),rr2
	ld	r1,r5
	mult	rr0,#%000C
	add	r7,r1
	ldl	|_stkseg+6|(r15),rr6
	jr	L6fc0
L6f66:
	ldl	rr2,|_stkseg+2|(r15)
	cpl	rr2,|_stkseg+10|(r15)
	jr	c,L6fbc
	ldb	rl4,rr2(#%0004)
	extsb	r4
	and	r4,#%0080
	test	r4
	jr	eq,L6f94
	inc	r3,#4
	ldl	rr4,|_stkseg+2|(r15)
	ldb	rl6,rr4(#%0004)
	extsb	r6
	and	r6,#%007F
	add	r3,r6
	ldl	|_stkseg+10|(r15),rr2
L6f94:
	ldl	rr6,|_stkseg+2|(r15)
	inc	r7,#4
	calr	L6b84
	ldl	rr4,@rr2
	ldl	|_stkseg+14|(r15),rr4
	testl	rr4
	jr	eq,L6fbc
	ldl	rr2,rr4
	ldb	rl4,rr2(#%0004)
	cpb	rl4,#%20
	jr	eq,L6fbc
	sub	r7,r7
	ldl	rr4,rr2
	call	L3b58
L6fbc:
	inc	|_stkseg+4|(r15),#12
L6fc0:
	ldl	rr2,|_stkseg+2|(r15)
	cpl	rr2,|_stkseg+6|(r15)
	jr	c,L6f66
	add	r15,#%0012
	ret
L6fd0:
	dec	r15,#14
	ld	|_stkseg+0|(r15),r7
	cp	r7,<<1>>%04EE
	jr	nc,L6fec
	sla	r7,#4
	ldl	rr2,<<2>>%E47E
	add	r3,r7
	jr	L6ffa
L6fec:
	ldk	r7,#2
	ldl	rr4,#%01001660
	call	L3408
L6ffa:
	inc	r15,#14
	ret
L6ffe:
	dec	r15,#2
	ld	|_stkseg+0|(r15),r7
	ld	r6,#%0010
	call	_calloc
	ldl	<<2>>%E47E,rr2
	testl	rr2
	jr	ne,L7026
	ldk	r7,#2
	ldl	rr4,#%01001688
	call	L3408
L7026:
	inc	r15,#2
	ret
L702a:
	dec	r15,#2
	ld	|_stkseg+0|(r15),r7
	sla	r7,#4
	ld	r5,r7
	ldl	rr6,<<2>>%E47E
	call	_realloc
	ldl	<<2>>%E47E,rr2
	testl	rr2
	jr	ne,L705a
	ldk	r7,#2
	ldl	rr4,#%010016C2
	call	L3408
L705a:
	inc	r15,#2
	ret
L705e:
	sub	r15,#%0014
	ldm	_stkseg+8(r15),r8,#6
	ldl	rr12,rr6
	ldb	rl2,rr12(#%0008)
	extsb	r2
	ld	r11,r2
	bit	r2,#7
	jr	eq,L708e
	ldl	rr2,rr6
	inc	r3,#8
	ld	r4,r11
	and	r4,#%007F
	add	r3,r4
	ldl	rr8,rr2
L7086:
	inc	r13,#16
	cpl	rr8,rr12
	jr	ule,L7090
	jr	L7086
L708e:
	inc	r13,#16
L7090:
	cpl	rr12,<<2>>%E47E
	jr	c,L70b2
	ld	r2,<<1>>%04EE
	sla	r2,#4
	ldl	rr4,<<2>>%E47E
	add	r5,r2
	cpl	rr12,rr4
	jr	nc,L70b2
	ldl	rr2,rr12
	jr	L70c0
L70b2:
	ldk	r7,#2
	ldl	rr4,#%010016FC
	call	L3408
L70c0:
	ldm	r8,_stkseg+8(r15),#6
	add	r15,#%0014
	ret
L70ce:
	dec	r15,#8
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr2,<<2>>%E47E
	cpl	rr2,rr6
	jr	ugt,L7100
	ld	r4,<<1>>%04EE
	sla	r4,#4
	add	r3,r4
	cpl	rr2,rr6
	jr	ule,L7100
	ld	r2,|_stkseg+2|(r15)
	ldl	rr4,<<2>>%E47E
	sub	r2,r5
	sra	r2,#4
	jr	L710e
L7100:
	ldk	r7,#2
	ldl	rr4,#%0100170E
	call	L3408
L710e:
	inc	r15,#8
	ret
L7112:
	dec	r15,#8
	ldl	|_stkseg+0|(r15),rr6
	ldk	r2,#0
	ld	r3,<<1>>%15DA
	cpl	rr2,rr6
	jr	nc,L717c
	cpl	rr6,#%0000FFFF
	jr	ule,L713a
	ldk	r7,#2
	ldl	rr4,#%01001720
	call	L3408
L713a:
	test	<<1>>%15DA
	jr	eq,L714e
	ldl	rr6,<<1>>%90DE
	call	_free
L714e:
	ld	r6,|_stkseg+2|(r15)
	ldk	r7,#1
	call	_calloc
	ldl	<<1>>%90DE,rr2
	testl	rr2
	jr	ne,L7172
	ldk	r7,#2
	ldl	rr4,#%01001744
	call	L3408
L7172:
	ld	r2,|_stkseg+2|(r15)
	ld	<<1>>%15DA,r2
L717c:
	ldl	rr2,<<1>>%90DE
	inc	r15,#8
	ret
	ldl	rr6,#%01001780
	ld	r5,<<2>>%04FA
	ld	r4,#%0FFA
	call	_printf
	ret
L719e:
	dec	r15,#6
	ld	|_stkseg+0|(r15),r7
	cp	r7,#%0100
	jr	nc,L71b8
	sla	r7,#5
	ldl	rr2,#%01001C02
	add	r3,r7
	jr	L71c6
L71b8:
	ldk	r7,#2
	ldl	rr4,#%0100179C
	call	L3408
L71c6:
	inc	r15,#6
	ret
