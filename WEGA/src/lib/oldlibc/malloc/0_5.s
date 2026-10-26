	.extern	_sbrk
	.psec
	.code
_malloc::
	dec	r15,#12
	ldm	%0002(r15),r10,#5
	ld	%0000(r15),r7
	test	D0000
	jr	ne,Lce1c
	ld	r2,#D0002
	or	r2,#%0001
	ld	D0000,r2
	ld	r2,#D0000
	or	r2,#%0001
	ld	D0002,r2
	ld	D0006,#D0002
	ld	D0004,#D0000
Lce1c:
	ld	r2,%0000(r15)
	inc	r2,#3
	res	r2,#0
	rr	r2,#1
	ld	r11,r2
	ld	r14,D0004
	jr	Lcee8
Lce2e:
	ld	r13,r14
	ld	r2,@r14
	and	r2,#%FFFE
	ld	r14,r2
	cp	r2,r13
	jr	ugt,Lceec
	cp	r13,D0006
	jr	ne,Lce48
	cp	r2,#D0000
	jr	eq,Lce4c
Lce48:
	sub	r2,r2
	jr	Lcf3a
Lce4c:
	inc	D000a,#1
	cp	D000a,#%0001
	jr	ule,Lceec
	ld	r2,@r12
	cp	r2,D0006
	jr	ne,Lce70
	ld	r2,D0006
	sub	r2,r12
	sra	r2,#1
	ld	r3,r11
	sub	r3,r2
	jr	Lce72
Lce70:
	ld	r3,r11
Lce72:
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
	jr	ugt,Lce48
	ld	r7,D000a
	add	r7,r7
	call	_sbrk
	ld	r13,r2
	cp	r2,#%FFFF
	jr	eq,Lce48
	ld	r2,D0006
	ld	@r2,r13
	ld	r2,D0006
	inc	r2,#2
	cp	r13,r2
	jr	eq,Lceca
	ld	r2,D0006
	ld	r2,@r2
	or	r2,#%0001
	ld	r3,D0006
	ld	@r3,r2
Lceca:
	ld	r2,D000a
	add	r2,r2
	dec	r2,#2
	add	r2,r13
	ld	@r13,r2
	ld	D0006,r2
	ld	r2,#D0000
	or	r2,#%0001
	ld	r3,D0006
	ld	@r3,r2
Lcee8:
	clr	D000a
Lceec:
	bit	@r14,#0
	jr	ne,Lce2e
	ld	r12,r14
	jr	Lcef8
Lcef4:
	ld	r2,@r13
	ld	@r14,r2
Lcef8:
	ld	r13,@r14
	bit	@r13,#0
	jr	eq,Lcef4
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r13,r2
	jr	c,Lce2e
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	cp	r14,r2
	jr	ugt,Lce2e
	ld	r2,r11
	add	r2,r11
	add	r2,r14
	ld	D0004,r2
	cp	r13,D0004
	jr	ule,Lcf2c
	ld	r3,@r2
	ld	D0008,r3
	ld	r3,@r14
	ld	@r2,r3
Lcf2c:
	ld	r2,D0004
	or	r2,#%0001
	ld	@r14,r2
	lda	r2,%0002(r14)
Lcf3a:
	ldm	r10,%0002(r15),#5
	inc	r15,#12
	ret
_free::
	dec	r15,#4
	ld	r1,r7
	ld	r3,r7
	dec	r3,#2
	ld	D0004,r3
	ld	r2,@r3
	and	r2,#%FFFE
	ld	@r3,r2
	inc	r15,#4
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
