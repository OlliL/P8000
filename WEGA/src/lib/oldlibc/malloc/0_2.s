	.extern	_sbrk
	.psec
	.code
_malloc::
	sub	r15,#%0018
	ldm	%000E(r15),r10,#5
	ld	@r15,r7
	test	D0000
	jr	ne,L0036
	ld	r6,#D0002
	or	r6,#%0001
	ld	D0000,r6
	ld	r6,#D0000
	or	r6,#%0001
	ld	D0002,r6
	ld	D0006,#D0002
	ld	D0004,#D0000
L0036:
	ld	r2,%0000(r15)
	inc	r2,#3
	res	r2,#0
	rr	r2,#1
	ld	r11,r2
	ld	r14,D0004
	jr	L00fe
L0048:
	ld	r13,r14
	ld	r2,@r14
	and	r2,#%FFFE
	ld	r14,r2
	cp	r2,r13
	jr	ugt,L0102
	cp	r13,D0006
	jr	ne,L0062
	cp	r2,#D0000
	jr	eq,L0066
L0062:
	sub	r2,r2
	jr	L0150
L0066:
	inc	D000a,#1
	cp	D000a,#%0001
	jr	ule,L0102
	ld	r2,@r12
	cp	r2,D0006
	jr	ne,L008a
	ld	r2,D0006
	sub	r2,r12
	sra	r2,#1
	ld	r3,r11
	sub	r3,r2
	jr	L008c
L008a:
	ld	r3,r11
L008c:
	ld	r10,r3
	lda	r2,%0200(r10)
	srl	r2,#9
	sla	r2,#9
	ld	D000a,r2
	sub	r7,r7
	call	_sbrk
	ld	r13,r2
	ld	r2,D000a
	add	r2,r2
	add	r2,r13
	cp	r13,r2
	jr	ugt,L0062
	ld	r7,D000a
	add	r7,r7
	call	_sbrk
	ld	r13,r2
	cp	r2,#%FFFF
	jr	eq,L0062
	ld	r2,D0006
	ld	@r2,r13
	inc	r2,#2
	cp	r13,r2
	jr	eq,L00e0
	ld	r2,D0006
	ld	r3,@r2
	or	r3,#%0001
	ld	r2,D0006
	ld	@r2,r3
L00e0:
	ld	r2,D000a
	add	r2,r2
	add	r2,r13
	dec	r2,#2
	ld	@r13,r2
	ld	D0006,r2
	ld	r2,#D0000
	or	r2,#%0001
	ld	r3,D0006
	ld	@r3,r2
L00fe:
	clr	D000a
L0102:
	bit	@r14,#0
	jr	ne,L0048
	ld	r12,r14
	jr	L010e
L010a:
	ld	r2,@r13
	ld	@r14,r2
L010e:
	ld	r13,@r14
	bit	@r13,#0
	jr	eq,L010a
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r13,r2
	jr	c,L0048
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r14,r2
	jr	ugt,L0048
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	ld	D0004,r2
	cp	r13,D0004
	jr	ule,L0142
	ld	r3,@r2
	ld	D0008,r3
	ld	r4,@r14
	ld	@r2,r4
L0142:
	ld	r2,D0004
	or	r2,#%0001
	ld	@r14,r2
	lda	r2,%0002(r14)
L0150:
	ldm	r10,%000E(r15),#5
	add	r15,#%0018
	ret
_free::
	dec	r15,#8
	ld	r1,r7
	ld	r3,r7
	dec	r3,#2
	ld	D0004,r3
	ld	r2,@r3
	and	r2,#%FFFE
	ld	@r3,r2
	inc	r15,#8
	ret
	sub	r15,#%0016
	ldm	%0010(r15),r12,#3
	ld	@r15,r6
	ld	r14,r7
	bit	%FFFE(r14),#0
	jr	eq,L018a
	calr	_free
L018a:
	ld	r2,%FFFE(r14)
	sub	r2,r14
	sra	r2,#1
	ld	%000C(r15),r2
	ld	r7,%0000(r15)
	calr	_malloc
	ld	r13,r2
	test	r2
	jr	eq,L020c
	cp	r2,r14
	jr	eq,L020c
	ld	%0006(r15),r14
	ld	%0008(r15),r2
	ld	r2,%0000(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r12,r2
	cp	r2,%000C(r15)
	jr	nc,L01dc
	ld	%000C(r15),r2
	jr	L01dc
L01c8:
	ld	r2,%0006(r15)
	ld	r3,%0008(r15)
	ld	r4,@r2
	ld	@r3,r4
	inc	%0006(r15),#2
	inc	%0008(r15),#2
L01dc:
	ld	r2,%000C(r15)
	dec	%000C(r15),#1
	test	r2
	jr	ne,L01c8
	cp	r13,r14
	jr	nc,L020c
	ld	r2,r12
	add	r2,r12
	add	r2,r13
	cp	r14,r2
	jr	ugt,L020c
	ld	r2,r12
	add	r2,r12
	add	r2,r13
	sub	r2,r14
	sra	r2,#1
	add	r2,r2
	add	r2,r13
	ld	r3,D0008
	ld	@r2,r3
L020c:
	ld	r2,r13
	ldm	r12,%0010(r15),#3
	add	r15,#%0016
	ret
	.psec
	.data
D0000:
	.byte	%00,%00
D0002:
	.byte	%00,%00
D0004:
	.byte	%00,%00
D0006:
	.byte	%00,%00
D0008:
	.byte	%00,%00
D000a:
	.byte	%00,%00
