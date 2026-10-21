	.comm	2, _swap_flg
	.extern	_swap
	.psec
	.code
_goodmagic::
	dec	r15,#8
	ld	%0006(r15),r14
	ld	@r15,r7
	clr	_swap_flg
L000c:
	ld	r2,%0000(r15)
	ld	_magics,r2
	ldk	r14,#10
L0016:
	dec	r14,#1
	ld	r2,r14
	add	r2,r14
	ld	r3,%0000(r15)
	cp	r3,_magics(r2)
	jr	ne,L0016
	test	r14
	jr	ne,L0040
	ld	r7,r3
	call	_swap
	ld	%0000(r15),r2
	ld	r2,_swap_flg
	inc	_swap_flg,#1
	test	r2
	jr	eq,L000c
L0040:
	test	r14
	jr	ne,L0048
	clr	_swap_flg
L0048:
	ld	r2,r14
	ld	r14,%0006(r15)
	inc	r15,#8
	ret
	.psec
	.data
_magics::
	.byte	%00,%00,%e7,%07,%e7,%11,%e7,%05
	.byte	%e6,%07,%e6,%11,%e6,%05,%e5,%07
	.byte	%e5,%11,%e5,%05
