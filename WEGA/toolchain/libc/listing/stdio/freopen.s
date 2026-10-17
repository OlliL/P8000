
fp		:= r15;
sp		:= r15;
/*~~freopen:*/

.code

_freopen::
{
	dec	fp,#~L2
	ld	8(fp),r14
	ldl	~L1+2(fp),rr6
	ld	r14,r5
	ld	r7,r5
	callr	_fclose
	ld	r7,~L1+4(fp)
	ld	r6,~L1+2(fp)
	ld	r5,r14
	callr	__endopen
	ld	r14,8(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _freopen */

.data

