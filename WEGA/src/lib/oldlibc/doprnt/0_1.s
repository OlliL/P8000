	.extern	__cnvt, __flsbuf, __fegcvt, __length
	.psec
	.code
__align::
	dec	r15,#8
	ldm	@r15,r5,#3
	ld	r4,r7
	test	@r4
	jr	eq,L0012
	cp	@r7,#%0002
	jr	ne,L0020
L0012:
	ld	r2,%0004(r15)
	inc	@r2,#1
	ld	r2,%0002(r15)
	inc	@r2,#2
	jr	L0038
L0020:
	ld	r2,%0004(r15)
	cp	@r2,#%0004
	jr	ne,L0038
	inc	@r2,#2
	ld	r2,%0002(r15)
	ld	r3,%0000(r15)
	ld	@r2,r3
	jr	L005c
L0038:
	ld	r2,%0002(r15)
	ld	r3,@r2
	ld	r4,@r3
	ld	%0006(r15),r4
	ld	r3,@r2
	ld	r5,@r2
	ld	r6,%0002(r3)
	ld	@r5,r6
	ld	r2,%0002(r15)
	ld	r3,@r2
	ld	r5,%0006(r15)
	ld	%0002(r3),r5
L005c:
	inc	r15,#8
	ret
__doprnt::
	sub	r15,#%0082
	ldm	%007A(r15),r11,#4
	ldm	%000E(r15),r2,#5
	ld	r14,r7
	clr	%001A(r15)
	ld	%001C(r15),r6
	call	__length
	ld	r12,r2
	jr	L00b6
L0082:
	ld	r2,r13
	ld	r3,%000E(r15)
	ld	r4,@r3
	ldb	@r4,rl2
	extsb	r2
	ld	@r15,r2
L0090:
	ld	r3,%000E(r15)
	inc	@r3,#1
	jr	L00b6
L0098:
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0082
	ld	r7,r13
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	@r15,r2
L00b6:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L05d6
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r13,r3
	cp	r3,#%0025
	jr	ne,L0098
	test	r12
	jp	eq,L05d6
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
	clr	%0026(r15)
	ld	%0028(r15),#%FFFF
	ldb	%0077(r15),#%20
	clr	%0072(r15)
	cp	r2,#%002D
	jr	ne,L0110
	inc	%0072(r15),#1
	ld	r2,r12
	dec	r12,#1
	test	r2
	jp	eq,L05d6
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L0110:
	cp	r13,#%0061
	jr	ge,L01c2
	cp	r13,#%002A
	jr	ne,L0140
	ld	r2,%001C(r15)
	ld	r3,@r2
	ld	%0026(r15),r3
	inc	%001C(r15),#2
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jr	ne,L0160
	ld	r4,%0012(r15)
	ld	%001C(r15),r4
	jr	L0160
L0140:
	cp	r13,#%0030
	jr	ne,L014c
	ldb	%0077(r15),#%30
L014c:
	lda	r5,%0026(r15)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L0160:
	test	r12
	jp	eq,L05d6
	cpb	@r14,#%2E
	jr	ne,L01b2
	inc	r14,#1
	dec	r12,#1
	test	r12
	jp	eq,L05d6
	cpb	@r14,#%2A
	jr	ne,L01a2
	ld	r2,%001C(r15)
	ld	r3,@r2
	ld	%0028(r15),r3
	inc	%001C(r15),#2
	ldk	r11,#1
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jr	ne,L01ae
	ld	r4,%0012(r15)
	ld	%001C(r15),r4
	jr	L01ae
L01a2:
	lda	r5,%0028(r15)
	ld	r6,r12
	ld	r7,r14
	calr	__atoip
	ld	r11,r2
L01ae:
	sub	r12,r11
	add	r14,r11
L01b2:
	test	r12
	jp	eq,L05d6
	dec	r12,#1
	ldk	r2,#0
	ldb	rl2,@r14
	ld	r13,r2
	inc	r14,#1
L01c2:
	cp	%0028(r15),#%0000
	jr	ge,L01e2
	ld	r2,r13
	sub	r2,#%0065
	cp	r2,#%000E
	jr	ugt,L01e2
	rl	r2,#1
	ld	r2,D0024(r2)
	jp	@r2
L01de:
	clr	%0028(r15)
L01e2:
	ld	%0078(r15),r13
	ld	r2,r13
	ld	r3,#D0008
	ld	r4,#%000E
	cpir	r2,@r3,r4,eq
	jp	ne,L049c
	add	r4,r4
	neg	r4
	ld	r2,D005c(r4)
	jp	@r2
L0202:
	ld	%0028(r15),#%0006
	jr	L01e2
L020a:
	ld	%002A(r15),#%0008
	jr	L0220
L0212:
	ld	%002A(r15),#%0010
	jr	L0220
L021a:
	ld	%002A(r15),#%000A
L0220:
	ld	r3,%001C(r15)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	%002C(r15),rr6
	inc	%001C(r15),#2
	clr	%002C(r15)
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jp	ne,L0396
	ld	r4,%0012(r15)
	ld	%001C(r15),r4
	jp	L0396
L0250:
	test	r12
	jr	eq,L032c
	dec	r12,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r3
	subb	rh4,rh4
	ld	r13,r4
	cp	r4,#%006F
	jr	ne,L02b6
L0266:
	ld	%002A(r15),#%0008
	jr	L0284
L026e:
	ld	%002A(r15),#%0010
	jr	L0284
L0276:
	ld	%002A(r15),#%FFF6
	jr	L0284
L027e:
	ld	%002A(r15),#%000A
L0284:
	cp	%0010(r15),#%0005
	jr	ge,L029a
	lda	r7,%0010(r15)
	lda	r6,%001C(r15)
	ld	r5,%0012(r15)
	calr	__align
L029a:
	ld	r2,%001C(r15)
	ldl	rr4,@r2
	ldl	%002C(r15),rr4
	inc	%001C(r15),#4
	inc	%0010(r15),#2
	cp	%0010(r15),#%0005
	jr	eq,L038e
	jr	L0396
L02b6:
	cp	r13,#%0078
	jr	eq,L026e
	cp	r13,#%0064
	jr	eq,L0276
	cp	r13,#%0075
	jr	eq,L027e
	ld	r3,%000E(r15)
	dec	%0002(r3),#1
	ld	r4,%0002(r3)
	cp	r4,#%0000
	jr	lt,L02f4
	ld	r3,%000E(r15)
	ld	r4,@r3
	ldb	@r4,#%6C
	ldb	rl4,#%6C
	extsb	r4
	ld	%0002(r15),r4
	ld	r3,%000E(r15)
	inc	@r3,#1
	jr	L0304
L02f4:
	ld	r7,#%006C
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%0002(r15),r2
L0304:
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L035a
	ld	r2,r13
	ld	r3,%000E(r15)
	ld	r4,@r3
	ldb	@r4,rl2
	extsb	r2
	ld	%0004(r15),r2
	ld	r3,%000E(r15)
	inc	@r3,#1
L032c:
	test	%0028(r15)
	jr	eq,L0344
	cp	%0078(r15),#%0073
	jr	ne,L0344
	cp	r11,%0028(r15)
	jr	le,L0344
	ld	r11,%0028(r15)
L0344:
	cp	r11,%0026(r15)
	jp	ge,L04d4
	ld	r2,%0026(r15)
	sub	r2,r11
	ld	%0074(r15),r2
	jp	L04d8
L035a:
	ld	r7,r13
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%0004(r15),r2
	jr	L032c
L036a:
	ld	%002A(r15),#%FFF6
	ld	r2,%001C(r15)
	ld	r3,@r2
	ld	r5,r3
	exts	rr4
	ldl	%002C(r15),rr4
	inc	%001C(r15),#2
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jr	ne,L0396
L038e:
	ld	r3,%0012(r15)
	ld	%001C(r15),r3
L0396:
	lda	r2,%0032(r15)
	ld	%0030(r15),r2
	ld	r7,r2
	ld	r6,%002A(r15)
	ldl	rr4,%002C(r15)
	call	__cnvt
L03ac:
	ld	r11,r2
	jr	L032c
L03b0:
	ldb	%0077(r15),#%20
	ld	r2,%001C(r15)
	ld	r3,@r2
	ld	%0030(r15),r3
	inc	%001C(r15),#2
	test	r3
	jr	ne,L03ce
	ld	%0030(r15),#D0000
L03ce:
	ld	r7,%0030(r15)
	call	__length
	ld	r11,r2
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jr	ne,L032c
	ld	r2,%0012(r15)
	ld	%001C(r15),r2
	jr	L032c
L03ee:
	ldb	%0077(r15),#%20
	ldk	r11,#1
	ld	r2,%001C(r15)
	ld	%0030(r15),r2
	inc	%001C(r15),#2
	inc	%0030(r15),#1
	inc	%0010(r15),#1
	cp	%0010(r15),#%0005
	jr	ne,L032c
	ld	r3,%0012(r15)
	ld	%001C(r15),r3
	jr	L032c
L041c:
	cp	%0010(r15),#%0005
	jr	ge,L0460
	cp	%001A(r15),#%0004
	jr	ge,L0446
	ld	r2,%001C(r15)
	ld	%001E(r15),r2
	ld	r3,%0014(r15)
	ld	%001C(r15),r3
	inc	%0014(r15),#8
	inc	%001A(r15),#1
	jr	L0464
L0446:
	cp	%001A(r15),#%0004
	jr	ne,L0464
	clr	%001E(r15)
	ld	r2,%0012(r15)
	ld	%001C(r15),r2
	inc	%0010(r15),#5
	jr	L0464
L0460:
	clr	%001E(r15)
L0464:
	lda	r6,%0032(r15)
	ld	r7,%001C(r15)
	.word	%0F74,%8303
	ld	r7,%0028(r15)
	ld	r5,r13
	call	__fegcvt
	ld	%0030(r15),r2
	test	%001E(r15)
	jr	eq,L048e
	ld	r2,%001E(r15)
	ld	%001C(r15),r2
	jr	L0492
L048e:
	inc	%001C(r15),#8
L0492:
	ld	r7,%0030(r15)
	call	__length
	jr	L03ac
L049c:
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	lt,L04c2
	ld	r2,r13
	ld	r3,%000E(r15)
	ld	r4,@r3
	ldb	@r4,rl2
	extsb	r2
	ld	%0006(r15),r2
	jp	L0090
L04c2:
	ld	r7,r13
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%0006(r15),r2
	jp	L00b6
L04d4:
	clr	%0074(r15)
L04d8:
	test	%0072(r15)
	jr	eq,L051e
	jr	L0578
L04e0:
	ld	r2,%000E(r15)
	ld	r3,@r2
	ldb	rl4,%0077(r15)
	ldb	@r3,rl4
	extsb	r4
	ld	%0008(r15),r4
	ld	r2,%000E(r15)
	inc	@r2,#1
	jr	L051e
L04fa:
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L04e0
	ldb	rl7,%0077(r15)
	extsb	r7
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%0008(r15),r2
L051e:
	ld	r2,%0074(r15)
	dec	%0074(r15),#1
	test	r2
	jr	ne,L04fa
	jr	L0578
L052c:
	ld	r2,%0030(r15)
	ld	r3,%000E(r15)
	ld	r4,@r3
	ldb	rl5,@r2
	ldb	@r4,rl5
	extsb	r5
	ld	%000A(r15),r5
	inc	%0030(r15),#1
	ld	r3,%000E(r15)
	inc	@r3,#1
	jr	L0578
L054c:
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L052c
	ld	r7,%0030(r15)
	inc	%0030(r15),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%000A(r15),r2
L0578:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L054c
	test	%0072(r15)
	jr	ne,L05a2
	jp	L00b6
L058a:
	ld	r2,%000E(r15)
	ld	r3,@r2
	ldb	rl4,%0077(r15)
	ldb	@r3,rl4
	extsb	r4
	ld	%000C(r15),r4
	ld	r2,%000E(r15)
	inc	@r2,#1
L05a2:
	ld	r2,%0074(r15)
	dec	%0074(r15),#1
	test	r2
	jp	eq,L00b6
	ld	r2,%000E(r15)
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L058a
	ldb	rl7,%0077(r15)
	extsb	r7
	ld	r6,%000E(r15)
	call	__flsbuf
	ld	%000C(r15),r2
	jr	L05a2
L05d6:
	ldm	r11,%007A(r15),#4
	add	r15,#%0082
	ret
__atoip::
	sub	r15,#%0018
	ldm	%0002(r15),r5,#2
	ld	r1,r7
	clr	%000C(r15)
	ld	r4,r6
	test	r6
	jr	eq,L060a
	cpb	@r1,#%2D
	jr	ne,L060a
	dec	%0004(r15),#1
	inc	r1,#1
	ld	%000C(r15),#%0001
L060a:
	ldk	r5,#0
	jr	L063a
L060e:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	ldb	%000F(r15),rl3
	cpb	rl3,#%30
	jr	lt,L063e
	cpb	rl3,#%39
	jr	gt,L063e
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,%000F(r15)
	extsb	r2
	sub	r2,#%0030
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L063a:
	test	r4
	jr	ne,L060e
L063e:
	test	%000C(r15)
	jr	eq,L064c
	ld	r2,r5
	neg	r2
	ld	@r15,r2
	jr	L064e
L064c:
	ld	@r15,r5
L064e:
	ld	r2,%0002(r15)
	ld	r3,@r15
	ld	@r2,r3
	ld	r2,%0004(r15)
	sub	r2,r4
	add	r15,#%0018
	ret
	.psec
	.data
D0000:
	.byte	%28,%6e,%75,%6c,%6c,%29,%00,%00
D0008:
	.byte	%00,%44,%00,%4f,%00,%55,%00,%58
	.byte	%00,%63,%00,%64,%00,%65,%00,%66
	.byte	%00,%67,%00,%6c,%00,%6f,%00,%73
	.byte	%00,%75,%00,%78
D0024:
	.addr	L0202
	.addr	L0202
	.addr	L0202
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01e2
	.addr	L01de
	.addr	L0276
	.addr	L0266
	.addr	L027e
	.addr	L026e
	.addr	L03ee
	.addr	L036a
	.addr	L041c
	.addr	L041c
	.addr	L041c
	.addr	L0250
	.addr	L020a
	.addr	L03b0
	.addr	L021a
D005c:
	.addr	L0212
