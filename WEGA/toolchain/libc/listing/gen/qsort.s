
fp		:= r15;
sp		:= r15;
.even
_qscmp:
.word	0
.even
_qses:
.word	0
/*~~qsort:*/

.code

_qsort::
{
	dec	fp,#~L2
	ldm	@fp,r4,#4
	ld	_qscmp,r4
	ld	_qses,r5
	ld	r3,r6
	mult	rr2,r5
	add	r7,r3
	ld	r6,r7
	ld	r7,~L1+6(fp)
	callr	_qs1
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _qsort */

.data


.code

_qs1:
{
	sub	fp,#~L2
	ldm	18(fp),r12,#3
	ldl	@fp,rr6
	ld	r12,_qses
L16:
	ld	r2,~L1(fp)
	sub	r2,~L1+2(fp)
	ld	~L1+14(fp),r2
	cp	r12,r2
	jpr	uge,L10003
	ld	r2,r12
	add	r2,r12
	ld	r5,~L1+14(fp)
	ldk	r4,#0
	div	rr4,r2
	ld	r3,r12
	mult	rr2,r5
	ld	~L1+14(fp),r3
	ld	r2,~L1+2(fp)
	add	r2,r3
	ld	~L1+8(fp),r2
	ld	~L1+10(fp),r2
	ld	r14,~L1+2(fp)
	ld	r2,~L1(fp)
	sub	r2,r12
L20011:
	ld	r13,r2
	jpr	L20
L20001:
	ld	r6,~L1+8(fp)
	sub	r6,r12
	ld	~L1+8(fp),r6
	ld	r7,r14
	callr	_qsexc
	jpr	L20
L20003:
	ld	r2,_qscmp
	ld	~L1+16(fp),r2
	ld	r7,r14
	ld	r6,~L1+8(fp)
	ld	r1,r2
	call	@r1
	ld	~L1+12(fp),r2
	test	r2
	jpr	eq,L20001
	cp	r2,#0
	jpr	ge,L25
L20008:
	add	r14,r12
L20:
	cp	r14,~L1+8(fp)
	jpr	ult,L20003
	jpr	L25
L20005:
	ld	r7,~L1+10(fp)
	add	r7,r12
	ld	~L1+10(fp),r7
	ld	r6,r13
	callr	_qsexc
	jpr	L25
L20007:
	ld	r6,~L1+10(fp)
	add	r6,r12
	ld	~L1+10(fp),r6
	ld	r7,r14
	ld	r5,r13
	callr	_qstexc
	ld	r2,~L1+8(fp)
	add	r2,r12
	ld	~L1+8(fp),r2
	ld	r14,r2
	jpr	L25
L20010:
	ld	r2,_qscmp
	ld	~L1+16(fp),r2
	ld	r7,~L1+10(fp)
	ld	r6,r13
	ld	r1,r2
	call	@r1
	ld	~L1+12(fp),r2
	test	r2
	jpr	eq,L20005
	cp	r2,#0
	jpr	le,L28
	cp	r14,~L1+8(fp)
	jpr	eq,L20007
	ld	r7,r14
	ld	r6,r13
	callr	_qsexc
	sub	r13,r12
	jpr	L20008
L28:
	sub	r13,r12
L25:
	cp	r13,~L1+10(fp)
	jpr	ugt,L20010
	cp	r14,~L1+8(fp)
	jpr	ne,L31
	ld	r2,~L1+8(fp)
	sub	r2,~L1+2(fp)
	ld	r3,~L1(fp)
	sub	r3,~L1+10(fp)
	cp	r2,r3
	jpr	lt,L32
	ld	r7,~L1+10(fp)
	add	r7,r12
	ld	r6,~L1(fp)
	callr	_qs1
	ld	r2,~L1+8(fp)
	ld	~L1(fp),r2
	jpr	L16
L32:
	ld	r7,~L1+2(fp)
	ld	r6,~L1+8(fp)
	callr	_qs1
	ld	r2,~L1+10(fp)
	add	r2,r12
	ld	~L1+2(fp),r2
	jpr	L16
L31:
	ld	r6,~L1+8(fp)
	sub	r6,r12
	ld	~L1+8(fp),r6
	ld	r7,r13
	ld	r5,r14
	callr	_qstexc
	ld	r2,~L1+10(fp)
	sub	r2,r12
	ld	~L1+10(fp),r2
	jpr	L20011
L10003:
	ldm	r12,18(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 24
	}	/* _qs1 */

.data


.code

_qsexc:
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,_qses
	ld	~L1+6(fp),r2
	ld	r1,r7
	ld	r4,r6
L37:
	ldb	rl2,@r1
	ldb	~L1+5(fp),rl2
	ldb	rl3,@r4
	ldb	@r1,rl3
	inc	r1,#1
	ldb	rl3,~L1+5(fp)
	ldb	@r4,rl3
	inc	r4,#1
	dec	~L1+6(fp),#1
	jpr	ne,L37
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _qsexc */

.data


.code

_qstexc:
{
	dec	fp,#~L2
	ld	10(fp),r12
	ldm	@fp,r5,#3
	ld	r4,_qses
	ld	~L1+8(fp),r4
	ld	r1,r7
	ld	r3,r6
	ld	r12,r5
L41:
	ldb	rl2,@r1
	extsb	r2
	ld	~L1+6(fp),r2
	ldb	rl2,@r12
	ldb	@r1,rl2
	inc	r1,#1
	ldb	rl2,@r3
	ldb	@r12,rl2
	inc	r12,#1
	ldb	rl2,~L1+7(fp)
	ldb	@r3,rl2
	inc	r3,#1
	dec	~L1+8(fp),#1
	jpr	ne,L41
	ld	r12,10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _qstexc */

.data

