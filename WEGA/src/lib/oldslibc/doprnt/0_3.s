	.seg
	.extern	_stkseg
	.extern	__length
	.extern	__flsbuf
	.extern	__cnvt
	.extern	__fegcvt
	.code

Laa8a:
__strout::
	jr	Laafc
Laa8c:
	ldl	rr0,rr6
	ldl	rr2,rr0
	cp	@rr2,#%0000
	jr	eq,Laa9e
	ldl	rr2,rr6
	cp	@rr2,#%0002
	jr	ne,Laaac
Laa9e:
	ldl	rr2,rr6
	inc	@rr2,#1
	ldl	rr2,rr4
	inc	r3,#2
	inc	@rr2,#2
	dec	r3,#2
	jr	Laac2
Laaac:
	ldl	rr2,rr6
	cp	@rr2,#%0004
	jr	ne,Laac2
	ldl	rr2,rr6
	inc	@rr2,#2
	ldl	rr2,rr4
	ldl	rr0,|_stkseg+0|(r15)
	ldl	@rr2,rr0
	jr	Laaf8
Laac2:
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr0,@rr2
	ldl	rr2,rr0
	ld	r0,@rr2
	ld	|_stkseg+12|(r15),r0
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr0,@rr2
	inc	r1,#2
	ldl	rr2,rr0
	ldl	rr0,|_stkseg+4|(r15)
	ldl	rr4,rr0
	ldl	rr0,@rr4
	ldl	rr4,rr0
	ld	r0,@rr2
	ld	@rr4,r0
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr0,@rr2
	inc	r1,#2
	ldl	rr2,rr0
	ld	r0,|_stkseg+12|(r15)
	ld	@rr2,r0
Laaf8:
	inc	r15,#14
	ret
Laafc:
__align::
	dec	r15,#14
	ldm	|_stkseg+0|(r15),r2,#4
	jr	Laa8c
__doprnt::
	jp	Lb204
Lab0c:
	ldl	rr12,rr6
	ldl	rr2,|_stkseg+10|(r15)
	ldl	|_stkseg+14|(r15),rr2
	ldl	rr6,rr12
	call	__length
	ld	r10,r2
Lab20:
	ld	r2,r10
	dec	r10,#1
	cp	r2,#%0000
	jp	eq,Lb1f6
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl2,@rr2
	subb	rh2,rh2
	ld	r11,r2
	cp	r2,#%0025
	jr	eq,Lab7c
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lab6a
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ld	r0,r11
	ldb	@rr2,rl0
	extsb	r0
	jr	Lab7a
Lab6a:
	ld	r7,r11
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lab7a:
	jr	Lab20
Lab7c:
	cp	r10,#%0000
	jr	ne,Lab88
	jp	Lb1f6
Lab88:
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
	clr	|_stkseg+18|(r15)
	ld	|_stkseg+20|(r15),#%FFFF
	ldb	|_stkseg+101|(r15),#%20
	clr	|_stkseg+96|(r15)
	cp	r11,#%002D
	jr	ne,Labc8
	inc	|_stkseg+96|(r15),#1
	ld	r2,r10
	dec	r10,#1
	cp	r2,#%0000
	jr	ne,Labc0
	jp	Lb1f6
Labc0:
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
Labc8:
	cp	r11,#%0061
	jr	ge,Lacb2
Labce:
	cp	r11,#%002A
	jr	eq,Labe6
	cp	r11,#%0030
	jr	lt,Labe0
	cp	r11,#%0039
	jr	le,Labe6
Labe0:
	cp	r11,#%002E
	jr	ne,Lacb2
Labe6:
	cp	r11,#%002A
	jr	ne,Lac14
	ldl	rr2,|_stkseg+14|(r15)
	ld	r0,@rr2
	ld	|_stkseg+18|(r15),r0
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+118(r15),#1
	cp	_stkseg+118(r15),#%0005
	jr	ne,Lac12
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lac12:
	jr	Lac3c
Lac14:
	cp	r11,#%0030
	jr	ne,Lac20
	ldb	|_stkseg+101|(r15),#%30
Lac20:
	ldl	rr2,#%BF000012
	add	r3,r15
	dec	r13,#1
	ldl	rr6,rr12
	inc	r10,#1
	ld	r5,r10
	calr	Lb21c
	ld	r9,r2
	ld	r3,r13
	add	r3,r9
	ld	r13,r3
	sub	r10,r9
Lac3c:
	cp	r10,#%0000
	jr	ne,Lac44
	jr	Lacb2
Lac44:
	cpb	@rr12,#%2E
	jr	ne,Lac9e
	inc	r13,#1
	dec	r10,#1
	cp	r10,#%0000
	jr	ne,Lac56
	jr	Lacb2
Lac56:
	cpb	@rr12,#%2A
	jr	ne,Lac86
	ldl	rr2,|_stkseg+14|(r15)
	ld	r0,@rr2
	ld	|_stkseg+20|(r15),r0
	inc	|_stkseg+16|(r15),#2
	ldk	r9,#1
	inc	_stkseg+118(r15),#1
	cp	_stkseg+118(r15),#%0005
	jr	ne,Lac84
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lac84:
	jr	Lac96
Lac86:
	ldl	rr2,#%BF000014
	add	r3,r15
	ld	r5,r10
	ldl	rr6,rr12
	calr	Lb21c
	ld	r9,r2
Lac96:
	sub	r10,r9
	ld	r3,r13
	add	r3,r9
	ld	r13,r3
Lac9e:
	cp	r10,#%0000
	jr	ne,Laca6
	jr	Lacb2
Laca6:
	dec	r10,#1
	ldk	r2,#0
	ldb	rl2,@rr12
	ld	r11,r2
	inc	r13,#1
	jr	Labce
Lacb2:
	cp	|_stkseg+20|(r15),#%0000
	jr	ge,Lacec
	ld	r2,r11
	jr	Laccc
Lacbe:
	clr	|_stkseg+20|(r15)
	jr	Lacec
Lacc4:
	ld	|_stkseg+20|(r15),#%0006
	jr	Lacec
Laccc:
	ldl	rr6,#D75bc
	ld	r4,#%0004
	cpir	r2,@rr6,r4,eq
	jr	ne,Lacec
	ld	r2,#%75BE
	sub	r7,r2
	rl	r7,#1
	ldl	rr2,D75c4(r7)
	jp	@rr2
Lacec:
	ld	|_stkseg+102|(r15),r11
	ld	r2,r11
	jp	Lb090
Lacf8:
	ld	|_stkseg+22|(r15),#%0008
	jr	Lad0e
Lad00:
	ld	|_stkseg+22|(r15),#%0010
	jr	Lad0e
Lad08:
	ld	|_stkseg+22|(r15),#%000A
Lad0e:
	ldl	rr2,|_stkseg+14|(r15)
	ld	r2,@rr2
	ld	r1,r2
	exts	rr0
	ldl	|_stkseg+24|(r15),rr0
	inc	|_stkseg+16|(r15),#2
	clr	|_stkseg+24|(r15)
	inc	_stkseg+118(r15),#1
	cp	_stkseg+118(r15),#%0005
	jr	ne,Lad3c
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lad3c:
	jp	Laeaa
Lad42:
	ld	|_stkseg+22|(r15),#%0008
	jr	Lae32
Lad4a:
	ld	|_stkseg+22|(r15),#%0010
	jr	Lae32
Lad52:
	ld	|_stkseg+22|(r15),#%000A
	jr	Lae32
Lad5a:
	ld	|_stkseg+22|(r15),#%FFF6
	jr	Lae32
Lad62:
	cp	r10,#%0000
	jr	ne,Lad6e
	jp	Lb0b0
Lad6e:
	dec	r10,#1
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl2,@rr2
	subb	rh2,rh2
	ld	r11,r2
	cp	r2,#%006F
	jr	ne,Lad88
	ld	|_stkseg+22|(r15),#%0008
	jr	Lae32
Lad88:
	cp	r11,#%0078
	jr	ne,Lad96
	ld	|_stkseg+22|(r15),#%0010
	jr	Lae32
Lad96:
	cp	r11,#%0064
	jr	ne,Lada4
	ld	|_stkseg+22|(r15),#%FFF6
	jr	Lae32
Lada4:
	cp	r11,#%0075
	jr	ne,Ladb2
	ld	|_stkseg+22|(r15),#%000A
	jr	Lae32
Ladb2:
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lade0
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	@rr2,#%6C
	ldb	rl2,#%6C
	extsb	r2
	jr	Ladf0
Lade0:
	ld	r7,#%006C
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
Ladf0:
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lae1c
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ld	r0,r11
	ldb	@rr2,rl0
	extsb	r0
	jr	Lae2c
Lae1c:
	ld	r7,r11
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lae2c:
	jp	Lb0b0
Lae32:
	cp	_stkseg+118(r15),#%0005
	jr	ge,Lae52
	ldl	rr6,#%BF000076
	add	r7,r15
	ldl	rr4,#%BF00000E
	add	r5,r15
	ldl	rr2,|_stkseg+6|(r15)
	calr	Laa8a
Lae52:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr0,@rr2
	ldl	|_stkseg+24|(r15),rr0
	inc	|_stkseg+16|(r15),#4
	inc	_stkseg+118(r15),#2
	cp	_stkseg+118(r15),#%0005
	jr	ne,Lae78
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lae78:
	jr	Laeaa
Lae7a:
	ld	|_stkseg+22|(r15),#%FFF6
	ldl	rr2,|_stkseg+14|(r15)
	ld	r2,@rr2
	ld	r1,r2
	exts	rr0
	ldl	|_stkseg+24|(r15),rr0
	inc	|_stkseg+16|(r15),#2
	inc	_stkseg+118(r15),#1
	cp	_stkseg+118(r15),#%0005
	jr	ne,Laeaa
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Laeaa:
	ldl	rr2,#%BF000020
	add	r3,r15
	ldl	|_stkseg+28|(r15),rr2
	cp	|_stkseg+22|(r15),#%FFF6
	jr	ne,Laf3e
	ldl	rr2,|_stkseg+24|(r15)
	cpl	rr2,#%00000000
	jr	ge,Laf3e
	ldl	rr2,|_stkseg+24|(r15)
	cpl	rr2,#%80000000
	jr	ne,Laedc
	ld	|_stkseg+22|(r15),#%000A
Laedc:
	ldl	rr6,|_stkseg+24|(r15)
	multl	rq4,#%FFFFFFFF
	ldl	|_stkseg+24|(r15),rr6
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Laf18
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	@rr2,#%2D
	ldb	rl2,#%2D
	extsb	r2
	jr	Laf28
Laf18:
	ld	r7,#%002D
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
Laf28:
	ldl	rr6,|_stkseg+28|(r15)
	ld	r5,|_stkseg+22|(r15)
	ldl	rr2,|_stkseg+24|(r15)
	call	__cnvt
	ld	r9,r2
	jr	Laf52
Laf3e:
	ldl	rr6,|_stkseg+28|(r15)
	ld	r5,|_stkseg+22|(r15)
	ldl	rr2,|_stkseg+24|(r15)
	call	__cnvt
	ld	r9,r2
Laf52:
	jp	Lb0b0
Laf58:
	ldb	|_stkseg+101|(r15),#%20
	cp	_stkseg+118(r15),#%0005
	jr	ge,Laf7e
	ldl	rr6,#%BF000076
	add	r7,r15
	ldl	rr4,#%BF00000E
	add	r5,r15
	ldl	rr2,|_stkseg+6|(r15)
	calr	Laa8a
Laf7e:
	ldl	rr2,|_stkseg+14|(r15)
	ldl	rr0,@rr2
	ldl	|_stkseg+28|(r15),rr0
	inc	|_stkseg+16|(r15),#4
	ldl	rr2,|_stkseg+28|(r15)
	cpl	rr2,#%00000000
	jr	ne,Lafa2
	ldl	rr2,#D75b4
	ldl	|_stkseg+28|(r15),rr2
Lafa2:
	ldl	rr6,|_stkseg+28|(r15)
	call	__length
	ld	r9,r2
	inc	_stkseg+118(r15),#2
	cp	_stkseg+118(r15),#%0005
	jr	ne,Lafc6
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lafc6:
	jr	Lb0b0
Lafc8:
	ldb	|_stkseg+101|(r15),#%20
	ldk	r9,#1
	ldl	rr2,|_stkseg+14|(r15)
	ldl	|_stkseg+28|(r15),rr2
	inc	|_stkseg+16|(r15),#2
	inc	|_stkseg+30|(r15),#1
	inc	_stkseg+118(r15),#1
	cp	_stkseg+118(r15),#%0005
	jr	ne,Laff8
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Laff8:
	jr	Lb0b0
Laffa:
	cp	_stkseg+118(r15),#%0005
	jr	ge,Lb00c
	ldl	rr2,|_stkseg+6|(r15)
	ldl	|_stkseg+14|(r15),rr2
Lb00c:
	ldl	rr2,#%BF000020
	add	r3,r15
	ldl	|_stkseg+0|(r15),rr2
	ld	|_stkseg+4|(r15),r11
	ldl	rr2,|_stkseg+14|(r15)
	.word	%0F24,%8003	// EPU
	.word	%8F00,%8403	// EPU
	ld	r3,|_stkseg+20|(r15)
	call	__fegcvt
	ldl	|_stkseg+28|(r15),rr2
	inc	|_stkseg+16|(r15),#8
	inc	_stkseg+118(r15),#5
	ldl	rr6,|_stkseg+28|(r15)
	call	__length
	ld	r9,r2
	jr	Lb0b0
Lb04e:
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lb07a
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ld	r0,r11
	ldb	@rr2,rl0
	extsb	r0
	jr	Lb08a
Lb07a:
	ld	r7,r11
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lb08a:
	jp	Lab20
Lb090:
	ldl	rr6,#D75d4
	ld	r4,#%000E
	cpir	r2,@rr6,r4,eq
	jr	ne,Lb04e
	ld	r2,#%75D6
	sub	r7,r2
	rl	r7,#1
	ldl	rr2,D75f0(r7)
	jp	@rr2
Lb0b0:
	cp	|_stkseg+20|(r15),#%0000
	jr	eq,Lb0ca
	cp	|_stkseg+102|(r15),#%0073
	jr	ne,Lb0ca
	cp	r9,|_stkseg+20|(r15)
	jr	le,Lb0ca
	ld	r9,|_stkseg+20|(r15)
Lb0ca:
	cp	r9,|_stkseg+18|(r15)
	jr	ge,Lb0dc
	ld	r2,|_stkseg+18|(r15)
	sub	r2,r9
	ld	|_stkseg+98|(r15),r2
	jr	Lb0e0
Lb0dc:
	clr	|_stkseg+98|(r15)
Lb0e0:
	cp	|_stkseg+96|(r15),#%0000
	jr	ne,Lb13a
Lb0e8:
	ld	r2,|_stkseg+98|(r15)
	dec	|_stkseg+98|(r15),#1
	cp	r2,#%0000
	jr	eq,Lb13a
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lb124
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl0,|_stkseg+101|(r15)
	ldb	@rr2,rl0
	extsb	r0
	jr	Lb138
Lb124:
	ldb	rl7,|_stkseg+101|(r15)
	extsb	r7
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lb138:
	jr	Lb0e8
Lb13a:
	ld	r2,r9
	dec	r9,#1
	cp	r2,#%0000
	jr	eq,Lb196
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lb17a
	ldl	rr2,|_stkseg+28|(r15)
	inc	|_stkseg+30|(r15),#1
	ldl	rr0,_stkseg+120(r15)
	ldl	rr6,rr0
	inc	r7,#2
	inc	@rr6,#1
	dec	r7,#2
	ldl	rr0,@rr6
	dec	r1,#1
	ldl	rr6,rr0
	ldb	rl0,@rr2
	ldb	@rr6,rl0
	extsb	r0
	jr	Lb194
Lb17a:
	ldl	rr2,|_stkseg+28|(r15)
	inc	|_stkseg+30|(r15),#1
	ldb	rl7,@rr2
	extsb	r7
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lb194:
	jr	Lb13a
Lb196:
	cp	|_stkseg+96|(r15),#%0000
	jr	eq,Lb1f0
Lb19e:
	ld	r2,|_stkseg+98|(r15)
	dec	|_stkseg+98|(r15),#1
	cp	r2,#%0000
	jr	eq,Lb1f0
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,Lb1da
	ldl	rr2,_stkseg+120(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl0,|_stkseg+101|(r15)
	ldb	@rr2,rl0
	extsb	r0
	jr	Lb1ee
Lb1da:
	ldb	rl7,|_stkseg+101|(r15)
	extsb	r7
	ldl	rr4,_stkseg+120(r15)
	call	__flsbuf
	ld	r0,r2
Lb1ee:
	jr	Lb19e
Lb1f0:
	jp	Lab20
Lb1f6:
	ldm	r9,_stkseg+104(r15),#5
	add	r15,#%0072
	ret
Lb204:
	sub	r15,#%0072
	ldm	_stkseg+104(r15),r9,#5
	ldm	|_stkseg+6|(r15),r2,#4
	jp	Lab0c
Lb21c:
	jr	Lb2a6
Lb21e:
	ldl	rr12,rr6
	clr	|_stkseg+8|(r15)
	ld	r11,r5
	cp	r5,#%0000
	jr	eq,Lb23e
	cpb	@rr12,#%2D
	jr	ne,Lb23e
	dec	|_stkseg+6|(r15),#1
	inc	r13,#1
	ld	|_stkseg+8|(r15),#%0001
Lb23e:
	ldk	r10,#0
Lb240:
	cp	r11,#%0000
	jr	eq,Lb278
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl0,@rr2
	ldb	|_stkseg+11|(r15),rl0
	cpb	rl0,#%30
	jr	lt,Lb25e
	cpb	|_stkseg+11|(r15),#%39
	jr	le,Lb260
Lb25e:
	jr	Lb278
Lb260:
	ld	r3,r10
	mult	rr2,#%000A
	ldb	rl2,|_stkseg+11|(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r10,r3
	dec	r11,#1
	jr	Lb240
Lb278:
	cp	|_stkseg+8|(r15),#%0000
	jr	eq,Lb286
	ld	r2,r10
	neg	r2
	jr	Lb288
Lb286:
	ld	r2,r10
Lb288:
	ldl	rr0,|_stkseg+0|(r15)
	ldl	rr6,rr0
	ld	@rr6,r2
	ld	r2,|_stkseg+6|(r15)
	sub	r2,r11
	jr	Lb298
Lb298:
	ldm	r10,_stkseg+12(r15),#4
	add	r15,#%0014
	ret
Lb2a6:
	sub	r15,#%0014
	ldm	_stkseg+12(r15),r10,#4
	ldm	|_stkseg+0|(r15),r2,#4
	jr	Lb21e

	.psec data
	.data
D75b4:
	.byte	%28
	.byte	%6e
	.byte	%75
	.byte	%6c
	.byte	%6c
	.byte	%29
	.byte	%00
	.byte	%00
D75bc:
	.byte	%00
	.byte	%65
	.byte	%00
	.byte	%66
	.byte	%00
	.byte	%67
	.byte	%00
	.byte	%73
D75c4:
	.long	Lacc4
	.long	Lacc4
	.long	Lacc4
	.long	Lacbe
D75d4:
	.byte	%00
	.byte	%44
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
D75f0:
	.long	Lad5a
	.long	Lad42
	.long	Lad52
	.long	Lad4a
	.long	Lafc8
	.long	Lae7a
	.long	Laffa
	.long	Laffa
	.long	Laffa
	.long	Lad62
	.long	Lacf8
	.long	Laf58
	.long	Lad08
	.long	Lad00
