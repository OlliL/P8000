
fp		:= r15;
sp		:= r15;
__sctab::
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%1
.byte	%1
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%1
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.=.+64
.even
L59:
.word	0
.word	9
.word	10
.word	32
.word	37
.even
/*~~_doscan:*/

.code

__doscan::
{
	.data
	.even
L60:
	.word	L20004
	.word	L52
	.word	L52
	.word	L52
	.word	L29
	.code
	sub	fp,#~L2
	ldm	20(fp),r12,#3
	ld	~L1+4(fp),r7
	ld	r13,r5
	ld	r14,r6
	clr	~L1+6(fp)
	clr	~L1+14(fp)
	jpr	L25
L20003:
	add	r4,r4
	neg	r4
	ld	r2,L60+8(r4)
	jp	@r2
L29:
	ld	r2,r14
	inc	r14,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r12,r3
	cp	r3,#37
	jpr	eq,L31
	clr	~L1+12(fp)
	cp	r3,#42
	jpr	eq,L32
	ld	~L1+12(fp),r13
	inc	r13,#2
	jpr	L33
L32:
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L33:
	clr	~L1+8(fp)
	ld	~L1+16(fp),#1
	jpr	L34
L20001:
	ld	r3,~L1+8(fp)
	mult	rr2,#10
	add	r3,r12
	sub	r3,#48
	ld	~L1+8(fp),r3
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
L34:
	ldb	rl2,__ctype+1(r12)
	extsb	r2
	bit	r2,#2
	jpr	ne,L20001
	test	~L1+8(fp)
	jpr	ne,L36
	ld	~L1+8(fp),#30000
L36:
	cp	r12,#108
	jpr	ne,L37
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	ld	~L1+16(fp),#2
	jpr	L38
L37:
	cp	r12,#104
	jpr	ne,L39
	clr	~L1+16(fp)
	ldb	rl2,@r14
	extsb	r2
	ld	r12,r2
	inc	r14,#1
	jpr	L38
L39:
	cp	r12,#91
	jpr	ne,L38
	ld	r7,r14
	callr	__getccl
	ld	r14,r2
L38:
	ldb	rl2,__ctype+1(r12)
	extsb	r2
	bit	r2,#0
	jpr	eq,L42
	ld	r7,r12
	callr	_tolower
	ld	r12,r2
	ld	~L1+16(fp),#2
L42:
	test	r12
	jpr	ne,L44
L20005:
	ld	r2,#-1
	jpr	L10000
L44:
	lda	r2,~L1+14(fp)
	ld	r6,r12
	ld	r5,~L1+8(fp)
	ld	r4,~L1+16(fp)
	ld	r3,~L1+4(fp)
	ld	r7,~L1+12(fp)
	callr	__innum
	test	r2
	jpr	eq,L46
	test	~L1+12(fp)
	jpr	eq,L46
	inc	~L1+6(fp),#1
L46:
	test	~L1+14(fp)
	jpr	eq,L25
	test	~L1+6(fp)
	jpr	eq,L20005
	jpr	L20004
L52:
	ld	r2,~L1+4(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10006
	ld	r7,~L1+4(fp)
	callr	__filbuf
	jpr	L10007
L10006:
	ld	r2,~L1+4(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10007:
	ld	~L1+10(fp),r2
	cp	r2,#32
	jpr	eq,L52
	cp	r2,#9
	jpr	eq,L52
	cp	r2,#10
	jpr	eq,L52
	cp	r2,#-1
	jpr	eq,L25
	ld	r7,r2
	ld	r6,~L1+4(fp)
	callr	_ungetc
L25:
	ldb	rl3,@r14
	extsb	r3
	ld	r12,r3
	ld	r2,r3
	inc	r14,#1
	ld	r3,#L59
	ld	r4,#5
	cpir	r2,@r3,r4,eq
	jpr	eq,L20003
L31:
	ld	r2,~L1+4(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10008
	ld	r7,~L1+4(fp)
	callr	__filbuf
	jpr	L10009
L10008:
	ld	r2,~L1+4(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10009:
	ld	~L1+10(fp),r2
	cp	r12,~L1+10(fp)
	jpr	eq,L25
	cp	r2,#-1
	jpr	eq,L20005
	ld	r7,r2
	ld	r6,~L1+4(fp)
	callr	_ungetc
L20004:
	ld	r2,~L1+6(fp)
L10000:
	ldm	r12,20(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 26
	}	/* __doscan */

.data

.even
/*~~_innum:*/

.code

__innum::
{
	.data
	.even
L105:
	.word	L101
	.word	L101
	.word	L103
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L96
	.word	L99
	.word	L99
	.word	L100
	.code
	sub	fp,#~L2
	ldm	92(fp),r12,#3
	ldm	@fp,r2,#6
	cp	r6,#99
	jpr	eq,L10013
	cp	r6,#115
	jpr	eq,L10013
	cp	~L1+8(fp),#91
	jpr	ne,L63
L10013:
	test	~L1+10(fp)
	jpr	eq,L10014
	ld	r7,~L1+10(fp)
	ld	r6,@r7
	ld	r7,r6
	jpr	L10015
L10014:
	sub	r7,r7
L10015:
	ld	r6,~L1+8(fp)
	ld	r5,~L1+6(fp)
	ld	r4,~L1+2(fp)
	ld	r3,~L1(fp)
	callr	__instr
	jpr	L10010
L63:
	subl	rr2,rr2
	ldl	~L1+86(fp),rr2
	clr	~L1+84(fp)
	clr	~L1+78(fp)
	cp	~L1+8(fp),#101
	jpr	eq,L10016
	cp	~L1+8(fp),#102
	jpr	ne,L65
L10016:
	ld	~L1+78(fp),#1
L65:
	ldk	r12,#10
	cp	~L1+8(fp),#111
	jpr	ne,L66
	ldk	r12,#8
	jpr	L67
L66:
	cp	~L1+8(fp),#120
	jpr	ne,L67
	ld	r12,#16
L67:
	lda	r14,~L1+12(fp)
	clr	~L1+76(fp)
	clr	~L1+80(fp)
L69:
	ld	r2,~L1+2(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10018
	ld	r7,~L1+2(fp)
	callr	__filbuf
	jpr	L10019
L10018:
	ld	r2,~L1+2(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10019:
	ld	r13,r2
	cp	r2,#32
	jpr	eq,L69
	cp	r2,#9
	jpr	eq,L69
	cp	r2,#10
	jpr	eq,L69
	cp	r2,#45
	jpr	ne,L71
	inc	~L1+80(fp),#1
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,~L1+2(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10020
	ld	r7,~L1+2(fp)
	callr	__filbuf
	jpr	L10021
L10020:
	ld	r2,~L1+2(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10021:
	ld	r13,r2
	dec	~L1+6(fp),#1
	jpr	L76
L71:
	cp	r13,#43
	jpr	ne,L76
	dec	~L1+6(fp),#1
	ld	r2,~L1+2(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	lt,L20017
L10022:
	ld	r2,~L1+2(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
	jpr	L10023
L20015:
	cp	r13,#97
	jpr	lt,L10026
	cp	r13,#102
	jpr	le,L10024
L10026:
	cp	r13,#65
	jpr	lt,L77
	cp	r13,#70
	jpr	gt,L77
L10024:
	inc	~L1+84(fp),#1
	cp	r12,#8
	jpr	ne,L78
	ldl	rr2,~L1+86(fp)
	slal	rr2,#3
	jpr	L20006
L78:
	cp	r12,#10
	jpr	ne,L80
	ldl	rr2,~L1+86(fp)
	slal	rr2,#2
	addl	rr2,~L1+86(fp)
	slal	rr2,#1
	jpr	L20006
L80:
	ldl	rr2,~L1+86(fp)
	slal	rr2,#4
L20006:
	ldl	~L1+86(fp),rr2
	ld	~L1+82(fp),r13
	cp	r13,#48
	jpr	lt,L82
	cp	r13,#57
	jpr	gt,L82
	sub	r13,#48
	jpr	L83
L82:
	cp	r13,#97
	jpr	lt,L84
	cp	r13,#102
	jpr	gt,L84
	sub	r13,#87
	jpr	L83
L84:
	sub	r13,#55
L83:
	ld	r3,r13
	exts	rr2
	addl	rr2,~L1+86(fp)
	ldl	~L1+86(fp),rr2
	ld	r13,~L1+82(fp)
	jpr	L74
L20008:
	cp	r12,#10
	jpr	ne,L75
	test	~L1+78(fp)
	jpr	eq,L75
	inc	~L1+84(fp),#1
L74:
	ld	r2,r13
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,~L1+2(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10022
L20017:
	ld	r7,~L1+2(fp)
	callr	__filbuf
L10023:
	ld	r13,r2
L76:
	dec	~L1+6(fp),#1
	cp	~L1+6(fp),#0
	jpr	lt,L75
	ldb	rl2,__ctype+1(r13)
	extsb	r2
	bit	r2,#2
	jpr	ne,L10024
	cp	r12,#16
	jpr	eq,L20015
L77:
	cp	r13,#46
	jpr	eq,L20008
	cp	r13,#101
	jpr	eq,L10028
	cp	r13,#69
	jpr	ne,L75
L10028:
	test	~L1+76(fp)
	jpr	ne,L75
	cp	r12,#10
	jpr	ne,L75
	test	~L1+78(fp)
	jpr	eq,L75
	test	~L1+84(fp)
	jpr	eq,L75
	inc	~L1+76(fp),#1
	ld	r2,r13
	ldb	@r14,rl2
	inc	r14,#1
	ld	r2,~L1+2(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10030
	ld	r7,~L1+2(fp)
	callr	__filbuf
	jpr	L10031
L10030:
	ld	r2,~L1+2(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10031:
	ld	r13,r2
	cp	r2,#43
	jpr	eq,L74
	cp	r2,#45
	jpr	eq,L74
	cp	r2,#48
	jpr	lt,L75
	cp	r2,#57
	jpr	le,L74
L75:
	test	~L1+80(fp)
	jpr	eq,L92
	subl	rr2,rr2
	subl	rr2,~L1+86(fp)
	ldl	~L1+86(fp),rr2
L92:
	cp	r13,#-1
	jpr	eq,L93
	ld	r7,r13
	ld	r6,~L1+2(fp)
	callr	_ungetc
	ld	r2,~L1(fp)
	clr	@r2
	jpr	L94
L93:
	ld	r2,~L1(fp)
	ld	@r2,#1
L94:
	test	~L1+10(fp)
	jpr	eq,L10035
	lda	r2,~L1+12(fp)
	cp	r14,r2
	jpr	ne,L95
L10035:
	sub	r2,r2
	jpr	L10010
L95:
	clrb	@r14
	inc	r14,#1
	ld	r3,~L1+78(fp)
	sla	r3,#4
	or	r3,~L1+4(fp)
	ld	r2,r3
	cp	r3,#18
	jpr	ugt,L96
	rl	r2,#1
	ld	r2,L105(r2)
	jp	@r2
L99:
	lda	r7,~L1+12(fp)
	callr	_atof
	ld	r2,~L1+10(fp)
	ld	r3,@r2
	flds	@r3,f0
L96:
	ldk	r2,#1
	jpr	L10010
L100:
	lda	r7,~L1+12(fp)
	callr	_atof
	ld	r2,~L1+10(fp)
	ld	r3,@r2
	fldd	@r3,f0
	jpr	L96
L101:
	ld	r2,~L1+10(fp)
	ld	r3,@r2
	ld	r4,~L1+88(fp)
	ld	@r3,r4
	jpr	L96
L103:
	ld	r2,~L1+10(fp)
	ld	r3,@r2
	ldl	rr4,~L1+86(fp)
	ldl	@r3,rr4
	jpr	L96
L10010:
	ldm	r12,92(fp),#3
	add	fp,#~L2
	ret
	.extern	__fltuse
	~L1 := 0
	~L2 := 98
	}	/* __innum */

.data

/*~~_instr:*/

.code

__instr::
{
	sub	fp,#~L2
	ldm	14(fp),r11,#4
	ldm	@fp,r3,#4
	ld	r13,r4
	ld	r14,r7
	ld	r2,r3
	clr	@r2
	ld	r11,r14
	cp	r6,#99
	jpr	ne,L107
	cp	r5,#30000
	jpr	ne,L107
	ld	~L1+4(fp),#1
L107:
	clr	~L1+10(fp)
	cp	~L1+6(fp),#115
	jpr	ne,L109
	ld	~L1+10(fp),#1
L109:
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10039
	ld	r7,r13
	callr	__filbuf
	jpr	L10040
L10039:
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10040:
	ld	r12,r2
	ldb	rl3,__sctab(r2)
	extsb	r3
	and	r3,~L1+10(fp)
	jpr	eq,L110
	cp	r12,#-1
	jpr	ne,L109
L110:
	ld	~L1+10(fp),#1
	cp	~L1+6(fp),#99
	jpr	ne,L112
	clr	~L1+10(fp)
	jpr	L115
L112:
	cp	~L1+6(fp),#91
	jpr	ne,L115
	ld	~L1+10(fp),#2
	jpr	L115
L20022:
	ldb	rl2,__sctab(r12)
	extsb	r2
	and	r2,~L1+10(fp)
	test	r2
	jpr	ne,L116
	test	r14
	jpr	eq,L117
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
L117:
	dec	~L1+4(fp),#1
	cp	~L1+4(fp),#0
	jpr	le,L116
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10041
	ld	r7,r13
	callr	__filbuf
	jpr	L10042
L10041:
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10042:
	ld	r12,r2
L115:
	cp	r12,#-1
	jpr	ne,L20022
L116:
	cp	r12,#-1
	jpr	eq,L119
	cp	~L1+4(fp),#0
	jpr	le,L120
	ld	r7,r12
	ld	r6,r13
	callr	_ungetc
L120:
	ld	r2,~L1(fp)
	clr	@r2
	jpr	L121
L119:
	ld	r2,~L1(fp)
	ld	@r2,#1
L121:
	test	r14
	jpr	eq,L122
	cp	r14,r11
	jpr	eq,L122
	cp	~L1+6(fp),#99
	jpr	eq,L123
	clrb	@r14
	inc	r14,#1
L123:
	ldk	r2,#1
	jpr	L10036
L122:
	sub	r2,r2
L10036:
	ldm	r11,14(fp),#4
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* __instr */

.data

/*~~_getccl:*/

.code

__getccl::
{
	dec	fp,#~L2
	ld	r1,r7
	ldk	r5,#0
	cpb	@r1,#94
	jpr	ne,L125
	inc	r5,#1
	inc	r1,#1
L125:
	ldk	r4,#0
L20024:
	test	r5
	jpr	eq,L129
	resb	__sctab(r4),#1
	jpr	L126
L129:
	setb	__sctab(r4),#1
L126:
	inc	r4,#1
	cp	r4,#128
	jpr	lt,L20024
L131:
	ld	r2,r1
	inc	r1,#1
	ldb	rl3,@r2
	extsb	r3
	ld	r4,r3
	and	r3,#127
	cp	r3,#93
	jpr	eq,L132
	test	r5
	jpr	eq,L133
	setb	__sctab(r4),#1
	jpr	L20025
L133:
	resb	__sctab(r4),#1
L20025:
	inc	r4,#1
	test	r4
	jpr	ne,L131
	dec	r1,#1
L132:
	ld	r2,r1
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* __getccl */

.data

