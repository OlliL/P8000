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
itod::
	{
	ld	r3,r2
	exts	rr2
	callr	ltod
	ret
	}

	.psec	data   
	.data
	.psec
	.code
itof::
	{
	ld	r3,r2
	exts	rr2
	calr	ltof
	ret
	}

	.psec	data   
	.data
	.psec
	.code
/* _dtoi:: */

dtoi::
	{
	callr	dtol
	ld	r2,r3
	ret
	}
	.psec	data   
	.data
	.psec
	.code
ftoi::
	{
	callr	ftol
	ld	r2,r3
	}

	.psec	data   
	.data
	.psec
	.code
uitod::
	{
	ld	r3,r2
	xor	r2,r2
	callr	ultod
	}

	.psec	data   
	.data
	.psec
	.code
uitof::
	{
	ld 	r3,r2
	xor	r2,r2
	callr	ultod
	}

	.psec	data   
	.data
	.psec
	.code
dtoui::
	{
	callr	dtoul
	ld	r2,r3
	ret
	}

	.psec	data   
	.data
	.psec
	.code
ftoui::
	{
	callr	ftoul
	ld	r2,r3
	ret
	}

	.psec	data   
	.data
	.psec
	.code
/* _cvtuld:: */
ultod::
	{
	ldl	rr0,rr2
	cpl	rr2,#0
	jpr	ne,L40
	subl	rr2,rr2
	subl	rr0,rr0
	jpr	L39
L40:
	subl	rr2,rr2
	pushl	@SP,rr2
	pushl	@SP,rr0 /* Argument */
	ldl	rr2,#%41d
	pushl	@SP,rr2
	subl	rr2,rr2
	pushl	@SP,rr2
	callr	normd
	inc	sp,#%10
L39:
	ret
	}

	.psec	data   
	.data
	.psec
	.code
/* _cvtdul:: */
dtoul::
	{
	callr	dtol	
	}

	.psec	data   
	.data
	.psec
	.code
/* _cvtful:: */
ftoul::
	{
	callr	ftol
	}

	.psec	data   
	.data
	.psec
	.code
/* _cvtulf:: */
ultof::
	{
	cpl	rr2,#0
	jpr	ne,L49
	subl	rr2,rr2
	ret
L49:
	pushl	@SP,rr2
	ldl	rr2,#%41c
	pushl	@SP,rr2
	callr	normf
	inc	sp,#%8
	ret
	}

	.psec	data   
	.data
