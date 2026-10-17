
fp		:= r15;
sp		:= r15;
/*~~strncmp:*/

.code

_strncmp::
{
	dec	fp,#~L2
	ld	8(fp),r12
	ld	r12,r5
	ld	r4,r6
	ld	r1,r7
L12:
	dec	r12,#1
	cp	r12,#0
	jpr	lt,L13
	ld	r2,r4
	inc	r4,#1
	ldb	rl3,@r2
	cpb	rl3,@r1
	jpr	ne,L13
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	testb	rl3
	jpr	ne,L12
L20000:
	sub	r2,r2
	jpr	L10000
L13:
	cp	r12,#0
	jpr	lt,L20000
	ldb	rl2,@r1
	extsb	r2
	dec	r4,#1
	ldb	rl3,@r4
	extsb	r3
	sub	r2,r3
L10000:
	ld	r12,8(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _strncmp */

.data

