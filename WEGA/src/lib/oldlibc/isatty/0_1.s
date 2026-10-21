	.extern	_gtty
	.psec
	.code
_isatty::
	dec	r15,#8
	ld	%0000(r15),r7
	lda	r6,%0002(r15)
	call	_gtty
	cp	r2,#%0000
	jr	ge,L0018
	sub	r2,r2
	jr	L001a
L0018:
	ldk	r2,#1
L001a:
	inc	r15,#8
	ret
