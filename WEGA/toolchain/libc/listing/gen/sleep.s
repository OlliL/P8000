
fp		:= r15;
sp		:= r15;
/*~~sleep:*/

.code

_sleep::
{
	dec	fp,#~L2
	ld	@fp,r7
	test	r7
	jpr	ne,L15
	sub	r2,r2
	jpr	L10000
L15:
	sub	r7,r7
	callr	_alarm
	ld	~L1+8(fp),r2
	ldk	r7,#14
	ld	r6,#_awake
	callr	_signal
	ld	~L1+4(fp),r2
	clr	~L1+2(fp)
	clr	~L1+10(fp)
	test	~L1+8(fp)
	jpr	eq,L16
	ld	r2,~L1+8(fp)
	cp	r2,~L1(fp)
	jpr	ule,L17
	ld	r3,r2
	sub	r3,~L1(fp)
	ld	~L1+8(fp),r3
	inc	~L1+2(fp),#1
	jpr	L16
L17:
	ld	r2,~L1(fp)
	sub	r2,~L1+8(fp)
	ld	~L1+10(fp),r2
	ld	r2,~L1+8(fp)
	ld	~L1(fp),r2
	clr	~L1+8(fp)
	dec	~L1+2(fp),#1
	ldk	r7,#14
	ld	r6,~L1+4(fp)
	callr	_signal
L16:
	ld	r7,~L1(fp)
	callr	_alarm
	callr	_pause
	sub	r7,r7
	callr	_alarm
	ld	~L1+6(fp),r2
	cp	~L1+2(fp),#0
	jpr	lt,L20
	ldk	r7,#14
	ld	r6,~L1+4(fp)
	callr	_signal
L20:
	cp	~L1+2(fp),#0
	jpr	gt,L10003
	cp	~L1+2(fp),#0
	jpr	ge,L21
	test	~L1+6(fp)
	jpr	eq,L21
L10003:
	ld	r7,~L1+8(fp)
	add	r7,~L1+6(fp)
	callr	_alarm
L21:
	ld	r2,~L1+10(fp)
	add	r2,~L1+6(fp)
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 14
	}	/* _sleep */

.data


.code

_awake:
{
	ret
	~L1 := 0
	~L2 := 0
	}	/* _awake */

.data

