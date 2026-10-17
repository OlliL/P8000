// scanf.o: magic E711 nonsegmented
	.extern	_scanf
	.extern	__iob
	.extern	__doscan
	.extern	_fscanf
	.extern	_sscanf

// segment table entry 0 (segment 0), text, 220 bytes
	.psec
	.code
_scanf::
	sub	r15,#%0018
	ldm	@r15,r2,#6
	ld	%000C(r15),r6
	ld	%000E(r15),r5
	ld	%0010(r15),r4
	ld	%0012(r15),r3
	ld	%0014(r15),r2
	lda	r5,%000C(r15)
	lda	r4,%001A(r15)
	ld	r7,#__iob
	ld	r6,%000A(r15)
	call	__doscan
	add	r15,#%0018
	ret
_fscanf::
	sub	r15,#%0018
	ldm	@r15,r2,#6
	ld	%000C(r15),r5
	ld	%000E(r15),r4
	ld	%0010(r15),r3
	ld	%0012(r15),r2
	ld	r0,%001A(r15)
	ld	%0014(r15),r0
	lda	r5,%000C(r15)
	lda	r4,%001C(r15)
	call	__doscan
	add	r15,#%0018
	ret
_sscanf::
	sub	r15,#%0022
	ld	%0020(r15),r14
	ldm	@r15,r2,#5
	ld	r14,r7
	ldb	%0012(r15),#%41
	ld	%0010(r15),r7
	ld	%000C(r15),r7
	clr	%000E(r15)
	jr	L0t_008e
L0t_008a:
	inc	%000E(r15),#1
L0t_008e:
	ld	r2,r14
	inc	r14,#1
	testb	@r2
	jr	ne,L0t_008a
	ld	r2,%0006(r15)
	ld	%0014(r15),r2
	ld	r3,%0004(r15)
	ld	%0016(r15),r3
	ld	r4,%0002(r15)
	ld	%0018(r15),r4
	ld	r5,%0000(r15)
	ld	%001A(r15),r5
	ld	r6,%0024(r15)
	ld	%001C(r15),r6
	lda	r7,%000C(r15)
	lda	r5,%0014(r15)
	lda	r4,%0026(r15)
	ld	r6,%0008(r15)
	call	__doscan
	ld	r14,%0020(r15)
	add	r15,#%0022
	ret
