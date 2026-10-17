
fp		:= r15;
sp		:= r15;
/*~~_length:*/

.code

__length::
{
	dec	fp,#~L2
	ld	r1,r7
	ldk	r3,#0
L14:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jpr	eq,L12
	inc	r3,#1
	cp	r3,#16384
	jpr	le,L14
L12:
	ld	r2,r3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* __length */

.data

