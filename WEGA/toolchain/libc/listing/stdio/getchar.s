
fp		:= r15;
sp		:= r15;
/*~~getchar:*/

.code

_getchar::
{
	dec	fp,#~L2
	dec	__iob+2,#1
	cp	__iob+2,#0
	jpr	ge,L10003
	ld	r7,#__iob
	callr	__filbuf
	jpr	L10000
L10003:
	ld	r2,__iob
	inc	__iob,#1
	ldb	rl3,@r2
	subb	rh3,rh3
	ld	r2,r3
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _getchar */

.data

