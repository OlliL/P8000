
fp		:= r15;
sp		:= r15;
/*~~strncpy:*/

.code

_strncpy::
{
	dec	fp,#~L2
	ldm	8(fp),r11,#3
	ld	@fp,r5
	ld	r13,r6
	ld	r1,r7
	ld	r11,r7
	ldk	r12,#0
	jpr	L14
L20003:
	ld	r2,r13
	inc	r13,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jpr	eq,L16
	jpr	L12
L20001:
	clrb	@r1
	inc	r1,#1
L16:
	inc	r12,#1
	cp	r12,~L1(fp)
	jpr	lt,L20001
	jpr	L20004
L12:
	inc	r12,#1
L14:
	cp	r12,~L1(fp)
	jpr	lt,L20003
L20004:
	ld	r2,r11
	ldm	r11,8(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _strncpy */

.data

