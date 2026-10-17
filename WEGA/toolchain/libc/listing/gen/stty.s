
fp		:= r15;
sp		:= r15;
/*~~stty:*/

.code

_stty::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,r7
	ld	r6,#29705
	ld	r5,~L1(fp)
	callr	_ioctl
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _stty */

.data

/*~~gtty:*/

.code

_gtty::
{
	dec	fp,#~L2
	ldl	@fp,rr6
	ld	r2,r7
	ld	r6,#29704
	ld	r5,~L1(fp)
	callr	_ioctl
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 6
	}	/* _gtty */

.data

