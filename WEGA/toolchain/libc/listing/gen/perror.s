
fp		:= r15;
sp		:= r15;
.comm	2,	_errno;
.comm	2,	_sys_nerr;
.comm	0,	_sys_errlist;
.comm	2,	_deverr;
.comm	2,	_dev_nerr;
.comm	0,	_dev_errlist;
L18:
.byte	%55,%6e,%6b,%6e,%6f,%77,%6e,%20
.byte	%65,%72,%72,%6f,%72,%0
.even
L30:
.byte	%3a,%20,%0
.even
L31:
.byte	%a,%0
.even
/*~~perror:*/

.code

_perror::
{
	dec	fp,#~L2
	ldm	4(fp),r13,#2
	ld	@fp,r7
	ld	r14,#L18
	cp	_errno,#5
	jpr	eq,L20
	cp	_errno,#6
	jpr	ne,L22
L20:
	test	_deverr
	jpr	eq,L22
	ld	r2,_deverr
	cp	r2,_dev_nerr
	jpr	ge,L25
	add	r2,r2
	ld	r14,_dev_errlist(r2)
	jpr	L25
L22:
	ld	r2,_errno
	cp	r2,_sys_nerr
	jpr	ge,L25
	add	r2,r2
	ld	r14,_sys_errlist(r2)
L25:
	ld	r7,~L1(fp)
	callr	_strlen
	ld	r13,r2
	test	r2
	jpr	eq,L28
	ldk	r7,#2
	ld	r6,~L1(fp)
	ld	r5,r2
	callr	_write
	ldk	r7,#2
	ld	r6,#L30
	ld	r5,r7
	callr	_write
L28:
	ld	r7,r14
	callr	_strlen
	ld	~L1+2(fp),r2
	ldk	r7,#2
	ld	r6,r14
	ld	r5,r2
	callr	_write
	ldk	r7,#2
	ld	r6,#L31
	ldk	r5,#1
	callr	_write
	ldm	r13,4(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _perror */

.data

