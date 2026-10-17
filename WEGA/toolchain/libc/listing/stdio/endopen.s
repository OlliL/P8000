
fp		:= r15;
sp		:= r15;
.even
L34:
.word	97
.word	114
.word	119
.even
/*~~_endopen:*/

.code

__endopen::
{
	.data
	.even
L35:
	.word	L27
	.word	L32
	.word	L25
	.code
	dec	fp,#~L2
	ldm	8(fp),r12,#3
	ldl	~L1+2(fp),rr6
	ld	r14,r5
	test	r5
	jpr	ne,L21
L20001:
	sub	r2,r2
	jpr	L10000
L21:
	ld	r2,~L1+2(fp)
	cpb	@r2,#114
	jpr	ne,L10004
	cpb	1(r2),#119
	jpr	eq,L10003
L10004:
	ld	r2,~L1+2(fp)
	cpb	@r2,#119
	jpr	ne,L22
	cpb	1(r2),#114
	jpr	ne,L22
L10003:
	ld	r2,~L1+2(fp)
	ldb	@r2,#114
	ld	r2,~L1+2(fp)
	ldb	1(r2),#43
L22:
	ld	r2,~L1+2(fp)
	cpb	1(r2),#43
	ldk	r3,#0
	tcc	eq,r3
	extsb	r3
	ld	r13,r3
	ld	r2,~L1+2(fp)
	ldb	rl4,@r2
	extsb	r4
	ld	r3,r4
	ld	r2,#L34
	ld	r4,#3
	cpir	r3,@r2,r4,eq
	jpr	ne,L20001
	add	r4,r4
	neg	r4
	ld	r3,L35+4(r4)
	jp	@r3
L25:
	ld	r7,~L1+4(fp)
	ld	r6,r13
	callr	_create
	jpr	L20000
L27:
	test	r13
	jpr	eq,L10006
	ldk	r6,#2
	jpr	L10007
L10006:
	ldk	r6,#1
L10007:
	ld	r7,~L1+4(fp)
	callr	_open
	ld	r12,r2
	cp	r2,#0
	jpr	ge,L29
	cp	_errno,#2
	jpr	ne,L29
	ld	r7,~L1+4(fp)
	ld	r6,r13
	callr	_create
	ld	r12,r2
L29:
	ld	r7,r12
	subl	rr4,rr4
	ldk	r3,#2
	callr	_lseek
	jpr	L23
L32:
	test	r13
	jpr	eq,L10008
	ldk	r6,#2
	jpr	L10009
L10008:
	sub	r6,r6
L10009:
	ld	r7,~L1+4(fp)
	callr	_open
L20000:
	ld	r12,r2
L23:
	cp	r12,#0
	jpr	lt,L20001
	clr	2(r14)
	ld	r2,r12
	ldb	7(r14),rl2
	test	r13
	jpr	eq,L37
	ldb	rl2,6(r14)
	orb	rl2,#-128
	ldb	6(r14),rl2
	jpr	L38
L37:
	ld	r2,~L1+2(fp)
	cpb	@r2,#114
	jpr	ne,L39
	setb	6(r14),#0
	jpr	L38
L39:
	setb	6(r14),#1
L38:
	ld	r2,r14
L10000:
	ldm	r12,8(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* __endopen */

.data


.code

_create:
{
	dec	fp,#~L2
	ldm	6(fp),r12,#3
	ld	@fp,r6
	ld	r14,r7
	test	r6
	jpr	eq,L42
	sub	r7,r7
	callr	_umask
	ld	r13,r2
	ld	r7,r14
	ld	r6,#438
	callr	_creat
	ld	r12,r2
	cp	r2,#0
	jpr	lt,L45
	ld	r7,r2
	callr	_close
	ld	r7,r14
	ldk	r6,#2
	callr	_open
	ld	r12,r2
	ld	r6,r13
	com	r6
	and	r6,#438
	ld	r7,r14
	callr	_chmod
L45:
	ld	r7,r13
	callr	_umask
	jpr	L48
L42:
	ld	r7,r14
	ld	r6,#438
	callr	_creat
	ld	r12,r2
L48:
	ld	r2,r12
	ldm	r12,6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _create */

.data

