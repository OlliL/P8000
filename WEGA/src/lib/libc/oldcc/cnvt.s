// cnvt.o: magic E711 nonsegmented
	.extern	__cnvt
	.extern	__deciml

// segment table entry 0 (segment 0), text, 244 bytes
	.psec
	.code
__cnvt::
	sub	r15,#%0020
	ldm	%001C(r15),r13,#2
	ldm	@r15,r4,#4
	ld	r13,r7
	cp	r6,#%FFF6
	jr	ne,L0t_0032
	ld	%0004(r15),#%000A
	cpl	rr4,#%00000000
	jr	ge,L0t_0032
	ldb	@r13,#%2D
	inc	r13,#1
	subl	rr2,rr2
	subl	rr2,rr4
	ldl	%0000(r15),rr2
L0t_0032:
	lda	r14,%000C(r15)
	ldl	rr4,%0000(r15)
	ldl	%0008(r15),rr4
	ld	r3,%0004(r15)
	ld	r2,#L0d_0000
	ld	r4,#%0003
	cpir	r3,@r2,r4,eq
	jr	ne,L0t_00e8
	add	r4,r4
	neg	r4
	ld	r3,L0d_000a(r4)
	jp	@r3
L0t_005a:
	ldl	rr4,%0008(r15)
	ldk	r4,#0
	and	r5,#%0007
	addl	rr4,#%00000030
	ldb	@r14,rl5
	inc	r14,#1
	ldl	rr4,%0008(r15)
	srll	rr4,#3
	ldl	%0008(r15),rr4
	testl	rr4
	jr	ne,L0t_005a
	jr	L0t_00b2
L0t_0080:
	ldl	rr4,%0008(r15)
	ldk	r4,#0
	and	r5,#%000F
	addl	rr4,#%00000030
	ldb	@r14,rl5
	cpb	@r14,#%39
	jr	le,L0t_00a0
	ldb	rl2,@r14
	addb	rl2,#%27
	ldb	@r14,rl2
L0t_00a0:
	inc	r14,#1
	ldl	rr2,%0008(r15)
	srll	rr2,#4
	ldl	%0008(r15),rr2
	testl	rr2
	jr	ne,L0t_0080
L0t_00b2:
	dec	r14,#1
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r13,#1
	lda	r2,%000C(r15)
	cp	r14,r2
	jr	ne,L0t_00b2
	jr	L0t_00e2
L0t_00c4:
	lda	r7,%0015(r15)
	ldl	rr4,%0000(r15)
	call	__deciml
	ld	r14,r2
L0t_00d2:
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r14,#1
	inc	r13,#1
	lda	r2,%0016(r15)
	cp	r14,r2
	jr	ne,L0t_00d2
L0t_00e2:
	ld	r2,r13
	sub	r2,%0006(r15)
L0t_00e8:
	ldm	r13,%001C(r15),#2
	add	r15,#%0020
	ret

// segment table entry 0 (segment 0), data, 12 bytes
	.psec
	.data
L0d_0000:
	.word	%0008
	.word	%000A
	.word	%0010
	.word	L0t_005a
	.word	L0t_00c4
L0d_000a:
	.word	L0t_0080
