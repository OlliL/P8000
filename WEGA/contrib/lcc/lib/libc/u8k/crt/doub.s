.line 1 ""
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec	data   
	.data
	.comm	2,	_debug;
	.psec
	.code
subd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr4
	ldl	_stkseg+%ffec(fp),rr2
	ldl	_stkseg+%ffe8(fp),rr6
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L27
	ldl	rr2,_stkseg+%ffe8(fp)
	cpl	rr2,#0
	jpr	ne,L27
	jpr	L26
L27:
	ldl	rr6,_stkseg+%ffe8(fp)
	ldl	rr2,_stkseg+%ffe4(fp)
	com	r2
	com	r3
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	rr4,_stkseg+%fff0(fp)
	com	r4
	com	r5
	and	r4,_stkseg+%ffe4(fp)
	and	r5,_stkseg+%ffe6(fp)
	or	r2,r4
	or	r3,r5
	ldl	rr4,rr2
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	rr0,_stkseg+%fff4(fp)
	callr	addd
	inc	sp,#%10
L26:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 28
	~L2 := 32
	}

	.psec	data   
	.data
	.psec
	.code
negd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	com	r2
	com	r3
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,_stkseg+%fff4(fp)
	com	r4
	com	r5
	and	r4,_stkseg+%ffec(fp)
	and	r5,_stkseg+%ffee(fp)
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr0,_stkseg+%ffe4(fp)
	jpr	L29
L29:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 28
	~L2 := 32
	}

	.psec	data   
	.data
	.psec
	.code
addd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr4
	ldl	_stkseg+%ffec(fp),rr2
	ldl	_stkseg+%ffe8(fp),rr6
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffc0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L31
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	ne,L31
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	_stkseg+%ffb4(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	_stkseg+%ffb8(fp),rr2
	ldl	rr2,_stkseg+%ffb8(fp)
	ldl	rr0,_stkseg+%ffb4(fp)
	jpr	L30
L31:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L32
	ldl	rr2,_stkseg+%ffe8(fp)
	cpl	rr2,#0
	jpr	ne,L32
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	_stkseg+%ffb4(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	_stkseg+%ffb8(fp),rr2
	ldl	rr2,_stkseg+%ffb8(fp)
	ldl	rr0,_stkseg+%ffb4(fp)
	jpr	L30
L32:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,#%7ff00000
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	subl	rr2,rr4
	sral	rr2,#%14
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	cpl	rr2,#0
	jpr	ge,L33
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	_stkseg+%ffbc(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,_stkseg+%ffbc(fp)
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	_stkseg+%ffbc(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,_stkseg+%ffbc(fp)
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	com	r2
	com	r3
	addl	rr2,#1
	ldl	_stkseg+%ffd8(fp),rr2
L33:
	ldl	rr2,_stkseg+%ffc0(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	_stkseg+%ffcc(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	_stkseg+%ffc4(fp),rr2
	ldl	rr2,#%9
	ldl	_stkseg+%ffbc(fp),rr2
L34:
	ldl	rr2,_stkseg+%ffbc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffbc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L35
	ldl	rr2,_stkseg+%ffc0(fp)
	and	r2,_stkseg+%ffe8(fp)
	and	r3,_stkseg+%ffea(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffc4(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffc4(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffe8(fp),rr2
	jpr	L34
L35:
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffc4(fp)
	or	r3,_stkseg+%ffc6(fp)
	ldl	_stkseg+%ffc4(fp),rr2
	ldl	rr2,_stkseg+%ffcc(fp)
	cpl	rr2,#0
	jpr	eq,L36
	ldl	rr2,_stkseg+%ffd4(fp)
	subl	rr2,_stkseg+%ffe8(fp)
	ldl	_stkseg+%ffe8(fp),rr2
	callr	carry
	ld	_stkseg+%ffb2(fp),r2
	ld	r2,_stkseg+%ffb2(fp)
	neg	r2
	ld	r5,r2
	exts	rr4
	subl	rr4,_stkseg+%ffc4(fp)
	ldl	_stkseg+%ffc4(fp),rr4
L36:
	ldl	rr2,_stkseg+%ffd8(fp)
	ldl	_stkseg+%ffbc(fp),rr2
L38:
	ldl	rr2,_stkseg+%ffbc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffbc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L39
	ldl	rr2,#%1
	and	r2,_stkseg+%ffc4(fp)
	and	r3,_stkseg+%ffc6(fp)
	slal	rr2,#%1f
	ldl	rr4,_stkseg+%ffe8(fp)
	srll	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%ffc4(fp)
	sral	rr2,#%1
	ldl	_stkseg+%ffc4(fp),rr2
	jpr	L38
L39:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	sral	rr2,#%14
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%ffc0(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffd0(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,#%9
	ldl	_stkseg+%ffbc(fp),rr2
L40:
	ldl	rr2,_stkseg+%ffbc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffbc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L41
	ldl	rr2,_stkseg+%ffc0(fp)
	and	r2,_stkseg+%ffec(fp)
	and	r3,_stkseg+%ffee(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffc8(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffec(fp),rr2
	jpr	L40
L41:
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffc8(fp)
	or	r3,_stkseg+%ffca(fp)
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,_stkseg+%ffd0(fp)
	cpl	rr2,#0
	jpr	eq,L42
	ldl	rr2,_stkseg+%ffd4(fp)
	subl	rr2,_stkseg+%ffec(fp)
	ldl	_stkseg+%ffec(fp),rr2
	callr	carry
	ld	_stkseg+%ffb2(fp),r2
	ld	r2,_stkseg+%ffb2(fp)
	neg	r2
	ld	r5,r2
	exts	rr4
	subl	rr4,_stkseg+%ffc8(fp)
	ldl	_stkseg+%ffc8(fp),rr4
L42:
	ldl	rr2,_stkseg+%ffec(fp)
	addl	rr2,_stkseg+%ffe8(fp)
	ldl	_stkseg+%ffb8(fp),rr2
	callr	carry
	ld	_stkseg+%ffb2(fp),r2
	ld	r3,_stkseg+%ffb2(fp)
	exts	rr2
	addl	rr2,_stkseg+%ffc8(fp)
	addl	rr2,_stkseg+%ffc4(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	cpl	rr2,#0
	jpr	ne,L43
	ldl	rr2,_stkseg+%ffdc(fp)
	cpl	rr2,#0
	jpr	ne,L43
	ldl	rr2,_stkseg+%ffb8(fp)
	cpl	rr2,#0
	jpr	ne,L43
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffb4(fp),rr2
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffb8(fp),rr2
	ldl	rr2,_stkseg+%ffb8(fp)
	ldl	rr0,_stkseg+%ffb4(fp)
	jpr	L30
L43:
	ldl	rr2,_stkseg+%ffc0(fp)
	and	r2,_stkseg+%ffdc(fp)
	and	r3,_stkseg+%ffde(fp)
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,_stkseg+%ffe4(fp)
	cpl	rr2,#0
	jpr	eq,L44
	ldl	rr2,_stkseg+%ffd4(fp)
	subl	rr2,_stkseg+%ffb8(fp)
	ldl	_stkseg+%ffb8(fp),rr2
	callr	carry
	ld	_stkseg+%ffb2(fp),r2
	ld	r2,_stkseg+%ffb2(fp)
	neg	r2
	ld	r5,r2
	exts	rr4
	subl	rr4,_stkseg+%ffdc(fp)
	ldl	_stkseg+%ffdc(fp),rr4
L44:
	pushl	@SP,_stkseg+%ffb8(fp)
	pushl	@SP,_stkseg+%ffdc(fp)
	pushl	@SP,_stkseg+%ffe0(fp)
	pushl	@SP,_stkseg+%ffe4(fp)
	callr	normd
	inc	sp,#%10
L30:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 78
	~L2 := 82
	}

	.psec	data   
	.data
	.psec
	.code
normd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,#%40000000
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,#%20000000
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%10(fp)
	cpl	rr2,#0
	jpr	ne,L46
	ldl	rr2,_stkseg+%14(fp)
	cpl	rr2,#0
	jpr	ne,L46
	ldl	rr2,#%1
	ldl	_stkseg+%c(fp),rr2
	jpr	L47
L46:
	ldl	rr2,_stkseg+%10(fp)
	and	r2,_stkseg+%ffec(fp)
	and	r3,_stkseg+%ffee(fp)
	cpl	rr2,#0
	jpr	eq,L48
	jpr	L49
L48:
L50:
	ldl	rr2,_stkseg+%10(fp)
	and	r2,_stkseg+%ffe8(fp)
	and	r3,_stkseg+%ffea(fp)
	cpl	rr2,#0
	jpr	eq,L51
	jpr	L52
L51:
L53:
	ldl	rr2,_stkseg+%ffe0(fp)
	and	r2,_stkseg+%14(fp)
	and	r3,_stkseg+%16(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%10(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%10(fp),rr2
	ldl	rr2,_stkseg+%14(fp)
	addl	rr2,rr2
	ldl	_stkseg+%14(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	subl	rr2,#%1
	ldl	_stkseg+%c(fp),rr2
	jpr	L50
L49:
	ldl	rr2,#%1
	and	r2,_stkseg+%10(fp)
	and	r3,_stkseg+%12(fp)
	slal	rr2,#%1f
	ldl	rr4,_stkseg+%14(fp)
	srll	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%14(fp),rr2
	ldl	rr2,_stkseg+%10(fp)
	srll	rr2,#%1
	ldl	_stkseg+%10(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	addl	rr2,#%1
	ldl	_stkseg+%c(fp),rr2
L52:
	ldl	rr2,_stkseg+%14(fp)
	addl	rr2,#%100
	ldl	_stkseg+%14(fp),rr2
	callr	carry
	ld	_stkseg+%ffde(fp),r2
	ld	r3,_stkseg+%ffde(fp)
	exts	rr2
	addl	rr2,_stkseg+%10(fp)
	ldl	_stkseg+%10(fp),rr2
	ldl	rr2,_stkseg+%10(fp)
	and	r2,_stkseg+%ffec(fp)
	and	r3,_stkseg+%ffee(fp)
	cpl	rr2,#0
	jpr	eq,L54
	jpr	L49
L54:
	ldl	rr2,_stkseg+%c(fp)
	cpl	rr2,#0
	jpr	ge,L55
	ldl	rr2,#%1
	ldl	_stkseg+%c(fp),rr2
	ld	_errno,#%22
	.psec	data   
	.data
L57:
	.byte	%65,%78,%70,%2d,%75,%6e,%64,%65
	.byte	%72,%66,%6c,%6f,%77,%0
	.psec
	.code
	ldl	rr2,#L57
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L55:
	ldl	rr2,_stkseg+%c(fp)
	cpl	rr2,#%7ff
	jpr	le,L58
	ldl	rr2,#%7ff
	ldl	_stkseg+%c(fp),rr2
	ld	_errno,#%22
	.psec	data   
	.data
L59:
	.byte	%65,%78,%70,%2d,%6f,%76,%65,%72
	.byte	%66,%6c,%6f,%77,%0
	.psec
	.code
	ldl	rr2,#L59
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L58:
L47:
	ldl	rr2,#%9
	ldl	_stkseg+%ffe4(fp),rr2
L60:
	ldl	rr2,_stkseg+%ffe4(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffe4(fp),rr4
	cpl	rr2,#0
	jpr	eq,L61
	ldl	rr2,#%1
	and	r2,_stkseg+%10(fp)
	and	r3,_stkseg+%12(fp)
	slal	rr2,#%1f
	ldl	rr4,_stkseg+%14(fp)
	srll	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%14(fp),rr2
	ldl	rr2,_stkseg+%10(fp)
	srll	rr2,#%1
	ldl	_stkseg+%10(fp),rr2
	jpr	L60
L61:
	ldl	rr2,_stkseg+%c(fp)
	slal	rr2,#%14
	or	r2,_stkseg+%8(fp)
	or	r3,_stkseg+%a(fp)
	ldl	rr4,#%fffff
	and	r4,_stkseg+%10(fp)
	and	r5,_stkseg+%12(fp)
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%10(fp),rr2
	ldl	rr2,_stkseg+%10(fp)
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%14(fp)
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	rr0,_stkseg+%fff0(fp)
	jpr	L45
L45:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 34
	~L2 := 38
	}

	.psec	data   
	.data
	.psec
	.code
multd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr4
	ldl	_stkseg+%ffec(fp),rr2
	ldl	_stkseg+%ffe8(fp),rr6
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffd0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L66
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	eq,L65
L66:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L64
	ldl	rr2,_stkseg+%ffe8(fp)
	cpl	rr2,#0
	jpr	ne,L64
L65:
	.psec	data   
	.data
	.even
L67:
	.long	%0	/* 0.000000e0 */
	.long	%0
	.psec
	.code
	ldl	rr2,L67+%4
	ldl	rr0,L67
	jpr	L63
L64:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,#%7ff00000
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	addl	rr2,rr4
	srll	rr2,#%14
	subl	rr2,#%3fe
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,_stkseg+%ffd0(fp)
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	xor	r2,r4
	xor	r3,r5
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,#%9
	ldl	_stkseg+%ffdc(fp),rr2
L68:
	ldl	rr2,_stkseg+%ffdc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffdc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L69
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%ffec(fp)
	and	r3,_stkseg+%ffee(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffd8(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffec(fp),rr2
	jpr	L68
L69:
	ldl	rr2,#%9
	ldl	_stkseg+%ffdc(fp),rr2
L70:
	ldl	rr2,_stkseg+%ffdc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffdc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L71
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%ffe8(fp)
	and	r3,_stkseg+%ffea(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffd4(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffe8(fp),rr2
	jpr	L70
L71:
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffd8(fp)
	or	r3,_stkseg+%ffda(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffd4(fp)
	or	r3,_stkseg+%ffd6(fp)
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	srll	rr2,#%1
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	srll	rr2,#%1
	ldl	_stkseg+%ffe8(fp),rr2
	pushl	@SP,_stkseg+%ffe8(fp)
	pushl	@SP,_stkseg+%ffd8(fp)
	callr	mul_hwt
	inc	sp,#%8
	ldl	_stkseg-~L1+%fffc(fp),rr2
	pushl	@SP,_stkseg+%ffec(fp)
	pushl	@SP,_stkseg+%ffd4(fp)
	callr	mul_hwt
	inc	sp,#%8
	addl	rr2,_stkseg-~L1+%fffc(fp)
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,_stkseg+%ffc8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	ldl	rr6,rr2
	multl	rq4,_stkseg+%ffd4(fp)
	ldl	rr2,rr6
	ldl	_stkseg+%ffdc(fp),rr2
	pushl	@SP,_stkseg+%ffd4(fp)
	pushl	@SP,_stkseg+%ffd8(fp)
	callr	mul_hwt
	inc	sp,#%8
	ldl	rr10,rr2
	ldl	rr2,_stkseg+%ffc8(fp)
	addl	rr2,_stkseg+%ffdc(fp)
	ldl	_stkseg+%ffc8(fp),rr2
	callr	carry
	ld	_stkseg+%ffc6(fp),r2
	ld	r3,_stkseg+%ffc6(fp)
	exts	rr2
	addl	rr2,rr10
	ldl	rr10,rr2
	ldl	rr2,#%2
	ldl	_stkseg+%ffdc(fp),rr2
L72:
	ldl	rr2,_stkseg+%ffdc(fp)
	ldl	rr4,rr2
subl	rr4,#%1
	ldl	_stkseg+%ffdc(fp),rr4
	cpl	rr2,#0
	jpr	eq,L73
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%ffc8(fp)
	and	r3,_stkseg+%ffca(fp)
	srll	rr2,#%1f
	ldl	rr4,rr10
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	rr10,rr2
	ldl	rr2,_stkseg+%ffc8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffc8(fp),rr2
	jpr	L72
L73:
	pushl	@SP,_stkseg+%ffc8(fp)
	pushl	@SP,rr10
	pushl	@SP,_stkseg+%ffe0(fp)
	pushl	@SP,_stkseg+%ffe4(fp)
	callr	normd
	inc	sp,#%10
L63:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 58
	~L2 := 66
	}

	.psec	data   
	.data
	.psec
	.code
divd::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr4
	ldl	_stkseg+%ffec(fp),rr2
	ldl	_stkseg+%ffe8(fp),rr6
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffd0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L76
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	ne,L76
	.psec	data   
	.data
	.even
L77:
	.long	%0	/* 0.000000e0 */
	.long	%0
	.psec
	.code
	ldl	rr2,L77+%4
	ldl	rr0,L77
	jpr	L75
L76:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L78
	ldl	rr2,_stkseg+%ffe8(fp)
	cpl	rr2,#0
	jpr	ne,L78
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	_stkseg+%ffc8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	_stkseg+%ffcc(fp),rr2
	ld	_errno,#%22
	.psec	data   
	.data
L79:
	.byte	%64,%69,%76,%69,%73,%69,%6f,%6e
	.byte	%20,%62,%79,%20,%7a,%65,%72,%6f
	.byte	%0
	.psec
	.code
	ldl	rr2,#L79
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
	ldl	rr2,_stkseg+%ffcc(fp)
	ldl	rr0,_stkseg+%ffc8(fp)
	jpr	L75
L78:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	sral	rr2,#%14
	ldl	rr4,#%7ff00000
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	sral	rr4,#%14
	subl	rr2,rr4
	addl	rr2,#%3ff
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	rr4,_stkseg+%ffd0(fp)
	and	r4,_stkseg+%fff4(fp)
	and	r5,_stkseg+%fff6(fp)
	xor	r2,r4
	xor	r3,r5
	ldl	_stkseg+%ffe4(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr10,#%9
L80:
	ldl	rr2,rr10
	subl	rr10,#%1
	cpl	rr2,#0
	jpr	eq,L81
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%ffec(fp)
	and	r3,_stkseg+%ffee(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffd8(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffec(fp),rr2
	jpr	L80
L81:
	ldl	rr10,#%9
L82:
	ldl	rr2,rr10
	subl	rr10,#%1
	cpl	rr2,#0
	jpr	eq,L83
	ldl	rr2,_stkseg+%ffd0(fp)
	and	r2,_stkseg+%ffe8(fp)
	and	r3,_stkseg+%ffea(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffd4(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	addl	rr2,rr2
	ldl	_stkseg+%ffe8(fp),rr2
	jpr	L82
L83:
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffd8(fp)
	or	r3,_stkseg+%ffda(fp)
	ldl	_stkseg+%ffd8(fp),rr2
	ldl	rr2,#%20000000
	or	r2,_stkseg+%ffd4(fp)
	or	r3,_stkseg+%ffd6(fp)
	ldl	_stkseg+%ffd4(fp),rr2
	ldl	rr2,#%1
	and	r2,_stkseg+%ffd8(fp)
	and	r3,_stkseg+%ffda(fp)
	slal	rr2,#%1f
	ldl	rr4,_stkseg+%ffec(fp)
	srll	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	srll	rr2,#%1
	ldl	_stkseg+%ffd8(fp),rr2
	pushl	@SP,_stkseg+%ffe8(fp)
	pushl	@SP,_stkseg+%ffd4(fp)
	pushl	@SP,_stkseg+%ffec(fp)
	pushl	@SP,_stkseg+%ffd8(fp)
	callr	divgdd
	inc	sp,#%10
	ldl	rr4,FP
	add	r5,#%ffc8
	ld	r6,#%4
	ldir	@rr4,@rr2,r6
	ldl	rr2,_stkseg+%ffc8(fp)
	ldl	_stkseg+%ffc4(fp),rr2
	ldl	rr8,FP(#%ffcc)
	ldl	rr10,#%2
L84:
	ldl	rr2,rr10
	subl	rr10,#%1
	cpl	rr2,#0
	jpr	eq,L85
	ldl	rr2,#%1
	and	r2,_stkseg+%ffc4(fp)
	and	r3,_stkseg+%ffc6(fp)
	slal	rr2,#%1f
	ldl	rr4,rr8
	srll	rr4,#%1
	or	r2,r4
	or	r3,r5
	ldl	rr8,rr2
	ldl	rr2,_stkseg+%ffc4(fp)
	srll	rr2,#%1
	ldl	_stkseg+%ffc4(fp),rr2
	jpr	L84
L85:
	pushl	@SP,rr8
	pushl	@SP,_stkseg+%ffc4(fp)
	pushl	@SP,_stkseg+%ffe0(fp)
	pushl	@SP,_stkseg+%ffe4(fp)
	callr	normd
	inc	sp,#%10
L75:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 62
	~L2 := 66
	}

	.psec	data   
	.data
	.psec
	.code
dtol::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr0
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L88
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L88
	xor	r3,r3
	ld	r2,r3
	jpr	L87
L88:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	sral	rr2,#%14
	ldl	_stkseg+%ffe0(fp),rr2
	ldl	rr2,_stkseg+%ffe8(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,#%100000
	or	r2,_stkseg+%ffdc(fp)
	or	r3,_stkseg+%ffde(fp)
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%ffe0(fp)
	cpl	rr2,#%413
	jpr	eq,L94
	ldl	rr2,_stkseg+%ffe0(fp)
	cpl	rr2,#%413
	jpr	le,L90
	ldl	rr2,_stkseg+%ffe0(fp)
	subl	rr2,#%413
	ldl	_stkseg+%ffe4(fp),rr2
L91:
	ldl	rr2,_stkseg+%ffe4(fp)
	ldl	rr4,rr2
	subl	rr4,#%1
	ldl	_stkseg+%ffe4(fp),rr4
	cpl	rr2,#0
	jpr	eq,L92
	ldl	rr2,_stkseg+%ffe8(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	srll	rr2,#%1f
	ldl	rr4,_stkseg+%ffdc(fp)
	addl	rr4,rr4
	or	r2,r4
	or	r3,r5
	ldl	_stkseg+%ffdc(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	addl	rr2,rr2
	ldl	_stkseg+%fff0(fp),rr2
	jpr	L91
L92:
L90:
	ldl	rr2,_stkseg+%ffe0(fp)
	cpl	rr2,#%413
	jpr	ge,L93
	ldl	rr2,_stkseg+%ffdc(fp)
	ldl	rr4,#%413
	subl	rr4,_stkseg+%ffe0(fp)
	cp 	r5,#%20
	jr	le,L90a
	subl	rr2,rr2
	jr	L90c
L90a:
	com	r4
	com	r5
	addl	rr4,#1
	sdal	rr2,r5
L90c:
	ldl	_stkseg+%ffdc(fp),rr2
L93:
	ldl	rr2,_stkseg+%ffe0(fp)
	cpl	rr2,#%41d
	jpr	le,L94
	ld	_errno,#%22
	.psec	data   
	.data
L95:
	.byte	%63,%76,%74,%64,%6c,%3a,%20,%69
	.byte	%6e,%74,%65,%67,%65,%72,%20,%6f
	.byte	%76,%65,%72,%66,%6c,%6f,%77,%0
	.psec
	.code
	ldl	rr2,#L95
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L94:
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	eq,L96
	ldl	rr2,_stkseg+%ffdc(fp)
	com	r2
	com	r3
	addl	rr2,#1
	jpr	L87
L96:
	ldl	rr2,FP(#%ffdc)
	jpr	L87
L87:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 36
	~L2 := 40
	}

	.psec	data   
	.data
	.psec
	.code
ltod::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L99
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffec(fp),rr2
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%ffe8(fp),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	rr0,_stkseg+%ffe8(fp)
	jpr	L98
L99:
	ldl	rr2,_stkseg+%fff0(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	eq,L100
	ldl	rr2,_stkseg+%fff4(fp)
	com	r2
	com	r3
	addl	rr2,#1
	ldl	_stkseg+%fff4(fp),rr2
L100:
	xor	r3,r3
	ld	r2,r3
	pushl	@SP,rr2
	pushl	@SP,_stkseg+%fff4(fp)
	ldl	rr2,#%41c
	pushl	@SP,rr2
	pushl	@SP,_stkseg+%fff0(fp)
	callr	normd
	inc	sp,#%10
L98:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 24
	~L2 := 28
	}

	.psec	data   
	.data
	.psec
	.code
ftod::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	_stkseg+%ffec(fp),rr2
	xor	r3,r3
	ld	r2,r3
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	rr0,_stkseg+%ffec(fp)
	jpr	L102
L102:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 20
	~L2 := 24
	}

	.psec	data   
	.data
	.psec
	.code
dtof::
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	_stkseg+%fff4(fp),rr2
	ldl	_stkseg+%fff0(fp),rr4
	ldl	rr2,FP(#%fff4)
	jpr	L104
L104:
	ld	sp,fp
	ldm	r8,_stkseg-8(sp),#6
	inc	sp,#4
	ret
	~L1 := 16
	~L2 := 20
	}

	.psec	data   
	.data
