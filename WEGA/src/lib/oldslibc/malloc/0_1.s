	.seg
	.extern	_stkseg
	.extern	_f95ee
	.extern	_f9622
	.code

_malloc::
L8cb4:
	sub	r15,#%001A
	ldm	_stkseg+14(r15),r8,#6
	ld	r13,r7
	clr	|_stkseg+8|(r15)
	cp	r7,_d1A90+0
	jr	ule,L8cd6
L8cce:
	subl	rr2,rr2
	jp	L8f6e
L8cd6:
	ldl	rr2,_d1A90+16
	testl	rr2
	jr	eq,L8ce8
	test	_d1A90+24
	jr	ne,L8cee
L8ce8:
	ld	|_stkseg+8|(r15),#%0001
L8cee:
	ldl	rr2,_d1A90+16
	testl	rr2
	jr	ne,L8d68
	ldl	rr4,#%01001A98
	ldl	_d1A90+16,rr4
	ldl	rr2,#%01001A9C
	ldl	_d1A90+20,rr2
	ldl	rr2,#%01001A9C
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr2,_d1A90+16
	ldl	@rr2,rr4
	ldl	rr4,#%01001A98
	or	r5,#%0001
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	rr4,_d1A90+20
	ldl	@rr4,rr6
	ldl	rr2,_d1A90+16
	ldl	rr6,rr2
	ldl	rr0,@rr2
	ldl	@rr6,rr0
	ldl	rr4,_d1A90+20
	ldl	rr6,rr4
	ldl	rr0,@rr4
	ldl	@rr6,rr0
	sub	r7,r7
	sub	r6,r6
	call	_f95ee
	ldl	_d1A90+4,rr2
L8d68:
	ld	r2,r13
	inc	r2,#7
	srl	r2,#2
	ld	r12,r2
	ldl	rr10,_d1A90+16
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	rr2,_d1A90+20
	ldl	|_stkseg+2|(r15),rr2
L8d90:
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L8dce
	jr	L8e06
L8d9e:
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
	ldl	_d1A90+16,rr4
	jr	L8dce
L8db6:
	cpl	rr8,_d1A90+16
	jr	eq,L8d9e
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
L8dce:
	ldl	rr2,@rr8
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L8db6
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr8,rr4
	jr	c,L8dfa
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr10,rr4
	jp	ule,L8f3c
L8dfa:
	cpl	rr8,_d1A90+20
	jr	ne,L8e06
	ldl	|_stkseg+2|(r15),rr10
L8e06:
	ldl	rr10,rr8
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	cpl	rr10,_d1A90+16
	jr	ne,L8d90
	ldl	rr2,_d1A90+20
	sub	r3,|_stkseg+4|(r15)
	sra	r3,#2
	ld	r6,r12
	sub	r6,r3
	add	r6,|_stkseg+8|(r15)
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#2
	addl	rr2,#%000007FF
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#%00000800
	ld	|_stkseg+6|(r15),r7
	ld	r2,r7
	cp	r7,_d1A90+2
	jp	ugt,L8cce
	sla	r2,#11
	dec	r2,#1
	ld	r13,r2
	ldb	rl7,_d1A90+4
	extsb	r7
	ld	r6,r2
	call	_f9622
	ldl	rr10,rr2
	cpl	rr2,#%FFFFFFFF
	jr	ne,L8ecc
	ld	r6,r12
	inc	r6,#1
	ldk	r2,#0
	ld	r3,r6
	slal	rr2,#2
	addl	rr2,#%000007FF
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#%00000800
	ld	|_stkseg+6|(r15),r7
	ld	r2,r7
	sla	r2,#11
	dec	r2,#1
	ld	r13,r2
	sub	r7,r7
	ld	r6,r2
	call	_f95ee
	ldl	rr10,rr2
	cp	r3,#%FFFF
	jp	eq,L8cce
	ldl	_d1A90+4,rr2
	ld	|_stkseg+8|(r15),#%0001
L8ecc:
	test	|_stkseg+8|(r15)
	jr	eq,L8f04
	ldl	rr2,rr10
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr2,_d1A90+20
	ldl	@rr2,rr4
	ld	r4,|_stkseg+6|(r15)
	sla	r4,#9
	sla	r4,#2
	ldl	rr6,rr10
	add	r7,r4
	dec	r7,#4
	ldl	rr8,rr6
	ld	_d1A90+24,#%0001
	jr	L8f20
L8f04:
	ldl	rr10,|_stkseg+2|(r15)
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#9
	sla	r2,#2
	ldl	rr4,_d1A90+20
	add	r5,r2
	dec	r5,#4
	ldl	rr8,rr4
L8f20:
	ldl	rr2,#%01001A98
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr8,rr4
	ldl	rr2,@rr8
	ldl	@rr8,rr2
	ldl	_d1A90+20,rr8
L8f3c:
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	ldl	_d1A90+16,rr4
	cpl	rr8,_d1A90+16
	jr	ule,L8f58
	ldl	rr2,rr4
	ldl	@rr2,rr8
L8f58:
	ldl	rr2,_d1A90+16
	or	r3,#%0001
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr10,rr4
	ldl	rr2,rr10
	inc	r3,#4
L8f6e:
	ldm	r8,_stkseg+14(r15),#6
	add	r15,#%001A
	ret
L8f7c:
	sub	r15,#%0012
	ldm	_stkseg+6(r15),r8,#4
	ld	r1,r7
	ld	r2,r7
	inc	r2,#7
	srl	r2,#2
	ld	r6,r2
	ldl	rr10,_d1A90+16
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
L8fa8:
	ldl	rr2,@rr10
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L8fe6
	jr	L9012
L8fb6:
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
	ldl	_d1A90+16,rr4
	jr	L8fe6
L8fce:
	cpl	rr8,_d1A90+16
	jr	eq,L8fb6
	ldl	rr2,@rr8
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	ldl	@rr10,rr4
L8fe6:
	ldl	rr2,@rr8
	ldk	r2,#0
	and	r3,#%0001
	testl	rr2
	jr	eq,L8fce
	ld	r2,r6
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr8,rr4
	jr	c,L9012
	ld	r2,r6
	sla	r2,#2
	ldl	rr4,rr10
	add	r5,r2
	cpl	rr10,rr4
	jr	ugt,L9012
	ldl	rr2,rr10
	jr	L902c
L9012:
	ldl	rr10,rr8
	ldl	rr2,@rr10
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	rr8,rr4
	cpl	rr10,_d1A90+16
	jr	ne,L8fa8
	subl	rr2,rr2
L902c:
	ldm	r8,_stkseg+6(r15),#4
	add	r15,#%0012
	ret
L903a:
_f903a::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg+0|(r15),rr6
	ldl	rr12,|_stkseg+0|(r15)
	ldl	rr2,rr12
	dec	r3,#4
	ldl	rr12,rr2
	ldl	_d1A90+16,rr2
	ldl	rr2,@rr12
	and	r3,#%FFFE
	ldl	rr4,rr2
	and	r4,#%7F00
	ldl	@rr12,rr4
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
	sub	r15,#%0026
	ldm	_stkseg+26(r15),r8,#6
	ld	|_stkseg+0|(r15),r5
	ldl	rr12,rr6
	ldl	rr2,rr6
	dec	r3,#4
	ldl	|_stkseg+8|(r15),rr2
	ldl	rr6,@rr2
	and	r7,#%FFFE
	sub	r7,r13
	sra	r7,#2
	ld	|_stkseg+20|(r15),r7
	inc	r5,#3
	srl	r5,#2
	ld	|_stkseg+6|(r15),r5
	ldl	rr2,|_stkseg+8|(r15)
	ldl	rr4,@rr2
	ldk	r4,#0
	and	r5,#%0001
	testl	rr4
	jr	eq,L90ba
	ldl	rr6,rr12
	calr	L903a
L90ba:
	ld	r7,|_stkseg+0|(r15)
	calr	L8f7c
	ldl	|_stkseg+12|(r15),rr2
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#2
	ldl	rr4,|_stkseg+12|(r15)
	add	r5,r2
	cpl	rr4,|_stkseg+8|(r15)
	jr	ule,L9128
	ld	r2,|_stkseg+6|(r15)
	sla	r2,#2
	ldl	rr4,|_stkseg+12|(r15)
	add	r5,r2
	ld	r2,|_stkseg+20|(r15)
	sla	r2,#2
	ldl	rr6,|_stkseg+8|(r15)
	add	r7,r2
	cpl	rr4,rr6
	jr	nc,L9128
	ldl	rr2,|_stkseg+12|(r15)
	ldl	rr4,@rr2
	or	r5,#%0001
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	@rr2,rr6
	ld	r7,|_stkseg+0|(r15)
	calr	L8cb4
	ldl	|_stkseg+16|(r15),rr2
	ldl	rr2,|_stkseg+12|(r15)
	ldl	rr4,@rr2
	and	r5,#%FFFE
	ldl	rr6,rr4
	and	r6,#%7F00
	ldl	@rr2,rr6
	jr	L9132
L9128:
	ld	r7,|_stkseg+0|(r15)
	calr	L8cb4
	ldl	|_stkseg+16|(r15),rr2
L9132:
	ldl	rr2,|_stkseg+16|(r15)
	testl	rr2
	jr	eq,L9172
	cpl	rr12,|_stkseg+16|(r15)
	jr	eq,L9172
	ldl	rr10,rr12
	ldl	rr8,rr2
	ld	r2,|_stkseg+6|(r15)
	cp	r2,|_stkseg+20|(r15)
	jr	ule,L9154
	ld	r3,|_stkseg+20|(r15)
	jr	L9158
L9154:
	ld	r3,|_stkseg+6|(r15)
L9158:
	ld	|_stkseg+6|(r15),r3
	jr	L9166
L915e:
	ldl	rr2,@rr10
	ldl	@rr8,rr2
	inc	r11,#4
	inc	r9,#4
L9166:
	ld	r2,|_stkseg+6|(r15)
	dec	|_stkseg+6|(r15),#1
	test	r2
	jr	ne,L915e
L9172:
	ldl	rr2,|_stkseg+16|(r15)
	ldm	r8,_stkseg+26(r15),#6
	add	r15,#%0026
	ret
	sub	r15,#%0016
	ldm	_stkseg+10(r15),r8,#6
	ldl	|_stkseg+0|(r15),rr6
	ld	r3,|_stkseg+2|(r15)
	mult	rr2,r6
	ld	|_stkseg+2|(r15),r3
	ld	r7,r3
	calr	L8cb4
	ldl	rr8,rr2
	testl	rr2
	jr	ne,L91ac
	subl	rr2,rr2
	jr	L91cc
L91ac:
	ldl	rr12,rr8
	ld	r2,|_stkseg+2|(r15)
	inc	r2,#3
	srl	r2,#2
	ld	r11,r2
	jr	L91c2
L91bc:
	subl	rr2,rr2
	ldl	@rr12,rr2
	inc	r13,#4
L91c2:
	dec	r11,#1
	cp	r11,#%0000
	jr	ge,L91bc
	ldl	rr2,rr8
L91cc:
	ldm	r8,_stkseg+10(r15),#6
	add	r15,#%0016
	ret
	dec	r15,#8
	ldm	|_stkseg+0|(r15),r4,#4
	ldl	rr2,rr6
	calr	L903a
	inc	r15,#8

	.data

_d1A90::
	.word	%FFF8
	.word	%0020
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
	.word	%0000
