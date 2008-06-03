.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_sys2wstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%79
.byte	%73
.byte	%32
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
.byte	%30
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
/*~~read:*/

.psec
.code

_read::
{
	ldk	r7,#1
	callr	_rdwr
	ret
	~L1 := 0
	~L2 := 0
	}	/* _read */

.psec data
.data

/*~~write:*/

.psec
.code

_write::
{
	ldk	r7,#2
	callr	_rdwr
	ret
	~L1 := 0
	~L2 := 0
	}	/* _write */

.psec data
.data

/*~~rdwr:*/

.psec
.code

_rdwr::
{
	sub	fp,#~L2
	ldm	_stkseg+12(fp),r8,#6
	ld	r13,r7
	ldl	rr2,_u+36
	ldl	|_stkseg+~L1+6|(fp),rr2
	ld	r7,@rr2
	callr	_getf
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L10006
	ldb	rl2,@rr10
	extsb	r2
	and	r2,r13
	test	r2
	jpr	ne,L96
	ldb	_u+21,#9
	jpr	L10006
L96:
	ldl	rr2,|_stkseg+~L1+6|(fp)
	ldl	rr4,rr2(#2)
	and	r4,#32512
	ldl	_u+44,rr4
	ld	r4,rr2(#6)
	ld	_u+48,r4
	clrb	_u+20
	ldb	rl4,@rr10
	extsb	r4
	ld	_u+54,r4
	ldl	rr4,rr10(#2)
	ldl	rr8,rr4
	ld	r4,rr8(#6)
	and	r4,#61440
	ld	|_stkseg+~L1+10|(fp),r4
	cp	r4,#32768
	jpr	eq,L10009
	cp	r4,#16384
	jpr	ne,L97
L10009:
	ldl	rr2,rr10(#6)
	ldl	_u+50,rr2
	ldl	rr2,rr8(#74)
	testl	rr2
	jpr	eq,L98
	ldk	r2,#0
	ld	r3,_u+48
	addl	rr2,_u+50
	ldl	|_stkseg+~L1+2|(fp),rr2
	cp	r13,#1
	jpr	ne,L10010
	ld	r7,#256
	jpr	L10011
L10010:
	sub	r7,r7
L10011:
	ldl	rr4,rr8
	ldl	rr2,_u+50
	callr	_locked
	test	r2
	jpr	ne,L10006
L98:
	ldl	rr6,rr8
	callr	_plock
	bit	_u+54,#3
	jpr	eq,L103
	cp	r13,#2
	jpr	ne,L103
	ldl	rr2,rr8(#14)
	ldl	rr4,rr10
	inc	r5,#6
	ldl	@rr4,rr2
	jpr	L103
L97:
	cp	|_stkseg+~L1+10|(fp),#4096
	jpr	ne,L103
	ldl	rr6,rr8
	callr	_plock
	ldl	rr2,rr10
	inc	r3,#6
	subl	rr4,rr4
	ldl	@rr2,rr4
L103:
	ldl	rr2,rr10(#6)
	ldl	_u+50,rr2
	cp	r13,#1
	jpr	ne,L105
	ldl	rr6,rr8
	callr	_readi
	jpr	L107
L105:
	ldl	rr6,rr8
	callr	_writei
L107:
	cp	|_stkseg+~L1+10|(fp),#32768
	jpr	eq,L10012
	cp	|_stkseg+~L1+10|(fp),#16384
	jpr	eq,L10012
	cp	|_stkseg+~L1+10|(fp),#4096
	jpr	ne,L109
L10012:
	ldl	rr6,rr8
	callr	_prele
L109:
	ldl	rr2,|_stkseg+~L1+6|(fp)
	ld	r4,rr2(#6)
	sub	r4,_u+48
	ldk	r6,#0
	ld	r7,r4
	ldl	rr4,rr10
	inc	r5,#6
	addl	rr6,@rr4
	ldl	@rr4,rr6
	ldl	rr2,|_stkseg+~L1+6|(fp)
	ld	r4,rr2(#6)
	sub	r4,_u+48
	ld	_u+40,r4
	ld	r5,r4
	exts	rr4
	addl	rr4,_u+1236
	ldl	_u+1236,rr4
	cp	r13,#1
	jpr	ne,L111
	ld	r5,_u+40
	exts	rr4
	addl	rr4,_sysinfo+80
	ldl	_sysinfo+80,rr4
	jpr	L10006
L111:
	ld	r3,_u+40
	exts	rr2
	addl	rr2,_sysinfo+84
	ldl	_sysinfo+84,rr2
L10006:
	ldm	r8,_stkseg+12(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 24
	}	/* _rdwr */

.psec data
.data

/*~~open:*/

.psec
.code

_open::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r12,#2
	ldl	rr12,_u+36
	ld	r7,rr12(#4)
	inc	r7,#1
	ld	r6,rr12(#6)
	callr	_copen
	ldm	r12,_stkseg+0(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _open */

.psec data
.data

/*~~creat:*/

.psec
.code

_creat::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r12,#2
	ldl	rr12,_u+36
	ld	r6,rr12(#4)
	ld	r7,#770
	callr	_copen
	ldm	r12,_stkseg+0(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _creat */

.psec data
.data

/*~~copen:*/

.psec
.code

_copen::
{
	sub	fp,#~L2
	ldm	_stkseg+10(fp),r8,#6
	ld	|_stkseg+~L1+0|(fp),r6
	ld	r13,r7
	and	r7,#3
	test	r7
	jpr	ne,L119
	ldb	_u+21,#22
	jpr	L10019
L119:
	bit	r13,#8
	jpr	eq,L120
	ldl	rr6,#_uchar
	ldk	r5,#1
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	ne,L121
	testb	_u+21
	jpr	ne,L10019
	ld	r7,|_stkseg+~L1|(fp)
	and	r7,#3583
	callr	_maknode
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L10019
	res	r13,#9
L133:
	bit	r13,#8
	jpr	ne,L135
	bit	r13,#0
	jpr	eq,L136
	ldl	rr6,rr10
	ld	r5,#256
	callr	_access
L136:
	ld	r2,r13
	and	r2,#514
	jpr	eq,L135
	ldl	rr6,rr10
	ld	r5,#128
	callr	_access
	ld	r2,rr10(#6)
	and	r2,#61440
	cp	r2,#16384
	jpr	ne,L135
	ldb	_u+21,#21
L135:
	testb	_u+21
	jpr	ne,L20001
	callr	_falloc
	ldl	rr8,rr2
	testl	rr2
	jpr	ne,L140
	jpr	L20001
L121:
	bit	r13,#10
	jpr	eq,L125
	ldb	_u+21,#17
	jpr	L20001
L125:
	ldl	rr6,#_u+222
	callr	_save
	test	r2
	jpr	eq,L128
	callr	_setscr
	ldl	rr6,rr10
	callr	_iput
	testb	_u+21
	jpr	ne,L10019
	ldb	_u+21,#4
	jpr	L10019
L128:
	ldl	rr2,rr10(#74)
	testl	rr2
	jpr	eq,L131
	ldl	rr2,#1073741824
	ldl	|_stkseg+~L1+4|(fp),rr2
	sub	r7,r7
	ldl	rr4,rr10
	sub	r3,r3
	callr	_locked
	test	r2
	jpr	ne,L20001
L131:
	res	r13,#8
	jpr	L133
L120:
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L10019
	jpr	L133
L140:
	bit	r13,#9
	jpr	eq,L141
	ldl	rr6,rr10
	callr	_itrunc
L141:
	ldl	rr6,rr10
	callr	_prele
	ld	r2,r13
	and	r2,#255
	ldb	@rr8,rl2
	ldl	rr2,rr8
	inc	r3,#2
	ldl	@rr2,rr10
	ld	r2,_u+40
	ld	|_stkseg+~L1+8|(fp),r2
	ldl	rr6,#_u+222
	callr	_save
	test	r2
	jpr	eq,L143
	callr	_setscr
	testb	_u+21
	jpr	ne,L144
	ldb	_u+21,#4
L144:
	ld	r2,|_stkseg+~L1+8|(fp)
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
	ldl	rr6,rr8
	callr	_closef
	jpr	L10019
L143:
	ldl	rr6,rr10
	ld	r5,r13
	callr	_openi
	testb	_u+21
	jpr	eq,L10019
	ld	r2,|_stkseg+~L1+8|(fp)
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
	ldl	rr2,rr8
	inc	r3,#1
	ldl	rr2,rr8
	inc	r3,#1
	decb	@rr2,#1
L20001:
	ldl	rr6,rr10
	callr	_iput
L10019:
	ldm	r8,_stkseg+10(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 22
	}	/* _copen */

.psec data
.data

/*~~close:*/

.psec
.code

_close::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	ldl	rr10,_u+36
	ld	r7,@rr10
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10023
	ld	r2,@rr10
	sla	r2,#2
	ldl	rr4,#_u+104
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
	ldl	rr6,rr12
	callr	_closef
L10023:
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _close */

.psec data
.data

/*~~seek:*/

.psec
.code

_seek::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r8,#6
	ldl	rr8,_u+36
	ld	r7,@rr8
	callr	_getf
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10026
	ldl	rr2,rr12(#2)
	ldl	rr10,rr2
	ld	r2,rr10(#6)
	and	r2,#61440
	cp	r2,#4096
	jpr	ne,L155
	ldb	_u+21,#29
	jpr	L10026
L155:
	ld	r2,rr8(#6)
	cp	r2,#1
	jpr	ne,L156
	ldl	rr2,rr12(#6)
	ldl	rr4,rr8
	inc	r5,#2
	addl	rr2,@rr4
	ldl	@rr4,rr2
	jpr	L157
L156:
	ld	r2,rr8(#6)
	cp	r2,#2
	jpr	ne,L157
	ldl	rr2,rr12(#2)
	ldl	rr4,rr2(#14)
	ldl	rr2,rr8
	inc	r3,#2
	addl	rr4,@rr2
	ldl	@rr2,rr4
L157:
	ldl	rr2,rr8(#2)
	ldl	rr4,rr12
	inc	r5,#6
	ldl	@rr4,rr2
	ldl	rr2,rr8(#2)
	ldl	_u+40,rr2
L10026:
	ldm	r8,_stkseg+0(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _seek */

.psec data
.data

/*~~link:*/

.psec
.code

_link::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r8,#6
	ldl	rr8,_u+36
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10029
	ld	r2,rr12(#6)
	and	r2,#61440
	cp	r2,#16384
	jpr	ne,L163
	callr	_suser
	test	r2
	jpr	eq,L164
L163:
	ldl	rr6,rr12
	callr	_prele
	ldl	rr2,rr8(#4)
	and	r2,#32512
	ldl	_u+78,rr2
	ldl	rr6,#_uchar
	ldk	r5,#1
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L165
	ldb	_u+21,#17
	ldl	rr6,rr2
	callr	_iput
	jpr	L164
L165:
	testb	_u+21
	jpr	ne,L164
	ldl	rr2,rr12
	inc	r3,#2
	ldl	rr4,_u+98
	ld	r6,rr4(#2)
	cp	r6,@rr2
	jpr	eq,L167
	ldl	rr6,rr4
	callr	_iput
	ldb	_u+21,#18
	jpr	L164
L167:
	ldl	rr6,rr12
	callr	_wdir
	testb	_u+21
	jpr	ne,L164
	ldl	rr2,rr12
	inc	r3,#8
	ldl	rr2,rr12
	inc	r3,#8
	inc	@rr2,#1
	setb	@rr12,#6
L164:
	ldl	rr6,rr12
	callr	_iput
L10029:
	ldm	r8,_stkseg+0(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _link */

.psec data
.data

/*~~mknod:*/

.psec
.code

_mknod::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	ldl	rr10,_u+36
	ld	r2,rr10(#4)
	and	r2,#61440
	cp	r2,#4096
	jpr	eq,L172
	callr	_suser
	test	r2
	jpr	eq,L10032
L172:
	ldl	rr6,#_uchar
	ldk	r5,#1
	callr	_namei
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L173
	ldb	_u+21,#17
	jpr	L174
L173:
	testb	_u+21
	jpr	ne,L10032
	ld	r7,rr10(#4)
	callr	_maknode
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L10032
	ld	r3,rr12(#6)
	and	r3,#61440
	ld	r2,r3
	jpr	L178
L180:
	ld	r3,rr10(#6)
	ld	r5,r3
	exts	rr4
	ldl	rr6,rr12
	add	r7,#18
	ldl	@rr6,rr4
	setb	@rr12,#6
	jpr	L174
L178:
	cp	r2,#8192
	jpr	eq,L180
	cp	r2,#24576
	jpr	eq,L180
L174:
	ldl	rr6,rr12
	callr	_iput
L10032:
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _mknod */

.psec data
.data

/*~~saccess:*/

.psec
.code

_saccess::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r8,#6
	ldl	rr8,_u+36
	ld	r13,_u+24
	ld	r12,_u+26
	ld	r2,_u+28
	ld	_u+24,r2
	ld	r3,_u+30
	ld	_u+26,r3
	ldl	rr6,#_uchar
	sub	r5,r5
	callr	_namei
	ldl	rr10,rr2
	testl	rr2
	jpr	eq,L183
	ld	r2,rr8(#4)
	bit	r2,#2
	jpr	eq,L184
	ldl	rr6,rr10
	ld	r5,#256
	callr	_access
L184:
	ld	r2,rr8(#4)
	bit	r2,#1
	jpr	eq,L185
	ldl	rr6,rr10
	ld	r5,#128
	callr	_access
L185:
	ld	r2,rr8(#4)
	bit	r2,#0
	jpr	eq,L186
	ldl	rr6,rr10
	ld	r5,#64
	callr	_access
L186:
	ldl	rr6,rr10
	callr	_iput
L183:
	ld	_u+24,r13
	ld	_u+26,r12
	ldm	r8,_stkseg+0(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _saccess */

.psec data
.data

