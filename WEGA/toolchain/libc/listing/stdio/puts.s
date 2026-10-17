
fp		:= r15;
sp		:= r15;
/*~~puts:*/

.code

_puts::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r14,r7
	jpr	L20
L20001:
	ld	r7,r13
	ld	r6,#__iob+8
	callr	__flsbuf
	jpr	L20
L20003:
	dec	__iob+10,#1
	cp	__iob+10,#0
	jpr	lt,L20001
	ld	r2,r13
	ld	r3,__iob+8
	inc	__iob+8,#1
	ldb	@r3,rl2
	subb	rh2,rh2
L20:
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r13,r3
	test	r3
	jpr	ne,L20003
	dec	__iob+10,#1
	cp	__iob+10,#0
	jpr	ge,L10005
	ldk	r7,#10
	ld	r6,#__iob+8
	callr	__flsbuf
	jpr	L10000
L10005:
	ld	r2,__iob+8
	inc	__iob+8,#1
	ldb	@r2,#10
	ldb	rl2,#10
	subb	rh2,rh2
L10000:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _puts */

.data

