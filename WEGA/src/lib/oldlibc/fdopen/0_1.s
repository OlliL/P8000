	.extern	__findiop, _lseek
	.psec
	.code
_fdopen::
	dec	r15,#6
	ldm	%0002(r15),r13,#2
	ld	%0000(r15),r7
	ld	r14,r6
	call	__findiop
	ld	r13,r2
	test	r2
	jr	eq,L006a
	clr	%0002(r13)
	ldb	rl2,%0001(r15)
	ldb	%0007(r13),rl2
	ldb	rl2,@r14
	extsb	r2
	cp	r2,#%0061
	jr	eq,L006e
	cp	r2,#%0072
	jr	ne,L0064
	setb	%0006(r13),#0
L0038:
	cpb	%0001(r14),#%2B
	jr	eq,L0048
	cpb	%0001(r14),#%77
	jr	ne,L0060
L0048:
	ldb	rl2,%0006(r13)
	andb	rl2,#%FC
	ldb	%0006(r13),rl2
	ldb	rl2,%0006(r13)
	orb	rl2,#%80
	ldb	%0006(r13),rl2
L0060:
	ld	r2,r13
	jr	L0080
L0064:
	cp	r2,#%0077
	jr	eq,L007a
L006a:
	sub	r2,r2
	jr	L0080
L006e:
	ld	r7,%0000(r15)
	subl	rr4,rr4
	ldk	r3,#2
	call	_lseek
L007a:
	setb	%0006(r13),#1
	jr	L0038
L0080:
	ldm	r13,%0002(r15),#2
	inc	r15,#6
	ret
