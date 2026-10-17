
fp		:= r15;
sp		:= r15;
/*~~swap:*/

.code

_swap::
{
	dec	fp,#~L2
	ld	6(fp),r14
	ld	@fp,r7
	lda	r14,~L1+2(fp)
	ldb	rl2,~L1+1(fp)
	ldb	@r14,rl2
	ldb	rl2,~L1(fp)
	ldb	1(r14),rl2
	ld	r2,@r14
	ld	r14,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _swap */

.data

/*~~longswap:*/

.code

_longswap::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	@fp,r7
	ld	r14,r7
	test	@r14
	jpr	eq,L14
	ld	r7,@r14
	callr	_swap
	ld	@r14,r2
L14:
	inc	r14,#2
	test	@r14
	jpr	eq,L10003
	ld	r7,@r14
	callr	_swap
	ld	@r14,r2
L10003:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _longswap */

.data

/*~~swapsegt:*/

.code

_swapsegt::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	@fp,r7
	ld	r14,r7
	test	4(r14)
	jpr	eq,L18
	ld	r7,4(r14)
	callr	_swap
	ld	4(r14),r2
L18:
	test	6(r14)
	jpr	eq,L19
	ld	r7,6(r14)
	callr	_swap
	ld	6(r14),r2
L19:
	test	8(r14)
	jpr	eq,L20
	ld	r7,8(r14)
	callr	_swap
	ld	8(r14),r2
L20:
	test	10(r14)
	jpr	eq,L10006
	ld	r7,10(r14)
	callr	_swap
	ld	10(r14),r2
L10006:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _swapsegt */

.data

