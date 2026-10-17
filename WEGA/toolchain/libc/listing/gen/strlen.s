
fp		:= r15;
sp		:= r15;
/*~~strlen:*/

.code

_strlen::
{
	dec	fp,#~L2
	ld	r1,r7
	ldk	r3,#0
	jpr	L12
L20001:
	inc	r3,#1
L12:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jpr	ne,L20001
	ld	r2,r3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _strlen */

.data

