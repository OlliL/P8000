
fp		:= r15;
sp		:= r15;
L21:
.=.+8
.byte	0
.byte	0
L22:
.byte	%2f,%64,%65,%76,%2f,%74,%74,%79
.byte	%0
.even
L23:
.byte	%72,%0
.even
L33:
.byte	%a,%0
.even
/*~~getpass:*/

.code

_getpass::
{
	sub	fp,#~L2
	ldm	28(fp),r13,#2
	ld	@fp,r7
	ld	r7,#L22
	ld	r6,#L23
	callr	_fopen
	ld	~L1+22(fp),r2
	test	r2
	jpr	ne,L24
	sub	r2,r2
	jpr	L10000
L24:
	ld	r7,~L1+22(fp)
	sub	r6,r6
	callr	_setbuf
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	ld	~L1+24(fp),r2
	ld	r7,~L1+22(fp)
	ldb	rl6,7(r7)
	ldb	rl7,rl6
	extsb	r7
	lda	r5,~L1+2(fp)
	ld	r6,#21505
	callr	_ioctl
	ld	r2,~L1+8(fp)
	ld	~L1+20(fp),r2
	ld	r3,~L1+8(fp)
	and	r3,#65415
	ld	~L1+8(fp),r3
	ld	r7,~L1+22(fp)
	ldb	rl6,7(r7)
	ldb	rl7,rl6
	extsb	r7
	lda	r5,~L1+2(fp)
	ld	r6,#21508
	callr	_ioctl
	ld	r7,#__iob+16
	ld	r6,~L1(fp)
	callr	_fprintf
	ld	r14,#L21
	jpr	L31
L20001:
	cp	r13,#-1
	jpr	eq,L30
	cp	r14,#L21+8
	jpr	uge,L31
	ld	r2,r13
	ldb	@r14,rl2
	inc	r14,#1
L31:
	ld	r2,~L1+22(fp)
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10003
	ld	r7,~L1+22(fp)
	callr	__filbuf
	jpr	L10004
L10003:
	ld	r2,~L1+22(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10004:
	ld	r13,r2
	cp	r2,#10
	jpr	ne,L20001
L30:
	clrb	@r14
	ld	r7,#__iob+16
	ld	r6,#L33
	callr	_fprintf
	ld	r2,~L1+20(fp)
	ld	~L1+8(fp),r2
	ld	r7,~L1+22(fp)
	ldb	rl3,7(r7)
	ldb	rl7,rl3
	extsb	r7
	lda	r5,~L1+2(fp)
	ld	r6,#21507
	callr	_ioctl
	ldk	r7,#2
	ld	r6,~L1+24(fp)
	callr	_signal
	cp	~L1+22(fp),#__iob
	jpr	eq,L34
	ld	r7,~L1+22(fp)
	callr	_fclose
L34:
	ld	r2,#L21
L10000:
	ldm	r13,28(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 32
	}	/* _getpass */

.data

