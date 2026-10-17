
fp		:= r15;
sp		:= r15;
/*~~setbuf:*/

.code

_setbuf::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	@fp,r6
	ld	r14,r7
	test	4(r14)
	jpr	eq,L20
	ldb	rl7,6(r14)
	extsb	r7
	bit	r7,#3
	jpr	eq,L20
	ld	r7,4(r14)
	callr	_free
L20:
	ldb	rl2,6(r14)
	andb	rl2,#-13
	ldb	6(r14),rl2
	ld	r2,~L1(fp)
	ld	4(r14),r2
	test	r2
	jpr	ne,L22
	setb	6(r14),#2
	jpr	L23
L22:
	ld	r2,4(r14)
	ld	@r14,r2
L23:
	clr	2(r14)
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _setbuf */

.data

