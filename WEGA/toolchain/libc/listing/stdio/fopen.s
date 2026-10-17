
fp		:= r15;
sp		:= r15;
/*~~fopen:*/

.code

_fopen::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	callr	__findiop
	ld	~L1+6(fp),r2
	ld	r7,~L1+2(fp)
	ld	r6,~L1(fp)
	ld	r5,r2
	callr	__endopen
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fopen */

.data

