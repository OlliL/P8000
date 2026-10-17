
fp		:= r15;
sp		:= r15;
_dev:
.byte	%2f
.byte	%64
.byte	%65
.byte	%76
.byte	%2f
.byte	%0
L15:
.=.+32
/*~~ttyname:*/

.code

_ttyname::
{
	sub	fp,#~L2
	ld	80(fp),r14
	ld	@fp,r7
	ld	r6,r7
	callr	_isatty
	test	r2
	jpr	eq,L20000
	lda	r6,~L1+2(fp)
	ld	r7,~L1(fp)
	callr	_fstat
	cp	r2,#0
	jpr	lt,L20000
	ld	r2,~L1+6(fp)
	and	r2,#61440
	cp	r2,#8192
	jpr	ne,L20000
	ld	r7,#_dev
	sub	r6,r6
	callr	_open
	ld	r14,r2
	cp	r2,#0
	jpr	lt,L20000
L24:
	lda	r6,~L1+62(fp)
	ld	r7,r14
	ld	r5,#16
	callr	_read
	cp	r2,#16
	jpr	ne,L25
	test	~L1+62(fp)
	jpr	eq,L24
	ld	r2,~L1+62(fp)
	cp	r2,~L1+4(fp)
	jpr	ne,L24
	ld	r7,#L15
	ld	r6,#_dev
	callr	_strcpy
	lda	r6,~L1+64(fp)
	ld	r7,#L15
	callr	_strcat
	lda	r6,~L1+32(fp)
	ld	r7,#L15
	callr	_stat
	cp	r2,#0
	jpr	lt,L24
	ld	r2,~L1+32(fp)
	cp	r2,~L1+2(fp)
	jpr	ne,L24
	ld	r3,~L1+34(fp)
	cp	r3,~L1+4(fp)
	jpr	ne,L24
	ld	r7,r14
	callr	_close
	ld	r2,#L15
	jpr	L10000
L25:
	ld	r7,r14
	callr	_close
L20000:
	sub	r2,r2
L10000:
	ld	r14,80(fp)
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 82
	}	/* _ttyname */

.data

