
fp		:= r15;
sp		:= r15;
/*~~fgetc:*/

.code

_fgetc::
{
	dec	fp,#~L2
	ld	@fp,r7
	ld	r6,r7
	dec	2(r6),#1
	ld	r2,2(r6)
	cp	r2,#0
	jpr	ge,L10003
	ld	r6,r7
	callr	__filbuf
	jpr	L10000
L10003:
	ld	r2,~L1(fp)
	inc	@r2,#1
	ld	r3,@r2
	dec	r3,#1
	ldb	rl2,@r3
	subb	rh2,rh2
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 4
	}	/* _fgetc */

.data

