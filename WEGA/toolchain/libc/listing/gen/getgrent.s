
fp		:= r15;
sp		:= r15;
_GROUP:
.byte	%2f
.byte	%65
.byte	%74
.byte	%63
.byte	%2f
.byte	%67
.byte	%72
.byte	%6f
.byte	%75
.byte	%70
.byte	%0
.byte	0
.even
_grf:
.word	%0
_line:
.=.+512
.byte	0
.byte	0
.even
_group:
.=.+8
.even
_gr_mem:
.=.+200
L26:
.byte	%72,%0
.even
/*~~setgrent:*/

.code

_setgrent::
{
	test	_grf
	jpr	ne,L25
	ld	r7,#_GROUP
	ld	r6,#L26
	callr	_fopen
	ld	_grf,r2
	jpr	L10000
L25:
	ld	r7,_grf
	callr	_rewind
L10000:
	ret
	~L1 := 0
	~L2 := 0
	}	/* _setgrent */

.data

/*~~endgrent:*/

.code

_endgrent::
{
	test	_grf
	jpr	eq,L10003
	ld	r7,_grf
	callr	_fclose
	clr	_grf
L10003:
	ret
	~L1 := 0
	~L2 := 0
	}	/* _endgrent */

.data


.code

_grskip:
{
	dec	fp,#~L2
	ld	r3,r6
	ld	r1,r7
	jpr	L35
L20001:
	ldb	rl2,@r1
	extsb	r2
	cp	r3,r2
	jpr	eq,L36
	inc	r1,#1
L35:
	testb	@r1
	jpr	ne,L20001
L36:
	testb	@r1
	jpr	eq,L37
	clrb	@r1
	inc	r1,#1
L37:
	ld	r2,r1
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _grskip */

.data

L40:
.byte	%72,%0
.even
/*~~getgrent:*/

.code

_getgrent::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	test	_grf
	jpr	ne,L41
	ld	r7,#_GROUP
	ld	r6,#L40
	callr	_fopen
	ld	_grf,r2
	test	r2
	jpr	ne,L41
L20004:
	sub	r2,r2
	jpr	L10009
L41:
	ld	r7,#_line
	ld	r6,#512
	ld	r5,_grf
	callr	_fgets
	ld	r14,r2
	test	r2
	jpr	eq,L20004
	ld	_group,r2
	ld	r7,r2
	ld	r6,#58
	callr	_grskip
	ld	r14,r2
	ld	_group+2,r2
	ld	r7,r14
	ld	r6,#58
	callr	_grskip
	ld	r14,r2
	ld	~L1+2(fp),r2
	ld	r7,r2
	callr	_atoi
	ld	_group+4,r2
	ld	_group+6,#_gr_mem
	ld	r7,r14
	ld	r6,#58
	callr	_grskip
	ld	r14,r2
	ld	r7,r2
	ldk	r6,#10
	callr	_grskip
	ld	r13,#_gr_mem
	jpr	L44
L20003:
	ld	@r13,r14
	inc	r13,#2
	ld	r7,r14
	ld	r6,#44
	callr	_grskip
	ld	r14,r2
L44:
	testb	@r14
	jpr	ne,L20003
	clr	@r13
	ld	r2,#_group
L10009:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _getgrent */

.data

