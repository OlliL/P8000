.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_sys3wstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%79
.byte	%73
.byte	%33
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
.byte	%31
.byte	%3a
.byte	%35
.byte	%39
.byte	%3a
.byte	%32
.byte	%38
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
.comm	0,	_mount;
/*~~fstat:*/

.psec
.code

_fstat::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	ldl	rr10,_u+36
	ld	r7,@rr10
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10000
	ldl	rr6,rr12(#2)
	ldl	rr4,rr10(#2)
	ldb	rl2,@rr12
	extsb	r2
	bit	r2,#2
	jpr	eq,L10003
	ldl	rr2,rr12(#6)
	jpr	L10004
L10003:
	subl	rr2,rr2
L10004:
	callr	_stat1
L10000:
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fstat */

.psec data
.data

/*~~stat:*/

.psec
.code

_stat::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	ldl	rr10,_u+36
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10005
	ldl	rr4,rr10(#4)
	ldl	rr6,rr2
	subl	rr2,rr2
	callr	_stat1
	ldl	rr6,rr12
	callr	_iput
L10005:
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _stat */

.psec data
.data

/*~~stat1:*/

.psec
.code

_stat1::
{
	sub	fp,#~L2
	ldm	_stkseg+42(fp),r8,#6
	ldm	|_stkseg+~L1+0|(fp),r2,#4
	ldl	rr12,rr6
	ldl	rr4,#_time
	ldl	rr2,rr4
	callr	_iupdat
	ld	r2,rr12(#2)
	ld	|_stkseg+~L1+12|(fp),r2
	ld	r2,rr12(#4)
	ld	|_stkseg+~L1+14|(fp),r2
	ld	r2,rr12(#6)
	ld	|_stkseg+~L1+16|(fp),r2
	ld	r2,rr12(#8)
	ld	|_stkseg+~L1+18|(fp),r2
	ld	r2,rr12(#10)
	ld	|_stkseg+~L1+20|(fp),r2
	ld	r2,rr12(#12)
	ld	|_stkseg+~L1+22|(fp),r2
	ldl	rr2,rr12(#18)
	ld	|_stkseg+~L1+24|(fp),r3
	ldl	rr2,rr12(#14)
	subl	rr2,|_stkseg+~L1|(fp)
	ldl	|_stkseg+~L1+26|(fp),rr2
	ld	r7,rr12(#2)
	ld	r4,rr12(#4)
	inc	r4,#15
	srl	r4,#3
	ldk	r2,#0
	ld	r3,r4
	ldl	rr4,rr2
	callr	_bread
	ldl	rr8,rr2
	ldl	rr2,rr8(#22)
	ldl	rr10,rr2
	ld	r2,rr12(#4)
	inc	r2,#15
	and	r2,#7
	sla	r2,#6
	add	r2,r11
	ld	r11,r2
	ldl	rr2,rr10(#52)
	ldl	|_stkseg+~L1+30|(fp),rr2
	ldl	rr2,rr10(#56)
	ldl	|_stkseg+~L1+34|(fp),rr2
	ldl	rr2,rr10(#60)
	ldl	|_stkseg+~L1+38|(fp),rr2
	ldl	rr6,rr8
	callr	_brelse
	ldl	rr6,#_stkseg+~L1+12
	add	r7,fp
	ldl	rr4,|_stkseg+~L1+4|(fp)
	ld	r3,#30
	callr	_copyout
	cp	r2,#0
	jpr	ge,L10008
	ldb	_u+21,#14
L10008:
	ldm	r8,_stkseg+42(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 54
	}	/* _stat1 */

.psec data
.data

/*~~dup:*/

.psec
.code

_dup::
{
	sub	fp,#~L2
	ldm	_stkseg+6(fp),r8,#6
	ldl	rr10,_u+36
	ld	r2,@rr10
	and	r2,#-64
	ld	r8,r2
	ld	r2,@rr10
	and	r2,#63
	ld	@rr10,r2
	ld	r7,@rr10
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10011
	ld	r2,r8
	and	r2,#64
	test	r2
	jpr	ne,L107
	sub	r7,r7
	callr	_ufalloc
	ld	r9,r2
	cp	r2,#0
	jpr	ge,L110
	jpr	L10011
L107:
	ld	r2,rr10(#2)
	ld	r9,r2
	cp	r2,#0
	jpr	lt,L10014
	cp	r2,#20
	jpr	lt,L111
L10014:
	ldb	_u+21,#9
	jpr	L10011
L111:
	ld	_u+40,r9
L110:
	cp	r9,@rr10
	jpr	eq,L10011
	ld	r2,r9
	sla	r2,#2
	ldl	rr4,_u+104(r2)
	testl	rr4
	jpr	eq,L113
	ld	r2,r9
	sla	r2,#2
	ldl	rr6,_u+104(r2)
	callr	_closef
L113:
	ld	r2,r9
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	ldl	@rr4,rr12
	ldl	rr2,rr12
	inc	r3,#1
	ldl	rr2,rr12
	inc	r3,#1
	incb	@rr2,#1
L10011:
	ldm	r8,_stkseg+6(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _dup */

.psec data
.data

.even
/*~~fcntl:*/

.psec
.code

_fcntl::
{
	.psec	data
	.data
	.even
L128:
	.long	L120
	.long	L123
	.long	L124
	.long	L125
	.long	L126
	.psec
	.code
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r9,#5
	ldl	rr10,_u+36
	ld	r7,@rr10
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10015
	ld	r3,rr10(#2)
	ld	r2,r3
	cp	r3,#4
	jpr	ugt,L127
	rl	r2,#2
	ldl	rr4,L128(r2)
	jp	@rr4
L120:
	ld	r3,rr10(#4)
	ld	r9,r3
	cp	r3,#0
	jpr	lt,L127
	cp	r3,#20
	jpr	gt,L127
	ld	r7,r3
	callr	_ufalloc
	ld	r9,r2
	cp	r2,#0
	jpr	lt,L10015
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	ldl	@rr4,rr12
	ldl	rr2,rr12
	inc	r3,#1
	ldl	rr2,rr12
	inc	r3,#1
	incb	@rr2,#1
	jpr	L10015
L123:
	ld	r2,@rr10
	ldb	rl3,_u+184(r2)
	extsb	r3
	ld	_u+40,r3
	jpr	L10015
L124:
	ld	r2,rr10(#4)
	ld	r3,@rr10
	ldl	rr4,#_u+184
	add	r5,r3
	ldb	@rr4,rl2
	jpr	L10015
L125:
	ldb	rl2,@rr12
	extsb	r2
	dec	r2,#1
	ld	_u+40,r2
	jpr	L10015
L126:
	ldb	rl2,@rr12
	andb	rl2,#3
	ldb	@rr12,rl2
	ld	r2,rr10(#4)
	inc	r2,#1
	and	r2,#-4
	orb	rl2,@rr12
	ldb	@rr12,rl2
	jpr	L10015
L127:
	ldb	_u+21,#22
L10015:
	ldm	r9,_stkseg+0(fp),#5
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _fcntl */

.psec data
.data

/*~~smount:*/

.psec
.code

_smount::
{
	sub	fp,#~L2
	ldm	_stkseg+20(fp),r8,#6
	ldl	rr2,_u+36
	ldl	|_stkseg+~L1+12|(fp),rr2
	callr	_getmdev
	ld	r13,r2
	testb	_u+21
	jpr	ne,L10019
	ldl	rr2,|_stkseg+~L1+12|(fp)
	ldl	rr4,rr2(#4)
	and	r4,#32512
	ldl	_u+78,rr4
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L10019
	ldb	rl2,rr10(#1)
	cpb	rl2,#1
	jpr	ne,L134
	ld	r2,rr10(#6)
	and	r2,#8192
	test	r2
	jpr	ne,L134
	subl	rr2,rr2
	ldl	|_stkseg+~L1|(fp),rr2
	ldl	rr8,#_mount
	jpr	L138
L20001:
	ldl	rr2,rr8(#8)
	testl	rr2
	jpr	eq,L139
	ldl	rr2,rr8
	inc	r3,#2
	cp	r13,@rr2
	jpr	ne,L136
	jpr	L134
L139:
	ldl	rr2,|_stkseg+~L1|(fp)
	testl	rr2
	jpr	ne,L136
	ldl	|_stkseg+~L1|(fp),rr8
L136:
	inc	r9,#12
L138:
	ld	r3,_Nmount
	mult	rr2,#12
	ldl	rr4,#_mount
	add	r5,r3
	cpl	rr8,rr4
	jpr	ult,L20001
	ldl	rr8,|_stkseg+~L1|(fp)
	testl	rr8
	jpr	eq,L134
	ld	r2,r13
	ldb	rl2,rh2
	extsb	r2
	ld	r5,r2
	mult	rr4,#12
	ldl	rr2,_bdevsw(r5)
	ldl	|_stkseg+~L1+16|(fp),rr2
	ldl	rr4,|_stkseg+~L1+12|(fp)
	ld	r6,rr4(#8)
	test	r6
	jpr	ne,L10022
	ldk	r6,#1
	jpr	L10023
L10022:
	ldk	r6,#0
L10023:
	ld	r7,r13
	ldl	rr2,|_stkseg+~L1+16|(fp)
	call	@rr2
	testb	_u+21
	jpr	ne,L134
	ld	r7,r13
	ldl	rr4,#1
	callr	_bread
	ldl	|_stkseg+~L1+8|(fp),rr2
	testb	_u+21
	jpr	eq,L145
	ldl	rr6,rr2
	callr	_brelse
	jpr	L146
L145:
	ldl	rr2,rr8
	inc	r3,#4
	ldl	@rr2,rr10
	ldl	rr2,rr8
	inc	r3,#2
	ld	@rr2,r13
	callr	_geteblk
	ldl	rr4,rr8
	inc	r5,#8
	ldl	@rr4,rr2
	ldl	rr6,|_stkseg+~L1+8|(fp)
	ldl	rr2,rr6(#22)
	ldl	rr6,rr2
	ldl	rr4,rr8(#8)
	ldl	rr2,rr4(#22)
	ldl	rr4,rr2
	ld	r3,#512
	callr	_bcopy
	ldl	rr2,rr8(#8)
	ldl	rr4,rr2(#22)
	ldl	|_stkseg+~L1+4|(fp),rr4
	ld	@rr8,#1
	ldl	rr2,|_stkseg+~L1+4|(fp)
	add	r3,#411
	clrb	@rr2
	ldl	rr2,|_stkseg+~L1+4|(fp)
	add	r3,#410
	clrb	@rr2
	ldl	rr2,|_stkseg+~L1+12|(fp)
	ld	r4,rr2(#8)
	and	r4,#1
	ldl	rr6,|_stkseg+~L1+4|(fp)
	add	r7,#413
	ldb	@rr6,rl4
	ldl	rr6,|_stkseg+~L1+8|(fp)
	callr	_brelse
	setb	@rr10,#3
	ldl	rr6,rr10
	callr	_prele
	jpr	L10019
L134:
	ldb	_u+21,#16
L146:
	ldl	rr6,rr10
	callr	_iput
L10019:
	ldm	r8,_stkseg+20(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 32
	}	/* _smount */

.psec data
.data

/*~~sumount:*/

.psec
.code

_sumount::
{
	sub	fp,#~L2
	ldm	_stkseg+8(fp),r8,#6
	callr	_getmdev
	ld	r13,r2
	testb	_u+21
	jpr	ne,L10024
	ld	r7,r2
	callr	_xumount
	callr	_update
	ldl	rr8,#_mount
	jpr	L156
L20003:
	ldl	rr2,rr8(#8)
	testl	rr2
	jpr	eq,L154
	ldl	rr2,rr8
	inc	r3,#2
	cp	r13,@rr2
	jpr	eq,L158
L154:
	inc	r9,#12
L156:
	ld	r3,_Nmount
	mult	rr2,#12
	ldl	rr4,#_mount
	add	r5,r3
	cpl	rr8,rr4
	jpr	ult,L20003
	ldb	_u+21,#22
	jpr	L10024
L158:
	ldl	rr10,#_inode
	jpr	L161
L20005:
	ld	r2,rr10(#4)
	test	r2
	jpr	eq,L159
	ldl	rr2,rr10
	inc	r3,#2
	cp	r13,@rr2
	jpr	ne,L159
	ldb	_u+21,#16
	jpr	L10024
L159:
	add	r11,#82
L161:
	ld	r3,_Ninode
	mult	rr2,#82
	ldl	rr4,#_inode
	add	r5,r3
	cpl	rr10,rr4
	jpr	ult,L20005
	ld	r2,r13
	ldb	rl2,rh2
	extsb	r2
	ld	r5,r2
	mult	rr4,#12
	ldl	rr2,_bdevsw+4(r5)
	ldl	|_stkseg+~L1+4|(fp),rr2
	ld	r7,r13
	sub	r6,r6
	call	@rr2
	ld	r7,r13
	callr	_binval
	ldl	rr2,rr8(#4)
	ldl	rr10,rr2
	resb	@rr10,#3
	ldl	rr6,rr10
	callr	_plock
	ldl	rr6,rr10
	callr	_iput
	ldl	rr2,rr8(#8)
	ldl	|_stkseg+~L1|(fp),rr2
	ldl	rr2,rr8
	inc	r3,#8
	subl	rr4,rr4
	ldl	@rr2,rr4
	ldl	rr6,|_stkseg+~L1|(fp)
	callr	_brelse
	clr	@rr8
L10024:
	ldm	r8,_stkseg+8(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 20
	}	/* _sumount */

.psec data
.data

/*~~getmdev:*/

.psec
.code

_getmdev::
{
	dec	fp,#~L2
	ldm	_stkseg+2(fp),r10,#4
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	ne,L166
	ld	r2,#-1
	jpr	L10027
L166:
	ld	r2,rr10(#6)
	and	r2,#61440
	cp	r2,#24576
	jpr	eq,L167
	ldb	_u+21,#15
L167:
	ldl	rr2,rr10(#18)
	ld	r13,r3
	ld	r2,r3
	ldb	rl2,rh2
	extsb	r2
	cp	r2,_nblkdev
	jpr	lt,L168
	ldb	_u+21,#6
L168:
	ldl	rr6,rr10
	callr	_iput
	ld	r2,r13
L10027:
	ldm	r10,_stkseg+2(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _getmdev */

.psec data
.data

/*~~ioctl:*/

.psec
.code

_ioctl::
{
	sub	fp,#~L2
	ldm	_stkseg+10(fp),r8,#6
	ldl	rr8,_u+36
	ld	r7,@rr8
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10030
	ldl	rr2,rr12(#2)
	ldl	rr10,rr2
	ld	r2,rr10(#6)
	and	r2,#61440
	cp	r2,#8192
	jpr	eq,L172
	ldb	_u+21,#25
	jpr	L10030
L172:
	clr	_u+40
	ldl	rr2,rr10(#18)
	ld	|_stkseg+~L1|(fp),r3
	ld	r2,r3
	ldb	rl2,rh2
	extsb	r2
	ld	r5,r2
	mult	rr4,#28
	ldl	rr2,_cdevsw+16(r5)
	ldl	|_stkseg+~L1+2|(fp),rr2
	ld	r6,rr8(#2)
	ldl	rr4,rr8(#4)
	ld	r7,|_stkseg+~L1|(fp)
	and	r7,#255
	ldb	rl3,@rr12
	extsb	r3
	ldl	_stkseg+~L1+6(fp),rr8
	ldl	rr8,_stkseg+~L1+2(fp)
	call	@rr8
	ldl	rr8,_stkseg+~L1+6(fp)
L10030:
	ldm	r8,_stkseg+10(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _ioctl */

.psec data
.data

/*~~stty:*/

.psec
.code

_stty::
{
	ld	_u+206,#29705
	ld	r2,_u+208
	ld	_u+210,r2
	ld	_u+208,#16128
	callr	_ioctl
	ret
	~L1 := 0
	~L2 := 0
	}	/* _stty */

.psec data
.data

/*~~gtty:*/

.psec
.code

_gtty::
{
	ld	_u+206,#29704
	ld	r2,_u+208
	ld	_u+210,r2
	ld	_u+208,#16128
	callr	_ioctl
	ret
	~L1 := 0
	~L2 := 0
	}	/* _gtty */

.psec data
.data

/*~~mdmctl:*/

.psec
.code

_mdmctl::
{
	dec	fp,#~L2
	ldm	_stkseg+8(fp),r12,#2
	ldl	rr12,_u+36
	ld	r2,rr12(#6)
	cp	r2,#0
	jpr	lt,L10042
	cp	r2,_nchrdev
	jpr	lt,L179
L10042:
	ldb	_u+21,#19
	jpr	L10039
L179:
	clr	_u+40
	ld	r2,rr12(#6)
	ld	r5,r2
	mult	rr4,#28
	ldl	rr2,_cdevsw+16(r5)
	ldl	|_stkseg+~L1|(fp),rr2
	ldl	rr4,rr12(#2)
	ld	r3,rr12(#6)
	sub	r7,r7
	ld	r6,@rr12
	ldl	_stkseg+~L1+4(fp),rr8
	ldl	rr8,_stkseg+~L1+0(fp)
	call	@rr8
	ldl	rr8,_stkseg+~L1+4(fp)
L10039:
	ldm	r12,_stkseg+8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _mdmctl */

.psec data
.data

