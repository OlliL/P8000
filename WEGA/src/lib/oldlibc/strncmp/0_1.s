	.psec
	.code
_strncmp::
	dec	r15,#6
	ld	%0000(r15),r12
	ld	r1,r7
	ld	r4,r6
	ld	r12,r5
L000c:
	dec	r12,#1
	cp	r12,#%0000
	jr	lt,L002c
	ld	r2,r4
	inc	r4,#1
	ldb	rl2,@r2
	cpb	rl2,@r1
	jr	ne,L002c
	ld	r2,r1
	inc	r1,#1
	ldb	rl2,@r2
	testb	rl2
	jr	ne,L000c
L0028:
	sub	r2,r2
	jr	L003e
L002c:
	cp	r12,#%0000
	jr	lt,L0028
	ldb	rl2,@r1
	extsb	r2
	dec	r4,#1
	ldb	rl3,@r4
	extsb	r3
	sub	r2,r3
L003e:
	ld	r12,%0000(r15)
	inc	r15,#6
	ret
