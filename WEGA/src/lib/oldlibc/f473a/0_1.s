	.psec
	.code
	sub	r15,#%0014
	ldm	%000C(r15),r11,#3
	ld	@r15,r5
	ld	r13,r6
	ld	r1,r7
	ld	r11,r7
	ldk	r12,#0
	jr	L0038
L0016:
	ld	r2,r13
	inc	r13,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jr	eq,L002c
	jr	L0036
L0028:
	clrb	@r1
	inc	r1,#1
L002c:
	inc	r12,#1
	cp	r12,%0000(r15)
	jr	lt,L0028
	jr	L003e
L0036:
	inc	r12,#1
L0038:
	cp	r12,%0000(r15)
	jr	lt,L0016
L003e:
	ld	r2,r11
	ldm	r11,%000C(r15),#3
	add	r15,#%0014
	ret
