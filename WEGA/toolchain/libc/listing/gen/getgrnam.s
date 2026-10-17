
fp		:= r15;
sp		:= r15;
/*~~getgrnam:*/

.code

_getgrnam::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r14,r7
	callr	_setgrent
L15:
	callr	_getgrent
	ld	r13,r2
	test	r2
	jpr	eq,L16
	ld	r7,@r13
	ld	r6,r14
	callr	_strcmp
	test	r2
	jpr	ne,L15
L16:
	callr	_endgrent
	ld	r2,r13
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _getgrnam */

.data

