
fp		:= r15;
sp		:= r15;
L21:
.byte	%2f,%0
.even
/*~~mktemp:*/

.code

_mktemp::
{
	dec	fp,#~L2
	ldm	4(fp),r12,#3
	ld	@fp,r7
	callr	_getpid
	ld	r13,r2
	ld	r14,~L1(fp)
L13:
	ld	r2,r14
	inc	r14,#1
	testb	@r2
	jpr	ne,L13
	dec	r14,#1
	jpr	L15
L20001:
	ld	r3,r13
	ldk	r2,#0
	div	rr2,#10
	add	r2,#48
	ldb	@r14,rl2
	ld	r3,r13
	ldk	r2,#0
	div	rr2,#10
	ld	r13,r3
L15:
	dec	r14,#1
	ldb	rl2,@r14
	cpb	rl2,#88
	jpr	eq,L20001
	inc	r14,#1
	ld	r12,#97
	jpr	L18
L20003:
	cp	r12,#122
	jpr	ne,L20
	ld	r2,#L21
	jpr	L10000
L20:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r12,#1
L18:
	ld	r7,~L1(fp)
	sub	r6,r6
	callr	_access
	cp	r2,#-1
	jpr	ne,L20003
	ld	r2,~L1(fp)
L10000:
	ldm	r12,4(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _mktemp */

.data

