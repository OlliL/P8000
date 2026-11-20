	.seg
	.extern	_stkseg
	.extern	__ctype
	.extern	_tolower
	.extern	__filbuf
	.extern	_ungetc
	.extern	__inflt
	.code

__doscan::
	sub	r15,#%0026
	ldm	_stkseg+28(r15),r9,#5
	ldl	|_stkseg+8|(r15),rr6
	ldl	rr12,rr4
	ldl	rr10,rr2
	clr	|_stkseg+12|(r15)
	clr	|_stkseg+22|(r15)
	clr	|_stkseg+26|(r15)
	jp	La418
La28a:
	ld	r2,#%7598
	sub	r7,r2
	rl	r7,#1
	ldl	rr2,D75a0(r7)
	jp	@rr2
La29a:
	ldl	rr2,rr12
	inc	r13,#1
	ldb	rl2,@rr2
	extsb	r2
	ld	r9,r2
	cp	r2,#%0025
	jp	eq,La434
	subl	rr2,rr2
	ldl	|_stkseg+18|(r15),rr2
	cp	r9,#%002A
	jr	eq,La2d4
	ldl	|_stkseg+18|(r15),rr10
	inc	r11,#4
	inc	|_stkseg+26|(r15),#1
	cp	|_stkseg+26|(r15),#%0002
	jr	ne,La2dc
	ldl	rr10,_stkseg+42(r15)
	jr	La2dc
La2d4:
	ldb	rl2,@rr12
	extsb	r2
	ld	r9,r2
	inc	r13,#1
La2dc:
	clr	|_stkseg+14|(r15)
	ld	|_stkseg+24|(r15),#%0001
	jr	La302
La2e8:
	ld	r3,|_stkseg+14|(r15)
	mult	rr2,#%000A
	add	r3,r9
	sub	r3,#%0030
	ld	|_stkseg+14|(r15),r3
	ldb	rl2,@rr12
	extsb	r2
	ld	r9,r2
	inc	r13,#1
La302:
	ldb	rl2,__ctype+1(r9)
	extsb	r2
	bit	r2,#2
	jr	ne,La2e8
	test	|_stkseg+14|(r15)
	jr	ne,La31a
	ld	|_stkseg+14|(r15),#%7530
La31a:
	cp	r9,#%006C
	jr	ne,La330
	ldb	rl2,@rr12
	extsb	r2
	ld	r9,r2
	inc	r13,#1
	ld	|_stkseg+24|(r15),#%0002
	jr	La350
La330:
	cp	r9,#%0068
	jr	ne,La344
	clr	|_stkseg+24|(r15)
	ldb	rl2,@rr12
	extsb	r2
	ld	r9,r2
	inc	r13,#1
	jr	La350
La344:
	cp	r9,#%005B
	jr	ne,La350
	ldl	rr6,rr12
	calr	La8de
	ldl	rr12,rr2
La350:
	ldb	rl2,__ctype+1(r9)
	extsb	r2
	bit	r2,#0
	jr	eq,La36c
	ld	r7,r9
	call	_tolower
	ld	r9,r2
	ld	|_stkseg+24|(r15),#%0002
La36c:
	test	r9
	jr	ne,La37a
La370:
	ld	r2,#%FFFF
	jp	La48c
La37a:
	ldl	rr2,|_stkseg+8|(r15)
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr2,#%BF000016
	add	r3,r15
	ldl	|_stkseg+4|(r15),rr2
	ldl	rr6,|_stkseg+18|(r15)
	ld	r5,r9
	ld	r4,|_stkseg+14|(r15)
	ld	r3,|_stkseg+24|(r15)
	calr	La49a
	test	r2
	jr	eq,La3ae
	ldl	rr2,|_stkseg+18|(r15)
	testl	rr2
	jr	eq,La3ae
	inc	|_stkseg+12|(r15),#1
La3ae:
	test	|_stkseg+22|(r15)
	jr	eq,La418
	test	|_stkseg+12|(r15)
	jr	eq,La370
	jr	La488
La3bc:
	ldl	rr2,|_stkseg+8|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La3e6
	ldl	rr2,|_stkseg+8|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La3f0
La3e6:
	ldl	rr6,|_stkseg+8|(r15)
	call	__filbuf
La3f0:
	ld	|_stkseg+16|(r15),r2
	cp	r2,#%0020
	jr	eq,La3bc
	cp	r2,#%0009
	jr	eq,La3bc
	cp	r2,#%000A
	jr	eq,La3bc
	cp	r2,#%FFFF
	jr	eq,La418
	ld	r7,r2
	ldl	rr4,|_stkseg+8|(r15)
	call	_ungetc
La418:
	ldb	rl2,@rr12
	extsb	r2
	ld	r9,r2
	inc	r13,#1
	ldl	rr6,#D7596
	ld	r4,#%0005
	cpir	r2,@rr6,r4,eq
	jp	eq,La28a
La434:
	ldl	rr2,|_stkseg+8|(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La45e
	ldl	rr2,|_stkseg+8|(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La468
La45e:
	ldl	rr6,|_stkseg+8|(r15)
	call	__filbuf
La468:
	ld	|_stkseg+16|(r15),r2
	cp	r9,|_stkseg+16|(r15)
	jr	eq,La418
	cp	r2,#%FFFF
	jp	eq,La370
	ld	r7,r2
	ldl	rr4,|_stkseg+8|(r15)
	call	_ungetc
La488:
	ld	r2,|_stkseg+12|(r15)
La48c:
	ldm	r9,_stkseg+28(r15),#5
	add	r15,#%0026
	ret
La49a:
__innum::
	sub	r15,#%001C
	ldm	_stkseg+24(r15),r12,#2
	ldm	|_stkseg+4|(r15),r3,#5
	clr	|_stkseg+18|(r15)
	cp	r5,#%0063
	jr	eq,La4c2
	cp	r5,#%0073
	jr	eq,La4c2
	cp	r5,#%005B
	jr	ne,La4f2
La4c2:
	ldl	rr0,_stkseg+36(r15)
	ldl	|_stkseg+0|(r15),rr0
	ldl	rr6,|_stkseg+10|(r15)
	testl	rr6
	jr	eq,La4da
	ldl	rr0,@rr6
	ldl	rr6,rr0
	jr	La4dc
La4da:
	subl	rr6,rr6
La4dc:
	ld	r5,|_stkseg+8|(r15)
	ld	r4,|_stkseg+6|(r15)
	ldl	rr2,_stkseg+32(r15)
	calr	La78c
	jp	La77e
La4f2:
	cp	|_stkseg+8|(r15),#%0065
	jr	eq,La502
	cp	|_stkseg+8|(r15),#%0066
	jr	ne,La52a
La502:
	ldl	rr2,_stkseg+36(r15)
	ldl	|_stkseg+0|(r15),rr2
	ldl	rr6,|_stkseg+10|(r15)
	ld	r5,|_stkseg+6|(r15)
	ld	r4,|_stkseg+4|(r15)
	ldl	rr2,_stkseg+32(r15)
	call	__inflt
	jp	La77e
La52a:
	subl	rr2,rr2
	ldl	|_stkseg+20|(r15),rr2
	ldk	r12,#10
	cp	|_stkseg+8|(r15),#%006F
	jr	ne,La53e
	ldk	r12,#8
	jr	La54a
La53e:
	cp	|_stkseg+8|(r15),#%0078
	jr	ne,La54a
	ld	r12,#%0010
La54a:
	clr	|_stkseg+14|(r15)
La54e:
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La57c
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La588
La57c:
	ldl	rr6,_stkseg+32(r15)
	call	__filbuf
La588:
	ld	r13,r2
	cp	r2,#%0020
	jr	eq,La54e
	cp	r2,#%0009
	jr	eq,La54e
	cp	r2,#%000A
	jr	eq,La54e
	cp	r2,#%002D
	jr	ne,La61e
	inc	|_stkseg+14|(r15),#1
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La5d4
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La5e0
La5d4:
	ldl	rr6,_stkseg+32(r15)
	call	__filbuf
La5e0:
	ld	r13,r2
	dec	|_stkseg+6|(r15),#1
	jr	La664
La5e8:
	test	|_stkseg+14|(r15)
	jr	eq,La5f8
	subl	rr2,rr2
	subl	rr2,|_stkseg+20|(r15)
	ldl	|_stkseg+20|(r15),rr2
La5f8:
	cp	r13,#%FFFF
	jp	eq,La72c
	ld	r7,r13
	ldl	rr4,_stkseg+32(r15)
	call	_ungetc
	ldl	rr2,_stkseg+36(r15)
	clr	@rr2
	jp	La736
La61e:
	cp	r13,#%002B
	jr	ne,La664
	dec	|_stkseg+6|(r15),#1
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La656
La63a:
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr0,@rr2
	dec	r1,#1
	ldl	rr2,rr0
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La662
La656:
	ldl	rr6,_stkseg+32(r15)
	call	__filbuf
La662:
	ld	r13,r2
La664:
	dec	|_stkseg+6|(r15),#1
	cp	|_stkseg+6|(r15),#%0000
	jr	lt,La5e8
	ldb	rl2,__ctype+1(r13)
	extsb	r2
	bit	r2,#2
	jr	ne,La69a
	cp	r12,#%0010
	jr	ne,La5e8
	cp	r13,#%0061
	jr	lt,La68e
	cp	r13,#%0066
	jr	le,La69a
La68e:
	cp	r13,#%0041
	jr	lt,La5e8
	cp	r13,#%0046
	jr	gt,La5e8
La69a:
	ld	|_stkseg+18|(r15),#%0001
	cp	r12,#%0008
	jr	ne,La6b0
	ldl	rr2,|_stkseg+20|(r15)
	ld	r0,#%0003
	jr	La6d4
La6b0:
	cp	r12,#%000A
	jr	ne,La6cc
	ldl	rr2,|_stkseg+20|(r15)
	ld	r0,#%0002
	sdal	rr2,r0
	addl	rr2,|_stkseg+20|(r15)
	ld	r0,#%0001
	jr	La6d4
La6cc:
	ldl	rr2,|_stkseg+20|(r15)
	ld	r0,#%0004
La6d4:
	sdal	rr2,r0
	ldl	|_stkseg+20|(r15),rr2
	ld	|_stkseg+16|(r15),r13
	cp	r13,#%0030
	jr	lt,La6f2
	cp	r13,#%0039
	jr	gt,La6f2
	sub	r13,#%0030
	jr	La708
La6f2:
	cp	r13,#%0061
	jr	lt,La704
	cp	r13,#%0066
	jr	gt,La704
	sub	r13,#%0057
	jr	La708
La704:
	sub	r13,#%0037
La708:
	ld	r3,r13
	exts	rr2
	addl	rr2,|_stkseg+20|(r15)
	ldl	|_stkseg+20|(r15),rr2
	ld	r13,|_stkseg+16|(r15)
	ldl	rr2,_stkseg+32(r15)
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La656
	jr	La63a
La72c:
	ldl	rr2,_stkseg+36(r15)
	ld	@rr2,#%0001
La736:
	ldl	rr2,|_stkseg+10|(r15)
	testl	rr2
	jr	eq,La77c
	ld	r2,|_stkseg+4|(r15)
	test	r2
	jr	eq,La758
	cp	r2,#%0001
	jr	eq,La758
	cp	r2,#%0002
	jr	eq,La768
	jr	La776
La754:
	ldk	r2,#1
	jr	La77e
La758:
	ldl	rr2,|_stkseg+10|(r15)
	ldl	rr0,@rr2
	ldl	rr2,rr0
	ld	r0,|_stkseg+22|(r15)
	ld	@rr2,r0
	jr	La776
La768:
	ldl	rr2,|_stkseg+10|(r15)
	ldl	rr0,@rr2
	ldl	rr2,rr0
	ldl	rr0,|_stkseg+20|(r15)
	ldl	@rr2,rr0
La776:
	test	|_stkseg+18|(r15)
	jr	ne,La754
La77c:
	sub	r2,r2
La77e:
	ldm	r12,_stkseg+24(r15),#2
	add	r15,#%001C
	ret
La78c:
__instr::
	sub	r15,#%0014
	ldm	_stkseg+10(r15),r9,#5
	ldl	|_stkseg+0|(r15),rr4
	ldl	rr12,rr6
	ldl	rr10,rr2
	ldl	rr2,_stkseg+24(r15)
	clr	@rr2
	ldl	|_stkseg+4|(r15),rr12
	cp	r5,#%0063
	jr	ne,La7be
	cp	r4,#%7530
	jr	ne,La7be
	ld	|_stkseg+0|(r15),#%0001
La7be:
	clr	|_stkseg+8|(r15)
	cp	|_stkseg+2|(r15),#%0073
	jr	ne,La7d0
	ld	|_stkseg+8|(r15),#%0001
La7d0:
	ldl	rr2,rr10
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La7f0
	ldl	rr2,@rr10
	inc	r11,#2
	inc	@rr10,#1
	dec	r11,#2
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La7f8
La7f0:
	ldl	rr6,rr10
	call	__filbuf
La7f8:
	ld	r9,r2
	ldb	rl2,D7516(r2)
	extsb	r2
	and	r2,|_stkseg+8|(r15)
	jr	eq,La80e
	cp	r9,#%FFFF
	jr	ne,La7d0
La80e:
	ld	|_stkseg+8|(r15),#%0001
	cp	|_stkseg+2|(r15),#%0063
	jr	ne,La822
	clr	|_stkseg+8|(r15)
	jr	La882
La822:
	cp	|_stkseg+2|(r15),#%005B
	jr	ne,La882
	ld	|_stkseg+8|(r15),#%0002
	jr	La882
La832:
	ldb	rl2,D7516(r9)
	extsb	r2
	and	r2,|_stkseg+8|(r15)
	test	r2
	jr	ne,La888
	testl	rr12
	jr	eq,La84c
	ld	r2,r9
	ldb	@rr12,rl2
	inc	r13,#1
La84c:
	dec	|_stkseg+0|(r15),#1
	cp	|_stkseg+0|(r15),#%0000
	jr	le,La888
	ldl	rr2,rr10
	inc	r3,#4
	dec	@rr2,#1
	ld	r2,@rr2
	cp	r2,#%0000
	jr	lt,La878
	ldl	rr2,@rr10
	inc	r11,#2
	inc	@rr10,#1
	dec	r11,#2
	ldb	rl2,@rr2
	extsb	r2
	and	r2,#%00FF
	jr	La880
La878:
	ldl	rr6,rr10
	call	__filbuf
La880:
	ld	r9,r2
La882:
	cp	r9,#%FFFF
	jr	ne,La832
La888:
	cp	r9,#%FFFF
	jr	eq,La8aa
	cp	|_stkseg+0|(r15),#%0000
	jr	le,La8a0
	ld	r7,r9
	ldl	rr4,rr10
	call	_ungetc
La8a0:
	ldl	rr2,_stkseg+24(r15)
	clr	@rr2
	jr	La8b4
La8aa:
	ldl	rr2,_stkseg+24(r15)
	ld	@rr2,#%0001
La8b4:
	testl	rr12
	jr	eq,La8ce
	cpl	rr12,|_stkseg+4|(r15)
	jr	eq,La8ce
	cp	|_stkseg+2|(r15),#%0063
	jr	eq,La8ca
	clrb	@rr12
	inc	r13,#1
La8ca:
	ldk	r2,#1
	jr	La8d0
La8ce:
	sub	r2,r2
La8d0:
	ldm	r9,_stkseg+10(r15),#5
	add	r15,#%0014
	ret
La8de:
__getccl::
	dec	r15,#8
	ldm	_stkseg+0(r15),r10,#2
	ldl	rr4,rr6
	ldk	r10,#0
	cpb	@rr4,#%5E
	jr	ne,La8f6
	inc	r10,#1
	inc	r5,#1
La8f6:
	ldk	r11,#0
La8f8:
	test	r10
	jr	eq,La904
	resb	D7516(r11),#1
	jr	La90a
La904:
	setb	D7516(r11),#1
La90a:
	inc	r11,#1
	cp	r11,#%0080
	jr	lt,La8f8
La912:
	ldl	rr2,rr4
	inc	r5,#1
	ldb	rl2,@rr2
	extsb	r2
	ld	r11,r2
	and	r2,#%007F
	cp	r2,#%005D
	jr	eq,La940
	test	r10
	jr	eq,La932
	setb	D7516(r11),#1
	jr	La938
La932:
	resb	D7516(r11),#1
La938:
	inc	r11,#1
	test	r11
	jr	ne,La912
	dec	r5,#1
La940:
	ldl	rr2,rr4
	ldm	r10,_stkseg+0(r15),#2
	inc	r15,#8
	ret

	.psec data
	.data
D7516:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%01
	.byte	%01
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%01
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D7596:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%09
	.byte	%00
	.byte	%0a
	.byte	%00
	.byte	%20
	.byte	%00
	.byte	%25
D75a0:
	.addr	La488
	.addr	La3bc
	.addr	La3bc
	.addr	La3bc
	.addr	La29a
