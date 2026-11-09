	.seg
	.extern	_stkseg
	.extern	_malloc
	.extern	_read
	.psec
	.code

__filbuf::
	dec	r15,#10
	ldm	_stkseg+6(r15),r12,#2
	ldl	rr12,rr6
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,La6a8
	ldl	rr2,rr6
	inc	r3,#10
	setb	@rr2,#0
La6a8:
	ldb	rl2,rr12(#%000A)
	extsb	r2
	and	r2,#%0001
	test	r2
	jr	eq,La6c0
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#6
	jr	eq,La6e0
La6c0:
	ld	r2,#%FFFF
	jr	La794
La6c6:
	ld	r7,#%0200
	call	_malloc
	ldl	rr4,rr12
	inc	r5,#6
	ldl	@rr4,rr2
	testl	rr2
	jr	ne,La708
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#2
La6e0:
	ldl	rr2,rr12(#%0006)
	testl	rr2
	jr	ne,La70e
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#2
	jr	eq,La6c6
	ldb	rl2,rr12(#%000B)
	extsb	r2
	ldl	rr4,#D8ff6
	add	r5,r2
	ldl	rr2,rr12
	inc	r3,#6
	ldl	@rr2,rr4
	jr	La6e0
La708:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#3
La70e:
	ldl	rr2,rr12(#%0006)
	ldl	@rr12,rr2
	ldb	rl7,rr12(#%000B)
	extsb	r7
	ldb	rl3,rr12(#%000A)
	extsb	r3
	bit	r3,#2
	jr	eq,La728
	ldk	r3,#1
	jr	La72c
La728:
	ld	r3,#%0200
La72c:
	ldl	rr4,@rr12
	call	_read
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
	ldl	rr2,rr12
	inc	r3,#4
	dec	@rr2,#1
	ld	r4,@rr2
	cp	r4,#%0000
	jr	ge,La778
	ld	r2,rr12(#%0004)
	cp	r2,#%FFFF
	jr	ne,La76a
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#4
	ldb	rl2,rr12(#%000A)
	extsb	r2
	bit	r2,#7
	jr	eq,La770
	ldl	rr2,rr12
	inc	r3,#10
	resb	@rr2,#0
	jr	La770
La76a:
	ldl	rr2,rr12
	inc	r3,#10
	setb	@rr2,#5
La770:
	ldl	rr2,rr12
	inc	r3,#4
	clr	@rr2
	jr	La6c0
La778:
	ldl	rr2,@rr12
	ldb	rl4,@rr2
	extsb	r4
	and	r4,#%00FF
	ld	_stkseg+4(r15),r4
	inc	r13,#2
	inc	@rr12,#1
	dec	r13,#2
	ld	r2,_stkseg+4(r15)
La794:
	ldm	r12,_stkseg+6(r15),#2
	inc	r15,#10
	ret
	.psec data
	.data
D8ff6:
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
	.byte	%00
