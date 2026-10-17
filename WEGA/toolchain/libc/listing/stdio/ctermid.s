
fp		:= r15;
sp		:= r15;
L20:
.=.+8
.byte	0
.byte	0
L22:
.byte	%2f,%64,%65,%76,%2f,%74,%74,%79
.byte	%0
.even
/*~~ctermid:*/

.code

_ctermid::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	@fp,r7
	test	r7
	jpr	ne,L10003
	ld	r6,#L20
	jpr	L10004
L10003:
	ld	r6,~L1(fp)
L10004:
	ld	r14,r6
	ld	r7,r6
	ld	r6,#L22
	callr	_strcpy
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _ctermid */

.data

