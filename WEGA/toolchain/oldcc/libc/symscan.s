// symscan.o: magic E711 nonsegmented
	.extern	_symscan
	.extern	_readsym

// segment table entry 0 (segment 0), text, 260 bytes
	.psec
	.code
_symscan::
	sub	r15,#%009C
	ldm	%0096(r15),r12,#3
	ldl	@r15,rr6
	ld	r2,r7
	clrb	%0006(r2)
	ldk	r14,#0
	ld	r3,%0002(r15)
	ld	r13,@r3
	jr	L0t_001e
L0t_001c:
	inc	r14,#1
L0t_001e:
	ld	r2,r13
	inc	r13,#1
	testb	@r2
	jr	ne,L0t_001c
	cp	r14,#%0008
	jr	gt,L0t_006c
L0t_002c:
	lda	r7,%0004(r15)
	ld	r6,%0000(r15)
	call	_readsym
	ld	r14,r2
	test	r2
	jr	ne,L0t_0064
	clrb	%0012(r15)
	lda	r2,%000A(r15)
	ld	r13,r2
	ld	r3,%0002(r15)
	ld	r12,@r3
	jr	L0t_005a
L0t_0050:
	ldb	rl2,@r12
	cpb	rl2,@r13
	jr	ne,L0t_005e
	inc	r12,#1
	inc	r13,#1
L0t_005a:
	testb	@r12
	jr	ne,L0t_0050
L0t_005e:
	ldb	rl2,@r13
	cpb	rl2,@r12
	jr	eq,L0t_00da
L0t_0064:
	cp	r14,#%0000
	jr	ge,L0t_002c
	jr	L0t_00da
L0t_006c:
	lda	r2,%0001(r14)
	ldb	%0093(r15),rl2
	ldb	rl3,rl2
	extsb	r3
	inc	r3,#5
	ld	r5,r3
	exts	rr4
	div	rr4,#%000E
	ld	%0094(r15),r5
	ldb	rl3,%0093(r15)
	orb	rl3,#%80
	ldb	%0093(r15),rl3
L0t_0092:
	lda	r7,%0004(r15)
	ld	r6,%0000(r15)
	call	_readsym
	ld	r14,r2
	cp	r2,%0094(r15)
	jr	ne,L0t_00d4
	ldb	rl2,%000A(r15)
	cpb	rl2,%0093(r15)
	jr	ne,L0t_00d4
	ld	r3,%0002(r15)
	ld	r13,@r3
	lda	r4,%000B(r15)
	ld	r12,r4
L0t_00bc:
	ld	r2,r13
	inc	r13,#1
	testb	@r2
	jr	eq,L0t_00d0
	ld	r2,r12
	inc	r12,#1
	testb	@r2
	jr	eq,L0t_00d0
	testb	@r12
	jr	ne,L0t_00bc
L0t_00d0:
	testb	@r12
	jr	eq,L0t_00da
L0t_00d4:
	cp	r14,#%0000
	jr	ge,L0t_0092
L0t_00da:
	cp	r14,#%0000
	jr	lt,L0t_00f8
	ld	r2,%0002(r15)
	ldl	rr4,%0004(r15)
	ldl	%0002(r2),rr4
	ld	r2,%0002(r15)
	ldb	rl3,%0008(r15)
	ldb	%0006(r2),rl3
L0t_00f8:
	ldm	r12,%0096(r15),#3
	add	r15,#%009C
	ret
