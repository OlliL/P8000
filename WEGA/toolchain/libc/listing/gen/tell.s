
fp		:= r15;
sp		:= r15;
/*~~tell:*/

.code

_tell::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r6,r7
	subl	rr4,rr4
	ldk	r3,#1
	callr	_lseek
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _tell */

.data

