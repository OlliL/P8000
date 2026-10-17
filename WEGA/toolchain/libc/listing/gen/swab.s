
fp		:= r15;
sp		:= r15;
/*~~swab:*/

.code

_swab::
{
	dec	fp,#~L2
	ld	6(fp),r12
	ld	r4,r6
	ld	r1,r7
	ld	r3,r5
	exts	rr2
	div	rr2,#2
	ld	r12,r3
	jpr	L12
L20001:
	ld	r2,@r1
	ldb	rh2,rl2
	clrb	rl2
	ld	r3,@r1
	ldb	rl3,rh3
	extsb	r3
	and	r3,#255
	add	r2,r3
	ld	@r4,r2
	inc	r4,#2
	inc	r1,#2
L12:
	dec	r12,#1
	cp	r12,#0
	jpr	ge,L20001
	ld	r12,6(fp)
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _swab */

.data

