	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec
	.code
_strncat::
{
	ldm	r2,|_stkseg+4|(sp),#5	// rr2 <-> s1 , rr4 <-> s2 , r6 <-> n
	subl	rr0,rr0
	cpirb	rl0,@rr2,r1,eq
	dec	r3,#%1		// 0 Byte of s1
	ld	r0,r3		// offset(s1)
	ld	r1,r5		// offset(s2)
L15:
	ld	r3,r0
	inc	r0,#1
	ld	r5,r1
	inc	r1,#1
	ldb	rl7,@rr4
	ldb	@rr2,rl7
	testb	rl7
	jr	z,L16
	dec	r6,#1
	jpr	ge,L15
	clrb	@rr2
L16:
	ldl	rr2,SP(#4)
	ret
	}
