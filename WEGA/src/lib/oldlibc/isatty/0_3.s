	.extern	_gtty
	.psec
	.code
_isatty::
	dec	r15,#8
	ld	%0000(r15),r7
	lda	r6,%0002(r15)
	ld	r7,%0000(r15)
	call	_gtty
	cp	r2,#%0000
	jr	ge,L001c
	sub	r2,r2
	jr	L001e
L001c:
	ldk	r2,#1
L001e:
	inc	r15,#8
	ret
