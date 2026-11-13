// symscan.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_symscan
	.extern	_stkseg
	.extern	_readsym
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 296 bytes
	.psec
	.code
_symscan::
	sub	r15,#%00A6
	ldm	_stkseg+154(r15),r8,#6
	ldm	|_stkseg|(r15),r4,#4
	ldl	rr2,rr6
	inc	r3,#8
	clrb	@rr2
	ldk	r13,#0
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr10,@rr2
	jr	L0t_0024
L0t_0022:
	inc	r13,#1
L0t_0024:
	ldl	rr2,rr10
	inc	r11,#1
	testb	@rr2
	jr	ne,L0t_0022
	cp	r13,#%0008
	jr	gt,L0t_0084
L0t_0032:
	ldl	rr6,#_stkseg+8
	add	r7,r15
	ldl	rr4,|_stkseg|(r15)
	call	_readsym
	ld	r13,r2
	test	r2
	jr	ne,L0t_007c
	ldl	rr2,#_stkseg+14
	add	r3,r15
	inc	r3,#8
	clrb	@rr2
	ldl	rr2,#_stkseg+14
	add	r3,r15
	ldl	rr10,rr2
	ldl	rr2,|_stkseg+4|(r15)
	ldl	rr8,@rr2
	jr	L0t_0072
L0t_0068:
	ldb	rl2,@rr8
	cpb	rl2,@rr10
	jr	ne,L0t_0076
	inc	r9,#1
	inc	r11,#1
L0t_0072:
	testb	@rr8
	jr	ne,L0t_0068
L0t_0076:
	ldb	rl2,@rr10
	cpb	rl2,@rr8
	jr	eq,L0t_00fc
L0t_007c:
	cp	r13,#%0000
	jr	ge,L0t_0032
	jr	L0t_00fc
L0t_0084:
	ld	r2,r13
	inc	r2,#1
	ldb	|_stkseg+151|(r15),rl2
	extsb	r2
	inc	r2,#5
	ld	r5,r2
	exts	rr4
	div	rr4,#%000E
	ld	|_stkseg+152|(r15),r5
	ldb	rl2,|_stkseg+151|(r15)
	orb	rl2,#%80
	ldb	|_stkseg+151|(r15),rl2
L0t_00a8:
	ldl	rr6,#_stkseg+8
	add	r7,r15
	ldl	rr4,|_stkseg|(r15)
	call	_readsym
	ld	r13,r2
	cp	r2,|_stkseg+152|(r15)
	jr	ne,L0t_00f6
	ldb	rl2,|_stkseg+14|(r15)
	cpb	rl2,|_stkseg+151|(r15)
	jr	ne,L0t_00f6
	ldl	rr4,|_stkseg+4|(r15)
	ldl	rr10,@rr4
	ldl	rr6,#_stkseg+14
	add	r7,r15
	inc	r7,#1
	ldl	rr8,rr6
L0t_00de:
	ldl	rr2,rr10
	inc	r11,#1
	testb	@rr2
	jr	eq,L0t_00f2
	ldl	rr2,rr8
	inc	r9,#1
	testb	@rr2
	jr	eq,L0t_00f2
	testb	@rr8
	jr	ne,L0t_00de
L0t_00f2:
	testb	@rr8
	jr	eq,L0t_00fc
L0t_00f6:
	cp	r13,#%0000
	jr	ge,L0t_00a8
L0t_00fc:
	cp	r13,#%0000
	jr	lt,L0t_011a
	ldl	rr2,|_stkseg+4|(r15)
	inc	r3,#4
	ldl	rr4,|_stkseg+8|(r15)
	ldl	@rr2,rr4
	ldl	rr2,|_stkseg+4|(r15)
	inc	r3,#8
	ldb	rl4,|_stkseg+12|(r15)
	ldb	@rr2,rl4
L0t_011a:
	ldm	r8,_stkseg+154(r15),#6
	add	r15,#%00A6
	ret
