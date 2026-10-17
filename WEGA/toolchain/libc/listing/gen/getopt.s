
fp		:= r15;
sp		:= r15;
.even
_opterr::
.word	%1
.even
_optind::
.word	%1
.comm	2,	_optopt;
.comm	2,	_optarg;
.even
L25:
.word	%1
L29:
.byte	%2d,%2d,%0
.even
L34:
.byte	%3a,%20,%69,%6c,%6c,%65,%67,%61
.byte	%6c,%20,%6f,%70,%74,%69,%6f,%6e
.byte	%20,%2d,%2d,%20,%0
.even
L42:
.byte	%3a,%20,%6f,%70,%74,%69,%6f,%6e
.byte	%20,%72,%65,%71,%75,%69,%72,%65
.byte	%73,%20,%61,%6e,%20,%61,%72,%67
.byte	%75,%6d,%65,%6e,%74,%20,%2d,%2d
.byte	%20,%0
.even
/*~~getopt:*/

.code

_getopt::
{
	dec	fp,#~L2
	ldm	8(fp),r13,#2
	ldm	@fp,r5,#3
	cp	L25,#1
	jpr	ne,L26
	ld	r4,_optind
	cp	r4,r7
	jpr	ge,L10003
	add	r4,r4
	ld	r2,r6
	add	r2,r4
	ld	r3,@r2
	cpb	@r3,#45
	jpr	ne,L10003
	ld	r2,_optind
	add	r2,r2
	ld	r3,~L1+2(fp)
	add	r3,r2
	ld	r2,@r3
	testb	1(r2)
	jpr	eq,L10003
	ld	r2,_optind
	add	r2,r2
	ld	r7,~L1+2(fp)
	add	r7,r2
	ld	r6,@r7
	ld	r7,r6
	ld	r6,#L29
	callr	_strcmp
	test	r2
	jpr	eq,L20004
L26:
	ld	r2,L25
	ld	r3,_optind
	add	r3,r3
	ld	r4,~L1+2(fp)
	add	r4,r3
	ld	r3,@r4
	add	r3,r2
	ldb	rl4,@r3
	extsb	r4
	ld	r14,r4
	ld	_optopt,r4
	cp	r14,#58
	jpr	eq,L10004
	ld	r7,~L1(fp)
	ld	r6,r14
	callr	_strchr
	ld	r13,r2
	test	r2
	jpr	ne,L31
L10004:
	test	_opterr
	jpr	eq,L32
	ld	r7,~L1+2(fp)
	ld	r6,@r7
	ld	r7,r6
	ld	r6,#__iob+16
	callr	_fputs
	ld	r7,#L34
	ld	r6,#__iob+16
	callr	_fputs
	ld	r7,r14
	ld	r6,#__iob+16
	callr	_fputc
	ldk	r7,#10
	ld	r6,#__iob+16
	callr	_fputc
L32:
	inc	L25,#1
	ld	r2,L25
	ld	r3,_optind
	add	r3,r3
	ld	r4,~L1+2(fp)
	add	r4,r3
	ld	r3,@r4
	add	r3,r2
	testb	@r3
	jpr	ne,L36
	inc	_optind,#1
L20002:
	ld	L25,#1
L36:
	ld	r2,#63
	jpr	L10000
L20004:
	inc	_optind,#1
L10003:
	ld	r2,#-1
	jpr	L10000
L31:
	inc	r13,#1
	cpb	@r13,#58
	jpr	ne,L37
	ld	r2,L25
	inc	r2,#1
	ld	r3,_optind
	add	r3,r3
	ld	r4,~L1+2(fp)
	add	r4,r3
	ld	r3,@r4
	add	r3,r2
	testb	@r3
	jpr	eq,L38
	ld	r2,_optind
	add	r2,r2
	ld	r3,~L1+2(fp)
	add	r3,r2
	ld	r2,L25
	inc	r2,#1
	add	r2,@r3
	jpr	L20001
L38:
	inc	_optind,#1
	ld	r2,_optind
	cp	r2,~L1+4(fp)
	jpr	lt,L40
	test	_opterr
	jpr	eq,L20002
	ld	r7,~L1+2(fp)
	ld	r3,@r7
	ld	r7,r3
	ld	r6,#__iob+16
	callr	_fputs
	ld	r7,#L42
	ld	r6,#__iob+16
	callr	_fputs
	ld	r7,r14
	ld	r6,#__iob+16
	callr	_fputc
	ldk	r7,#10
	ld	r6,#__iob+16
	callr	_fputc
	jpr	L20002
L40:
	ld	r2,_optind
	add	r2,r2
	ld	r3,~L1+2(fp)
	add	r3,r2
	ld	r2,@r3
L20001:
	ld	_optarg,r2
	inc	_optind,#1
	ld	L25,#1
	jpr	L43
L37:
	inc	L25,#1
	ld	r2,L25
	ld	r3,_optind
	add	r3,r3
	ld	r4,~L1+2(fp)
	add	r4,r3
	ld	r3,@r4
	add	r3,r2
	testb	@r3
	jpr	ne,L44
	ld	L25,#1
	inc	_optind,#1
L44:
	clr	_optarg
L43:
	ld	r2,r14
L10000:
	ldm	r13,8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _getopt */

.data

