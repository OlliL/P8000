
fp		:= r15;
sp		:= r15;
/*~~rewind:*/

.code

_rewind::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r14,r7
	callr	_fflush
	ldb	rl7,7(r14)
	extsb	r7
	subl	rr4,rr4
	sub	r3,r3
	callr	_lseek
	clr	2(r14)
	ld	r2,4(r14)
	ld	@r14,r2
	ldb	rl2,6(r14)
	andb	rl2,#-49
	ldb	6(r14),rl2
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	eq,L10000
	ldb	rl2,6(r14)
	andb	rl2,#-4
	ldb	6(r14),rl2
L10000:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _rewind */

.data

