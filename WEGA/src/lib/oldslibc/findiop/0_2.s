	.seg
	.extern	__lastbuf
	.extern	__iob
	.psec
	.code

__findiop::
	dec	r15,#12
	ldl	rr4,#__iob
	jr	Labb6
Laba8:
	cpl	rr4,__lastbuf
	jr	c,Labb4
	subl	rr2,rr2
	jr	Labc4
Labb4:
	inc	r5,#12
Labb6:
	ldb	rl2,rr4(#%000A)
	extsb	r2
	and	r2,#%0083
	jr	ne,Laba8
	ldl	rr2,rr4
Labc4:
	inc	r15,#12
	ret
