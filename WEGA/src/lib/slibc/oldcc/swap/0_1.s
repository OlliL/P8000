// swap.o: magic E607 segmented
	.seg
	.psec
	.code
	.extern	_swap
	.extern	_stkseg
	.extern	_longswap
	.extern	_swapsegt
	.psec	data
	.data

// segment table entry 0 (segment 0), text, 190 bytes
	.psec
	.code
_swap::
	dec	r15,#10
	ld	|_stkseg|(r15),r7
	ldl	rr2,#_stkseg+2
	add	r3,r15
	ldl	rr4,rr2
	ldb	rl6,|_stkseg+1|(r15)
	ldb	@rr4,rl6
	ldl	rr2,rr4
	inc	r3,#1
	ldb	rl6,|_stkseg|(r15)
	ldb	@rr2,rl6
	ld	r2,@rr4
	inc	r15,#10
	ret
_longswap::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg|(r15),rr6
	ldl	rr12,|_stkseg|(r15)
	test	@rr12
	jr	eq,L0t_0042
	ld	r7,@rr12
	calr	_swap
	ld	@rr12,r2
L0t_0042:
	inc	r13,#2
	test	@rr12
	jr	eq,L0t_004e
	ld	r7,@rr12
	calr	_swap
	ld	@rr12,r2
L0t_004e:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
_swapsegt::
	dec	r15,#8
	ldm	_stkseg+4(r15),r12,#2
	ldl	|_stkseg|(r15),rr6
	ldl	rr12,rr6
	ld	r2,rr12(#%0004)
	test	r2
	jr	eq,L0t_007c
	ld	r7,r2
	calr	_swap
	ldl	rr4,rr12
	inc	r5,#4
	ld	@rr4,r2
L0t_007c:
	ld	r2,rr12(#%0006)
	test	r2
	jr	eq,L0t_008e
	ld	r7,r2
	calr	_swap
	ldl	rr4,rr12
	inc	r5,#6
	ld	@rr4,r2
L0t_008e:
	ld	r2,rr12(#%0008)
	test	r2
	jr	eq,L0t_00a0
	ld	r7,r2
	calr	_swap
	ldl	rr4,rr12
	inc	r5,#8
	ld	@rr4,r2
L0t_00a0:
	ld	r2,rr12(#%000A)
	test	r2
	jr	eq,L0t_00b2
	ld	r7,r2
	calr	_swap
	ldl	rr4,rr12
	inc	r5,#10
	ld	@rr4,r2
L0t_00b2:
	ldm	r12,_stkseg+4(r15),#2
	inc	r15,#8
	ret
