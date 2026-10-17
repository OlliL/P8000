
fp		:= r15;
sp		:= r15;
/*~~fgets:*/

.code

_fgets::
{
	dec	fp,#~L2
	ldm	8(fp),r12,#3
	ldl	~L1+2(fp),rr6
	ld	r14,r5
	ld	r12,r7
L20:
	dec	~L1+2(fp),#1
	cp	~L1+2(fp),#0
	jpr	le,L21
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
	cp	r2,#0
	jpr	lt,L21
	ldb	@r12,rl2
	inc	r12,#1
	cp	r13,#10
	jpr	ne,L20
L21:
	cp	r13,#0
	jpr	ge,L23
	cp	r12,~L1+4(fp)
	jpr	ne,L23
	sub	r2,r2
	jpr	L10000
L23:
	clrb	@r12
	inc	r12,#1
	ld	r2,~L1+4(fp)
L10000:
	ldm	r12,8(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _fgets */

.data

