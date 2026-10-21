	.extern	__deciml
	.psec
	.code
__cnvt::
	sub	r15,#%001E
	ldm	%001A(r15),r13,#2
	ldm	%0000(r15),r4,#4
	ld	r13,r7
	cp	r6,#%FFF6
	jr	ne,L0036
	ld	%0004(r15),#%000A
	cpl	rr4,#%00000000
	jr	ge,L0036
	ldb	@r13,#%2D
	inc	r13,#1
	subl	rr2,rr2
	subl	rr2,%0000(r15)
	ldl	%0000(r15),rr2
L0036:
	lda	r2,%000C(r15)
	ld	r14,r2
	ldl	rr2,%0000(r15)
	ldl	%0008(r15),rr2
	ld	r2,%0004(r15)
	jr	L00e6
L004a:
	ldl	rr2,%0008(r15)
	ldk	r2,#0
	and	r3,#%0007
	addl	rr2,#%00000030
	ldb	@r14,rl3
	inc	r14,#1
	ldl	rr2,%0008(r15)
	ld	r0,#%0003
	neg	r0
	sdll	rr2,r0
	ldl	%0008(r15),rr2
	testl	rr2
	jr	ne,L004a
	jr	L00ae
L0076:
	ldl	rr2,%0008(r15)
	ldk	r2,#0
	and	r3,#%000F
	addl	rr2,#%00000030
	ldb	@r14,rl3
	cpb	@r14,#%39
	jr	le,L0096
	ldb	rl2,@r14
	addb	rl2,#%27
	ldb	@r14,rl2
L0096:
	inc	r14,#1
	ldl	rr2,%0008(r15)
	ld	r0,#%0004
	neg	r0
	sdll	rr2,r0
	ldl	%0008(r15),rr2
	testl	rr2
	jr	ne,L0076
L00ae:
	dec	r14,#1
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r13,#1
	lda	r2,%000C(r15)
	cp	r14,r2
	jr	ne,L00ae
L00be:
	ld	r2,r13
	sub	r2,%0006(r15)
	jr	L00f8
L00c6:
	lda	r7,%0015(r15)
	ldl	rr4,%0000(r15)
	call	__deciml
	ld	r14,r2
L00d4:
	ldb	rl2,@r14
	ldb	@r13,rl2
	inc	r14,#1
	inc	r13,#1
	lda	r2,%0016(r15)
	cp	r14,r2
	jr	ne,L00d4
	jr	L00be
L00e6:
	cp	r2,#%0008
	jr	eq,L004a
	cp	r2,#%000A
	jr	eq,L00c6
	cp	r2,#%0010
	jr	eq,L0076
L00f8:
	ldm	r13,%001A(r15),#2
	add	r15,#%001E
	ret
