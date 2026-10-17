
fp		:= r15;
sp		:= r15;
/*~~_flsbuf:*/

.code

__flsbuf::
{
	dec	fp,#~L2
	ldm	8(fp),r11,#4
	ld	~L1+2(fp),r7
	ld	r14,r6
	ldb	rl6,6(r14)
	extsb	r6
	bit	r6,#7
	jpr	eq,L23
	setb	6(r14),#1
	resb	6(r14),#4
	jpr	L23
L20001:
	ldb	rl7,__iob+15
	extsb	r7
	callr	_isatty
	test	r2
	jpr	eq,L30
L20002:
	setb	6(r14),#2
	jpr	L23
L30:
	ld	4(r14),#__sobuf
	ld	@r14,#__sobuf
L23:
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#2
	jpr	eq,L24
	ldb	rl2,~L1+3(fp)
	ldb	~L1+5(fp),rl2
	ldk	r11,#1
	lda	r6,~L1+5(fp)
	ldb	rl7,7(r14)
	extsb	r7
	ld	r5,r11
	callr	_write
	ld	r12,r2
	clr	2(r14)
	jpr	L26
L24:
	ld	r13,4(r14)
	test	r13
	jpr	ne,L27
	cp	r14,#__iob+8
	jpr	eq,L20001
	ld	r7,#512
	callr	_malloc
	ld	r13,r2
	ld	4(r14),r2
	test	r2
	jpr	eq,L20002
	setb	6(r14),#3
	ldk	r12,#0
	ld	r11,r12
	jpr	L32
L27:
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	ld	r11,r2
	cp	r2,#0
	jpr	le,L32
	ld	@r14,r13
	ldb	rl7,7(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	callr	_write
	ld	r12,r2
L32:
	ld	2(r14),#511
	ldb	rl2,~L1+3(fp)
	ldb	@r13,rl2
	inc	r13,#1
	ld	@r14,r13
L26:
	cp	r11,r12
	jpr	eq,L34
	setb	6(r14),#5
	ld	r2,#-1
	jpr	L10000
L34:
	ld	r2,~L1+2(fp)
L10000:
	ldm	r11,8(fp),#4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* __flsbuf */

.data

/*~~fflush:*/

.code

_fflush::
{
	dec	fp,#~L2
	ldm	4(fp),r12,#3
	ld	r14,r7
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#6
	cp	r2,#2
	jpr	ne,L37
	ld	r13,4(r14)
	test	r13
	jpr	eq,L37
	ld	r2,@r14
	sub	r2,r13
	ld	r12,r2
	cp	r2,#0
	jpr	le,L37
	ld	@r14,r13
	ld	2(r14),#512
	ldb	rl7,7(r14)
	extsb	r7
	ld	r6,r13
	ld	r5,r12
	callr	_write
	cp	r12,r2
	jpr	eq,L37
	setb	6(r14),#5
	ld	r2,#-1
	jpr	L10003
L37:
	sub	r2,r2
L10003:
	ldm	r12,4(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _fflush */

.data

/*~~_cleanup:*/

.code

__cleanup::
{
	dec	fp,#~L2
	ld	@fp,r14
	ld	r14,#__iob
	jpr	L44
L20004:
	ld	r7,r14
	callr	_fclose
	inc	r14,#8
L44:
	cp	r14,__lastbuf
	jpr	ult,L20004
	ld	r14,@fp
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* __cleanup */

.data

/*~~fclose:*/

.code

_fclose::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	r14,r7
	ld	r13,#-1
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#131
	jpr	eq,L47
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#64
	test	r2
	jpr	ne,L47
	callr	_fflush
	ld	r13,r2
	ldb	rl7,7(r14)
	extsb	r7
	callr	_close
	cp	r2,#0
	jpr	ge,L49
	ld	r2,#-1
	jpr	L10009
L49:
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#3
	jpr	eq,L50
	ld	r7,4(r14)
	callr	_free
L50:
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#12
	jpr	eq,L47
	clr	4(r14)
L47:
	ldb	rl2,6(r14)
	andb	rl2,#0
	ldb	6(r14),rl2
	clr	2(r14)
	ld	r2,r13
L10009:
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fclose */

.data

