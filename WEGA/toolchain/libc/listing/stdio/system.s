
fp		:= r15;
sp		:= r15;
/*~~system:*/

.code

_system::
{
	dec	fp,#~L2
	ld	@fp,r7
	ldk	r6,#1
	callr	__system
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _system */

.data

/*~~csystem:*/

.code

_csystem::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r6,r7
	sub	r6,r7
	callr	__system
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _csystem */

.data

L21:
.byte	%2f,%62,%69,%6e,%2f,%63,%73,%68
.byte	%0
.even
L22:
.byte	%63,%73,%68,%0
.even
L23:
.byte	%2d,%63,%0
.even
L25:
.byte	%2f,%62,%69,%6e,%2f,%73,%68,%0
.even
L26:
.byte	%73,%68,%0
.even
L27:
.byte	%2d,%63,%0
.even
/*~~_system:*/

.code

__system::
{
	dec	fp,#~L2
	ldm	12(fp),r13,#2
	ldl	@fp,rr6
	callr	_fork
	ld	~L1+6(fp),r2
	test	r2
	jpr	ne,L18
	test	~L1(fp)
	jpr	ne,L19
	ld	r7,#L21
	ld	r6,#L22
	ld	r5,#L23
	jpr	L20002
L19:
	ld	r7,#L25
	ld	r6,#L26
	ld	r5,#L27
L20002:
	ld	r4,~L1+2(fp)
	sub	r3,r3
	callr	_execl
	ld	r7,#127
	callr	__exit
L18:
	ldk	r7,#2
	ldk	r6,#1
	callr	_signal
	ld	r14,r2
	ldk	r7,#3
	ldk	r6,#1
	callr	_signal
	ld	r13,r2
L30:
	lda	r7,~L1+4(fp)
	callr	_wait
	ld	~L1+8(fp),r2
	cp	r2,~L1+6(fp)
	jpr	eq,L31
	cp	r2,#-1
	jpr	ne,L30
L31:
	cp	~L1+8(fp),#-1
	jpr	ne,L32
	ld	~L1+4(fp),#-1
L32:
	ldk	r7,#2
	ld	r6,r14
	callr	_signal
	ldk	r7,#3
	ld	r6,r13
	callr	_signal
	ld	r2,~L1+4(fp)
	ldm	r13,12(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* __system */

.data

