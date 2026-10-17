
fp		:= r15;
sp		:= r15;
/*~~clearerr:*/

.code

_clearerr::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r14,r7
	ldb	rl2,6(r14)
	andb	rl2,#-49
	ldb	6(r14),rl2
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _clearerr */

.data

