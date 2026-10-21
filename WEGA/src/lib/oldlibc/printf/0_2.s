	.extern	__doprnt, __flsbuf, __iob
	.psec
	.code
_printf::
	sub	r15,#%0058
	.word	%4FFC,%8303,%0024
	.word	%4FFC,%8203,%001C
	.word	%4FFC,%8103,%0014
	.word	%4FFC,%8003,%000C
	ldm	@r15,r2,#6
	ld	%002C(r15),r6
	ld	%002E(r15),r5
	ld	%0030(r15),r4
	ld	%0032(r15),r3
	ld	%0034(r15),r2
	.word	%4FFC,%8303,%0036
	.word	%4FFC,%8203,%003E
	.word	%4FFC,%8103,%0046
	.word	%4FFC,%8003,%004E
	lda	r6,%002C(r15)
	lda	r5,%0036(r15)
	lda	r4,%005A(r15)
	sub	r3,r3
	ld	r2,#__iob+8
	call	__doprnt
	ldb	rl2,__iob+14
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L0076
	ld	r2,#%FFFF
	jr	L0078
L0076:
	sub	r2,r2
L0078:
	add	r15,#%0058
	ret
_fprintf::
	sub	r15,#%0056
	.word	%4FFC,%8303,%0024
	.word	%4FFC,%8203,%001C
	.word	%4FFC,%8103,%0014
	.word	%4FFC,%8003,%000C
	ldm	@r15,r2,#6
	ld	%002C(r15),r5
	ld	%002E(r15),r4
	ld	%0030(r15),r3
	ld	%0032(r15),r2
	.word	%4FFC,%8303,%0034
	.word	%4FFC,%8203,%003C
	.word	%4FFC,%8103,%0044
	.word	%4FFC,%8003,%004C
	lda	r6,%002C(r15)
	lda	r5,%0034(r15)
	lda	r4,%0058(r15)
	ld	r7,%0008(r15)
	ldk	r3,#1
	ld	r2,%000A(r15)
	call	__doprnt
	ld	r2,%000A(r15)
	ldb	rl3,%0006(r2)
	extsb	r3
	and	r3,#%0020
	test	r3
	jr	eq,L00f8
	ld	r2,#%FFFF
	jr	L00fa
L00f8:
	sub	r2,r2
L00fa:
	add	r15,#%0056
	ret
_sprintf::
	sub	r15,#%005E
	.word	%4FFC,%8303,%0024
	.word	%4FFC,%8203,%001C
	.word	%4FFC,%8103,%0014
	.word	%4FFC,%8003,%000C
	ldm	@r15,r2,#6
	ldb	%0032(r15),#%42
	ld	%002C(r15),r7
	ld	%002E(r15),#%7FFF
	ld	%0034(r15),r5
	ld	%0036(r15),r4
	ld	%0038(r15),r3
	ld	%003A(r15),r2
	.word	%4FFC,%8303,%003C
	.word	%4FFC,%8203,%0044
	.word	%4FFC,%8103,%004C
	.word	%4FFC,%8003,%0054
	lda	r6,%0034(r15)
	lda	r5,%003C(r15)
	lda	r4,%0060(r15)
	lda	r2,%002C(r15)
	ldk	r3,#1
	ld	r7,%0008(r15)
	call	__doprnt
	dec	%002E(r15),#1
	cp	%002E(r15),#%0000
	jr	lt,L018e
	ld	r2,%002C(r15)
	inc	%002C(r15),#1
	clrb	@r2
	subb	rl2,rl2
	extsb	r2
	jr	L0198
L018e:
	lda	r6,%002C(r15)
	sub	r7,r7
	call	__flsbuf
L0198:
	ld	r2,%000A(r15)
	add	r15,#%005E
	ret
