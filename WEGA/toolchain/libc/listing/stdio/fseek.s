
fp		:= r15;
sp		:= r15;
/*~~fseek:*/

.code

_fseek::
{
	sub	fp,#~L2
	ldm	16(fp),r13,#2
	ldm	@fp,r3,#3
	ld	r14,r7
	resb	6(r14),#4
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#0
	jpr	eq,L21
	cp	r3,#2
	jpr	ge,L22
	test	4(r14)
	jpr	eq,L22
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#2
	jpr	ne,L22
	ld	r13,2(r14)
	ldl	rr2,~L1+2(fp)
	ldl	~L1+10(fp),rr2
	test	~L1(fp)
	jpr	ne,L23
	ldb	rl7,7(r14)
	extsb	r7
	subl	rr4,rr4
	ldk	r3,#1
	callr	_lseek
	ld	r5,r13
	exts	rr4
	subl	rr4,rr2
	addl	rr4,~L1+10(fp)
	ldl	~L1+10(fp),rr4
	jpr	L24
L23:
	ld	r3,r13
	exts	rr2
	ldl	rr4,~L1+2(fp)
	subl	rr4,rr2
	ldl	~L1+2(fp),rr4
L24:
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	ne,L22
	cp	r13,#0
	jpr	le,L22
	ld	r3,r13
	exts	rr2
	cpl	rr2,~L1+10(fp)
	jpr	lt,L22
	ld	r2,4(r14)
	sub	r2,@r14
	ld	r5,r2
	exts	rr4
	cpl	rr4,~L1+10(fp)
	jpr	gt,L22
	ld	r2,~L1+12(fp)
	add	r2,@r14
	ld	@r14,r2
	ld	r2,~L1+12(fp)
	ld	r3,2(r14)
	sub	r3,r2
	ld	2(r14),r3
	jpr	L10003
L22:
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	eq,L26
	ld	r2,4(r14)
	ld	@r14,r2
	resb	6(r14),#0
L26:
	ldb	rl7,7(r14)
	extsb	r7
	ldl	rr4,~L1+2(fp)
	ld	r3,~L1(fp)
	callr	_lseek
	ldl	~L1+10(fp),rr2
	clr	2(r14)
	jpr	L27
L21:
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#130
	jpr	eq,L27
	ld	r7,r14
	callr	_fflush
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	eq,L30
	clr	2(r14)
	resb	6(r14),#1
	ld	r2,4(r14)
	ld	@r14,r2
L30:
	ldb	rl7,7(r14)
	extsb	r7
	ldl	rr4,~L1+2(fp)
	ld	r3,~L1(fp)
	callr	_lseek
	ldl	~L1+10(fp),rr2
L27:
	ldl	rr2,~L1+10(fp)
	cpl	rr2,#-1
	jpr	ne,L10003
	ld	r2,#-1
	jpr	L10000
L10003:
	sub	r2,r2
L10000:
	ldm	r13,16(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 20
	}	/* _fseek */

.data

