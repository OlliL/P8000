
fp		:= r15;
sp		:= r15;
/*~~getenv:*/

.code

_getenv::
{
	dec	fp,#~L2
	ldm	4(fp),r12,#3
	ld	r14,r7
	ld	r13,_environ
	jpr	L14
L20001:
	ld	r6,r13
	inc	r13,#2
	ld	r7,@r6
	ld	r6,r7
	ld	r7,r14
	callr	_nvmatch
	ld	r12,r2
	test	r2
	jpr	ne,L10000
L14:
	test	@r13
	jpr	ne,L20001
	sub	r2,r2
L10000:
	ldm	r12,4(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _getenv */

.data


.code

_nvmatch:
{
	dec	fp,#~L2
	ld	r4,r6
	ld	r1,r7
L18:
	ld	r2,r4
	inc	r4,#1
	ldb	rl3,@r2
	cpb	rl3,@r1
	jpr	ne,L19
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	cpb	rl3,#61
	jpr	ne,L18
L20002:
	ld	r2,r4
	jpr	L10003
L19:
	testb	@r1
	jpr	ne,L21
	cpb	-1(r4),#61
	jpr	eq,L20002
L21:
	sub	r2,r2
L10003:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _nvmatch */

.data

