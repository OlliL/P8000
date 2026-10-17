
fp		:= r15;
sp		:= r15;
/*~~strncat:*/

.code

_strncat::
{
	dec	fp,#~L2
	ldm	8(fp),r11,#3
	ld	r12,r5
	ld	r13,r6
	ld	r1,r7
	ld	r11,r7
L12:
	ld	r2,r1
	inc	r1,#1
	testb	@r2
	jpr	ne,L12
	dec	r1,#1
L14:
	ld	r2,r13
	inc	r13,#1
	ld	r3,r1
	inc	r1,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jpr	eq,L15
	dec	r12,#1
	cp	r12,#0
	jpr	ge,L14
	dec	r1,#1
	clrb	@r1
L15:
	ld	r2,r11
	ldm	r11,8(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _strncat */

.data

