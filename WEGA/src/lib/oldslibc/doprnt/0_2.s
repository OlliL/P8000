	.seg
	.extern	_stkseg
	.extern	__length
	.extern	__flsbuf
	.extern	__cnvt
	.extern	_strcpy
	.comm	4,__pfile
	.psec
	.code

__align::
L9b4a:
	dec	r15,#14
	ldm	|_stkseg+0|(r15),r2,#6
	ldl	rr0,rr6
	test	@rr6
	jr	eq,L9b5e
	cp	@rr6,#%0002
	jr	ne,L9b6c
L9b5e:
	ldl	rr0,rr6
	inc	@rr6,#1
	ldl	rr6,rr4
	inc	r7,#2
	inc	@rr6,#2
	dec	r7,#2
	jr	L9b84
L9b6c:
	ldl	rr2,|_stkseg+8|(r15)
	cp	@rr2,#%0004
	jr	ne,L9b84
	inc	@rr2,#2
	ldl	rr4,|_stkseg+4|(r15)
	ldl	rr6,|_stkseg+0|(r15)
	ldl	@rr4,rr6
	jr	L9bac
L9b84:
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
L9bac:
	inc	r15,#14
	ret
__doprnt::
	sub	r15,#%0096
	ldm	_stkseg+140(r15),r9,#5
	ldm	|_stkseg+0|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+12|(r15)
	clr	|_stkseg+132|(r15)
	ldl	|_stkseg+14|(r15),rr4
	call	__length
	ld	r10,r2
	jr	L9c16
L9bda:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L9c04
L9bec:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	extsb	r2
	jr	L9c12
L9c04:
	ld	r7,r11
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
L9c12:
	inc	|_stkseg+132|(r15),#1
L9c16:
	ld	r2,r10
	dec	r10,#1
	test	r2
	jp	eq,La558
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	subb	rh4,rh4
	ld	r11,r4
	cp	r4,#%0025
	jr	ne,L9bda
	test	r10
	jp	eq,La558
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
	clr	|_stkseg+28|(r15)
	ld	|_stkseg+30|(r15),#%FFFF
	ldb	|_stkseg+135|(r15),#%20
	clr	|_stkseg+130|(r15)
	clr	|_stkseg+126|(r15)
	clr	|_stkseg+124|(r15)
	clr	|_stkseg+128|(r15)
	clr	|_stkseg+120|(r15)
	jr	L9cb0
L9c6a:
	test	|_stkseg+126|(r15)
	jr	eq,L9ce0
	ldb	|_stkseg+115|(r15),#%20
	ld	|_stkseg+128|(r15),#%0001
	jr	L9ce6
L9c7e:
	test	|_stkseg+128|(r15)
	jr	ne,L9cda
	ld	|_stkseg+128|(r15),#%0001
	jr	L9cd2
L9c8c:
	test	|_stkseg+126|(r15)
	jr	ne,L9cda
	ld	|_stkseg+126|(r15),#%0001
	jr	L9cd2
L9c9a:
	test	|_stkseg+124|(r15)
	jr	ne,L9cda
	ld	|_stkseg+124|(r15),#%0001
	jr	L9cd2
L9ca8:
	ldk	r3,#0
	ldb	rl3,@rr12
	ld	r11,r3
	inc	r13,#1
L9cb0:
	ld	r2,r11
	sub	r2,#%0020
	cp	r2,#%000D
	jr	ugt,L9cda
	rl	r2,#2
	ldl	rr4,D8f66(r2)
	jp	@rr4
L9cc6:
	test	|_stkseg+120|(r15)
	jr	ne,L9cda
	ld	|_stkseg+120|(r15),#%0001
L9cd2:
	ld	r3,r10
	dec	r10,#1
	test	r3
	jr	ne,L9ca8
L9cda:
	test	|_stkseg+128|(r15)
	jr	eq,L9c6a
L9ce0:
	ldb	|_stkseg+115|(r15),#%2B
L9ce6:
	cp	r11,#%0061
	jr	lt,L9db0
	jr	L9dc8
L9cee:
	cp	r11,#%002A
	jr	ne,L9d1e
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+28|(r15),r4
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+158(r15),#1
	cp	_stkseg+158(r15),#%0005
	jr	ne,L9d46
	ldl	rr6,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L9d46
L9d1e:
	cp	r11,#%0030
	jr	ne,L9d2a
	ldb	|_stkseg+135|(r15),#%30
L9d2a:
	ldl	rr2,#%7F00001C
	add	r3,r15
	dec	r13,#1
	ldl	rr6,rr12
	inc	r10,#1
	ld	r5,r10
	calr	La58a
	ld	r9,r2
	ld	r3,r13
	add	r3,r2
	ld	r13,r3
	sub	r10,r2
L9d46:
	test	r10
	jr	eq,L9dc8
	cpb	@rr12,#%2E
	jr	ne,L9da2
	inc	r13,#1
	dec	r10,#1
	test	r10
	jr	eq,L9dc8
	cpb	@rr12,#%2A
	jr	ne,L9d8a
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	|_stkseg+30|(r15),r4
	inc	|_stkseg+16|(r15),#2
	ldk	r9,#1
	inc	_stkseg+158(r15),#1
	cp	_stkseg+158(r15),#%0005
	jr	ne,L9d9a
	ldl	rr6,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr6
	jr	L9d9a
L9d8a:
	ldl	rr2,#%7F00001E
	add	r3,r15
	ld	r5,r10
	ldl	rr6,rr12
	calr	La58a
	ld	r9,r2
L9d9a:
	sub	r10,r9
	ld	r3,r13
	add	r3,r9
	ld	r13,r3
L9da2:
	test	r10
	jr	eq,L9dc8
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
L9db0:
	cp	r11,#%002A
	jr	eq,L9cee
	cp	r11,#%0030
	jr	lt,L9dc2
	cp	r11,#%0039
	jr	le,L9cee
L9dc2:
	cp	r11,#%002E
	jr	eq,L9cee
L9dc8:
	cp	|_stkseg+30|(r15),#%0000
	jr	ge,L9df4
	ld	r2,r11
	ldl	rr4,#D8f3a
	ld	r3,#%0006
	cpir	r2,@rr4,r3,eq
	jr	ne,L9df4
	sll	r3,#2
	neg	r3
	ldl	rr4,D8fb2(r3)
	jp	@rr4
L9df0:
	clr	|_stkseg+30|(r15)
L9df4:
	ld	|_stkseg+136|(r15),r11
	ld	r2,r11
	ldl	rr4,#D8f46
	ld	r6,#%0010
	cpir	r2,@rr4,r6,eq
	jp	ne,La42a
	sll	r6,#2
	neg	r6
	ldl	rr4,D8ff2(r6)
	jp	@rr4
L9e1c:
	ld	|_stkseg+30|(r15),#%0006
	jr	L9df4
L9e24:
	ld	|_stkseg+32|(r15),#%0008
	jr	L9e3a
L9e2c:
	ld	|_stkseg+32|(r15),#%0010
	jr	L9e3a
L9e34:
	ld	|_stkseg+32|(r15),#%000A
L9e3a:
	ldl	rr4,|_stkseg+14|(r15)
	ld	r3,@rr4
	ld	r7,r3
	exts	rr6
	ldl	|_stkseg+34|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	clr	|_stkseg+34|(r15)
	inc	_stkseg+158(r15),#1
	cp	_stkseg+158(r15),#%0005
	jr	eq,L9eb8
	jp	L9fdc
L9e66:
	ld	|_stkseg+32|(r15),#%0008
	jr	L9e74
L9e6e:
	ld	|_stkseg+32|(r15),#%0010
L9e74:
	cp	_stkseg+158(r15),#%0005
	jr	ge,L9e96
	ldl	rr6,#%7F00009E
	add	r7,r15
	ldl	rr4,#%7F00000E
	add	r5,r15
	ldl	rr2,_stkseg+154(r15)
	calr	L9b4a
L9e96:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+34|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	inc	_stkseg+158(r15),#2
	cp	_stkseg+158(r15),#%0005
	jp	ne,L9fdc
L9eb8:
	ldl	rr6,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr6
	jp	L9fdc
L9ec8:
	ld	|_stkseg+32|(r15),#%000A
	jr	L9e74
L9ed0:
	ld	|_stkseg+32|(r15),#%FFF6
	jr	L9e74
L9ed8:
	test	r10
	jr	eq,L9f78
	dec	r10,#1
	ldl	rr4,rr12
	inc	r13,#1
	ldb	rl3,@rr4
	subb	rh3,rh3
	ld	r11,r3
	cp	r3,#%006F
	jr	eq,L9e66
	cp	r3,#%0078
	jr	eq,L9e6e
	cp	r3,#%0064
	jr	eq,L9ed0
	cp	r3,#%0075
	jr	eq,L9ec8
	ldl	rr4,_stkseg+160(r15)
	inc	r5,#4
	dec	@rr4,#1
	ld	r3,@rr4
	cp	r3,#%0000
	jr	lt,L9f2c
	ldl	rr4,_stkseg+160(r15)
	inc	r5,#2
	inc	@rr4,#1
	dec	r5,#2
	ldl	rr6,@rr4
	dec	r7,#1
	ldb	@rr6,#%6C
	ldb	rl3,#%6C
	extsb	r3
	jr	L9f3c
L9f2c:
	ld	r7,#%006C
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
L9f3c:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,L9f66
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ld	r2,r11
	ldb	@rr4,rl2
	extsb	r2
	jr	L9f74
L9f66:
	ld	r7,r11
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
L9f74:
	inc	|_stkseg+132|(r15),#2
L9f78:
	test	|_stkseg+30|(r15)
	jr	eq,L9f90
	cp	|_stkseg+136|(r15),#%0073
	jr	ne,L9f90
	cp	r9,|_stkseg+30|(r15)
	jr	le,L9f90
	ld	r9,|_stkseg+30|(r15)
L9f90:
	cp	r9,|_stkseg+28|(r15)
	jp	ge,La446
	ld	r2,|_stkseg+28|(r15)
	sub	r2,r9
	ld	|_stkseg+122|(r15),r2
	jp	La44a
L9faa:
	ld	|_stkseg+32|(r15),#%FFF6
	ldl	rr2,|_stkseg+14|(r15)
	ld	r4,@rr2
	ld	r7,r4
	exts	rr6
	ldl	|_stkseg+34|(r15),rr6
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+158(r15),#1
	cp	_stkseg+158(r15),#%0005
	jr	ne,L9fdc
	ldl	rr4,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr4
L9fdc:
	test	|_stkseg+128|(r15)
	jr	eq,La014
	cp	|_stkseg+32|(r15),#%FFF6
	jr	ne,La014
	ldl	rr2,|_stkseg+34|(r15)
	cpl	rr2,#%00000000
	jr	lt,La014
	ldb	rl4,|_stkseg+115|(r15)
	ldb	|_stkseg+42|(r15),rl4
	ldl	rr6,#%7F00002A
	add	r7,r15
	ld	|_stkseg+130|(r15),#%0001
	inc	r7,#1
	ldl	|_stkseg+106|(r15),rr6
	jr	La08e
La014:
	cp	|_stkseg+32|(r15),#%0008
	jr	ne,La07a
	test	|_stkseg+124|(r15)
	jr	eq,La07a
	ldl	rr2,|_stkseg+34|(r15)
	testl	rr2
	jr	eq,La07a
	ldb	|_stkseg+42|(r15),#%30
	ldl	rr4,#%7F00002A
	add	r5,r15
	ld	|_stkseg+130|(r15),#%0001
	inc	r5,#1
	jr	La074
La042:
	test	|_stkseg+124|(r15)
	jr	eq,La082
	ldl	rr2,|_stkseg+34|(r15)
	testl	rr2
	jr	eq,La082
	ldb	|_stkseg+42|(r15),#%30
	ldl	rr4,#%7F00002A
	add	r5,r15
	inc	r5,#1
	ldb	@rr4,#%78
	ldl	rr4,#%7F00002A
	add	r5,r15
	ld	|_stkseg+130|(r15),#%0002
	inc	r5,#2
La074:
	ldl	|_stkseg+106|(r15),rr4
	jr	La08e
La07a:
	cp	|_stkseg+32|(r15),#%0010
	jr	eq,La042
La082:
	ldl	rr2,#%7F00002A
	add	r3,r15
	ldl	|_stkseg+106|(r15),rr2
La08e:
	cp	|_stkseg+32|(r15),#%FFF6
	jr	ne,La0fe
	ldl	rr2,|_stkseg+34|(r15)
	cpl	rr2,#%00000000
	jr	ge,La0fe
	cpl	rr2,#%80000000
	jr	ne,La0b0
	ld	|_stkseg+32|(r15),#%000A
La0b0:
	ldl	rr6,|_stkseg+34|(r15)
	multl	rq4,#%FFFFFFFF
	ldl	|_stkseg+34|(r15),rr6
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,La0ea
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	@rr4,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	La0fa
La0ea:
	ld	r7,#%002D
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
La0fa:
	inc	|_stkseg+132|(r15),#1
La0fe:
	ldl	rr2,#%7F00002A
	add	r3,r15
	ldl	|_stkseg+38|(r15),rr2
	ldl	rr6,|_stkseg+106|(r15)
	ld	r5,|_stkseg+32|(r15)
	ldl	rr2,|_stkseg+34|(r15)
	call	__cnvt
	add	r2,|_stkseg+130|(r15)
La120:
	ld	r9,r2
	jp	L9f78
La128:
	ldb	|_stkseg+135|(r15),#%20
	cp	_stkseg+158(r15),#%0005
	jr	ge,La150
	ldl	rr6,#%7F00009E
	add	r7,r15
	ldl	rr4,#%7F00000E
	add	r5,r15
	ldl	rr2,_stkseg+154(r15)
	calr	L9b4a
La150:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr4,@rr2
	ldl	|_stkseg+38|(r15),rr4
	inc	|_stkseg+16|(r15),#4
	ldl	rr4,|_stkseg+38|(r15)
	testl	rr4
	jr	ne,La170
	ldl	rr6,#D8f32
	ldl	|_stkseg+38|(r15),rr6
La170:
	ldl	rr6,|_stkseg+38|(r15)
	call	__length
	ld	r9,r2
	inc	_stkseg+158(r15),#2
	cp	_stkseg+158(r15),#%0005
	jp	ne,L9f78
	ldl	rr2,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr2
	jp	L9f78
La1a0:
	ldb	|_stkseg+135|(r15),#%20
	ldk	r9,#1
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+38|(r15),rr2
	inc	|_stkseg+16|(r15),#2
	inc	|_stkseg+40|(r15),#1
	inc	_stkseg+158(r15),#1
	cp	_stkseg+158(r15),#%0005
	jp	ne,L9f78
	ldl	rr4,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr4
	jp	L9f78
La1dc:
	cp	|_stkseg+12|(r15),#%0004
	jr	ge,La1fe
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr4,|_stkseg+0|(r15)
	ldl	|_stkseg+14|(r15),rr4
	inc	|_stkseg+2|(r15),#8
	inc	|_stkseg+12|(r15),#1
	jr	La214
La1fe:
	subl	rr2,rr2
	ldl	|_stkseg+18|(r15),rr2
	ldl	rr2,_stkseg+154(r15)
	ldl	|_stkseg+14|(r15),rr2
	inc	_stkseg+158(r15),#5
La214:
	ldl	rr4,#%7F00002A
	add	r5,r15
	ldl	rr2,|_stkseg+14|(r15)
	.word	%0F24,%8303	// EPU
	ld	r7,|_stkseg+30|(r15)
	ld	r3,r11
	call	La616
	ldl	|_stkseg+38|(r15),rr2
	ldl	rr2,|_stkseg+18|(r15)
	testl	rr2
	jr	eq,La242
	ldl	|_stkseg+14|(r15),rr2
	jr	La246
La242:
	inc	|_stkseg+16|(r15),#8
La246:
	test	|_stkseg+128|(r15)
	jr	eq,La2a2
	ldl	rr2,|_stkseg+38|(r15)
	cpb	@rr2,#%2D
	jr	eq,La2a2
	testb	@rr2
	jr	eq,La2a2
	ldl	rr4,rr2
	ldl	|_stkseg+106|(r15),rr2
	ldb	rl6,@rr4
	ldb	|_stkseg+111|(r15),rl6
	ldl	rr4,|_stkseg+106|(r15)
	ldb	rl7,|_stkseg+115|(r15)
	ldb	@rr4,rl7
	inc	|_stkseg+108|(r15),#1
	jr	La296
La276:
	ldl	rr2,|_stkseg+106|(r15)
	ldb	rl4,@rr2
	ldb	|_stkseg+113|(r15),rl4
	ldl	rr2,|_stkseg+106|(r15)
	ldb	rl5,|_stkseg+111|(r15)
	ldb	@rr2,rl5
	inc	|_stkseg+108|(r15),#1
	ldb	rl5,|_stkseg+113|(r15)
	ldb	|_stkseg+111|(r15),rl5
La296:
	testb	|_stkseg+111|(r15)
	jr	ne,La276
	ldl	rr2,|_stkseg+106|(r15)
	clrb	@rr2
La2a2:
	test	|_stkseg+124|(r15)
	jr	ne,La354
	cp	r11,#%0065
	jr	eq,La2b4
	cp	r11,#%0045
	jr	ne,La300
La2b4:
	ldl	rr2,|_stkseg+38|(r15)
	ldl	|_stkseg+106|(r15),rr2
	jr	La2d2
La2be:
	ldl	rr2,|_stkseg+106|(r15)
	ldb	rl4,rr2(#%0001)
	ldb	@rr2,rl4
	inc	|_stkseg+108|(r15),#1
	jr	La2f6
La2ce:
	inc	|_stkseg+108|(r15),#1
La2d2:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jp	eq,La3ee
	cpb	@rr2,#%2E
	jr	ne,La2ce
	inc	|_stkseg+108|(r15),#1
	ldl	rr2,|_stkseg+106|(r15)
	cpb	@rr2,#%65
	jr	ne,La2ce
	dec	|_stkseg+108|(r15),#1
La2f6:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	ne,La2be
	jr	La3ee
La300:
	cp	r11,#%0067
	jr	eq,La30c
	cp	r11,#%0047
	jr	ne,La3ee
La30c:
	ldl	rr2,|_stkseg+38|(r15)
	ldl	|_stkseg+106|(r15),rr2
	jr	La32a
La316:
	ldl	rr6,|_stkseg+116|(r15)
	ldl	rr4,|_stkseg+106|(r15)
	call	_strcpy
	jr	La3ee
La326:
	inc	|_stkseg+108|(r15),#1
La32a:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	eq,La3ee
	cpb	@rr2,#%2E
	jr	ne,La326
	ldl	rr4,rr2
	ldl	|_stkseg+116|(r15),rr2
La33e:
	inc	|_stkseg+108|(r15),#1
	ldl	rr2,|_stkseg+106|(r15)
	cpb	@rr2,#%30
	jr	eq,La33e
	cpb	@rr2,#%65
	jr	eq,La316
	jr	La3ee
La354:
	cp	r11,#%0066
	jr	ne,La38c
	ldl	rr2,|_stkseg+38|(r15)
	ldl	|_stkseg+106|(r15),rr2
	jr	La372
La364:
	ldl	rr2,|_stkseg+106|(r15)
	cpb	@rr2,#%2E
	jr	eq,La37a
	inc	|_stkseg+108|(r15),#1
La372:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	ne,La364
La37a:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	ne,La3ee
	ldb	@rr2,#%2E
	inc	|_stkseg+108|(r15),#1
	jr	La3e8
La38c:
	cp	r11,#%0067
	jr	eq,La398
	cp	r11,#%0047
	jr	ne,La3ee
La398:
	ldl	rr2,|_stkseg+38|(r15)
	ldl	|_stkseg+106|(r15),rr2
	jr	La3b0
La3a2:
	ldl	rr2,|_stkseg+106|(r15)
	cpb	@rr2,#%2E
	jr	eq,La3b8
	inc	|_stkseg+108|(r15),#1
La3b0:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	ne,La3a2
La3b8:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	ne,La3ee
	ldb	@rr2,#%2E
	inc	|_stkseg+108|(r15),#1
	clr	|_stkseg+130|(r15)
	jr	La3de
La3ce:
	ldl	rr4,|_stkseg+106|(r15)
	ldb	@rr4,#%30
	inc	|_stkseg+108|(r15),#1
	inc	|_stkseg+130|(r15),#1
La3de:
	ld	r2,|_stkseg+130|(r15)
	cp	r2,|_stkseg+30|(r15)
	jr	lt,La3ce
La3e8:
	ldl	rr2,|_stkseg+106|(r15)
	clrb	@rr2
La3ee:
	cp	r11,#%0045
	jr	eq,La3fa
	cp	r11,#%0047
	jr	ne,La414
La3fa:
	ldl	rr2,|_stkseg+38|(r15)
	ldl	|_stkseg+106|(r15),rr2
La402:
	ldl	rr2,|_stkseg+106|(r15)
	testb	@rr2
	jr	eq,La414
	cpb	@rr2,#%65
	jr	ne,La424
	ldb	@rr2,#%45
La414:
	ldl	rr6,|_stkseg+38|(r15)
	call	__length
	jp	La120
La424:
	inc	|_stkseg+108|(r15),#1
	jr	La402
La42a:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jp	lt,L9c04
	jp	L9bec
La446:
	clr	|_stkseg+122|(r15)
La44a:
	test	|_stkseg+120|(r15)
	jr	eq,La494
	jr	La4f2
La452:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,La47e
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+135|(r15)
	ldb	@rr4,rl2
	extsb	r2
	jr	La490
La47e:
	ldb	rl7,|_stkseg+135|(r15)
	extsb	r7
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
La490:
	inc	|_stkseg+132|(r15),#1
La494:
	ld	r2,|_stkseg+122|(r15)
	dec	|_stkseg+122|(r15),#1
	test	r2
	jr	ne,La452
	jr	La4f2
La4a2:
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,La4d4
	ldl	rr2,|_stkseg+38|(r15)
	inc	|_stkseg+40|(r15),#1
	ldl	rr4,_stkseg+160(r15)
	inc	r5,#2
	inc	@rr4,#1
	dec	r5,#2
	ldl	rr6,@rr4
	dec	r7,#1
	ldb	rl4,@rr2
	ldb	@rr6,rl4
	extsb	r4
	jr	La4ee
La4d4:
	ldl	rr2,|_stkseg+38|(r15)
	inc	|_stkseg+40|(r15),#1
	ldb	rl7,@rr2
	extsb	r7
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
	ld	r4,r2
La4ee:
	inc	|_stkseg+132|(r15),#1
La4f2:
	ld	r2,r9
	dec	r9,#1
	test	r2
	jr	ne,La4a2
	test	|_stkseg+120|(r15)
	jp	eq,L9c16
La504:
	ld	r2,|_stkseg+122|(r15)
	dec	|_stkseg+122|(r15),#1
	test	r2
	jp	eq,L9c16
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	lt,La540
	ldl	rr2,_stkseg+160(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+135|(r15)
	ldb	@rr4,rl2
	extsb	r2
	jr	La552
La540:
	ldb	rl7,|_stkseg+135|(r15)
	extsb	r7
	ldl	rr4,_stkseg+160(r15)
	call	__flsbuf
La552:
	inc	|_stkseg+132|(r15),#1
	jr	La504
La558:
	ldl	rr2,__pfile
	testl	rr2
	jr	eq,La578
	ldl	rr4,rr2
	ldb	rl6,rr4(#%000A)
	extsb	r6
	and	r6,#%0020
	test	r6
	jr	eq,La578
	ld	r2,#%FFFF
	jr	La57c
La578:
	ld	r2,|_stkseg+132|(r15)
La57c:
	ldm	r9,_stkseg+140(r15),#5
	add	r15,#%0096
	ret
La58a:
__atoip::
	sub	r15,#%001E
	ldm	_stkseg+22(r15),r10,#4
	ldm	|_stkseg+0|(r15),r2,#4
	ldl	rr12,rr6
	clr	|_stkseg+16|(r15)
	ld	r11,r5
	test	r5
	jr	eq,La5ba
	cpb	@rr12,#%2D
	jr	ne,La5ba
	dec	|_stkseg+6|(r15),#1
	inc	r13,#1
	ld	|_stkseg+16|(r15),#%0001
La5ba:
	ldk	r10,#0
	jr	La5e8
La5be:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl4,@rr2
	ldb	|_stkseg+19|(r15),rl4
	cpb	rl4,#%30
	jr	lt,La5ec
	cpb	rl4,#%39
	jr	gt,La5ec
	ld	r3,r10
	mult	rr2,#%000A
	ldb	rl2,rl4
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r10,r3
	dec	r11,#1
La5e8:
	test	r11
	jr	ne,La5be
La5ec:
	test	|_stkseg+16|(r15)
	jr	eq,La5f8
	ld	r2,r10
	neg	r2
	jr	La5fa
La5f8:
	ld	r2,r10
La5fa:
	ldl	rr4,|_stkseg+0|(r15)
	ld	@rr4,r2
	ld	r2,|_stkseg+6|(r15)
	sub	r2,r11
	ldm	r10,_stkseg+22(r15),#4
	add	r15,#%001E
	ret
	ret
La616:
	ret
	ret
	.psec data
	.data
D8f32:
	.byte	%28
	.byte	%6e
	.byte	%75
	.byte	%6c
	.byte	%6c
	.byte	%29
	.byte	%00
	.byte	%00
D8f3a:
	.byte	%00
	.byte	%45
	.byte	%00
	.byte	%47
	.byte	%00
	.byte	%65
	.byte	%00
	.byte	%66
	.byte	%00
	.byte	%67
	.byte	%00
	.byte	%73
D8f46:
	.byte	%00
	.byte	%44
	.byte	%00
	.byte	%45
	.byte	%00
	.byte	%47
	.byte	%00
	.byte	%4f
	.byte	%00
	.byte	%55
	.byte	%00
	.byte	%58
	.byte	%00
	.byte	%63
	.byte	%00
	.byte	%64
	.byte	%00
	.byte	%65
	.byte	%00
	.byte	%66
	.byte	%00
	.byte	%67
	.byte	%00
	.byte	%6c
	.byte	%00
	.byte	%6f
	.byte	%00
	.byte	%73
	.byte	%00
	.byte	%75
	.byte	%00
	.byte	%78
D8f66:
	.long	L9c8c
	.long	L9cda
	.long	L9cda
	.long	L9c9a
	.long	L9cda
	.long	L9cda
	.long	L9cda
	.long	L9cda
	.long	L9cda
	.long	L9cda
	.long	L9cda
	.long	L9c7e
	.long	L9cda
	.long	L9cc6
	.long	L9e1c
	.long	L9e1c
	.long	L9e1c
	.long	L9e1c
	.long	L9e1c
D8fb2:
	.long	L9df0
	.long	L9ed0
	.long	La1dc
	.long	La1dc
	.long	L9e66
	.long	L9ec8
	.long	L9e6e
	.long	La1a0
	.long	L9faa
	.long	La1dc
	.long	La1dc
	.long	La1dc
	.long	L9ed8
	.long	L9e24
	.long	La128
	.long	L9e34
D8ff2:
	.long	L9e2c
