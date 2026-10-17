
fp		:= r15;
sp		:= r15;
_UTMP:
.byte	%2f
.byte	%65
.byte	%74
.byte	%63
.byte	%2f
.byte	%75
.byte	%74
.byte	%6d
.byte	%70
.byte	%0
.even
_ubuf:
.=.+20
/*~~getlogin:*/

.code

_getlogin::
{
	dec	fp,#~L2
	ldm	2(fp),r12,#3
	callr	_ttyslot
	ld	r14,r2
	cp	r2,#0
	jpr	lt,L20000
	jpr	L15
L20002:
	ld	r7,r13
	callr	_close
L20000:
	sub	r2,r2
	jpr	L10000
L15:
	ld	r7,#_UTMP
	sub	r6,r6
	callr	_open
	ld	r13,r2
	cp	r2,#0
	jpr	lt,L20000
	ld	r5,r14
	mult	rr4,#20
	ld	r3,r5
	exts	rr2
	ldl	rr4,rr2
	ld	r7,r13
	sub	r3,r3
	callr	_lseek
	ld	r7,r13
	ld	r6,#_ubuf
	ld	r5,#20
	callr	_read
	cp	r2,#20
	jpr	ne,L20002
	ld	r7,r13
	callr	_close
	testb	_ubuf+8
	jpr	eq,L20000
	ldb	_ubuf+16,#32
	ld	r12,#_ubuf+8
L25:
	ld	r2,r12
	inc	r12,#1
	ldb	rl3,@r2
	cpb	rl3,#32
	jpr	ne,L25
	dec	r12,#1
	clrb	@r12
	ld	r2,#_ubuf+8
L10000:
	ldm	r12,2(fp),#3
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _getlogin */

.data

