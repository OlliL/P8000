
fp		:= r15;
sp		:= r15;
/*~~strcspn:*/

.code

_strcspn::
{
	dec	fp,#~L2
	ld	~L1+2(fp),r7
	ld	r1,r6
	ld	r4,r7
	jpr	L14
L20003:
	ld	r3,r1
	jpr	L17
L20001:
	ldb	rl2,@r3
	cpb	rl2,@r4
	jpr	eq,L16
	inc	r3,#1
L17:
	testb	@r3
	jpr	ne,L20001
L16:
	testb	@r3
	jpr	ne,L13
	inc	r4,#1
L14:
	testb	@r4
	jpr	ne,L20003
L13:
	ld	r2,r4
	sub	r2,~L1+2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _strcspn */

.data

