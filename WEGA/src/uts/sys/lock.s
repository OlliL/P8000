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
/*~~lockfree:*/

.psec
.code

_lockfree::
{
	dec	fp,#~L2
	ldm	_stkseg+4(fp),r10,#4
	ldl	rr12,rr6
	ldl	rr10,#0
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
	ldb     _stkseg+~L1+21,#21
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
	}	/*	_lockfree	*/

.psec	data
.data

/*~~lockadd:*/

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
	ldl     rr6,#%0100009a
	call    _printf
	jr      L50
L90:
	ldl     rr6,|_stkseg+~L1+22|(fp)
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
	ldb     _u+21,#%0d
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
	ldl     rr4,#%0000000c
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
	ldl     rr2,|_stkseg+~L1+22|(fp)
L130:
	ldl     rr4,rr2(#%0006)
	ldl     @rr10,rr4
L170:
	ldl     rr2,@rr10
	cpl     rr2,#%00000000
	jr      lt,L180
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
	ldl     rr2,|_stkseg+~L1+22|(fp)
L200:
	ldm     r8,_stkseg+~L1+36(fp),#6
	add     fp,#~L2
	ret
	~L1	:=	0
	~L2	:=	44
}	/*	_lockfree	*/

.psec	data
.data
