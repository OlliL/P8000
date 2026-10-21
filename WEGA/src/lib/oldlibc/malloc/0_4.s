	.extern	_sbrk
	.psec
	.code
_malloc::
	sub	r15,#%001E
	ldm	%0010(r15),r8,#7
	ld	r14,r7
	ldk	r8,#0
	cp	r7,D0000
	jr	ule,L001a
L0014:
	sub	r2,r2
	jp	L0184
L001a:
	test	D0008
	jr	eq,L0026
	test	D000c
	jr	ne,L0028
L0026:
	ldk	r8,#1
L0028:
	test	D0008
	jr	ne,L0056
	ld	D0008,#D0004
	ld	D000a,#D0006
	ld	r2,#D0006
	or	r2,#%0001
	ld	r3,D0008
	ld	@r3,r2
	ld	r2,#D0004
	or	r2,#%0001
	ld	r4,D000a
	ld	@r4,r2
L0056:
	lda	r2,%0003(r14)
	res	r2,#0
	rr	r2,#1
	ld	r10,r2
	ld	r13,D0008
	ld	r2,@r13
	and	r2,#%FFFE
	ld	r12,r2
	ld	r11,D000a
L0070:
	bit	@r13,#0
	jr	eq,L0096
	jr	L00b6
L0076:
	ld	r2,@r12
	and	r2,#%FFFE
	ld	r12,r2
	ld	@r13,r2
	ld	D0008,r2
	jr	L0096
L0086:
	cp	r12,D0008
	jr	eq,L0076
	ld	r2,@r12
	and	r2,#%FFFE
	ld	r12,r2
	ld	@r13,r2
L0096:
	bit	@r12,#0
	jr	eq,L0086
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r12,r2
	jr	c,L00ae
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r13,r2
	jr	ule,L0164
L00ae:
	cp	r12,D000a
	jr	ne,L00b6
	ld	r11,r13
L00b6:
	ld	r13,r12
	ld	r2,@r13
	and	r2,#%FFFE
	ld	r12,r2
	cp	r13,D0008
	jr	ne,L0070
	ld	r2,D000a
	sub	r2,r11
	sra	r2,#1
	ld	r6,r10
	sub	r6,r2
	add	r6,r8
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#1
	addl	rr2,#%000003FF
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#%00000400
	ld	r9,r7
	cp	r7,D0002
	jr	ugt,L0014
	ld	r2,r7
	sla	r2,#10
	ld	r14,r2
	sub	r7,r7
	call	_sbrk
	ld	r13,r2
	add	r2,r14
	cp	r13,r2
	jr	ugt,L0014
	ld	r7,r14
	call	_sbrk
	ld	r13,r2
	cp	r2,#%FFFF
	jp	eq,L0014
	test	r8
	jr	eq,L0140
	or	r2,#%0001
	ld	r3,D000a
	ld	@r3,r2
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,r13
	dec	r2,#2
	ld	r12,r2
	ld	D000c,#%0001
	jr	L0152
L0140:
	ld	r13,r11
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,D000a
	dec	r2,#2
	ld	r12,r2
L0152:
	ld	r2,#D0004
	or	r2,#%0001
	ld	@r12,r2
	ld	r2,@r12
	ld	@r12,r2
	ld	D000a,r12
L0164:
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	ld	D0008,r2
	cp	r12,D0008
	jr	ule,L0176
	ld	@r2,r12
L0176:
	ld	r2,D0008
	or	r2,#%0001
	ld	@r13,r2
	lda	r2,%0002(r13)
L0184:
	ldm	r8,%0010(r15),#7
	add	r15,#%001E
	ret
_free::
	dec	r15,#6
	ld	%0004(r15),r14
	ld	@r15,r7
	ld	r14,%0000(r15)
	ld	r6,r14
	dec	r6,#2
	ld	r14,r6
	ld	D0008,r6
	ld	r6,@r14
	and	r6,#%FFFE
	ld	@r14,r6
	ld	r14,%0004(r15)
	inc	r15,#6
	ret
	sub	r15,#%0022
	ldm	%0014(r15),r8,#7
	ld	@r15,r6
	ld	r14,r7
	dec	r7,#2
	ld	r10,r7
	bit	@r10,#0
	jr	eq,L01d0
	ld	r7,r14
	calr	_free
L01d0:
	ld	r2,@r10
	sub	r2,r10
	sra	r2,#1
	dec	r2,#1
	ld	r8,r2
	ld	r7,%0000(r15)
	calr	_malloc
	ld	%000E(r15),r2
	test	r2
	jr	eq,L021e
	cp	r14,%000E(r15)
	jr	eq,L021e
	ld	r13,r14
	ld	r12,r2
	ld	r2,%0000(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r11,r2
	cp	r2,r8
	jr	ule,L0208
	ld	r2,r8
	jr	L020a
L0208:
	ld	r2,r11
L020a:
	ld	r11,r2
	jr	L0216
L020e:
	ld	r2,@r13
	ld	@r12,r2
	inc	r13,#2
	inc	r12,#2
L0216:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L020e
L021e:
	ld	r2,%000E(r15)
	ldm	r8,%0014(r15),#7
	add	r15,#%0022
	ret
_calloc::
	sub	r15,#%0012
	ldm	%000C(r15),r12,#3
	ldl	@r15,rr6
	ld	r3,%0002(r15)
	mult	rr2,r6
	ld	%0002(r15),r3
	ld	r7,r3
	calr	_malloc
	ld	r12,r2
	test	r2
	jr	ne,L0252
	sub	r2,r2
	jr	L0270
L0252:
	ld	r14,r12
	ld	r2,%0002(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r13,r2
	jr	L0266
L0262:
	clr	@r14
	inc	r14,#2
L0266:
	dec	r13,#1
	cp	r13,#%0000
	jr	ge,L0262
	ld	r2,r12
L0270:
	ldm	r12,%000C(r15),#3
	add	r15,#%0012
	ret
	dec	r15,#6
	ldm	@r15,r5,#3
	ld	r4,r7
	calr	_free
	inc	r15,#6
	ret
	.psec
	.data
D0000:
	.byte	%ff,%f8
D0002:
	.byte	%00,%40
D0004:
	.byte	%00,%00
D0006:
	.byte	%00,%00
D0008:
	.byte	%00,%00
D000a:
	.byte	%00,%00
D000c:
	.byte	%00,%00
