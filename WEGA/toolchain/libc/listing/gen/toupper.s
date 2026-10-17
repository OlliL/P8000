
fp		:= r15;
sp		:= r15;
/*~~toupper:*/

.code

_toupper::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	r14,r7
	cp	r7,#97
	jpr	lt,L12
	cp	r7,#122
	jpr	gt,L12
	add	r14,#-32
L12:
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _toupper */

.data

