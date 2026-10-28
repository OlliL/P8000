	.seg
	.extern	_stkseg
	.extern	__doprnt
	.extern	__flsbuf
	.code

_printf::
	sub	r15,#%0060
	.word	%4FFC,%8303,%7F2E	// EPU
	.word	%4FFC,%8203,%7F26	// EPU
	.word	%4FFC,%8103,%7F1E	// EPU
	.word	%4FFC,%8003,%7F16	// EPU
	ldm	|_stkseg+10|(r15),r2,#6
	ld	|_stkseg+54|(r15),r5
	ld	|_stkseg+56|(r15),r4
	ld	|_stkseg+58|(r15),r3
	ld	|_stkseg+60|(r15),r2
	.word	%4FFC,%8303,%7F3E	// EPU
	.word	%4FFC,%8203,%7F46	// EPU
	.word	%4FFC,%8103,%7F4E	// EPU
	.word	%4FFC,%8003,%7F56	// EPU
	ldl	rr0,#%0100194E
	ldl	<<1>>%90E4,rr0
	ldl	rr0,#%7F000064
	add	r1,r15
	ldl	|_stkseg+0|(r15),rr0
	ldk	r0,#1
	ld	|_stkseg+4|(r15),r0
	ldl	rr0,#%0100194E
	ldl	|_stkseg+6|(r15),rr0
	ldl	rr4,#%7F000036
	add	r5,r15
	ldl	rr2,#%7F00003E
	add	r3,r15
	call	__doprnt
	ld	<<1>>%90E2,r2
	add	r15,#%0060
	ret
_fprintf::
	sub	r15,#%005C
	.word	%4FFC,%8303,%7F2E	// EPU
	.word	%4FFC,%8203,%7F26	// EPU
	.word	%4FFC,%8103,%7F1E	// EPU
	.word	%4FFC,%8003,%7F16	// EPU
	ldm	|_stkseg+10|(r15),r2,#6
	ld	|_stkseg+54|(r15),r3
	ld	|_stkseg+56|(r15),r2
	.word	%4FFC,%8303,%7F3A	// EPU
	.word	%4FFC,%8203,%7F42	// EPU
	.word	%4FFC,%8103,%7F4A	// EPU
	.word	%4FFC,%8003,%7F52	// EPU
	ldl	<<1>>%90E4,rr6
	ldl	rr0,#%7F000060
	add	r1,r15
	ldl	|_stkseg+0|(r15),rr0
	ldk	r0,#3
	ld	|_stkseg+4|(r15),r0
	ldl	|_stkseg+6|(r15),rr6
	ldl	rr4,#%7F000036
	add	r5,r15
	ldl	rr2,#%7F00003A
	add	r3,r15
	ldl	rr6,|_stkseg+14|(r15)
	call	__doprnt
	ld	<<1>>%90E2,r2
	add	r15,#%005C
	ret
_sprintf::
	sub	r15,#%0068
	.word	%4FFC,%8303,%7F2E	// EPU
	.word	%4FFC,%8203,%7F26	// EPU
	.word	%4FFC,%8103,%7F1E	// EPU
	.word	%4FFC,%8003,%7F16	// EPU
	ldm	|_stkseg+10|(r15),r2,#6
	ldb	|_stkseg+64|(r15),#%42
	ldl	rr0,|_stkseg+18|(r15)
	ldl	|_stkseg+54|(r15),rr0
	ld	|_stkseg+58|(r15),#%7FFF
	ld	|_stkseg+66|(r15),r3
	ld	|_stkseg+68|(r15),r2
	.word	%4FFC,%8303,%7F46	// EPU
	.word	%4FFC,%8203,%7F4E	// EPU
	.word	%4FFC,%8103,%7F56	// EPU
	.word	%4FFC,%8003,%7F5E	// EPU
	ldl	rr0,#%7F000036
	add	r1,r15
	ldl	<<1>>%90E4,rr0
	ldl	rr0,#%7F00006C
	add	r1,r15
	ldl	|_stkseg+0|(r15),rr0
	ldk	r0,#3
	ld	|_stkseg+4|(r15),r0
	ldl	rr0,#%7F000036
	add	r1,r15
	ldl	|_stkseg+6|(r15),rr0
	ldl	rr4,#%7F000042
	add	r5,r15
	ldl	rr2,#%7F000046
	add	r3,r15
	ldl	rr6,|_stkseg+14|(r15)
	call	__doprnt
	ld	<<1>>%90E2,r2
	dec	|_stkseg+58|(r15),#1
	cp	|_stkseg+58|(r15),#%0000
	jr	ge,L7ba2
	ldl	rr4,#%7F000036
	add	r5,r15
	sub	r7,r7
	call	__flsbuf
	jr	L7bb0
L7ba2:
	ldl	rr2,|_stkseg+54|(r15)
	inc	|_stkseg+56|(r15),#1
	clrb	@rr2
	subb	rl2,rl2
	subb	rh2,rh2
L7bb0:
	ld	r2,<<1>>%90E2
	add	r15,#%0068
	ret
