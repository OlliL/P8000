
fp		:= r15;
sp		:= r15;
/*~~lsearch:*/

.code

_lsearch::
{
	dec	fp,#~L2
	ldm	@fp,r3,#5
	ld	~L1+10(fp),r6
	ld	r2,r5
	ld	r1,@r2
	mult	rr0,r4
	add	r6,r1
	ld	~L1+12(fp),r6
	ld	r7,r6
	ld	r6,~L1+8(fp)
	ld	r5,~L1+2(fp)
	callr	_strncpy
	jpr	L13
L20001:
	ld	r2,~L1+10(fp)
	add	r2,~L1+2(fp)
	ld	~L1+10(fp),r2
L13:
	ld	r2,~L1(fp)
	ld	~L1+14(fp),r2
	ld	r7,~L1+8(fp)
	ld	r6,~L1+10(fp)
	ld	r1,r2
	call	@r1
	test	r2
	jpr	ne,L20001
	ld	r2,~L1+10(fp)
	cp	r2,~L1+12(fp)
	jpr	ne,L15
	ld	r3,~L1+4(fp)
	inc	@r3,#1
L15:
	ld	r2,~L1+10(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 16
	}	/* _lsearch */

.data

