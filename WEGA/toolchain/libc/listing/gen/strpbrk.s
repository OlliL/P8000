
fp		:= r15;
sp		:= r15;
/*~~strpbrk:*/

.code

_strpbrk::
{
	dec	fp,#~L2
	ld	r3,r6
	ld	r1,r7
L14:
	ld	r4,r3
	jpr	L17
L20001:
	ldb	rl2,@r4
	cpb	rl2,@r1
	jpr	eq,L16
	inc	r4,#1
L17:
	testb	@r4
	jpr	ne,L20001
L16:
	testb	@r4
	jpr	eq,L13
	ld	r2,r1
	jpr	L10000
L13:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jpr	ne,L14
	sub	r2,r2
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _strpbrk */

.data

