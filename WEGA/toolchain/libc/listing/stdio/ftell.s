
fp		:= r15;
sp		:= r15;
/*~~ftell:*/

.code

_ftell::
{
	dec	fp,#~L2
	ld	10(fp),r14
	ld	@fp,r7
	ld	r6,r7
	cp	2(r6),#0
	jpr	ge,L20
	clr	2(r7)
L20:
	ld	r2,~L1(fp)
	ldb	rl3,6(r2)
	extsb	r3
	bit	r3,#0
	jpr	eq,L21
	ld	r3,2(r2)
	neg	r3
	ld	r14,r3
	jpr	L22
L20001:
	ldk	r14,#0
	ld	r2,~L1(fp)
	ldb	rl3,6(r2)
	extsb	r3
	bit	r3,#1
	jpr	eq,L22
	test	4(r2)
	jpr	eq,L22
	ldb	rl3,6(r2)
	extsb	r3
	and	r3,#4
	test	r3
	jpr	ne,L22
	ld	r2,~L1(fp)
	ld	r3,r2
	ld	r4,@r3
	sub	r4,4(r2)
	ld	r14,r4
L22:
	ld	r7,~L1(fp)
	ldb	rl6,7(r7)
	ldb	rl7,rl6
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	callr	_lseek
	ldl	~L1+2(fp),rr2
	cpl	rr2,#0
	jpr	ge,L26
	jpr	L20002
L21:
	ld	r2,~L1(fp)
	ldb	rl3,6(r2)
	extsb	r3
	and	r3,#130
	jpr	ne,L20001
	ldl	rr2,#-1
	jpr	L10000
L26:
	ld	r3,r14
	exts	rr2
	addl	rr2,~L1+2(fp)
	ldl	~L1+2(fp),rr2
L20002:
	ldl	rr2,~L1+2(fp)
L10000:
	ld	r14,10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ftell */

.data

