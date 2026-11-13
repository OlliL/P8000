// allprint.s.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_allprint
	.extern	_stkseg
	.extern	_yyout
	.extern	_fprintf
	.extern	__flsbuf
	.extern	_sprint
	.extern	_printable
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 262 bytes
	.psec
	.code
_allprint::
	dec	r15,#2
	ld	|_stkseg|(r15),r7
	extsb	r7
	ld	r6,r7
	ldl	rr2,#L1d_0000
	ld	r4,#%0004
	cpir	r6,@rr2,r4,eq
	jr	ne,L0t_0066
	sll	r4,#2
	neg	r4
	ldl	rr2,L1d_0014(r4)
	jp	@rr2
L0t_0028:
	ldl	rr6,_yyout
	ldl	rr4,#L1d_0018
L0t_0034:
	call	_fprintf
	jr	L0t_00c4
L0t_003c:
	ldl	rr6,_yyout
	ldl	rr4,#L1d_0018+3
	jr	L0t_0034
L0t_004a:
	ldl	rr6,_yyout
	ldl	rr4,#L1d_001e
	jr	L0t_0034
L0t_0058:
	ldl	rr6,_yyout
	ldl	rr4,#L1d_001e+3
	jr	L0t_0034
L0t_0066:
	ldb	rl7,|_stkseg+1|(r15)
	extsb	r7
	calr	_printable
	test	r2
	jr	ne,L0t_00a0
	ldl	rr6,_yyout
	ldl	rr4,#L1d_001e+7
	ldb	rl3,|_stkseg+1|(r15)
	extsb	r3
	jr	L0t_0034
L0t_0086:
	ldl	rr2,_yyout
	inc	r3,#2
	inc	@rr2,#1
	dec	r3,#2
	ldl	rr4,@rr2
	dec	r5,#1
	ldb	rl2,|_stkseg+1|(r15)
	ldb	@rr4,rl2
	extsb	r2
	jr	L0t_00c4
L0t_00a0:
	ldl	rr2,_yyout
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,L0t_0086
	ldb	rl7,|_stkseg+1|(r15)
	extsb	r7
	ldl	rr4,_yyout
	call	__flsbuf
L0t_00c4:
	inc	r15,#2
	ret
_sprint::
	dec	r15,#4
	ldl	|_stkseg|(r15),rr6
	jr	L0t_00de
L0t_00d0:
	ldl	rr4,|_stkseg|(r15)
	inc	|_stkseg+2|(r15),#1
	ldb	rl7,@rr4
	extsb	r7
	calr	_allprint
L0t_00de:
	ldl	rr2,|_stkseg|(r15)
	testb	@rr2
	jr	ne,L0t_00d0
	inc	r15,#4
	ret
_printable::
	dec	r15,#4
	ld	|_stkseg|(r15),r7
	cp	r7,#%0020
	jr	le,L0t_0100
	cp	r7,#%007F
	jr	ge,L0t_0100
	ldk	r2,#1
	jr	L0t_0102
L0t_0100:
	ldk	r2,#0
L0t_0102:
	inc	r15,#4
	ret

// segment table entry 1 (segment 1), data, 44 bytes
	.psec	data
	.data
L1d_0000:
	.word	%0008
	.word	%0009
	.word	%000A
	.word	%0020
	.long	L0t_004a
	.long	L0t_003c
	.long	L0t_0028
L1d_0014:
	.long	L0t_0058
L1d_0018:
	.word	%5C6E
	.word	%005C
	.word	%7400
L1d_001e:
	.word	%5C62
	.word	%005C
	.word	%0862
	.word	%005C
	.word	%252D
	.word	%336F
	.word	%0000
