
fp		:= r15;
sp		:= r15;
L21:
.byte	%25,%73,%3a,%25,%73,%0
.even
L23:
.byte	%2c,%25,%73,%0
.even
L24:
.byte	%3a,%25,%75,%3a,%25,%75,%3a,%25
.byte	%73,%3a,%25,%73,%3a,%25,%73,%0
.even
/*~~putpwent:*/

.code

_putpwent::
{
	dec	fp,#~L2
	ldm	8(fp),r13,#2
	ld	r13,r6
	ld	r14,r7
	ld	r7,r6
	ld	r6,#L21
	ld	r5,@r14
	ld	r4,2(r14)
	callr	_fprintf
	ld	r2,8(r14)
	testb	@r2
	jpr	eq,L22
	ld	r7,r13
	ld	r6,#L23
	ld	r5,8(r14)
	callr	_fprintf
L22:
	ld	r2,16(r14)
	ld	@fp,r2
	ld	r7,r13
	ld	r6,#L24
	ld	r5,4(r14)
	ld	r4,6(r14)
	ld	r3,12(r14)
	ld	r2,14(r14)
	callr	_fprintf
	dec	2(r13),#1
	cp	2(r13),#0
	jpr	ge,L10003
	ldk	r7,#10
	ld	r6,r13
	callr	__flsbuf
	jpr	L10004
L10003:
	ld	r2,@r13
	inc	@r13,#1
	ldb	@r2,#10
	ldb	rl2,#10
	subb	rh2,rh2
L10004:
	ldb	rl2,6(r13)
	extsb	r2
	and	r2,#32
	cp	r2,#0
	ldk	r2,#0
	tcc	ne,r2
	ldm	r13,8(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 2
	~L2 := 12
	}	/* _putpwent */

.data

