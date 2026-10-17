
fp		:= r15;
sp		:= r15;
/*~~_findiop:*/

.code

__findiop::
{
	dec	fp,#~L2
	ld	2(fp),r14
	ld	r14,#__iob
	jpr	L23
L20001:
	cp	r14,__lastbuf
	jpr	ult,L21
	sub	r2,r2
	jpr	L10000
L21:
	inc	r14,#8
L23:
	ldb	rl2,6(r14)
	extsb	r2
	and	r2,#131
	jpr	ne,L20001
	ld	r2,r14
L10000:
	ld	r14,2(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* __findiop */

.data

