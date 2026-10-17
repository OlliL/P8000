
fp		:= r15;
sp		:= r15;
.even
_sigs:
.=.+42
/*~~ssignal:*/

.code

_ssignal::
{
	dec	fp,#~L2
	ld	r3,r6
	ld	r1,r7
	cp	r7,#-4
	jpr	lt,L14
	cp	r7,#16
	jpr	gt,L14
	ld	r2,r7
	dec	r2,#4
	add	r2,r2
	ld	r4,_sigs(r2)
	ld	r2,r1
	dec	r2,#4
	add	r2,r2
	ld	_sigs(r2),r6
	jpr	L15
L14:
	ldk	r4,#0
L15:
	ld	r2,r4
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ssignal */

.data

/*~~gsignal:*/

.code

_gsignal::
{
	dec	fp,#~L2
	ldm	6(fp),r13,#2
	ld	r14,r7
	cp	r7,#-4
	jpr	lt,L10006
	cp	r7,#16
	jpr	gt,L10006
	ld	r2,r7
	dec	r2,#4
	add	r2,r2
	ld	r13,_sigs(r2)
	test	r13
	jpr	ne,L18
L10006:
	sub	r2,r2
	jpr	L10003
L18:
	cp	r13,#1
	jpr	ne,L19
	ldk	r2,#1
	jpr	L10003
L19:
	ld	r2,r14
	dec	r2,#4
	add	r2,r2
	clr	_sigs(r2)
	ld	~L1+4(fp),r13
	ld	r7,r14
	ld	r1,r13
	call	@r1
L10003:
	ldm	r13,6(fp),#2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _gsignal */

.data

