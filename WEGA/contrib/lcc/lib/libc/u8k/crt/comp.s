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
cps::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	FP(#%fff4),rr0
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%80000000
	ldl	FP(#%ffec),rr2
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffe4),rr2
	ldl	rr2,FP(#%ffec)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L24
	ldl	rr2,FP(#%ffe4)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe4),rr2
L24:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffec)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L25
	ldl	rr2,FP(#%ffe0)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe0),rr2
L25:
	ldl	rr2,FP(#%ffe4)
	cpl	rr2,_stkseg+%ffe0(fp)
	jpr	eq,L26
	ldl	rr2,FP(#%ffe0)
	subl	rr2,_stkseg+%ffe4(fp)
	ldl	FP(#%ffe0),rr2
	ldctlb	rl0,flags
	jpr	L23
L26:
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffe4),rr2
	ldl	rr2,FP(#%ffec)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L27
	ldl	rr2,FP(#%ffe4)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe4),rr2
L27:
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffec)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	cpl	rr2,#0
	jpr	eq,L28
	ldl	rr2,FP(#%ffe0)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe0),rr2
L28:
	ldl	rr2,FP(#%ffe4)
	subl	rr2,_stkseg+%ffe0(fp)
	ldl	FP(#%ffe4),rr2
	ldctlb	rl0,flags
	jpr	L23
L23:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ldctlb	flags,rl0
	ret
	~L1 := 32
	~L2 := 36
	}

	.psec	data   
	.data
	.psec
	.code
cpdd::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	FP(#%fff4),rr0
	ldl	FP(#%fff0),rr4
	ldl	FP(#%ffec),rr2
	ldl	FP(#%ffe8),rr6
	ldl	rr2,#%80000000
	ldl	FP(#%ffdc),rr2
	subl	rr2,rr2
	ldl	FP(#%ffd8),rr2
	ldl	rr2,FP(#%ffec)
	cpl	rr2,_stkseg+%ffe8(fp)
	jpr	ne,L31
	ldl	rr2,FP(#%fff4)
	cpl	rr2,_stkseg+%fff0(fp)
	jpr	ne,L31
	ldctlb	rl0,flags
	jpr	L30
L31:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffe4),rr2
	ldl	rr2,FP(#%ffdc)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L32
	ldl	rr2,FP(#%ffe4)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe4),rr2
L32:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffdc)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	cpl	rr2,#0
	jpr	eq,L33
	ldl	rr2,FP(#%ffe0)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe0),rr2
L33:
	ldl	rr2,FP(#%ffe4)
	cpl	rr2,_stkseg+%ffe0(fp)
	jpr	eq,L34
	ldctlb	rl0,flags
	jpr	L30
L34:
	ldl	rr2,FP(#%ffdc)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L35
	ldl	rr2,FP(#%ffd8)
	subl	rr2,_stkseg+%ffec(fp)
	ldl	FP(#%ffec),rr2
	callr	carry
	ld	FP(#%ffd2),r2
	ld	r2,FP(#%ffd2)
	neg	r2
	ld	r3,r2
	exts	rr2
	subl	rr2,_stkseg+%fff4(fp)
	ldl	FP(#%fff4),rr2
L35:
	ldl	rr2,FP(#%ffdc)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	cpl	rr2,#0
	jpr	eq,L37
	ldl	rr2,FP(#%ffd8)
	subl	rr2,_stkseg+%ffe8(fp)
	ldl	FP(#%ffe8),rr2
	callr	carry
	ld	FP(#%ffd2),r2
	ld	r2,FP(#%ffd2)
	neg	r2
	ld	r3,r2
	exts	rr2
	subl	rr2,_stkseg+%fff0(fp)
	ldl	FP(#%fff0),rr2
L37:
	ldl	rr2,FP(#%ffec)
	subl	rr2,_stkseg+%ffe8(fp)
	ldl	FP(#%ffd4),rr2
	callr	carry
	ld	FP(#%ffd2),r2
	ld	r2,FP(#%ffd2)
	neg	r2
	ld	r3,r2
	exts	rr2
	addl	rr2,_stkseg+%fff4(fp)
	subl	rr2,_stkseg+%fff0(fp)
	ldl	FP(#%ffe4),rr2
	ldl	rr2,FP(#%ffd4)
	cpl	rr2,#0
	jpr	eq,L39
	ldl	rr2,FP(#%ffe4)
	cpl	rr2,#0
	jpr	eq,L38
L39:
	ldl	rr2,FP(#%ffe4)
	testl	rr2
	ldctlb	rl0,flags
	jpr	L30
L38:
	ld	r2,#%1
	test	r2
	ldctlb	rl0,flags
	jpr	L30
L30:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ldctlb	flags,rl0
	ret
	~L1 := 46
	~L2 := 50
	}

	.psec	data   
	.data
	.psec
	.code
cpzs::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,FP(#%8)
	ldl	FP(#%8),rr2
	ldctlb	rl0,flags
	jpr	L41
L41:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ldctlb	flags,rl0
	ret
	~L1 := 8
	~L2 := 12
	}

	.psec	data   
	.data
	.psec
	.code
cpzd::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	FP(#%fff4),rr0
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%1
	ldl	FP(#%ffec),rr2
	ldl	rr2,FP(#%fff4)
	cpl	rr2,#0
	jpr	ne,L44
	ldl	rr2,FP(#%fff0)
	cpl	rr2,#0
	jpr	ne,L44
	ldctlb	rl0,flags
	jpr	L43
L44:
	ldl	rr2,FP(#%fff4)
	cpl	rr2,#0
	jpr	eq,L45
	ldctlb	rl0,flags
	jpr	L43
L45:
	ldl	rr2,#%1
	ldl	FP(#%fff4),rr2
	ldctlb	rl0,flags
	jpr	L43
L43:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ldctlb	flags,rl0
	ret
	~L1 := 20
	~L2 := 24
	}

	.psec	data   
	.data
