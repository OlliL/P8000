	.seg
	.extern	_stkseg
	.code

_tolower::
	dec	r15,#6
	ld	_stkseg+4(r15),r13
	ld	r13,r7
	cp	r7,#%0041
	jr	lt,L9558
	cp	r7,#%005A
	jr	gt,L9558
	sub	r13,#%FFE0
L9558:
	ld	r2,r13
	ld	r13,_stkseg+4(r15)
	inc	r15,#6
	ret
