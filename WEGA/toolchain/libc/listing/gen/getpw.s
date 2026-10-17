
fp		:= r15;
sp		:= r15;
.even
L20:
.word	0
L22:
.byte	%2f,%65,%74,%63,%2f,%70,%61,%73
.byte	%73,%77,%64,%0
.even
L23:
.byte	%72,%0
.even
/*~~getpw:*/

.code

_getpw::
{
	dec	fp,#~L2
	ldm	6(fp),r12,#3
	ldl	@fp,rr6
	test	L20
	jpr	ne,L21
	ld	r7,#L22
	ld	r6,#L23
	callr	_fopen
	ld	L20,r2
L21:
	test	L20
	jpr	ne,L24
L20002:
	ldk	r2,#1
	jpr	L10000
L24:
	ld	r7,L20
	callr	_rewind
L28:
	ld	r12,~L1(fp)
	jpr	L30
L20001:
	cp	r13,#-1
	jpr	eq,L20002
	ld	r2,r13
	ldb	@r12,rl2
	inc	r12,#1
L30:
	ld	r2,L20
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10003
	ld	r7,L20
	callr	__filbuf
	jpr	L10004
L10003:
	ld	r2,L20
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10004:
	ld	r13,r2
	cp	r2,#10
	jpr	ne,L20001
	clrb	@r12
	inc	r12,#1
	ld	r12,~L1(fp)
	ldk	r14,#3
L33:
	dec	r14,#1
	jpr	eq,L38
L35:
	ld	r2,r12
	inc	r12,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r13,r3
	cp	r3,#58
	jpr	eq,L33
	cp	r3,#10
	jpr	ne,L35
	jpr	L20002
L38:
	ld	r2,r12
	inc	r12,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r13,r3
	cp	r3,#58
	jpr	eq,L39
	cp	r3,#48
	jpr	lt,L38
	cp	r3,#57
	jpr	gt,L38
	ld	r3,r14
	mult	rr2,#10
	add	r3,r13
	sub	r3,#48
	ld	r14,r3
	jpr	L38
L39:
	cp	r14,~L1+2(fp)
	jpr	ne,L28
	sub	r2,r2
L10000:
	ldm	r12,6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _getpw */

.data

