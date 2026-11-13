// nlist.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_a_magic
	.extern	_nlist
	.extern	_stkseg
	.extern	_open
	.extern	_read
	.extern	_close
	.extern	_lseek
	.extern	_readby
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 782 bytes
	.psec
	.code
_nlist::
	sub	r15,#%064A
	ldm	_stkseg+1602(r15),r10,#4
	ldm	|_stkseg|(r15),r4,#4
	clr	_stkseg+1586(r15)
	ldl	rr10,rr4
	jr	L0t_0032
L0t_001c:
	ldl	rr2,rr10
	inc	r3,#8
	clrb	@rr2
	ldl	rr2,rr10
	inc	r3,#4
	subl	rr4,rr4
	ldl	@rr2,rr4
	inc	_stkseg+1586(r15),#1
	inc	r11,#10
L0t_0032:
	ldl	rr2,@rr10
	testb	@rr2
	jr	ne,L0t_001c
	ldl	rr6,|_stkseg+4|(r15)
	sub	r5,r5
	call	_open
	ld	|_stkseg+8|(r15),r2
	cp	r2,#%0000
	jr	ge,L0t_0058
L0t_004e:
	ld	r2,#%FFFF
	jp	L0t_02be
L0t_0058:
	ldl	rr4,#_stkseg+22
	add	r5,r15
	ld	r7,|_stkseg+8|(r15)
	ld	r3,#%0018
	call	_read
	clr	|_stkseg+10|(r15)
	jr	L0t_008a
L0t_0074:
	ld	r2,|_stkseg+10|(r15)
	add	r2,r2
	ld	r3,_a_magic(r2)
	cp	r3,|_stkseg+22|(r15)
	jr	eq,L0t_009a
	inc	|_stkseg+10|(r15),#1
L0t_008a:
	ld	r2,|_stkseg+10|(r15)
	add	r2,r2
	ld	r3,_a_magic(r2)
	test	r3
	jr	ne,L0t_0074
L0t_009a:
	ld	r2,|_stkseg+10|(r15)
	add	r2,r2
	ld	r3,_a_magic(r2)
	test	r3
	jr	ne,L0t_00b6
	ld	r7,|_stkseg+8|(r15)
	call	_close
	jr	L0t_004e
L0t_00b6:
	ldl	rr2,|_stkseg+24|(r15)
	ldl	|_stkseg+18|(r15),rr2
	ld	r4,|_stkseg+40|(r15)
	and	r4,#%0001
	cp	r4,#%0001
	jr	eq,L0t_00d8
	ldl	rr6,rr2
	multl	rq4,#%00000002
	ldl	|_stkseg+18|(r15),rr6
L0t_00d8:
	ld	r2,|_stkseg+32|(r15)
	add	r2,#%0018
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,|_stkseg+18|(r15)
	ldl	|_stkseg+18|(r15),rr4
	ld	r7,|_stkseg+8|(r15)
	sub	r3,r3
	call	_lseek
	ld	r2,|_stkseg+34|(r15)
	ld	|_stkseg+14|(r15),r2
L0t_0100:
	test	|_stkseg+14|(r15)
	jp	eq,L0t_02b2
	ld	|_stkseg+16|(r15),#%0578
	cp	|_stkseg+14|(r15),#%0578
	jr	nc,L0t_0120
	ld	r2,|_stkseg+14|(r15)
	ld	|_stkseg+16|(r15),r2
L0t_0120:
	ldl	rr4,#_stkseg+46
	add	r5,r15
	ld	r7,|_stkseg+8|(r15)
	ld	r3,|_stkseg+16|(r15)
	call	_read
	ld	r2,|_stkseg+14|(r15)
	sub	r2,|_stkseg+16|(r15)
	ld	|_stkseg+14|(r15),r2
	ldl	rr2,#_stkseg+46
	add	r3,r15
	ldl	rr12,rr2
L0t_014c:
	test	|_stkseg+16|(r15)
	jr	eq,L0t_0100
	ldl	rr10,|_stkseg|(r15)
L0t_0156:
	ldl	rr2,@rr10
	testb	@rr2
	jr	eq,L0t_0216
	ldl	rr2,@rr10
	ldl	_stkseg+1592(r15),rr2
	ldl	rr4,rr12
	inc	r5,#6
	ldl	_stkseg+1596(r15),rr4
	ldb	rl6,@rr4
	extsb	r6
	bit	r6,#7
	jr	eq,L0t_018e
	ldb	rl6,@rr4
	extsb	r6
	and	r6,#%007F
	add	r5,r6
	ldl	_stkseg+1588(r15),rr4
	inc	_stkseg+1598(r15),#1
	jr	L0t_019c
L0t_018e:
	ldl	rr2,_stkseg+1596(r15)
	inc	r3,#8
	ldl	_stkseg+1588(r15),rr2
L0t_019c:
	clr	_stkseg+1600(r15)
L0t_01a2:
	ldl	rr2,_stkseg+1592(r15)
	testb	@rr2
	jr	eq,L0t_01da
	ldl	rr4,_stkseg+1596(r15)
	cpl	rr4,_stkseg+1588(r15)
	jr	nc,L0t_01da
	inc	_stkseg+1594(r15),#1
	ldb	rl4,@rr2
	ldl	rr2,_stkseg+1596(r15)
	inc	_stkseg+1598(r15),#1
	ldb	rl5,@rr2
	cpb	rl4,rl5
	jr	eq,L0t_01a2
	inc	_stkseg+1600(r15),#1
L0t_01da:
	ldl	rr2,_stkseg+1596(r15)
	cpl	rr2,_stkseg+1588(r15)
	jr	nc,L0t_01f2
	testb	@rr2
	jr	eq,L0t_01f2
	inc	_stkseg+1600(r15),#1
L0t_01f2:
	test	_stkseg+1600(r15)
	jr	ne,L0t_0220
	ldl	rr2,rr10
	inc	r3,#4
	ldl	rr4,@rr12
	ldl	@rr2,rr4
	ldb	rl2,rr12(#%0004)
	ldl	rr4,rr10
	inc	r5,#8
	ldb	@rr4,rl2
	testb	rl2
	jr	eq,L0t_0216
	dec	_stkseg+1586(r15),#1
L0t_0216:
	test	_stkseg+1586(r15)
	jr	ne,L0t_0224
	jr	L0t_02b2
L0t_0220:
	inc	r11,#10
	jr	L0t_0156
L0t_0224:
	ldl	rr6,rr12
	calr	_readby
	ld	|_stkseg+10|(r15),r2
	ld	r5,r2
	mult	rr4,#%000E
	ld	r4,|_stkseg+16|(r15)
	sub	r4,r5
	ld	|_stkseg+16|(r15),r4
	test	r4
	jr	eq,L0t_014c
	ld	r3,|_stkseg+10|(r15)
	mult	rr2,#%000E
	add	r3,r13
	ld	r13,r3
	ldl	rr6,rr12
	calr	_readby
	ld	|_stkseg+10|(r15),r2
	ld	r5,|_stkseg+16|(r15)
	ldk	r4,#0
	div	rr4,#%000E
	ld	|_stkseg+12|(r15),r5
	cp	r2,r5
	jp	le,L0t_014c
	ldl	rr4,#_stkseg+46
	add	r5,r15
	add	r5,#%0578
	ld	r3,|_stkseg+10|(r15)
	sub	r3,|_stkseg+12|(r15)
	ld	r7,r3
	mult	rr6,#%000E
	ld	|_stkseg+10|(r15),r7
	ld	r3,r7
	ld	r7,|_stkseg+8|(r15)
	call	_read
	ld	r2,|_stkseg+14|(r15)
	sub	r2,|_stkseg+10|(r15)
	ld	|_stkseg+14|(r15),r2
	ld	r2,|_stkseg+16|(r15)
	add	r2,|_stkseg+10|(r15)
	ld	|_stkseg+16|(r15),r2
	jp	L0t_014c
L0t_02b2:
	ld	r7,|_stkseg+8|(r15)
	call	_close
	sub	r2,r2
L0t_02be:
	ldm	r10,_stkseg+1602(r15),#4
	add	r15,#%064A
	ret
_readby::
	dec	r15,#8
	ldl	|_stkseg|(r15),rr6
	ldl	rr2,rr6
	ldb	rl4,rr2(#%0006)
	extsb	r4
	bit	r4,#7
	jr	eq,L0t_0308
	ldb	rl4,rr6(#%0006)
	extsb	r4
	and	r4,#%007F
	dec	r4,#7
	ld	|_stkseg+4|(r15),r4
	dec	r4,#7
	ld	r1,r4
	exts	rr0
	div	rr0,#%000E
	inc	r1,#1
	ld	|_stkseg+4|(r15),r1
	inc	|_stkseg+4|(r15),#1
	ld	r2,|_stkseg+4|(r15)
	jr	L0t_030a
L0t_0308:
	ldk	r2,#1
L0t_030a:
	inc	r15,#8
	ret

// segment table entry 1 (segment 1), data, 10 bytes
	.psec	data
	.data
_a_magic::
	.word	%E607
	.word	%E611
	.word	%E707
	.word	%E711
	.word	%0000
