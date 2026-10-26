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
	jr	ge,Ld01c
	sub	r2,r2
	jr	Ld01e
Ld01c:
	ldk	r2,#1
Ld01e:
	inc	r15,#8
	ret
