	.psec
	.code
_atoi::
	dec	r15,#6
	ld	r1,r7
	ldk	r5,#0
	ldk	r6,#0
	jr	L503c
L5020:
	cpb	@r1,#%39
	jr	gt,L505e
	ld	r3,r5
	mult	rr2,#%000A
	ldb	rl2,@r1
	extsb	r2
	add	r3,r2
	sub	r3,#%0030
	ld	r5,r3
	jr	L5056
L503a:
	inc	r1,#1
L503c:
	ldb	rl2,@r1
	extsb	r2
	ld	r3,#D0bc8
	ld	r4,#%0004
	cpir	r2,@r3,r4,eq
	jr	ne,L5058
	ld	r2,%0006(r3)
	jp	@r2
L5054:
	inc	r6,#1
L5056:
	inc	r1,#1
L5058:
	cpb	@r1,#%30
	jr	ge,L5020
L505e:
	test	r6
	jr	eq,L5068
	ld	r2,r5
	neg	r2
	jr	L506a
L5068:
	ld	r2,r5
L506a:
	inc	r15,#6
	ret
	.psec
	.data
D0bc8:
	.word	9,32,43,45
D0bd0:
	.word	L503a,L503a,L5056,L5054
