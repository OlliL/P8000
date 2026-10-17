
fp		:= r15;
sp		:= r15;
_shell:
.byte	%2f
.byte	%62
.byte	%69
.byte	%6e
.byte	%2f
.byte	%73
.byte	%68
.byte	%0
_cshell:
.byte	%2f
.byte	%62
.byte	%69
.byte	%6e
.byte	%2f
.byte	%63
.byte	%73
.byte	%68
.byte	%0
.byte	0
L19:
.byte	%50,%41,%54,%48,%0
.even
L21:
.byte	%3a,%2f,%62,%69,%6e,%3a,%2f,%75
.byte	%73,%72,%2f,%62,%69,%6e,%0
.even
L22:
.byte	%0
.even
L32:
.byte	%73,%68,%0
.even
L34:
.byte	%63,%73,%68,%0
.even
.even
L46:
.word	7
.word	8
.word	12
.word	13
.word	26
.even
/*~~execvp:*/

.code

_execvp::
{
	.data
	.even
L47:
	.word	L44
	.word	L30
	.word	L44
	.word	L42
	.word	L39
	.code
	sub	fp,#~L2
	ldm	652(fp),r12,#3
	ldl	@fp,rr6
	ldk	r13,#1
	ldk	r12,#0
	ld	r7,#L19
	callr	_getenv
	ld	~L1+4(fp),r2
	test	r2
	jpr	ne,L20
	ld	~L1+4(fp),#L21
L20:
	ld	r7,~L1+2(fp)
	ld	r6,#47
	callr	_index
	test	r2
	jpr	eq,L10003
	ld	r2,#L22
	jpr	L10004
L10003:
	ld	r2,~L1+4(fp)
L10004:
	ld	r14,r2
L25:
	lda	r5,~L1+8(fp)
	ld	r6,~L1+2(fp)
	ld	r7,r14
	callr	_execat
	ld	r14,r2
L26:
	lda	r7,~L1+8(fp)
	ld	r6,~L1(fp)
	callr	_execv
	ld	r2,_errno
	ld	r3,#L46
	ld	r4,#5
	cpir	r2,@r3,r4,eq
	jpr	ne,L24
	add	r4,r4
	neg	r4
	ld	r2,L47+8(r4)
	jp	@r2
L30:
	lda	r7,~L1+8(fp)
	callr	_whichshell
	ld	~L1+6(fp),r2
	cp	r2,#_shell
	jpr	ne,L31
	ld	~L1+136(fp),#L32
	jpr	L33
L31:
	ld	~L1+136(fp),#L34
L33:
	lda	r2,~L1+8(fp)
	ld	~L1+138(fp),r2
	ld	~L1+648(fp),#1
	jpr	L37
L20001:
	cp	~L1+648(fp),#254
	jpr	lt,L35
	ld	_errno,#7
	jpr	L44
L35:
	inc	~L1+648(fp),#1
L37:
	ld	r2,~L1+648(fp)
	inc	r2,#1
	add	r2,r2
	lda	r3,~L1+136(fp)
	ld	r4,~L1+648(fp)
	add	r4,r4
	ld	r5,~L1(fp)
	add	r5,r4
	add	r3,r2
	ld	r2,@r5
	ld	@r3,r2
	test	r2
	jpr	ne,L20001
	lda	r6,~L1+136(fp)
	ld	r7,~L1+6(fp)
	callr	_execv
	jpr	L44
L39:
	inc	r13,#1
	cp	r13,#5
	jpr	ugt,L44
	ld	r7,r13
	callr	_sleep
	jpr	L26
L42:
	inc	r12,#1
L24:
	test	r14
	jpr	ne,L25
	test	r12
	jpr	eq,L44
	ld	_errno,#13
L44:
	ld	r2,#-1
	ldm	r12,652(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 658
	}	/* _execvp */

.data


.code

_execat:
{
	dec	fp,#~L2
	ld	@fp,r5
	ld	r3,r6
	ld	r1,r7
	ld	r4,r5
	jpr	L50
L20003:
	cpb	@r1,#58
	jpr	eq,L51
	cpb	@r1,#45
	jpr	eq,L51
	ldb	rl2,@r1
	ldb	@r4,rl2
	inc	r1,#1
	inc	r4,#1
L50:
	testb	@r1
	jpr	ne,L20003
L51:
	cp	r4,~L1(fp)
	jpr	eq,L53
	ldb	@r4,#47
	jpr	L20004
L20008:
	inc	r1,#1
	ld	r2,r1
	jpr	L10005
L20006:
	ldb	rl2,@r3
	ldb	@r4,rl2
	inc	r3,#1
L20004:
	inc	r4,#1
L53:
	testb	@r3
	jpr	ne,L20006
	clrb	@r4
	testb	@r1
	jpr	ne,L20008
	sub	r2,r2
L10005:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _execat */

.data


.code

_whichshell:
{
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ld	r14,r7
	sub	r6,r6
	callr	_open
	ld	r13,r2
	cp	r2,#0
	jpr	ge,L57
	ld	r7,r2
	callr	_close
	jpr	L60
L57:
	lda	r6,~L1+3(fp)
	ld	r7,r13
	ldk	r5,#1
	callr	_read
	ld	r7,r13
	callr	_close
	cpb	~L1+3(fp),#35
	jpr	ne,L60
	ld	r2,#_cshell
	jpr	L10010
L60:
	ld	r2,#_shell
L10010:
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _whichshell */

.data

