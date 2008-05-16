.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_ttywstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%74
.byte	%74
.byte	%79
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
.byte	%32
.byte	%2b
.byte	%20
.byte	%9
.byte	%30
.byte	%32
.byte	%2f
.byte	%30
.byte	%37
.byte	%2f
.byte	%38
.byte	%34
.byte	%20
.byte	%31
.byte	%33
.byte	%3a
.byte	%35
.byte	%30
.byte	%3a
.byte	%35
.byte	%39
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
.even
_tthiwat::
.word	%0
.word	%3c
.word	%3c
.word	%3c
.word	%3c
.word	%3c
.word	%3c
.word	%78
.word	%78
.word	%b4
.word	%b4
.word	%f0
.word	%f0
.word	%f0
.word	%64
.word	%64
.even
_ttlowat::
.word	%0
.word	%14
.word	%14
.word	%14
.word	%14
.word	%14
.word	%14
.word	%28
.word	%28
.word	%3c
.word	%3c
.word	%50
.word	%50
.word	%50
.word	%32
.word	%32
_ttcchar::
.byte	%7f
.byte	%1c
.byte	%8
.byte	%18
.byte	%4
.byte	%0
.byte	%0
.byte	%0
.even
_noterm:
.word	0
.comm	10,	_ttnulq;
_maptab::
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
.byte	%0
.byte	%0
.byte	%7c
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%60
.byte	%7b
.byte	%7d
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
.byte	%7e
.byte	%0
.byte	%0
.byte	%41
.byte	%42
.byte	%43
.byte	%44
.byte	%45
.byte	%46
.byte	%47
.byte	%48
.byte	%49
.byte	%4a
.byte	%4b
.byte	%4c
.byte	%4d
.byte	%4e
.byte	%4f
.byte	%50
.byte	%51
.byte	%52
.byte	%53
.byte	%54
.byte	%55
.byte	%56
.byte	%57
.byte	%58
.byte	%59
.byte	%5a
.byte	%0
.byte	%0
.byte	%0
.byte	%0
.byte	%0
/*~~ttopen:*/

.psec
.code

_ttopen::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldl	rr12,rr6
	ldl	rr10,_u+32
	ldl	rr2,rr12(#46)
	and	r2,#2
	ldk	r3,#0
	testl	rr2
	jpr	eq,L96
	ldb	_u+21,#13
	jpr	L10000
L96:
	ldl	rr2,rr10
	inc	r3,#12
	ld	r4,rr10(#14)
	cp	r4,@rr2
	jpr	ne,L97
	ldl	rr2,_u+478
	testl	rr2
	jpr	ne,L97
	ld	r2,rr12(#50)
	test	r2
	jpr	ne,L97
	ldl	_u+478,rr12
	ld	r2,rr10(#12)
	ldl	rr4,rr12
	add	r5,#50
	ld	@rr4,r2
L97:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65533
	ldl	@rr2,rr4
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#4
	ldl	@rr2,rr4
L10000:
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ttopen */

.psec data
.data

/*~~ttclose:*/

.psec
.code

_ttclose::
{
	dec	fp,#~L2
	ldm	_stkseg+8(fp),r12,#2
	ldl	rr12,rr6
	callr	_dvi
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+4|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#3
	call	@rr2
	callr	_evi
	ldl	rr6,rr12
	ldk	r5,#1
	callr	_ttyflush
	ldl	rr6,rr12
	callr	_ttywait
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65529
	ldl	@rr2,rr4
	ldm	r12,_stkseg+8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ttclose */

.psec data
.data

.even
.even
L149:
.word	0
.word	1
.word	2
.even
.even
L213:
.word	21505
.word	21506
.word	21507
.word	21508
.word	21509
.word	21510
.word	21511
.word	21517
.word	21518
.word	29698
.word	29704
.word	29705
.word	29706
.word	29712
.word	29713
.word	29714
.word	65280
.even
/*~~ttiocom:*/

.psec
.code

_ttiocom::
{
	.psec	data
	.data
	.even
L138:
	.long	L133
	.long	L134
	.long	L135
	.long	L136
	.even
L150:
	.long	L147
	.long	L147
	.long	L147
	.even
L214:
	.long	L127
	.long	L112
	.long	L110
	.long	L110
	.long	L139
	.long	L130
	.long	L142
	.long	L202
	.long	L203
	.long	L151
	.long	L182
	.long	L152
	.long	L153
	.long	L141
	.long	L204
	.long	L208
	.long	L108
	.psec
	.code
	sub	fp,#~L2
	ldm	_stkseg+54(fp),r9,#5
	ldm	|_stkseg+~L1+0|(fp),r2,#4
	ldl	rr12,rr6
	ld	r4,r5
	ldl	rr2,#L213
	ld	r5,#17
	cpir	r4,@rr2,r5,eq
	jpr	ne,L210
	sll	r5,#2
	neg	r5
	ldl	rr2,L214+64(r5)
	jp	@rr2
L108:
	ld	_u+40,#21504
L106:
	sub	r2,r2
	jpr	L10006
L110:
	ldl	rr6,rr12
	callr	_ttywait
	cp	|_stkseg+~L1+6|(fp),#21508
	jpr	ne,L112
	ldl	rr6,rr12
	ldk	r5,#3
	callr	_ttyflush
L112:
	ldl	rr4,#_stkseg+~L1+14
	add	r5,fp
	ldl	rr6,|_stkseg+~L1|(fp)
	ld	r3,#18
	callr	_copyin
	test	r2
	jpr	ne,L114
	ldb	rl2,rr12(#52)
	cpb	rl2,|_stkseg+~L1+22|(fp)
	jpr	eq,L115
	cpb	|_stkseg+~L1+22|(fp),#0
	jpr	lt,L10009
	ldb	rl2,|_stkseg+~L1+22|(fp)
	extsb	r2
	cp	r2,_linecnt
	jpr	lt,L116
L10009:
	ldb	_u+21,#22
	jpr	L106
L116:
	ldb	rl2,rr12(#52)
	extsb	r2
	ld	r5,r2
	mult	rr4,#36
	ldl	rr2,_linesw+16(r5)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ld	r4,_stkseg+~L2+4(fp)
	ld	|_stkseg|(fp),r4
	ldl	rr6,rr12
	ld	r5,#17409
	subl	rr2,rr2
	ldl	_stkseg+~L1+48(fp),rr8
	ldl	rr8,_stkseg+~L1+44(fp)
	call	@rr8
	ldl	rr8,_stkseg+~L1+48(fp)
L115:
	ld	r2,rr12(#44)
	ld	r9,r2
	ldk	r10,#0
	ld	r2,rr12(#42)
	and	r2,#15
	ld	r3,|_stkseg+~L1+18|(fp)
	and	r3,#15
	cp	r2,r3
	jpr	eq,L117
	inc	r10,#1
L117:
	ld	r2,rr12(#42)
	and	r2,#48
	ld	r3,|_stkseg+~L1+18|(fp)
	and	r3,#48
	cp	r2,r3
	jpr	eq,L118
	inc	r10,#1
L118:
	ld	r2,rr12(#42)
	xor	r2,|_stkseg+~L1+18|(fp)
	and	r2,#3008
	jpr	eq,L119
	inc	r10,#1
L119:
	ld	r2,rr12(#38)
	xor	r2,|_stkseg+~L1+14|(fp)
	bit	r2,#0
	jpr	eq,L120
	inc	r10,#1
L120:
	ldl	rr2,rr12
	add	r3,#38
	ld	r4,|_stkseg+~L1+14|(fp)
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#40
	ld	r4,|_stkseg+~L1+16|(fp)
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#42
	ld	r4,|_stkseg+~L1+18|(fp)
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#44
	ld	r4,|_stkseg+~L1+20|(fp)
	ld	@rr2,r4
	ldl	rr6,#_stkseg+~L1+23
	add	r7,fp
	ldl	rr4,rr12
	add	r5,#56
	ldk	r3,#8
	callr	_bcopy
	ldb	rl2,rr12(#52)
	cpb	rl2,|_stkseg+~L1+22|(fp)
	jpr	eq,L122
	ldl	rr2,rr12
	add	r3,#52
	ldb	rl4,|_stkseg+~L1+22|(fp)
	ldb	@rr2,rl4
	ldb	rl2,rr12(#52)
	extsb	r2
	ld	r5,r2
	mult	rr4,#36
	ldl	rr2,_linesw+16(r5)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ld	r4,_stkseg+~L2+4(fp)
	ld	|_stkseg|(fp),r4
	ldl	rr6,rr12
	ld	r5,#17408
	subl	rr2,rr2
	jpr	L20003
L122:
	ldl	rr2,rr12
	add	r3,#44
	cp	r9,@rr2
	jpr	eq,L123
	ldb	rl2,rr12(#52)
	extsb	r2
	ld	r5,r2
	mult	rr4,#36
	ldl	rr2,_linesw+16(r5)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ld	r4,_stkseg+~L2+4(fp)
	ld	|_stkseg|(fp),r4
	ld	r3,r9
	exts	rr2
	ld	r5,#17410
	ldl	rr6,rr12
L20003:
	ldl	_stkseg+~L1+48(fp),rr8
	ldl	rr8,_stkseg+~L1+44(fp)
	call	@rr8
	ldl	rr8,_stkseg+~L1+48(fp)
L123:
	test	r10
	jpr	eq,L106
L20005:
	ldk	r2,#1
	jpr	L10006
L127:
	ld	r2,rr12(#38)
	ld	|_stkseg+~L1+14|(fp),r2
	ld	r2,rr12(#40)
	ld	|_stkseg+~L1+16|(fp),r2
	ld	r2,rr12(#42)
	ld	|_stkseg+~L1+18|(fp),r2
	ld	r2,rr12(#44)
	ld	|_stkseg+~L1+20|(fp),r2
	ldb	rl2,rr12(#52)
	ldb	|_stkseg+~L1+22|(fp),rl2
	ldl	rr6,rr12
	add	r7,#56
	ldl	rr4,#_stkseg+~L1+23
	add	r5,fp
	ldk	r3,#8
	callr	_bcopy
	ldl	rr6,#_stkseg+~L1+14
	add	r7,fp
	ldl	rr4,|_stkseg+~L1|(fp)
	ld	r3,#18
	callr	_copyout
	test	r2
	jpr	eq,L106
L114:
	ldb	_u+21,#14
	jpr	L106
L130:
	ld	r2,|_stkseg+~L1+2|(fp)
	cp	r2,#3
	jpr	ugt,L10009
	rl	r2,#2
	ldl	rr4,L138(r2)
	jp	@rr4
L133:
	ldl	rr4,rr12(#34)
	ldl	|_stkseg+~L1+44|(fp),rr4
	ldl	rr6,rr12
	ldk	r5,#2
L20007:
	ldl	rr2,|_stkseg+~L1+44|(fp)
	call	@rr2
	jpr	L106
L134:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#3
	jpr	L20007
L135:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#4
	jpr	L20007
L136:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#5
	jpr	L20007
L139:
	ldl	rr6,rr12
	callr	_ttywait
	ldl	rr2,|_stkseg+~L1|(fp)
	testl	rr2
	jpr	ne,L106
	ldl	rr4,rr12(#34)
	ldl	|_stkseg+~L1+44|(fp),rr4
	ldl	rr6,rr12
	ldk	r5,#8
	jpr	L20007
L141:
	ldl	rr6,rr12
	ldk	r5,#3
L20011:
	callr	_ttyflush
	jpr	L106
L142:
	ld	r2,|_stkseg+~L1+2|(fp)
	ldl	rr4,#L149
	ld	r6,#3
	cpir	r2,@rr4,r6,eq
	jpr	ne,L10009
	sll	r6,#2
	neg	r6
	ldl	rr4,L150+8(r6)
	jp	@rr4
L147:
	ld	r5,|_stkseg+~L1+2|(fp)
	inc	r5,#1
	and	r5,#3
	ldl	rr6,rr12
	jpr	L20011
L151:
	ldl	rr2,rr12
	add	r3,#42
	set	@rr2,#10
	jpr	L106
L152:
	ldl	rr6,rr12
	callr	_ttywait
	ldl	rr6,rr12
	ldk	r5,#3
	callr	_ttyflush
L153:
	ldl	rr4,#_stkseg+~L1+32
	add	r5,fp
	ldl	rr6,|_stkseg+~L1|(fp)
	ldk	r3,#6
	callr	_copyin
	test	r2
	jpr	ne,L114
	ld	r2,rr12(#42)
	ld	r11,r2
	ld	r2,rr12(#38)
	ld	|_stkseg+~L1+12|(fp),r2
	ldl	rr2,rr12
	add	r3,#38
	clr	@rr2
	ldl	rr2,rr12
	add	r3,#40
	clr	@rr2
	ldl	rr2,rr12
	add	r3,#44
	clr	@rr2
	ldl	rr2,rr12
	add	r3,#42
	ldb	rl4,|_stkseg+~L1+32|(fp)
	extsb	r4
	and	r4,#15
	or	r4,#128
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#42
	ld	r4,@rr2
	or	r4,#2112
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#58
	ldb	rl4,|_stkseg+~L1+34|(fp)
	ldb	@rr2,rl4
	ldl	rr2,rr12
	add	r3,#59
	ldb	rl4,|_stkseg+~L1+35|(fp)
	ldb	@rr2,rl4
	ld	r9,|_stkseg+~L1+36|(fp)
	ld	r2,r9
	and	r2,#3072
	jpr	eq,L155
	bit	r9,#10
	jpr	eq,L156
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#11
L156:
	bit	r9,#11
	jpr	eq,L155
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#12
L155:
	bit	r9,#2
	jpr	eq,L158
	ldl	rr2,rr12
	add	r3,#38
	set	@rr2,#9
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#1
	ldl	rr2,rr12
	add	r3,#44
	set	@rr2,#2
L158:
	bit	r9,#3
	jpr	eq,L159
	ldl	rr2,rr12
	add	r3,#44
	set	@rr2,#3
L159:
	bit	r9,#4
	jpr	eq,L160
	ldl	rr2,rr12
	add	r3,#38
	set	@rr2,#8
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#2
L160:
	bit	r9,#12
	jpr	eq,L161
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#9
L161:
	bit	r9,#13
	jpr	eq,L162
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#10
L162:
	bit	r9,#8
	jpr	eq,L163
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#8
L163:
	bit	r9,#9
	jpr	eq,L164
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#8
L164:
	bit	r9,#5
	jpr	eq,L165
	ldl	rr2,rr12
	add	r3,#61
	ldb	@rr2,#6
	ldl	rr2,rr12
	add	r3,#60
	ldb	@rr2,#1
	ldl	rr2,rr12
	add	r3,#38
	ld	r4,@rr2
	and	r4,#-769
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#42
	ld	r4,@rr2
	or	r4,#48
	jpr	L20004
L165:
	bit	r9,#1
	jpr	eq,L167
	ldl	rr2,rr12
	add	r3,#61
	ldb	@rr2,#6
	ldl	rr2,rr12
	add	r3,#60
	ldb	@rr2,#1
	ldl	rr2,rr12
	add	r3,#38
	ld	r4,@rr2
	or	r4,#1057
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#0
	ldl	rr2,rr12
	add	r3,#42
	ld	r4,@rr2
	or	r4,#1072
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#44
	ld	r4,@rr2
	or	r4,#113
	jpr	L20004
L167:
	ldl	rr2,rr12
	add	r3,#60
	ldb	@rr2,#4
	ldl	rr2,rr12
	add	r3,#61
	clrb	@rr2
	ldl	rr2,rr12
	add	r3,#38
	ld	r4,@rr2
	or	r4,#1057
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#0
	ldl	rr2,rr12
	add	r3,#42
	ld	r4,@rr2
	or	r4,#1072
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#44
	ld	r4,@rr2
	or	r4,#115
L20004:
	ld	@rr2,r4
	ldl	rr2,rr12
	add	r3,#38
	set	@rr2,#4
	ldl	rr2,rr12
	add	r3,#42
	set	@rr2,#8
	bit	r9,#6
	jpr	eq,L169
	bit	r9,#7
	jpr	eq,L170
	ldl	rr2,rr12
	add	r3,#38
	res	@rr2,#4
	ldl	rr2,rr12
	add	r3,#42
	res	@rr2,#8
	jpr	L169
L170:
	ldl	rr2,rr12
	add	r3,#42
	set	@rr2,#9
L169:
	bit	r9,#14
	jpr	eq,L172
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#15
L172:
	bit	r9,#15
	jpr	eq,L173
	ldl	rr2,rr12
	add	r3,#40
	set	@rr2,#13
L173:
	bit	r9,#0
	jpr	eq,L174
	ldl	rr2,rr12
	add	r3,#38
	set	@rr2,#12
L174:
	bit	|_stkseg+~L1+12|(fp),#13
	jpr	eq,L175
	ldl	rr2,rr12
	add	r3,#38
	set	@rr2,#13
L175:
	ldk	r10,#0
	ld	r2,rr12(#42)
	and	r2,#15
	ld	r3,r11
	and	r3,#15
	cp	r2,r3
	jpr	eq,L176
	inc	r10,#1
L176:
	ld	r2,rr12(#42)
	and	r2,#48
	ld	r3,r11
	and	r3,#48
	cp	r2,r3
	jpr	eq,L177
	inc	r10,#1
L177:
	ld	r2,rr12(#42)
	xor	r2,r11
	and	r2,#2496
	jpr	eq,L178
	inc	r10,#1
L178:
	ld	r2,rr12(#38)
	xor	r2,|_stkseg+~L1+12|(fp)
	bit	r2,#0
	jpr	eq,L179
	inc	r10,#1
L179:
	test	r10
	jpr	ne,L20005
	jpr	L106
L182:
	ld	r2,rr12(#42)
	and	r2,#15
	ldb	|_stkseg+~L1+32|(fp),rl2
	ldb	|_stkseg+~L1+33|(fp),rl2
	ldb	rl3,rr12(#58)
	ldb	|_stkseg+~L1+34|(fp),rl3
	ldb	rl3,rr12(#59)
	ldb	|_stkseg+~L1+35|(fp),rl3
	ldk	r9,#0
	ld	r3,rr12(#44)
	bit	r3,#1
	jpr	ne,L183
	bit	r3,#0
	jpr	ne,L183
	set	r9,#5
L183:
	ld	r2,rr12(#44)
	bit	r2,#1
	jpr	ne,L184
	bit	r2,#0
	jpr	eq,L184
	set	r9,#1
L184:
	ld	r2,rr12(#44)
	bit	r2,#2
	jpr	eq,L185
	set	r9,#2
L185:
	ld	r2,rr12(#44)
	bit	r2,#3
	jpr	eq,L186
	set	r9,#3
L186:
	ld	r2,rr12(#42)
	bit	r2,#9
	jpr	eq,L187
	set	r9,#6
	jpr	L188
L187:
	ld	r2,rr12(#38)
	bit	r2,#4
	jpr	eq,L189
	set	r9,#7
	jpr	L188
L189:
	or	r9,#192
L188:
	ld	r2,rr12(#40)
	bit	r2,#2
	jpr	eq,L191
	set	r9,#4
	ld	r2,rr12(#40)
	bit	r2,#9
	jpr	eq,L192
	set	r9,#12
L192:
	ld	r2,rr12(#40)
	bit	r2,#10
	jpr	eq,L194
	set	r9,#13
	jpr	L194
L191:
	ld	r2,rr12(#40)
	bit	r2,#9
	jpr	eq,L195
	set	r9,#8
L195:
	ld	r2,rr12(#40)
	bit	r2,#10
	jpr	eq,L194
	set	r9,#9
L194:
	ld	r2,rr12(#40)
	and	r2,#6144
	cp	r2,#6144
	jpr	eq,L10010
	ld	r2,rr12(#40)
	bit	r2,#11
	jpr	eq,L197
L10010:
	or	r9,#3072
L197:
	ld	r2,rr12(#40)
	bit	r2,#15
	jpr	eq,L198
	set	r9,#14
L198:
	ld	r2,rr12(#40)
	bit	r2,#13
	jpr	eq,L199
	set	r9,#15
L199:
	ld	r2,rr12(#38)
	bit	r2,#12
	jpr	eq,L200
	set	r9,#0
L200:
	ld	|_stkseg+~L1+36|(fp),r9
	ldl	rr6,#_stkseg+~L1+32
	add	r7,fp
	ldl	rr4,|_stkseg+~L1|(fp)
	ldk	r3,#6
	callr	_copyout
	test	r2
	jpr	ne,L114
	jpr	L106
L202:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r4,#2
L20012:
	ldl	@rr2,rr4
	jpr	L106
L203:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r4,#65533
	jpr	L20012
L204:
	ldl	rr4,#_stkseg+~L1+38
	add	r5,fp
	ldl	rr6,|_stkseg+~L1|(fp)
	ldk	r3,#6
	callr	_copyin
	test	r2
	jpr	ne,L114
	ldl	rr2,rr12
	add	r3,#56
	ldb	rl4,|_stkseg+~L1+38|(fp)
	ldb	@rr2,rl4
	ldl	rr2,rr12
	add	r3,#57
	ldb	rl4,|_stkseg+~L1+39|(fp)
	ldb	@rr2,rl4
	ld	r2,rr12(#44)
	bit	r2,#1
	jpr	eq,L106
	ldl	rr2,rr12
	add	r3,#60
	ldb	rl4,|_stkseg+~L1+42|(fp)
	ldb	@rr2,rl4
	jpr	L106
L208:
	ldb	rl2,rr12(#56)
	ldb	|_stkseg+~L1+38|(fp),rl2
	ldb	rl2,rr12(#57)
	ldb	|_stkseg+~L1+39|(fp),rl2
	ldb	rl2,rr12(#60)
	ldb	|_stkseg+~L1+42|(fp),rl2
	ldb	|_stkseg+~L1+40|(fp),#17
	ldb	|_stkseg+~L1+41|(fp),#19
	clrb	|_stkseg+~L1+43|(fp)
	ldl	rr6,#_stkseg+~L1+38
	add	r7,fp
	ldl	rr4,|_stkseg+~L1|(fp)
	ldk	r3,#6
	callr	_copyout
	test	r2
	jpr	ne,L114
	jpr	L106
L210:
	ld	r2,|_stkseg+~L1+6|(fp)
	and	r2,#65280
	cp	r2,#17408
	jpr	ne,L10009
	ldb	rl2,rr12(#52)
	extsb	r2
	ld	r5,r2
	mult	rr4,#36
	ldl	rr2,_linesw+16(r5)
	ldl	|_stkseg+~L1+44|(fp),rr2
	ld	r4,_stkseg+~L2+4(fp)
	ld	|_stkseg|(fp),r4
	ldl	rr6,rr12
	ld	r5,|_stkseg+~L1+6|(fp)
	ldl	rr2,|_stkseg+~L1|(fp)
	ldl	_stkseg+~L1+48(fp),rr8
	ldl	rr8,_stkseg+~L1+44(fp)
	call	@rr8
	ldl	rr8,_stkseg+~L1+48(fp)
	jpr	L106
L10006:
	ldm	r9,_stkseg+54(fp),#5
	add	fp,#~L2
	ret
	~L1 := 2
	~L2 := 64
	}	/* _ttiocom */

.psec data
.data

/*~~ttinit:*/

.psec
.code

_ttinit::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r12,#2
	ldl	rr12,rr6
	ldl	rr2,rr6
	add	r3,#52
	clrb	@rr2
	ldl	rr2,rr12
	add	r3,#38
	ld	@rr2,#1313
	ldl	rr2,rr12
	add	r3,#40
	ld	@rr2,#6149
	ldl	rr2,rr12
	add	r3,#42
	ld	@rr2,#1277
	ldl	rr2,rr12
	add	r3,#44
	ld	@rr2,#123
	ldl	rr4,rr12
	add	r5,#56
	ldl	rr6,#_ttcchar
	ldk	r3,#8
	callr	_bcopy
	ldm	r12,_stkseg+4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ttinit */

.psec data
.data

/*~~ttywait:*/

.psec
.code

_ttywait::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r12,#2
	ldl	rr12,rr6
	callr	_dvi
	jpr	L218
L10017:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#8192
	ldl	@rr2,rr4
	ldl	rr6,rr12
	add	r7,#40
	ld	r5,#29
	callr	_sleep
L218:
	ld	r2,rr12(#20)
	test	r2
	jpr	ne,L10017
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#33
	testl	rr2
	jpr	ne,L10017
	callr	_evi
	ldm	r12,_stkseg+4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ttywait */

.psec data
.data

/*~~ttyflush:*/

.psec
.code

_ttyflush::
{
	sub	fp,#~L2
	ldm	_stkseg+10(fp),r9,#5
	ld	|_stkseg+~L1+0|(fp),r5
	ldl	rr12,rr6
	bit	r5,#1
	jpr	ne,L223
	jpr	L222
L20015:
	ldl	rr6,rr10
	callr	_putcf
L223:
	ldl	rr6,rr12
	add	r7,#20
	callr	_getcb
	ldl	rr10,rr2
	testl	rr2
	jpr	ne,L20015
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+6|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#7
	call	@rr2
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#64
	testl	rr2
	jpr	eq,L226
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65471
	ldl	@rr2,rr4
	ldl	rr6,rr12
	add	r7,#20
	callr	_wakeup
L226:
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#8192
	testl	rr2
	jpr	eq,L222
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#57343
	ldl	@rr2,rr4
	ldl	rr6,rr12
	add	r7,#40
	callr	_wakeup
L222:
	bit	|_stkseg+~L1|(fp),#0
	jpr	ne,L230
	jpr	L10018
L20017:
	ldl	rr6,rr10
	callr	_putcf
L230:
	ldl	rr6,rr12
	inc	r7,#10
	callr	_getcb
	ldl	rr10,rr2
	testl	rr2
	jpr	ne,L20017
	callr	_dvi
	ld	r9,r2
	jpr	L232
L20019:
	ldl	rr6,rr10
	callr	_putcf
L232:
	ldl	rr6,rr12
	callr	_getcb
	ldl	rr10,rr2
	testl	rr2
	jpr	ne,L20019
	ldl	rr2,rr12
	add	r3,#53
	clrb	@rr2
	ld	r7,r9
	callr	_rvi
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+6|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#6
	call	@rr2
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#128
	testl	rr2
	jpr	eq,L10018
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#65407
	ldl	@rr2,rr4
	ldl	rr6,rr12
	callr	_wakeup
L10018:
	ldm	r9,_stkseg+10(fp),#5
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 20
	}	/* _ttyflush */

.psec data
.data

/*~~canon:*/

.psec
.code

_canon::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r8,#6
	ldl	rr12,rr6
	callr	_dvi
	test	@rr12
	jpr	ne,L239
	ldl	rr2,rr12
	add	r3,#53
	clrb	@rr2
	jpr	L239
L20021:
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#16
	testl	rr2
	jpr	ne,L241
L20025:
	callr	_evi
	jpr	L10021
L241:
	ld	r2,rr12(#44)
	bit	r2,#1
	jpr	ne,L242
	ldb	rl2,rr12(#60)
	testb	rl2
	jpr	ne,L242
	ldb	rl2,rr12(#61)
	testb	rl2
	jpr	eq,L240
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	and	r5,#61439
	ldl	@rr2,rr4
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#1024
	testl	rr2
	jpr	ne,L242
	ldl	rr6,rr12
	callr	_tttimeo
L242:
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#128
	ldl	@rr2,rr4
	ldl	rr6,rr12
	ld	r5,#28
	callr	_sleep
L239:
	ldb	rl2,rr12(#53)
	testb	rl2
	jpr	eq,L20021
L240:
	ld	r2,rr12(#44)
	bit	r2,#1
	jpr	ne,L246
	ldl	rr2,rr12
	inc	r3,#10
	ldl	rr6,rr12
	ldk	r4,#5
	ldir	@rr2,@rr6,r4
	sub	r7,#10
	ldl	rr4,#_ttnulq
	ldl	rr0,rr12
	ldl	rr6,rr12
	ldk	r2,#5
	ldir	@rr6,@rr4,r2
	sub	r5,#10
	ldl	rr2,rr12
	add	r3,#53
	clrb	@rr2
	jpr	L20025
L246:
	callr	_evi
	ldl	rr10,#_canonb
	ldk	r8,#0
	ldl	rr2,rr12(#46)
	and	r2,#4
	ld	r3,r8
	testl	rr2
	jpr	eq,L255
L250:
	ldl	rr6,rr12
	callr	_getc
	ld	r9,r2
	cp	r2,#0
	jpr	lt,L254
	ldb	rl2,rr12(#60)
	subb	rh2,rh2
	cp	r9,r2
	jpr	eq,L254
	ldl	rr4,rr12
	inc	r5,#10
	ld	r7,r9
	callr	_putc
	ldl	rr2,_sysinfo+104
	addl	rr2,#1
	ldl	_sysinfo+104,rr2
	cp	r9,#10
	jpr	eq,L254
	ldb	rl2,rr12(#61)
	subb	rh2,rh2
	cp	r9,r2
	jpr	ne,L250
L254:
	ldl	rr2,rr12
	add	r3,#53
	ldl	rr2,rr12
	add	r3,#53
	decb	@rr2,#1
	jpr	L10021
L258:
	ldb	rl2,rr12(#58)
	subb	rh2,rh2
	cp	r9,r2
	jpr	ne,L260
	cpl	rr10,#_canonb
	jpr	ule,L255
L20026:
	dec	r11,#1
	jpr	L255
L20024:
	ldl	rr10,#_canonb
L255:
	ldl	rr6,rr12
	callr	_getc
	ld	r9,r2
	cp	r2,#0
	jpr	lt,L256
	test	r8
	jpr	ne,L257
	cp	r2,#92
	jpr	ne,L258
	jpr	L20022
L260:
	ldb	rl2,rr12(#59)
	subb	rh2,rh2
	cp	r9,r2
	jpr	eq,L20024
	ldb	rl2,rr12(#60)
	subb	rh2,rh2
	cp	r9,r2
	jpr	ne,L264
L256:
	ld	r2,r11
	ld	r3,#^o _canonb
	sub	r2,r3
	ld	r9,r2
	ld	r5,r2
	exts	rr4
	addl	rr4,_sysinfo+104
	ldl	_sysinfo+104,rr4
	ldl	rr10,#_canonb
L275:
	ld	r2,r9
	dec	r9,#1
	test	r2
	jpr	eq,L254
	ldl	rr4,rr12
	inc	r5,#10
	ldl	rr2,rr10
	inc	r11,#1
	ldb	rl7,@rr2
	extsb	r7
	callr	_putc
	jpr	L275
L257:
	ldk	r8,#0
	ldb	rl2,rr12(#58)
	subb	rh2,rh2
	cp	r9,r2
	jpr	eq,L10026
	ldb	rl2,rr12(#59)
	subb	rh2,rh2
	cp	r9,r2
	jpr	eq,L10026
	ldb	rl2,rr12(#60)
	subb	rh2,rh2
	cp	r9,r2
	jpr	ne,L265
L10026:
	dec	r11,#1
	jpr	L264
L265:
	ld	r2,rr12(#44)
	bit	r2,#2
	jpr	eq,L267
	cp	r9,#128
	jpr	ge,L268
	testb	_maptab(r9)
	jpr	eq,L268
	dec	r11,#1
	ldb	rl2,_maptab(r9)
	extsb	r2
	ld	r9,r2
	jpr	L264
L268:
	cp	r9,#92
	jpr	ne,L264
	jpr	L255
L267:
	cp	r9,#92
	jpr	ne,L264
L20022:
	inc	r8,#1
L264:
	ld	r2,r9
	ldb	@rr10,rl2
	inc	r11,#1
	cp	r9,#10
	jpr	eq,L256
	ldb	rl2,rr12(#61)
	subb	rh2,rh2
	cp	r9,r2
	jpr	eq,L256
	ldl	rr2,#_canonb
	add	r3,_Canbsiz
	cpl	rr10,rr2
	jpr	ult,L255
	jpr	L20026
L10021:
	ldm	r8,_stkseg+4(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _canon */

.psec data
.data

/*~~ttrstrt:*/

.psec
.code

_ttrstrt::
{
	dec	fp,#~L2
	ldm	_stkseg+8(fp),r12,#2
	ldl	rr12,rr6
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+4|(fp),rr2
	ldk	r5,#1
	call	@rr2
	ldm	r12,_stkseg+8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ttrstrt */

.psec data
.data

/*~~ttread:*/

.psec
.code

_ttread::
{
	sub	fp,#~L2
	ldm	_stkseg+32(fp),r8,#6
	ldl	rr12,rr6
	ldl	rr2,rr6
	inc	r3,#10
	ldl	rr10,rr2
	test	@rr10
	jpr	ne,L283
	bit	_u+54,#2
	jpr	ne,L10031
	ldl	rr6,rr12
	callr	_canon
	jpr	L283
L20030:
	testb	_u+21
	jpr	ne,L284
	cp	_u+48,#24
	jpr	ult,L285
	ldl	rr6,rr10
	callr	_getcb
	ldl	|_stkseg+~L1+4|(fp),rr2
	testl	rr2
	jpr	eq,L284
	ldl	rr6,rr2
	ldb	rl2,rr6(#5)
	extsb	r2
	ldb	rl3,rr6(#4)
	extsb	r3
	sub	r2,r3
	ld	r6,r2
	ld	r7,_u+48
	callr	_min
	ld	r9,r2
	ldl	rr6,|_stkseg+~L1+4|(fp)
	inc	r7,#6
	ldl	rr2,|_stkseg+~L1+4|(fp)
	ldb	rl4,rr2(#4)
	extsb	r4
	add	r7,r4
	ldl	rr4,_u+44
	ld	r3,r9
	callr	_copyout
	test	r2
	jpr	eq,L288
	ldb	_u+21,#14
L288:
	ldl	rr6,|_stkseg+~L1+4|(fp)
	callr	_putcf
	jpr	L289
L285:
	ldk	r9,#0
	jpr	L290
L20028:
	ldl	rr6,rr10
	callr	_getc
	ld	r8,r2
	cp	r2,#0
	jpr	lt,L291
	ldl	rr2,#_stkseg+~L1+4
	add	r3,fp
	add	r3,r9
	ld	r4,r8
	ldb	@rr2,rl4
	inc	r9,#1
L290:
	cp	r9,_u+48
	jpr	ult,L20028
L291:
	test	r9
	jpr	eq,L284
	ldl	rr6,#_stkseg+~L1+4
	add	r7,fp
	ldl	rr4,_u+44
	ld	r3,r9
	callr	_copyout
	test	r2
	jpr	eq,L289
	ldb	_u+21,#14
L289:
	ld	r2,_u+46
	add	r2,r9
	ld	_u+46,r2
	ld	r2,_u+48
	sub	r2,r9
	ld	_u+48,r2
L283:
	test	_u+48
	jpr	ne,L20030
L284:
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#8
	testl	rr2
	jpr	eq,L10031
	cp	@rr12,#60
	jpr	ge,L10031
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+28|(fp),rr2
	ldl	rr6,rr12
	ldk	r5,#5
	call	@rr2
L10031:
	ldm	r8,_stkseg+32(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 44
	}	/* _ttread */

.psec data
.data

/*~~ttwrite:*/

.psec
.code

_ttwrite::
{
	sub	fp,#~L2
	ldm	_stkseg+10(fp),r8,#6
	ldl	rr12,rr6
	ldl	rr2,rr12(#46)
	ldk	r2,#0
	and	r3,#16
	testl	rr2
	jpr	ne,L299
	jpr	L10034
L20034:
	callr	_dvi
	jpr	L301
L20032:
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+4|(fp),rr2
	ldl	rr6,rr12
	sub	r5,r5
	call	@rr2
	ldl	rr2,rr12
	add	r3,#46
	ldl	rr4,@rr2
	or	r5,#64
	ldl	@rr2,rr4
	ldl	rr6,rr12
	add	r7,#20
	ld	r5,#29
	callr	_sleep
L301:
	ld	r2,rr12(#42)
	and	r2,#15
	add	r2,r2
	ldl	rr4,#_tthiwat
	add	r5,r2
	ld	r2,rr12(#20)
	cp	r2,@rr4
	jpr	gt,L20032
	callr	_evi
	cp	_u+48,#8
	jpr	ult,L303
	callr	_getcf
	ldl	rr8,rr2
	testl	rr2
	jpr	eq,L300
	ldb	rl6,rr8(#5)
	extsb	r6
	ld	r7,_u+48
	callr	_min
	ld	r11,r2
	ldl	rr4,rr8
	inc	r5,#6
	ldl	rr6,_u+44
	ld	r3,r11
	callr	_copyin
	test	r2
	jpr	eq,L305
	ldb	_u+21,#14
	ldl	rr6,rr8
	callr	_putcf
	jpr	L300
L305:
	ld	r2,_u+46
	add	r2,r11
	ld	_u+46,r2
	ld	r2,_u+48
	sub	r2,r11
	ld	_u+48,r2
	ldl	rr2,rr8
	inc	r3,#5
	ld	r4,r11
	ldb	@rr2,rl4
	ldl	rr6,rr12
	ldl	rr4,rr8
	ld	r3,r11
	jpr	L20035
L303:
	ldl	rr4,#_stkseg+~L1+5
	add	r5,fp
	ldl	rr6,_u+44
	inc	_u+46,#1
	callr	_fubyte
	test	r2
	jpr	eq,L309
	ldb	_u+21,#14
	jpr	L300
L309:
	dec	_u+48,#1
	ldb	rl5,|_stkseg+~L1+5|(fp)
	extsb	r5
	exts	rr4
	ldl	rr6,rr12
	sub	r3,r3
L20035:
	callr	_ttxput
L299:
	test	_u+48
	jpr	ne,L20034
L300:
	callr	_dvi
	ldl	rr2,rr12(#34)
	ldl	|_stkseg+~L1+6|(fp),rr2
	ldl	rr6,rr12
	sub	r5,r5
	call	@rr2
	callr	_evi
L10034:
	ldm	r8,_stkseg+10(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _ttwrite */

.psec data
.data

/*~~ttchkopen:*/

.psec
.code

_ttchkopen::
{
	dec	fp,#~L2
	ld	r2,_noterm
	cp	r2,_numterms
	jpr	ge,L312
	inc	_noterm,#1
	sub	r2,r2
	jpr	L10037
L312:
	ldk	r2,#1
L10037:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _ttchkopen */

.psec data
.data

/*~~ttchkclose:*/

.psec
.code

_ttchkclose::
{
	dec	_noterm,#1
	ret
	~L1 := 0
	~L2 := 0
	}	/* _ttchkclose */

.psec data
.data

