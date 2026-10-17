
fp		:= r15;
sp		:= r15;
.even
L12:
.word	0
.even
L13:
.word	0
L17:
.byte	%6d,%6f,%6e,%2e,%6f,%75,%74,%0
.even
/*~~monitor:*/

.code

_monitor::
{
	dec	fp,#~L2
	ld	10(fp),r14
	ldm	@fp,r3,#5
	test	r7
	jpr	ne,L14
	sub	r7,r7
	sub	r6,r6
	sub	r5,r5
	sub	r4,r4
	callr	_profil
	ld	r7,#L17
	ld	r6,#438
	callr	_creat
	ld	r14,r2
	ld	r5,L13
	add	r5,r5
	ld	r6,L12
	ld	r7,r2
	callr	_write
	ld	r7,r14
	callr	_close
	jpr	L10000
L14:
	ld	r2,~L1+2(fp)
	ld	L13,r2
	ld	r3,~L1+4(fp)
	ld	r4,~L1+8(fp)
	ld	@r3,r4
	ld	r3,~L1+4(fp)
	ld	r4,~L1+6(fp)
	ld	2(r3),r4
	ld	r3,~L1+4(fp)
	ld	r4,~L1(fp)
	ld	4(r3),r4
	ld	r4,~L1+4(fp)
	ld	L12,r4
	ld	r5,~L1(fp)
	inc	r5,#1
	ld	r7,r5
	mult	rr6,#3
	add	r7,r7
	add	r7,~L1+4(fp)
	ld	~L1+4(fp),r7
	ld	r5,~L1(fp)
	inc	r5,#1
	ld	r7,r5
	mult	rr6,#3
	ld	r5,~L1+2(fp)
	sub	r5,r7
	ld	~L1+2(fp),r5
	cp	r5,#0
	jpr	le,L10000
	ld	r2,~L1+6(fp)
	sub	r2,~L1+8(fp)
	sra	r2,#1
	and	r2,#32767
	ld	r14,r2
	cp	r14,~L1+2(fp)
	jpr	le,L21
	ld	r7,r5
	exts	rr6
	slal	rr6,#15
	ld	r3,r2
	exts	rr2
	extsl	rq4
	divl	rq4,rr2
	ld	r14,r7
	jpr	L22
L21:
	ld	r14,#32767
L22:
	ld	r6,~L1+2(fp)
	add	r6,r6
	ld	r4,r14
	add	r4,r14
	ld	r5,~L1+8(fp)
	ld	r7,~L1+4(fp)
	callr	_profil
L10000:
	ld	r14,10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _monitor */

.data

