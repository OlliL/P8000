
fp		:= r15;
sp		:= r15;
/*~~scanf:*/

.code

_scanf::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	lda	r5,~L1(fp)
	ld	r6,~L1+2(fp)
	ld	r7,#__iob
	callr	__doscan
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _scanf */

.data

/*~~fscanf:*/

.code

_fscanf::
{
	dec	fp,#~L2
	ldm	@fp,r5,#3
	lda	r5,~L1(fp)
	ld	r4,r6
	ld	r2,r7
	callr	__doscan
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 8
	}	/* _fscanf */

.data

/*~~sscanf:*/

.code

_sscanf::
{
	sub	fp,#~L2
	ld	16(fp),r14
	ldm	@fp,r5,#2
	ld	r14,r7
	ldb	~L1+12(fp),#65
	ld	~L1+10(fp),r7
	ld	~L1+6(fp),r7
	clr	~L1+8(fp)
	jpr	L25
L20001:
	inc	~L1+8(fp),#1
L25:
	ld	r2,r14
	inc	r14,#1
	testb	@r2
	jpr	ne,L20001
	lda	r7,~L1+6(fp)
	lda	r5,~L1(fp)
	ld	r6,~L1+2(fp)
	callr	__doscan
	ld	r14,16(fp)
	add	fp,#~L2
	ret
	~L1 := 0
	~L2 := 18
	}	/* _sscanf */

.data

