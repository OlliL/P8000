	.extern	__deciml
	.psec
	.code
__cnvt::
	sub	r15,#%0024
	ldm	%0020(r15),r13,#2
	ldm	@r15,r4,#4
	ld	r13,r7
	cp	r6,#%FFF6
	jr	ne,L0032
	ld	%0004(r15),#%000A
	cpl	rr4,#%00000000
	jr	ge,L0032
	ldb	@r13,#%2D
	inc	r13,#1
	subl	rr2,rr2
	subl	rr2,rr4
	ldl	%0000(r15),rr2
L0032:
	lda	r14,%0010(r15)
	ldl	rr4,%0000(r15)
	ldl	%000C(r15),rr4
	ld	r3,%0004(r15)
	ld	r2,#D0000
	ld	r4,#%0003
	cpir	r3,@r2,r4,eq
	jr	ne,L00e8
	add	r4,r4
	neg	r4
	ld	r3,D000a(r4)
	jp	@r3
L005a:
	ldl	rr4,%000C(r15)
	ldk	r4,#0
	and	r5,#%0007
	addl	rr4,#%00000030
	ldb	@r14,rl5
	inc	r14,#1
	ldl	rr4,%000C(r15)
	srll	rr4,#3
	ldl	%000C(r15),rr4
	testl	rr4
	jr	ne,L005a
	jr	L00b2
L0080:
	ldl	rr4,%000C(r15)
	ldk	r4,#0
	and	r5,#%000F
	addl	rr4,#%00000030
	ldb	@r14,rl5
	cpb	@r14,#%39
	jr	le,L00a0
	ldb	rl2,@r14
	addb	rl2,#%27
	ldb	@r14,rl2
L00a0:
	inc	r14,#1
	ldl	rr2,%000C(r15)
	srll	rr2,#4
	ldl	%000C(r15),rr2
	testl	rr2
	jr	ne,L0080
L00b2:
	dec	r14,#1
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r13,#1
	lda	r2,%0010(r15)
	cp	r14,r2
	jr	ne,L00b2
	jr	L00e2
L00c4:
	lda	r7,%0019(r15)
	ldl	rr4,%0000(r15)
	call	__deciml
	ld	r14,r2
L00d2:
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r14,#1
	inc	r13,#1
	lda	r2,%001A(r15)
	cp	r14,r2
	jr	ne,L00d2
L00e2:
	ld	r2,r13
	sub	r2,%0006(r15)
L00e8:
	ldm	r13,%0020(r15),#2
	add	r15,#%0024
	ret
	.psec
	.data
D0000:
	.byte	%00,%08,%00,%0a,%00,%10
	.addr	L005a
	.addr	L00c4
D000a:
	.addr	L0080
