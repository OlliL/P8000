
fp		:= r15;
sp		:= r15;
/*~~fputc:*/

.code

_fputc::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,r6
	dec	2(r2),#1
	ld	r3,2(r2)
	cp	r3,#0
	jpr	ge,L10003
	ld	r7,~L1+2(fp)
	ld	r2,r6
	callr	__flsbuf
	jpr	L10000
L10003:
	ld	r2,~L1(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,~L1+3(fp)
	ldb	@r3,rl2
	subb	rh2,rh2
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _fputc */

.data

