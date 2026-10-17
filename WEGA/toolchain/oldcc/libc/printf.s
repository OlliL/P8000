// printf.o: magic E711 nonsegmented
	.comm	2, _rc
	.extern	_printf
	.extern	__pfile
	.extern	__iob
	.extern	__doprnt
	.extern	_fprintf
	.extern	_sprintf
	.extern	__flsbuf

// segment table entry 0 (segment 0), text, 288 bytes
	.psec
	.code
_printf::
	sub	r15,#%001E
	ldm	%0018(r15),r12,#3
	ldm	@r15,r2,#5
	ld	r14,r7
	ldk	r12,#0
	ld	%000C(r15),r6
	ld	%000E(r15),r5
	ld	%0010(r15),r4
	ld	%0012(r15),r3
	ld	%0014(r15),r2
	ld	__pfile,#__iob+8
	lda	r6,%000C(r15)
	lda	r5,%0020(r15)
	sub	r4,r4
	ld	r3,#__iob+8
	call	__doprnt
	ld	_rc,r2
	ldm	r12,%0018(r15),#3
	add	r15,#%001E
	ret
_fprintf::
	sub	r15,#%001E
	ldm	%0018(r15),r12,#3
	ldm	@r15,r2,#6
	ld	r14,r6
	ldk	r12,#0
	ld	%000C(r15),r5
	ld	%000E(r15),r4
	ld	%0010(r15),r3
	ld	%0012(r15),r2
	ld	__pfile,r7
	lda	r6,%000C(r15)
	lda	r5,%0020(r15)
	ld	r7,r14
	ldk	r4,#1
	ld	r3,%000A(r15)
	call	__doprnt
	ld	_rc,r2
	ldm	r12,%0018(r15),#3
	add	r15,#%001E
	ret
_sprintf::
	sub	r15,#%002A
	ldm	%0024(r15),r12,#3
	ldm	@r15,r2,#6
	ld	r14,r6
	ldk	r12,#0
	ld	%0018(r15),r5
	ld	%001A(r15),r4
	ld	%001C(r15),r3
	ld	%001E(r15),r2
	ldb	%0012(r15),#%42
	ld	%000C(r15),r7
	ld	%000E(r15),#%7FFF
	lda	r6,%000C(r15)
	ld	__pfile,r6
	lda	r6,%0018(r15)
	lda	r5,%002C(r15)
	lda	r3,%000C(r15)
	ldk	r4,#1
	ld	r7,r14
	call	__doprnt
	ld	_rc,r2
	dec	%000E(r15),#1
	cp	%000E(r15),#%0000
	jr	lt,L0t_0106
	ld	r2,%000C(r15)
	inc	%000C(r15),#1
	clrb	@r2
	subb	rl2,rl2
	extsb	r2
	jr	L0t_0110
L0t_0106:
	lda	r6,%000C(r15)
	sub	r7,r7
	call	__flsbuf
L0t_0110:
	ld	r2,_rc
	ldm	r12,%0024(r15),#3
	add	r15,#%002A
	ret
