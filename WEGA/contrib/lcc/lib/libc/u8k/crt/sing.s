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
subs::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,_stkseg+%c(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffec(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L27
	ldl	rr2,FP(#%fff4)
	jpr	L26
L27:
	ldl	rr2,_stkseg+%ffec(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	rr4,_stkseg+%fff0(fp)
	ldl	rr4,rr4
	com	r4
	com	r5
	and	r4,_stkseg+%ffec(fp)
	and	r5,_stkseg+%ffee(fp)
	or	r2,r4
	or	r3,r5
	pushl	@SP,rr2
	pushl	@SP,_stkseg+%fff4(fp)
	callr	adds
	inc	sp,#%8
	ldl	rr2,rr2
	jpr	L26
L26:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 20
	~L2 := 24
	}

	.psec	data   
	.data
	.psec
	.code
negs::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,_stkseg+%fff4(fp)
	ldl	rr4,rr4
	com	r4
	com	r5
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	or	r2,r4
	or	r3,r5
	ldl	rr2,rr2
	jpr	L29
L29:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 16
	~L2 := 20
	}

	.psec	data   
	.data
	.psec
	.code
adds::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,_stkseg+%c(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffc4(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L31
	ldl	rr2,FP(#%fff0)
	jpr	L30
L31:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L32
	ldl	rr2,FP(#%fff4)
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
	ldl	FP(#%ffdc),rr2
	ldl	rr2,_stkseg+%ffdc(fp)
	cpl	rr2,#0
	jpr	ge,L33
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	FP(#%ffd8),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,_stkseg+%ffdc(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffdc),rr2
L33:
	ldl	rr2,_stkseg+%ffc4(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	FP(#%ffd0),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffc8),rr2
	ldl	rr2,_stkseg+%ffd0(fp)
	cpl	rr2,#0
	jpr	eq,L34
	ldl	rr2,_stkseg+%ffc8(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffc8),rr2
L34:
	ldl	rr2,_stkseg+%ffdc(fp)
	cpl	rr2,#0
	jpr	eq,L35
	ldl	rr2,_stkseg+%ffc8(fp)
	ldl	rr4,_stkseg+%ffdc(fp)
	ldl	rr4,rr4
	com	r4
	com	r5
	addl	rr4,#1
	sdal	rr2,r5
	ldl	FP(#%ffc8),rr2
L35:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	sral	rr2,#%14
	ldl	FP(#%ffe8),rr2
	ldl	rr2,_stkseg+%ffc4(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffd4),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffcc),rr2
	ldl	rr2,_stkseg+%ffd4(fp)
	cpl	rr2,#0
	jpr	eq,L36
	ldl	rr2,_stkseg+%ffcc(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffcc),rr2
L36:
	ldl	rr2,_stkseg+%ffcc(fp)
	addl	rr2,_stkseg+%ffc8(fp)
	ldl	FP(#%ffe4),rr2
	ldl	rr2,_stkseg+%ffdc(fp)
	cpl	rr2,#0
	jpr	ne,L37
	ldl	rr2,_stkseg+%ffe4(fp)
	cpl	rr2,#0
	jpr	ne,L37
	subl	rr2,rr2
	jpr	L30
L37:
	ldl	rr2,_stkseg+%ffc4(fp)
	and	r2,_stkseg+%ffe4(fp)
	and	r3,_stkseg+%ffe6(fp)
	ldl	FP(#%ffec),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	eq,L38
	ldl	rr2,_stkseg+%ffe4(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe4),rr2
L38:
	pushl	@SP,_stkseg+%ffe4(fp)
	pushl	@SP,_stkseg+%ffe8(fp)
	callr	normf
	inc	sp,#%8
	or	r2,_stkseg+%ffec(fp)
	or	r3,_stkseg+%ffee(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffe0)
	jpr	L30
L30:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 60
	~L2 := 64
	}

	.psec	data   
	.data
	.psec
	.code
normf::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,#%40000000
	ldl	_stkseg+%fff4(fp),rr2
	ldl	rr2,#%20000000
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%c(fp)
	cpl	rr2,#0
	jpr	ne,L40
	ldl	rr2,#%1
	ldl	_stkseg+%8(fp),rr2
	jpr	L41
L40:
	ldl	rr2,_stkseg+%c(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L42
	jpr	L43
L42:
L44:
	ldl	rr2,_stkseg+%c(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	cpl	rr2,#0
	jpr	eq,L45
	jpr	L46
L45:
L47:
	ldl	rr2,_stkseg+%c(fp)
	addl	rr2,rr2
	ldl	FP(#%c),rr2
	ldl	rr2,_stkseg+%8(fp)
	subl	rr2,#%1
	ldl	FP(#%8),rr2
	jpr	L44
L43:
	ldl	rr2,_stkseg+%c(fp)
	sral	rr2,#%1
	ldl	FP(#%c),rr2
	ldl	rr2,_stkseg+%8(fp)
	addl	rr2,#%1
	ldl	FP(#%8),rr2
L46:
	ldl	rr2,_stkseg+%c(fp)
	addl	rr2,#%100
	ldl	FP(#%c),rr2
	ldl	rr2,_stkseg+%c(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	cpl	rr2,#0
	jpr	eq,L48
	jpr	L43
L48:
	ldl	rr2,_stkseg+%8(fp)
	cpl	rr2,#0
	jpr	ge,L49
	ldl	rr2,#%1
	ldl	_stkseg+%8(fp),rr2
	ld	_errno,#%22
	.psec	strdata   
	.data
L51:
	.byte	%65,%78,%70,%6f,%6e,%65,%6e,%74
	.byte	%20,%75,%6e,%64,%65,%72,%66,%6c
	.byte	%6f,%77,%a,%0
	.psec
	.code
	ldl	rr2,#L51
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L49:
	ldl	rr2,_stkseg+%8(fp)
	cpl	rr2,#%7ff
	jpr	le,L52
	ldl	rr2,#%7ff
	ldl	_stkseg+%8(fp),rr2
	ld	_errno,#%22
	.psec	strdata   
	.data
L53:
	.byte	%65,%78,%70,%6f,%6e,%65,%6e,%74
	.byte	%20,%6f,%76,%65,%72,%66,%6c,%6f
	.byte	%77,%a,%0
	.psec
	.code
	ldl	rr2,#L53
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L52:
L41:
	ldl	rr2,_stkseg+%8(fp)
	slal	rr2,#%14
	ldl	rr4,_stkseg+%c(fp)
	sral	rr4,#%9
	ldl	rr6,#%fffff
	and	r4,r6
	and	r5,r7
	or	r2,r4
	or	r3,r5
	ldl	rr2,rr2
	jpr	L39
L39:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 16
	~L2 := 20
	}

	.psec	data   
	.data
	.psec
	.code
mults::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,_stkseg+%c(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffcc(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	eq,L57
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L56
L57:
	subl	rr2,rr2
	jpr	L55
L56:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,#%7ff00000
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	addl	rr2,rr4
	srll	rr2,#%14
	subl	rr2,#%3fe
	ldl	FP(#%ffe8),rr2
	ldl	rr2,_stkseg+%ffcc(fp)
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	ldl	FP(#%ffd8),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffd0),rr2
	ldl	rr2,_stkseg+%ffd8(fp)
	cpl	rr2,#0
	jpr	eq,L58
	ldl	rr2,_stkseg+%ffd0(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffd0),rr2
L58:
	ldl	rr2,_stkseg+%ffcc(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffdc),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffd4),rr2
	ldl	rr2,_stkseg+%ffdc(fp)
	cpl	rr2,#0
	jpr	eq,L59
	ldl	rr2,_stkseg+%ffd4(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffd4),rr2
L59:
	pushl	@SP,_stkseg+%ffd0(fp)
	pushl	@SP,_stkseg+%ffd4(fp)
	callr	mul_hwt
	inc	sp,#%8
	slal	rr2,#%2
	ldl	FP(#%ffe4),rr2
	ldl	rr2,_stkseg+%ffcc(fp)
	and	r2,_stkseg+%ffe4(fp)
	and	r3,_stkseg+%ffe6(fp)
	ldl	FP(#%ffec),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#0
	jpr	eq,L60
	ldl	rr2,_stkseg+%ffe4(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%ffe4),rr2
L60:
	pushl	@SP,_stkseg+%ffe4(fp)
	pushl	@SP,_stkseg+%ffe8(fp)
	callr	normf
	inc	sp,#%8
	or	r2,_stkseg+%ffec(fp)
	or	r3,_stkseg+%ffee(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffe0)
	jpr	L55
L55:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 52
	~L2 := 56
	}

	.psec	data   
	.data
	.psec
	.code
divs::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,_stkseg+%c(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%ffcc(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L63
	subl	rr2,rr2
	jpr	L62
L63:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	ne,L64
	ld	_errno,#%22
	.psec	strdata   
	.data
L65:
	.byte	%64,%69,%76,%69,%73,%69,%6f,%6e
	.byte	%20,%62,%79,%20,%7a,%65,%72,%6f
	.byte	%0
	.psec
	.code
	ldl	rr2,#L65
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
	ldl	rr2,FP(#%fff4)
	jpr	L62
L64:
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
	ldl	FP(#%ffe8),rr2
	ldl	rr2,_stkseg+%ffcc(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	rr4,_stkseg+%ffcc(fp)
	and	r4,_stkseg+%fff0(fp)
	and	r5,_stkseg+%fff2(fp)
	xor	r2,r4
	xor	r3,r5
	ldl	FP(#%ffec),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffd4),rr2
	ldl	rr2,_stkseg+%ffd4(fp)
	sral	rr2,#%1
	ldl	FP(#%ffd4),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff0(fp)
	and	r3,_stkseg+%fff2(fp)
	slal	rr2,#%9
	ldl	rr4,#%20000000
	or	r2,r4
	or	r3,r5
	ldl	FP(#%ffd0),rr2
	pushl	@SP,_stkseg+%ffd0(fp)
	pushl	@SP,_stkseg+%ffd4(fp)
	callr	divg
	inc	sp,#%8
	srll	rr2,#%2
	ldl	FP(#%ffe4),rr2
	pushl	@SP,_stkseg+%ffe4(fp)
	pushl	@SP,_stkseg+%ffe8(fp)
	callr	normf
	inc	sp,#%8
	or	r2,_stkseg+%ffec(fp)
	or	r3,_stkseg+%ffee(fp)
	ldl	FP(#%ffe0),rr2
	ldl	rr2,FP(#%ffe0)
	jpr	L62
L62:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 52
	~L2 := 56
	}

	.psec	data   
	.data
	.psec
	.code
ftol::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L68
	subl	rr2,rr2
	jpr	L67
L68:
	ldl	rr2,#%7ff00000
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	sral	rr2,#%14
	ldl	FP(#%ffec),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,#%fffff
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%ffe8),rr2
	ldl	rr2,#%100000
	or	r2,_stkseg+%ffe8(fp)
	or	r3,_stkseg+%ffea(fp)
	ldl	FP(#%ffe8),rr2
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#%413
	jpr	ne,L69
	ldl	rr2,FP(#%ffe8)
	jpr	L67
L69:
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#%413
	jpr	le,L70
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr4,_stkseg+%ffec(fp)
	subl	rr4,#%413
	sdal	rr2,r5
	ldl	FP(#%ffe8),rr2
L70:
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#%413
	jpr	ge,L71
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr4,#%413
	subl	rr4,_stkseg+%ffec(fp)
	ldl	rr4,rr4
	com	r4
	com	r5
	addl	rr4,#1
	sdal	rr2,r5
	ldl	FP(#%ffe8),rr2
L71:
	ldl	rr2,_stkseg+%ffec(fp)
	cpl	rr2,#%41d
	jpr	le,L72
	ld	_errno,#%22
	.psec	strdata   
	.data
L73:
	.byte	%63,%76,%74,%66,%6c,%3a,%20,%69
	.byte	%6e,%74,%65,%67,%65,%72,%20,%6f
	.byte	%76,%65,%72,%66,%6c,%6f,%77,%0
	.psec
	.code
	ldl	rr2,#L73
	pushl	@SP,rr2
	callr	_perror
	inc	sp,#%4
L72:
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	eq,L74
	ldl	rr2,_stkseg+%ffe8(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	rr2,rr2
	jpr	L67
L74:
	ldl	rr2,FP(#%ffe8)
	jpr	L67
L67:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 24
	~L2 := 28
	}

	.psec	data   
	.data
	.psec
	.code
ltof::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	ld	fp,sp
	inc	fp,#8
	sub	sp,#~L2-12
	ldl	rr2,_stkseg+%8(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,#%80000000
	ldl	_stkseg+%fff0(fp),rr2
	ldl	rr2,_stkseg+%fff4(fp)
	cpl	rr2,#0
	jpr	ne,L77
	subl	rr2,rr2
	jpr	L76
L77:
	ldl	rr2,_stkseg+%fff0(fp)
	and	r2,_stkseg+%fff4(fp)
	and	r3,_stkseg+%fff6(fp)
	ldl	FP(#%fff0),rr2
	ldl	rr2,_stkseg+%fff0(fp)
	cpl	rr2,#0
	jpr	eq,L78
	ldl	rr2,_stkseg+%fff4(fp)
	ldl	rr2,rr2
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#%fff4),rr2
L78:
	pushl	@SP,_stkseg+%fff4(fp)
	ldl	rr2,#%41c
	pushl	@SP,rr2
	callr	normf
	inc	sp,#%8
	or	r2,_stkseg+%fff0(fp)
	or	r3,_stkseg+%fff2(fp)
	ldl	FP(#%fff4),rr2
	ldl	rr2,FP(#%fff4)
	jpr	L76
L76:
	ld	sp,fp
	dec	sp,#8
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 16
	~L2 := 20
	}

	.psec	data   
	.data
