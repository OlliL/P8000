
fp		:= r15;
sp		:= r15;
/*~~a64l:*/

.code

_a64l::
{
	dec	fp,#~L2
	ld	10(fp),r12
	ld	r1,r7
	ldk	r6,#0
	subl	rr2,rr2
	ldl	~L1+2(fp),rr2
	jpr	L12
L20001:
	cp	r12,#97
	jpr	lt,L14
	dec	r12,#6
L14:
	cp	r12,#65
	jpr	lt,L15
	dec	r12,#7
L15:
	ld	r2,r12
	sub	r2,#46
	ld	r5,r2
	exts	rr4
	sdal	rr4,r6
	ldl	rr2,~L1+2(fp)
	or	r2,r4
	or	r3,r5
	ldl	~L1+2(fp),rr2
	inc	r6,#6
L12:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r12,r3
	test	r3
	jpr	ne,L20001
	ldl	rr2,~L1+2(fp)
	ld	r12,10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _a64l */

.data

