// malloc.o: magic E711 nonsegmented
	.extern	_malloc
	.extern	_sbrk
	.extern	_free
	.extern	_realloc
	.extern	_calloc
	.extern	_cfree

// segment table entry 0 (segment 0), text, 824 bytes
	.psec
	.code
_malloc::
	sub	r15,#%0012
	ldm	%0004(r15),r8,#7
	ld	r14,r7
	ldk	r8,#0
	cp	r7,L0d_0030
	jr	ule,L0t_001a
L0t_0014:
	sub	r2,r2
	jp	L0t_0184
L0t_001a:
	test	L0d_0038
	jr	eq,L0t_0026
	test	L0d_003c
	jr	ne,L0t_0028
L0t_0026:
	ldk	r8,#1
L0t_0028:
	test	L0d_0038
	jr	ne,L0t_0056
	ld	L0d_0038,#L0d_0034
	ld	L0d_003a,#L0d_0036
	ld	r2,#L0d_0036
	or	r2,#%0001
	ld	r3,L0d_0038
	ld	@r3,r2
	ld	r2,#L0d_0034
	or	r2,#%0001
	ld	r4,L0d_003a
	ld	@r4,r2
L0t_0056:
	lda	r2,%0003(r14)
	res	r2,#0
	rr	r2,#1
	ld	r10,r2
	ld	r13,L0d_0038
	ld	r2,@r13
	and	r2,#%FFFE
	ld	r12,r2
	ld	r11,L0d_003a
L0t_0070:
	bit	@r13,#0
	jr	eq,L0t_0096
	jr	L0t_00b6
L0t_0076:
	ld	r2,@r12
	and	r2,#%FFFE
	ld	r12,r2
	ld	@r13,r2
	ld	L0d_0038,r2
	jr	L0t_0096
L0t_0086:
	cp	r12,L0d_0038
	jr	eq,L0t_0076
	ld	r2,@r12
	and	r2,#%FFFE
	ld	r12,r2
	ld	@r13,r2
L0t_0096:
	bit	@r12,#0
	jr	eq,L0t_0086
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r12,r2
	jr	c,L0t_00ae
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	cp	r13,r2
	jr	ule,L0t_0164
L0t_00ae:
	cp	r12,L0d_003a
	jr	ne,L0t_00b6
	ld	r11,r13
L0t_00b6:
	ld	r13,r12
	ld	r2,@r13
	and	r2,#%FFFE
	ld	r12,r2
	cp	r13,L0d_0038
	jr	ne,L0t_0070
	ld	r2,L0d_003a
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
	cp	r7,L0d_0032
	jr	ugt,L0t_0014
	ld	r2,r7
	sla	r2,#10
	ld	r14,r2
	sub	r7,r7
	call	_sbrk
	ld	r13,r2
	add	r2,r14
	cp	r13,r2
	jr	ugt,L0t_0014
	ld	r7,r14
	call	_sbrk
	ld	r13,r2
	cp	r2,#%FFFF
	jp	eq,L0t_0014
	test	r8
	jr	eq,L0t_0140
	or	r2,#%0001
	ld	r3,L0d_003a
	ld	@r3,r2
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,r13
	dec	r2,#2
	ld	r12,r2
	ld	L0d_003c,#%0001
	jr	L0t_0152
L0t_0140:
	ld	r13,r11
	ld	r2,r9
	sla	r2,#9
	add	r2,r2
	add	r2,L0d_003a
	dec	r2,#2
	ld	r12,r2
L0t_0152:
	ld	r2,#L0d_0034
	or	r2,#%0001
	ld	@r12,r2
	ld	r2,@r12
	ld	@r12,r2
	ld	L0d_003a,r12
L0t_0164:
	ld	r2,r10
	add	r2,r10
	add	r2,r13
	ld	L0d_0038,r2
	cp	r12,L0d_0038
	jr	ule,L0t_0176
	ld	@r2,r12
L0t_0176:
	ld	r2,L0d_0038
	or	r2,#%0001
	ld	@r13,r2
	lda	r2,%0002(r13)
L0t_0184:
	ldm	r8,%0004(r15),#7
	add	r15,#%0012
	ret
L0t_0190:
	dec	r15,#12
	ld	r1,r7
	lda	r2,%0003(r1)
	res	r2,#0
	rr	r2,#1
	ld	r3,r2
	ld	r4,L0d_0038
	ld	r2,@r4
	and	r2,#%FFFE
	ld	r5,r2
L0t_01aa:
	bit	@r4,#0
	jr	eq,L0t_01d0
	jr	L0t_01ec
L0t_01b0:
	ld	r2,@r5
	and	r2,#%FFFE
	ld	r5,r2
	ld	@r4,r2
	ld	L0d_0038,r2
	jr	L0t_01d0
L0t_01c0:
	cp	r5,L0d_0038
	jr	eq,L0t_01b0
	ld	r2,@r5
	and	r2,#%FFFE
	ld	r5,r2
	ld	@r4,r2
L0t_01d0:
	bit	@r5,#0
	jr	eq,L0t_01c0
	ld	r2,r3
	add	r2,r3
	add	r2,r4
	cp	r5,r2
	jr	c,L0t_01ec
	ld	r2,r3
	add	r2,r3
	add	r2,r4
	cp	r4,r2
	jr	ugt,L0t_01ec
	ld	r2,r4
	jr	L0t_01fe
L0t_01ec:
	ld	r4,r5
	ld	r2,@r4
	and	r2,#%FFFE
	ld	r5,r2
	cp	r4,L0d_0038
	jr	ne,L0t_01aa
	sub	r2,r2
L0t_01fe:
	inc	r15,#12
	ret
_free::
	dec	r15,#4
	ld	%0002(r15),r14
	ld	@r15,r7
	ld	r14,%0000(r15)
	ld	r6,r14
	dec	r6,#2
	ld	r14,r6
	ld	L0d_0038,r6
	ld	r6,@r14
	and	r6,#%FFFE
	ld	@r14,r6
	ld	r14,%0002(r15)
	inc	r15,#4
	ret
_realloc::
	sub	r15,#%0016
	ldm	%0008(r15),r8,#7
	ld	@r15,r6
	ld	r14,r7
	dec	r7,#2
	ld	r10,r7
	ld	r7,@r10
	and	r7,#%FFFE
	sub	r7,r14
	sra	r7,#1
	ld	r8,r7
	inc	r6,#1
	res	r6,#0
	rr	r6,#1
	ld	r11,r6
	bit	@r10,#0
	jr	eq,L0t_0258
	ld	r7,r14
	calr	_free
L0t_0258:
	ld	r7,%0000(r15)
	calr	L0t_0190
	ld	r9,r2
	ld	r2,r11
	add	r2,r11
	add	r2,r9
	cp	r10,r2
	jr	nc,L0t_0296
	ld	r2,r11
	add	r2,r11
	add	r2,r9
	ld	r3,r8
	add	r3,r8
	add	r3,r10
	cp	r2,r3
	jr	nc,L0t_0296
	ld	r2,@r9
	or	r2,#%0001
	ld	@r9,r2
	ld	r7,%0000(r15)
	calr	_malloc
	ld	%0004(r15),r2
	ld	r2,@r9
	and	r2,#%FFFE
	ld	@r9,r2
	jr	L0t_02a0
L0t_0296:
	ld	r7,%0000(r15)
	calr	_malloc
	ld	%0004(r15),r2
L0t_02a0:
	test	%0004(r15)
	jr	eq,L0t_02d0
	cp	r14,%0004(r15)
	jr	eq,L0t_02d0
	ld	r13,r14
	ld	r12,%0004(r15)
	cp	r11,r8
	jr	ule,L0t_02ba
	ld	r2,r8
	jr	L0t_02bc
L0t_02ba:
	ld	r2,r11
L0t_02bc:
	ld	r11,r2
	jr	L0t_02c8
L0t_02c0:
	ld	r2,@r13
	ld	@r12,r2
	inc	r13,#2
	inc	r12,#2
L0t_02c8:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jr	ne,L0t_02c0
L0t_02d0:
	ld	r2,%0004(r15)
	ldm	r8,%0008(r15),#7
	add	r15,#%0016
	ret
_calloc::
	dec	r15,#12
	ldm	%0006(r15),r12,#3
	ldl	@r15,rr6
	ld	r3,%0002(r15)
	mult	rr2,r6
	ld	%0002(r15),r3
	ld	r7,r3
	calr	_malloc
	ld	r12,r2
	test	r2
	jr	ne,L0t_0302
	sub	r2,r2
	jr	L0t_0320
L0t_0302:
	ld	r14,r12
	ld	r2,%0002(r15)
	inc	r2,#1
	res	r2,#0
	rr	r2,#1
	ld	r13,r2
	jr	L0t_0316
L0t_0312:
	clr	@r14
	inc	r14,#2
L0t_0316:
	dec	r13,#1
	cp	r13,#%0000
	jr	ge,L0t_0312
	ld	r2,r12
L0t_0320:
	ldm	r12,%0006(r15),#3
	inc	r15,#12
	ret
_cfree::
	dec	r15,#6
	ldm	@r15,r5,#3
	ld	r4,r7
	calr	_free
	inc	r15,#6
	ret

// segment table entry 0 (segment 0), data, 62 bytes
	.psec
	.data
	.word	%405B
	.word	%245D
	.word	%6D61
	.word	%6C6C
	.word	%6F63
	.word	%2E63
	.word	%0933
	.word	%2E33
	.word	%2020
	.word	%3039
	.word	%2F31
	.word	%322F
	.word	%3833
	.word	%2031
	.word	%303A
	.word	%3332
	.word	%3A33
	.word	%3020
	.word	%2D20
	.word	%3837
	.word	%7765
	.word	%6761
	.word	%332E
	.word	%3200
L0d_0030:
	.word	%FFF8
L0d_0032:
	.word	%0040
L0d_0034:
	.word	%0000
L0d_0036:
	.word	%0000
L0d_0038:
	.word	%0000
L0d_003a:
	.word	%0000
L0d_003c:
	.word	%0000
