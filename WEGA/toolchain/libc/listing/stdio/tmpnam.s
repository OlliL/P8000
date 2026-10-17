
fp		:= r15;
sp		:= r15;
L20:
.=.+18
.byte	0
.byte	0
L21:
.byte	%61
.byte	%61
.byte	%61
.byte	%0
L23:
.byte	%2f,%75,%73,%72,%2f,%74,%6d,%70
.byte	%2f,%0
.even
L25:
.byte	%58,%58,%58,%58,%58,%58,%0
.even
/*~~tmpnam:*/

.code

_tmpnam::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
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
	ld	r6,#L23
	callr	_strcpy
	ld	r7,r14
	ld	r6,#L21
	callr	_strcat
	ld	r7,r14
	ld	r6,#L25
	callr	_strcat
	ld	r13,#L21
	jpr	L26
L20001:
	ldb	@r13,#97
	inc	r13,#1
L26:
	cpb	@r13,#122
	jpr	eq,L20001
	incb	@r13,#1
	ld	r7,r14
	callr	_mktemp
	ld	r2,r14
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _tmpnam */

.data

