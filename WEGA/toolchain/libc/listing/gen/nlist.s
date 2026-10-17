
fp		:= r15;
sp		:= r15;
.even
_a_magic::
.word	%e607
.word	%e611
.word	%e707
.word	%e711
.word	%0
/*~~nlist:*/

.code

_nlist::
{
	sub	fp,#~L2
	ldm	1594(fp),r13,#2
	ldl	@fp,rr6
	clr	~L1+1582(fp)
	ld	r13,r6
	jpr	L15
L20001:
	clrb	6(r13)
	subl	rr2,rr2
	ldl	2(r13),rr2
	inc	~L1+1582(fp),#1
	inc	r13,#8
L15:
	ld	r2,@r13
	testb	@r2
	jpr	ne,L20001
	ld	r7,~L1+2(fp)
	sub	r6,r6
	callr	_open
	ld	~L1+4(fp),r2
	cp	r2,#0
	jpr	ge,L17
L20006:
	ld	r2,#-1
	jpr	L10000
L17:
	lda	r6,~L1+18(fp)
	ld	r7,~L1+4(fp)
	ld	r5,#24
	callr	_read
	clr	~L1+6(fp)
	jpr	L21
L20003:
	ld	r2,~L1+6(fp)
	add	r2,r2
	ld	r3,_a_magic(r2)
	cp	r3,~L1+18(fp)
	jpr	eq,L20
	inc	~L1+6(fp),#1
L21:
	ld	r2,~L1+6(fp)
	add	r2,r2
	test	_a_magic(r2)
	jpr	ne,L20003
L20:
	ld	r2,~L1+6(fp)
	add	r2,r2
	test	_a_magic(r2)
	jpr	ne,L23
	ld	r7,~L1+4(fp)
	callr	_close
	jpr	L20006
L23:
	ldl	rr2,~L1+20(fp)
	ldl	~L1+14(fp),rr2
	ld	r4,~L1+36(fp)
	and	r4,#1
	cp	r4,#1
	jpr	eq,L25
	ldl	rr6,rr2
	multl	rq4,#2
	ldl	~L1+14(fp),rr6
L25:
	ld	r2,~L1+28(fp)
	add	r2,#24
	ldk	r4,#0
	ld	r5,r2
	addl	rr4,~L1+14(fp)
	ldl	~L1+14(fp),rr4
	ld	r7,~L1+4(fp)
	sub	r3,r3
	callr	_lseek
	ld	r2,~L1+30(fp)
	ld	~L1+10(fp),r2
L27:
	test	~L1+10(fp)
	jpr	eq,L28
	ld	~L1+12(fp),#1400
	cp	~L1+10(fp),#1400
	jpr	uge,L29
	ld	r2,~L1+10(fp)
	ld	~L1+12(fp),r2
L29:
	lda	r6,~L1+42(fp)
	ld	r7,~L1+4(fp)
	ld	r5,~L1+12(fp)
	callr	_read
	ld	r2,~L1+10(fp)
	sub	r2,~L1+12(fp)
	ld	~L1+10(fp),r2
	lda	r2,~L1+42(fp)
	ld	r14,r2
L30:
	test	~L1+12(fp)
	jpr	eq,L27
	ld	r13,~L1(fp)
L34:
	ld	r2,@r13
	testb	@r2
	jpr	eq,L33
	ld	r2,@r13
	ld	~L1+1586(fp),r2
	lda	r3,6(r14)
	ld	~L1+1588(fp),r3
	ld	r4,r3
	ldb	rl5,@r4
	extsb	r5
	bit	r5,#7
	jpr	eq,L35
	ldb	rl5,@r4
	extsb	r5
	and	r5,#127
	add	r5,~L1+1588(fp)
	ld	~L1+1584(fp),r5
	inc	~L1+1588(fp),#1
	jpr	L36
L35:
	ld	r2,~L1+1588(fp)
	inc	r2,#8
	ld	~L1+1584(fp),r2
L36:
	clr	~L1+1590(fp)
L37:
	ld	r2,~L1+1586(fp)
	testb	@r2
	jpr	eq,L38
	ld	r3,~L1+1588(fp)
	cp	r3,~L1+1584(fp)
	jpr	uge,L38
	inc	~L1+1586(fp),#1
	ldb	rl3,@r2
	ld	r2,~L1+1588(fp)
	inc	~L1+1588(fp),#1
	ldb	rl4,@r2
	cpb	rl3,rl4
	jpr	eq,L37
	inc	~L1+1590(fp),#1
L38:
	ld	r2,~L1+1588(fp)
	cp	r2,~L1+1584(fp)
	jpr	uge,L40
	testb	@r2
	jpr	eq,L40
	inc	~L1+1590(fp),#1
L40:
	test	~L1+1590(fp)
	jpr	ne,L32
	ldl	rr2,@r14
	ldl	2(r13),rr2
	ldb	rl2,4(r14)
	ldb	6(r13),rl2
	testb	rl2
	jpr	eq,L33
	dec	~L1+1582(fp),#1
L33:
	test	~L1+1582(fp)
	jpr	ne,L43
	jpr	L28
L32:
	inc	r13,#8
	jpr	L34
L43:
	ld	r7,r14
	callr	_readby
	ld	~L1+6(fp),r2
	ld	r5,r2
	mult	rr4,#14
	ld	r4,~L1+12(fp)
	sub	r4,r5
	ld	~L1+12(fp),r4
	test	r4
	jpr	eq,L30
	ld	r3,~L1+6(fp)
	mult	rr2,#14
	add	r14,r3
	ld	r7,r14
	callr	_readby
	ld	~L1+6(fp),r2
	ld	r5,~L1+12(fp)
	ldk	r4,#0
	div	rr4,#14
	ld	~L1+8(fp),r5
	cp	r2,r5
	jpr	le,L30
	ld	r5,r2
	sub	r5,~L1+8(fp)
	ld	r3,r5
	mult	rr2,#14
	ld	~L1+6(fp),r3
	ld	r5,r3
	lda	r6,~L1+1442(fp)
	ld	r7,~L1+4(fp)
	callr	_read
	ld	r2,~L1+10(fp)
	sub	r2,~L1+6(fp)
	ld	~L1+10(fp),r2
	ld	r2,~L1+12(fp)
	add	r2,~L1+6(fp)
	ld	~L1+12(fp),r2
	jpr	L30
L28:
	ld	r7,~L1+4(fp)
	callr	_close
	sub	r2,r2
L10000:
	ldm	r13,1594(fp),#2
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 1598
	}	/* _nlist */

.data

/*~~readby:*/

.code

_readby::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r6,r7
	ldb	rl2,6(r6)
	extsb	r2
	bit	r2,#7
	jpr	eq,L48
	ldb	rl2,6(r7)
	extsb	r2
	and	r2,#127
	dec	r2,#7
	ld	~L1+2(fp),r2
	dec	r2,#7
	ld	r5,r2
	exts	rr4
	div	rr4,#14
	inc	r5,#1
	ld	~L1+2(fp),r5
	inc	~L1+2(fp),#1
	ld	r2,~L1+2(fp)
	jpr	L10003
L48:
	ldk	r2,#1
L10003:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _readby */

.data

