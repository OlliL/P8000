
fp		:= r15;
sp		:= r15;
.even
L22:
.word	9
.word	32
.word	43
.word	45
.even
/*~~atol:*/

.code

_atol::
{
	.data
	.even
L23:
	.word	L12
	.word	L12
	.word	L20
	.word	L19
	.code
	dec	fp,#~L2
	ld	10(fp),r13
	ld	r1,r7
	subl	rr2,rr2
	ldl	~L1+2(fp),rr2
	ldk	r13,#0
	jpr	L14
L20001:
	cpb	@r1,#57
	jpr	gt,L25
	ldl	rr6,~L1+2(fp)
	multl	rq4,#10
	ldb	rl3,@r1
	extsb	r3
	exts	rr2
	addl	rr6,rr2
	subl	rr6,#48
	ldl	~L1+2(fp),rr6
	jpr	L20
L12:
	inc	r1,#1
L14:
	ldb	rl3,@r1
	extsb	r3
	ld	r2,r3
	ld	r3,#L22
	ld	r4,#4
	cpir	r2,@r3,r4,eq
	jpr	ne,L24
	add	r4,r4
	neg	r4
	ld	r2,L23+6(r4)
	jp	@r2
L19:
	inc	r13,#1
L20:
	inc	r1,#1
L24:
	cpb	@r1,#48
	jpr	ge,L20001
L25:
	test	r13
	jpr	eq,L10003
	subl	rr2,rr2
	subl	rr2,~L1+2(fp)
	jpr	L10000
L10003:
	ldl	rr2,~L1+2(fp)
L10000:
	ld	r13,10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _atol */

.data

