
fp		:= r15;
sp		:= r15;
/*~~index:*/

.code

_index::
{
	dec	fp,#~L2
	ld	6(fp),r14
	ld	@fp,r6
	ld	r14,r7
L14:
	ldb	rl2,@r14
	cpb	rl2,~L1+1(fp)
	jpr	ne,L13
	ld	r2,r14
	jpr	L10000
L13:
	ld	r2,r14
	inc	r14,#1
	testb	@r2
	jpr	ne,L14
	sub	r2,r2
L10000:
	ld	r14,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _index */

.data

