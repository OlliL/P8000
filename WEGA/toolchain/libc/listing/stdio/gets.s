
fp		:= r15;
sp		:= r15;
/*~~gets:*/

.code

_gets::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	@fp,r7
	ld	r13,r7
	jpr	L20
L20001:
	cp	r14,#0
	jpr	lt,L21
	ld	r2,r14
	ldb	@r13,rl2
	inc	r13,#1
L20:
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10003
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10004
L10003:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10004:
	ld	r14,r2
	cp	r2,#10
	jpr	ne,L20001
L21:
	cp	r14,#0
	jpr	ge,L22
	cp	r13,~L1(fp)
	jpr	ne,L22
	sub	r2,r2
	jpr	L10000
L22:
	clrb	@r13
	inc	r13,#1
	ld	r2,~L1(fp)
L10000:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _gets */

.data

