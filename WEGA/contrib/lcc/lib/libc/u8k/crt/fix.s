
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec	data   
	.data
	.psec
	.code
divg::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ld	_stkseg+%fff6(fp),#%20
L47:
	ld	r2,_stkseg+%fff6(fp)
	dec	_stkseg+%fff6(fp),#%1
	cp	r2,#0
	jpr	eq,L48
	ldl	rr2,_stkseg+%8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%8(fp),rr2
	ldl	rr2,_stkseg+%8(fp)
	subl	rr2,_stkseg+%c(fp)
	ldl	_stkseg+%8(fp),rr2
	callr	carry
	cp	r2,#0
	jpr	eq,L49
	ldl	rr2,_stkseg+%8(fp)
	addl	rr2,_stkseg+%c(fp)
	ldl	_stkseg+%8(fp),rr2
	ldl	rr2,_stkseg+%fff2(fp)
	addl	rr2,rr2
	ldl	_stkseg+%fff2(fp),rr2
	jpr	L50
L49:
	ldl	rr2,_stkseg+%fff2(fp)
	addl	rr2,rr2
	ldl	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%fff2(fp),rr2
L50:
	jpr	L47
L48:
	ldl	rr2,FP(#%fff2)
	jpr	L46
L46:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 14
	~L2 := 18
	}

	.psec	data   
	.data
	.psec
	.code
divgdd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	.psec	data   
	.data
	.comm	8,	___StRet;
	.psec
	.code
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffec(fp),rr2
	ld	_stkseg+%ffea(fp),#%40
L61:
	ld	r2,_stkseg+%ffea(fp)
	dec	_stkseg+%ffea(fp),#%1
	cp	r2,#0
	jpr	eq,L62
	ldl	rr2,_stkseg+%ffec(fp)
	and	r2,_stkseg+%c(fp)
	and	r3,_stkseg+%e(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%8(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%8(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	addl	rr2,rr2
	ldl	_stkseg+%c(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	subl	rr2,_stkseg+%14(fp)
	ldl	_stkseg+%c(fp),rr2
	callr	carry
	ld	r5,r2
	exts	rr4
	ldl	_stkseg+%ffe6(fp),rr4
	ldl	rr2,_stkseg+%ffe6(fp)
	addl	rr2,_stkseg+%10(fp)
	ldl	_stkseg+%ffe6(fp),rr2
	ldl	rr2,_stkseg+%8(fp)
	subl	rr2,_stkseg+%ffe6(fp)
	ldl	_stkseg+%8(fp),rr2
	callr	carry
	cp	r2,#0
	jpr	eq,L63
	ldl	rr2,_stkseg+%c(fp)
	addl	rr2,_stkseg+%14(fp)
	ldl	_stkseg+%c(fp),rr2
	callr	carry
	ld	_stkseg+%ffe8(fp),r2
	ld	r3,_stkseg+%ffe8(fp)
	exts	rr2
	addl	rr2,_stkseg+%8(fp)
	addl	rr2,_stkseg+%10(fp)
	ldl	_stkseg+%8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%fff0(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	addl	rr2,rr2
	ldl	_stkseg+%fff4(fp),rr2
	jpr	L64
L63:
	ldl	rr2,_stkseg+%ffec(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%fff0(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	addl	rr2,rr2
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,#%1
	or	r2,_stkseg+%fff4(fp)
	or	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%fff4(fp),rr2
L64:
	jpr	L61
L62:
	ldl	rr2,#___StRet
	ldl	rr4,FP
	add	r5,#%fff0
	ld	r6,#%4
	ldir	@rr2,@rr4,r6
	jpr	L60
L60:
	ldl	rr2,#___StRet
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 26
	~L2 := 30
	}


	.psec	data   
	.data
	.psec
	.code
mul::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%c(fp)
	ldl	rr2,rr6
	jpr	L69
L69:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 8
	~L2 := 12
	}

	.psec	data   
	.data
	.seg
	.psec
	.code
carry::
	{
	ldk	r2,#0
	ret	nc
	inc	r2,#1
	ret
	}
notcarry::
	{
	ldk	r2,#0
	ret	c
	inc	r2,#1
	ret
	}
