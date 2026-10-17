
fp		:= r15;
sp		:= r15;
/*~~_strout:*/

.code

__strout::
{
	dec	fp,#~L2
	ldm	10(fp),r12,#3
	ldm	@fp,r3,#3
	ld	r12,r4
	ld	r13,r6
	ld	r14,r7
	jpr	L20
L10003:
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r12
	inc	@r12,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	subb	rh4,rh4
	ld	r2,r4
L10004:
	dec	r13,#1
L22:
	dec	2(r12),#1
	cp	2(r12),#0
	jpr	ge,L10005
	ld	r7,~L1(fp)
	ld	r6,r12
	callr	__flsbuf
	jpr	L10006
L10005:
	ld	r2,@r12
	inc	@r12,#1
	ldb	rl3,~L1+1(fp)
	ldb	@r2,rl3
	subb	rh3,rh3
	ld	r2,r3
L10006:
	inc	~L1+4(fp),#1
L20:
	cp	~L1+4(fp),#0
	jpr	ge,L24
	cpb	@r14,#45
	jpr	ne,L22
	cp	~L1(fp),#48
	jpr	ne,L22
	dec	2(r12),#1
	cp	2(r12),#0
	jpr	ge,L10003
	ld	r7,r14
	inc	r14,#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,r12
	callr	__flsbuf
	jpr	L10004
L20003:
	ld	r7,r14
	inc	r14,#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,r12
	callr	__flsbuf
	jpr	L24
L20005:
	dec	2(r12),#1
	cp	2(r12),#0
	jpr	lt,L20003
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r12
	inc	@r12,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	subb	rh4,rh4
	ld	r2,r4
L24:
	dec	r13,#1
	cp	r13,#0
	jpr	ge,L20005
	jpr	L26
L20007:
	dec	2(r12),#1
	cp	2(r12),#0
	jpr	ge,L10009
	ld	r7,~L1(fp)
	ld	r6,r12
	callr	__flsbuf
	jpr	L10010
L10009:
	ld	r2,@r12
	inc	@r12,#1
	ldb	rl3,~L1+1(fp)
	ldb	@r2,rl3
	subb	rh3,rh3
	ld	r2,r3
L10010:
	dec	~L1+4(fp),#1
L26:
	test	~L1+4(fp)
	jpr	ne,L20007
	ldm	r12,10(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* __strout */

.data

