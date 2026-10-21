	.extern	__doprnt, __flsbuf, __iob
	.psec
	.code
_printf::
	sub	r15,#%0016
	ldm	%0000(r15),r2,#6
	ld	%000C(r15),r6
	ld	%000E(r15),r5
	ld	%0010(r15),r4
	ld	%0012(r15),r3
	ld	%0014(r15),r2
	lda	r6,%000C(r15)
	lda	r5,%0018(r15)
	sub	r4,r4
	ld	r3,#__iob+8
	call	__doprnt
	ldb	rl2,__iob+14
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L0044
	ld	r2,#%FFFF
	jr	L0046
L0044:
	sub	r2,r2
L0046:
	add	r15,#%0016
	ret
_fprintf::
	sub	r15,#%0014
	ldm	%0000(r15),r2,#6
	ld	%000C(r15),r5
	ld	%000E(r15),r4
	ld	%0010(r15),r3
	ld	%0012(r15),r2
	lda	r6,%000C(r15)
	lda	r5,%0016(r15)
	ld	r7,%0008(r15)
	ldk	r4,#1
	ld	r3,%000A(r15)
	call	__doprnt
	ld	r2,%000A(r15)
	ldb	rl2,%0006(r2)
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L0094
	ld	r2,#%FFFF
	jr	L0096
L0094:
	sub	r2,r2
L0096:
	add	r15,#%0014
	ret
_sprintf::
	sub	r15,#%001C
	ldm	%0000(r15),r2,#6
	ldb	%0012(r15),#%42
	ld	%000C(r15),r7
	ld	%000E(r15),#%7FFF
	ld	%0014(r15),r5
	ld	%0016(r15),r4
	ld	%0018(r15),r3
	ld	%001A(r15),r2
	lda	r6,%0014(r15)
	lda	r5,%001E(r15)
	lda	r3,%000C(r15)
	ldk	r4,#1
	ld	r7,%0008(r15)
	call	__doprnt
	dec	%000E(r15),#1
	cp	%000E(r15),#%0000
	jr	lt,L00f8
	ld	r2,%000C(r15)
	inc	%000C(r15),#1
	clrb	@r2
	subb	rl2,rl2
	extsb	r2
	jr	L0102
L00f8:
	lda	r6,%000C(r15)
	sub	r7,r7
	call	__flsbuf
L0102:
	ld	r2,%000A(r15)
	add	r15,#%001C
	ret
