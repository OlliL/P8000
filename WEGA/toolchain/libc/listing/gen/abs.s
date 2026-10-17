
fp		:= r15;
sp		:= r15;
/*~~abs:*/

.code

_abs::
{
	dec	fp,#~L2
	ld	@fp,r7
	cp	r7,#0
	jpr	ge,L12
	neg	r7
	ld	~L1(fp),r7
L12:
	ld	r2,~L1(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _abs */

.data

