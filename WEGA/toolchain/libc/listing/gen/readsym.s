
fp		:= r15;
sp		:= r15;
/*~~readsym:*/

.code

_readsym::
{
	dec	fp,#~L2
	ld	8(fp),r14
	ldl	@fp,rr6
	ld	r2,r7
	ldk	r6,#14
	ldk	r5,#1
	ld	r4,~L1(fp)
	callr	_fread
	cp	r2,#1
	jpr	eq,L21
	ld	r2,#65535
	jpr	L10000
L21:
	ld	r2,~L1+2(fp)
	ldb	rl3,6(r2)
	extsb	r3
	bit	r3,#7
	jpr	ne,L22
	sub	r2,r2
	jpr	L10000
L22:
	ld	r2,~L1+2(fp)
	ldb	rl3,6(r2)
	extsb	r3
	and	r3,#127
	dec	r3,#8
	ldb	~L1+5(fp),rl3
	extsb	r3
	inc	r3,#13
	ld	r5,r3
	exts	rr4
	div	rr4,#14
	ld	r14,r5
	inc	~L1+2(fp),#14
	ld	r7,~L1+2(fp)
	ldk	r6,#14
	ld	r4,~L1(fp)
	callr	_fread
	cp	r14,r2
	jpr	eq,L23
	ld	r14,#65535
L23:
	ld	r2,r14
L10000:
	ld	r14,8(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _readsym */

.data

