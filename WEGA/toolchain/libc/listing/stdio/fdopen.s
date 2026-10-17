
fp		:= r15;
sp		:= r15;
.even
L28:
.word	97
.word	114
.word	119
.even
/*~~fdopen:*/

.code

_fdopen::
{
	.data
	.even
L29:
	.word	L24
	.word	L23
	.word	L26
	.code
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ld	~L1+2(fp),r7
	ld	r14,r6
	callr	__findiop
	ld	r13,r2
	test	r2
	jpr	eq,L27
	clr	2(r13)
	ldb	rl2,~L1+3(fp)
	ldb	7(r13),rl2
	ldb	rl3,@r14
	extsb	r3
	ld	r2,r3
	ld	r3,#L28
	ld	r4,#3
	cpir	r2,@r3,r4,eq
	jpr	ne,L27
	add	r4,r4
	neg	r4
	ld	r2,L29+4(r4)
	jp	@r2
L23:
	setb	6(r13),#0
L21:
	cpb	1(r14),#43
	jpr	eq,L10003
	cpb	1(r14),#119
	jpr	ne,L30
L10003:
	ldb	rl2,6(r13)
	andb	rl2,#-4
	ldb	6(r13),rl2
	ldb	rl2,6(r13)
	orb	rl2,#-128
	ldb	6(r13),rl2
L30:
	ld	r2,r13
	jpr	L10000
L24:
	ld	r7,~L1+2(fp)
	subl	rr4,rr4
	ldk	r3,#2
	callr	_lseek
L26:
	setb	6(r13),#1
	jpr	L21
L27:
	sub	r2,r2
L10000:
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _fdopen */

.data

