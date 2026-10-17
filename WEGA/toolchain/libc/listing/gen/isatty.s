
fp		:= r15;
sp		:= r15;
/*~~isatty:*/

.code

_isatty::
{
	dec	fp,#~L2
	ld	@fp,r7
	lda	r6,~L1+2(fp)
	ld	r2,r7
	callr	_gtty
	cp	r2,#0
	jpr	ge,L13
	sub	r2,r2
	jpr	L10000
L13:
	ldk	r2,#1
L10000:
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 10
	}	/* _isatty */

.data

