
fp		:= r15;
sp		:= r15;
/*~~ltol3:*/

.code

_ltol3::
{
	dec	fp,#~L2
	ld	~L1+2(fp),r6
	ld	r3,r5
	ld	r1,r7
	ld	r4,r6
	jpr	L12
L20001:
	inc	r4,#1
	ldb	rl2,@r4
	ldb	@r1,rl2
	inc	r4,#1
	inc	r1,#1
	ldb	rl2,@r4
	ldb	@r1,rl2
	inc	r4,#1
	inc	r1,#1
	ldb	rl2,@r4
	ldb	@r1,rl2
	inc	r4,#1
	inc	r1,#1
L12:
	ld	r2,r3
	dec	r3,#1
	test	r2
	jpr	ne,L20001
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _ltol3 */

.data

/*~~l3tol:*/

.code

_l3tol::
{
	dec	fp,#~L2
	ld	~L1+4(fp),r7
	ld	r3,r5
	ld	r1,r6
	ld	r4,r7
	jpr	L16
L20003:
	clrb	@r4
	inc	r4,#1
	ldb	rl2,@r1
	ldb	@r4,rl2
	inc	r1,#1
	inc	r4,#1
	ldb	rl2,@r1
	ldb	@r4,rl2
	inc	r1,#1
	inc	r4,#1
	ldb	rl2,@r1
	ldb	@r4,rl2
	inc	r1,#1
	inc	r4,#1
L16:
	ld	r2,r3
	dec	r3,#1
	test	r2
	jpr	ne,L20003
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 12
	}	/* _l3tol */

.data

