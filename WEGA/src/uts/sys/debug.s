.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_debugwstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%64
.byte	%65
.byte	%62
.byte	%75
.byte	%67
.byte	%2e
.byte	%63
.byte	%9
.byte	%9
.byte	%52
.byte	%65
.byte	%76
.byte	%20
.byte	%3a
.byte	%20
.byte	%34
.byte	%2e
.byte	%31
.byte	%20
.byte	%9
.byte	%30
.byte	%39
.byte	%2f
.byte	%31
.byte	%37
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%30
.byte	%32
.byte	%3a
.byte	%35
.byte	%36
.byte	%3a
.byte	%30
.byte	%37
.byte	%0
.even
_kdebug::
.word	%0
.comm	0,	_buffer1;
.comm	0,	_buffer2;
.comm	0,	_buffer3;
.comm	0,	_buffer4;
.comm	0,	_buffer5;
.comm	0,	_canonb;
.comm	4,	_rootdir;
.comm	4,	_runq;
.comm	2,	_lbolt;
.comm	4,	_lifetime;
.comm	4,	_time;
.comm	2,	_nblkdev;
.comm	2,	_nchrdev;
.comm	2,	_mpid;
.comm	1,	_runin;
.comm	1,	_runout;
.comm	1,	_runrun;
.comm	1,	_curpri;
.comm	2,	_maxmem;
.comm	2,	_umemory;
.comm	4,	_swplo;
.comm	2,	_nswap;
.comm	2,	_updlock;
.comm	4,	_rablock;
.comm	128,	_msgbuf;
.comm	2,	_rootdev;
.comm	2,	_swapdev;
.comm	2,	_pipedev;
.comm	4,	_serial;
.comm	2,	_numterm;
.comm	2,	_manboot;
.comm	2,	_blkacty;
.comm	2,	_cputype;
.comm	112,	_sysinfo;
.comm	6,	_syswait;
.comm	2,	_dk_busy;
.comm	128,	_dk_time;
.comm	12,	_dk_numb;
.comm	12,	_dk_wds;
.comm	4,	_tk_nin;
.comm	4,	_tk_nout;
.comm	200,	_callout;
.comm	50,	_breakpt;
_kdbstr:
.=.+6
_hzif:
.byte	%30
.byte	%31
.byte	%32
.byte	%33
.byte	%34
.byte	%35
.byte	%36
.byte	%37
.byte	%38
.byte	%39
.byte	%41
.byte	%42
.byte	%43
.byte	%44
.byte	%45
.byte	%46
.even
_foo:
.word	0
_format:
.=.+16
.byte	0
.byte	0
.even
_kval:
.word	0
/*~~gethex:*/

.psec
.code

_gethex::
{
	dec	fp,#~L2
	subl	rr2,rr2
	jpr	L20000
L96:
	cpb	|_stkseg+~L1+1|(fp),#97
	jpr	ge,L20005
L98:
	cpb	|_stkseg+~L1+1|(fp),#48
	jpr	lt,L100
	cpb	|_stkseg+~L1+1|(fp),#57
	jpr	gt,L100
	ldb	rl2,|_stkseg+~L1+1|(fp)
	subb	rl2,#48
L20007:
	ldb	|_stkseg+~L1+1|(fp),rl2
	ldl	rr2,|_stkseg+~L1+2|(fp)
	slal	rr2,#4
	ldb	rl5,|_stkseg+~L1+1|(fp)
	extsb	r5
	exts	rr4
	addl	rr2,rr4
L20000:
	ldl	|_stkseg+~L1+2|(fp),rr2
	callr	_getchar
	ldb	|_stkseg+~L1+1|(fp),rl2
	testb	rl2
	jpr	eq,L10000
	cpb	rl2,#65
	jpr	lt,L96
	cpb	rl2,#70
	jpr	gt,L96
	extsb	r2
	sub	r2,#55
	jpr	L20007
L20005:
	cpb	|_stkseg+~L1+1|(fp),#102
	jpr	gt,L98
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sub	r2,#87
	jpr	L20007
L100:
	ldl	rr2,|_stkseg+~L1+2|(fp)
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _gethex */

.psec data
.data

/*~~zpr:*/

.psec
.code

_zpr::
{
	dec	fp,#~L2
	ld	|_stkseg+~L1+0|(fp),r7
	ldl	rr2,#_hzif
	ldl	|_stkseg+~L1+4|(fp),rr2
	ld	|_stkseg+~L1+2|(fp),r7
	ld	r6,r7
	sra	r6,#12
	and	r6,#15
	subb	rh6,rh6
	ldl	rr2,|_stkseg+~L1+4|(fp)
	ldb	rl4,rr2(r6)
	ldb	_kdbstr,rl4
	ld	r6,|_stkseg+~L1+2|(fp)
	ldb	rl6,rh6
	extsb	r6
	and	r6,#15
	subb	rh6,rh6
	ldb	rl4,rr2(r6)
	ldb	_kdbstr+1,rl4
	ld	r6,|_stkseg+~L1+2|(fp)
	sra	r6,#4
	and	r6,#15
	subb	rh6,rh6
	ldb	rl4,rr2(r6)
	ldb	_kdbstr+2,rl4
	ld	r6,|_stkseg+~L1+2|(fp)
	and	r6,#15
	subb	rh6,rh6
	ldb	rl4,rr2(r6)
	ldb	_kdbstr+3,rl4
	clrb	_kdbstr+4
	ldl	rr2,#_kdbstr
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _zpr */

.psec data
.data

L115:
.byte	%4b,%44,%42,%3a,%20,%55,%6e,%73
.byte	%65,%74,%20,%62,%72,%65,%61,%6b
.byte	%70,%6f,%69,%6e,%74,%a,%0
.even
L117:
.byte	%4b,%44,%42,%3a,%20,%40,%25,%58
.byte	%20,%42,%72,%65,%61,%6b,%70,%6f
.byte	%69,%6e,%74,%a,%0
.even
L122:
.byte	%4b,%44,%42,%3e,%7,%0
.even
L127:
.byte	%75,%6d,%70,%20,%63,%61,%6c,%6c
.byte	%20,%74,%6f,%20,%40,%0
.even
L132:
.byte	%72,%65,%61,%6b,%70,%6f,%69,%6e
.byte	%74,%20,%40,%0
.even
L137:
.byte	%4b,%44,%42,%3a,%20,%4e,%6f,%20
.byte	%6d,%6f,%72,%65,%20,%62,%72,%65
.byte	%61,%6b,%20,%73,%6c,%6f,%74,%73
.byte	%a,%0
.even
L140:
.byte	%20,%46,%41,%55,%4c,%54,%20,%0
.even
L143:
.byte	%73,%65,%72,%20,%73,%70,%61,%63
.byte	%65,%20,%64,%0
.even
L147:
.byte	%68,%79,%73,%69,%63,%61,%6c,%20
.byte	%73,%70,%61,%63,%65,%20,%64,%0
.even
L150:
.byte	%69,%73,%70,%6c,%61,%79,%20,%40
.byte	%0
.even
L151:
.byte	%66,%6f,%72,%20,%0
.even
L152:
.byte	%77,%6f,%72,%64,%73,%a,%0
.even
L158:
.byte	%a,%0
.even
L159:
.byte	%25,%73,%0
.even
L160:
.byte	%25,%73,%3a,%20,%0
.even
L162:
.byte	%20,%46,%41,%55,%4c,%54,%20,%0
.even
L165:
.byte	%20,%46,%41,%55,%4c,%54,%20,%0
.even
L167:
.byte	%25,%73,%20,%0
.even
L169:
.byte	%20,%20,%7c,%25,%73,%7c,%0
.even
L172:
.byte	%6f,%64,%69,%66,%79,%20,%0
.even
L177:
.byte	%65,%67,%69,%73,%74,%65,%72,%20
.byte	%0
.even
L178:
.byte	%28,%25,%73,%29,%20,%77,%69,%74
.byte	%68,%20,%0
.even
L179:
.byte	%4f,%4b,%20,%0
.even
L186:
.byte	%65,%6d,%6f,%72,%79,%20,%40,%0
.even
L187:
.byte	%28,%25,%73,%29,%20,%77,%69,%74
.byte	%68,%20,%0
.even
L188:
.byte	%4f,%4b,%20,%0
.even
L194:
.byte	%20,%46,%41,%55,%4c,%54,%20,%0
.even
.even
L196:
.word	77
.word	82
.word	109
.word	114
.even
L200:
.byte	%65,%67,%69,%73,%74,%65,%72,%73
.byte	%a,%0
.even
L205:
.byte	%72,%25,%64,%3a,%25,%73,%20,%0
.even
L206:
.byte	%a,%66,%63,%77,%3d,%25,%73,%20
.byte	%0
.even
L207:
.byte	%70,%63,%73,%65,%67,%3d,%25,%73
.byte	%20,%0
.even
L208:
.byte	%70,%63,%6f,%66,%66,%3d,%25,%73
.byte	%0
.even
L211:
.byte	%69,%6e,%67,%6c,%65,%20,%73,%74
.byte	%65,%70,%a,%0
.even
L215:
.byte	%61,%70,%65,%20,%63,%72,%61,%73
.byte	%68,%20,%64,%75,%6d,%70,%a,%0
.even
L216:
.byte	%77,%72,%69,%74,%61,%62,%6c,%65
.byte	%20,%74,%61,%70,%65,%20,%6c,%6f
.byte	%61,%64,%65,%64,%20,%69,%6e,%20
.byte	%64,%72,%69,%76,%65,%20,%30,%3f
.byte	%20,%0
.even
L224:
.byte	%6f,%6e,%74,%69,%6e,%75,%65,%a
.byte	%0
.even
L227:
.byte	%78,%69,%74,%a,%0
.even
.even
/*~~debug:*/

.psec
.code

_debug::
{
	.psec	data
	.data
	.even
L197:
	.long	L185
	.long	L176
	.long	L185
	.long	L176
	.even
L228:
	.long	L131
	.long	L223
	.long	L149
	.long	L226
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L126
	.long	L128
	.long	L128
	.long	L171
	.long	L128
	.long	L128
	.long	L146
	.long	L128
	.long	L199
	.long	L210
	.long	L214
	.long	L142
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L131
	.long	L223
	.long	L149
	.long	L226
	.long	L128
	.long	L128
	.long	L128
	.long	L128
	.long	L126
	.long	L128
	.long	L128
	.long	L171
	.long	L128
	.long	L128
	.long	L146
	.long	L128
	.long	L199
	.long	L210
	.long	L214
	.long	L142
	.psec
	.code
	sub	fp,#~L2
	ldl	|_stkseg+~L1+0|(fp),rr6
	test	_kdebug
	jpr	eq,L10006
	callr	_dvi
	ld	|_stkseg+~L1+12|(fp),r2
	ldl	rr2,|_stkseg+~L1|(fp)
	ld	r4,rr2(#32)
	and	r4,#255
	ld	|_stkseg+~L1+22|(fp),r4
	ld	|_stkseg+~L1+20|(fp),#5
	cp	|_stkseg+~L1+22|(fp),#2
	jpr	ne,L120
	clr	|_stkseg+~L1+20|(fp)
L20009:
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ld	r2,_breakpt(r3)
	test	r2
	jpr	eq,L109
	ld	r5,|_stkseg+~L1+20|(fp)
	mult	rr4,#10
	ld	r2,_breakpt+2(r5)
	ldl	rr4,|_stkseg+~L1|(fp)
	ld	r3,rr4(#36)
	and	r3,#16128
	cp	r2,r3
	jpr	ne,L109
	ld	r7,|_stkseg+~L1+20|(fp)
	mult	rr6,#10
	ld	r2,_breakpt+4(r7)
	ld	r3,rr4(#38)
	dec	r3,#2
	cp	r2,r3
	jpr	eq,L110
L109:
	inc	|_stkseg+~L1+20|(fp),#1
	cp	|_stkseg+~L1+20|(fp),#5
	jpr	lt,L20009
L110:
	cp	|_stkseg+~L1+20|(fp),#5
	jpr	lt,L113
	ldl	rr6,#L115
L20020:
	callr	_printf
	jpr	L120
L113:
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,#_breakpt
	add	r5,r3
	clr	@rr4
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,_breakpt+2(r3)
	ldl	|_stkseg+~L1+8|(fp),rr4
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,_breakpt+2(r3)
	ldl	rr6,#L117
	callr	_printf
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_getsd
	ldb	rl2,|_stkseg+~L1+39|(fp)
	ldb	|_stkseg+~L1+41|(fp),rl2
	ldb	rl3,|_stkseg+~L1+39|(fp)
	andb	rl3,#254
	ldb	|_stkseg+~L1+39|(fp),rl3
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_loadsd
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ld	r2,_breakpt+6(r3)
	ldl	rr4,|_stkseg+~L1+8|(fp)
	ld	@rr4,r2
	ldb	rl3,|_stkseg+~L1+41|(fp)
	ldb	|_stkseg+~L1+39|(fp),rl3
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_loadsd
	ldl	rr2,|_stkseg+~L1|(fp)
	add	r3,#38
	dec	@rr2,#2
L120:
	ldl	rr6,#L122
	callr	_printf
	callr	_getchar
	ld	|_stkseg+~L1+18|(fp),r2
	sub	r2,#66
	cp	r2,#51
	jpr	ugt,L128
	rl	r2,#2
	ldl	rr4,L228(r2)
	jp	@rr4
L126:
	ldl	rr6,#L127
	callr	_printf
	callr	_gethex
	and	r2,#32512
	ldl	|_stkseg+~L1+32|(fp),rr2
	call	@rr2
L128:
	ldk	r7,#10
	callr	_putchar
	jpr	L120
L131:
	ldl	rr6,#L132
	callr	_printf
	callr	_gethex
	and	r2,#16128
	ldl	|_stkseg+~L1+8|(fp),rr2
	clr	|_stkseg+~L1+20|(fp)
L135:
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ld	r2,_breakpt(r3)
	test	r2
	jpr	eq,L134
	inc	|_stkseg+~L1+20|(fp),#1
	cp	|_stkseg+~L1+20|(fp),#5
	jpr	lt,L135
L134:
	cp	|_stkseg+~L1+20|(fp),#5
	jpr	lt,L136
	ldl	rr6,#L137
	jpr	L20020
L136:
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,#_breakpt
	add	r5,r3
	ld	@rr4,#1
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,#_breakpt+2
	add	r5,r3
	ldl	rr2,|_stkseg+~L1+8|(fp)
	ldl	@rr4,rr2
	ld	r3,|_stkseg+~L1+20|(fp)
	mult	rr2,#10
	ldl	rr4,#_breakpt+6
	add	r5,r3
	ldl	rr2,|_stkseg+~L1+8|(fp)
	ld	r6,@rr2
	ld	@rr4,r6
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_getsd
	ldb	rl2,|_stkseg+~L1+39|(fp)
	ldb	|_stkseg+~L1+41|(fp),rl2
	ldb	rl3,|_stkseg+~L1+39|(fp)
	andb	rl3,#254
	ldb	|_stkseg+~L1+39|(fp),rl3
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_loadsd
	ldl	rr6,|_stkseg+~L1+8|(fp)
	ld	r5,#127
	callr	_skbyte
	test	r2
	jpr	ne,L10009
	ldl	rr6,|_stkseg+~L1+8|(fp)
	inc	r7,#1
	ldk	r5,#2
	callr	_skbyte
	test	r2
	jpr	eq,L139
L10009:
	ldl	rr6,#L140
L20027:
	callr	_printf
	jpr	L128
L139:
	ldb	rl2,|_stkseg+~L1+41|(fp)
	ldb	|_stkseg+~L1+39|(fp),rl2
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_loadsd
	jpr	L128
L142:
	ldl	rr6,#L143
	callr	_printf
	ldl	rr2,#_fubyte
	jpr	L20010
L146:
	ldl	rr6,#L147
	callr	_printf
	ldl	rr2,#_fpbyte
	jpr	L20010
L149:
	ldl	rr2,#_fkbyte
L20010:
	ldl	|_stkseg+~L1+32|(fp),rr2
	ldl	rr6,#L150
	callr	_printf
	callr	_gethex
	and	r2,#32512
	ldl	|_stkseg+~L1+4|(fp),rr2
	ldl	rr6,#L151
	callr	_printf
	callr	_gethex
	ldl	|_stkseg+~L1+28|(fp),rr2
	ldl	rr6,#L152
	callr	_printf
	clr	|_stkseg+~L1+16|(fp)
	ldl	rr2,|_stkseg+~L1+28|(fp)
	addl	rr2,#7
	srll	rr2,#3
	slal	rr2,#3
	ldl	|_stkseg+~L1+28|(fp),rr2
	subl	rr2,rr2
L20012:
	ldl	|_stkseg+~L1+24|(fp),rr2
	cpl	rr2,|_stkseg+~L1+28|(fp)
	jpr	uge,L128
	test	|_stkseg+~L1+16|(fp)
	jpr	ne,L156
	testl	rr2
	jpr	eq,L157
	ldl	rr6,#L158
	callr	_printf
L157:
	ld	r7,|_stkseg+~L1+4|(fp)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L159
	ldl	rr4,rr2
	callr	_printf
	ldl	rr2,#_stkseg+~L1+4
	add	r3,fp
	ld	r7,rr2(#2)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L160
	ldl	rr4,rr2
	callr	_printf
L156:
	ldl	rr2,|_stkseg+~L1+32|(fp)
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr4,#_format
	add	r5,|_stkseg+~L1+16|(fp)
	ldl	rr6,|_stkseg+~L1+4|(fp)
	call	@rr2
	test	r2
	jpr	eq,L161
	ldl	rr6,#L162
	jpr	L20027
L161:
	ld	r2,|_stkseg+~L1+16|(fp)
	ldb	rl3,_format(r2)
	subb	rh3,rh3
	ldb	rh3,rl3
	clrb	rl3
	ld	_kval,r3
	ldb	rl3,_format(r2)
	cpb	rl3,#32
	jpr	ult,L10010
	ldb	rl3,_format(r2)
	cpb	rl3,#126
	jpr	ule,L163
L10010:
	ld	r2,|_stkseg+~L1+16|(fp)
	ldl	rr4,#_format
	add	r5,r2
	ldb	@rr4,#32
L163:
	inc	|_stkseg+~L1+16|(fp),#1
	inc	|_stkseg+~L1+6|(fp),#1
	ldl	rr2,|_stkseg+~L1+32|(fp)
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr4,#_format
	add	r5,|_stkseg+~L1+16|(fp)
	ldl	rr6,|_stkseg+~L1+4|(fp)
	call	@rr2
	test	r2
	jpr	eq,L164
	ldl	rr6,#L165
	jpr	L20027
L164:
	ld	r2,|_stkseg+~L1+16|(fp)
	ldb	rl3,_format(r2)
	subb	rh3,rh3
	or	r3,_kval
	ld	_kval,r3
	ld	r2,|_stkseg+~L1+16|(fp)
	ldb	rl3,_format(r2)
	cpb	rl3,#32
	jpr	ult,L10011
	ldb	rl3,_format(r2)
	cpb	rl3,#126
	jpr	ule,L166
L10011:
	ld	r2,|_stkseg+~L1+16|(fp)
	ldl	rr4,#_format
	add	r5,r2
	ldb	@rr4,#32
L166:
	inc	|_stkseg+~L1+16|(fp),#1
	inc	|_stkseg+~L1+6|(fp),#1
	ld	r7,_kval
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L167
	ldl	rr4,rr2
	callr	_printf
	ld	r3,|_stkseg+~L1+16|(fp)
	exts	rr2
	div	rr2,#16
	test	r2
	jpr	ne,L153
	ldl	rr6,#L169
	ldl	rr4,#_format
	callr	_printf
	clr	|_stkseg+~L1+16|(fp)
L153:
	ldl	rr2,|_stkseg+~L1+24|(fp)
	addl	rr2,#1
	jpr	L20012
L171:
	ldl	rr6,#L172
	callr	_printf
	callr	_getchar
	ldl	rr4,#L196
	ld	r6,#4
	cpir	r2,@rr4,r6,eq
	jpr	ne,L128
	sll	r6,#2
	neg	r6
	ldl	rr4,L197+12(r6)
	jp	@rr4
L176:
	ldl	rr6,#L177
	callr	_printf
	callr	_gethex
	ldl	|_stkseg+~L1+28|(fp),rr2
	slal	rr2,#1
	ldl	rr4,|_stkseg+~L1|(fp)
	ld	r7,rr4(r3)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L178
	ldl	rr4,rr2
	callr	_printf
	callr	_gethex
	ld	|_stkseg+~L1+14|(fp),r3
	ldl	rr6,#L179
	callr	_printf
	callr	_getchar
	cp	r2,#89
	jpr	eq,L183
	cp	r2,#121
	jpr	ne,L128
L183:
	ldl	rr2,|_stkseg+~L1+28|(fp)
	slal	rr2,#1
	ldl	rr4,|_stkseg+~L1|(fp)
	add	r5,r3
	ld	r2,|_stkseg+~L1+14|(fp)
	ld	@rr4,r2
	jpr	L128
L185:
	ldl	rr6,#L186
	callr	_printf
	callr	_gethex
	and	r2,#32512
	ldl	|_stkseg+~L1+8|(fp),rr2
	ld	r7,@rr2
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L187
	ldl	rr4,rr2
	callr	_printf
	callr	_gethex
	ld	|_stkseg+~L1+14|(fp),r3
	ldl	rr6,#L188
	callr	_printf
	callr	_getchar
	cp	r2,#89
	jpr	eq,L192
	cp	r2,#121
	jpr	ne,L128
L192:
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_getsd
	ldb	rl2,|_stkseg+~L1+39|(fp)
	ldb	|_stkseg+~L1+41|(fp),rl2
	ldb	rl3,|_stkseg+~L1+39|(fp)
	andb	rl3,#254
	ldb	|_stkseg+~L1+39|(fp),rl3
	ldl	rr4,#_stkseg+~L1+36
	add	r5,fp
	ldb	rl6,|_stkseg+~L1+8|(fp)
	ldb	rh6,#0
	ld	r7,_mmut
	callr	_loadsd
	ldl	rr6,|_stkseg+~L1+8|(fp)
	ldb	rl5,|_stkseg+~L1+14|(fp)
	ldb	rh5,#0
	callr	_skbyte
	test	r2
	jpr	ne,L10012
	ldl	rr6,|_stkseg+~L1+8|(fp)
	inc	r7,#1
	ldb	rl5,|_stkseg+~L1+14|(fp)
	ldb	rh5,#0
	callr	_skbyte
	test	r2
	jpr	eq,L139
L10012:
	ldl	rr6,#L194
	jpr	L20027
L199:
	ldl	rr6,#L200
	callr	_printf
	subl	rr2,rr2
L20013:
	ldl	|_stkseg+~L1+24|(fp),rr2
	cpl	rr2,#16
	jpr	ult,L20015
	ldl	rr2,|_stkseg+~L1|(fp)
	ld	r7,rr2(#34)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L206
	ldl	rr4,rr2
	callr	_printf
	ldl	rr2,|_stkseg+~L1|(fp)
	ld	r7,rr2(#36)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L207
	ldl	rr4,rr2
	callr	_printf
	ldl	rr2,|_stkseg+~L1|(fp)
	ld	r7,rr2(#38)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L208
	ldl	rr4,rr2
	jpr	L20027
L20015:
	ldl	rr2,|_stkseg+~L1+24|(fp)
	ldk	r2,#0
	and	r3,#7
	testl	rr2
	jpr	ne,L204
	ldk	r7,#10
	callr	_putchar
L204:
	ldl	rr2,|_stkseg+~L1+24|(fp)
	slal	rr2,#1
	ldl	rr4,|_stkseg+~L1|(fp)
	ld	r7,rr4(r3)
	callr	_zpr
	ldl	|_stkseg+~L1+42|(fp),rr2
	ldl	rr6,#L205
	ldl	rr4,|_stkseg+~L1+24|(fp)
	callr	_printf
	ldl	rr2,|_stkseg+~L1+24|(fp)
	addl	rr2,#1
	jpr	L20013
L210:
	ldl	rr6,#L211
	callr	_printf
	ldl	rr2,|_stkseg+~L1|(fp)
	add	r3,#34
	set	@rr2,#0
	jpr	L20018
L214:
	ldl	rr6,#L215
	callr	_printf
	ldl	rr6,#L216
	callr	_printf
	callr	_getchar
	cp	r2,#89
	jpr	eq,L220
	cp	r2,#121
	jpr	ne,L128
L220:
	ldl	rr6,|_stkseg+~L1|(fp)
	callr	_dump
	jpr	L128
L223:
	ldl	rr6,#L224
	jpr	L20019
L226:
	ldl	rr6,#L227
L20019:
	callr	_printf
L20018:
	ld	r7,|_stkseg+~L1+12|(fp)
	callr	_rvi
	ldk	r2,#1
L10006:
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 46
	}	/* _debug */

.psec data
.data

