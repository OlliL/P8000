
fp		:= r15;
sp		:= r15;
/*~~strcpy:*/

.code

_strcpy::
{
	dec	fp,#~L2
	ld	6(fp),r12
	ld	r5,r6
	ld	r1,r7
	ld	r12,r7
L12:
	ld	r2,r5
	inc	r5,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jpr	ne,L12
	ld	r2,r12
	ld	r12,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _strcpy */

.data

