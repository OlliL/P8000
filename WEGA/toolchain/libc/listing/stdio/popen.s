
fp		:= r15;
sp		:= r15;
.even
_popen_pid:
.=.+40
L28:
.byte	%2f,%62,%69,%6e,%2f,%73,%68,%0
.even
L29:
.byte	%73,%68,%0
.even
L30:
.byte	%2d,%63,%0
.even
/*~~popen:*/

.code

_popen::
{
	dec	fp,#~L2
	ldm	10(fp),r12,#3
	ldl	@fp,rr6
	lda	r7,~L1+4(fp)
	callr	_pipe
	cp	r2,#0
	jpr	ge,L22
L20000:
	sub	r2,r2
	jpr	L10000
L22:
	ld	r2,~L1(fp)
	cpb	@r2,#114
	jpr	ne,L10003
	ld	r3,~L1+4(fp)
	jpr	L10004
L10003:
	ld	r3,~L1+6(fp)
L10004:
	ld	r14,r3
	ld	r2,~L1(fp)
	cpb	@r2,#114
	jpr	ne,L10005
	ld	r3,~L1+6(fp)
	jpr	L10006
L10005:
	ld	r3,~L1+4(fp)
L10006:
	ld	r13,r3
	callr	_fork
	ld	r12,r2
	test	r2
	jpr	ne,L24
	ld	r7,r14
	callr	_close
	ld	r2,~L1(fp)
	cpb	@r2,#114
	jpr	ne,L10007
	ldk	r6,#1
	jpr	L10008
L10007:
	sub	r6,r6
L10008:
	ld	r7,r13
	callr	_dup2
	ld	r7,r13
	callr	_close
	ld	r7,#L28
	ld	r6,#L29
	ld	r5,#L30
	ld	r4,~L1+2(fp)
	sub	r3,r3
	callr	_execl
	ldk	r7,#1
	callr	__exit
L24:
	cp	r12,#-1
	jpr	eq,L20000
	ld	r2,r14
	add	r2,r14
	ld	_popen_pid(r2),r12
	ld	r7,r13
	callr	_close
	ld	r7,r14
	ld	r6,~L1(fp)
	callr	_fdopen
L10000:
	ldm	r12,10(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _popen */

.data

/*~~pclose:*/

.code

_pclose::
{
	dec	fp,#~L2
	ldm	6(fp),r10,#5
	ld	@fp,r7
	ld	r6,r7
	ldb	rl2,7(r6)
	extsb	r2
	ld	r14,r2
	callr	_fclose
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	ld	r11,r2
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
	ld	r10,r2
	ldk	r7,#1
	ldk	r6,#1
	callr	_signal
	ld	r12,r2
L37:
	lda	r7,~L1+2(fp)
	callr	_wait
	ld	r13,r2
	ld	r3,r14
	add	r3,r14
	cp	r2,_popen_pid(r3)
	jpr	eq,L38
	cp	r2,#-1
	jpr	ne,L37
L38:
	cp	r13,#-1
	jpr	ne,L39
	ld	~L1+2(fp),#-1
L39:
	ldk	r7,#2
	ld	r6,r11
	callr	_signal
	ldk	r7,#3
	ld	r6,r10
	callr	_signal
	ldk	r7,#1
	ld	r6,r12
	callr	_signal
	ld	r2,~L1+2(fp)
	ldm	r10,6(fp),#5
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _pclose */

.data

