
fp		:= r15;
sp		:= r15;
/*~~fputs:*/

.code

_fputs::
{
	dec	fp,#~L2
	ldm	6(fp),r11,#4
	ld	r13,r6
	ld	r14,r7
	jpr	L20
L20001:
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10003
	ld	r7,r11
	ld	r6,r13
	callr	__flsbuf
	jpr	L10004
L10003:
	ld	r2,r11
	ld	r3,@r13
	inc	@r13,#1
	ldb	@r3,rl2
	subb	rh2,rh2
L10004:
	ld	r12,r2
L20:
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r11,r3
	test	r3
	jpr	ne,L20001
	ld	r2,r12
	ldm	r11,6(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _fputs */

.data

