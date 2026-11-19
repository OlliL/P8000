	.seg
	.extern	_stkseg
	.extern	_ssgbrk
	.code

Lbe20:
_malloc::
	sub	r15,#%0012
	ldm	_stkseg+6(r15),r8,#6
	ld	|_stkseg+0|(r15),r7
	ldl	rr2,D7892
	testl	rr2
	jr	ne,Lbebe
	ldl	rr2,#D7896
	or	r3,#%0001
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	D7892,rr0
	ldl	rr2,#D7892
	or	r3,#%0001
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	D7896,rr0
	ldl	rr2,#D7896
	ldl	D78ac,rr2
	and	r2,#%7F00
	ldl	D78ac,rr2
	ldl	D789e,rr2
	ldl	rr2,#D7892
	ldl	D78ac,rr2
	and	r2,#%7F00
	ldl	D78ac,rr2
	ldl	D789a,rr2
	ldl	rr2,#D78a8
	ldl	D78a8,rr2
	ld	r2,D78a8
	and	r2,#%7FFF
	ldb	rl2,rh2
	clrb	rh2
	ld	D78a8,r2
Lbebe:
	ld	r2,|_stkseg+0|(r15)
	inc	r2,#11
	srl	r2,#2
	ld	|_stkseg+2|(r15),r2
	subl	rr8,rr8
	ldl	rr12,D789a
Lbed4:
	clr	D78a6
Lbeda:
	ldl	rr2,@rr12
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	ne,Lbf20
	ldl	rr8,rr12
	jr	Lbeee
Lbeea:
	ldl	rr2,@rr10
	ldl	@rr12,rr2
Lbeee:
	ldl	rr10,@rr12
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,Lbeea
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#2
	ldl	rr0,rr12
	add	r1,r2
	cpl	rr10,rr0
	jr	c,Lbf20
	ld	r2,|_stkseg+2|(r15)
	sla	r2,#2
	ldl	rr0,rr12
	add	r1,r2
	cpl	rr12,rr0
	jp	ule,Lc052
Lbf20:
	ldl	rr10,rr12
	ldl	rr2,@rr12
	and	r3,#%FFFE
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	rr12,rr0
	cpl	rr0,rr10
	jr	ugt,Lbeda
	cpl	rr10,D789e
	jr	ne,Lbf44
	cpl	rr0,D78ac
	jr	eq,Lbf4c
Lbf44:
	subl	rr2,rr2
	jp	Lc096
Lbf4c:
	inc	D78a6,#1
	cp	D78a6,#%0001
	jr	ule,Lbeda
	testl	rr8
	jr	eq,Lbf7e
	ldl	rr2,@rr8
	cpl	rr2,D789e
	jr	ne,Lbf7e
	ldl	rr2,D789e
	sub	r3,r9
	sra	r3,#2
	ld	r2,|_stkseg+2|(r15)
	sub	r2,r3
	jr	Lbf82
Lbf7e:
	ld	r2,|_stkseg+2|(r15)
Lbf82:
	ld	|_stkseg+4|(r15),r2
	add	r2,#%0100
	ldb	rl2,rh2
	clrb	rh2
	ldb	rh2,rl2
	clrb	rl2
	ld	D78a6,r2
	ld	r7,D78a8
	sub	r6,r6
	call	_ssgbrk
	and	r2,#%7F00
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	rr10,rr0
	ld	r2,D78a6
	sla	r2,#2
	add	r1,r2
	cpl	rr10,rr0
	jr	ugt,Lbf44
	ld	r6,D78a6
	sla	r6,#2
	ld	r7,D78a8
	call	_ssgbrk
	and	r2,#%7F00
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	rr10,rr0
	cpl	rr0,#%FFFFFFFF
	jr	eq,Lbf44
	ldl	rr2,D789e
	ldl	@rr2,rr0
	ldl	rr2,D789e
	inc	r3,#4
	cpl	rr10,rr2
	jr	eq,Lc01c
	ldl	rr2,D789e
	ldl	rr0,@rr2
	or	r1,#%0001
	ldl	rr2,rr0
	and	r2,#%7F00
	ldl	rr0,D789e
	ldl	rr6,rr0
	ldl	@rr6,rr2
Lc01c:
	ld	r2,D78a6
	sla	r2,#2
	dec	r2,#4
	ldl	rr0,rr10
	add	r1,r2
	ldl	@rr10,rr0
	ldl	D789e,rr0
	ldl	rr2,#D7892
	or	r3,#%0001
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	rr2,D789e
	ldl	@rr2,rr0
	jp	Lbed4
Lc052:
	ldk	r2,#0
	ld	r3,|_stkseg+2|(r15)
	ld	r0,#%0002
	sdal	rr2,r0
	ldl	rr0,rr12
	add	r1,r3
	ldl	D789a,rr0
	cpl	rr10,D789a
	jr	ule,Lc080
	ldl	rr2,rr0
	ldl	rr0,@rr2
	ldl	D78a2,rr0
	ldl	rr0,@rr12
	ldl	@rr2,rr0
Lc080:
	ldl	rr2,D789a
	or	r3,#%0001
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	@rr12,rr0
	ldl	rr2,rr12
	inc	r3,#4
Lc096:
	ldm	r8,_stkseg+6(r15),#6
	add	r15,#%0012
	ret
Lc0a4:
_free::
	dec	r15,#8
	ldm	_stkseg+0(r15),r10,#2
	ldl	rr4,rr6
	ldl	rr10,rr6
	dec	r11,#4
	ldl	D789a,rr10
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	@rr10,rr0
	ldm	r10,_stkseg+0(r15),#2
	inc	r15,#8
	ret
_realloc::
	sub	r15,#%0018
	ldm	_stkseg+14(r15),r9,#5
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	ldl	rr2,rr12(#%FFFC)
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,Lc0f6
	calr	Lc0a4
Lc0f6:
	ldl	rr2,rr12(#%FFFC)
	sub	r3,r13
	sra	r3,#2
	ld	r1,r3
	exts	rr0
	ldl	|_stkseg+10|(r15),rr0
	ld	r7,|_stkseg+0|(r15)
	calr	Lbe20
	and	r2,#%7F00
	ldl	rr0,rr2
	and	r0,#%7F00
	ldl	rr10,rr0
	testl	rr0
	jr	eq,Lc1a4
	cpl	rr0,rr12
	jr	eq,Lc1a4
	ldl	|_stkseg+2|(r15),rr12
	ldl	|_stkseg+6|(r15),rr0
	ld	r2,|_stkseg+0|(r15)
	inc	r2,#1
	srl	r2,#2
	ld	r9,r2
	ldk	r2,#0
	ld	r3,r9
	cpl	rr2,|_stkseg+10|(r15)
	jr	nc,Lc15e
	ld	r3,r9
	ldl	|_stkseg+10|(r15),rr2
	jr	Lc15e
Lc148:
	ldl	rr2,|_stkseg+2|(r15)
	ldl	rr0,|_stkseg+6|(r15)
	ldl	rr6,rr0
	ldl	rr0,@rr2
	ldl	@rr6,rr0
	inc	|_stkseg+4|(r15),#4
	inc	|_stkseg+8|(r15),#4
Lc15e:
	ldl	rr2,|_stkseg+10|(r15)
	ldl	rr0,rr2
	subl	rr2,#%00000001
	ldl	|_stkseg+10|(r15),rr2
	testl	rr0
	jr	ne,Lc148
	cpl	rr10,rr12
	jr	nc,Lc1a4
	ld	r2,r9
	sla	r2,#2
	ldl	rr0,rr10
	add	r1,r2
	cpl	rr12,rr0
	jr	ugt,Lc1a4
	ld	r2,r9
	sla	r2,#2
	ldl	rr0,rr10
	add	r1,r2
	sub	r1,r13
	sra	r1,#2
	sla	r1,#2
	ldl	rr2,rr10
	add	r3,r1
	ldl	rr0,D78a2
	ldl	@rr2,rr0
Lc1a4:
	ldl	rr2,rr10
	ldm	r9,_stkseg+14(r15),#5
	add	r15,#%0018
	ret

	.psec data
	.data
D7892:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D7896:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D789a:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D789e:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D78a2:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D78a6:
	.byte	%00
	.byte	%00
D78a8:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
D78ac:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
