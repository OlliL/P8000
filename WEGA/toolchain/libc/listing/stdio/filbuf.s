
fp		:= r15;
sp		:= r15;
L21:
.=.+20
/*~~_filbuf:*/

.code

__filbuf::
{
	dec	fp,#~L2
	ld	4(fp),r14
	ld	r14,r7
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	eq,L22
	setb	6(r14),#0
L22:
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#1
	test	r2
	jpr	eq,L10003
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#6
	jpr	eq,L24
L10003:
	ld	r2,#-1
	jpr	L10000
L26:
	ld	r7,#512
	callr	_malloc
	ld	4(r14),r2
	test	r2
	jpr	ne,L27
	setb	6(r14),#2
L24:
	test	4(r14)
	jpr	ne,L25
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#2
	jpr	eq,L26
	ldb	rl2,7(r14)
	extsb	r2
	add	r2,#L21
	ld	4(r14),r2
	jpr	L24
L27:
	setb	6(r14),#3
L25:
	ld	r2,4(r14)
	ld	@r14,r2
	ldb	rl5,6(r14)
	extsb	r5
	bit	r5,#2
	jpr	eq,L10004
	ldk	r5,#1
	jpr	L10005
L10004:
	ld	r5,#512
L10005:
	ld	r6,@r14
	ldb	rl7,7(r14)
	extsb	r7
	callr	_read
	ld	2(r14),r2
	dec	2(r14),#1
	cp	2(r14),#0
	jpr	ge,L29
	cp	2(r14),#-1
	jpr	ne,L30
	setb	6(r14),#4
	ldb	rl2,6(r14)
	extsb	r2
	bit	r2,#7
	jpr	eq,L32
	resb	6(r14),#0
	jpr	L32
L30:
	setb	6(r14),#5
L32:
	clr	2(r14)
	jpr	L10003
L29:
	ld	r2,@r14
	ldb	rl3,@r2
	subb	rh3,rh3
	and	r3,#255
	ld	~L1+2(fp),r3
	inc	@r14,#1
	ld	r2,~L1+2(fp)
L10000:
	ld	r14,4(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* __filbuf */

.data

