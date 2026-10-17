
fp		:= r15;
sp		:= r15;
.even
_randx:
.long	%1
/*~~srand:*/

.code

_srand::
{
	dec	fp,#~L2
	ld	@fp,r7
	ldk	r2,#0
	ld	r3,r7
	ldl	_randx,rr2
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _srand */

.data

/*~~rand:*/

.code

_rand::
{
	dec	fp,#~L2
	ldl	rr6,_randx
	multl	rq4,#1103515245
	addl	rr6,#12345
	ldl	_randx,rr6
	sral	rr6,#16
	ldk	r6,#0
	and	r7,#32767
	ld	r2,r7
	inc	fp,#~L2
	ret
	~L1 := 0
	~L2 := 2
	}	/* _rand */

.data

