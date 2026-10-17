
fp		:= r15;
sp		:= r15;
/*~~getgrgid:*/

.code

_getgrgid::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r14,r7
	callr	_setgrent
L14:
	callr	_getgrent
	ld	r13,r2
	test	r2
	jpr	eq,L15
	cp	r14,4(r13)
	jpr	ne,L14
L15:
	callr	_endgrent
	ld	r2,r13
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _getgrgid */

.data

