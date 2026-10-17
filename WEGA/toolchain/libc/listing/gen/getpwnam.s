
fp		:= r15;
sp		:= r15;
/*~~getpwnam:*/

.code

_getpwnam::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	@fp,r7
	callr	_setpwent
L15:
	callr	_getpwent
	ld	r14,r2
	test	r2
	jpr	eq,L16
	ld	r7,~L1(fp)
	ld	r6,@r14
	callr	_strcmp
	test	r2
	jpr	ne,L15
L16:
	callr	_endpwent
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _getpwnam */

.data

