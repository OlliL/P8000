	.extern	__iob, __lastbuf
	.psec
	.code
__findiop::
	dec	r15,#2
	ld	%0000(r15),r14
	ld	r14,#__iob
	jr	L0018
L000c:
	cp	r14,__lastbuf
	jr	c,L0016
	sub	r2,r2
	jr	L0026
L0016:
	inc	r14,#8
L0018:
	ldb	rl2,%0006(r14)
	extsb	r2
	and	r2,#%0083
	jr	ne,L000c
	ld	r2,r14
L0026:
	ld	r14,%0000(r15)
	inc	r15,#2
	ret
