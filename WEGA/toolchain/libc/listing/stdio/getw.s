
fp		:= r15;
sp		:= r15;
/*~~getw:*/

.code

_getw::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r14,r7
	dec	2(r14),#1
	cp	2(r14),#0
	jpr	ge,L10003
	ld	r7,r14
	callr	__filbuf
	jpr	L10004
L10003:
	ld	r2,@r14
	inc	@r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10004:
	ld	r13,r2
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#4
	jpr	eq,L21
	ld	r2,#-1
	jpr	L10000
L21:
	dec	2(r14),#1
	cp	2(r14),#0
	jpr	ge,L10005
	ld	r7,r14
	callr	__filbuf
	jpr	L10006
L10005:
	ld	r2,@r14
	inc	@r14,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10006:
	ld	r3,r13
	ldb	rh3,rl3
	clrb	rl3
	or	r3,r2
	ld	r2,r3
L10000:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _getw */

.data

