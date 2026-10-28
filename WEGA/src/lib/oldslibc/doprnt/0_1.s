	.seg
	.extern	_stkseg
	.extern	__length
	.extern	__flsbuf
	.extern	__cnvt
	.extern	__fegcvt
	.extern	_strcpy
	.code

__align::
L7bbc:
	dec	r15,#14
	ldm	|_stkseg+0|(r15),r2,#6
	ldl	rr0,rr6
	test	@rr6
	jr	eq,L7bd0
	cp	@rr6,#%0002
	jr	ne,L7be2
L7bd0:
	ldl	rr2,|_stkseg+8|(r15)
	inc	@rr2,#1
	ldl	rr4,|_stkseg+4|(r15)
	inc	r5,#2
	inc	@rr4,#2
	dec	r5,#2
	jr	L7bfa
L7be2:
	ldl	rr2,|_stkseg+8|(r15)
	cp	@rr2,#%0004
	jr	ne,L7bfa
	inc	@rr2,#2
	ldl	rr4,|_stkseg+4|(r15)
	ldl	rr6,|_stkseg+0|(r15)
	ldl	@rr4,rr6
	jr	L7c22
L7bfa:
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
L7c22:
	inc	r15,#14
	ret
__doprnt::
	sub	r15,#%0090
	ldm	_stkseg+134(r15),r9,#5
	ldm	|_stkseg+0|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+12|(r15)
	clr	|_stkseg+126|(r15)
	ldl	|_stkseg+14|(r15),rr4
	call	__length
	ld	r10,r2
	jr	L7c8c
L7c50:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L7c72
L7c62:
	ld	r7,r11
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L7c88
L7c72:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	subb	rh2,rh2
L7c88:
	inc	|_stkseg+126|(r15),#1
L7c8c:
	ld	r2,r10
	dec	r10,#1
	test	r2
	jp	eq,L85c6
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	subb	rh4,rh4
	ld	r11,r4
	cp	r4,#%0025
	jr	ne,L7c50
	test	r10
	jp	eq,L85c6
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
	clr	|_stkseg+22|(r15)
	ld	|_stkseg+24|(r15),#%FFFF
	ldb	|_stkseg+129|(r15),#%20
	clr	|_stkseg+124|(r15)
	clr	|_stkseg+120|(r15)
	clr	|_stkseg+118|(r15)
	clr	|_stkseg+122|(r15)
	clr	|_stkseg+114|(r15)
	jr	L7d26
L7ce0:
	test	|_stkseg+120|(r15)
	jr	eq,L7d56
	ldb	|_stkseg+109|(r15),#%20
	ld	|_stkseg+122|(r15),#%0001
	jr	L7d5c
L7cf4:
	test	|_stkseg+122|(r15)
	jr	ne,L7d50
	ld	|_stkseg+122|(r15),#%0001
	jr	L7d48
L7d02:
	test	|_stkseg+120|(r15)
	jr	ne,L7d50
	ld	|_stkseg+120|(r15),#%0001
	jr	L7d48
L7d10:
	test	|_stkseg+118|(r15)
	jr	ne,L7d50
	ld	|_stkseg+118|(r15),#%0001
	jr	L7d48
L7d1e:
	ldk	r3,#0
	ldb	rl3,@rr12
	ld	r11,r3
	inc	r13,#1
L7d26:
	ld	r2,r11
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L7d50
	rl	r2,#2
	ldl	rr4,_d189E+0(r2)
	jp	@rr4
L7d3c:
	test	|_stkseg+114|(r15)
	jr	ne,L7d50
	ld	|_stkseg+114|(r15),#%0001
L7d48:
	ld	r3,r10
	dec	r10,#1
	test	r3
	jr	ne,L7d1e
L7d50:
	test	|_stkseg+122|(r15)
	jr	eq,L7ce0
L7d56:
	ldb	|_stkseg+109|(r15),#%2B
L7d5c:
	cp	r11,#%0061
	jr	lt,L7e26
	jr	L7e3e
L7d64:
	cp	r11,#%002A
	jr	ne,L7d94
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+22|(r15),r4
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+152(r15),#1
	cp	_stkseg+152(r15),#%0005
	jr	ne,L7dbc
	ldl	rr6,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L7dbc
L7d94:
	cp	r11,#%0030
	jr	ne,L7da0
	ldb	|_stkseg+129|(r15),#%30
L7da0:
	ldl	rr2,#%7F000016
	add	r3,r15
	dec	r13,#1
	ldl	rr6,rr12
	inc	r10,#1
	ld	r5,r10
	calr	L85f8
	ld	r9,r2
	ld	r3,r13
	add	r3,r2
	ld	r13,r3
	sub	r10,r2
L7dbc:
	test	r10
	jr	eq,L7e3e
	cpb	@rr12,#%2E
	jr	ne,L7e18
	inc	r13,#1
	dec	r10,#1
	test	r10
	jr	eq,L7e3e
	cpb	@rr12,#%2A
	jr	ne,L7e00
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+24|(r15),r4
	inc	|_stkseg+16|(r15),#2
	ldk	r9,#1
	inc	_stkseg+152(r15),#1
	cp	_stkseg+152(r15),#%0005
	jr	ne,L7e10
	ldl	rr6,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L7e10
L7e00:
	ldl	rr2,#%7F000018
	add	r3,r15
	ld	r5,r10
	ldl	rr6,rr12
	calr	L85f8
	ld	r9,r2
L7e10:
	sub	r10,r9
	ld	r3,r13
	add	r3,r9
	ld	r13,r3
L7e18:
	test	r10
	jr	eq,L7e3e
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
L7e26:
	cp	r11,#%002A
	jr	eq,L7d64
	cp	r11,#%0030
	jr	lt,L7e38
	cp	r11,#%0039
	jr	le,L7d64
L7e38:
	cp	r11,#%002E
	jr	eq,L7d64
L7e3e:
	cp	|_stkseg+24|(r15),#%0000
	jr	ge,L7e6a
	ld	r2,r11
	ldl	rr4,#_d1872
	ld	r3,#%0006
	cpir	r2,@rr4,r3,eq
	jr	ne,L7e6a
	sll	r3,#2
	neg	r3
	ldl	rr4,_d18EA+0(r3)
	jp	@rr4
L7e66:
	clr	|_stkseg+24|(r15)
L7e6a:
	ld	|_stkseg+130|(r15),r11
	ld	r2,r11
	ldl	rr4,#_d187E
	ld	r6,#%0010
	cpir	r2,@rr4,r6,eq
	jp	ne,L849a
	sll	r6,#2
	neg	r6
	ldl	rr4,_d192A+0(r6)
	jp	@rr4
L7e92:
	ld	|_stkseg+24|(r15),#%0006
	jr	L7e6a
L7e9a:
	ld	|_stkseg+26|(r15),#%0008
	jr	L7eb0
L7ea2:
	ld	|_stkseg+26|(r15),#%0010
	jr	L7eb0
L7eaa:
	ld	|_stkseg+26|(r15),#%000A
L7eb0:
	ldl	rr4,|_stkseg+14|(r15)
	ld	r3,@rr4
	ld	r7,r3
	exts	rr6
	ldl	|_stkseg+28|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	clr	|_stkseg+28|(r15)
	inc	_stkseg+152(r15),#1
	cp	_stkseg+152(r15),#%0005
	jr	eq,L7f2e
	jp	L8052
L7edc:
	ld	|_stkseg+26|(r15),#%0008
	jr	L7eea
L7ee4:
	ld	|_stkseg+26|(r15),#%0010
L7eea:
	cp	_stkseg+152(r15),#%0005
	jr	ge,L7f0c
	ldl	rr6,#%7F000098
	add	r7,r15
	ldl	rr4,#%7F00000E
	add	r5,r15
	ldl	rr2,_stkseg+148(r15)
	calr	L7bbc
L7f0c:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+28|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	inc	_stkseg+152(r15),#2
	cp	_stkseg+152(r15),#%0005
	jp	ne,L8052
L7f2e:
	ldl	rr6,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr6
	jp	L8052
L7f3e:
	ld	|_stkseg+26|(r15),#%000A
	jr	L7eea
L7f46:
	ld	|_stkseg+26|(r15),#%FFF6
	jr	L7eea
L7f4e:
	test	r10
	jr	eq,L7fee
	dec	r10,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl3,@rr4
	subb	rh3,rh3
	ld	r11,r3
	cp	r3,#%006F
	jr	eq,L7edc
	cp	r3,#%0078
	jr	eq,L7ee4
	cp	r3,#%0064
	jr	eq,L7f46
	cp	r3,#%0075
	jr	eq,L7f3e
	ldl	rr4,_stkseg+154(r15)
	inc	r5,#4
	dec	@rr4,#1
	ld	r3,@rr4
	cp	r3,#%0000
	jr	ge,L7f9a
	ld	r7,#%006C
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L7fb2
L7f9a:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	@rr4,#%6C
	ldb	rl2,#%6C
	subb	rh2,rh2
L7fb2:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L7fd4
	ld	r7,r11
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L7fea
L7fd4:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	subb	rh2,rh2
L7fea:
	inc	|_stkseg+126|(r15),#2
L7fee:
	test	|_stkseg+24|(r15)
	jr	eq,L8006
	cp	|_stkseg+130|(r15),#%0073
	jr	ne,L8006
	cp	r9,|_stkseg+24|(r15)
	jr	le,L8006
	ld	r9,|_stkseg+24|(r15)
L8006:
	cp	r9,|_stkseg+22|(r15)
	jp	ge,L84b6
	ld	r2,|_stkseg+22|(r15)
	sub	r2,r9
	ld	|_stkseg+116|(r15),r2
	jp	L84ba
L8020:
	ld	|_stkseg+26|(r15),#%FFF6
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	r7,r4
	exts	rr6
	ldl	|_stkseg+28|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+152(r15),#1
	cp	_stkseg+152(r15),#%0005
	jr	ne,L8052
	ldl	rr4,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr4
L8052:
	test	|_stkseg+122|(r15)
	jr	eq,L808a
	cp	|_stkseg+26|(r15),#%FFF6
	jr	ne,L808a
	ldl	rr2,|_stkseg+28|(r15)
	cpl	rr2,#%00000000
	jr	lt,L808a
	ldb	rl2,|_stkseg+109|(r15)
	ldb	|_stkseg+36|(r15),rl2
	ldl	rr4,#%7F000024
	add	r5,r15
	ld	|_stkseg+124|(r15),#%0001
	inc	r5,#1
	ldl	|_stkseg+100|(r15),rr4
	jr	L8100
L808a:
	cp	|_stkseg+26|(r15),#%0008
	jr	ne,L80b8
	test	|_stkseg+118|(r15)
	jr	eq,L80b8
	ldl	rr2,|_stkseg+28|(r15)
	testl	rr2
	jr	eq,L80b8
	ldb	|_stkseg+36|(r15),#%30
	ldl	rr2,#%7F000024
	add	r3,r15
	ld	|_stkseg+124|(r15),#%0001
	inc	r3,#1
	jr	L80fc
L80b8:
	cp	|_stkseg+26|(r15),#%0010
	jr	ne,L80f4
	test	|_stkseg+118|(r15)
	jr	eq,L80f4
	ldl	rr2,|_stkseg+28|(r15)
	testl	rr2
	jr	eq,L80f4
	ldb	|_stkseg+36|(r15),#%30
	ldl	rr2,#%7F000024
	add	r3,r15
	inc	r3,#1
	ldb	@rr2,#%78
	ldl	rr2,#%7F000024
	add	r3,r15
	ld	|_stkseg+124|(r15),#%0002
	inc	r3,#2
	jr	L80fc
L80f4:
	ldl	rr2,#%7F000024
	add	r3,r15
L80fc:
	ldl	|_stkseg+100|(r15),rr2
L8100:
	cp	|_stkseg+26|(r15),#%FFF6
	jr	ne,L8170
	ldl	rr2,|_stkseg+28|(r15)
	cpl	rr2,#%00000000
	jr	ge,L8170
	cpl	rr2,#%80000000
	jr	ne,L8122
	ld	|_stkseg+26|(r15),#%000A
L8122:
	ldl	rr6,|_stkseg+28|(r15)
	multl	rq4,#%FFFFFFFF
	ldl	|_stkseg+28|(r15),rr6
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L8154
	ld	r7,#%002D
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L816c
L8154:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	@rr4,#%2D
	ldb	rl2,#%2D
	subb	rh2,rh2
L816c:
	inc	|_stkseg+126|(r15),#1
L8170:
	ldl	rr2,#%7F000024
	add	r3,r15
	ldl	|_stkseg+32|(r15),rr2
	ldl	rr6,|_stkseg+100|(r15)
	ld	r5,|_stkseg+26|(r15)
	ldl	rr2,|_stkseg+28|(r15)
	call	__cnvt
	add	r2,|_stkseg+124|(r15)
L8192:
	ld	r9,r2
	jp	L7fee
L819a:
	ldb	|_stkseg+129|(r15),#%20
	cp	_stkseg+152(r15),#%0005
	jr	ge,L81c2
	ldl	rr6,#%7F000098
	add	r7,r15
	ldl	rr4,#%7F00000E
	add	r5,r15
	ldl	rr2,_stkseg+148(r15)
	calr	L7bbc
L81c2:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+32|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	ldl	rr4,|_stkseg+32|(r15)
	testl	rr4
	jr	ne,L81e2
	ldl	rr6,#_d186A
	ldl	|_stkseg+32|(r15),rr6
L81e2:
	ldl	rr6,|_stkseg+32|(r15)
	call	__length
	ld	r9,r2
	inc	_stkseg+152(r15),#2
	cp	_stkseg+152(r15),#%0005
	jp	ne,L7fee
	ldl	rr2,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr2
	jp	L7fee
L8212:
	ldb	|_stkseg+129|(r15),#%20
	ldk	r9,#1
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+32|(r15),rr2
	inc	|_stkseg+16|(r15),#2
	inc	|_stkseg+34|(r15),#1
	inc	_stkseg+152(r15),#1
	cp	_stkseg+152(r15),#%0005
	jp	ne,L7fee
	ldl	rr4,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr4
	jp	L7fee
L824e:
	cp	|_stkseg+12|(r15),#%0004
	jr	ge,L8270
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr4,|_stkseg+0|(r15)
	ldl	|_stkseg+14|(r15),rr4
	inc	|_stkseg+2|(r15),#8
	inc	|_stkseg+12|(r15),#1
	jr	L8286
L8270:
	subl	rr2,rr2
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr2,_stkseg+148(r15)
	ldl	|_stkseg+14|(r15),rr2
	inc	_stkseg+152(r15),#5
L8286:
	ldl	rr4,#%7F000024
	add	r5,r15
	ldl	rr2,|_stkseg+14|(r15)
	.word	%0F24,%8303	// EPU
	ld	r7,|_stkseg+24|(r15)
	ld	r3,r11
	call	__fegcvt
	ldl	|_stkseg+32|(r15),rr2
	ldl	rr2,|_stkseg+18|(r15)
	testl	rr2
	jr	eq,L82b4
	ldl	|_stkseg+14|(r15),rr2
	jr	L82b8
L82b4:
	inc	|_stkseg+16|(r15),#8
L82b8:
	test	|_stkseg+122|(r15)
	jr	eq,L8312
	ldl	rr2,|_stkseg+32|(r15)
	cpb	@rr2,#%2D
	jr	eq,L8312
	testb	@rr2
	jr	eq,L8312
	ldl	|_stkseg+100|(r15),rr2
	ldb	rl4,@rr2
	ldb	|_stkseg+105|(r15),rl4
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl5,|_stkseg+109|(r15)
	ldb	@rr2,rl5
	inc	|_stkseg+102|(r15),#1
	jr	L8306
L82e6:
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl4,@rr2
	ldb	|_stkseg+107|(r15),rl4
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl5,|_stkseg+105|(r15)
	ldb	@rr2,rl5
	inc	|_stkseg+102|(r15),#1
	ldb	rl5,|_stkseg+107|(r15)
	ldb	|_stkseg+105|(r15),rl5
L8306:
	testb	|_stkseg+105|(r15)
	jr	ne,L82e6
	ldl	rr2,|_stkseg+100|(r15)
	clrb	@rr2
L8312:
	test	|_stkseg+118|(r15)
	jr	ne,L83c4
	cp	r11,#%0065
	jr	eq,L8324
	cp	r11,#%0045
	jr	ne,L8370
L8324:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L8342
L832e:
	ldl	rr2,|_stkseg+100|(r15)
	ldb	rl4,rr2(#%0001)
	ldb	@rr2,rl4
	inc	|_stkseg+102|(r15),#1
	jr	L8366
L833e:
	inc	|_stkseg+102|(r15),#1
L8342:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jp	eq,L845e
	cpb	@rr2,#%2E
	jr	ne,L833e
	inc	|_stkseg+102|(r15),#1
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%65
	jr	ne,L833e
	dec	|_stkseg+102|(r15),#1
L8366:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L832e
	jr	L845e
L8370:
	cp	r11,#%0067
	jr	eq,L837c
	cp	r11,#%0047
	jr	ne,L845e
L837c:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L839a
L8386:
	ldl	rr6,|_stkseg+110|(r15)
	ldl	rr4,|_stkseg+100|(r15)
	call	_strcpy
	jr	L845e
L8396:
	inc	|_stkseg+102|(r15),#1
L839a:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	eq,L845e
	cpb	@rr2,#%2E
	jr	ne,L8396
	ldl	rr4,rr2
	ldl	|_stkseg+110|(r15),rr2
L83ae:
	inc	|_stkseg+102|(r15),#1
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%30
	jr	eq,L83ae
	cpb	@rr2,#%65
	jr	eq,L8386
	jr	L845e
L83c4:
	cp	r11,#%0066
	jr	ne,L83fc
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L83e2
L83d4:
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%2E
	jr	eq,L83ea
	inc	|_stkseg+102|(r15),#1
L83e2:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L83d4
L83ea:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L845e
	ldb	@rr2,#%2E
	inc	|_stkseg+102|(r15),#1
	jr	L8458
L83fc:
	cp	r11,#%0067
	jr	eq,L8408
	cp	r11,#%0047
	jr	ne,L845e
L8408:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
	jr	L8420
L8412:
	ldl	rr2,|_stkseg+100|(r15)
	cpb	@rr2,#%2E
	jr	eq,L8428
	inc	|_stkseg+102|(r15),#1
L8420:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L8412
L8428:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	ne,L845e
	ldb	@rr2,#%2E
	inc	|_stkseg+102|(r15),#1
	clr	|_stkseg+124|(r15)
	jr	L844e
L843e:
	ldl	rr4,|_stkseg+100|(r15)
	ldb	@rr4,#%30
	inc	|_stkseg+102|(r15),#1
	inc	|_stkseg+124|(r15),#1
L844e:
	ld	r2,|_stkseg+124|(r15)
	cp	r2,|_stkseg+24|(r15)
	jr	lt,L843e
L8458:
	ldl	rr2,|_stkseg+100|(r15)
	clrb	@rr2
L845e:
	cp	r11,#%0045
	jr	eq,L846a
	cp	r11,#%0047
	jr	ne,L8484
L846a:
	ldl	rr2,|_stkseg+32|(r15)
	ldl	|_stkseg+100|(r15),rr2
L8472:
	ldl	rr2,|_stkseg+100|(r15)
	testb	@rr2
	jr	eq,L8484
	cpb	@rr2,#%65
	jr	ne,L8494
	ldb	@rr2,#%45
L8484:
	ldl	rr6,|_stkseg+32|(r15)
	call	__length
	jp	L8192
L8494:
	inc	|_stkseg+102|(r15),#1
	jr	L8472
L849a:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jp	ge,L7c72
	jp	L7c62
L84b6:
	clr	|_stkseg+116|(r15)
L84ba:
	test	|_stkseg+114|(r15)
	jr	eq,L8504
	jr	L8560
L84c2:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L84e8
	ldb	rl7,|_stkseg+129|(r15)
	extsb	r7
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L8500
L84e8:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+129|(r15)
	ldb	@rr4,rl2
	subb	rh2,rh2
L8500:
	inc	|_stkseg+126|(r15),#1
L8504:
	ld	r2,|_stkseg+116|(r15)
	dec	|_stkseg+116|(r15),#1
	test	r2
	jr	ne,L84c2
	jr	L8560
L8512:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L853e
	ldl	rr2,|_stkseg+32|(r15)
	inc	|_stkseg+34|(r15),#1
	ldb	rl7,@rr2
	extsb	r7
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L855c
L853e:
	ldl	rr2,|_stkseg+32|(r15)
	inc	|_stkseg+34|(r15),#1
	ldl	rr4,_stkseg+154(r15)
	inc	r5,#2
	inc	@rr4,#1
	dec	r5,#2
	ldl	rr6,@rr4
	dec	r7,#1
	ldb	rl4,@rr2
	ldb	@rr6,rl4
	subb	rh4,rh4
L855c:
	inc	|_stkseg+126|(r15),#1
L8560:
	ld	r2,r9
	dec	r9,#1
	test	r2
	jr	ne,L8512
	test	|_stkseg+114|(r15)
	jp	eq,L7c8c
L8572:
	ld	r2,|_stkseg+116|(r15)
	dec	|_stkseg+116|(r15),#1
	test	r2
	jp	eq,L7c8c
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L85a8
	ldb	rl7,|_stkseg+129|(r15)
	extsb	r7
	ldl	rr4,_stkseg+154(r15)
	call	__flsbuf
	jr	L85c0
L85a8:
	ldl	rr2,_stkseg+154(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+129|(r15)
	ldb	@rr4,rl2
	subb	rh2,rh2
L85c0:
	inc	|_stkseg+126|(r15),#1
	jr	L8572
L85c6:
	ldl	rr2,<<1>>%90E4
	testl	rr2
	jr	eq,L85e6
	ldl	rr4,rr2
	ldb	rl6,rr4(#%000A)
	extsb	r6
	and	r6,#%0020
	test	r6
	jr	eq,L85e6
	ld	r2,#%FFFF
	jr	L85ea
L85e6:
	ld	r2,|_stkseg+126|(r15)
L85ea:
	ldm	r9,_stkseg+134(r15),#5
	add	r15,#%0090
	ret
L85f8:
__atoip::
	sub	r15,#%001A
	ldm	_stkseg+18(r15),r10,#4
	ldm	|_stkseg+0|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+12|(r15)
	ld	r11,r5
	test	r5
	jr	eq,L8628
	cpb	@rr12,#%2D
	jr	ne,L8628
	dec	|_stkseg+6|(r15),#1
	inc	r13,#1
	ld	|_stkseg+12|(r15),#%0001
L8628:
	ldk	r10,#0
	jr	L8656
L862c:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	ldb	|_stkseg+15|(r15),rl4
	cpb	rl4,#%30
	jr	lt,L865a
	cpb	rl4,#%39
	jr	gt,L865a
	ld	r3,r10
	mult	rr2,#%000A
	ldb	rl2,rl4
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r10,r3
	dec	r11,#1
L8656:
	test	r11
	jr	ne,L862c
L865a:
	test	|_stkseg+12|(r15)
	jr	eq,L8666
	ld	r2,r10
	neg	r2
	jr	L8668
L8666:
	ld	r2,r10
L8668:
	ldl	rr4,|_stkseg+0|(r15)
	ld	@rr4,r2
	ld	r2,|_stkseg+6|(r15)
	sub	r2,r11
	ldm	r10,_stkseg+18(r15),#4
	add	r15,#%001A
	ret

	.data

_d186A::
	.byte	40,110,117,108,108,41,0,0

_d1872::
	.word	%0045
	.word	%0047
	.word	%0065
	.word	%0066
	.word	%0067
	.word	%0073
_d187E::
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

_d189E::
	.long	L7d02
	.long	L7d50
	.long	L7d50
	.long	L7d10
	.long	L7d50
	.long	L7d50
	.long	L7d50
	.long	L7d50
	.long	L7d50
	.long	L7d50
	.long	L7d50
	.long	L7cf4
	.long	L7d50
	.long	L7d3c
	.long	L7e92
	.long	L7e92
	.long	L7e92
	.long	L7e92
	.long	L7e92
_d18EA::
	.long	L7e66
	.long	L7f46
	.long	L824e
	.long	L824e
	.long	L7edc
	.long	L7f3e
	.long	L7ee4
	.long	L8212
	.long	L8020
	.long	L824e
	.long	L824e
	.long	L824e
	.long	L7f4e
	.long	L7e9a
	.long	L819a
	.long	L7eaa
_d192A::
	.long	L7ea2
