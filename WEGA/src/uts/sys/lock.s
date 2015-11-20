.seg
.psec	data
.data


fp		:=	r15;
sp		:=	rr14;
_lockwstr::
.byte	%40
.byte	%5b
.byte	%24
.byte	%5d
.byte	%6c
.byte	%6f
.byte	%63
.byte	%6b
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
.byte	%35
.byte	%3a
.byte	%31
.byte	%34
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
.comm	200,	_callout;
.even
/*~~lkdata:*/

.psec
.code

_lkdata::
{
        .psec   data
        .data
        .even
L6000:
	.long L6001
	.long L6002
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L6003
	.long L1080
 	.long L1010
 	.long L1140
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L6004
	.long L1000
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L1010
	.long L1040
	.long L1020
	.long L1010
	.long L6005
	.even
        .psec
        .code
	sub     fp,#~L2
	ldm     _stkseg+~L1+%0012(fp),r8,#6
	ldl     rr2,_u+%0024
	ldl     |_stkseg+~L1+%000c|(fp),rr2
	ldl     rr4,#_stkseg+~L1+4
	add     r5,fp
	ldl     rr2,#_stkseg+~L1+8
	add     r3,fp
	ldl     rr6,|_stkseg+~L1+%000c|(fp)
	calr    _lkfront
	ldl     rr12,rr2
	testl   rr2
	jp      z,L1000
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr4,|_stkseg+~L1+%000c|(fp)
	ld      r7,rr4(#%0002)
	bit     r7,#2
	jr      z,L1180
	ld      r7,#%0080
	jr      L1170
L1180:
	sub     r7,r7
L1170:
	ldl     rr4,rr12
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	calr    _locked
	test    r2
	jp      nz,L1000
	ldl     rr2,rr12
	add     r3,#%004a
	ldl     rr10,rr2
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	ld      r4,rr2(#%0002)
	bit     r4,#0
	jr      z,L1190
	sub     r4,r4
	jr      L1200
L1190:
	ld      r4,#%0100
L1200:
	ld      |_stkseg+~L1+%0010|(fp),r4
	jp      L1010
L1160:
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0008|(fp)
	cpl     rr4,@rr2
	jp      lt,L1150
	ldl     rr2,rr8(#%0006)
	cpl     rr2,_u+%0020
	jp      nz,L1140
	ldl     rr4,rr8(#%000a)
	cpl     rr4,|_stkseg+~L1+%0004|(fp)
	jr      gt,L1210
	ldk     r3,#1
	jr      L1220
L1210:
	sub     r3,r3
L1220:
	ldl     rr4,rr8(#%000e)
	cpl     rr4,|_stkseg+~L1+%0004|(fp)
	jr      gt,L1230
	ldk     r4,#2
	jr      L1240
L1230:
	sub     r4,r4
L1240:
	add     r3,r4
	ldl     rr4,rr8(#%000a)
	cpl     rr4,|_stkseg+~L1+%0008|(fp)
	jr      gt,L1250
	ldk     r4,#4
	jr      L1260
L1250:
	sub     r4,r4
L1260:
	add     r3,r4
	ldl     rr4,rr8(#%000e)
	cpl     rr4,|_stkseg+~L1+%0008|(fp)
	jr      gt,L1270
	ldk     r4,#8
	jr      L1280
L1270:
	sub     r4,r4
L1280:
	add     r3,r4
	ld      r4,rr8(#%0004)
	and     r4,#%0100
	cp      r4,|_stkseg+~L1+%0010|(fp)
	jr      nz,L1290
	ld      r4,#%0010
	jr      L1130
L1290:
	sub     r4,r4
L1130:
	add     r3,r4
	ld      r2,r3
	dec     r2,#4
	cp      r2,#%001b
	jp      ugt,L1010
	rl      r2,#2
	ldl     rr4,L6000(r2)
	jp      @rr4
L6001:
	test    |_stkseg+~L1+%0010|(fp)
	jr      z,L1120
	ldl     rr4,rr8(#%000a)
	ldl     |_stkseg+~L1+%0000|(fp),rr4
	jp      L1110
L1120:
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr6,rr10
	ld      r5,|_stkseg+~L1+%0010|(fp)
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	calr    _lockadd
	test    r2
	jr      z,L1060
L6002:
	test    |_stkseg+~L1+%0010|(fp)
	jp      nz,L1000
	ldl     rr2,rr8(#%000a)
	cpl     rr2,|_stkseg+~L1+%0004|(fp)
	jr      ge,L1090
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr2,rr8(#%000a)
	ld      r5,#%0100
	ldl     rr6,rr10
	calr    _lockadd
	test    r2
	jr      z,L1100
L6003:
	test    |_stkseg+~L1+%0010|(fp)
	jr      z,L1040
	ldl     rr2,rr8(#%000a)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr6,rr10
	ld      r5,|_stkseg+~L1+%0010|(fp)
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	calr    _lockadd
	test    r2
	jr      nz,L1080
	ldl     rr2,_locklist
	testl   rr2
	jr      nz,L1070
	ldl     rr4,@rr8
	ldl     @rr10,rr4
	ldl     rr6,@rr10
	calr    _lockfree
	ldb     _u+%0015,#%23
	jr      L1000
L1100:
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr6,@rr10
	sub     r5,r5
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	calr    _lockadd
	test    r2
	jr      z,L1060
	ldl     rr2,@rr8
	ldl     @rr10,rr2
	ldl     rr6,@rr10
	calr    _lockfree
	jr      L1000
L1090:
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ldl     rr2,rr8(#%000a)
	sub     r5,r5
	ldl     rr6,rr10
	calr    _lockadd
	test    r2
	jr      nz,L1000
L1060:
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0008|(fp)
L1030:
	ldl     @rr2,rr4
	jr      L1000
L1070:
	ldl     rr2,rr8(#%000e)
	ldl     |_stkseg+~L1+%0004|(fp),rr2
	cpl     rr2,|_stkseg+~L1+%0008|(fp)
	jr      nz,L1010
	jr      L1000
L1080:
	test    |_stkseg+~L1+%0010|(fp)
	jr      z,L1050
	ldl     rr2,rr8(#%000e)
	ldl     |_stkseg+~L1+%0004|(fp),rr2
	cpl     rr2,|_stkseg+~L1+%0008|(fp)
	jr      nz,L1010
	jr      L1000
L1050:
	ldl     rr2,rr8(#%000a)
	cpl     rr2,|_stkseg+~L1+%0004|(fp)
	jr      ge,L1040
	ldl     rr2,rr8
	inc     r3,#14
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	ldl     @rr2,rr4
	jr      L1010
L6004:
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	jr      L1030
L6005:
	ldl     rr2,rr8(#%000e)
	cpl     rr2,|_stkseg+~L1+%0004|(fp)
	jr      z,L1020
L1140:
	ldl     rr10,rr8
	jr      L1010
L1020:
	ldl     rr2,rr8(#%000a)
	ldl     |_stkseg+~L1+%0004|(fp),rr2
L1040:
	ldl     rr2,@rr8
	ldl     @rr10,rr2
	ldl     rr6,rr8
	calr    _lockfree
L1010:
	ldl     rr8,@rr10
	testl   rr8
	jp      nz,L1160
L1150:
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
L1110:
	ldl     rr6,rr10
	ld      r5,|_stkseg+~L1+%0010|(fp)
	ldl     rr2,|_stkseg+~L1+%0004|(fp)
	calr    _lockadd
L1000:
	ldm     r8,_stkseg+~L1+%0012(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	30
}	/*	_lkdata	*/

.psec	data
.data

/*~~unlk:*/

.psec
.code

_unlk::
{
	sub     fp,#~L2
	ldm     _stkseg+%0010(fp),r8,#6
	ldl     rr2,_u+%0024
	ldl     |_stkseg+~L1+%000c|(fp),rr2
	ldl     rr4,#_stkseg+~L1+4
	add     r5,fp
	ldl     rr2,#_stkseg+~L1+8
	add     r3,fp
	ldl     rr6,|_stkseg+~L1+%000c|(fp)
	calr    _lkfront
	ldl     rr12,rr2
	testl   rr2
	jr      z,L700
	add     r3,#%004a
	ldl     rr10,rr2
	jr      L710
L760:
	ld      r2,rr8(#%0004)
	bit     r2,#4
	jr      z,L720
	ldl     rr2,rr8
	inc     r3,#4
	res     @rr2,#4
	ldl     rr6,rr8
	call    _wakeup
L720:
	ldl     rr2,rr8(#%000a)
	cpl     rr2,|_stkseg+~L1+%0004|(fp)
	jr      ge,L730
	ldl     rr2,rr8
	inc     r3,#14
	ldl     rr4,|_stkseg+~L1+%0008|(fp)
	cpl     rr4,@rr2
	jr      ge,L730
	ldl     rr2,rr8(#%000e)
	ldl     |_stkseg+~L1+%0000|(fp),rr2
	ld      r5,rr8(#%0004)
	ldl     rr6,rr8
	ldl     rr2,|_stkseg+~L1+%0008|(fp)
	calr    _lockadd
	test    r2
	jr      nz,L700
	ldl     rr2,rr8
	inc     r3,#14
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	ldl     @rr2,rr4
	jr      L700
L730:
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	cpl     rr4,@rr2
	jr      gt,L740
	ldl     rr2,rr8
	inc     r3,#14
	ldl     rr6,|_stkseg+~L1+%0008|(fp)
	cpl     rr6,@rr2
	jr      ge,L740
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr0,|_stkseg+~L1+%0008|(fp)
	ldl     @rr2,rr0
	jr      L700
L740:
	ldl     rr2,rr8
	inc     r3,#14
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	ldl     @rr2,rr4
L750:
	ldl     rr10,rr8
	jr      L710
L770:
	ldl     rr2,rr8(#%0006)
	cpl     rr2,_u+%20
	jr      nz,L750
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0008|(fp)
	cpl     rr4,@rr2
	jr      le,L700
	ldl     rr2,rr8(#%000e)
	cpl     rr2,|_stkseg+~L1+%0004|(fp)
	jr      le,L750
	ldl     rr2,rr8
	inc     r3,#10
	ldl     rr4,|_stkseg+~L1+%0004|(fp)
	cpl     rr4,@rr2
	jr      gt,L760
	ldl     rr2,rr8(#%000e)
	cpl     rr2,|_stkseg+~L1+%0008|(fp)
	jr      gt,L760
	ldl     rr2,@rr8
	ldl     @rr10,rr2
	ldl     rr6,rr8
	calr    _lockfree
L710:
	ldl     rr8,@rr10
	testl   rr8
	jr      nz,L770
L700:
	ldm     r8,_stkseg+%0010(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	28
}	/*	_unlk	*/

.psec	data
.data

/*~~locked:*/

.psec
.code

_locked::
{
	sub     fp,#~L2
	ldm     _stkseg+%0012(fp),r8,#6
	ld      |_stkseg+~L1+%000a|(fp),r7
	ldl     rr8,_stkseg+~L1+%0022(fp)
	ldl     rr10,rr2
	ldl     rr12,rr4
	ldl     rr2,rr12(#%004a)
	ldl     |_stkseg+~L1+%000c|(fp),rr2
	jr      L600
L610:
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	ldl     rr4,@rr2
	ldl     |_stkseg+~L1+%000c|(fp),rr4
	jr      L600
L650:
	inc     r3,#10
	cpl     rr8,@rr2
	jr      le,L620
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	ldl     rr4,rr2(#%0006)
	cpl     rr4,_u+%0020
	jr      z,L610
	inc     r3,#14
	cpl     rr10,@rr2
	jr      ge,L610
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	inc     r3,#4
	ld      r4,|_stkseg+~L1+%000a|(fp)
	and     r4,#%0100
	and     r4,@rr2
	jr      nz,L610
	bit     |_stkseg+~L1+%000a|(fp),#7
	jr      z,L660
	ldb     _u+%0015,#%0d
L640:
	ldk     r2,#1
	jr      L630
L660:
	ldl     rr6,|_stkseg+~L1+%000c|(fp)
	calr    _deadlock
	test    r2
	jr      nz,L640
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	inc     r3,#4
	set     @rr2,#4
	ldl     rr6,|_stkseg+~L1+%000c|(fp)
	ld      r5,#%0028
	call    _sleep
	ldl     rr2,rr12(#%004a)
	ldl     |_stkseg+~L1+%000c|(fp),rr2
	testb   _u+%0015
	jr      nz,L640
L600:
	ldl     rr2,|_stkseg+~L1+%000c|(fp)
	testl   rr2
	jr      nz,L650
L620:
	sub     r2,r2
L630:
	ldm     r8,_stkseg+%0012(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	30
}	/*	_locked	*/

.psec	data
.data

/*~~deadlock:*/

.psec
.code

_deadlock::
{
	sub     fp,#~L2
	ldm     _stkseg+%000a(fp),r10,#4
	ldl     rr12,rr6
	jr      L500
L540:
	ldl     rr2,rr12(#%0006)
	ldl     rr4,rr2(#%0016)
	ldl     rr10,rr4
	cpl     rr4,#_locklist
	jr      c,L510
	ld      r3,_Nflock
	mult    rr2,#%0012
	ldl     rr4,#_locklist
	add     r5,r3
	cpl     rr10,rr4
	jr      nc,L510
	ldl     rr2,rr10(#%0006)
	cpl     rr2,_u+%0020
	jr      nz,L520
	ldb     _u+%0015,#35
	ldk     r2,#1
	jr      L530
L520:
	ldl     rr12,rr10
L500:
	ldl     rr2,rr12(#%0006)
	cpb     @rr2,#%01
	jr      z,L540
L510:
	sub     r2,r2
L530:
	ldm     r10,_stkseg+%000a(fp),#4
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	18
}	/*	_deadlock	*/

.psec	data
.data

/*~~unlock:*/

.psec
.code

_unlock::
{
	sub     fp,#~L2
	ldm     _stkseg+%0010(fp),r8,#6
	ldl     rr12,rr6
	ldl     rr2,rr12(#%004a)
	testl   rr2
	jr      z,L400
	ldl     rr2,rr6
	add     r3,#%004a
	ldl     rr8,rr2
	clr     |_stkseg+~L1+%000e|(fp)
	clr     |_stkseg+~L1+%000c|(fp)
L450:
	ld      r2,|_stkseg+~L1+%000c|(fp)
	sla     r2,#2
	ldl     rr4,_u+%0068(r2)
	testl   rr4
	jr      z,L410
	ld      r2,|_stkseg+~L1+%000c|(fp)
	sla     r2,#2
	ldl     rr4,_u+%0068(r2)
	inc     r5,#2
	cpl     rr12,@rr4
	jr      ne,L410
	inc     |_stkseg+~L1+%000e|(fp),#1
L410:
	inc     |_stkseg+~L1+%000c|(fp),#1
	cp      |_stkseg+~L1+%000c|(fp),#%0014
	jr      lt,L450
	test    |_stkseg+~L1+%000e|(fp)
	jr      z,L420
	jr      L400
L430:
	ldl     rr2,@rr10
	ldl     @rr8,rr2
	ldl     rr6,rr10
	calr    _lockfree
	jr      L420
L440:
	ldl     rr2,rr10(#%0006)
	cpl     rr2,_u+%0020
	jr      z,L430
	ldl     rr8,rr10
L420:
	ldl     rr10,@rr8
	testl   rr10
	jr      nz,L440
L400:
	ldm     r8,_stkseg+%0010(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	28
}	/*	_unlock	*/

.psec	data
.data

/*~~lockalloc:*/

.psec
.code

_lockalloc::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldl     rr12,#_locklist
	ldl     rr2,rr12(#6)
	testl   rr2
	jr      nz,L300
	ldl     rr2,rr12
	inc     r3,#6
	ldl     rr4,#_proc
	ldl     @rr2,rr4
	ldl     rr10,#_locklist+18
	jr      L310
L320:
	ldl     rr6,rr10
	calr    _lockfree
	add     r11,#18
L310:
	ld      r3,_Nflock
	mult    rr2,#18
	ldl     rr4,#_locklist
	add     r5,r3
	cpl     rr10,rr4
	jr      c,L320
L300:
	ldl     rr10,@rr12
	testl   rr10
	jr      nz,L330
	ldb     _u+21,#%23
	subl    rr2,rr2
	jr      L340
L330:
	ldl     rr2,@rr10
	ldl     @rr12,rr2
	subl    rr2,rr2
	ldl     @rr10,rr2
	ldl     rr2,rr10
L340:
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	12
}	/*	_lockalloc	*/

.psec	data
.data

/*~~lockfree:*/

.psec
.code

_lockfree::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldl	rr12,rr6
	ldl	rr10,#_locklist
	ld	r2,r12(#4)
	bit	r2,#4
	jr	z,L10
	ldl	rr2,rr6
	inc	r3,#4
	res	@rr2,#4
	ldl	rr6,rr12
	call	_wakeup
L10:
	ldl     rr2,@rr10
	ldl	@r12,rr2
	ldl	@r10,rr12
	ldm	r10,_stkseg+4(fp),#4
	inc	fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	12
}	/*	_lockfree	*/

.psec	data
.data

/*~~lockadd:*/

.psec
.code

_lockadd::
{
	sub	fp,#~L2
	ldm	_stkseg+18(fp),r8,#6
	ldl     rr8,rr2
	ld      r11,r5
	ldl     rr12,rr6
	calr    _lockalloc
	ldl     |_stkseg+~L1+12|(fp),rr2
	testl   rr2
	jr      nz,L20
	ldb     _u+21,#%23
	ldk     r2,#1
	jr      L30
L20:
	ldl     rr2,|_stkseg+~L1+12|(fp)
	ldl     rr4,@rr12
	ldl     @rr2,rr4
	ldl     rr4,|_stkseg+~L1+12|(fp)
	ldl     @rr12,rr4
	ldl     rr2,|_stkseg+~L1+12|(fp)
	inc     r3,#6
	ldl     rr4,_u+32
	ldl     @rr2,rr4
	ldl     rr2,|_stkseg+~L1+12|(fp)
	inc     r3,#10
	ldl     @rr2,rr8
	ldl     rr2,|_stkseg+~L1+12|(fp)
	inc     r3,#14
	ldl     rr4,_stkseg+~L1+34(fp)
	ldl     @rr2,rr4
	ldl     rr2,|_stkseg+~L1+12|(fp)
	inc     r3,#4
	ld      @rr2,r11
	sub     r2,r2
L30:
	ldm	r8,_stkseg+18(fp),#6
	add	fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	30
}	/*	_lockadd	*/

.psec	data
.data

L5000:
.byte	%50,%41,%4e,%49,%43,%20,%69,%70
.byte	%20,%6e,%75,%6c,%6c,%20,%69,%6e
.byte	%20,%6c,%6b,%66,%72,%6f,%6e,%74
.byte	%0a,%00
.even
/*~~lkfront:*/

.psec
.code

_lkfront::
{
	sub     fp,#~L2
	ldm     _stkseg+36(fp),r8,#6
	ldl     rr8,rr2
	ldl     rr10,rr4
	ldl     rr12,rr6
	ld      r7,@rr12
	call    _getf
	ldl     |_stkseg+~L1+22|(fp),rr2
	testl   rr2
	jr      nz,L40
	jr      L50
L70:
	ldl     rr6,#L5000
	call    _printf
	jr      L50
L90:
	ldl     rr6,|_stkseg+~L1+%1a|(fp)
	ld      r5,#%0080
	call    _access
	test    r2
	jr      z,L60
L80:
	ldb     _u+21,#%0d
	jr      L50
L100:
	ld      r4,rr2(#%0006)
	bit     r4,#12
	jr      z,L80
	ldb     _u+21,#%1d
	jr      L50
L40:
	ldl     rr2,|_stkseg+~L1+22|(fp)
	ldl     rr4,rr2(#%0002)
	ldl     |_stkseg+~L1+26|(fp),rr4
	testl   rr4
	jr      z,L70
	ldl     rr2,|_stkseg+~L1+22|(fp)
	ldb     rl4,@rr2
	extsb   r4
	and     r4,#%0002
	test    r4
	jr      z,L90
L60:
	ld      |_stkseg+~L1+30|(fp),#%7000
	ldl     rr2,|_stkseg+~L1+26|(fp)
	ld      r4,rr2(#%0006)
	and     r4,#%f000
	and     r4,|_stkseg+~L1+30|(fp)
	jr      nz,L100
	ldl     rr6,rr12(#%0004)
	ldl     rr4,#_stkseg+~L1+%0000000c
	add     r5,fp
	ldk     r3,#10
	call    _copyin
	test    r2
	jr      z,L110
	ldb     _u+21,#%0e
	jr      L50
L120:
	ldb     _u+21,#%16
L50:
	subl    rr2,rr2
	jr      L200
L110:
	ld      r2,rr12(#%0002)
	cp      r2,#%0000
	jr      lt,L120
	cp      r2,#%0007
	jr      gt,L120
	bit     r2,#1
	jr      z,L130
	cp      |_stkseg+~L1+20|(fp),#%0001
	jr      nz,L140
	ldl     rr2,|_stkseg+~L1+22|(fp)
	ldl     rr4,rr2(#%0006)
	jr      L150
L140:
	cp      |_stkseg+~L1+20|(fp),#%0002
	jr      nz,L160
	ldl     rr2,|_stkseg+~L1+26|(fp)
	ldl     rr4,rr2(#%000e)
L150:
	addl    rr4,|_stkseg+~L1+16|(fp)
	ldl     |_stkseg+~L1+16|(fp),rr4
L160:
	ldl     rr2,|_stkseg+~L1+16|(fp)
	ldl     @rr10,rr2
	jr      L170
L130:
	ldl     rr2,|_stkseg+~L1+22|(fp)
	ldl     rr4,rr2(#%0006)
	ldl     @rr10,rr4
L170:
	ldl     rr2,@rr10
	cpl     rr2,#%00000000
	jr      lt,L120
	ldl     rr2,|_stkseg+~L1+12|(fp)
	testl   rr2
	jr      z,L180
	ldl     rr2,@rr10
	addl    rr2,|_stkseg+~L1+12|(fp)
	jr      L190
L180:
	ldl     rr2,#%40000000
L190:
	ldl     @rr8,rr2
	ldl     rr2,@rr10
	cpl     rr2,@rr8
	jr      ge,L120
	ldl     rr2,@rr10
	ldl     _u+40,rr2
	ldl     rr2,|_stkseg+~L1+%1a|(fp)
L200:
	ldm     r8,_stkseg+~L1+36(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	48
}	/*	_lkfront	*/

.psec	data
.data
