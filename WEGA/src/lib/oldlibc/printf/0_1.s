	.extern	__doprnt, __flsbuf, __iob
	.psec
	.code
_printf::
	sub	r15,#%005A
	.word	%4FFC,%8303,%0026
	.word	%4FFC,%8203,%001E
	.word	%4FFC,%8103,%0016
	.word	%4FFC,%8003,%000E
	ldm	%0002(r15),r2,#6
	ld	%002E(r15),r6
	ld	%0030(r15),r5
	ld	%0032(r15),r4
	ld	%0034(r15),r3
	ld	%0036(r15),r2
	.word	%4FFC,%8303,%0038
	.word	%4FFC,%8203,%0040
	.word	%4FFC,%8103,%0048
	.word	%4FFC,%8003,%0050
	lda	r6,%002E(r15)
	lda	r5,%0038(r15)
	lda	r4,%005C(r15)
	sub	r3,r3
	ld	r2,#__iob+8
	call	__doprnt
	ldb	rl2,__iob+14
	extsb	r2
	and	r2,#%0020
	test	r2
	jr	eq,L0078
	ld	@r15,#%FFFF
	jr	L007a
L0078:
	clr	@r15
L007a:
	ld	r2,@r15
	add	r15,#%005A
	ret
_fprintf::
	sub	r15,#%0058
	.word	%4FFC,%8303,%0026
	.word	%4FFC,%8203,%001E
	.word	%4FFC,%8103,%0016
	.word	%4FFC,%8003,%000E
	ldm	%0002(r15),r2,#6
	ld	%002E(r15),r5
	ld	%0030(r15),r4
	ld	%0032(r15),r3
	ld	%0034(r15),r2
	.word	%4FFC,%8303,%0036
	.word	%4FFC,%8203,%003E
	.word	%4FFC,%8103,%0046
	.word	%4FFC,%8003,%004E
	lda	r6,%002E(r15)
	lda	r5,%0036(r15)
	lda	r4,%005A(r15)
	ld	r7,%000A(r15)
	ldk	r3,#1
	ld	r2,%000C(r15)
	call	__doprnt
	ld	r2,%000C(r15)
	ldb	rl3,%0006(r2)
	extsb	r3
	and	r3,#%0020
	test	r3
	jr	eq,L00fe
	ld	@r15,#%FFFF
	jr	L0100
L00fe:
	clr	@r15
L0100:
	ld	r2,@r15
	add	r15,#%0058
	ret
_sprintf::
	sub	r15,#%0060
	.word	%4FFC,%8303,%0026
	.word	%4FFC,%8203,%001E
	.word	%4FFC,%8103,%0016
	.word	%4FFC,%8003,%000E
	ldm	%0002(r15),r2,#6
	ldb	%0034(r15),#%42
	ld	%002E(r15),r7
	ld	%0030(r15),#%7FFF
	ld	%0036(r15),r5
	ld	%0038(r15),r4
	ld	%003A(r15),r3
	ld	%003C(r15),r2
	.word	%4FFC,%8303,%003E
	.word	%4FFC,%8203,%0046
	.word	%4FFC,%8103,%004E
	.word	%4FFC,%8003,%0056
	lda	r6,%0036(r15)
	lda	r5,%003E(r15)
	lda	r4,%0062(r15)
	lda	r2,%002E(r15)
	ldk	r3,#1
	ld	r7,%000A(r15)
	call	__doprnt
	dec	%0030(r15),#1
	cp	%0030(r15),#%0000
	jr	lt,L019a
	ld	r2,%002E(r15)
	clrb	@r2
	subb	rl3,rl3
	extsb	r3
	ld	@r15,r3
	inc	%002E(r15),#1
	jr	L01a6
L019a:
	lda	r6,%002E(r15)
	sub	r7,r7
	call	__flsbuf
	ld	@r15,r2
L01a6:
	ld	r2,%000C(r15)
	add	r15,#%0060
	ret
