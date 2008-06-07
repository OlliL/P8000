.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_sys1wstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%79
.byte	%73
.byte	%31
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
.byte	%9
.byte	%30
.byte	%39
.byte	%2f
.byte	%32
.byte	%38
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%30
.byte	%30
.byte	%3a
.byte	%32
.byte	%38
.byte	%3a
.byte	%35
.byte	%32
.byte	%0
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
.comm	200,	_coremap;
.comm	200,	_swapmap;
/*~~exec:*/

.psec
.code

_exec::
{
	ldl	rr2,_u+36
	inc	r3,#8
	subl	rr4,rr4
	ldl	@rr2,rr4
	callr	_exece
	ret
	~L1 := 0
	~L2 := 0
	}	/* _exec */

.psec data
.data

L117:
.byte	%4f,%75,%74,%20,%6f,%66,%20,%73
.byte	%77,%61,%70,%0
.even
/*~~exece:*/

.psec
.code

_exece::
{
	sub	fp,#~L2
	ldm	_stkseg+24(fp),r8,#6
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	|_stkseg+~L1+20|(fp),rr2
	testl	rr2
	jpr	eq,L10003
	clr	|_stkseg+~L1+6|(fp)
	subl	rr8,rr8
	ldl	rr6,|_stkseg+~L1+20|(fp)
	ld	r5,#64
	callr	_access
	test	r2
	jpr	ne,L111
	ldl	rr2,|_stkseg+~L1+20|(fp)
	ld	r4,rr2(#6)
	and	r4,#61440
	cp	r4,#32768
	jpr	ne,L113
	ld	r4,rr2(#6)
	and	r4,#73
	test	r4
	jpr	ne,L112
L113:
	ldb	_u+21,#13
	jpr	L111
L112:
	ldk	r12,#0
	clr	|_stkseg+~L1+4|(fp)
	ldk	r13,#0
	ldl	rr2,_u+36
	ldl	|_stkseg+~L1|(fp),rr2
	ldl	rr6,#_swapmap
	ldk	r5,#10
	callr	_malloc
	ld	|_stkseg+~L1+6|(fp),r2
	test	r2
	jpr	ne,L115
	ldl	rr6,#L117
	callr	_panic
L115:
	testb	_u+544
	jpr	ne,L10008
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#4)
	ldk	r4,#0
	testl	rr4
	jpr	ne,L121
L10008:
	testb	_u+544
	jpr	eq,L118
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#4)
	testl	rr4
	jpr	eq,L118
L121:
	subl	rr2,rr2
	ldl	|_stkseg+~L1+12|(fp),rr2
	testb	_u+544
	jpr	ne,L10010
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#4)
	ldk	r4,#0
	testl	rr4
	jpr	ne,L10009
L10010:
	testb	_u+544
	jpr	eq,L122
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#4)
	testl	rr4
	jpr	eq,L122
L10009:
	testb	_u+544
	jpr	eq,L123
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#4)
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+12
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#4)
	addl	rr2,#2
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+14
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ldl	rr2,|_stkseg+~L1|(fp)
	inc	r3,#4
	ldl	rr4,@rr2
	addl	rr4,#4
	jpr	L20000
L123:
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#4)
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+14
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ld	r7,|_stkseg+~L1+14|(fp)
	callr	_nsseg
	ld	|_stkseg+~L1+12|(fp),r2
	ldl	rr2,|_stkseg+~L1|(fp)
	inc	r3,#4
	ldl	rr4,@rr2
	addl	rr4,#2
L20000:
	ldl	@rr2,rr4
L122:
	testb	_u+544
	jpr	ne,L10013
	ldk	r2,#0
	ld	r3,|_stkseg+~L1+14|(fp)
	testl	rr2
	jpr	ne,L10013
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#8)
	ldk	r4,#0
	testl	rr4
	jpr	ne,L10012
L10013:
	testb	_u+544
	jpr	eq,L129
	ldl	rr2,|_stkseg+~L1+12|(fp)
	testl	rr2
	jpr	ne,L129
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	rr4,rr2(#8)
	testl	rr4
	jpr	eq,L129
L10012:
	ldl	rr2,|_stkseg+~L1|(fp)
	inc	r3,#4
	subl	rr4,rr4
	ldl	@rr2,rr4
	testb	_u+544
	jpr	eq,L130
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#8)
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+12
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#8)
	addl	rr2,#2
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+14
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ldl	rr2,|_stkseg+~L1+12|(fp)
	testl	rr2
	jpr	eq,L118
	ldl	rr2,|_stkseg+~L1|(fp)
	inc	r3,#8
	ldl	rr4,@rr2
	addl	rr4,#2
	ldl	@rr2,rr4
	jpr	L133
L130:
	ldl	rr6,|_stkseg+~L1|(fp)
	ldl	rr2,rr6(#8)
	ldl	rr6,rr2
	ldl	rr4,#_stkseg+~L1+14
	add	r5,fp
	callr	_fuword
	test	r2
	jpr	ne,L113
	ldk	r2,#0
	ld	r3,|_stkseg+~L1+14|(fp)
	testl	rr2
	jpr	eq,L118
	ld	r7,r3
	callr	_nsseg
	ld	|_stkseg+~L1+12|(fp),r2
L133:
	ldl	rr2,|_stkseg+~L1|(fp)
	inc	r3,#8
	ldl	rr4,@rr2
	addl	rr4,#2
	ldl	@rr2,rr4
	inc	|_stkseg+~L1+4|(fp),#1
L129:
	testb	_u+544
	jpr	ne,L10015
	ldk	r2,#0
	ld	r3,|_stkseg+~L1+14|(fp)
	testl	rr2
	jpr	eq,L118
L10015:
	testb	_u+544
	jpr	eq,L10016
	ldl	rr2,|_stkseg+~L1+12|(fp)
	testl	rr2
	jpr	eq,L118
L10016:
	inc	r12,#1
L139:
	cp	r13,#5119
	jpr	lt,L140
	ldb	_u+21,#7
L140:
	ldl	rr4,#_stkseg+~L1+11
	add	r5,fp
	ldl	rr6,|_stkseg+~L1+12|(fp)
	inc	|_stkseg+~L1+14|(fp),#1
	callr	_fubyte
	test	r2
	jpr	eq,L142
	ldb	_u+21,#14
L142:
	testb	_u+21
	jpr	ne,L111
	ld	r2,r13
	and	r2,#511
	test	r2
	jpr	ne,L144
	testl	rr8
	jpr	eq,L145
	ldl	rr6,rr8
	callr	_bawrite
L145:
	ld	r3,|_stkseg+~L1+6|(fp)
	exts	rr2
	addl	rr2,_swplo
	ld	r4,r13
	sra	r4,#9
	ld	r7,r4
	exts	rr6
	addl	rr2,rr6
	ldl	rr4,rr2
	ld	r7,_swapdev
	callr	_getblk
	ldl	rr8,rr2
	ldl	rr2,rr8(#22)
	ldl	rr10,rr2
L144:
	inc	r13,#1
	ldb	rl2,|_stkseg+~L1+11|(fp)
	ldb	@rr10,rl2
	inc	r11,#1
	cpb	|_stkseg+~L1+11|(fp),#0
	jpr	gt,L139
	jpr	L121
L118:
	testl	rr8
	jpr	eq,L148
	ldl	rr6,rr8
	callr	_bawrite
L148:
	subl	rr8,rr8
	ld	r2,r13
	inc	r2,#1
	and	r2,#-2
	ld	r13,r2
	testb	_u+21
	jpr	ne,L10017
	ldl	rr6,|_stkseg+~L1+20|(fp)
	ld	r5,r2
	callr	_getxfile
	test	r2
	jpr	ne,L10017
	testb	_u+21
	jpr	eq,L150
L10017:
	ldl	rr2,_u+362
	ld	r4,rr2(#34)
	bit	r4,#15
	jpr	eq,L10018
	ldk	r4,#1
	jpr	L10019
L10018:
	sub	r4,r4
L10019:
	ldb	_u+544,rl4
	callr	_setscr
	jpr	L111
L150:
	ld	r2,r13
	neg	r2
	dec	r2,#2
	ld	|_stkseg+~L1+8|(fp),r2
	ld	r2,_u+1390
	ldb	rh2,rl2
	clrb	rl2
	ld	|_stkseg+~L1+12|(fp),r2
	testb	_u+544
	jpr	eq,L152
	ld	r2,r12
	add	r2,r12
	add	r2,r2
	ld	r3,|_stkseg+~L1+8|(fp)
	sub	r3,r2
	dec	r3,#12
	jpr	L20001
L152:
	ld	r2,r12
	add	r2,r12
	ld	r3,|_stkseg+~L1+8|(fp)
	sub	r3,r2
	dec	r3,#6
L20001:
	ld	|_stkseg+~L1+14|(fp),r3
	ldl	rr2,_u+362
	add	r3,#30
	ld	r4,|_stkseg+~L1+14|(fp)
	ld	@rr2,r4
	ld	r5,r12
	sub	r5,|_stkseg+~L1+4|(fp)
	ldl	rr6,|_stkseg+~L1+12|(fp)
	callr	_suword
	test	r2
	jpr	ne,L113
	ldk	r13,#0
	jpr	L156
L20002:
	ld	r5,_u+1390
	ldb	rh5,rl5
	clrb	rl5
	ldl	rr6,|_stkseg+~L1+12|(fp)
	callr	_suword
	test	r2
	jpr	ne,L113
	ldl	rr6,|_stkseg+~L1+12|(fp)
	inc	r7,#2
	ld	r5,|_stkseg+~L1+8|(fp)
	callr	_suword
	test	r2
	jpr	ne,L113
	inc	|_stkseg+~L1+14|(fp),#2
L170:
	ld	r2,r13
	and	r2,#511
	test	r2
	jpr	ne,L172
	testl	rr8
	jpr	eq,L173
	ldl	rr6,rr8
	callr	_brelse
L173:
	ld	r3,|_stkseg+~L1+6|(fp)
	exts	rr2
	addl	rr2,_swplo
	ld	r4,r13
	sra	r4,#9
	ld	r7,r4
	exts	rr6
	addl	rr2,rr6
	ldl	rr4,rr2
	ld	r7,_swapdev
	callr	_bread
	ldl	rr8,rr2
	ldl	rr2,rr8(#22)
	ldl	rr10,rr2
L172:
	ld	r2,_u+1390
	ldb	rh2,rl2
	clrb	rl2
	ld	|_stkseg+~L1+16|(fp),r2
	ld	r2,|_stkseg+~L1+8|(fp)
	ld	|_stkseg+~L1+18|(fp),r2
	inc	|_stkseg+~L1+8|(fp),#1
	ldl	rr4,rr10
	inc	r11,#1
	ldb	rl3,@rr4
	ldb	|_stkseg+~L1+11|(fp),rl3
	ldb	rl5,rl3
	extsb	r5
	ldl	rr6,|_stkseg+~L1+16|(fp)
	callr	_subyte
	test	r2
	jpr	eq,L176
	jpr	L113
L158:
	inc	|_stkseg+~L1+14|(fp),#2
	cp	r12,|_stkseg+~L1+4|(fp)
	jpr	ne,L165
	ldl	rr6,|_stkseg+~L1+12|(fp)
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
	inc	|_stkseg+~L1+14|(fp),#2
L165:
	dec	r12,#1
	cp	r12,#0
	jpr	lt,L157
	ldl	rr6,|_stkseg+~L1+12|(fp)
	ld	r5,|_stkseg+~L1+8|(fp)
	callr	_suword
	test	r2
	jpr	eq,L170
	jpr	L113
L176:
	inc	r13,#1
	ldb	rl2,|_stkseg+~L1+11|(fp)
	extsb	r2
	and	r2,#255
	jpr	ne,L170
L156:
	testb	_u+544
	jpr	eq,L158
	inc	|_stkseg+~L1+14|(fp),#2
	cp	r12,|_stkseg+~L1+4|(fp)
	jpr	ne,L159
	ldl	rr6,|_stkseg+~L1+12|(fp)
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
	ldl	rr6,|_stkseg+~L1+12|(fp)
	inc	r7,#2
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
	inc	|_stkseg+~L1+14|(fp),#4
L159:
	dec	r12,#1
	cp	r12,#0
	jpr	ge,L20002
L157:
	ldl	rr6,|_stkseg+~L1+12|(fp)
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
	testb	_u+544
	jpr	eq,L179
	ldl	rr6,|_stkseg+~L1+12|(fp)
	inc	r7,#2
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
L179:
	ld	r2,_u+1390
	ldb	rh2,rl2
	clrb	rl2
	ld	|_stkseg+~L1+16|(fp),r2
	ld	r2,|_stkseg+~L1+8|(fp)
	ld	|_stkseg+~L1+18|(fp),r2
	ldl	rr6,|_stkseg+~L1+16|(fp)
	sub	r5,r5
	callr	_suword
	test	r2
	jpr	ne,L113
	callr	_setregs
L111:
	testl	rr8
	jpr	eq,L183
	ldl	rr6,rr8
	callr	_brelse
L183:
	test	|_stkseg+~L1+6|(fp)
	jpr	eq,L184
	ldl	rr6,#_swapmap
	ldk	r5,#10
	ld	r4,|_stkseg+~L1+6|(fp)
	callr	_mfree
L184:
	ldl	rr6,|_stkseg+~L1+20|(fp)
	callr	_iput
L10003:
	ldm	r8,_stkseg+24(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 36
	}	/* _exece */

.psec data
.data

.even
L248:
.word	58885
.word	58887
.word	58897
.even
.even
L260:
.word	59141
.word	59143
.word	59153
.even
/*~~getxfile:*/

.psec
.code

_getxfile::
{
	.psec	data
	.data
	.even
L249:
	.long	L191
	.long	L199
	.long	L198
	.even
L261:
	.long	L258
	.long	L255
	.long	L257
	.psec
	.code
	sub	fp,#~L2
	ldm	_stkseg+438(fp),r8,#6
	ld	r11,r5
	ldl	rr12,rr6
	ldl	rr2,#_u+488
	ldl	_u+44,rr2
	ld	_u+48,#56
	subl	rr2,rr2
	ldl	_u+50,rr2
	ldb	_u+20,#1
	callr	_readi
	clrb	_u+20
	testb	_u+21
	jpr	ne,L20012
	test	_u+48
	jpr	eq,L190
L191:
	ldb	_u+21,#8
	jpr	L20012
L190:
	clr	|_stkseg+~L1+12|(fp)
	clr	|_stkseg+~L1+14|(fp)
	ld	r9,_u+488
	ld	r2,r9
	and	r2,#256
	test	r2
	jpr	ne,L192
	subl	rr2,rr2
	ldl	|_stkseg+~L1+28|(fp),rr2
	subl	rr2,rr2
	ldl	|_stkseg+~L1+24|(fp),rr2
	subl	rr2,rr2
	ldl	|_stkseg+~L1+20|(fp),rr2
	clr	|_stkseg+~L1+36|(fp)
	ld	r2,_u+498
	srl	r2,#4
	ld	|_stkseg+~L1+34|(fp),r2
	bit	_u+506,#5
	jpr	eq,L193
	ld	|_stkseg+~L1+40|(fp),#127
	jpr	L194
L193:
	ld	|_stkseg+~L1+40|(fp),#63
L194:
	cp	|_stkseg+~L1+34|(fp),#128
	jpr	gt,L191
	ld	r2,r9
	ldl	rr4,#L248
	ld	r6,#3
	cpir	r2,@rr4,r6,eq
	jpr	ne,L191
	sll	r6,#2
	neg	r6
	ldl	rr4,L249+8(r6)
	jp	@rr4
L198:
	inc	|_stkseg+~L1+12|(fp),#1
L199:
	ld	|_stkseg+~L1+32|(fp),#-1
	ldk	r10,#0
L202:
	ld	r3,|_stkseg+~L1+34|(fp)
	inc	r3,#1
	ld	r5,r3
	exts	rr4
	div	rr4,#2
	cp	r10,r5
	jpr	ge,L201
	clr	|_stkseg+~L1+172|(fp)
L205:
	cp	|_stkseg+~L1+172|(fp),#1
	jpr	gt,L204
	ld	r3,|_stkseg+~L1+172|(fp)
	sla	r3,#4
	ldb	rl4,_u+512(r3)
	extsb	r4
	ld	|_stkseg+~L1+38|(fp),r4
	ld	r3,|_stkseg+~L1+172|(fp)
	sla	r3,#4
	ld	r4,_u+516(r3)
	ld	_stkseg+~L1+430(fp),r4
	ld	r3,|_stkseg+~L1+172|(fp)
	sla	r3,#4
	ld	r4,_u+518(r3)
	ld	_stkseg+~L1+432(fp),r4
	ld	r3,|_stkseg+~L1+172|(fp)
	sla	r3,#4
	ld	r4,_u+520(r3)
	ld	_stkseg+~L1+434(fp),r4
	inc	|_stkseg+~L1+36|(fp),#1
	ld	r3,|_stkseg+~L1+36|(fp)
	cp	r3,|_stkseg+~L1+34|(fp)
	jpr	le,L208
L204:
	ldl	rr2,#_u+512
	ldl	_u+44,rr2
	ld	_u+48,#32
	ldb	_u+20,#1
	ldl	rr6,rr12
	callr	_readi
	clrb	_u+20
	inc	r10,#1
	jpr	L202
L208:
	inc	|_stkseg+~L1+32|(fp),#1
	ld	r3,|_stkseg+~L1+32|(fp)
	ldl	rr4,#_stkseg+~L1+44
	add	r5,fp
	add	r5,r3
	ldb	rl6,|_stkseg+~L1+39|(fp)
	ldb	@rr4,rl6
	ld	r3,|_stkseg+~L1+32|(fp)
	add	r3,r3
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	add	r5,r3
	clr	@rr4
	ld	r3,|_stkseg+~L1+172|(fp)
	sla	r3,#4
	ld	r4,_u+522(r3)
	bit	r4,#0
	jpr	eq,L209
	ld	r3,|_stkseg+~L1+32|(fp)
	add	r3,r3
	ldl	rr6,#_stkseg+~L1+174
	add	r7,fp
	add	r7,r3
	ld	r3,_stkseg+~L1+430(fp)
	ld	@rr6,r3
	test	|_stkseg+~L1+12|(fp)
	jpr	eq,L210
	ld	r3,|_stkseg+~L1+32|(fp)
	ldl	rr6,#_stkseg+~L1+44
	add	r7,fp
	add	r7,r3
	ldb	rl5,@rr6
	orb	rl5,#-128
	ldb	@rr6,rl5
	ldk	r6,#0
	ld	r7,_stkseg+~L1+430(fp)
	addl	rr6,|_stkseg+~L1+20|(fp)
	ldl	|_stkseg+~L1+20|(fp),rr6
	ldk	r6,#0
	ld	r7,_stkseg+~L1+430(fp)
	addl	rr6,#255
	sral	rr6,#8
	ld	r3,|_stkseg+~L1+32|(fp)
	ldl	rr0,#_stkseg+~L1+44
	add	r1,fp
	add	r1,r3
	ldl	rr2,rr0
	ldb	rl6,@rr2
	extsb	r6
	ld	r5,|_stkseg+~L1+32|(fp)
	sub	r4,r4
	callr	_sestabur
	test	r2
	jpr	eq,L203
	jpr	L191
L210:
	ld	r2,|_stkseg+~L1+32|(fp)
	ldl	rr4,#_stkseg+~L1+44
	add	r5,fp
	ldb	rl3,rr4(r2)
	ldb	_u+1191,rl3
	ldk	r4,#0
	ld	r5,_stkseg+~L1+430(fp)
	addl	rr4,#255
	sral	rr4,#8
	ld	_u+1192,r5
	ldk	r4,#0
	ld	r5,_stkseg+~L1+430(fp)
	addl	rr4,|_stkseg+~L1+24|(fp)
	ldl	|_stkseg+~L1+24|(fp),rr4
	test	_stkseg+~L1+432(fp)
	jpr	ne,L209
	test	_stkseg+~L1+434(fp)
	jpr	ne,L209
	ldk	r2,#0
	ld	r3,_stkseg+~L1+430(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	r7,r3
	ld	r6,|_stkseg+~L1+32|(fp)
	ldl	rr2,#_stkseg+~L1+44
	add	r3,fp
	ldb	rl4,rr2(r6)
	ldb	rl6,rl4
	extsb	r6
	ld	r5,|_stkseg+~L1+32|(fp)
	ldk	r4,#1
	callr	_sestabur
	test	r2
	jpr	ne,L191
L209:
	ld	r2,|_stkseg+~L1+172|(fp)
	sla	r2,#4
	ld	r3,_u+522(r2)
	bit	r3,#1
	jpr	eq,L215
	ld	r2,|_stkseg+~L1+32|(fp)
	add	r2,r2
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	add	r5,r2
	ld	r2,@rr4
	add	r2,_stkseg+~L1+432(fp)
	ld	@rr4,r2
	ld	r2,|_stkseg+~L1+32|(fp)
	add	r2,r2
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	ld	r3,rr4(r2)
	ldk	r4,#0
	ld	r5,r3
	ld	r2,r4
	ld	r3,_stkseg+~L1+434(fp)
	addl	rr4,rr2
	ldl	|_stkseg+~L1+16|(fp),rr4
	ld	r2,|_stkseg+~L1+18|(fp)
	ldk	r4,#0
	ld	r5,r2
	cpl	rr4,|_stkseg+~L1+16|(fp)
	jpr	eq,L216
L20013:
	ldb	_u+21,#12
	jpr	L20012
L216:
	ldk	r2,#0
	ld	r3,_stkseg+~L1+432(fp)
	addl	rr2,|_stkseg+~L1+24|(fp)
	ldl	|_stkseg+~L1+24|(fp),rr2
	ldk	r2,#0
	ld	r3,_stkseg+~L1+434(fp)
	addl	rr2,|_stkseg+~L1+28|(fp)
	ldl	|_stkseg+~L1+28|(fp),rr2
	ld	r2,|_stkseg+~L1+32|(fp)
	add	r2,r2
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	ld	r7,rr4(r2)
	add	r7,_stkseg+~L1+434(fp)
	ldk	r2,#0
	ld	r3,r7
	addl	rr2,#255
	sral	rr2,#8
	ld	r7,r3
	ld	r6,|_stkseg+~L1+32|(fp)
	ldl	rr2,#_stkseg+~L1+44
	add	r3,fp
	ldb	rl4,rr2(r6)
	ldb	rl6,rl4
	extsb	r6
	ld	r5,|_stkseg+~L1+32|(fp)
	ldk	r4,#1
	callr	_sestabur
	test	r2
	jpr	eq,L203
	jpr	L191
L215:
	ld	r2,|_stkseg+~L1+172|(fp)
	sla	r2,#4
	ld	r3,_u+522(r2)
	bit	r3,#2
	jpr	eq,L203
	ldk	r4,#0
	ld	r5,_stkseg+~L1+434(fp)
	addl	rr4,|_stkseg+~L1+28|(fp)
	ldl	|_stkseg+~L1+28|(fp),rr4
	ld	r7,_stkseg+~L1+434(fp)
	add	r7,_stkseg+~L1+430(fp)
	ldk	r4,#0
	ld	r5,r7
	addl	rr4,#255
	sral	rr4,#8
	ld	r7,r5
	ld	r2,|_stkseg+~L1+32|(fp)
	ldl	rr4,#_stkseg+~L1+44
	add	r5,fp
	ldb	rl6,rr4(r2)
	extsb	r6
	ld	r5,r2
	ldk	r4,#1
	callr	_sestabur
	test	r2
	jpr	ne,L191
L203:
	inc	|_stkseg+~L1+172|(fp),#1
	jpr	L205
L201:
	ldl	rr2,|_stkseg+~L1+20|(fp)
	testl	rr2
	jpr	eq,L221
	ldb	rl2,@rr12
	extsb	r2
	and	r2,#32
	test	r2
	jpr	ne,L221
	ldb	rl2,rr12(#1)
	cpb	rl2,#1
	jpr	eq,L221
L20014:
	ldb	_u+21,#26
	jpr	L20012
L221:
	ldk	r2,#0
	ld	r3,r11
	addl	rr2,#255
	sral	rr2,#8
	inc	r3,#5
	ld	|_stkseg+~L1+8|(fp),r3
	ld	r2,|_stkseg+~L1+40|(fp)
	ld	_u+1390,r2
	ld	r7,r3
	ld	r6,|_stkseg+~L1+40|(fp)
	sub	r5,r5
	sub	r4,r4
	callr	_sestabur
	test	r2
	jpr	ne,L191
	ldl	rr2,|_stkseg+~L1+20|(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	|_stkseg+~L1+6|(fp),r3
	ldl	rr2,|_stkseg+~L1+24|(fp)
	addl	rr2,|_stkseg+~L1+28|(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	r8,r3
	ld	r2,|_stkseg+~L1+6|(fp)
	inc	r2,#10
	add	r2,r8
	add	r2,|_stkseg+~L1+8|(fp)
	cp	r2,_umemory
	jpr	ugt,L20013
	ldb	_u+544,#1
	callr	_setscr
	ld	r2,_u+498
	add	r2,#24
	ld	_u+486,r2
	ldb	rl2,|_stkseg+~L1+35|(fp)
	ldb	_u+1190,rl2
	ldk	r10,#0
	jpr	L226
L20004:
	ldl	rr2,#_stkseg+~L1+44
	add	r3,fp
	ldb	rl4,rr2(r10)
	ldb	_u+1062(r10),rl4
	inc	r10,#1
L226:
	cp	r10,|_stkseg+~L1+34|(fp)
	jpr	lt,L20004
	ldb	rl2,|_stkseg+~L1+13|(fp)
	ldb	_u+484,rl2
	clr	_u+374
	clrb	_u+476
	callr	_xfree
	ld	r2,r8
	inc	r2,#10
	add	r2,|_stkseg+~L1+8|(fp)
	ld	r10,r2
	ld	r7,r2
	callr	_expand
	jpr	L229
L20006:
	ldl	rr2,_u+32
	ld	r7,rr2(#18)
	add	r7,r10
	callr	_clearseg
L229:
	dec	r10,#1
	cp	r10,#10
	jpr	ge,L20006
	ldl	rr2,#_stkseg+~L1+174
	add	r3,fp
	ldl	rr4,|_stkseg+~L1+20|(fp)
	ldl	rr6,rr12
	callr	_xalloc
	ldl	rr2,|_stkseg+~L1+20|(fp)
	testl	rr2
	jpr	ne,L233
	callr	_invsdrs
	callr	_segureg
L233:
	ld	r3,_u+486
	exts	rr2
	ldl	_u+50,rr2
	clr	|_stkseg+~L1+32|(fp)
	jpr	L238
L20008:
	ld	r2,|_stkseg+~L1+32|(fp)
	ldb	rl3,_u+1062(r2)
	extsb	r3
	bit	r3,#7
	jpr	eq,L239
	add	r2,r2
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	ld	r3,rr4(r2)
	ldk	r4,#0
	ld	r5,r3
	addl	rr4,_u+50
	ldl	_u+50,rr4
	ld	r2,|_stkseg+~L1+32|(fp)
	sla	r2,#2
	ldb	rl7,_u+552(r2)
	subb	rh7,rh7
	inc	r7,#1
	ld	r6,|_stkseg+~L1+32|(fp)
	ldb	rl2,_u+1062(r6)
	ldb	rl6,rl2
	extsb	r6
	ld	r5,|_stkseg+~L1+32|(fp)
	ldk	r4,#1
	callr	_sestabur
	jpr	L236
L239:
	ld	r2,|_stkseg+~L1+32|(fp)
	ldb	rl3,_u+1062(r2)
	extsb	r3
	and	r3,#127
	ldb	rh3,rl3
	clrb	rl3
	ld	_u+44,r3
	clr	_u+46
	ld	r2,|_stkseg+~L1+32|(fp)
	add	r2,r2
	ldl	rr4,#_stkseg+~L1+174
	add	r5,fp
	ld	r3,rr4(r2)
	ld	_u+48,r3
	ldl	rr6,rr12
	callr	_readi
L236:
	inc	|_stkseg+~L1+32|(fp),#1
L238:
	ld	r2,|_stkseg+~L1+32|(fp)
	cp	r2,|_stkseg+~L1+34|(fp)
	jpr	lt,L20008
	ldl	rr2,_u+32
	ldb	rl4,rr2(#1)
	extsb	r4
	and	r4,#16
	test	r4
	jpr	ne,L241
	ld	r4,rr12(#6)
	bit	r4,#11
	jpr	eq,L242
	test	_u+24
	jpr	eq,L242
	ld	r2,rr12(#10)
	ld	_u+24,r2
	ld	r2,rr12(#10)
	ldl	rr4,_u+32
	inc	r5,#10
	ld	@rr4,r2
L242:
	ld	r2,rr12(#6)
	bit	r2,#10
	jpr	eq,L244
	ld	r2,rr12(#12)
	ld	_u+26,r2
	jpr	L244
L241:
	ldl	rr6,_u+32
	ldk	r5,#5
	callr	_psignal
L244:
	ld	r2,|_stkseg+~L1+6|(fp)
	ld	_u+216,r2
	ld	_u+218,r8
	ld	r2,|_stkseg+~L1+8|(fp)
	ld	_u+220,r2
	callr	_segureg
	jpr	L20012
L192:
	ldk	r2,#0
	ld	r3,_u+516
	ldl	|_stkseg+~L1+20|(fp),rr2
	ld	r2,_u+498
	ld	|_stkseg+~L1+10|(fp),r2
	cp	r2,#16
	jpr	eq,L10025
	cp	r9,#59143
	jpr	ne,L250
L10025:
	ldk	r2,#0
	ld	r3,_u+518
	jpr	L20009
L250:
	ldk	r2,#0
	ld	r3,_u+534
L20009:
	ldl	|_stkseg+~L1+24|(fp),rr2
	ldl	rr2,_u+494
	ldk	r4,#0
	ld	r5,r3
	ldl	|_stkseg+~L1+28|(fp),rr4
	ld	_u+486,#56
	cp	|_stkseg+~L1+10|(fp),#16
	jpr	ne,L252
	ld	r2,_u+486
	sub	r2,|_stkseg+~L1+10|(fp)
	ld	_u+486,r2
L252:
	ld	r2,r9
	ldl	rr4,#L260
	ld	r6,#3
	cpir	r2,@rr4,r6,eq
	jpr	ne,L191
	sll	r6,#2
	neg	r6
	ldl	rr4,L261+8(r6)
	jp	@rr4
L255:
	ldl	rr4,|_stkseg+~L1+24|(fp)
	addl	rr4,|_stkseg+~L1+20|(fp)
	ldl	|_stkseg+~L1+16|(fp),rr4
	ldl	|_stkseg+~L1+24|(fp),rr4
	ld	r3,|_stkseg+~L1+18|(fp)
	ld	_u+518,r3
	ld	r4,|_stkseg+~L1+18|(fp)
	ldk	r6,#0
	ld	r7,r4
	cpl	rr6,|_stkseg+~L1+16|(fp)
	jpr	ne,L20013
	subl	rr2,rr2
	ldl	|_stkseg+~L1+20|(fp),rr2
	clr	_u+516
	jpr	L253
L20016:
	ldb	rl2,@rr12
	extsb	r2
	and	r2,#32
	test	r2
	jpr	ne,L262
	ldb	rl2,rr12(#1)
	cpb	rl2,#1
	jpr	eq,L262
	jpr	L20014
L257:
	inc	|_stkseg+~L1+12|(fp),#1
	jpr	L253
L258:
	inc	|_stkseg+~L1+14|(fp),#1
L253:
	ldl	rr2,|_stkseg+~L1+20|(fp)
	testl	rr2
	jpr	ne,L20016
L262:
	ldl	rr2,|_stkseg+~L1+20|(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	|_stkseg+~L1+6|(fp),r3
	ldl	rr2,|_stkseg+~L1+24|(fp)
	addl	rr2,|_stkseg+~L1+28|(fp)
	ldl	|_stkseg+~L1+16|(fp),rr2
	ld	r2,|_stkseg+~L1+18|(fp)
	ldk	r4,#0
	ld	r5,r2
	cpl	rr4,|_stkseg+~L1+16|(fp)
	jpr	ne,L20013
	ldl	rr2,|_stkseg+~L1+16|(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	r8,r3
	ld	r2,r4
	ld	r3,r11
	addl	rr2,#255
	sral	rr2,#8
	inc	r3,#5
	ld	|_stkseg+~L1+8|(fp),r3
	clrb	_u+544
	callr	_setscr
	test	|_stkseg+~L1+14|(fp)
	jpr	eq,L264
	testb	_u+484
	jpr	ne,L265
	ld	r2,|_stkseg+~L1+6|(fp)
	add	r2,#31
	srl	r2,#5
	ld	r3,_u+216
	add	r3,#31
	sra	r3,#5
	cp	r2,r3
	jpr	ne,L20013
L265:
	test	r11
	jpr	ne,L20013
	ld	r8,_u+218
	ld	r2,_u+220
	ld	|_stkseg+~L1+8|(fp),r2
	ldb	rl3,_u+484
	extsb	r3
	ld	|_stkseg+~L1+12|(fp),r3
	callr	_xfree
	ldl	rr6,rr12
	ldl	rr4,|_stkseg+~L1+20|(fp)
	callr	_xalloc
	ldl	rr2,_u+502
	and	r3,#65534
	ldl	rr4,_u+362
	add	r5,#38
	ld	@rr4,r3
	jpr	L268
L264:
	clr	_u+374
	callr	_xfree
	ld	r2,r8
	inc	r2,#10
	add	r2,|_stkseg+~L1+8|(fp)
	ld	r10,r2
	ld	r7,r2
	callr	_expand
	jpr	L269
L20011:
	ldl	rr2,_u+32
	ld	r7,rr2(#18)
	add	r7,r10
	callr	_clearseg
L269:
	dec	r10,#1
	cp	r10,#10
	jpr	ge,L20011
	ldl	rr6,rr12
	ldl	rr4,|_stkseg+~L1+20|(fp)
	callr	_xalloc
	sub	r7,r7
	ld	r6,r8
	sub	r5,r5
	ldk	r4,#1
	callr	_estabur
	ld	_u+44,#16128
	clr	_u+46
	ld	r2,_u+486
	add	r2,|_stkseg+~L1+22|(fp)
	ldk	r4,#0
	ld	r5,r2
	ldl	_u+50,rr4
	ld	r2,|_stkseg+~L1+26|(fp)
	ld	_u+48,r2
	ldl	rr6,rr12
	callr	_readi
	ldl	rr2,_u+32
	ldb	rl4,rr2(#1)
	extsb	r4
	and	r4,#16
	test	r4
	jpr	ne,L272
	ld	r4,rr12(#6)
	bit	r4,#11
	jpr	eq,L273
	test	_u+24
	jpr	eq,L273
	ld	r4,rr12(#10)
	ld	_u+24,r4
	ld	r2,rr12(#10)
	ldl	rr4,_u+32
	inc	r5,#10
	ld	@rr4,r2
L273:
	ld	r2,rr12(#6)
	bit	r2,#10
	jpr	eq,L268
	ld	r2,rr12(#12)
	ld	_u+26,r2
	jpr	L268
L272:
	ldl	rr6,_u+32
	ldk	r5,#5
	callr	_psignal
L268:
	ld	r2,|_stkseg+~L1+6|(fp)
	ld	_u+216,r2
	ld	_u+218,r8
	ld	r2,|_stkseg+~L1+8|(fp)
	ld	_u+220,r2
	ldb	rl3,|_stkseg+~L1+13|(fp)
	ldb	_u+484,rl3
	ld	_u+1390,#127
	ld	r7,|_stkseg+~L1+6|(fp)
	ld	r6,r8
	ld	r5,|_stkseg+~L1+8|(fp)
	ldk	r4,#1
	callr	_estabur
L20012:
	ld	r2,|_stkseg+~L1+14|(fp)
	ldm	r8,_stkseg+438(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 450
	}	/* _getxfile */

.psec data
.data

/*~~setregs:*/

.psec
.code

_setregs::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r8,#6
	ldl	rr12,#_u+262
	jpr	L280
L20018:
	ldl	rr2,@rr12
	ldk	r2,#0
	and	r3,#1
	testl	rr2
	jpr	ne,L278
	subl	rr2,rr2
	ldl	@rr12,rr2
L278:
	inc	r13,#4
L280:
	cpl	rr12,#_u+342
	jpr	ult,L20018
	ldk	r11,#0
L20020:
	ld	r2,r11
	add	r2,r11
	ldl	rr4,_u+362
	add	r5,r2
	clr	@rr4
	inc	r11,#1
	cp	r11,#15
	jpr	lt,L20020
	testb	_u+544
	jpr	eq,L285
	ldl	rr2,_u+362
	add	r3,#28
	ld	r4,_u+1390
	ldb	rh4,rl4
	clrb	rl4
	ld	@rr2,r4
L285:
	ldk	r11,#0
L20022:
	ld	r3,r11
	mult	rr2,#10
	ldl	rr4,#_u+1246
	add	r5,r3
	ld	@rr4,#16383
	ld	r3,r11
	mult	rr2,#10
	ldl	rr4,#_u+1246
	add	r5,r3
	inc	r5,#2
	ld	@rr4,#32768
	ld	r3,r11
	mult	rr2,#10
	ldl	rr4,#_u+1246
	add	r5,r3
	inc	r5,#4
	clr	@rr4
	ld	r3,r11
	mult	rr2,#10
	ldl	rr4,#_u+1246
	add	r5,r3
	inc	r5,#6
	clr	@rr4
	ld	r3,r11
	mult	rr2,#10
	ldl	rr4,#_u+1246
	add	r5,r3
	inc	r5,#8
	clr	@rr4
	inc	r11,#1
	cp	r11,#8
	jpr	lt,L20022
	callr	_fsetregs
	ldl	rr2,_u+502
	and	r3,#65534
	ldl	rr4,_u+362
	add	r5,#38
	ld	@rr4,r3
	testb	_u+544
	jpr	ne,L290
	ldl	rr2,_u+362
	add	r3,#36
	ld	@rr2,#16128
	jpr	L291
L290:
	ldl	rr2,_u+502
	sral	rr2,#16
	ldl	rr4,_u+362
	add	r5,#36
	ld	@rr4,r3
L291:
	ldk	r11,#0
L20024:
	ldb	rl2,_u+184(r11)
	extsb	r2
	bit	r2,#0
	jpr	eq,L292
	ld	r2,r11
	sla	r2,#2
	ldl	rr4,_u+104(r2)
	ldl	rr8,rr4
	ld	r2,r11
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
	ldl	rr6,rr8
	callr	_closef
	resb	_u+184(r11),#0
L292:
	inc	r11,#1
	cp	r11,#20
	jpr	lt,L20024
	resb	_u+1208,#0
	ldl	rr6,#_u+64
	ldl	rr4,#_u+1194
	ldk	r3,#14
	callr	_bcopy
	ldm	r8,_stkseg+0(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _setregs */

.psec data
.data

/*~~rexit:*/

.psec
.code

_rexit::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r12,#2
	ldl	rr12,_u+36
	ld	r7,@rr12
	and	r7,#255
	ldb	rh7,rl7
	clrb	rl7
	callr	_exit
	ldm	r12,_stkseg+0(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _rexit */

.psec data
.data

/*~~exit:*/

.psec
.code

_exit::
{
	sub	fp,#~L2
	ldm	_stkseg+6(fp),r8,#6
	ld	|_stkseg+~L1+0|(fp),r7
	ldl	rr10,_u+32
	ldl	rr2,rr10
	inc	r3,#1
	ldb	rl6,@rr2
	andb	rl6,#-81
	ldb	@rr2,rl6
	ldl	rr2,rr10
	add	r3,#34
	clr	@rr2
	ldk	r13,#0
L20026:
	ld	r2,r13
	sla	r2,#2
	ldl	rr4,#_u+262
	add	r5,r2
	ldl	rr2,#1
	ldl	@rr4,rr2
	inc	r13,#1
	cp	r13,#20
	jpr	lt,L20026
	ldl	rr2,rr10
	inc	r3,#12
	ld	r4,rr10(#14)
	cp	r4,@rr2
	jpr	ne,L305
	ldl	rr2,_u+478
	testl	rr2
	jpr	eq,L305
	ldl	rr4,rr10
	inc	r5,#12
	ldl	rr2,_u+478
	ld	r6,rr2(#50)
	cp	r6,@rr4
	jpr	ne,L305
	add	r3,#50
	clr	@rr2
	ld	r7,rr10(#12)
	ldk	r6,#1
	callr	_signal
L305:
	ldl	rr2,rr10
	inc	r3,#12
	clr	@rr2
	ldk	r13,#0
L20028:
	ld	r2,r13
	sla	r2,#2
	ldl	rr4,_u+104(r2)
	ldl	|_stkseg+~L1+2|(fp),rr4
	ld	r2,r13
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
	ldl	rr6,|_stkseg+~L1+2|(fp)
	callr	_closef
	inc	r13,#1
	cp	r13,#20
	jpr	lt,L20028
	ldl	rr6,_u+56
	callr	_plock
	ldl	rr6,_u+56
	callr	_iput
	ldl	rr2,_u+60
	testl	rr2
	jpr	eq,L311
	ldl	rr6,rr2
	callr	_plock
	ldl	rr6,_u+60
	callr	_iput
L311:
	callr	_xfree
	callr	_acct
	ld	r5,rr10(#20)
	ld	r4,rr10(#18)
	ldl	rr6,#_coremap
	callr	_mfree
	ldb	@rr10,#5
	ldl	rr2,rr10
	add	r3,#18
	ld	r4,|_stkseg+~L1|(fp)
	ld	@rr2,r4
	ldl	rr2,_u+350
	addl	rr2,_u+342
	ldl	rr4,rr10
	add	r5,#20
	ldl	@rr4,rr2
	ldl	rr2,_u+354
	addl	rr2,_u+346
	ldl	rr4,rr10
	add	r5,#24
	ldl	@rr4,rr2
	ldl	rr8,#_proc+36
	jpr	L315
L20030:
	ldl	rr2,rr8
	inc	r3,#16
	ld	r4,rr10(#14)
	cp	r4,@rr2
	jpr	ne,L316
	ldl	rr2,rr8
	inc	r3,#16
	ld	@rr2,#1
	cpb	@rr8,#5
	jpr	ne,L317
	ldl	rr6,#_proc+36
	ld	r5,#18
	callr	_psignal
L317:
	cpb	@rr8,#6
	jpr	ne,L320
	ldl	rr6,rr8
	callr	_setrun
	jpr	L320
L316:
	ldl	rr2,rr8
	inc	r3,#14
	ld	r4,rr10(#16)
	cp	r4,@rr2
	jpr	ne,L320
	ldl	rr6,rr8
	ld	r5,#18
	callr	_psignal
L320:
	ldl	rr2,rr8
	inc	r3,#12
	ld	r4,rr10(#14)
	cp	r4,@rr2
	jpr	ne,L313
	ldl	rr2,rr8
	inc	r3,#12
	clr	@rr2
L313:
	add	r9,#36
L315:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr8,rr4
	jpr	ult,L20030
	callr	_swtch
	ldm	r8,_stkseg+6(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _exit */

.psec data
.data

/*~~wait:*/

.psec
.code

_wait::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldk	r13,#0
L326:
	ldl	rr10,#_proc
	jpr	L329
L20032:
	ldl	rr2,_u+32
	inc	r3,#14
	ld	r4,rr10(#16)
	cp	r4,@rr2
	jpr	ne,L327
	inc	r13,#1
	cpb	@rr10,#5
	jpr	ne,L331
	ldl	rr6,rr10
	ldk	r5,#1
	callr	_freeproc
	jpr	L10035
L331:
	cpb	@rr10,#6
	jpr	ne,L327
	ldb	rl2,rr10(#1)
	extsb	r2
	and	r2,#32
	test	r2
	jpr	ne,L327
	ldl	rr2,rr10
	inc	r3,#1
	setb	@rr2,#5
	ld	r2,rr10(#14)
	ld	_u+40,r2
	ldl	rr6,rr10
	callr	_fsig
	ldb	rh2,rl2
	clrb	rl2
	or	r2,#127
	ld	_u+42,r2
	jpr	L10035
L327:
	add	r11,#36
L329:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr10,rr4
	jpr	ult,L20032
	test	r13
	jpr	eq,L336
	ldl	rr6,_u+32
	ld	r5,#30
	callr	_sleep
	jpr	L326
L336:
	ldb	_u+21,#10
L10035:
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _wait */

.psec data
.data

/*~~freeproc:*/

.psec
.code

_freeproc::
{
	dec	fp,#~L2
	ldm	_stkseg+6(fp),r12,#2
	ld	|_stkseg+~L1+0|(fp),r5
	ldl	rr12,rr6
	test	r5
	jpr	eq,L339
	ld	r4,rr12(#14)
	ld	_u+40,r4
	ld	r4,rr12(#18)
	ld	_u+42,r4
L339:
	ldl	rr2,rr12(#20)
	addl	rr2,_u+350
	ldl	_u+350,rr2
	ldl	rr2,rr12(#24)
	addl	rr2,_u+354
	ldl	_u+354,rr2
	clrb	@rr12
	ldl	rr2,rr12
	inc	r3,#14
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#16
	clr	@rr2
	ldl	rr2,rr12
	inc	r3,#6
	subl	rr4,rr4
	ldl	@rr2,rr4
	ldl	rr2,rr12
	inc	r3,#1
	clrb	@rr2
	ldl	rr2,rr12
	add	r3,#22
	subl	rr4,rr4
	ldl	@rr2,rr4
	ldm	r12,_stkseg+6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _freeproc */

.psec data
.data

/*~~fork:*/

.psec
.code

_fork::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r8,#6
	testb	_u+544
	jpr	eq,L342
	ld	r2,_u+216
	inc	r2,#10
	add	r2,_u+218
	add	r2,_u+220
	inc	r2,#1
	sra	r2,#1
	ld	r8,r2
	ldl	rr6,#_swapmap
	ld	r5,r2
	callr	_malloc
	ld	r9,r2
	test	r2
	jpr	ne,L343
L20037:
	ldb	_u+21,#12
	jpr	L344
L343:
	ldl	rr6,#_swapmap
	ld	r5,r8
	jpr	L20034
L342:
	ld	r5,_Maxmem
	inc	r5,#1
	sra	r5,#1
	ldl	rr6,#_swapmap
	callr	_malloc
	ld	r9,r2
	test	r2
	jpr	eq,L20037
	ld	r5,_Maxmem
	inc	r5,#1
	sra	r5,#1
	ldl	rr6,#_swapmap
L20034:
	ld	r4,r9
	callr	_mfree
	ldk	r9,#0
	subl	rr10,rr10
	ldl	rr12,#_proc
	jpr	L349
L20036:
	testb	@rr12
	jpr	ne,L350
	testl	rr10
	jpr	ne,L350
	ldl	rr10,rr12
	jpr	L347
L350:
	ld	r2,rr12(#10)
	cp	r2,_u+24
	jpr	ne,L347
	testb	@rr12
	jpr	eq,L347
	inc	r9,#1
L347:
	add	r13,#36
L349:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr12,rr4
	jpr	ult,L20036
	testl	rr10
	jpr	eq,L10044
	test	_u+24
	jpr	eq,L353
	ld	r2,_Nproc
	dec	r2,#1
	ld	r5,r2
	mult	rr4,#36
	ldl	rr2,#_proc
	add	r3,r5
	cpl	rr10,rr2
	jpr	eq,L10044
	cp	r9,_Maxuprc
	jpr	le,L353
L10044:
	ldb	_u+21,#11
	jpr	L344
L353:
	ldl	rr12,_u+32
	callr	_newproc
	test	r2
	jpr	eq,L355
	ld	r2,rr12(#14)
	ld	_u+40,r2
	ldl	rr2,_time
	ldl	_u+1212,rr2
	ld	r4,rr10(#20)
	ld	r7,r4
	exts	rr6
	ldl	_u+1220,rr6
	subl	rr2,rr2
	ldl	_u+1236,rr2
	subl	rr2,rr2
	ldl	_u+1228,rr2
	subl	rr2,rr2
	ldl	_u+1224,rr2
	subl	rr2,rr2
	ldl	_u+354,rr2
	subl	rr2,rr2
	ldl	_u+346,rr2
	subl	rr2,rr2
	ldl	_u+350,rr2
	subl	rr2,rr2
	ldl	_u+342,rr2
	ldb	_u+1208,#1
	jpr	L10041
L355:
	ld	r2,rr10(#14)
	ld	_u+40,r2
L344:
	ldl	rr2,_u+362
	add	r3,#38
	inc	@rr2,#2
L10041:
	ldm	r8,_stkseg+4(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _fork */

.psec data
.data

