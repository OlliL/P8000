	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec
	.code
_strncmp::
	{
	ldm	r2,|_stkseg+4|(sp),#5	// rr2 <-> s1 , rr4 <-> s2 , r6 <-> n
	cpl	rr2,rr4
	jr	eq,~zeroout
~while:
	dec	r6,#1
	jpr	lt,L15
	ldb	rl0,@rr2
	ldb	rl1,@rr4
	inc	r5,#1
	cpb	rl0,rl1
	jpr	ne,L15
	inc	r3,#%1
	testb	rl0
	jr	nz,~while
~zeroout:
	xor	r2,r2
	ret
L15:
	test	r6
	jr	mi,~zeroout
	subb	rl0,rl1
	ret
	}
