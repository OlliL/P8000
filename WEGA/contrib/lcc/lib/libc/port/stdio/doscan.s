.line 1 "doscan.c"
	.seg
	fp := r13
	FP := rr12
	sp := r15
	SP := rr14
	.psec	data   
	.data
	.psec
	.code
__doscan::
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	sub	sp,#~L2-%c
	ld	fp,sp
	ld	@SP,#~L2-4
	ldl	rr10,FP(#~L2+%4)
	ldl	rr8,FP(#~L2+%8)
	clr	_stkseg+~L1+%102(fp)
L36:
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	ld	FP(#~L1+%100),r2
	cp	r2,#0
	jpr	ne,L37
	ld	r2,FP(#~L1+%102)
	jpr	L32
L37:
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%100(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%3
	jpr	eq,L38
L40:
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L42
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L43
L42:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L43:
	ld	FP(#~L1+%106),r2
	ld	r3,r2
	ld	r2,#^s __ctype
	add	r3,#^o __ctype+%1
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%3
	jpr	eq,L41
	jpr	L40
L41:
	pushl	@SP,rr10
	push	@SP,_stkseg+~L1+%106(fp)
	callr	_ungetc
	inc	sp,#%6
	cp	r2,#%ffff
	jpr	eq,L44
	jpr	L34
L44:
	jpr	L35
L38:
	cp	_stkseg+~L1+%100(fp),#%25
	jpr	ne,L46
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	ld	FP(#~L1+%100),r2
	cp	r2,#%25
	jpr	ne,L45
L46:
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L48
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L49
L48:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L49:
	ld	FP(#~L1+%106),r2
	cp	r2,_stkseg+~L1+%100(fp)
	jpr	ne,L47
	jpr	L34
L47:
	pushl	@SP,rr10
	push	@SP,_stkseg+~L1+%106(fp)
	callr	_ungetc
	inc	sp,#%6
	cp	r2,#%ffff
	jpr	eq,L50
	ld	r2,FP(#~L1+%102)
	jpr	L32
L50:
	jpr	L35
L45:
	cp	_stkseg+~L1+%100(fp),#%2a
	jpr	ne,L51
	clr	_stkseg+~L1+%108(fp)
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	ld	FP(#~L1+%100),r2
	jpr	L52
L51:
	ld	_stkseg+~L1+%108(fp),#%1
L52:
	clr	_stkseg+~L1+%104(fp)
L55:
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%100(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%2
	jpr	eq,L54
	ld	r2,FP(#~L1+%104)
	add 	r2,r2
	ld	r3,r2
	sla 	r2,#%2
	add	r2,r3
	add	r2,_stkseg+~L1+%100(fp)
	sub	r2,#%30
	ld	FP(#~L1+%104),r2
L53:
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	ld	FP(#~L1+%100),r2
	jpr	L55
L54:
	cp	_stkseg+~L1+%104(fp),#0
	jpr	ne,L56
	ld	_stkseg+~L1+%104(fp),#%7fff
L56:
	ld	r2,FP(#~L1+%100)
	ld	FP(#~L1+%10a),r2
	cp	r2,#%6c
	jpr	eq,L58
	cp	_stkseg+~L1+%10a(fp),#%68
	jpr	ne,L57
L58:
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	ld	FP(#~L1+%100),r2
L57:
	cp	_stkseg+~L1+%100(fp),#0
	jpr	eq,L60
	cp	_stkseg+~L1+%100(fp),#%5b
	jpr	ne,L59
	ldl	rr2,FP
	add	r3,#~L1+%0
	pushl	@SP,rr2
	pushl	@SP,rr8
	callr	_setup
	inc	sp,#%8
	ldl	rr8,rr2
	cpl	rr8,#0
	jpr	ne,L59
L60:
	ld	r2,#%ffff
	jpr	L32
L59:
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%100(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%0
	jpr	eq,L61
	ld	_stkseg+~L1+%10a(fp),#%6c
	ld	r2,FP(#~L1+%100)
	add	r2,#%20
	ld	FP(#~L1+%100),r2
L61:
	cp	_stkseg+~L1+%100(fp),#%63
	jpr	eq,L62
	cp	_stkseg+~L1+%100(fp),#%5b
	jpr	eq,L62
L63:
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L65
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L66
L65:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L66:
	ld	FP(#~L1+%106),r2
	ld	r3,r2
	ld	r2,#^s __ctype
	add	r3,#^o __ctype+%1
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%3
	jpr	eq,L64
	jpr	L63
L64:
	pushl	@SP,rr10
	push	@SP,_stkseg+~L1+%106(fp)
	callr	_ungetc
	inc	sp,#%6
	cp	r2,#%ffff
	jpr	ne,L67
	jpr	L35
L67:
L62:
	cp	_stkseg+~L1+%100(fp),#%63
	jpr	eq,L73
	cp	_stkseg+~L1+%100(fp),#%73
	jpr	eq,L73
	cp	_stkseg+~L1+%100(fp),#%5b
	jpr	ne,L71
L73:
	ldl	rr2,FP
	add	r3,#~L2+%c
	pushl	@SP,rr2
	pushl	@SP,rr10
	ldl	rr2,FP
	add	r3,#~L1+%0
	pushl	@SP,rr2
	push	@SP,_stkseg+~L1+%104(fp)
	push	@SP,_stkseg+~L1+%100(fp)
	push	@SP,_stkseg+~L1+%108(fp)
	callr	_string
	add	sp,#%12
	jpr	L72
L71:
	ldl	rr2,FP
	add	r3,#~L2+%c
	pushl	@SP,rr2
	pushl	@SP,rr10
	push	@SP,_stkseg+~L1+%10a(fp)
	push	@SP,_stkseg+~L1+%104(fp)
	push	@SP,_stkseg+~L1+%100(fp)
	push	@SP,_stkseg+~L1+%108(fp)
	callr	_number
	inc	sp,#%10
L72:
	ld	FP(#~L1+%10a),r2
	cp	r2,#0
	jpr	eq,L70
	ld	r2,FP(#~L1+%102)
	add	r2,_stkseg+~L1+%108(fp)
	ld	FP(#~L1+%102),r2
L70:
	ldl	rr2,FP(#~L2+%c)
	cpl	rr2,#0
	jpr	ne,L74
	jpr	L35
L74:
	cp	_stkseg+~L1+%10a(fp),#0
	jpr	ne,L75
	ld	r2,FP(#~L1+%102)
	jpr	L32
L75:
L34:
	jpr	L36
L35:
	cp	_stkseg+~L1+%102(fp),#0
	jpr	eq,L76
	ld	r2,FP(#~L1+%102)
	jpr	L77
L76:
	ld	r2,#%ffff
L77:
	jpr	L32
L32:
	add	sp,#~L2-%c
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 2
	~L2 := 282
	}
	.psec	data   
	.data
	.psec
	.code
_number:
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	sub	sp,#~L2-%c
	ld	fp,sp
	ld	@SP,#~L2-4
	ldl	rr10,FP(#~L2+%c)
	ldl	rr8,FP
	add	r9,#~L1+%0
	clr	_stkseg+~L1+%44(fp)
	clr	_stkseg+~L1+%46(fp)
	clr	_stkseg+~L1+%48(fp)
	clr	_stkseg+~L1+%4a(fp)
	clr	_stkseg+~L1+%4c(fp)
	subl	rr2,rr2
	ldl	FP(#~L1+%4e),rr2
	ld	r2,FP(#~L2+%6)
	jpr	L80
L81:
L82:
L83:
	inc	_stkseg+~L1+%4a(fp),#%1
L84:
L85:
	ld	_stkseg+~L1+%42(fp),#%a
	jpr	L79
L86:
	ld	_stkseg+~L1+%42(fp),#%8
	jpr	L79
L87:
	ld	_stkseg+~L1+%42(fp),#%10
	jpr	L79
L88:
	xor	r2,r2
	jpr	L78
L80:
	sub	r2,#%64
	cp	r2,#%14
	jpr	ugt,L88
	rl	r2,#2
	ldl	rr4,L89(r2)
	jp	@rr4
	.psec data
	.data
	.even
L89:
	.addr	L84
	.addr	L81
	.addr	L82
	.addr	L83
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L86
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L88
	.addr	L85
	.addr	L88
	.addr	L88
	.addr	L87
	.psec
	.code
L79:
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L91
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L92
L91:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L92:
	ld	FP(#~L1+%40),r2
	jpr	L93
L94:
	inc	_stkseg+~L1+%4c(fp),#%1
L95:
	dec	_stkseg+~L2+%8(fp),#%1
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L96
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L97
L96:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L97:
	ld	FP(#~L1+%40),r2
	jpr	L90
L93:
	sub	r2,#%2b
	cp	r2,#%2
	jpr	ugt,L98
	rl	r2,#2
	ldl	rr4,L99(r2)
	jp	@rr4
	.psec data
	.data
	.even
L99:
	.addr	L95
	.addr	L98
	.addr	L94
	.psec
	.code
L98:
L90:
L102:
	dec	_stkseg+~L2+%8(fp),#%1
	cp	_stkseg+~L2+%8(fp),#0
	jpr	lt,L101
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%40(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%2
	jpr	ne,L104
	cp	_stkseg+~L1+%42(fp),#%10
	jpr	ne,L103
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%40(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%7
	jpr	eq,L103
L104:
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%40(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%2
	jpr	eq,L105
	ld	r2,#%30
	jpr	L106
L105:
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%40(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%0
	jpr	eq,L107
	ld	r2,#%37
	jpr	L108
L107:
	ld	r2,#%57
L108:
L106:
	ld	r3,FP(#~L1+%40)
	sub	r3,r2
	ld	FP(#~L1+%52),r3
	ld	r2,FP(#~L1+%52)
	cp	r2,_stkseg+~L1+%42(fp)
	jpr	lt,L109
	jpr	L101
L109:
	cp	_stkseg+~L2+%4(fp),#0
	jpr	eq,L110
	cp	_stkseg+~L1+%4a(fp),#0
	jpr	ne,L110
	ld	r3,FP(#~L1+%42)
	exts	rr2
	ldl	rr6,FP(#~L1+%4e)
	multl	rq4,rr2
	ld	r3,FP(#~L1+%52)
	exts	rr2
	addl	rr6,rr2
	ldl	FP(#~L1+%4e),rr6
L110:
	inc	_stkseg+~L1+%44(fp),#%1
	jpr	L100
L103:
	cp	_stkseg+~L1+%4a(fp),#0
	jpr	ne,L111
	jpr	L101
L111:
	cp	_stkseg+~L1+%40(fp),#%2e
	jpr	ne,L112
	ld	r2,FP(#~L1+%46)
	inc	_stkseg+~L1+%46(fp),#%1
	cp	r2,#0
	jpr	ne,L112
	jpr	L100
L112:
	cp	_stkseg+~L1+%40(fp),#%65
	jpr	eq,L114
	cp	_stkseg+~L1+%40(fp),#%45
	jpr	ne,L113
L114:
	cp	_stkseg+~L1+%44(fp),#0
	jpr	eq,L113
	ld	r2,FP(#~L1+%48)
	inc	_stkseg+~L1+%48(fp),#%1
	cp	r2,#0
	jpr	ne,L113
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl4,FP(#~L1+%41)
	extsb	r4
	ldb	@rr2,rl4
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L115
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L116
L115:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L116:
	ld	FP(#~L1+%40),r2
	ldl	rr2,#__ctype+%1
	add	r3,_stkseg+~L1+%40(fp)
	ldb	rl2,@rr2
	extsb	r2
	bit	r2,#%2
	jpr	ne,L118
	cp	_stkseg+~L1+%40(fp),#%2b
	jpr	eq,L118
	cp	_stkseg+~L1+%40(fp),#%2d
	jpr	ne,L117
L118:
	jpr	L100
L117:
L113:
	jpr	L101
L100:
	ldl	rr2,rr8
	inc	r9,#%1
	ldb	rl4,FP(#~L1+%41)
	extsb	r4
	ldb	@rr2,rl4
	ldl	rr2,rr10
	add	r3,#%4
	dec	@rr2,#%1
	cp	@rr2,#0
	jpr	lt,L119
	ldl	rr2,@rr10
	inc	r3,#%1
	ldl	@rr10,rr2
	dec	r3,#%1
	ldb	rl2,@rr2
	xorb	rh2,rh2
	jpr	L120
L119:
	pushl	@SP,rr10
	callr	__filbuf
	inc	sp,#%4
L120:
	ld	FP(#~L1+%40),r2
	jpr	L102
L101:
	cp	_stkseg+~L2+%4(fp),#0
	jpr	eq,L121
	cp	_stkseg+~L1+%44(fp),#0
	jpr	eq,L121
	cp	_stkseg+~L1+%4a(fp),#0
	jpr	eq,L122
	clrb	@rr8
	ldl	rr2,FP
	add	r3,#~L1+%0
	pushl	@SP,rr2
	callr	_atof
	inc	sp,#%4
	ldl	FP(#~L1+%56),rr2
	ldl	FP(#~L1+%52),rr0
	cp	_stkseg+~L1+%4c(fp),#0
	jpr	eq,L123
	ldl	rr0,FP(#~L1+%52)
	ldl	rr2,FP(#~L1+%56)
	callr	negd
	ldl	_stkseg+~L1+%56(fp),rr2
	ldl	_stkseg+~L1+%52(fp),rr0
L123:
	cp	_stkseg+~L2+%a(fp),#%6c
	jpr	ne,L124
	ldl	rr2,FP(#~L2+%10)
	ldl	rr4,FP(#~L2+%10)
	ldl	rr4,@rr4
	inc	r5,#%4
	ldl	@rr2,rr4
	dec	r5,#%4
	ldl	rr2,@rr4
	ldl	rr4,_stkseg+~L1+%52(fp)
	ldl	rr6,_stkseg+~L1+%56(fp)
	ldm	@rr2,r4,#4
	jpr	L125
L124:
	ldl	rr0,FP(#~L1+%52)
	ldl	rr2,FP(#~L1+%56)
	callr	dtof
	ldl	rr2,rr2
	callr	ftod
	callr	dtof
	ldl	rr4,FP(#~L2+%10)
	ldl	rr6,FP(#~L2+%10)
	ldl	rr6,@rr6
	inc	r7,#%4
	ldl	@rr4,rr6
	dec	r7,#%4
	ldl	rr4,@rr6
	ldl	@rr4,rr2
L125:
	jpr	L126
L122:
	cp	_stkseg+~L1+%4c(fp),#0
	jpr	eq,L127
	ldl	rr2,FP(#~L1+%4e)
	cpl	rr2,#%80000000
	jpr	eq,L127
	ldl	rr2,FP(#~L1+%4e)
	com	r2
	com	r3
	addl	rr2,#1
	ldl	FP(#~L1+%4e),rr2
L127:
	cp	_stkseg+~L2+%a(fp),#%6c
	jpr	ne,L128
	ldl	rr2,FP(#~L2+%10)
	ldl	rr4,FP(#~L2+%10)
	ldl	rr4,@rr4
	inc	r5,#%4
	ldl	@rr2,rr4
	dec	r5,#%4
	ldl	rr2,@rr4
	ldl	rr4,FP(#~L1+%4e)
	ldl	@rr2,rr4
	jpr	L129
L128:
	cp	_stkseg+~L2+%a(fp),#%68
	jpr	ne,L130
	ldl	rr2,FP(#~L2+%10)
	ldl	rr4,FP(#~L2+%10)
	ldl	rr4,@rr4
	inc	r5,#%4
	ldl	@rr2,rr4
	dec	r5,#%4
	ldl	rr2,@rr4
	ld	r4,FP(#~L1+%50)
	ld	@rr2,r4
	jpr	L131
L130:
	ldl	rr2,FP(#~L2+%10)
	ldl	rr4,FP(#~L2+%10)
	ldl	rr4,@rr4
	inc	r5,#%4
	ldl	@rr2,rr4
	dec	r5,#%4
	ldl	rr2,@rr4
	ld	r4,FP(#~L1+%50)
	ld	@rr2,r4
L131:
L129:
L126:
L121:
	pushl	@SP,rr10
	push	@SP,_stkseg+~L1+%40(fp)
	callr	_ungetc
	inc	sp,#%6
	cp	r2,#%ffff
	jpr	ne,L132
	ldl	rr2,FP(#~L2+%10)
	subl	rr4,rr4
	ldl	@rr2,rr4
L132:
	ld	r2,FP(#~L1+%44)
	jpr	L78
L78:
	add	sp,#~L2-%c
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 2
	~L2 := 104
	}
	.psec	data
	.data
	.psec
	.code
_string:
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	sub	sp,#~L2-%c
	ld	fp,sp
	ld	@SP,#~L2-4
	ld	r11,FP(#~L2+%4)
	ld	r10,FP(#~L2+%6)
	ld	r9,FP(#~L2+%8)
L134:
L135:
L137:
	jpr	L136
L138:
	jpr	L135
L136:
L139:
	jpr	L133
L140:
L141:
	jpr	L133
L133:
	add	sp,#~L2-%c
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 2
	~L2 := 22
	}
	.psec	data
	.data
	.psec
	.code
_setup:
	{
	dec	sp,#12
	ldm	@SP,r8,#6
	sub	sp,#~L2-%c
	ld	fp,sp
	ld	@SP,#~L2-4
L143:
L144:
L145:
	jpr	L142
L147:
	jpr	L149
L148:
L149:
	jpr	L145
L146:
	jpr	L142
L142:
	add	sp,#~L2-%c
	ldm	r8,@SP,#6
	inc	sp,#12
	ret
	~L1 := 2
	~L2 := 22
	}
	.psec	data
	.data
