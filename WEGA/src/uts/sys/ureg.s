.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_uregwstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%75
.byte	%72
.byte	%65
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
.byte	%38
.byte	%2f
.byte	%32
.byte	%37
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%31
.byte	%32
.byte	%3a
.byte	%30
.byte	%31
.byte	%3a
.byte	%30
.byte	%35
.byte	%0
.byte	0
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
L98:
.byte	%73,%75,%72,%65,%67,%3a,%20,%75
.byte	%73,%65,%72,%20,%69,%73,%20,%73
.byte	%65,%67,%6d,%65,%6e,%74,%65,%64
.byte	%a,%0
.even
/*~~sureg:*/

.psec
.code

_sureg::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	testb	_u+544
	jpr	ne,L10003
	ld	r7,#65473
	callr	_inb
	bit	r2,#2
	jpr	eq,L96
L10003:
	ldl	rr6,#L98
	callr	_panic
L96:
	ldl	rr2,_u+32
	ld	r4,rr2(#18)
	inc	r4,#10
	ld	_u+802,r4
	ldk	r2,#0
	ldb	rl2,_u+804
	add	r2,r4
	inc	r2,#1
	ldk	r3,#0
	ldb	rl3,_u+1060
	sub	r2,r3
	ld	_u+1058,r2
	ldl	rr2,_u+32
	ldl	rr4,rr2(#26)
	ldl	rr10,rr4
	testl	rr4
	jpr	eq,L99
	ld	r4,rr10(#2)
	ld	_u+546,r4
	ldl	rr12,#_u+546
	jpr	L100
L99:
	ldl	rr12,#_u+802
L100:
	ld	r7,_mmut
	ld	r6,#63
	ldl	rr4,rr12
	callr	_loadsd
	ld	r7,_mmud
	ld	r6,#63
	ldl	rr4,#_u+802
	callr	_loadsd
	ld	r7,_mmus
	ld	r6,#63
	ldl	rr4,#_u+1058
	callr	_loadsd
	ldk	r2,#0
	ldb	rl2,_u+804
	inc	r2,#1
	ld	_u+102,r2
	ld	r7,#65489
	ld	r6,r2
	callr	_outb
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _sureg */

.psec data
.data

/*~~estabur:*/

.psec
.code

_estabur::
{
	dec	fp,#~L2
	ldm	_stkseg+10(fp),r11,#3
	ld	|_stkseg+~L1+0|(fp),r4
	ld	r11,r5
	ld	r12,r6
	ld	r13,r7
	cp	r7,#256
	jpr	ugt,L10007
	ld	r5,r6
	add	r5,r11
	cp	r5,#255
	jpr	ugt,L10007
	ld	r5,r7
	add	r5,r12
	add	r5,r11
	inc	r5,#10
	cp	r5,_maxmem
	jpr	ule,L105
L10007:
	ldb	_u+21,#12
	ld	r2,#-1
	jpr	L10004
L105:
	test	r13
	jpr	ne,L10009
	sub	r2,r2
	jpr	L10010
L10009:
	ld	r2,r13
	dec	r2,#1
L10010:
	ldb	_u+548,rl2
	test	r12
	jpr	ne,L10011
	sub	r2,r2
	jpr	L10012
L10011:
	ld	r2,r12
	dec	r2,#1
L10012:
	ldb	_u+804,rl2
	test	r11
	jpr	ne,L10013
	sub	r2,r2
	jpr	L10014
L10013:
	ld	r2,r11
	neg	r2
	add	r2,#256
L10014:
	ldb	_u+1060,rl2
	test	r13
	jpr	ne,L10015
	ld	r3,#20
	jpr	L10016
L10015:
	ld	r3,|_stkseg+~L1|(fp)
L10016:
	ldb	_u+549,rl3
	test	r12
	jpr	ne,L10017
	ld	r2,#20
	jpr	L10018
L10017:
	sub	r2,r2
L10018:
	ldb	_u+805,rl2
	test	r11
	jpr	ne,L10019
	ld	r2,#20
	jpr	L10020
L10019:
	ld	r2,#32
L10020:
	ldb	_u+1061,rl2
	callr	_sureg
	sub	r2,r2
L10004:
	ldm	r11,_stkseg+10(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _estabur */

.psec data
.data

/*~~sestabur:*/

.psec
.code

_sestabur::
{
	sub	fp,#~L2
	ldm	_stkseg+12(fp),r11,#2
	ld	|_stkseg+~L1+4|(fp),r6
	ld	r11,r4
	ld	r12,r5
	ld	r1,r7
	cp	r7,#256
	jpr	ule,L108
	ldb	_u+21,#12
	jpr	L20000
L20003:
	ldb	_u+21,#8
L20000:
	ld	r2,#-1
	jpr	L10021
L108:
	ldb	rl2,|_stkseg+~L1+5|(fp)
	extsb	r2
	and	r2,#127
	ldb	|_stkseg+~L1+9|(fp),rl2
	cp	r12,#128
	jpr	ugt,L20003
	ldb	rl2,rl2
	extsb	r2
	cp	r2,_u+1390
	jpr	ne,L110
	test	r1
	jpr	ne,L10024
	sub	r2,r2
	jpr	L10025
L10024:
	ld	r2,r1
	neg	r2
	add	r2,#256
L10025:
	ldb	_u+1060,rl2
	test	r1
	jpr	ne,L10026
	ld	r3,#20
	jpr	L10027
L10026:
	ld	r3,#32
L10027:
	ldb	_u+1061,rl3
	jpr	L112
L110:
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,#_u+552
	add	r5,r2
	test	r1
	jpr	ne,L10028
	sub	r2,r2
	jpr	L10029
L10028:
	ld	r2,r1
	dec	r2,#1
L10029:
	ldb	@rr4,rl2
	ldb	rl2,|_stkseg+~L1+5|(fp)
	extsb	r2
	bit	r2,#7
	jpr	eq,L111
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,#_u+553
	add	r5,r2
	test	r1
	jpr	ne,L10030
L20001:
	ld	r2,#20
	jpr	L10033
L10030:
	ld	r2,r11
	jpr	L10033
L111:
	ld	r2,r12
	sla	r2,#2
	ldl	rr4,#_u+553
	add	r5,r2
	test	r1
	jpr	eq,L20001
	sub	r2,r2
L10033:
	ldb	@rr4,rl2
L112:
	sub	r2,r2
L10021:
	ldm	r11,_stkseg+12(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _sestabur */

.psec data
.data

/*~~segureg:*/

.psec
.code

_segureg::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r8,#6
	ldl	rr2,_u+32
	ld	r4,rr2(#18)
	inc	r4,#10
	ld	r12,r4
	ldl	rr2,_u+32
	ldl	rr4,rr2(#26)
	ldl	rr8,rr4
	testl	rr4
	jpr	eq,L115
	ld	r4,rr8(#2)
	ld	r13,r4
	jpr	L116
L115:
	ldk	r13,#0
L116:
	clrb	|_stkseg+~L1+1|(fp)
	jpr	L119
L20006:
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	ldb	rl4,_u+1062(r3)
	extsb	r4
	and	r4,#127
	ld	r11,r4
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	sla	r3,#2
	ldb	rl4,_u+553(r3)
	subb	rh4,rh4
	and	r4,#20
	ldb	|_stkseg+~L1+3|(fp),rl4
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	ldb	rl4,_u+1062(r3)
	extsb	r4
	bit	r4,#7
	jpr	eq,L120
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	sla	r3,#2
	ldl	rr4,#_u+550
	add	r5,r3
	ld	@rr4,r13
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	sla	r3,#2
	ldb	rl4,_u+552(r3)
	testb	rl4
	jpr	ne,L10037
	testb	|_stkseg+~L1+3|(fp)
	jpr	ne,L122
L10037:
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	sla	r3,#2
	ldb	rl4,_u+552(r3)
	subb	rh4,rh4
	inc	r4,#1
	add	r13,r4
	jpr	L122
L120:
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldl	rr4,#_u+550
	add	r5,r2
	ld	@rr4,r12
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldb	rl3,_u+552(r2)
	testb	rl3
	jpr	ne,L10038
	testb	|_stkseg+~L1+3|(fp)
	jpr	ne,L122
L10038:
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldb	rl3,_u+552(r2)
	subb	rh3,rh3
	inc	r3,#1
	add	r12,r3
L122:
	cp	r11,#64
	jpr	uge,L124
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldl	rr4,#_u+550
	add	r5,r2
	ld	r6,r11
	ld	r7,_mmud
	jpr	L20004
L124:
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldl	rr4,#_u+550
	add	r5,r2
	ld	r6,r11
	ld	r7,_mmus
L20004:
	callr	_loadsd
	incb	|_stkseg+~L1+1|(fp),#1
L119:
	ldb	rl2,|_stkseg+~L1+1|(fp)
	cpb	rl2,_u+1190
	jpr	lt,L20006
	ldk	r2,#0
	ldb	rl2,_u+1060
	ld	r3,r12
	sub	r3,r2
	ld	_u+1058,r3
	cp	_u+1390,#64
	jpr	ge,L10039
	ld	r7,_mmud
	jpr	L10040
L10039:
	ld	r7,_mmus
L10040:
	ld	r6,#127
	ldl	rr4,#_u+1058
	callr	_loadsd
	ldm	r8,_stkseg+4(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _segureg */

.psec data
.data

/*~~ldsdr:*/

.psec
.code

_ldsdr::
{
	dec	fp,#~L2
	ldm	_stkseg+8(fp),r11,#3
	ldm	|_stkseg+~L1+0|(fp),r4,#4
	ld	r2,|_stkseg+~L1+6|(fp)
	and	r2,#127
	ld	|_stkseg+~L1+6|(fp),r2
	cp	r2,_u+1390
	jpr	eq,L128
	ldk	r11,#0
	jpr	L131
L20008:
	ldb	rl2,_u+1062(r11)
	extsb	r2
	and	r2,#127
	cp	r2,|_stkseg+~L1+6|(fp)
	jpr	eq,L130
	inc	r11,#1
L131:
	ldb	rl2,_u+1190
	extsb	r2
	cp	r11,r2
	jpr	lt,L20008
L130:
	ldb	rl2,_u+1190
	extsb	r2
	cp	r11,r2
	jpr	ne,L133
	ldb	_u+21,#36
	jpr	L10041
L133:
	ld	r2,r11
	sla	r2,#2
	ldl	rr4,#_u+550
	add	r5,r2
	ldl	rr12,rr4
	jpr	L134
L128:
	ldl	rr12,#_u+1058
L134:
	cp	|_stkseg+~L1+6|(fp),#64
	jpr	ge,L10044
	ld	r7,_mmud
	jpr	L10045
L10044:
	ld	r7,_mmus
L10045:
	ld	r6,|_stkseg+~L1+6|(fp)
	ldl	rr4,rr12
	callr	_getsd
	cp	|_stkseg+~L1+4|(fp),#65535
	jpr	eq,L136
	ld	r2,|_stkseg+~L1+4|(fp)
	ld	@rr12,r2
L136:
	cp	|_stkseg+~L1+2|(fp),#65535
	jpr	eq,L137
	ldl	rr2,rr12
	inc	r3,#2
	ld	r4,|_stkseg+~L1+6|(fp)
	cp	r4,_u+1390
	jpr	ne,L10046
	ld	r5,#256
	sub	r5,|_stkseg+~L1+2|(fp)
	jpr	L10047
L10046:
	ld	r5,|_stkseg+~L1+2|(fp)
	dec	r5,#1
L10047:
	ldb	@rr2,rl5
L137:
	cp	|_stkseg+~L1|(fp),#65535
	jpr	eq,L138
	ldl	rr2,rr12
	inc	r3,#3
	ldb	rl4,|_stkseg+~L1+1|(fp)
	ldb	@rr2,rl4
L138:
	cp	|_stkseg+~L1+6|(fp),#64
	jpr	ge,L10048
	ld	r7,_mmud
	jpr	L10049
L10048:
	ld	r7,_mmus
L10049:
	ld	r6,|_stkseg+~L1+6|(fp)
	ldl	rr4,rr12
	callr	_loadsd
L10041:
	ldm	r11,_stkseg+8(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _ldsdr */

.psec data
.data

L144:
.byte	%63,%6f,%64,%65,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
L145:
.byte	%64,%61,%74,%61,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
L152:
.byte	%73,%65,%67,%2f,%73,%6c,%6f,%74
.byte	%20,%25,%78,%2f,%25,%78,%3a,%20
.byte	%25,%78,%20,%25,%78,%20,%25,%78
.byte	%a,%0
.even
L153:
.byte	%73,%74,%61,%6b,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
L154:
.byte	%4d,%41,%50,%55,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
L155:
.byte	%44,%4d,%41,%31,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
L156:
.byte	%44,%4d,%41,%32,%3a,%20,%25,%78
.byte	%20,%25,%78,%20,%25,%78,%a,%0
.even
/*~~prmmu:*/

.psec
.code

_prmmu::
{
	dec	fp,#~L2
	ldk	r7,#10
	callr	_putchar
	testb	_u+544
	jpr	ne,L142
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#63
	ld	r7,_mmut
	callr	_getsd
	ldl	rr6,#L144
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#63
	ld	r7,_mmud
	callr	_getsd
	ldl	rr6,#L145
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	jpr	L146
L142:
	clr	|_stkseg+~L1+2|(fp)
	jpr	L149
L20011:
	ld	r2,|_stkseg+~L1+2|(fp)
	ldb	rl3,_u+1062(r2)
	extsb	r3
	ld	|_stkseg+~L1|(fp),r3
	cp	r3,#64
	jpr	ge,L150
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,r3
	ld	r7,_mmud
	jpr	L20009
L150:
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,|_stkseg+~L1|(fp)
	ld	r7,_mmus
L20009:
	callr	_getsd
	ldb	rl2,|_stkseg+~L1+7|(fp)
	ldb	rh2,#0
	ld	|_stkseg|(fp),r2
	ldl	rr6,#L152
	ld	r5,|_stkseg+~L1|(fp)
	ld	r4,|_stkseg+~L1+2|(fp)
	ld	r3,|_stkseg+~L1+4|(fp)
	ldb	rl2,|_stkseg+~L1+6|(fp)
	callr	_printf
	inc	|_stkseg+~L1+2|(fp),#1
L149:
	ldb	rl2,_u+1190
	extsb	r2
	cp	r2,|_stkseg+~L1+2|(fp)
	jpr	gt,L20011
L146:
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#63
	ld	r7,_mmus
	callr	_getsd
	ldl	rr6,#L153
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#59
	ld	r7,_mmut
	callr	_getsd
	ldl	rr6,#L154
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#60
	ld	r7,_mmut
	callr	_getsd
	ldl	rr6,#L155
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	ldl	rr4,#_stkseg+~L1+4
	add	r5,fp
	ld	r6,#61
	ld	r7,_mmut
	callr	_getsd
	ldl	rr6,#L156
	ld	r5,|_stkseg+~L1+4|(fp)
	ldb	rl4,|_stkseg+~L1+6|(fp)
	ldb	rh4,#0
	ldb	rl3,|_stkseg+~L1+7|(fp)
	ldb	rh3,#0
	callr	_printf
	inc	fp,#~L2
	ret
	~L1 := 2
	~L2 := 10
	}	/* _prmmu */

.psec data
.data

