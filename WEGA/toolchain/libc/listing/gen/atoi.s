
fp		:= r15;
sp		:= r15;
.even
L22:
.word	9
.word	32
.word	43
.word	45
.even
/*~~atoi:*/

.code

_atoi::
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
	ld	r1,r7
	ldk	r5,#0
	ldk	r6,#0
	jpr	L14
L20001:
	cpb	@r1,#57
	jpr	gt,L25
	ld	r3,r5
	mult	rr2,#10
	ldb	rl2,@r1
	extsb	r2
	add	r3,r2
	sub	r3,#48
	ld	r5,r3
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
	inc	r6,#1
L20:
	inc	r1,#1
L24:
	cpb	@r1,#48
	jpr	ge,L20001
L25:
	test	r6
	jpr	eq,L10003
	ld	r2,r5
	neg	r2
	jpr	L10000
L10003:
	ld	r2,r5
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _atoi */

.data

