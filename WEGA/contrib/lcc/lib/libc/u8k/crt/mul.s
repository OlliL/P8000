.line 1 ""
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec	data   
	.data
	.psec
	.code
mul_hwt::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,_stkseg+%8(fp)
	sral	rr2,#%10
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,#%ffff
	and	r2,_stkseg+%8(fp)
	and	r3,_stkseg+%a(fp)
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	sral	rr2,#%10
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,#%ffff
	and	r2,_stkseg+%c(fp)
	and	r3,_stkseg+%e(fp)
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%fff0(fp)
	ldl	rr2,rr6
	ldl	_stkseg+%ffd4(fp),rr2
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%ffec(fp)
	ldl	rr2,rr6
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,#%ffff
	and	r2,_stkseg+%ffdc(fp)
	and	r3,_stkseg+%ffde(fp)
	slal	rr2,#%10
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,_stkseg+%ffdc(fp)
	sral	rr2,#%10
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	addl	rr2,_stkseg+%ffd4(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	callr	carry
	ld	_stkseg+%ffd2(fp),r2
	ld	r3,_stkseg+%ffd2(fp)
	exts	rr2
	addl	rr2,_stkseg+%ffdc(fp)
	addl	rr2,_stkseg+%ffe0(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%fff4(fp)
	ldl	rr2,rr6
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,#%ffff
	and	r2,_stkseg+%ffe0(fp)
	and	r3,_stkseg+%ffe2(fp)
	slal	rr2,#%10
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,_stkseg+%ffe0(fp)
	sral	rr2,#%10
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	addl	rr2,_stkseg+%ffd4(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	callr	carry
	ld	_stkseg+%ffd2(fp),r2
	ld	r3,_stkseg+%ffd2(fp)
	exts	rr2
	addl	rr2,_stkseg+%ffdc(fp)
	addl	rr2,_stkseg+%ffe0(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%ffec(fp)
	ldl	rr2,rr6
	addl	rr2,_stkseg+%ffdc(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,FP(#%ffdc)
	jpr	L22
L22:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 46
	~L2 := 50
	}
