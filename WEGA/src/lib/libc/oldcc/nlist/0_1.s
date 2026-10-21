// nlist.o: magic E711 nonsegmented
	.extern	_a_magic
	.extern	_nlist
	.extern	_open
	.extern	_read
	.extern	_close
	.extern	_lseek
	.extern	_readby

// segment table entry 0 (segment 0), text, 666 bytes
	.psec
	.code
_nlist::
	sub	r15,#%063C
	ldm	%0638(r15),r13,#2
	ldl	@r15,rr6
	clr	%062E(r15)
	ld	r13,r6
	jr	L0t_0024
L0t_0014:
	clrb	%0006(r13)
	subl	rr2,rr2
	ldl	%0002(r13),rr2
	inc	%062E(r15),#1
	inc	r13,#8
L0t_0024:
	ld	r2,@r13
	testb	@r2
	jr	ne,L0t_0014
	ld	r7,%0002(r15)
	sub	r6,r6
	call	_open
	ld	%0004(r15),r2
	cp	r2,#%0000
	jr	ge,L0t_0046
L0t_003e:
	ld	r2,#%FFFF
	jp	L0t_024e
L0t_0046:
	lda	r6,%0012(r15)
	ld	r7,%0004(r15)
	ld	r5,#%0018
	call	_read
	clr	%0006(r15)
	jr	L0t_0070
L0t_005c:
	ld	r2,%0006(r15)
	add	r2,r2
	ld	r3,_a_magic(r2)
	cp	r3,%0012(r15)
	jr	eq,L0t_007c
	inc	%0006(r15),#1
L0t_0070:
	ld	r2,%0006(r15)
	add	r2,r2
	test	_a_magic(r2)
	jr	ne,L0t_005c
L0t_007c:
	ld	r2,%0006(r15)
	add	r2,r2
	test	_a_magic(r2)
	jr	ne,L0t_0092
	ld	r7,%0004(r15)
	call	_close
	jr	L0t_003e
L0t_0092:
	ldl	rr2,%0014(r15)
	ldl	%000E(r15),rr2
	ld	r4,%0024(r15)
	and	r4,#%0001
	cp	r4,#%0001
	jr	eq,L0t_00b4
	ldl	rr6,rr2
	multl	rq4,#%00000002
	ldl	%000E(r15),rr6
L0t_00b4:
	ld	r2,%001C(r15)
	add	r2,#%0018
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,%000E(r15)
	ldl	%000E(r15),rr4
	ld	r7,%0004(r15)
	sub	r3,r3
	call	_lseek
	ld	r2,%001E(r15)
	ld	%000A(r15),r2
L0t_00da:
	test	%000A(r15)
	jp	eq,L0t_0244
	ld	%000C(r15),#%0578
	cp	%000A(r15),#%0578
	jr	nc,L0t_00f8
	ld	r2,%000A(r15)
	ld	%000C(r15),r2
L0t_00f8:
	lda	r6,%002A(r15)
	ld	r7,%0004(r15)
	ld	r5,%000C(r15)
	call	_read
	ld	r2,%000A(r15)
	sub	r2,%000C(r15)
	ld	%000A(r15),r2
	lda	r2,%002A(r15)
	ld	r14,r2
L0t_011a:
	test	%000C(r15)
	jr	eq,L0t_00da
	ld	r13,%0000(r15)
L0t_0124:
	ld	r2,@r13
	testb	@r2
	jr	eq,L0t_01be
	ld	r2,@r13
	ld	%0632(r15),r2
	lda	r3,%0006(r14)
	ld	%0634(r15),r3
	ld	r4,r3
	ldb	rl5,@r4
	extsb	r5
	bit	r5,#7
	jr	eq,L0t_0158
	ldb	rl5,@r4
	extsb	r5
	and	r5,#%007F
	add	r5,%0634(r15)
	ld	%0630(r15),r5
	inc	%0634(r15),#1
	jr	L0t_0162
L0t_0158:
	ld	r2,%0634(r15)
	inc	r2,#8
	ld	%0630(r15),r2
L0t_0162:
	clr	%0636(r15)
L0t_0166:
	ld	r2,%0632(r15)
	testb	@r2
	jr	eq,L0t_0190
	ld	r3,%0634(r15)
	cp	r3,%0630(r15)
	jr	nc,L0t_0190
	inc	%0632(r15),#1
	ldb	rl3,@r2
	ld	r2,%0634(r15)
	inc	%0634(r15),#1
	ldb	rl4,@r2
	cpb	rl3,rl4
	jr	eq,L0t_0166
	inc	%0636(r15),#1
L0t_0190:
	ld	r2,%0634(r15)
	cp	r2,%0630(r15)
	jr	nc,L0t_01a2
	testb	@r2
	jr	eq,L0t_01a2
	inc	%0636(r15),#1
L0t_01a2:
	test	%0636(r15)
	jr	ne,L0t_01c6
	ldl	rr2,@r14
	ldl	%0002(r13),rr2
	ldb	rl2,%0004(r14)
	ldb	%0006(r13),rl2
	testb	rl2
	jr	eq,L0t_01be
	dec	%062E(r15),#1
L0t_01be:
	test	%062E(r15)
	jr	ne,L0t_01ca
	jr	L0t_0244
L0t_01c6:
	inc	r13,#8
	jr	L0t_0124
L0t_01ca:
	ld	r7,r14
	calr	_readby
	ld	%0006(r15),r2
	ld	r5,r2
	mult	rr4,#%000E
	ld	r4,%000C(r15)
	sub	r4,r5
	ld	%000C(r15),r4
	test	r4
	jr	eq,L0t_011a
	ld	r3,%0006(r15)
	mult	rr2,#%000E
	add	r14,r3
	ld	r7,r14
	calr	_readby
	ld	%0006(r15),r2
	ld	r5,%000C(r15)
	ldk	r4,#0
	div	rr4,#%000E
	ld	%0008(r15),r5
	cp	r2,r5
	jr	le,L0t_011a
	ld	r5,r2
	sub	r5,%0008(r15)
	ld	r3,r5
	mult	rr2,#%000E
	ld	%0006(r15),r3
	ld	r5,r3
	lda	r6,%05A2(r15)
	ld	r7,%0004(r15)
	call	_read
	ld	r2,%000A(r15)
	sub	r2,%0006(r15)
	ld	%000A(r15),r2
	ld	r2,%000C(r15)
	add	r2,%0006(r15)
	ld	%000C(r15),r2
	jp	L0t_011a
L0t_0244:
	ld	r7,%0004(r15)
	call	_close
	sub	r2,r2
L0t_024e:
	ldm	r13,%0638(r15),#2
	add	r15,#%063C
	ret
_readby::
	dec	r15,#6
	ld	@r15,r7
	ld	r6,r7
	ldb	rl2,%0006(r6)
	extsb	r2
	bit	r2,#7
	jr	eq,L0t_0294
	ldb	rl2,%0006(r7)
	extsb	r2
	and	r2,#%007F
	dec	r2,#7
	ld	%0002(r15),r2
	dec	r2,#7
	ld	r5,r2
	exts	rr4
	div	rr4,#%000E
	inc	r5,#1
	ld	%0002(r15),r5
	inc	%0002(r15),#1
	ld	r2,%0002(r15)
	jr	L0t_0296
L0t_0294:
	ldk	r2,#1
L0t_0296:
	inc	r15,#6
	ret

// segment table entry 0 (segment 0), data, 10 bytes
	.psec
	.data
_a_magic::
	.word	%E607
	.word	%E611
	.word	%E707
	.word	%E711
	.word	%0000
