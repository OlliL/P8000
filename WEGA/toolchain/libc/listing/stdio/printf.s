
fp		:= r15;
sp		:= r15;
/*~~printf:*/

.code

_printf::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	lda	r6,~L1(fp)
	ld	r2,r7
	ld	r5,#__iob+8
	callr	__doprnt
	ldb	rl2,__iob+14
	extsb	r2
	and	r2,#32
	test	r2
	jpr	eq,L10003
	ld	r2,#-1
	jpr	L10000
L10003:
	sub	r2,r2
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _printf */

.data

