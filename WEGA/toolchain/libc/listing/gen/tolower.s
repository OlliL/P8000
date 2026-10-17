
fp		:= r15;
sp		:= r15;
/*~~tolower:*/

.code

_tolower::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	r14,r7
	cp	r7,#65
	jpr	lt,L12
	cp	r7,#90
	jpr	gt,L12
	sub	r14,#-32
L12:
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _tolower */

.data

