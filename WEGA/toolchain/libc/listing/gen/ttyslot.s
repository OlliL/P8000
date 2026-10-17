
fp		:= r15;
sp		:= r15;
_utmp:
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
/*~~ttyslot:*/

.code

_ttyslot::
{
	sub	fp,#~L2
	ldm	22(fp),r11,#4
	sub	r7,r7
	callr	_ttyname
	ld	r14,r2
	test	r2
	jpr	ne,L15
	ldk	r7,#1
	callr	_ttyname
	ld	r14,r2
	test	r2
	jpr	ne,L15
	ldk	r7,#2
	callr	_ttyname
	ld	r14,r2
	test	r2
	jpr	eq,L20002
L15:
	ld	r7,r14
	ld	r6,#47
	callr	_strrchr
	ld	r13,r2
	test	r2
	jpr	ne,L16
	ld	r13,r14
	jpr	L17
L16:
	inc	r13,#1
L17:
	ld	r7,#_utmp
	sub	r6,r6
	callr	_open
	ld	r11,r2
	cp	r2,#0
	jpr	lt,L20002
	ldk	r12,#0
	jpr	L21
L20001:
	lda	r6,~L1(fp)
	ld	r7,r13
	ldk	r5,#8
	callr	_strncmp
	test	r2
	jpr	ne,L24
	ld	r7,r11
	callr	_close
	ld	r2,r12
	jpr	L10000
L24:
	inc	r12,#1
L21:
	lda	r6,~L1(fp)
	ld	r7,r11
	ld	r5,#20
	callr	_read
	cp	r2,#20
	jpr	eq,L20001
	ld	r7,r11
	callr	_close
L20002:
	ld	r2,#-1
L10000:
	ldm	r11,22(fp),#4
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 30
	}	/* _ttyslot */

.data

