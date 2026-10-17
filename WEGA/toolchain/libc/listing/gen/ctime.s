
fp		:= r15;
sp		:= r15;
.comm	26,	_cbuf;
.even
_dmsize::
.word	%1f
.word	%1c
.word	%1f
.word	%1e
.word	%1f
.word	%1e
.word	%1f
.word	%1f
.word	%1e
.word	%1f
.word	%1e
.word	%1f
.even
_timezone::
.long	%4650
.even
_tzname::
.addr	L14
.addr	L15
L14:
.byte	%45,%53,%54,%0
L15:
.byte	%45,%44,%54,%0
.even
.even
_daylight::
.word	%1
.even
_daytab::
.word	%5
.word	%14d
.word	%3a
.word	%12f
/*~~ctime:*/

.code

_ctime::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r6,r7
	callr	_localtime
	ld	~L1+4(fp),r2
	ld	r7,r2
	callr	_asctime
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _ctime */

.data

/*~~localtime:*/

.code

_localtime::
{
	dec	fp,#~L2
	ldm	8(fp),r11,#4
	ld	@fp,r7
	callr	_tzset
	ld	r2,~L1(fp)
	ldl	rr4,@r2
	subl	rr4,_timezone
	ldl	~L1+2(fp),rr4
	lda	r7,~L1+2(fp)
	callr	_gmtime
	ld	r13,r2
	ld	r14,14(r13)
	ld	r12,#119
	ld	r11,#303
	cp	10(r13),#74
	jpr	eq,L10006
	cp	10(r13),#75
	jpr	ne,L27
L10006:
	ld	r2,10(r13)
	sub	r2,#74
	sla	r2,#2
	ld	r12,_daytab(r2)
	ld	r2,10(r13)
	sub	r2,#74
	sla	r2,#2
	ld	r11,_daytab+2(r2)
L27:
	ld	r7,r13
	ld	r6,r12
	callr	_sunday
	ld	r12,r2
	ld	r7,r13
	ld	r6,r11
	callr	_sunday
	ld	r11,r2
	test	_daylight
	jpr	eq,L29
	cp	r14,r12
	jpr	gt,L10007
	cp	r14,r12
	jpr	ne,L29
	cp	4(r13),#2
	jpr	lt,L29
L10007:
	cp	r14,r11
	jpr	lt,L10008
	cp	r14,r11
	jpr	ne,L29
	cp	4(r13),#1
	jpr	ge,L29
L10008:
	ldl	rr2,~L1+2(fp)
	addl	rr2,#3600
	ldl	~L1+2(fp),rr2
	lda	r7,~L1+2(fp)
	callr	_gmtime
	ld	r13,r2
	inc	16(r13),#1
L29:
	ld	r2,r13
	ldm	r11,8(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _localtime */

.data

/*~~sunday:*/

.code

_sunday::
{
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ld	r13,r6
	ld	r14,r7
	cp	r6,#58
	jpr	lt,L31
	ld	r7,10(r14)
	callr	_dysize
	sub	r2,#365
	add	r13,r2
L31:
	ld	r2,r13
	sub	r2,14(r14)
	add	r2,12(r14)
	add	r2,#700
	ld	r5,r2
	exts	rr4
	div	rr4,#7
	ld	r2,r13
	sub	r2,r4
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _sunday */

.data

.even
L34:
.=.+18
/*~~gmtime:*/

.code

_gmtime::
{
	sub	fp,#~L2
	ldm	16(fp),r12,#3
	ld	@fp,r7
	ld	r6,r7
	ldl	rr2,@r6
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#86400
	ldl	~L1+4(fp),rr4
	ld	r6,~L1(fp)
	ldl	rr2,@r6
	ldl	rr6,rr2
	extsl	rq4
	divl	rq4,#86400
	ldl	~L1+8(fp),rr6
	ldl	rr2,~L1+4(fp)
	cpl	rr2,#0
	jpr	ge,L35
	ldl	rr4,rr2
	addl	rr4,#86400
	ldl	~L1+4(fp),rr4
	ldl	rr4,rr6
	subl	rr4,#1
	ldl	~L1+8(fp),rr4
L35:
	ld	r12,#L34
	ldl	rr6,~L1+4(fp)
	extsl	rq4
	divl	rq4,#60
	ld	@r12,r5
	inc	r12,#2
	ldl	rr6,~L1+4(fp)
	extsl	rq4
	divl	rq4,#60
	ld	r13,r7
	ld	r3,r7
	exts	rr2
	div	rr2,#60
	ld	@r12,r2
	inc	r12,#2
	ld	r3,r13
	exts	rr2
	div	rr2,#60
	ld	r13,r3
	ld	@r12,r3
	inc	r12,#2
	ldl	rr6,~L1+8(fp)
	addl	rr6,#7340036
	extsl	rq4
	divl	rq4,#7
	ld	L34+12,r5
	ldl	rr2,~L1+8(fp)
	cpl	rr2,#0
	jpr	lt,L36
	ld	r13,#70
L39:
	ld	r7,r13
	callr	_dysize
	ld	r5,r2
	exts	rr4
	cpl	rr4,~L1+8(fp)
	jpr	gt,L40
	ld	r7,r13
	callr	_dysize
	ld	r5,r2
	exts	rr4
	ldl	rr2,~L1+8(fp)
	subl	rr2,rr4
	ldl	~L1+8(fp),rr2
	inc	r13,#1
	jpr	L39
L36:
	ld	r13,#70
	jpr	L43
L20001:
	ld	r7,r13
	dec	r7,#1
	callr	_dysize
	ld	r5,r2
	exts	rr4
	addl	rr4,~L1+8(fp)
	ldl	~L1+8(fp),rr4
	dec	r13,#1
L43:
	ldl	rr2,~L1+8(fp)
	cpl	rr2,#0
	jpr	lt,L20001
L40:
	ld	L34+10,r13
	ld	r14,~L1+10(fp)
	ld	L34+14,r14
	ld	r7,r13
	callr	_dysize
	cp	r2,#366
	jpr	ne,L44
	ld	_dmsize+2,#29
L44:
	ldk	r13,#0
	jpr	L47
L20003:
	ld	r2,r13
	add	r2,r13
	sub	r14,_dmsize(r2)
	inc	r13,#1
L47:
	ld	r2,r13
	add	r2,r13
	cp	r14,_dmsize(r2)
	jpr	ge,L20003
	ld	_dmsize+2,#28
	lda	r2,1(r14)
	ld	@r12,r2
	inc	r12,#2
	ld	@r12,r13
	inc	r12,#2
	clr	L34+16
	ld	r2,#L34
	ldm	r12,16(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _gmtime */

.data

L49:
.byte	%44,%61,%79,%20,%4d,%6f,%6e,%20
.byte	%30,%30,%20,%30,%30,%3a,%30,%30
.byte	%3a,%30,%30,%20,%31,%39,%30,%30
.byte	%a,%0
.even
L53:
.byte	%53,%75,%6e,%4d,%6f,%6e,%54,%75
.byte	%65,%57,%65,%64,%54,%68,%75,%46
.byte	%72,%69,%53,%61,%74,%0
.even
L54:
.byte	%4a,%61,%6e,%46,%65,%62,%4d,%61
.byte	%72,%41,%70,%72,%4d,%61,%79,%4a
.byte	%75,%6e,%4a,%75,%6c,%41,%75,%67
.byte	%53,%65,%70,%4f,%63,%74,%4e,%6f
.byte	%76,%44,%65,%63,%0
.even
/*~~asctime:*/

.code

_asctime::
{
	dec	fp,#~L2
	ldm	6(fp),r12,#3
	ld	@fp,r7
	ld	r14,#_cbuf
	ld	r13,#L49
L52:
	ld	r2,r13
	inc	r13,#1
	ld	r3,r14
	inc	r14,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	testb	rl4
	jpr	ne,L52
	ld	r2,~L1(fp)
	ld	r5,12(r2)
	mult	rr4,#3
	add	r5,#L53
	ld	r13,r5
	ld	r14,#_cbuf
	ldb	rl3,@r13
	ldb	@r14,rl3
	inc	r13,#1
	inc	r14,#1
	ldb	rl3,@r13
	ldb	@r14,rl3
	inc	r13,#1
	inc	r14,#1
	ldb	rl3,@r13
	ldb	@r14,rl3
	inc	r13,#1
	inc	r14,#1
	inc	r14,#1
	ld	r2,~L1(fp)
	inc	r2,#8
	ld	r12,r2
	ld	r3,@r12
	mult	rr2,#3
	add	r3,#L54
	ld	r13,r3
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	ldb	rl2,@r13
	ldb	@r14,rl2
	inc	r13,#1
	inc	r14,#1
	dec	r12,#2
	ld	r6,@r12
	ld	r7,r14
	callr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#100
	ld	r7,r2
	callr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#100
	ld	r7,r2
	callr	_ct_numb
	dec	r12,#2
	ld	r6,@r12
	add	r6,#100
	ld	r7,r2
	callr	_ct_numb
	ld	r14,r2
	ld	r2,~L1(fp)
	cp	10(r2),#100
	jpr	lt,L55
	ldb	1(r14),#50
	ldb	2(r14),#48
L55:
	inc	r14,#2
	ld	r6,~L1(fp)
	ld	r7,10(r6)
	add	r7,#100
	ld	r6,r7
	ld	r7,r14
	callr	_ct_numb
	ld	r14,r2
	ld	r2,#_cbuf
	ldm	r12,6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _asctime */

.data

/*~~dysize:*/

.code

_dysize::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r3,r7
	exts	rr2
	div	rr2,#4
	test	r2
	jpr	ne,L57
	ld	r2,#366
	jpr	L10018
L57:
	ld	r2,#365
L10018:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _dysize */

.data

/*~~ct_numb:*/

.code

_ct_numb::
{
	dec	fp,#~L2
	ld	6(fp),r14
	ld	@fp,r6
	ld	r14,r7
	inc	r14,#1
	cp	r6,#10
	jpr	lt,L59
	ld	r3,r6
	exts	rr2
	div	rr2,#10
	ld	r5,r3
	ldk	r4,#0
	div	rr4,#10
	add	r4,#48
	ldb	@r14,rl4
	jpr	L20004
L59:
	ldb	@r14,#32
L20004:
	inc	r14,#1
	ld	r3,~L1(fp)
	exts	rr2
	div	rr2,#10
	add	r2,#48
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,r14
	ld	r14,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ct_numb */

.data

L63:
.byte	%54,%5a,%0
.even
/*~~tzset:*/

.code

_tzset::
{
	dec	fp,#~L2
	ldm	4(fp),r12,#3
	ld	r7,#L63
	callr	_getenv
	ld	r14,r2
	test	r2
	jpr	eq,L10024
	testb	@r14
	jpr	eq,L10024
	ldk	r12,#3
	ld	r13,_tzname
L67:
	testb	@r14
	jpr	eq,L10027
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	jpr	L10028
L10027:
	ld	r3,#32
L10028:
	ldb	@r13,rl3
	inc	r13,#1
	dec	r12,#1
	jpr	ne,L67
	cpb	@r14,#45
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
	ld	~L1+2(fp),r2
	test	r2
	jpr	eq,L68
	inc	r14,#1
L68:
	ldk	r12,#0
	jpr	L69
L20006:
	cpb	@r14,#57
	jpr	gt,L70
	ld	r3,r12
	mult	rr2,#10
	ldb	rl2,@r14
	extsb	r2
	add	r3,r2
	sub	r3,#48
	ld	r12,r3
	inc	r14,#1
L69:
	cpb	@r14,#48
	jpr	ge,L20006
L70:
	test	~L1+2(fp)
	jpr	eq,L71
	ld	r2,r12
	neg	r2
	ld	r12,r2
L71:
	ld	r7,r12
	mult	rr6,#60
	ld	r3,r7
	exts	rr2
	ldl	rr6,rr2
	multl	rq4,#60
	ldl	_timezone,rr6
	cpb	@r14,#0
	ldk	r2,#0
	tcc	ne,r2
	extsb	r2
	ld	_daylight,r2
	test	r2
	jpr	eq,L10024
	ld	r13,_tzname+2
	ldk	r12,#3
L75:
	testb	@r14
	jpr	eq,L10031
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	jpr	L10032
L10031:
	ld	r3,#32
L10032:
	ldb	@r13,rl3
	inc	r13,#1
	dec	r12,#1
	jpr	ne,L75
L10024:
	ldm	r12,4(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _tzset */

.data

