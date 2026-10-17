
fp		:= r15;
sp		:= r15;
/*~~strcmp:*/

.code

_strcmp::
{
	dec	fp,#~L2
	ld	r4,r6
	ld	r1,r7
L12:
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
	sub	r2,r2
	jpr	L10000
L13:
	ldb	rl2,@r1
	extsb	r2
	dec	r4,#1
	ldb	rl3,@r4
	extsb	r3
	sub	r2,r3
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _strcmp */

.data

