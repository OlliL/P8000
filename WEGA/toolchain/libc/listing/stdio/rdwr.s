
fp		:= r15;
sp		:= r15;
/*~~fread:*/

.code

_fread::
{
	sub	fp,#~L2
	ldm	14(fp),r12,#3
	ldm	~L1+2(fp),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	~L1+8(fp)
	test	r6
	jpr	ne,L23
	jpr	L20
L20001:
	ld	r2,r12
	ldb	@r14,rl2
	inc	r14,#1
	dec	~L1+10(fp),#1
	jpr	ne,L26
	inc	~L1+8(fp),#1
L23:
	ld	r2,~L1+8(fp)
	cp	r2,~L1+2(fp)
	jpr	uge,L20
	ld	r3,~L1+4(fp)
	ld	~L1+10(fp),r3
L26:
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10003
	ld	r7,r13
	callr	__filbuf
	jpr	L10004
L10003:
	ld	r2,@r13
	inc	@r13,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10004:
	ld	r12,r2
	cp	r2,#0
	jpr	ge,L20001
L20:
	ld	r2,~L1+8(fp)
	ldm	r12,14(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 20
	}	/* _fread */

.data

/*~~fwrite:*/

.code

_fwrite::
{
	sub	fp,#~L2
	ldm	12(fp),r12,#3
	ldm	~L1+2(fp),r5,#2
	ld	r13,r4
	ld	r14,r7
	clr	~L1+8(fp)
	test	r6
	jpr	ne,L35
	jpr	L32
L20003:
	ld	r12,~L1+4(fp)
L38:
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10008
	ld	r7,r14
	inc	r14,#1
	ldb	rl6,@r7
	extsb	r6
	ld	r7,r6
	ld	r6,r13
	callr	__flsbuf
	jpr	L37
L10008:
	ld	r2,r14
	inc	r14,#1
	ld	r3,@r13
	inc	@r13,#1
	ldb	rl4,@r2
	ldb	@r3,rl4
	subb	rh4,rh4
	ld	r2,r4
L37:
	djnz	r12,L38
	ldb	rl2,6(r13)
	extsb	r2
	and	r2,#32
	test	r2
	jpr	ne,L32
	inc	~L1+8(fp),#1
L35:
	ld	r2,~L1+8(fp)
	cp	r2,~L1+2(fp)
	jpr	ult,L20003
L32:
	ld	r2,~L1+8(fp)
	ldm	r12,12(fp),#3
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _fwrite */

.data

