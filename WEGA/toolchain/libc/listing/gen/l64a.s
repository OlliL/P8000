
fp		:= r15;
sp		:= r15;
L12:
.=.+6
.byte	0
.byte	0
/*~~l64a:*/

.code

_l64a::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ldk	r1,#6
	ld	r5,#L12
	jpr	L13
L20001:
	ldl	rr2,~L1(fp)
	testl	rr2
	jpr	eq,L14
	ldk	r2,#0
	and	r3,#63
	addl	rr2,#46
	ld	r4,r3
	cp	r3,#57
	jpr	le,L15
	inc	r4,#7
L15:
	cp	r4,#90
	jpr	le,L16
	inc	r4,#6
L16:
	ld	r2,r4
	ldb	@r5,rl2
	inc	r5,#1
	ldl	rr2,~L1(fp)
	sral	rr2,#6
	ldl	~L1(fp),rr2
L13:
	dec	r1,#1
	cp	r1,#0
	jpr	gt,L20001
L14:
	clrb	@r5
	ld	r2,#L12
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _l64a */

.data

