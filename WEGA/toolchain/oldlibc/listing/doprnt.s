
fp		:= r15;
sp		:= r15;

.code

_fix:
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	ld	r4,r7
	test	@r4
	jpr	eq,L10003
	cp	@r7,#2
	jpr	ne,L19
L10003:
	ld	r2,~L1+4(fp)
	inc	@r2,#1
	ld	r2,~L1+2(fp)
	inc	@r2,#2
	jpr	L10000
L19:
	ld	r2,~L1+4(fp)
	cp	@r2,#4
	jpr	ne,L21
	inc	@r2,#2
	ld	r2,~L1+2(fp)
	ld	r3,~L1(fp)
	ld	@r2,r3
	jpr	L10000
L21:
	ld	r2,~L1+2(fp)
	ld	r3,@r2
	ld	r4,@r3
	ld	~L1+6(fp),r4
	ld	r3,@r2
	ld	r5,@r2
	ld	r6,2(r3)
	ld	@r5,r6
	ld	r2,~L1+2(fp)
	ld	r3,@r2
	ld	r5,~L1+6(fp)
	ld	2(r3),r5
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fix */

.data

.even
L85:
.byte	%28,%6e,%75,%6c,%6c,%29,%0
.even
.even
L100:
.word	68
.word	79
.word	85
.word	88
.word	99
.word	100
.word	101
.word	102
.word	103
.word	108
.word	111
.word	115
.word	117
.word	120
.even
/*~~_doprnt:*/

.code

__doprnt::
{
	.data
	.even
L54:
	.word	L51
	.word	L51
	.word	L51
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L46
	.word	L52
	.even
L101:
	.word	L20027
	.word	L20025
	.word	L80
	.word	L20026
	.word	L87
	.word	L63
	.word	L91
	.word	L91
	.word	L91
	.word	L65
	.word	L57
	.word	L83
	.word	L60
	.word	L59
	.code
	sub	fp,#~L2
	ldm	108(fp),r11,#4
	ldm	@fp,r2,#5
	ld	r14,r7
	clr	~L1+18(fp)
	ld	~L1+20(fp),r6
	callr	_strlen
	ld	r12,r2
	jpr	L27
L20003:
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	lt,L20001
L20010:
	ld	r2,r13
	ld	r3,~L1(fp)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	subb	rh2,rh2
L27:
	ld	r2,r12
	dec	r12,#1
	test	r2
	jpr	eq,L10004
	ldb	rl2,@r14
	extsb	r2
	and	r2,#255
	ld	r13,r2
	inc	r14,#1
	cp	r2,#37
	jpr	ne,L20003
	test	r12
	jpr	eq,L10004
	dec	r12,#1
	ldb	rl2,@r14
	extsb	r2
	and	r2,#255
	ld	r13,r2
	inc	r14,#1
	clr	~L1+24(fp)
	ld	~L1+26(fp),#-1
	ldb	~L1+105(fp),#32
	clr	~L1+100(fp)
	cp	r2,#45
	jpr	ne,L32
	inc	~L1+100(fp),#1
	ld	r2,r12
	dec	r12,#1
	test	r2
	jpr	eq,L10004
	ldb	rl2,@r14
	extsb	r2
	and	r2,#255
	ld	r13,r2
	inc	r14,#1
L32:
	cp	r13,#97
	jpr	ge,L34
	cp	r13,#42
	jpr	ne,L35
	ld	r2,~L1+20(fp)
	ld	r3,@r2
	ld	~L1+24(fp),r3
	inc	~L1+20(fp),#2
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	ne,L37
	ld	r4,~L1+4(fp)
	ld	~L1+20(fp),r4
	jpr	L37
L35:
	cp	r13,#48
	jpr	ne,L38
	ldb	~L1+105(fp),#48
L38:
	lda	r5,~L1+24(fp)
	inc	r12,#1
	ld	r6,r12
	dec	r14,#1
	ld	r7,r14
	callr	_getnum
	ld	r11,r2
	add	r14,r2
	sub	r12,r2
L37:
	test	r12
	jpr	eq,L10004
	cpb	@r14,#46
	jpr	ne,L40
	inc	r14,#1
	dec	r12,#1
	test	r12
	jpr	eq,L10004
	cpb	@r14,#42
	jpr	ne,L42
	ld	r2,~L1+20(fp)
	ld	r3,@r2
	ld	~L1+26(fp),r3
	inc	~L1+20(fp),#2
	ldk	r11,#1
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	ne,L44
	ld	r4,~L1+4(fp)
	ld	~L1+20(fp),r4
	jpr	L44
L42:
	lda	r5,~L1+26(fp)
	ld	r6,r12
	ld	r7,r14
	callr	_getnum
	ld	r11,r2
L44:
	sub	r12,r11
	add	r14,r11
L40:
	test	r12
	jpr	eq,L10004
	dec	r12,#1
	ldb	rl2,@r14
	extsb	r2
	and	r2,#255
	ld	r13,r2
	inc	r14,#1
L34:
	cp	~L1+26(fp),#0
	jpr	ge,L46
	ld	r2,r13
	sub	r2,#101
	cp	r2,#14
	jpr	ugt,L46
	rl	r2,#1
	ld	r2,L54(r2)
	jp	@r2
L51:
	ld	~L1+26(fp),#6
L46:
	ld	~L1+106(fp),r13
	ld	r2,r13
	ld	r3,#L100
	ld	r4,#14
	cpir	r2,@r3,r4,eq
	jpr	ne,L99
	add	r4,r4
	neg	r4
	ld	r2,L101+26(r4)
	jp	@r2
L52:
	clr	~L1+26(fp)
	jpr	L46
L57:
	ld	~L1+28(fp),#8
	jpr	L58
L59:
	ld	~L1+28(fp),#16
	jpr	L58
L60:
	ld	~L1+28(fp),#10
L58:
	ld	r3,~L1+20(fp)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	~L1+30(fp),rr6
	inc	~L1+20(fp),#2
	clr	~L1+30(fp)
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	ne,L62
L20024:
	ld	r4,~L1+4(fp)
	ld	~L1+20(fp),r4
	jpr	L62
L63:
	ld	~L1+28(fp),#-10
	ld	r3,~L1+20(fp)
	ld	r4,@r3
	ld	r7,r4
	exts	rr6
	ldl	~L1+30(fp),rr6
	inc	~L1+20(fp),#2
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	eq,L20024
	jpr	L62
L65:
	test	r12
	jpr	eq,L67
	dec	r12,#1
	ldb	rl3,@r14
	extsb	r3
	and	r3,#255
	ld	r13,r3
	inc	r14,#1
	cp	r3,#111
	jpr	ne,L68
L20025:
	ld	~L1+28(fp),#8
	jpr	L76
L68:
	cp	r13,#120
	jpr	ne,L70
L20026:
	ld	~L1+28(fp),#16
	jpr	L76
L70:
	cp	r13,#100
	jpr	ne,L72
L20027:
	ld	~L1+28(fp),#-10
	jpr	L76
L72:
	cp	r13,#117
	jpr	ne,L74
L80:
	ld	~L1+28(fp),#10
L76:
	cp	~L1+2(fp),#5
	jpr	ge,L81
	lda	r7,~L1+2(fp)
	lda	r6,~L1+20(fp)
	ld	r5,~L1+4(fp)
	callr	_fix
L81:
	ld	r2,~L1+20(fp)
	ldl	rr4,@r2
	ldl	~L1+30(fp),rr4
	inc	~L1+20(fp),#4
	inc	~L1+2(fp),#2
	cp	~L1+2(fp),#5
	jpr	ne,L62
	ld	r3,~L1+4(fp)
	ld	~L1+20(fp),r3
L62:
	lda	r2,~L1+36(fp)
	ld	~L1+34(fp),r2
	ld	r7,r2
	ld	r6,~L1+28(fp)
	ldl	rr4,~L1+30(fp)
	callr	__ltostr
L20028:
	ld	r11,r2
	jpr	L67
L74:
	ld	r3,~L1(fp)
	dec	2(r3),#1
	ld	r4,2(r3)
	cp	r4,#0
	jpr	ge,L10009
	ld	r7,#108
	ld	r6,~L1(fp)
	callr	__flsbuf
	jpr	L10010
L10009:
	ld	r2,~L1(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	@r3,#108
	ldb	rl2,#108
	subb	rh2,rh2
L10010:
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10011
	ld	r7,r13
	ld	r6,~L1(fp)
	callr	__flsbuf
L67:
	test	~L1+26(fp)
	jpr	eq,L102
	cp	~L1+106(fp),#115
	jpr	ne,L102
	cp	r11,~L1+26(fp)
	jpr	le,L102
	ld	r11,~L1+26(fp)
L102:
	cp	r11,~L1+24(fp)
	jpr	ge,L103
	ld	r2,~L1+24(fp)
	sub	r2,r11
	ld	~L1+102(fp),r2
	jpr	L104
L10011:
	ld	r2,r13
	ld	r3,~L1(fp)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	@r4,rl2
	subb	rh2,rh2
	jpr	L67
L83:
	ldb	~L1+105(fp),#32
	ld	r2,~L1+20(fp)
	ld	r3,@r2
	ld	~L1+34(fp),r3
	inc	~L1+20(fp),#2
	test	r3
	jpr	ne,L84
	ld	~L1+34(fp),#L85
L84:
	ld	r7,~L1+34(fp)
	callr	_strlen
	ld	r11,r2
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	ne,L67
	ld	r2,~L1+4(fp)
	ld	~L1+20(fp),r2
	jpr	L67
L87:
	ldb	~L1+105(fp),#32
	ldk	r11,#1
	ld	r2,~L1+20(fp)
	ld	~L1+34(fp),r2
	inc	~L1+20(fp),#2
	inc	~L1+34(fp),#1
	inc	~L1+2(fp),#1
	cp	~L1+2(fp),#5
	jpr	ne,L67
	ld	r3,~L1+4(fp)
	ld	~L1+20(fp),r3
	jpr	L67
L91:
	cp	~L1+2(fp),#5
	jpr	ge,L92
	cp	~L1+18(fp),#4
	jpr	ge,L93
	ld	r2,~L1+20(fp)
	ld	~L1+22(fp),r2
	ld	r3,~L1+6(fp)
	ld	~L1+20(fp),r3
	inc	~L1+6(fp),#8
	inc	~L1+18(fp),#1
	jpr	L96
L93:
	cp	~L1+18(fp),#4
	jpr	ne,L96
	clr	~L1+22(fp)
	ld	r2,~L1+4(fp)
	ld	~L1+20(fp),r2
	inc	~L1+2(fp),#5
	jpr	L96
L92:
	clr	~L1+22(fp)
L96:
	lda	r5,~L1+36(fp)
	ld	r7,~L1+26(fp)
	ld	r6,~L1+106(fp)
	ld	r4,~L1+20(fp)
	fldd	f3,@r4
	callr	_fcv
	ld	~L1+34(fp),r2
	test	~L1+22(fp)
	jpr	eq,L97
	ld	r2,~L1+22(fp)
	ld	~L1+20(fp),r2
	jpr	L98
L97:
	inc	~L1+20(fp),#8
L98:
	ld	r7,~L1+34(fp)
	callr	_strlen
	jpr	L20028
L99:
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L20010
L20001:
	ld	r7,r13
	ld	r6,~L1(fp)
	callr	__flsbuf
	jpr	L27
L103:
	clr	~L1+102(fp)
L104:
	test	~L1+100(fp)
	jpr	eq,L106
	jpr	L108
L20012:
	ldb	rl7,~L1+105(fp)
	extsb	r7
	ld	r6,~L1(fp)
	callr	__flsbuf
	jpr	L106
L20014:
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	lt,L20012
	ld	r2,~L1(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,~L1+105(fp)
	ldb	@r3,rl2
	subb	rh2,rh2
L106:
	ld	r2,~L1+102(fp)
	dec	~L1+102(fp),#1
	test	r2
	jpr	ne,L20014
	jpr	L108
L20016:
	ld	r7,~L1+34(fp)
	inc	~L1+34(fp),#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,~L1(fp)
	callr	__flsbuf
	jpr	L108
L20018:
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	lt,L20016
	ld	r2,~L1+34(fp)
	inc	~L1+34(fp),#1
	ld	r3,~L1(fp)
	inc	@r3,#1
	ld	r4,@r3
	dec	r4,#1
	ldb	rl3,@r2
	ldb	@r4,rl3
	subb	rh3,rh3
L108:
	ld	r2,r11
	dec	r11,#1
	test	r2
	jpr	ne,L20018
	test	~L1+100(fp)
	jpr	ne,L111
	jpr	L27
L20020:
	ldb	rl7,~L1+105(fp)
	extsb	r7
	ld	r6,~L1(fp)
	callr	__flsbuf
L111:
	ld	r2,~L1+102(fp)
	dec	~L1+102(fp),#1
	test	r2
	jpr	eq,L27
	ld	r2,~L1(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	lt,L20020
	ld	r2,~L1(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,~L1+105(fp)
	ldb	@r3,rl2
	subb	rh2,rh2
	jpr	L111
L10004:
	ldm	r11,108(fp),#4
	add	fp,#~L2
	ret
	.extern	__fltuse
	~L1 := 0
	~L2 := 116
	}	/* __doprnt */

.data


.code

_getnum:
{
	sub	fp,#~L2
	ldm	@fp,r5,#2
	ld	r1,r7
	ld	r4,r6
	clr	~L1+6(fp)
	test	r6
	jpr	eq,L114
	cpb	@r1,#45
	jpr	ne,L114
	dec	~L1+2(fp),#1
	inc	r1,#1
	ld	~L1+6(fp),#1
L114:
	ldk	r5,#0
	jpr	L115
L20030:
	ldb	rl2,@r1
	ldb	~L1+9(fp),rl2
	inc	r1,#1
	cpb	rl2,#48
	jpr	lt,L116
	cpb	rl2,#57
	jpr	gt,L116
	ld	r3,r5
	mult	rr2,#10
	ldb	rl2,~L1+9(fp)
	extsb	r2
	sub	r2,#48
	add	r3,r2
	ld	r5,r3
	dec	r4,#1
L115:
	test	r4
	jpr	ne,L20030
L116:
	test	~L1+6(fp)
	jpr	eq,L10025
	ld	r2,r5
	neg	r2
	jpr	L10026
L10025:
	ld	r2,r5
L10026:
	ld	r3,~L1(fp)
	ld	@r3,r2
	ld	r2,~L1+2(fp)
	sub	r2,r4
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _getnum */

.data


.code

_ecv:
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* _ecv */

.data


.code

_fcv:
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* _fcv */

.data


.code

_gcv:
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* _gcv */

.data

