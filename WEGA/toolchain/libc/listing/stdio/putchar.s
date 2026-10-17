
fp		:= r15;
sp		:= r15;
/*~~putchar:*/

.code

_putchar::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r14,r7
	dec	__iob+10,#1
	cp	__iob+10,#0
	jpr	ge,L10003
	ld	r6,#__iob+8
	callr	__flsbuf
	jpr	L10000
L10003:
	ld	r2,r14
	ld	r3,__iob+8
	inc	__iob+8,#1
	ldb	@r3,rl2
	subb	rh2,rh2
L10000:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _putchar */

.data

