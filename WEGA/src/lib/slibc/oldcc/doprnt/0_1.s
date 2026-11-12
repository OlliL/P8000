// doprnt.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	__align
	.extern	_stkseg
	.psec	data
	.data
	.comm	4, __pfile
	.psec
	.code
	.extern	__doprnt
	.extern	__length
	.extern	__flsbuf
	.extern	__cnvt
	.extern	__stfl
	.extern	__fegcvt
	.extern	_strcpy
	.extern	__fltuse
	.extern	__atoip
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 2792 bytes
	.psec
	.code
__align::
	dec	r15,#14
	ldm	|_stkseg|(r15),r2,#6
	ldl	rr0,rr6
	test	@rr6
	jr	eq,L0t_0014
	cp	@rr6,#%0002
	jr	ne,L0t_0026
L0t_0014:
	ldl	rr2,|_stkseg+8|(r15)
	inc	@rr2,#1
	ldl	rr4,|_stkseg+4|(r15)
	inc	r5,#2
	inc	@rr4,#2
	dec	r5,#2
	jr	L0t_003e
L0t_0026:
	ldl	rr2,|_stkseg+8|(r15)
	cp	@rr2,#%0004
	jr	ne,L0t_003e
	inc	@rr2,#2
	ldl	rr4,|_stkseg+4|(r15)
	ldl	rr6,|_stkseg|(r15)
	ldl	@rr4,rr6
	jr	L0t_0066
L0t_003e:
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr4,@rr2
	ld	r6,@rr4
	ld	|_stkseg+12|(r15),r6
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr4,@rr2
	ld	r7,rr4(#%0002)
	ldl	rr4,@rr2
	ld	@rr4,r7
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr4,@rr2
	inc	r5,#2
	ld	r7,|_stkseg+12|(r15)
	ld	@rr4,r7
L0t_0066:
	inc	r15,#14
	ret
__doprnt::
	sub	r15,#%00B4
	ldm	_stkseg+170(r15),r9,#5
	ldm	|_stkseg|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+12|(r15)
	clr	|_stkseg+126|(r15)
	ldl	rr6,#_stkseg+128
	add	r7,r15
	ldl	|_stkseg+160|(r15),rr6
	clr	L1d_0000
	ldl	|_stkseg+14|(r15),rr4
	ldl	rr6,rr12
	call	__length
	ld	r10,r2
	jr	L0t_00e4
L0t_00a8:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_00d2
L0t_00ba:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	extsb	r2
	jr	L0t_00e0
L0t_00d2:
	ld	r7,r11
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_00e0:
	inc	|_stkseg+126|(r15),#1
L0t_00e4:
	ld	r2,r10
	dec	r10,#1
	test	r2
	jp	eq,L0t_0a2c
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	subb	rh4,rh4
	ld	r11,r4
	cp	r4,#%0025
	jr	ne,L0t_00a8
	test	r10
	jp	eq,L0t_0a2c
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
	clr	|_stkseg+22|(r15)
	ld	|_stkseg+24|(r15),#%FFFF
	ldb	|_stkseg+165|(r15),#%20
	clr	|_stkseg+124|(r15)
	clr	|_stkseg+120|(r15)
	clr	|_stkseg+118|(r15)
	clr	|_stkseg+122|(r15)
	clr	|_stkseg+114|(r15)
	jr	L0t_017e
L0t_0138:
	test	|_stkseg+120|(r15)
	jr	eq,L0t_01ae
	ldb	|_stkseg+109|(r15),#%20
	ld	|_stkseg+122|(r15),#%0001
	jr	L0t_01b4
L0t_014c:
	test	|_stkseg+122|(r15)
	jr	ne,L0t_01a8
	ld	|_stkseg+122|(r15),#%0001
	jr	L0t_01a0
L0t_015a:
	test	|_stkseg+120|(r15)
	jr	ne,L0t_01a8
	ld	|_stkseg+120|(r15),#%0001
	jr	L0t_01a0
L0t_0168:
	test	|_stkseg+118|(r15)
	jr	ne,L0t_01a8
	ld	|_stkseg+118|(r15),#%0001
	jr	L0t_01a0
L0t_0176:
	ldk	r3,#0
	ldb	rl3,@rr12
	ld	r11,r3
	inc	r13,#1
L0t_017e:
	ld	r2,r11
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L0t_01a8
	rl	r2,#2
	ldl	rr4,L1d_0036(r2)
	jp	@rr4
L0t_0194:
	test	|_stkseg+114|(r15)
	jr	ne,L0t_01a8
	ld	|_stkseg+114|(r15),#%0001
L0t_01a0:
	ld	r3,r10
	dec	r10,#1
	test	r3
	jr	ne,L0t_0176
L0t_01a8:
	test	|_stkseg+122|(r15)
	jr	eq,L0t_0138
L0t_01ae:
	ldb	|_stkseg+109|(r15),#%2B
L0t_01b4:
	cp	r11,#%0061
	jr	lt,L0t_027a
	jr	L0t_0292
L0t_01bc:
	cp	r11,#%002A
	jr	ne,L0t_01ea
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+22|(r15),r4
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+184(r15),#1
	cp	_stkseg+184(r15),#%0005
	jr	ne,L0t_0212
	ldl	rr6,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L0t_0212
L0t_01ea:
	cp	r11,#%0030
	jr	ne,L0t_01f6
	ldb	|_stkseg+165|(r15),#%30
L0t_01f6:
	ldl	rr2,#_stkseg+22
	add	r3,r15
	dec	r13,#1
	ldl	rr6,rr12
	inc	r10,#1
	ld	r5,r10
	calr	__atoip
	ld	r9,r2
	ld	r3,r13
	add	r3,r2
	ld	r13,r3
	sub	r10,r2
L0t_0212:
	test	r10
	jr	eq,L0t_0292
	cpb	@rr12,#%2E
	jr	ne,L0t_026c
	inc	r13,#1
	dec	r10,#1
	test	r10
	jr	eq,L0t_0292
	cpb	@rr12,#%2A
	jr	ne,L0t_0254
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+24|(r15),r4
	inc	|_stkseg+16|(r15),#2
	ldk	r9,#1
	inc	_stkseg+184(r15),#1
	cp	_stkseg+184(r15),#%0005
	jr	ne,L0t_0264
	ldl	rr6,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L0t_0264
L0t_0254:
	ldl	rr2,#_stkseg+24
	add	r3,r15
	ld	r5,r10
	ldl	rr6,rr12
	calr	__atoip
	ld	r9,r2
L0t_0264:
	sub	r10,r9
	ld	r3,r13
	add	r3,r9
	ld	r13,r3
L0t_026c:
	test	r10
	jr	eq,L0t_0292
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
L0t_027a:
	cp	r11,#%002A
	jr	eq,L0t_01bc
	cp	r11,#%0030
	jr	lt,L0t_028c
	cp	r11,#%0039
	jr	le,L0t_01bc
L0t_028c:
	cp	r11,#%002E
	jr	eq,L0t_01bc
L0t_0292:
	cp	|_stkseg+24|(r15),#%0000
	jr	ge,L0t_02be
	ld	r2,r11
	ldl	rr4,#L1d_000a
	ld	r3,#%0006
	cpir	r2,@rr4,r3,eq
	jr	ne,L0t_02be
	sll	r3,#2
	neg	r3
	ldl	rr4,L1d_0082(r3)
	jp	@rr4
L0t_02ba:
	clr	|_stkseg+24|(r15)
L0t_02be:
	ld	|_stkseg+166|(r15),r11
	ld	r2,r11
	ldl	rr4,#L1d_0016
	ld	r6,#%0010
	cpir	r2,@rr4,r6,eq
	jp	ne,L0t_08fe
	sll	r6,#2
	neg	r6
	ldl	rr4,L1d_00c2(r6)
	jp	@rr4
L0t_02e6:
	ld	|_stkseg+24|(r15),#%0006
	jr	L0t_02be
L0t_02ee:
	ld	|_stkseg+26|(r15),#%0008
	jr	L0t_0304
L0t_02f6:
	ld	|_stkseg+26|(r15),#%0010
	jr	L0t_0304
L0t_02fe:
	ld	|_stkseg+26|(r15),#%000A
L0t_0304:
	ldl	rr4,|_stkseg+14|(r15)
	ld	r3,@rr4
	ld	r7,r3
	exts	rr6
	ldl	|_stkseg+28|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	clr	|_stkseg+28|(r15)
	inc	_stkseg+184(r15),#1
	cp	_stkseg+184(r15),#%0005
	jr	eq,L0t_0380
	jp	L0t_04a0
L0t_0330:
	ld	|_stkseg+26|(r15),#%0008
	jr	L0t_033e
L0t_0338:
	ld	|_stkseg+26|(r15),#%0010
L0t_033e:
	cp	_stkseg+184(r15),#%0005
	jr	ge,L0t_035e
	ldl	rr6,#_stkseg+184
	add	r7,r15
	ldl	rr4,#_stkseg+14
	add	r5,r15
	ldl	rr2,|_stkseg|(r15)
	calr	__align
L0t_035e:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+28|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	inc	_stkseg+184(r15),#2
	cp	_stkseg+184(r15),#%0005
	jp	ne,L0t_04a0
L0t_0380:
	ldl	rr6,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr6
	jp	L0t_04a0
L0t_038e:
	ld	|_stkseg+26|(r15),#%000A
	jr	L0t_033e
L0t_0396:
	ld	|_stkseg+26|(r15),#%FFF6
	jr	L0t_033e
L0t_039e:
	test	r10
	jr	eq,L0t_043e
	dec	r10,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl3,@rr4
	subb	rh3,rh3
	ld	r11,r3
	cp	r3,#%006F
	jr	eq,L0t_0330
	cp	r3,#%0078
	jr	eq,L0t_0338
	cp	r3,#%0064
	jr	eq,L0t_0396
	cp	r3,#%0075
	jr	eq,L0t_038e
	ldl	rr4,_stkseg+186(r15)
	inc	r5,#4
	dec	@rr4,#1
	ld	r3,@rr4
	cp	r3,#%0000
	jr	lt,L0t_03f2
	ldl	rr4,_stkseg+186(r15)
	inc	r5,#2
	inc	@rr4,#1
	dec	r5,#2
	ldl	rr6,@rr4
	dec	r7,#1
	ldb	@rr6,#%6C
	ldb	rl3,#%6C
	extsb	r3
	jr	L0t_0402
L0t_03f2:
	ld	r7,#%006C
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_0402:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_042c
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	extsb	r2
	jr	L0t_043a
L0t_042c:
	ld	r7,r11
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_043a:
	inc	|_stkseg+126|(r15),#2
L0t_043e:
	test	|_stkseg+24|(r15)
	jr	eq,L0t_0456
	cp	|_stkseg+166|(r15),#%0073
	jr	ne,L0t_0456
	cp	r9,|_stkseg+24|(r15)
	jr	le,L0t_0456
	ld	r9,|_stkseg+24|(r15)
L0t_0456:
	cp	r9,|_stkseg+22|(r15)
	jp	ge,L0t_091a
	ld	r2,|_stkseg+22|(r15)
	sub	r2,r9
	ld	|_stkseg+116|(r15),r2
	jp	L0t_091e
L0t_0470:
	ld	|_stkseg+26|(r15),#%FFF6
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	r7,r4
	exts	rr6
	ldl	|_stkseg+28|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+184(r15),#1
	cp	_stkseg+184(r15),#%0005
	jr	ne,L0t_04a0
	ldl	rr4,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr4
L0t_04a0:
	test	|_stkseg+122|(r15)
	jr	eq,L0t_04d8
	cp	|_stkseg+26|(r15),#%FFF6
	jr	ne,L0t_04d8
	ldl	rr2,|_stkseg+28|(r15)
	cpl	rr2,#%00000000
	jr	lt,L0t_04d8
	ldb	rl2,|_stkseg+109|(r15)
	ldb	|_stkseg+36|(r15),rl2
	ldl	rr4,#_stkseg+36
	add	r5,r15
	ld	|_stkseg+124|(r15),#%0001
	inc	r5,#1
	ldl	|_stkseg+100|(r15),rr4
	jr	L0t_054e
L0t_04d8:
	cp	|_stkseg+26|(r15),#%0008
	jr	ne,L0t_0506
	test	|_stkseg+118|(r15)
	jr	eq,L0t_0506
	ldl	rr2,|_stkseg+28|(r15)
	testl	rr2
	jr	eq,L0t_0506
	ldb	|_stkseg+36|(r15),#%30
	ldl	rr2,#_stkseg+36
	add	r3,r15
	ld	|_stkseg+124|(r15),#%0001
	inc	r3,#1
	jr	L0t_054a
L0t_0506:
	cp	|_stkseg+26|(r15),#%0010
	jr	ne,L0t_0542
	test	|_stkseg+118|(r15)
	jr	eq,L0t_0542
	ldl	rr2,|_stkseg+28|(r15)
	testl	rr2
	jr	eq,L0t_0542
	ldb	|_stkseg+36|(r15),#%30
	ldl	rr2,#_stkseg+36
	add	r3,r15
	inc	r3,#1
	ldb	@rr2,#%78
	ldl	rr2,#_stkseg+36
	add	r3,r15
	ld	|_stkseg+124|(r15),#%0002
	inc	r3,#2
	jr	L0t_054a
L0t_0542:
	ldl	rr2,#_stkseg+36
	add	r3,r15
L0t_054a:
	ldl	|_stkseg+100|(r15),rr2
L0t_054e:
	cp	|_stkseg+26|(r15),#%FFF6
	jr	ne,L0t_05be
	ldl	rr2,|_stkseg+28|(r15)
	cpl	rr2,#%00000000
	jr	ge,L0t_05be
	cpl	rr2,#%80000000
	jr	ne,L0t_0570
	ld	|_stkseg+26|(r15),#%000A
L0t_0570:
	ldl	rr6,|_stkseg+28|(r15)
	multl	rq4,#%FFFFFFFF
	ldl	|_stkseg+28|(r15),rr6
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_05aa
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	@rr4,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	L0t_05ba
L0t_05aa:
	ld	r7,#%002D
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_05ba:
	inc	|_stkseg+126|(r15),#1
L0t_05be:
	ldl	rr2,#_stkseg+36
	add	r3,r15
	ldl	|_stkseg+32|(r15),rr2
	ldl	rr6,|_stkseg+100|(r15)
	ld	r5,|_stkseg+26|(r15)
	ldl	rr2,|_stkseg+28|(r15)
	call	__cnvt
	add	r2,|_stkseg+124|(r15)
L0t_05e0:
	ld	r9,r2
	jp	L0t_043e
L0t_05e8:
	ldb	|_stkseg+165|(r15),#%20
	cp	_stkseg+184(r15),#%0005
	jr	ge,L0t_060e
	ldl	rr6,#_stkseg+184
	add	r7,r15
	ldl	rr4,#_stkseg+14
	add	r5,r15
	ldl	rr2,|_stkseg|(r15)
	calr	__align
L0t_060e:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+32|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	ldl	rr4,|_stkseg+32|(r15)
	testl	rr4
	jr	ne,L0t_062e
	ldl	rr6,#L1d_0002
	ldl	|_stkseg+32|(r15),rr6
L0t_062e:
	ldl	rr6,|_stkseg+32|(r15)
	call	__length
	ld	r9,r2
	inc	_stkseg+184(r15),#2
	cp	_stkseg+184(r15),#%0005
	jp	ne,L0t_043e
	ldl	rr2,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr2
	jp	L0t_043e
L0t_065c:
	ldb	|_stkseg+165|(r15),#%20
	ldk	r9,#1
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+32|(r15),rr2
	inc	|_stkseg+16|(r15),#2
	inc	|_stkseg+34|(r15),#1
	inc	_stkseg+184(r15),#1
	cp	_stkseg+184(r15),#%0005
	jp	ne,L0t_043e
	ldl	rr4,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr4
	jp	L0t_043e
L0t_0696:
	ld	r2,L1d_0000
	inc	L1d_0000,#1
	test	r2
	jr	ne,L0t_06b4
	ldl	rr6,#_stkseg+128
	add	r7,r15
	call	__stfl
L0t_06b4:
	cp	|_stkseg+12|(r15),#%0004
	jr	ge,L0t_06d6
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr4,|_stkseg+160|(r15)
	ldl	|_stkseg+14|(r15),rr4
	inc	|_stkseg+162|(r15),#8
	inc	|_stkseg+12|(r15),#1
	jr	L0t_06ea
L0t_06d6:
	subl	rr2,rr2
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr2,|_stkseg|(r15)
	ldl	|_stkseg+14|(r15),rr2
	inc	_stkseg+184(r15),#5
L0t_06ea:
	ldl	rr4,#_stkseg+36
	add	r5,r15
	ldl	rr2,|_stkseg+14|(r15)
	.word	%0F24,%8303	// EPU
	ld	r7,|_stkseg+24|(r15)
	ld	r3,r11
	call	__fegcvt
	ldl	|_stkseg+32|(r15),rr2
	ldl	rr2,|_stkseg+18|(r15)
	testl	rr2
	jr	eq,L0t_0718
	ldl	|_stkseg+14|(r15),rr2
	jr	L0t_071c
L0t_0718:
	inc	|_stkseg+16|(r15),#8
L0t_071c:
	test	|_stkseg+122|(r15)
	jr	eq,L0t_0776
	ldl	rr2,|_stkseg+32|(r15)
	cpb	@rr2,#%2D
	jr	eq,L0t_0776
	testb	@rr2
	jr	eq,L0t_0776
	ldl	|_stkseg+100|(r15),rr2
	ldb	rl4,@rr2
	ldb	|_stkseg+105|(r15),rl4
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl5,|_stkseg+109|(r15)
	ldb	@rr2,rl5
	inc	|_stkseg+102|(r15),#1
	jr	L0t_076a
L0t_074a:
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl4,@rr2
	ldb	|_stkseg+107|(r15),rl4
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl5,|_stkseg+105|(r15)
	ldb	@rr2,rl5
	inc	|_stkseg+102|(r15),#1
	ldb	rl5,|_stkseg+107|(r15)
	ldb	|_stkseg+105|(r15),rl5
L0t_076a:
	testb	|_stkseg+105|(r15)
	jr	ne,L0t_074a
	ldl	rr2,|_stkseg+100|(r15)
	clrb	@rr2
L0t_0776:
	test	|_stkseg+118|(r15)
	jr	ne,L0t_0828
	cp	r11,#%0065
	jr	eq,L0t_0788
	cp	r11,#%0045
	jr	ne,L0t_07d4
L0t_0788:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L0t_07a6
L0t_0792:
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl4,rr2(#%0001)
	ldb	@rr2,rl4
	inc	|_stkseg+102|(r15),#1
	jr	L0t_07ca
L0t_07a2:
	inc	|_stkseg+102|(r15),#1
L0t_07a6:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jp	eq,L0t_08c2
	cpb	@rr2,#%2E
	jr	ne,L0t_07a2
	inc	|_stkseg+102|(r15),#1
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%65
	jr	ne,L0t_07a2
	dec	|_stkseg+102|(r15),#1
L0t_07ca:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L0t_0792
	jr	L0t_08c2
L0t_07d4:
	cp	r11,#%0067
	jr	eq,L0t_07e0
	cp	r11,#%0047
	jr	ne,L0t_08c2
L0t_07e0:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L0t_07fe
L0t_07ea:
	ldl	rr6,|_stkseg+110|(r15)
	ldl	rr4,|_stkseg+100|(r15)
	call	_strcpy
	jr	L0t_08c2
L0t_07fa:
	inc	|_stkseg+102|(r15),#1
L0t_07fe:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	eq,L0t_08c2
	cpb	@rr2,#%2E
	jr	ne,L0t_07fa
	ldl	rr4,rr2
	ldl	|_stkseg+110|(r15),rr2
L0t_0812:
	inc	|_stkseg+102|(r15),#1
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%30
	jr	eq,L0t_0812
	cpb	@rr2,#%65
	jr	eq,L0t_07ea
	jr	L0t_08c2
L0t_0828:
	cp	r11,#%0066
	jr	ne,L0t_0860
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L0t_0846
L0t_0838:
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%2E
	jr	eq,L0t_084e
	inc	|_stkseg+102|(r15),#1
L0t_0846:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L0t_0838
L0t_084e:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L0t_08c2
	ldb	@rr2,#%2E
	inc	|_stkseg+102|(r15),#1
	jr	L0t_08bc
L0t_0860:
	cp	r11,#%0067
	jr	eq,L0t_086c
	cp	r11,#%0047
	jr	ne,L0t_08c2
L0t_086c:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L0t_0884
L0t_0876:
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%2E
	jr	eq,L0t_088c
	inc	|_stkseg+102|(r15),#1
L0t_0884:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L0t_0876
L0t_088c:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L0t_08c2
	ldb	@rr2,#%2E
	inc	|_stkseg+102|(r15),#1
	clr	|_stkseg+124|(r15)
	jr	L0t_08b2
L0t_08a2:
	ldl	rr4,|_stkseg+100|(r15)
	ldb	@rr4,#%30
	inc	|_stkseg+102|(r15),#1
	inc	|_stkseg+124|(r15),#1
L0t_08b2:
	ld	r2,|_stkseg+124|(r15)
	cp	r2,|_stkseg+24|(r15)
	jr	lt,L0t_08a2
L0t_08bc:
	ldl	rr2,|_stkseg+100|(r15)
	clrb	@rr2
L0t_08c2:
	cp	r11,#%0045
	jr	eq,L0t_08ce
	cp	r11,#%0047
	jr	ne,L0t_08e8
L0t_08ce:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
L0t_08d6:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	eq,L0t_08e8
	cpb	@rr2,#%65
	jr	ne,L0t_08f8
	ldb	@rr2,#%45
L0t_08e8:
	ldl	rr6,|_stkseg+32|(r15)
	call	__length
	jp	L0t_05e0
L0t_08f8:
	inc	|_stkseg+102|(r15),#1
	jr	L0t_08d6
L0t_08fe:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jp	lt,L0t_00d2
	jp	L0t_00ba
L0t_091a:
	clr	|_stkseg+116|(r15)
L0t_091e:
	test	|_stkseg+114|(r15)
	jr	eq,L0t_0968
	jr	L0t_09c6
L0t_0926:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_0952
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+165|(r15)
	ldb	@rr4,rl2
	extsb	r2
	jr	L0t_0964
L0t_0952:
	ldb	rl7,|_stkseg+165|(r15)
	extsb	r7
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_0964:
	inc	|_stkseg+126|(r15),#1
L0t_0968:
	ld	r2,|_stkseg+116|(r15)
	dec	|_stkseg+116|(r15),#1
	test	r2
	jr	ne,L0t_0926
	jr	L0t_09c6
L0t_0976:
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_09a8
	ldl	rr2,|_stkseg+32|(r15)
	inc	|_stkseg+34|(r15),#1
	ldl	rr4,_stkseg+186(r15)
	inc	r5,#2
	inc	@rr4,#1
	dec	r5,#2
	ldl	rr6,@rr4
	dec	r7,#1
	ldb	rl4,@rr2
	ldb	@rr6,rl4
	extsb	r4
	jr	L0t_09c2
L0t_09a8:
	ldl	rr2,|_stkseg+32|(r15)
	inc	|_stkseg+34|(r15),#1
	ldb	rl7,@rr2
	extsb	r7
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
	ld	r4,r2
L0t_09c2:
	inc	|_stkseg+126|(r15),#1
L0t_09c6:
	ld	r2,r9
	dec	r9,#1
	test	r2
	jr	ne,L0t_0976
	test	|_stkseg+114|(r15)
	jp	eq,L0t_00e4
L0t_09d8:
	ld	r2,|_stkseg+116|(r15)
	dec	|_stkseg+116|(r15),#1
	test	r2
	jp	eq,L0t_00e4
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L0t_0a14
	ldl	rr2,_stkseg+186(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+165|(r15)
	ldb	@rr4,rl2
	extsb	r2
	jr	L0t_0a26
L0t_0a14:
	ldb	rl7,|_stkseg+165|(r15)
	extsb	r7
	ldl	rr4,_stkseg+186(r15)
	call	__flsbuf
L0t_0a26:
	inc	|_stkseg+126|(r15),#1
	jr	L0t_09d8
L0t_0a2c:
	ldl	rr2,__pfile
	testl	rr2
	jr	eq,L0t_0a4c
	ldl	rr4,rr2
	ldb	rl6,rr4(#%000A)
	extsb	r6
	and	r6,#%0020
	test	r6
	jr	eq,L0t_0a4c
	ld	r2,#%FFFF
	jr	L0t_0a50
L0t_0a4c:
	ld	r2,|_stkseg+126|(r15)
L0t_0a50:
	ldm	r9,_stkseg+170(r15),#5
	add	r15,#%00B4
	ret
__atoip::
	sub	r15,#%001A
	ldm	_stkseg+18(r15),r10,#4
	ldm	|_stkseg|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+12|(r15)
	ld	r11,r5
	test	r5
	jr	eq,L0t_0a8e
	cpb	@rr12,#%2D
	jr	ne,L0t_0a8e
	dec	|_stkseg+6|(r15),#1
	inc	r13,#1
	ld	|_stkseg+12|(r15),#%0001
L0t_0a8e:
	ldk	r10,#0
	jr	L0t_0abc
L0t_0a92:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	ldb	|_stkseg+15|(r15),rl4
	cpb	rl4,#%30
	jr	lt,L0t_0ac0
	cpb	rl4,#%39
	jr	gt,L0t_0ac0
	ld	r3,r10
	mult	rr2,#%000A
	ldb	rl2,rl4
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r10,r3
	dec	r11,#1
L0t_0abc:
	test	r11
	jr	ne,L0t_0a92
L0t_0ac0:
	test	|_stkseg+12|(r15)
	jr	eq,L0t_0acc
	ld	r2,r10
	neg	r2
	jr	L0t_0ace
L0t_0acc:
	ld	r2,r10
L0t_0ace:
	ldl	rr4,|_stkseg|(r15)
	ld	@rr4,r2
	ld	r2,|_stkseg+6|(r15)
	sub	r2,r11
	ldm	r10,_stkseg+18(r15),#4
	add	r15,#%001A
	ret

// segment table entry 1 (segment 1), data, 198 bytes
	.psec	data
	.data
L1d_0000:
	.word	%0000
L1d_0002:
	.word	%286E
	.word	%756C
	.word	%6C29
	.word	%0000
L1d_000a:
	.word	%0045
	.word	%0047
	.word	%0065
	.word	%0066
	.word	%0067
	.word	%0073
L1d_0016:
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
L1d_0036:
	.long	L0t_015a
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_0168
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_01a8
	.long	L0t_014c
	.long	L0t_01a8
	.long	L0t_0194
	.long	L0t_02e6
	.long	L0t_02e6
	.long	L0t_02e6
	.long	L0t_02e6
	.long	L0t_02e6
L1d_0082:
	.long	L0t_02ba
	.long	L0t_0396
	.long	L0t_0696
	.long	L0t_0696
	.long	L0t_0330
	.long	L0t_038e
	.long	L0t_0338
	.long	L0t_065c
	.long	L0t_0470
	.long	L0t_0696
	.long	L0t_0696
	.long	L0t_0696
	.long	L0t_039e
	.long	L0t_02ee
	.long	L0t_05e8
	.long	L0t_02fe
L1d_00c2:
	.long	L0t_02f6
