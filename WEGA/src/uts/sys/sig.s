.seg
.psec data
.data


fp		:= r15;
sp		:= rr14;
_sigwstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%73
.byte	%69
.byte	%67
.byte	%2e
.byte	%63
.byte	%9
.byte	%9
.byte	%52
.byte	%65
.byte	%76
.byte	%3a
.byte	%20
.byte	%34
.byte	%2e
.byte	%32
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
.byte	%39
.byte	%3a
.byte	%34
.byte	%37
.byte	%3a
.byte	%34
.byte	%31
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
.comm	10,	_ipc;
/*~~signal:*/

.psec
.code

_signal::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ld	|_stkseg+~L1+0|(fp),r6
	ld	r13,r7
	test	r7
	jpr	eq,L10000
	ldl	rr10,#_proc
	jpr	L98
L20001:
	ldl	rr2,rr10
	inc	r3,#12
	cp	r13,@rr2
	jpr	ne,L96
	ldl	rr6,rr10
	ld	r5,|_stkseg+~L1|(fp)
	callr	_psignal
L96:
	add	r11,#36
L98:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr10,rr4
	jpr	ult,L20001
L10000:
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _signal */

.psec data
.data

/*~~psignal:*/

.psec
.code

_psignal::
{
	dec	fp,#~L2
	ldm	_stkseg+6(fp),r11,#3
	ld	r11,r5
	ldl	rr12,rr6
	dec	r11,#1
	cp	r11,#0
	jpr	lt,L10003
	cp	r11,#20
	jpr	ge,L10003
	ldl	rr2,#1
	sdal	rr2,r11
	ldl	rr4,rr12
	inc	r5,#6
	ldl	rr6,@rr4
	or	r6,r2
	or	r7,r3
	ldl	@rr4,rr6
	cpb	@rr12,#1
	jpr	ne,L10003
	ldb	rl2,rr12(#2)
	cpb	rl2,#25
	jpr	le,L10003
	cpb	rl2,#50
	jpr	le,L104
	ldl	rr2,rr12
	inc	r3,#2
	ldb	@rr2,#50
L104:
	ldl	rr6,rr12
	callr	_setrun
L10003:
	ldm	r11,_stkseg+6(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _psignal */

.psec data
.data

/*~~issig:*/

.psec
.code

_issig::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r8,#6
	ldl	rr10,_u+32
	jpr	L108
L20003:
	ldl	rr2,_u+330
	ldk	r2,#0
	and	r3,#1
	testl	rr2
	jpr	eq,L112
	ldl	rr8,#_proc+36
L115:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr8,rr4
	jpr	uge,L120
	ldl	rr2,rr8
	inc	r3,#16
	ld	r4,rr10(#14)
	cp	r4,@rr2
	jpr	ne,L113
	cpb	@rr8,#5
	jpr	ne,L113
	ldl	rr6,rr8
	sub	r5,r5
	callr	_freeproc
L113:
	add	r9,#36
	jpr	L115
L112:
	ldl	rr2,_u+330
	test	r3
	jpr	ne,L20004
	jpr	L120
L20006:
	ldl	rr6,rr10
	callr	_fsig
	ld	r13,r2
	cp	r2,#18
	jpr	eq,L20003
	dec	r2,#1
	sla	r2,#2
	ldl	rr4,_u+262(r2)
	ldk	r4,#0
	and	r5,#1
	testl	rr4
	jpr	eq,L20004
	ldb	rl2,rr10(#1)
	extsb	r2
	bit	r2,#4
	jpr	eq,L120
L20004:
	ld	r2,r13
	jpr	L10007
L120:
	ld	r2,r13
	dec	r2,#1
	ldl	rr4,#1
	sdal	rr4,r2
	com	r4
	com	r5
	ldl	rr2,rr10
	inc	r3,#6
	ldl	rr6,@rr2
	and	r6,r4
	and	r7,r5
	ldl	@rr2,rr6
L108:
	ldl	rr2,rr10(#6)
	testl	rr2
	jpr	ne,L20006
	sub	r2,r2
L10007:
	ldm	r8,_stkseg+4(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _issig */

.psec data
.data

/*~~stop:*/

.psec
.code

_stop::
{
	dec	fp,#~L2
	ldm	_stkseg+2(fp),r10,#4
	jpr	L124
L20010:
	ldl	rr6,rr12
	callr	_wakeup
	ldb	@rr10,#6
	callr	_swtch
	ldb	rl2,rr10(#1)
	extsb	r2
	and	r2,#16
	test	r2
	jpr	eq,L10011
	callr	_procxmt
	test	r2
	jpr	ne,L10011
L124:
	ldl	rr10,_u+32
	ld	r2,rr10(#16)
	cp	r2,#1
	jpr	eq,L125
	ldl	rr12,#_proc
	jpr	L128
L20008:
	ldl	rr2,rr10
	inc	r3,#16
	ld	r4,rr12(#14)
	cp	r4,@rr2
	jpr	eq,L20010
	add	r13,#36
L128:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr12,rr4
	jpr	ult,L20008
L125:
	ldl	rr6,_u+32
	callr	_fsig
	ld	|_stkseg+~L1|(fp),r2
	ld	r7,r2
	callr	_exit
L10011:
	ldm	r10,_stkseg+2(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _stop */

.psec data
.data

.even
/*~~psig:*/

.psec
.code

_psig::
{
	.psec	data
	.data
	.even
L156:
	.long	L152
	.long	L152
	.long	L152
	.long	L152
	.long	L152
	.long	L152
	.long	L142
	.long	L152
	.long	L152
	.long	L152
	.psec
	.code
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldl	rr10,_u+32
	ldb	rl2,rr10(#1)
	extsb	r2
	bit	r2,#4
	jpr	eq,L137
	callr	_stop
L137:
	ldl	rr6,rr10
	callr	_fsig
	ld	r13,r2
	test	r2
	jpr	eq,L10015
	dec	r2,#1
	ldl	rr4,#1
	sdal	rr4,r2
	com	r4
	com	r5
	ldl	rr2,rr10
	inc	r3,#6
	ldl	rr6,@rr2
	and	r6,r4
	and	r7,r5
	ldl	@rr2,rr6
	ld	r2,r13
	dec	r2,#1
	sla	r2,#2
	ldl	rr4,_u+262(r2)
	ldl	|_stkseg+~L1|(fp),rr4
	testl	rr4
	jpr	eq,L139
	clrb	_u+21
	cp	r13,#4
	jpr	eq,L140
	cp	r13,#5
	jpr	eq,L140
	ld	r2,r13
	dec	r2,#1
	sla	r2,#2
	ldl	rr4,#_u+262
	add	r5,r2
	subl	rr2,rr2
	ldl	@rr4,rr2
L140:
	ldl	rr6,|_stkseg+~L1|(fp)
	ld	r5,r13
	callr	_sendsig
	jpr	L10015
L139:
	ld	r2,r13
	dec	r2,#3
	cp	r2,#9
	jpr	ugt,L142
	rl	r2,#2
	ldl	rr4,L156(r2)
	jp	@rr4
L152:
	callr	_core
	test	r2
	jpr	eq,L142
	add	r13,#128
L142:
	ld	r7,r13
	callr	_exit
L10015:
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _psig */

.psec data
.data

/*~~fsig:*/

.psec
.code

_fsig::
{
	dec	fp,#~L2
	ld	_stkseg+10(fp),r13
	ldl	|_stkseg+~L1+0|(fp),rr6
	ldl	rr2,rr6
	ldl	rr4,rr2(#6)
	ldl	|_stkseg+~L1+4|(fp),rr4
	ldk	r13,#1
L20012:
	ldl	rr2,|_stkseg+~L1+4|(fp)
	ldk	r2,#0
	and	r3,#1
	testl	rr2
	jpr	eq,L161
	ld	r2,r13
	jpr	L10018
L161:
	ldl	rr2,|_stkseg+~L1+4|(fp)
	sral	rr2,#1
	ldl	|_stkseg+~L1+4|(fp),rr2
	inc	r13,#1
	cp	r13,#20
	jpr	lt,L20012
	sub	r2,r2
L10018:
	ld	r13,_stkseg+10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _fsig */

.psec data
.data

L165:
.byte	%63,%6f,%72,%65,%0
.even
/*~~core:*/

.psec
.code

_core::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r11,#3
	ld	r2,_u+24
	cp	r2,_u+28
	jpr	eq,L164
L20015:
	sub	r2,r2
	jpr	L10021
L164:
	clrb	_u+21
	ldl	rr2,#L165
	ldl	_u+78,rr2
	ldl	rr6,#_schar
	ldk	r5,#1
	callr	_namei
	ldl	rr12,rr2
	testl	rr2
	jpr	ne,L166
	testb	_u+21
	jpr	ne,L20015
	ld	r7,#438
	callr	_maknode
	ldl	rr12,rr2
	testl	rr2
	jpr	eq,L20015
L166:
	ldl	rr6,rr12
	ld	r5,#128
	callr	_access
	test	r2
	jpr	ne,L170
	ld	r2,rr12(#6)
	and	r2,#61440
	cp	r2,#32768
	jpr	ne,L170
	ldl	rr6,rr12
	callr	_itrunc
	subl	rr2,rr2
	ldl	_u+50,rr2
	ldl	rr2,#_u
	ldl	_u+44,rr2
	ld	_u+48,#2560
	ldb	_u+20,#1
	ld	r2,_umemory
	inc	r2,#1
	sra	r2,#1
	ld	r5,r2
	exts	rr4
	ldl	_u+1242,rr4
	ldl	rr6,rr12
	callr	_writei
	clrb	_u+20
	testb	_u+544
	jpr	ne,L173
	ldl	rr2,_u+32
	ld	r4,rr2(#20)
	dec	r4,#10
	ld	r11,r4
	sub	r7,r7
	ld	r6,r4
	sub	r5,r5
	ldk	r4,#1
	callr	_estabur
	ld	_u+44,#16128
	clr	_u+46
	ld	r2,r11
	ldb	rh2,rl2
	clrb	rl2
	ld	_u+48,r2
L20017:
	ldl	rr6,rr12
	callr	_writei
	jpr	L180
L173:
	clrb	|_stkseg+~L1+1|(fp)
	jpr	L178
L20014:
	ldb	rl3,|_stkseg+~L1+1|(fp)
	extsb	r3
	ldb	rl4,_u+1062(r3)
	extsb	r4
	bit	r4,#7
	jpr	ne,L176
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	ldb	rl3,_u+1062(r2)
	extsb	r3
	and	r3,#127
	ldb	rh3,rl3
	clrb	rl3
	ld	_u+44,r3
	clr	_u+46
	ldb	rl2,|_stkseg+~L1+1|(fp)
	extsb	r2
	sla	r2,#2
	ldb	rl3,_u+552(r2)
	subb	rh3,rh3
	inc	r3,#1
	ldb	rh3,rl3
	clrb	rl3
	ld	_u+48,r3
	ldl	rr6,rr12
	callr	_writei
L176:
	incb	|_stkseg+~L1+1|(fp),#1
L178:
	ldb	rl2,|_stkseg+~L1+1|(fp)
	cpb	rl2,_u+1190
	jpr	lt,L20014
	ld	r2,_u+1390
	ldb	rh2,rl2
	clrb	rl2
	ld	_u+44,r2
	ldk	r2,#0
	ldb	rl2,_u+1060
	ldb	rh2,rl2
	clrb	rl2
	ld	_u+46,r2
	ldk	r2,#0
	ldb	rl2,_u+1060
	ld	r3,r2
	neg	r3
	add	r3,#256
	ldb	rh3,rl3
	clrb	rl3
	ld	_u+48,r3
	jpr	L20017
L170:
	ldb	_u+21,#13
L180:
	ldl	rr6,rr12
	callr	_iput
	cpb	_u+21,#0
	ldk	r2,#0
	tcc	eq,r2
	extsb	r2
L10021:
	ldm	r11,_stkseg+4(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _core */

.psec data
.data

/*~~grow:*/

.psec
.code

_grow::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r8,#6
	ld	|_stkseg+~L1+0|(fp),r7
	ld	r6,_u+220
	dec	r6,#1
	ld	r2,r6
	neg	r2
	add	r2,#256
	ldb	rh2,rl2
	clrb	rl2
	cp	r2,r7
	jpr	ugt,L184
L20024:
	sub	r2,r2
	jpr	L10025
L184:
	ldk	r13,#6
	jpr	L185
L20019:
	ld	r2,_u+220
	inc	r2,#10
	add	r2,r12
	add	r2,_u+218
	cp	r2,#256
	jpr	ge,L185
	ld	r5,_u+220
	add	r5,r12
	ld	r6,_u+218
	ld	r7,_u+216
	ldk	r4,#1
	callr	_estabur
	test	r2
	jpr	eq,L192
L185:
	ld	r2,r13
	dec	r2,#1
	ld	r13,r2
	test	r2
	jpr	eq,L20024
	ldk	r2,#0
	ld	r3,|_stkseg+~L1|(fp)
	addl	rr2,#255
	sral	rr2,#8
	ld	r2,r3
	neg	r2
	add	r2,#256
	sub	r2,_u+220
	add	r2,r13
	ld	r12,r2
	cp	r2,#0
	jpr	le,L185
	testb	_u+544
	jpr	eq,L20019
	ld	r2,_u+220
	add	r2,r12
	cp	r2,#256
	jpr	gt,L185
	ld	r2,_u+220
	add	r2,r12
	ld	r3,r2
	neg	r3
	add	r3,#256
	ldb	_u+1060,rl3
L192:
	ldl	rr8,_u+32
	ld	r7,rr8(#20)
	add	r7,r12
	callr	_expand
	ld	r2,rr8(#18)
	ldl	rr4,rr8
	add	r5,#20
	add	r2,@rr4
	ld	r10,r2
	ld	r11,_u+220
	jpr	L197
L20021:
	dec	r10,#1
	ld	r7,r10
	sub	r7,r12
	ld	r6,r10
	callr	_copyseg
	dec	r11,#1
L197:
	test	r11
	jpr	ne,L20021
	ld	r11,r12
	jpr	L201
L20023:
	dec	r10,#1
	ld	r7,r10
	callr	_clearseg
	dec	r11,#1
L201:
	test	r11
	jpr	ne,L20023
	ld	r2,_u+220
	add	r2,r12
	ld	_u+220,r2
	ldk	r2,#1
L10025:
	ldm	r8,_stkseg+4(fp),#6
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _grow */

.psec data
.data

/*~~ptrace:*/

.psec
.code

_ptrace::
{
	dec	fp,#~L2
	ldm	_stkseg+0(fp),r10,#4
	ldl	rr10,_u+36
	cp	@rr10,#0
	jpr	gt,L205
	ldl	rr2,_u+32
	inc	r3,#1
	setb	@rr2,#4
	jpr	L10028
L205:
	ldl	rr12,#_proc
	jpr	L208
L20026:
	cpb	@rr12,#6
	jpr	ne,L206
	ldl	rr2,rr10
	inc	r3,#2
	ld	r4,rr12(#14)
	cp	r4,@rr2
	jpr	ne,L206
	ldl	rr2,_u+32
	inc	r3,#14
	ld	r4,rr12(#16)
	cp	r4,@rr2
	jpr	eq,L211
L206:
	add	r13,#36
L208:
	ld	r3,_Nproc
	mult	rr2,#36
	ldl	rr4,#_proc
	add	r5,r3
	cpl	rr12,rr4
	jpr	ult,L20026
	ldb	_u+21,#3
	jpr	L10028
L211:
	test	_ipc
	jpr	eq,L212
	ldl	rr6,#_ipc
	ld	r5,#25
	callr	_sleep
	jpr	L211
L212:
	ld	r2,rr12(#14)
	ld	_ipc,r2
	ld	r2,rr10(#8)
	ld	_ipc+8,r2
	ldl	rr2,rr10(#4)
	and	r2,#32512
	ldl	_ipc+4,rr2
	ld	r2,@rr10
	ld	_ipc+2,r2
	ldl	rr4,rr12
	inc	r5,#1
	resb	@rr4,#5
	ldl	rr6,rr12
	callr	_setrun
	jpr	L214
L20028:
	ldl	rr6,#_ipc
	ld	r5,#25
	callr	_sleep
L214:
	cp	_ipc+2,#0
	jpr	gt,L20028
	ld	r2,_ipc+8
	ld	_u+40,r2
	cp	_ipc+2,#0
	jpr	ge,L216
	ldb	_u+21,#5
L216:
	clr	_ipc
	ldl	rr6,#_ipc
	callr	_wakeup
L10028:
	ldm	r10,_stkseg+0(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _ptrace */

.psec data
.data

.even
/*~~procxmt:*/

.psec
.code

_procxmt::
{
	.psec	data
	.data
	.even
L264:
	.long	L223
	.long	L228
	.long	L231
	.long	L233
	.long	L246
	.long	L248
	.long	L258
	.long	L262
	.long	L257
	.psec
	.code
	sub	fp,#~L2
	ldm	_stkseg+6(fp),r8,#6
	ldl	rr2,_u+32
	inc	r3,#14
	ld	r4,_ipc
	cp	r4,@rr2
	jpr	eq,L218
L20031:
	sub	r2,r2
	jpr	L10031
L218:
	ld	r13,_ipc+2
	clr	_ipc+2
	ldl	rr6,#_ipc
	callr	_wakeup
	testb	_u+544
	jpr	ne,L219
	ld	r7,_ipc+6
	callr	_nsseg
	ld	_ipc+4,r2
L219:
	ld	r2,r13
	dec	r2,#1
	cp	r2,#8
	jpr	ugt,L227
	rl	r2,#2
	ldl	rr4,L264(r2)
	jp	@rr4
L223:
	testb	_u+544
	jpr	ne,L228
	ld	r7,_ipc+6
	ldl	rr4,#_ipc+8
	callr	_fuiword
	test	r2
	jpr	eq,L20031
	jpr	L227
L228:
	ldl	rr6,_ipc+4
	ldl	rr4,#_ipc+8
	callr	_fuword
	test	r2
	jpr	eq,L20031
	jpr	L227
L231:
	ld	r13,_ipc+6
	cp	r13,#0
	jpr	lt,L227
	cp	r13,#2560
	jpr	uge,L227
	ld	r2,r13
	sra	r2,#1
	add	r2,r2
	ld	r3,_u(r2)
	ld	_ipc+8,r3
	jpr	L20031
L233:
	ldl	rr2,_u+32
	ldl	rr4,rr2(#26)
	ldl	rr8,rr4
	testl	rr4
	jpr	eq,L234
	ldb	rl4,rr8(#10)
	cpb	rl4,#1
	jpr	ne,L227
	ldl	rr4,rr8(#6)
	ld	r6,rr4(#6)
	bit	r6,#9
	jpr	ne,L227
	ldl	rr2,rr4
	resb	@rr2,#5
L234:
	testb	_u+544
	jpr	eq,L236
	testb	_u+484
	jpr	eq,L237
	ldb	rl7,_ipc+4
	ldb	rh7,#0
	ld	r6,#-1
	ld	r5,r6
	sub	r4,r4
	callr	_ldsdr
L237:
	ldl	rr6,_ipc+4
	ld	r5,_ipc+8
	callr	_suword
	test	r2
	jpr	ne,L227
	testb	_u+484
	jpr	eq,L242
	ldb	rl7,_ipc+4
	ldb	rh7,#0
	ld	r6,#-1
	ld	r5,r6
	ldk	r4,#1
	callr	_ldsdr
	jpr	L242
L236:
	ld	r7,_u+216
	ld	r6,_u+218
	ld	r5,_u+220
	sub	r4,r4
	callr	_estabur
	ld	r7,_ipc+6
	ld	r6,_ipc+8
	callr	_suiword
	test	r2
	jpr	ne,L227
	ld	r7,_u+216
	ld	r6,_u+218
	ld	r5,_u+220
	ldk	r4,#1
	callr	_estabur
L242:
	testl	rr8
	jpr	eq,L20031
	ldl	rr2,rr8
	inc	r3,#12
	setb	@rr2,#1
	jpr	L20031
L246:
	ldl	rr6,_ipc+4
	ld	r5,_ipc+8
	callr	_suword
	test	r2
	jpr	ne,L227
	jpr	L20031
L248:
	ld	r13,_ipc+6
	ld	r2,r13
	sra	r2,#1
	add	r2,r2
	ldl	rr4,#_u
	add	r5,r2
	ldl	rr10,rr4
	ldk	r13,#0
L20030:
	ld	r2,r13
	add	r2,r13
	ldl	rr4,_u+362
	add	r5,r2
	cpl	rr10,rr4
	jpr	eq,L253
	inc	r13,#1
	cp	r13,#15
	jpr	lt,L20030
	ldl	rr2,_u+362
	add	r3,#30
	cpl	rr10,rr2
	jpr	eq,L253
	ldl	rr2,_u+362
	add	r3,#38
	cpl	rr10,rr2
	jpr	eq,L253
	ldl	rr2,_u+362
	add	r3,#34
	cpl	rr10,rr2
	jpr	ne,L227
	ldl	rr2,_u+362
	ld	r4,rr2(#34)
	and	r4,#65280
	ld	r5,_ipc+8
	and	r5,#255
	or	r5,r4
	ld	_ipc+8,r5
L253:
	ld	r2,_ipc+8
	ld	@rr10,r2
	jpr	L20031
L257:
	ldl	rr2,_u+362
	add	r3,#34
	set	@rr2,#0
L258:
	cp	_ipc+6,#1
	jpr	eq,L259
	testb	_u+544
	jpr	eq,L260
	ldl	rr2,_u+362
	add	r3,#36
	ld	r4,_ipc+4
	ld	@rr2,r4
L260:
	ldl	rr2,_u+362
	add	r3,#38
	ld	r4,_ipc+6
	ld	@rr2,r4
L259:
	ldl	rr2,_u+32
	inc	r3,#6
	subl	rr4,rr4
	ldl	@rr2,rr4
	test	_ipc+8
	jpr	eq,L261
	ldl	rr6,_u+32
	ld	r5,_ipc+8
	callr	_psignal
L261:
	ldk	r2,#1
	jpr	L10031
L262:
	ldl	rr6,_u+32
	callr	_fsig
	ld	|_stkseg+~L1+4|(fp),r2
	ld	r7,r2
	callr	_exit
L227:
	ld	_ipc+2,#-1
	jpr	L20031
L10031:
	ldm	r8,_stkseg+6(fp),#6
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _procxmt */

.psec data
.data

