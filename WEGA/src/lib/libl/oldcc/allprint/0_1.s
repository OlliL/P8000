// allprint.o: magic E711 nonsegmented
	.extern	_allprint
	.extern	_yyout
	.extern	_fprintf
	.extern	__flsbuf
	.extern	_sprint
	.extern	_printable

// segment table entry 0 (segment 0), text, 220 bytes
	.psec
	.code
_allprint::
	dec	r15,#2
	ld	@r15,r7
	extsb	r7
	ld	r6,r7
	ld	r2,#L0d_0000
	ld	r3,#%0004
	cpir	r6,@r2,r3,eq
	jr	ne,L0t_004c
	add	r3,r3
	neg	r3
	ld	r6,L0d_000e(r3)
	jp	@r6
L0t_0020:
	ld	r7,_yyout
	ld	r6,#L0d_0010
L0t_0028:
	call	_fprintf
	jr	L0t_009c
L0t_002e:
	ld	r7,_yyout
	ld	r6,#L0d_0010+3
	jr	L0t_0028
L0t_0038:
	ld	r7,_yyout
	ld	r6,#L0d_0016
	jr	L0t_0028
L0t_0042:
	ld	r7,_yyout
	ld	r6,#L0d_0016+3
	jr	L0t_0028
L0t_004c:
	ldb	rl7,%0001(r15)
	extsb	r7
	calr	_printable
	test	r2
	jr	ne,L0t_007c
	ld	r7,_yyout
	ld	r6,#L0d_0016+7
	ldb	rl5,%0001(r15)
	extsb	r5
	jr	L0t_0028
L0t_0068:
	ld	r2,_yyout
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,%0001(r15)
	ldb	@r3,rl2
	extsb	r2
	jr	L0t_009c
L0t_007c:
	ld	r2,_yyout
	dec	%0002(r2),#1
	ld	r3,%0002(r2)
	cp	r3,#%0000
	jr	ge,L0t_0068
	ldb	rl7,%0001(r15)
	extsb	r7
	ld	r6,_yyout
	call	__flsbuf
L0t_009c:
	inc	r15,#2
	ret
_sprint::
	dec	r15,#2
	ld	@r15,r7
	jr	L0t_00b6
L0t_00a6:
	ld	r7,%0000(r15)
	inc	%0000(r15),#1
	ldb	rl3,@r7
	ldb	rl7,rl3
	extsb	r7
	calr	_allprint
L0t_00b6:
	ld	r2,%0000(r15)
	testb	@r2
	jr	ne,L0t_00a6
	inc	r15,#2
	ret
_printable::
	dec	r15,#4
	ld	@r15,r7
	cp	r7,#%0020
	jr	le,L0t_00d6
	cp	r7,#%007F
	jr	ge,L0t_00d6
	ldk	r2,#1
	jr	L0t_00d8
L0t_00d6:
	ldk	r2,#0
L0t_00d8:
	inc	r15,#4
	ret

// segment table entry 0 (segment 0), data, 36 bytes
	.psec
	.data
L0d_0000:
	.word	%0008
	.word	%0009
	.word	%000A
	.word	%0020
	.word	L0t_0038
	.word	L0t_002e
	.word	L0t_0020
L0d_000e:
	.word	L0t_0042
L0d_0010:
	.word	%5C6E
	.word	%005C
	.word	%7400
L0d_0016:
	.word	%5C62
	.word	%005C
	.word	%0862
	.word	%005C
	.word	%252D
	.word	%336F
	.word	%0000
