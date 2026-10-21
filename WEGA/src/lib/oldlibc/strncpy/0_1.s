	.psec
	.code
_strncpy::
	dec	r15,#10
	ldm	%0002(r15),r11,#2
	ld	%0000(r15),r5
	ld	r1,r7
	ld	r4,r6
	ld	r11,r7
	ldk	r12,#0
	jr	L0038
L0016:
	ld	r2,r4
	inc	r4,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl0,@r2
	ldb	@r3,rl0
	testb	rl0
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
	ldm	r11,%0002(r15),#2
	inc	r15,#10
	ret
