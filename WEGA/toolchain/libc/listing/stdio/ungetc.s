
fp		:= r15;
sp		:= r15;
/*~~ungetc:*/

.code

_ungetc::
{
	dec	fp,#~L2
	ld	6(fp),r14
	ld	~L1+2(fp),r7
	ld	r14,r6
	cp	r7,#-1
	jpr	eq,L22
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#1
	test	r2
	jpr	eq,L10003
	ld	r2,@r14
	cp	r2,4(r14)
	jpr	ugt,L21
L10003:
	ld	r2,@r14
	cp	r2,4(r14)
	jpr	ne,L22
	test	2(r14)
	jpr	ne,L22
	inc	@r14,#1
L21:
	inc	2(r14),#1
	dec	@r14,#1
	ld	r2,@r14
	ldb	rl3,~L1+3(fp)
	ldb	@r2,rl3
	sub	r2,r2
	jpr	L10000
L22:
	ld	r2,#-1
L10000:
	ld	r14,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ungetc */

.data

