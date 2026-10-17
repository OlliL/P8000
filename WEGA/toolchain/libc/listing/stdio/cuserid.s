
fp		:= r15;
sp		:= r15;
L20:
.=.+8
.byte	0
.byte	0
/*~~cuserid:*/

.code

_cuserid::
{
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ld	@fp,r7
	test	r7
	jpr	ne,L10003
	ld	r6,#L20
	jpr	L10004
L10003:
	ld	r6,~L1(fp)
L10004:
	ld	r13,r6
	callr	_getlogin
	ld	r14,r2
	test	r2
	jpr	eq,L24
	ld	r7,r13
	ld	r6,r2
	callr	_strcpy
	jpr	L26
L24:
	callr	_getuid
	ld	~L1+4(fp),r2
	ld	r7,r2
	callr	_getpwuid
	ld	~L1+2(fp),r2
	test	r2
	jpr	ne,L27
	clrb	@r13
	ldk	r13,#0
	jpr	L28
L27:
	ld	r6,~L1+2(fp)
	ld	r7,@r6
	ld	r6,r7
	ld	r7,r13
	callr	_strcpy
L28:
	callr	_endpwent
L26:
	ld	r2,r13
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _cuserid */

.data

