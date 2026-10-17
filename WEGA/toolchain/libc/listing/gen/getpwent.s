
fp		:= r15;
sp		:= r15;
_PASSWD:
.byte	%2f
.byte	%65
.byte	%74
.byte	%63
.byte	%2f
.byte	%70
.byte	%61
.byte	%73
.byte	%73
.byte	%77
.byte	%64
.byte	%0
_EMPTY:
.byte	%0
.byte	0
.even
_pwf:
.word	%0
_line:
.=.+512
.byte	0
.byte	0
.even
_passwd:
.=.+18
L26:
.byte	%72,%0
.even
/*~~setpwent:*/

.code

_setpwent::
{
	test	_pwf
	jpr	ne,L25
	ld	r7,#_PASSWD
	ld	r6,#L26
	callr	_fopen
	ld	_pwf,r2
	jpr	L10000
L25:
	ld	r7,_pwf
	callr	_rewind
L10000:
	ret
	~L1 := 0
	~L2 := 0
	}	/* _setpwent */

.data

/*~~endpwent:*/

.code

_endpwent::
{
	test	_pwf
	jpr	eq,L10003
	ld	r7,_pwf
	callr	_fclose
	clr	_pwf
L10003:
	ret
	~L1 := 0
	~L2 := 0
	}	/* _endpwent */

.data


.code

_pwskip:
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	r14,r7
	jpr	L35
L20001:
	cpb	@r14,#58
	jpr	eq,L36
	cpb	@r14,#10
	jpr	eq,L36
	inc	r14,#1
L35:
	testb	@r14
	jpr	ne,L20001
L36:
	cpb	@r14,#10
	jpr	ne,L37
	clrb	@r14
	jpr	L38
L37:
	testb	@r14
	jpr	eq,L38
	clrb	@r14
	inc	r14,#1
L38:
	ld	r2,r14
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _pwskip */

.data

L43:
.byte	%72,%0
.even
/*~~getpwent:*/

.code

_getpwent::
{
	dec	fp,#~L2
	ld	2(fp),r14
	test	_pwf
	jpr	ne,L42
	ld	r7,#_PASSWD
	ld	r6,#L43
	callr	_fopen
	ld	_pwf,r2
	test	r2
	jpr	ne,L42
L20004:
	sub	r2,r2
	jpr	L10009
L42:
	ld	r7,#_line
	ld	r6,#512
	ld	r5,_pwf
	callr	_fgets
	ld	r14,r2
	test	r2
	jpr	eq,L20004
	ld	_passwd,r2
	ld	r7,r2
	callr	_pwskip
	ld	_passwd+2,r2
	ld	r7,r2
	callr	_pwskip
	ld	r14,r2
	ld	r7,r2
	callr	_atoi
	ld	_passwd+4,r2
	ld	r7,r14
	callr	_pwskip
	ld	r14,r2
	ld	r7,r2
	callr	_atoi
	ld	_passwd+6,r2
	ld	_passwd+10,#_EMPTY
	ld	r7,r14
	callr	_pwskip
	ld	_passwd+12,r2
	ld	r7,r2
	callr	_pwskip
	ld	_passwd+14,r2
	ld	r7,r2
	callr	_pwskip
	ld	_passwd+16,r2
	ld	r7,r2
	callr	_pwskip
	ld	r14,_passwd+2
	jpr	L47
L20003:
	cpb	@r14,#44
	jpr	eq,L48
	inc	r14,#1
L47:
	testb	@r14
	jpr	ne,L20003
L48:
	testb	@r14
	jpr	eq,L49
	clrb	@r14
	inc	r14,#1
L49:
	ld	_passwd+8,r14
	ld	r2,#_passwd
L10009:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _getpwent */

.data

