	.seg

	sp := r15
	SP := rr14
	fp := r13
	FP := rr12

	.psec
	.code
_strchr::
	{
	ldm	r2,|_stkseg+4|(sp),#3	// rr2 - s , rl4 - c
	jr	~start
~loop:
	inc	r3,#1			// nxt byte
~start:
	cpb	rl4,@rr2
	ret	eq
	testb	@rr2
	jr	nz,~loop		// no end of string
	subl	rr2,rr2
	ret
	}
