
fp		:= r15;
sp		:= r15;
/*~~putw:*/

.code

_putw::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r13,r6
	ld	r14,r7
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10003
	ld	r7,r14
	ldb	rl7,rh7
	clrb	rh7
	ld	r6,r13
	callr	__flsbuf
	jpr	L10004
L10003:
	ld	r2,r14
	ldb	rl2,rh2
	extsb	r2
	ld	r3,@r13
	inc	@r13,#1
	ldb	@r3,rl2
	subb	rh2,rh2
L10004:
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10005
	ld	r7,r14
	ld	r6,r13
	callr	__flsbuf
	jpr	L10000
L10005:
	ld	r2,r14
	ld	r3,@r13
	inc	@r13,#1
	ldb	@r3,rl2
	subb	rh2,rh2
L10000:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _putw */

.data

