	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec
	.code
_strrchr::
	{
	ldm	r4,|_stkseg+4|(sp),#3	// rr4 - s , rl6 - c
	subl	rr2,rr2
	jr	~start
~loop:
	inc	r5,#1			// nxt byte
~start:
	cpb	rl6,@rr4
	jr	ne,~endtest
	ldl	rr2,rr4			// preliminary return value
~endtest:
	testb	@rr4
	jr	nz,~loop		// no end of string
	ret
	}
