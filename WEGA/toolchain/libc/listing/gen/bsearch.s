
fp		:= r15;
sp		:= r15;
/*~~bsearch:*/

.code

_bsearch::
{
	sub	fp,#~L2
	ldm	@fp,r3,#5
	dec	r5,#1
	ld	r1,r5
	mult	rr0,r4
	add	r6,r1
	ld	~L1+10(fp),r6
	ld	r2,~L1+6(fp)
	ld	~L1+12(fp),r2
	jpr	L12
L20001:
	ld	r3,~L1+4(fp)
	dec	r3,#1
	ld	r5,r3
	exts	rr4
	div	rr4,#2
	ld	r7,~L1+2(fp)
	mult	rr6,r5
	add	r7,~L1+12(fp)
	ld	~L1+14(fp),r7
	ld	r3,~L1(fp)
	ld	~L1+18(fp),r3
	ld	r7,~L1+8(fp)
	ld	r6,~L1+14(fp)
	ld	r1,r3
	call	@r1
	ld	~L1+16(fp),r2
	test	r2
	jpr	ne,L14
	ld	r2,~L1+14(fp)
	jpr	L10000
L14:
	cp	~L1+16(fp),#0
	jpr	ge,L15
	ld	r2,~L1+14(fp)
	sub	r2,~L1+2(fp)
	ld	~L1+10(fp),r2
	jpr	L16
L15:
	ld	r2,~L1+14(fp)
	add	r2,~L1+2(fp)
	ld	~L1+12(fp),r2
L16:
	ld	r2,~L1+10(fp)
	sub	r2,~L1+12(fp)
	ld	r5,r2
	exts	rr4
	div	rr4,~L1+2(fp)
	inc	r5,#1
	ld	~L1+4(fp),r5
L12:
	ld	r2,~L1+10(fp)
	cp	r2,~L1+12(fp)
	jpr	uge,L20001
	sub	r2,r2
L10000:
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 20
	}	/* _bsearch */

.data

