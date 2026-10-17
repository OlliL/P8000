
fp		:= r15;
sp		:= r15;
.comm	2,	_swap_flg;
.even
_magics::
.word	%0
.word	%e707
.word	%e711
.word	%e705
.word	%e607
.word	%e611
.word	%e605
.word	%e507
.word	%e511
.word	%e505
/*~~goodmagic:*/

.code

_goodmagic::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	@fp,r7
	clr	_swap_flg
L16:
	ld	r2,~L1(fp)
	ld	_magics,r2
	ldk	r14,#10
L19:
	dec	r14,#1
	ld	r2,r14
	add	r2,r14
	ld	r3,~L1(fp)
	cp	r3,_magics(r2)
	jpr	ne,L19
	test	r14
	jpr	ne,L14
	ld	r7,r3
	callr	_swap
	ld	~L1(fp),r2
	ld	r2,_swap_flg
	inc	_swap_flg,#1
	test	r2
	jpr	eq,L16
L14:
	test	r14
	jpr	ne,L22
	clr	_swap_flg
L22:
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _goodmagic */

.data

