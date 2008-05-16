.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_tt0wstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%74
.byte	%74
.byte	%30
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
.byte	%32
.byte	%39
.byte	%2f
.byte	%38
.byte	%33
.byte	%20
.byte	%30
.byte	%31
.byte	%3a
.byte	%30
.byte	%32
.byte	%3a
.byte	%35
.byte	%38
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
L138:
.byte	%28,%69,%6e,%74,%72,%29,%0
.even
L146:
.byte	%28,%71,%75,%69,%74,%29,%a,%0
.even
L162:
.byte	%58,%58,%58,%0
.even
L169:
.byte	%28,%65,%6f,%66,%29,%a,%0
.even
/*~~ttin:*/

.psec
.code

_ttin::
{
	sub	fp,#~L2
	ldm	_stkseg+18(fp),r8,#6
	ldm	|_stkseg+~L1+0|(fp),r3,#3
	ldl	rr12,rr6
	ld	r2,rr12(#38)
	ld	r10,r2
	ld	r2,r3
	test	r3
	jpr	eq,L94
	cp	r3,#1
	jpr	eq,L124
	ldl	rr6,|_stkseg+~L1+2|(fp)
	ldl	rr4,rr12
	callr	_putcb
	ld	r3,|_stkseg+~L1|(fp)
	exts	rr2
	addl	rr2,_sysinfo+100
	ldl	_sysinfo+100,rr2
	ldl	rr2,|_stkseg+~L1+2|(fp)
	inc	r3,#6
	ldl	rr4,|_stkseg+~L1+2|(fp)
	ldb	rl6,rr4(#4)
	extsb	r6
	add	r3,r6
L20004:
	ldl	rr8,rr2
	cp	@rr12,#180
	jpr	gt,L20006
L128:
	ld	r2,rr12(#44)
	ld	r10,r2
	ldl	rr2,rr12(#46)
	and	r2,#4
	ldk	r3,#0
	testl	rr2
	jpr	ne,L131
	ld	r2,r10
	test	r10
	jpr	ne,L132
L131:
	bit	r10,#1
	jpr	ne,L176
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#61439
	ldl	@rr2,rr4
	ldb	rl2,rr12(#60)
	subb	rh2,rh2
	cp	r2,@rr12
	jpr	ugt,L177
	ldl	rr2,rr12
	add	r3,#53
	ldb	@rr2,#1
	jpr	L176
L20006:
	bit	r10,#12
	jpr	eq,L129
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#8
	testl	rr2
	jpr	ne,L129
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#4
	call	@rr2
L129:
	cp	@rr12,#256
	jpr	le,L128
	ldl	rr6,rr12
	ldk	r5,#1
	jpr	L20036
L20038:
	bit	r10,#0
	jpr	ne,L10000
	bit	r10,#1
	jpr	eq,L102
	ld	r7,rr12(#50)
	ldk	r6,#2
	callr	_signal
	ldl	rr6,rr12
	ldk	r5,#3
L20036:
	callr	_ttyflush
	jpr	L10000
L94:
	inc	|_stkseg+~L1|(fp),#1
	ld	r11,|_stkseg+~L1+4|(fp)
	bit	r11,#12
	jpr	eq,L95
	bit	r10,#4
	jpr	ne,L95
	res	r11,#12
L95:
	ld	r3,r11
	and	r3,#28672
	jpr	eq,L96
	ld	r3,r11
	and	r3,#255
	test	r3
	jpr	eq,L20038
	bit	r10,#2
	jpr	ne,L10000
L102:
	bit	r10,#3
	jpr	eq,L104
	ldl	rr6,rr12
	ldl	rr4,#255
	ldk	r3,#1
	callr	_ttin
	ldl	rr6,rr12
	subl	rr4,rr4
	ldk	r3,#1
	callr	_ttin
	jpr	L105
L104:
	ldk	r11,#0
L105:
	set	r11,#8
	jpr	L106
L20003:
	and	r11,#127
L106:
	bit	r10,#10
	jpr	eq,L112
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#256
	testl	rr2
	jpr	eq,L113
	cp	r11,#17
	jpr	eq,L10003
	bit	r10,#11
	jpr	eq,L115
L10003:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#3
	jpr	L20001
L96:
	bit	r10,#5
	jpr	ne,L20003
	and	r11,#255
	cp	r11,#255
	jpr	ne,L106
	bit	r10,#3
	jpr	eq,L106
	ld	r7,#255
	ldl	rr4,rr12
	callr	_putc
	test	r2
	jpr	eq,L106
	jpr	L10000
L113:
	cp	r11,#19
	jpr	ne,L115
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#2
L20001:
	ldl	rr2,|_stkseg+~L1+14|(fp)
	call	@rr2
L115:
	cp	r11,#17
	jpr	eq,L10000
	cp	r11,#19
	jpr	eq,L10000
L112:
	cp	r11,#10
	jpr	ne,L118
	bit	r10,#6
	jpr	eq,L118
	ldk	r11,#13
	jpr	L119
L118:
	cp	r11,#13
	jpr	ne,L119
	bit	r10,#7
	jpr	ne,L10000
	bit	r10,#8
	jpr	eq,L119
	ldk	r11,#10
L119:
	bit	r10,#9
	jpr	eq,L123
	cp	r11,#65
	jpr	lt,L123
	cp	r11,#90
	jpr	gt,L123
	add	r11,#32
L123:
	ld	r3,r11
	exts	rr2
	ldl	|_stkseg+~L1+2|(fp),rr2
L124:
	ldb	rl7,|_stkseg+~L1+5|(fp)
	extsb	r7
	ldl	rr4,rr12
	callr	_putc
	test	r2
	jpr	ne,L10000
	ldl	rr2,_sysinfo+100
	addl	rr2,#1
	ldl	_sysinfo+100,rr2
	ldl	rr2,#_stkseg+~L1+5
	add	r3,fp
	jpr	L20004
L20010:
	ld	r7,rr12(#50)
	ldk	r6,#2
	callr	_signal
	bit	r10,#7
	jpr	ne,L136
	ldl	rr6,rr12
	ldk	r5,#3
	callr	_ttyflush
L136:
	bit	r10,#3
	jpr	eq,L132
	bit	r10,#13
	jpr	eq,L132
	ldl	rr2,#L138
	ldl	|_stkseg+~L1+10|(fp),rr2
	jpr	L141
L20008:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	ldb	rl6,@rr4
	ldb	rl5,rl6
	extsb	r5
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
L141:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	ne,L20008
	jpr	L20035
L20020:
	ldb	rl2,rr12(#56)
	subb	rh2,rh2
	cp	r11,r2
	jpr	eq,L20010
	ldb	rl2,rr12(#57)
	subb	rh2,rh2
	cp	r11,r2
	jpr	ne,L134
	ld	r7,rr12(#50)
	ldk	r6,#3
	callr	_signal
	bit	r10,#7
	jpr	ne,L144
	ldl	rr6,rr12
	ldk	r5,#3
	callr	_ttyflush
L144:
	bit	r10,#3
	jpr	eq,L132
	bit	r10,#13
	jpr	eq,L132
	ldl	rr2,#L146
	ldl	|_stkseg+~L1+10|(fp),rr2
	jpr	L149
L20012:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	ldb	rl6,@rr4
	ldb	rl5,rl6
	extsb	r5
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
L149:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	ne,L20012
	jpr	L20035
L151:
	ldb	rl2,rr12(#61)
	subb	rh2,rh2
	cp	r11,r2
	jpr	ne,L153
L20025:
	ldl	rr2,rr12
	add	r3,#53
	ldl	rr2,rr12
	add	r3,#53
	incb	@rr2,#1
L153:
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#2048
	testl	rr2
	jpr	ne,L155
	cp	r11,#92
	jpr	ne,L156
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#2048
	ldl	@rr2,rr4
L156:
	ldb	rl2,rr12(#58)
	subb	rh2,rh2
	cp	r11,r2
	jpr	ne,L157
	bit	r10,#4
	jpr	eq,L157
	bit	r10,#3
	jpr	eq,L158
	ldl	rr6,rr12
	ldl	rr4,#8
	sub	r3,r3
	callr	_ttxput
L158:
	set	r10,#3
	ldl	rr6,rr12
	ldl	rr4,#32
	sub	r3,r3
	callr	_ttxput
	ldk	r11,#8
	jpr	L150
L157:
	ldb	rl2,rr12(#59)
	subb	rh2,rh2
	cp	r11,r2
	jpr	ne,L160
	bit	r10,#5
	jpr	eq,L160
	bit	r10,#3
	jpr	eq,L161
	bit	r10,#13
	jpr	eq,L161
	ldl	rr2,#L162
	ldl	|_stkseg+~L1+10|(fp),rr2
	jpr	L165
L20027:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	ldb	rl6,@rr4
	ldb	rl5,rl6
	extsb	r5
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
L165:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	ne,L20027
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	sub	r5,r5
	call	@rr2
L161:
	ld	r5,r11
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
	set	r10,#3
	ldk	r11,#10
	jpr	L150
L160:
	ldb	rl2,rr12(#60)
	subb	rh2,rh2
	cp	r11,r2
	jpr	ne,L150
	bit	r10,#3
	jpr	eq,L168
	bit	r10,#13
	jpr	eq,L168
	ldl	rr2,#L169
	ldl	|_stkseg+~L1+10|(fp),rr2
	jpr	L172
L20029:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	ldb	rl6,@rr4
	ldb	rl5,rl6
	extsb	r5
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
L172:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	ne,L20029
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	sub	r5,r5
	call	@rr2
L168:
	res	r10,#3
	ldl	rr2,rr12
	add	r3,#53
	ldl	rr2,rr12
	add	r3,#53
	incb	@rr2,#1
	jpr	L150
L155:
	cp	r11,#92
	jpr	ne,L10005
	bit	r10,#2
	jpr	eq,L150
L10005:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#63487
	ldl	@rr2,rr4
L150:
	bit	r10,#3
	jpr	eq,L132
	ld	r5,r11
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
	callr	_ttxput
L20035:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+14|(fp),rr2
	ldl	rr6,rr12
	sub	r5,r5
	call	@rr2
L132:
	ld	r2,|_stkseg+~L1|(fp)
	dec	|_stkseg+~L1|(fp),#1
	test	r2
	jpr	eq,L131
	ldk	r2,#0
	ldb	rl2,@rr8
	ld	r11,r2
	inc	r9,#1
	ld	r2,rr12(#44)
	ld	r10,r2
	bit	r2,#0
	jpr	ne,L20020
L134:
	bit	r10,#1
	jpr	eq,L150
	cp	r11,#10
	jpr	ne,L151
	bit	r10,#6
	jpr	eq,L20025
	set	r10,#3
	jpr	L20025
L177:
	ldb	rl2,rr12(#61)
	testb	rl2
	jpr	eq,L176
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#1024
	testl	rr2
	jpr	ne,L176
	ldl	rr6,rr12
	callr	_tttimeo
L176:
	ldb	rl2,rr12(#53)
	testb	rl2
	jpr	eq,L10000
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#128
	testl	rr2
	jpr	eq,L10000
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65407
	ldl	@rr2,rr4
	ldl	rr6,rr12
	callr	_wakeup
L10000:
	ldm	r8,_stkseg+18(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 30
	}	/* _ttin */

.psec data
.data

L196:
.byte	%28,%7b,%29,%7d,%21,%7c,%5e,%7e
.byte	%27,%60,%5c,%5c,%0
.even
.even
/*~~ttxput:*/

.psec
.code

_ttxput::
{
	.psec	data
	.data
	.even
L243:
	.long	L204
	.long	L202
	.long	L206
	.long	L209
	.long	L216
	.long	L228
	.long	L230
	.long	L240
	.psec
	.code
	sub	fp,#~L2
	ldm	_stkseg+22(fp),r8,#6
	ldm	|_stkseg+~L1+0|(fp),r3,#3
	ldl	rr12,rr6
	ld	r2,rr12(#40)
	ld	r10,r2
	ldl	rr6,rr12(#46)
	ldk	r6,#0
	and	r7,#512
	testl	rr6
	jpr	eq,L185
	res	r10,#0
L185:
	test	|_stkseg+~L1|(fp)
	jpr	ne,L186
	ld	r2,rr12(#20)
	cp	r2,#256
	jpr	ge,L10006
	inc	|_stkseg+~L1|(fp),#1
	bit	r10,#0
	jpr	ne,L188
	ldl	rr2,_sysinfo+108
	addl	rr2,#1
	ldl	_sysinfo+108,rr2
	ldl	rr4,rr12
	add	r5,#20
	ldb	rl7,|_stkseg+~L1+5|(fp)
	extsb	r7
	callr	_putc
	jpr	L10006
L188:
	ldl	rr2,#_stkseg+~L1+5
	add	r3,fp
	ldl	rr8,rr2
	subl	rr2,rr2
	jpr	L20039
L186:
	bit	r10,#0
	jpr	ne,L190
	ld	r3,|_stkseg+~L1|(fp)
	exts	rr2
	addl	rr2,_sysinfo+108
	ldl	_sysinfo+108,rr2
	ldl	rr4,rr12
	add	r5,#20
	ldl	rr6,|_stkseg+~L1+2|(fp)
	callr	_putcb
	jpr	L10006
L190:
	ldl	rr2,|_stkseg+~L1+2|(fp)
	inc	r3,#6
	ldl	rr4,|_stkseg+~L1+2|(fp)
	ldb	rl6,rr4(#4)
	extsb	r6
	add	r3,r6
	ldl	rr8,rr2
	ldl	rr2,rr4
L20039:
	ldl	|_stkseg+~L1+18|(fp),rr2
	jpr	L191
L20041:
	cp	r11,#128
	jpr	ne,L194
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,#128
	callr	_putc
L194:
	ldl	rr2,_sysinfo+108
	addl	rr2,#1
	ldl	_sysinfo+108,rr2
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,r11
L20044:
	callr	_putc
	jpr	L191
L20045:
	ldk	r2,#0
	ldb	rl2,@rr8
	ld	r11,r2
	inc	r9,#1
	cp	r11,#128
	jpr	ge,L20041
	ld	r2,rr12(#44)
	bit	r2,#2
	jpr	eq,L195
	ldl	rr2,#L196
	ldl	|_stkseg+~L1+10|(fp),rr2
L197:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	testb	@rr2
	jpr	eq,L198
	ldl	rr2,|_stkseg+~L1+10|(fp)
	inc	|_stkseg+~L1+12|(fp),#1
	ldb	rl4,@rr2
	extsb	r4
	cp	r11,r4
	jpr	ne,L197
	ldl	rr2,rr12
	add	r3,#44
	res	@rr2,#2
	ldl	rr6,rr12
	ldl	rr4,#92
	sub	r3,r3
	callr	_ttxput
	ldl	rr2,rr12
	add	r3,#44
	set	@rr2,#2
	ldl	rr2,|_stkseg+~L1+10|(fp)
	ldb	rl4,rr2(#-2)
	extsb	r4
	ld	r11,r4
L198:
	cp	r11,#65
	jpr	lt,L195
	cp	r11,#90
	jpr	gt,L195
	ldl	rr2,rr12
	add	r3,#44
	res	@rr2,#2
	ldl	rr6,rr12
	ldl	rr4,#92
	sub	r3,r3
	callr	_ttxput
	ldl	rr2,rr12
	add	r3,#44
	set	@rr2,#2
L195:
	bit	r10,#1
	jpr	eq,L201
	cp	r11,#97
	jpr	lt,L201
	cp	r11,#122
	jpr	gt,L201
	add	r11,#-32
L201:
	ld	|_stkseg+~L1+16|(fp),r11
	ldb	rl2,_partab(r11)
	extsb	r2
	ld	|_stkseg+~L1+14|(fp),r2
	ldl	rr2,rr12
	add	r3,#54
	ldl	|_stkseg+~L1+10|(fp),rr2
	ldk	r11,#0
	ld	r3,|_stkseg+~L1+14|(fp)
	and	r3,#63
	ld	r2,r3
	cp	r3,#7
	jpr	ugt,L202
	rl	r2,#2
	ldl	rr4,L243(r2)
	jp	@rr4
L204:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	incb	@rr4,#1
	jpr	L202
L20043:
	ld	|_stkseg+~L1+16|(fp),#127
	jpr	L247
L206:
	bit	r10,#13
	jpr	eq,L207
	ldk	r11,#2
L207:
	ldl	rr4,|_stkseg+~L1+10|(fp)
	testb	@rr4
	jpr	eq,L202
	decb	@rr4,#1
	jpr	L202
L209:
	bit	r10,#5
	jpr	ne,L211
	bit	r10,#2
	jpr	eq,L214
	bit	r10,#4
	jpr	eq,L10009
	ldl	rr4,|_stkseg+~L1+10|(fp)
	testb	@rr4
	jpr	eq,L211
L10009:
	ldl	rr4,_sysinfo+108
	addl	rr4,#1
	ldl	_sysinfo+108,rr4
	ldl	rr4,rr12
	add	r5,#20
	ldk	r7,#13
	callr	_putc
	jpr	L211
L20051:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	eq,L236
	ldb	rl7,@rr2
	extsb	r7
	sra	r7,#4
	inc	r7,#3
	ldk	r6,#6
	callr	_max
	ld	r11,r2
	jpr	L236
L216:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	ldb	rl4,@rr2
	extsb	r4
	and	r4,#7
	ld	r5,r4
	neg	r5
	add	r5,#8
	ld	r11,r5
	ld	r4,r5
	addb	rl4,@rr2
	ldb	@rr2,rl4
	ld	r4,r10
	and	r4,#6144
	ld	|_stkseg+~L1+14|(fp),r4
	test	r4
	jpr	eq,L20047
	cp	r4,#2048
	jpr	eq,L20049
	cp	r4,#4096
	jpr	ne,L222
	ldk	r11,#2
	jpr	L202
L20049:
	cp	r11,#5
	jpr	ge,L202
L20047:
	ldk	r11,#0
	jpr	L202
L222:
	cp	|_stkseg+~L1+14|(fp),#6144
	jpr	ne,L202
	ld	r3,r11
	exts	rr2
	addl	rr2,_sysinfo+108
	ldl	_sysinfo+108,rr2
L227:
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,#32
	callr	_putc
	djnz	r11,L227
	jpr	L191
L228:
	bit	r10,#14
	jpr	ne,L20046
	jpr	L202
L230:
	bit	r10,#3
	jpr	eq,L231
	ld	|_stkseg+~L1+16|(fp),#10
L214:
	bit	r10,#8
	jpr	eq,L202
	ldk	r11,#5
	jpr	L202
L231:
	bit	r10,#4
	jpr	eq,L211
	ldl	rr2,|_stkseg+~L1+10|(fp)
	testb	@rr2
	jpr	eq,L191
L211:
	ld	r2,r10
	and	r2,#1536
	ld	|_stkseg+~L1+14|(fp),r2
	cp	r2,#512
	jpr	eq,L20051
	cp	r2,#1024
	jpr	ne,L237
	ldk	r11,#5
	jpr	L236
L237:
	cp	|_stkseg+~L1+14|(fp),#1536
	jpr	ne,L236
	ldk	r11,#9
L236:
	ldl	rr2,|_stkseg+~L1+10|(fp)
	clrb	@rr2
	jpr	L202
L240:
	bit	r10,#15
	jpr	eq,L202
L20046:
	ld	r11,#127
L202:
	ldl	rr2,_sysinfo+108
	addl	rr2,#1
	ldl	_sysinfo+108,rr2
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,|_stkseg+~L1+16|(fp)
	callr	_putc
	test	r11
	jpr	eq,L191
	cp	r11,#32
	jpr	ge,L245
	bit	r10,#6
	jpr	eq,L245
	bit	r10,#7
	jpr	ne,L20043
	clr	|_stkseg+~L1+16|(fp)
L247:
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,|_stkseg+~L1+16|(fp)
	callr	_putc
	cp	r11,#3
	jpr	le,L191
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,|_stkseg+~L1+16|(fp)
	jpr	L20044
L245:
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,#128
	callr	_putc
	ldl	rr4,rr12
	add	r5,#20
	ld	r7,r11
	or	r7,#128
	jpr	L20044
L191:
	ld	r2,|_stkseg+~L1|(fp)
	dec	|_stkseg+~L1|(fp),#1
	test	r2
	jpr	ne,L20045
	ldl	rr2,|_stkseg+~L1+18|(fp)
	testl	rr2
	jpr	eq,L10006
	ldl	rr6,rr2
	callr	_putcf
L10006:
	ldm	r8,_stkseg+22(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 34
	}	/* _ttxput */

.psec data
.data

/*~~ttout:*/

.psec
.code

_ttout::
{
	dec	fp,#~L2
	ldl	rr2,rr6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ttout */

.psec data
.data

/*~~tttimeo:*/

.psec
.code

_tttimeo::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r12,#2
	ldl	rr12,rr6
	ldl	rr2,rr6
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#64511
	ldl	@rr2,rr4
	ld	r2,rr12(#44)
	bit	r2,#1
	jpr	ne,L10013
	ldb	rl2,rr12(#61)
	testb	rl2
	jpr	eq,L10013
	test	@rr12
	jpr	ne,L257
	ldb	rl2,rr12(#60)
	testb	rl2
	jpr	ne,L10013
L257:
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#4096
	testl	rr2
	jpr	eq,L258
	ldl	rr2,rr12
	add	r3,#53
	ldb	@rr2,#1
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#128
	testl	rr2
	jpr	eq,L10013
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65407
	ldl	@rr2,rr4
	ldl	rr6,rr12
	callr	_wakeup
	jpr	L10013
L258:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#5120
	ldl	@rr2,rr4
	ldb	rl3,rr12(#61)
	subb	rh3,rh3
	ld	r5,r3
	mult	rr4,#6
	ld	r3,r5
	ldl	rr4,rr12
	ldl	rr6,#_tttimeo
	callr	_timeout
L10013:
	ldm	r12,_stkseg+4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _tttimeo */

.psec data
.data

/*~~ttioctl:*/

.psec
.code

_ttioctl::
{
	sub	fp,#~L2
	ldm	_stkseg+14(fp),r12,#2
	ldm	|_stkseg+~L1+0|(fp),r2,#4
	ldl	rr12,rr6
	ld	r4,r5
	cp	r5,#17410
	jpr	ne,L10016
	ld	r5,rr12(#44)
	ldk	r2,#0
	ld	r3,r5
	ld	r7,|_stkseg+~L1+2|(fp)
	xor	r2,|_stkseg+~L1|(fp)
	xor	r3,r7
	ld	|_stkseg+~L1+12|(fp),r3
	bit	r3,#1
	jpr	eq,L10016
	callr	_dvi
	ld	r2,rr12(#10)
	test	r2
	jpr	eq,L269
	test	@rr12
	jpr	eq,L270
	ldl	rr2,rr12
	inc	r3,#10
	ld	r4,@rr2
	add	r4,@rr12
	ld	@rr2,r4
	ldl	rr2,rr12(#2)
	ldl	rr4,rr12(#16)
	ldl	@rr4,rr2
	ldl	rr2,rr12(#6)
	ldl	rr4,rr12
	inc	r5,#16
	ldl	@rr4,rr2
L270:
	ldl	rr2,rr12
	inc	r3,#10
	ldl	rr6,rr12
	ldk	r4,#5
	ldir	@rr6,@rr2,r4
	sub	r3,#10
	ldl	rr2,#_ttnulq
	ldl	rr4,rr12
	inc	r5,#10
	ldk	r6,#5
	ldir	@rr4,@rr2,r6
	sub	r3,#10
L269:
	ldl	rr2,rr12
	add	r3,#53
	ld	r4,@rr12
	ldb	@rr2,rl4
	callr	_evi
L10016:
	ldm	r12,_stkseg+14(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _ttioctl */

.psec data
.data

