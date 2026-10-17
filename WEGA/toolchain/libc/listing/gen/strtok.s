
fp		:= r15;
sp		:= r15;
.even
L14:
.word	0
/*~~strtok:*/

.code

_strtok::
{
	dec	fp,#~L2
	ldm	6(fp),r12,#3
	ldl	@fp,rr6
	test	r7
	jpr	ne,L15
	ld	r14,L14
	jpr	L16
L15:
	ld	r14,~L1+2(fp)
L16:
	test	r14
	jpr	ne,L17
L20000:
	sub	r2,r2
	jpr	L10000
L17:
	ld	r7,r14
	ld	r6,~L1(fp)
	callr	_strspn
	add	r2,r14
	ld	r13,r2
	testb	@r13
	jpr	eq,L20000
	ld	r7,r2
	ld	r6,~L1(fp)
	callr	_strpbrk
	ld	r12,r2
	test	r2
	jpr	ne,L19
	clr	L14
	jpr	L20
L19:
	clrb	@r12
	ld	r7,r12
	ld	r6,~L1(fp)
	callr	_strspn
	add	r2,r12
	ld	r14,r2
	cp	r2,r12
	jpr	ule,L10003
	inc	r14,#1
	ld	r2,r14
	jpr	L10004
L10003:
	inc	r12,#1
	ld	r2,r12
L10004:
	ld	L14,r2
L20:
	ld	r2,r13
L10000:
	ldm	r12,6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _strtok */

.data

