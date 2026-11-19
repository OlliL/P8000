	.seg
	.extern	_stkseg
	.extern	__filbuf
	.extern	_ungetc
	.extern	__ctype
	.extern	_atofd
	.extern	_ecvt
	.extern	_strcpy
	.extern	__cnvt
	.code

__fltuse::
	ret
__inflt::
	sub	r15,#%009C
	ldm	_stkseg+144(r15),r8,#6
	ldm	|_stkseg+0|(r15),r2,#6
	ldl	rr0,#%BF000010
	add	r1,r15
	ldl	rr12,rr0
	clr	|_stkseg+14|(r15)
	ldk	r8,#0
	ld	r9,r8
	cp	r5,#%007E
	jr	le,L9bf0
	ld	|_stkseg+6|(r15),#%007E
L9bf0:
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9c1a
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9c24
L9c1a:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9c24:
	ldb	|_stkseg+13|(r15),rl2
	cpb	rl2,#%20
	jr	eq,L9bf0
	cpb	rl2,#%09
	jr	eq,L9bf0
	cpb	rl2,#%0A
	jr	eq,L9bf0
	cpb	rl2,#%2D
	jr	ne,L9c82
	ldb	@rr12,rl2
	inc	r13,#1
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9c6e
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9c78
L9c6e:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9c78:
	ldb	|_stkseg+13|(r15),rl2
	dec	|_stkseg+6|(r15),#1
	jr	L9cc6
L9c82:
	cpb	|_stkseg+13|(r15),#%2B
	jr	ne,L9cc6
	dec	|_stkseg+6|(r15),#1
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9cb8
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9cc2
L9cb8:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9cc2:
	ldb	|_stkseg+13|(r15),rl2
L9cc6:
	cpb	|_stkseg+13|(r15),#%49
	jr	eq,L9cda
	cpb	|_stkseg+13|(r15),#%4E
	jp	ne,L9e50
L9cda:
	cpb	|_stkseg+13|(r15),#%49
	jr	ne,L9cea
	ldl	rr2,#D7500
	jr	L9cf0
L9cea:
	ldl	rr2,#D7509
L9cf0:
	ldl	rr10,rr2
	jr	L9d48
L9cf4:
	testb	@rr10
	jr	eq,L9d5e
	ldb	rl2,@rr10
	cpb	rl2,|_stkseg+13|(r15)
	jr	eq,L9d06
	cpb	@rr10,#%7E
	jr	ne,L9d5e
L9d06:
	ldb	rl2,|_stkseg+13|(r15)
	ldb	@rr12,rl2
	inc	r13,#1
	inc	r11,#1
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9d3a
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9d44
L9d3a:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9d44:
	ldb	|_stkseg+13|(r15),rl2
L9d48:
	dec	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0000
	jr	lt,L9d5e
	ldb	rl2,rr12(#%FFFF)
	cpb	rl2,#%29
	jr	ne,L9cf4
L9d5e:
	cpb	|_stkseg+13|(r15),#%FF
	jp	eq,L9e72
	ldb	rl7,|_stkseg+13|(r15)
	extsb	r7
	ldl	rr4,|_stkseg+0|(r15)
	call	_ungetc
	ldl	rr2,_stkseg+160(r15)
	clr	@rr2
	jr	L9e7c
L9d84:
	test	|_stkseg+14|(r15)
	jr	ne,L9dcc
	test	r8
	jr	ne,L9dcc
	inc	|_stkseg+14|(r15),#1
L9d92:
	ldb	rl2,|_stkseg+13|(r15)
	ldb	@rr12,rl2
	inc	r13,#1
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9e42
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9e4c
L9dc4:
	cpb	|_stkseg+13|(r15),#%2E
	jr	eq,L9d84
L9dcc:
	cpb	|_stkseg+13|(r15),#%65
	jr	eq,L9ddc
	cpb	|_stkseg+13|(r15),#%45
	jr	ne,L9d5e
L9ddc:
	test	r8
	jr	ne,L9d5e
	test	r9
	jr	eq,L9d5e
	inc	r8,#1
	ldb	rl2,|_stkseg+13|(r15)
	ldb	@rr12,rl2
	inc	r13,#1
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,L9e18
	ldl	rr2,|_stkseg+0|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	L9e22
L9e18:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9e22:
	ldb	|_stkseg+13|(r15),rl2
	cpb	rl2,#%2B
	jr	eq,L9d92
	cpb	rl2,#%2D
	jr	eq,L9d92
	cpb	rl2,#%30
	jr	lt,L9d5e
	extsb	r2
	cp	r2,#%0039
	jr	le,L9d92
	jr	L9d5e
L9e42:
	ldl	rr6,|_stkseg+0|(r15)
	call	__filbuf
L9e4c:
	ldb	|_stkseg+13|(r15),rl2
L9e50:
	dec	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0000
	jr	lt,L9d5e
	ldb	rl2,|_stkseg+13|(r15)
	extsb	r2
	ldb	rl2,__ctype+1(r2)
	extsb	r2
	bit	r2,#2
	jr	eq,L9dc4
	inc	r9,#1
	jr	L9d92
L9e72:
	ldl	rr2,_stkseg+160(r15)
	ld	@rr2,#%0001
L9e7c:
	ldl	rr2,|_stkseg+8|(r15)
	testl	rr2
	jr	eq,L9e90
	ldl	rr2,#%BF000010
	add	r3,r15
	cpl	rr12,rr2
	jr	ne,L9e94
L9e90:
	sub	r2,r2
	jr	L9ef0
L9e94:
	clrb	@rr12
	inc	r13,#1
	ld	r2,|_stkseg+4|(r15)
	test	r2
	jr	eq,L9eb0
	cp	r2,#%0001
	jr	eq,L9eb0
	cp	r2,#%0002
	jr	eq,L9ed0
L9eac:
	ldk	r2,#1
	jr	L9ef0
L9eb0:
	ldl	rr6,#%BF000010
	add	r7,r15
	call	_atofd
	.word	%8F08,%8403	// EPU
	ldl	rr2,|_stkseg+8|(r15)
	ldl	rr0,@rr2
	ldl	rr2,rr0
	.word	%0F2C,%8001	// EPU
	jr	L9eac
L9ed0:
	ldl	rr6,#%BF000010
	add	r7,r15
	call	_atofd
	.word	%8F08,%8403	// EPU
	ldl	rr2,|_stkseg+8|(r15)
	ldl	rr0,@rr2
	ldl	rr2,rr0
	.word	%0F2C,%8003	// EPU
	jr	L9eac
L9ef0:
	ldm	r8,_stkseg+144(r15),#6
	add	r15,#%009C
	ret
_gcvt::
	dec	r15,#16
	ld	|_stkseg+6|(r15),r3
	ldm	|_stkseg+8|(r15),r4,#4
	ldl	rr0,_stkseg+20(r15)
	ldl	|_stkseg+0|(r15),rr0
	ld	r2,#%0067
	ld	|_stkseg+4|(r15),r2
	ldm	r4,|_stkseg+8|(r15),#4
	ld	r3,|_stkseg+6|(r15)
	calr	L9f5e
	inc	r15,#16
	ret
_fegcvt::
	dec	r15,#16
	ld	|_stkseg+6|(r15),r3
	ldm	|_stkseg+8|(r15),r4,#4
	ldl	rr0,_stkseg+20(r15)
	ldl	|_stkseg+0|(r15),rr0
	ldb	rl2,_stkseg+25(r15)
	extsb	r2
	ld	|_stkseg+4|(r15),r2
	ldm	r4,|_stkseg+8|(r15),#4
	ld	r3,|_stkseg+6|(r15)
	calr	L9f5e
	inc	r15,#16
	ret
L9f5e:
__fegcvt::
	sub	r15,#%0022
	ldm	_stkseg+22(r15),r8,#6
	ld	|_stkseg+8|(r15),r3
	ldm	|_stkseg+10|(r15),r4,#4
	ldl	rr12,_stkseg+38(r15)
	cp	r3,#%0000
	jr	ge,L9f86
	ld	|_stkseg+8|(r15),#%0006
L9f86:
	cpb	_stkseg+43(r15),#%66
	jr	ne,L9f98
	ld	r2,|_stkseg+8|(r15)
	neg	r2
	jr	L9f9e
L9f98:
	ld	r2,|_stkseg+8|(r15)
	inc	r2,#1
L9f9e:
	ld	r8,r2
	ldl	rr2,#%BF000012
	add	r3,r15
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr2,#%BF000014
	add	r3,r15
	ldl	|_stkseg+4|(r15),rr2
	ldm	r4,|_stkseg+10|(r15),#4
	ld	r3,r8
	call	_ecvt
	ldl	rr10,rr2
	test	|_stkseg+20|(r15)
	jr	eq,L9fd4
	ldb	@rr12,#%2D
	inc	r13,#1
L9fd4:
	cpb	@rr10,#%39
	jr	le,L9fea
	ldl	rr6,rr12
	ldl	rr4,rr10
	call	_strcpy
	jp	La15c
L9fea:
	cpb	_stkseg+43(r15),#%67
	jr	ne,La05c
	ld	r2,|_stkseg+8|(r15)
	neg	r2
	ld	r1,r2
	exts	rr0
	div	rr0,#%0002
	cp	r1,|_stkseg+18|(r15)
	jr	gt,La05c
	cp	|_stkseg+8|(r15),#%0006
	jr	ge,La014
	ldk	r2,#6
	jr	La018
La014:
	ld	r2,|_stkseg+8|(r15)
La018:
	cp	r2,|_stkseg+18|(r15)
	jr	lt,La05c
	ld	r2,|_stkseg+18|(r15)
	add	r2,|_stkseg+8|(r15)
	cp	r2,#%0013
	jr	gt,La05c
	ld	r2,|_stkseg+8|(r15)
	neg	r2
	ld	r8,r2
	ldl	rr2,#%BF000012
	add	r3,r15
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr2,#%BF000014
	add	r3,r15
	ldl	|_stkseg+4|(r15),rr2
	ldm	r4,|_stkseg+10|(r15),#4
	ld	r3,r8
	call	_ecvt
	ldl	rr10,rr2
La05c:
	cpb	@rr10,#%30
	jr	ne,La0b2
	ldb	rl2,rr10(#%0001)
	testb	rl2
	jr	ne,La0b2
	cpb	_stkseg+43(r15),#%67
	jr	ne,La08e
	ldk	r8,#0
La076:
	ldk	r9,#1
La078:
	cp	r9,#%0000
	jr	le,La0de
	testb	@rr10
	jr	eq,La0de
	ldb	rl2,@rr10
	ldb	@rr12,rl2
	inc	r11,#1
	inc	r13,#1
	dec	r9,#1
	jr	La078
La08e:
	cpb	_stkseg+43(r15),#%66
	jr	ne,La0a2
	ldk	r2,#1
	sub	r2,|_stkseg+8|(r15)
	ld	r9,r2
	jr	La0bc
La0a2:
	ldk	r2,#1
	sub	r2,|_stkseg+8|(r15)
	ld	r9,r2
	ld	|_stkseg+18|(r15),#%0001
	jr	La0bc
La0b2:
	cp	r8,#%0000
	jr	gt,La076
	ld	r9,|_stkseg+18|(r15)
La0bc:
	cp	r9,#%0000
	jr	gt,La078
	ldb	@rr12,#%30
	inc	r13,#1
	ldb	@rr12,#%2E
	inc	r13,#1
La0ce:
	cp	r9,#%0000
	jr	ge,La100
	ldb	@rr12,#%30
	inc	r13,#1
	inc	r9,#1
	jr	La0ce
La0de:
	cp	r9,#%0000
	jr	le,La0ee
	ldb	@rr12,#%30
	inc	r13,#1
	dec	r9,#1
	jr	La0de
La0ee:
	ldb	@rr12,#%2E
	inc	r13,#1
	jr	La100
La0f6:
	ldb	rl2,@rr10
	ldb	@rr12,rl2
	inc	r11,#1
	inc	r13,#1
	inc	r9,#1
La100:
	testb	@rr10
	jr	ne,La0f6
	cp	r8,#%0000
	jr	le,La130
	ldb	@rr12,#%65
	inc	r13,#1
	dec	|_stkseg+18|(r15),#1
	ld	r3,|_stkseg+18|(r15)
	exts	rr2
	ld	r5,#%FFF6
	ldl	rr6,rr12
	call	__cnvt
	ld	r9,r2
	ldl	rr2,rr12
	add	r3,r9
	clrb	@rr2
	jr	La15c
La130:
	cp	r8,#%0000
	jr	ge,La158
	clrb	@rr12
	cpb	_stkseg+43(r15),#%67
	jr	eq,La146
	jr	La15c
La144:
	clrb	@rr12
La146:
	dec	r13,#1
	ldb	rl2,@rr12
	cpb	rl2,#%30
	jr	eq,La144
	cpb	@rr12,#%2E
	jr	eq,La15a
	jr	La15c
La158:
	dec	r13,#1
La15a:
	clrb	@rr12
La15c:
	ldl	rr2,_stkseg+38(r15)
	ldm	r8,_stkseg+22(r15),#6
	add	r15,#%0022
	ret

	.psec data
	.data
D7500:
	.byte	%49
	.byte	%4e
	.byte	%46
	.byte	%49
	.byte	%4e
	.byte	%49
	.byte	%54
	.byte	%59
	.byte	%00
D7509:
	.byte	%4e
	.byte	%41
	.byte	%4e
	.byte	%28
	.byte	%7e
	.byte	%7e
	.byte	%7e
	.byte	%7e
	.byte	%7e
	.byte	%7e
	.byte	%29
	.byte	%00
	.byte	%00
