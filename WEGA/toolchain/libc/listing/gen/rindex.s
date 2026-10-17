
fp		:= r15;
sp		:= r15;
/*~~rindex:*/

.code

_rindex::
{
	dec	fp,#~L2
	ld	@fp,r6
	ld	r1,r7
	ldk	r3,#0
L14:
	ldb	rl2,@r1
	cpb	rl2,~L1+1(fp)
	jpr	ne,L13
	ld	r3,r1
L13:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jpr	ne,L14
	ld	r2,r3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _rindex */

.data

